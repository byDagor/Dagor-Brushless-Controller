//###########################################
//                 DRV8305
//###########################################
// The chip is driven by the DRV8305Driver3PWM library class. The instance "driver" is declared in tab c.

#include "drv8305_status.h"

// Overcurrent trip of the DRV8305 VDS monitor. It protects the MOSFETs and the board, it is not the motor current limit (amp_limit).
// The chip trips at this voltage divided by RDS(on). TPH4R008NH: 4.0 mohm max at 25 C, about 6.4 mohm hot,
// so 0.282 V trips at about 44 A with hot MOSFETs and 70 A with cold ones.
const DRV8305_VDSLevel drvVdsLevel = DRV8305_VDSLevel::VDSLevel_282mV;

// True if every setting the firmware depends on still reads back as written.
bool drvSettingsOk(){
  return driver.isConfigured()
    && driver.getDeadTime() == DRV8305_DeadTime::DeadTime_52ns
    && driver.getVDSBlankingTime() == DRV8305_Time::Time_1us75
    && driver.getVDSDeglitchTime() == DRV8305_Time::Time_3us5
    && driver.getVDSMode() == DRV8305_VDSMode::VDSMode_LatchedShutdown
    && driver.getVDSLevel() == drvVdsLevel
    && driver.getCurrentSenseGain_A() == DRV8305_CSAGain::CSAGain_80
    && driver.getCurrentSenseGain_B() == DRV8305_CSAGain::CSAGain_80
    && driver.getCurrentSenseGain_C() == DRV8305_CSAGain::CSAGain_80;
}

// Writes the settings of this board and reads them back, returns false if the chip did not keep them.
// Call it with the gate driver off, it is left off.
bool drvConfigure(){
  Serial.println("DAGOR: DRV8305 INIT");

  // 3-PWM mode, overtemperature shutdown on, sense amplifier clamp on (protects the ESP32 ADC), watchdog off
  driver.configure();

  driver.enableGate();
  driver.setDeadTime(DRV8305_DeadTime::DeadTime_52ns);
  driver.setVDSBlankingTime(DRV8305_Time::Time_1us75);
  driver.setVDSDeglitchTime(DRV8305_Time::Time_3us5);
  driver.setVDSMode(DRV8305_VDSMode::VDSMode_LatchedShutdown);   // overcurrent shuts the gates off, never switch this protection off
  driver.setVDSLevel(drvVdsLevel);
  driver.setCurrentSenseGain(DRV8305_CSAGain::CSAGain_80);
  bool ok = drvSettingsOk();
  driver.disableGate();
  return ok;
}

// Set or reset the enable gate of the DRV8305
void drv_enable(bool enabled){

  if (enabled){
    driver.enableGate();
    Serial.println("DRV8305: enGate Enabled");
  } else{
    driver.disableGate();
    Serial.println("DRV8305: enGate Disabled");
  }
  _delay(500);
}

// Use the DRV8305 DC calibration mode, shorts the current control inputs to register 0 amps through the phase shunt resistor
void current_dc_calib(bool activate){
  driver.setCurrentSenseCalibration(activate);
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

  DRV8305_FaultPin pin = driver.getFaultPin();    // sample the pin before the status read, that read ends a warning pulse train
  if (pin == DRV8305_FaultPin::Idle){
    faultReported = false;
    return;
  }
  if (pin == DRV8305_FaultPin::Fault && faultReported) return;

  DRV8305Status chip = driver.getStatus();
  uint16_t status[DRV_STATUS_REGISTERS] = {chip.warning_reg.reg, chip.vds_reg.reg, chip.ic_reg.reg, chip.vgs_reg.reg};

  if (!drvStatusLooksValid(status)){
    Serial.println(pin == DRV8305_FaultPin::Fault ?
      "DRV8305: nFAULT is held low but the status registers are empty or all ones, is SPI answering?" :
      "DRV8305: nFAULT pulses but no status bit is set, the condition has passed or SPI is not answering.");
  }

  if (pin == DRV8305_FaultPin::Fault || (status[0] & DRV_FAULT_FLAG)){
    driver.disableGate();   // a real fault already switched the outputs off, this also covers a chip that stopped answering
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

// A supply dip below the DRV8305 undervoltage lockout resets the chip to its default settings without a fault or a warning.
// The gate driver keeps switching but no longer reads the PWM inputs as 3-PWM, so the motor is not driven correctly.
void drv_config_monitor(){
  if (state_machine >= DRV_ERROR || drvSettingsOk()) return;

  driver.disableGate();
  state_machine = DRV_ERROR;
  Serial.println("DRV8305: Configuration lost, the chip reset to its defaults (supply dip?). Gate driver off, restart the controller.");
}
