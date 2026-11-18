//###########################################
//                 DRV8305
//###########################################

#include "drv8305_status.h"

const unsigned long drvSpiClockHz = 1000000;        // Clock of the status register reads [Hz]
const uint8_t drvReadBit = 0x80;                    // First bit of a command, 1 = read
const int drvAddressShift = 3;                      // The 4 address bits follow the read bit in the first byte
const uint8_t drvFirstStatusAddress = 1;            // The status registers are 0x1 to 0x4
const int drvPinSamples = 16;                       // nFAULT samples per check
const unsigned int drvPinSampleGapUs = 10;          // Delay between samples [us], 16 samples cover more than one 112 us warning period

//Configure DRV8305 to desired operation mode
int drv_init(bool resp){
  Serial.println("DAGOR: DRV8305 INIT");

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10010110);
  digitalWrite(cs, HIGH);
  //Serial.println(resp1, BIN);
  //Serial.println(resp2, BIN);

  if(resp){
    if (resp1 != 0b000010 || resp2 != 0b10010110){
      Serial.println(resp1, BIN);
      Serial.println(resp2, BIN);
      Serial.println("Dagor: DRV cannot init.");
      return DRV_ERROR;
    }
  }
  

  //Clamp sense amplifier output to 3.3V - protect ESP32 adc
  digitalWrite(cs, LOW);
  byte resp3 = SPI.transfer(B01001100);
  byte resp4 = SPI.transfer(B10100000);
  digitalWrite(cs, HIGH);
  //Serial.println(resp3, BIN);
  //Serial.println(resp4, BIN);

  if(resp){
    if (resp3 != 0b00000100 || resp4 != 0b10100000){
      Serial.println(resp3, BIN);
      Serial.println(resp4, BIN);
      Serial.println("Dagor: DRV cannot init.");
      return DRV_ERROR;
    }
  }

  //Set DRV83045's amplifier gain to 80x
  digitalWrite(cs, LOW);
  byte resp7 = SPI.transfer(B01010000);
  byte resp8 = SPI.transfer(B00111111);
  digitalWrite(cs, HIGH);
  //Serial.println(resp7, BIN);
  //Serial.println(resp8, BIN);

  if(resp){
    if (resp7 != 0b00000000 || resp8 != 0b00111111){
      Serial.println(resp7, BIN);
      Serial.println(resp8, BIN);
      Serial.println("Dagor: DRV cannot init.");
      return DRV_ERROR;
    }
  }

  drv_enable(true);

  return LIFE_IS_GOOD;
  
}

// Set or reset the enable gate of the DRV8305
void drv_enable(bool enabled){

  if (enabled){
    digitalWrite(enGate, HIGH);
    Serial.println("DRV8305: enGate Enabled");
    _delay(500);
  } else{
    digitalWrite(enGate, LOW);
    Serial.println("DRV8305: enGate Disabled");
    _delay(500);
  }
}

// Use the DRV8305 DC calibration mode, shorts the current control inputs to register 0 amps through the phase shunt resistor
void current_dc_calib(bool activate){
  
  if (activate){
    digitalWrite(cs, LOW);
    byte resp5 = SPI.transfer(B01010111);
    byte resp6 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    //Serial.println(resp5, BIN);
    //Serial.println(resp6, BIN);
  } //Activate DC calibration mode on DRV8305
  else {
    digitalWrite(cs, LOW);
    byte resp5 = SPI.transfer(B01010000);
    byte resp6 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    //Serial.println(resp5, BIN);
    //Serial.println(resp6, BIN);
  } //Deactivate DC calibration mode on DRV8305
}

// Reads one DRV8305 register. The reply comes in the same frame as the command.
uint16_t drvReadRegister(uint8_t address){
  SPI.beginTransaction(SPISettings(drvSpiClockHz, MSBFIRST, SPI_MODE1));
  digitalWrite(cs, LOW);
  uint8_t first = SPI.transfer(drvReadBit | (address << drvAddressShift));
  uint8_t second = SPI.transfer(0x00);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  return drvReplyData(first, second);
}

// Samples nFAULT for longer than one warning pulse period, returns a DrvPinActivity.
int drvSamplePin(){
  bool samplesLow[drvPinSamples];
  for (int i = 0; i < drvPinSamples; i++){
    samplesLow[i] = digitalRead(nFault) == LOW;
    delayMicroseconds(drvPinSampleGapUs);
  }
  return drvClassifyPin(samplesLow, drvPinSamples);
}

// Prints the set bits that are warnings or, with warnings false, the set bits that are faults.
void drvPrintStatus(const uint16_t* status, bool warnings){
  for (int reg = 0; reg < DRV_STATUS_REGISTERS; reg++){
    for (int bit = 0; bit < DRV_STATUS_BITS; bit++){
      const char* name = DRV_STATUS_NAMES[reg][bit];
      if (!name || !((status[reg] >> bit) & 1) || drvBitIsWarning(reg, bit) != warnings) continue;
      Serial.print("- ");
      Serial.println(name);
    }
  }
}

// Fault and warning monitor of the DRV8305, called by the DRV task. Datasheet section 7.3.8.1:
// a warning is report only, the chip takes no action and pulses nFAULT until a status register is read.
// A fault shuts the gate drivers down, holds nFAULT low and stays latched.
void drv_fault_status(){
  static bool faultReported = false;

  int pin = drvSamplePin();
  if (pin == DRV_PIN_IDLE){
    faultReported = false;
    return;
  }
  if (pin == DRV_PIN_FAULT && faultReported) return;

  uint16_t status[DRV_STATUS_REGISTERS];
  for (int i = 0; i < DRV_STATUS_REGISTERS; i++){
    status[i] = drvReadRegister(drvFirstStatusAddress + i);
  }

  if (!drvStatusLooksValid(status)){
    Serial.println(pin == DRV_PIN_FAULT ?
      "DRV8305: nFAULT is held low but the status registers are empty or all ones, is SPI answering?" :
      "DRV8305: nFAULT pulses but no status bit is set, the condition has passed or SPI is not answering.");
  }

  if (pin == DRV_PIN_FAULT || (status[0] & DRV_FAULT_FLAG)){
    state_machine = DRV_ERROR;
    faultReported = true;
    Serial.println("DRV8305: Fault ->");
    drvPrintStatus(status, false);
    if (status[0] & DRV_WARNING_BITS){
      Serial.println("DRV8305: Warnings active too ->");
      drvPrintStatus(status, true);
    }
  } else {
    Serial.println("DRV8305: Warning ->");
    drvPrintStatus(status, true);
  }
}
