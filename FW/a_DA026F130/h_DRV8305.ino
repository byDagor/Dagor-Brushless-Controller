//###########################################
//                 DRV8305
//###########################################


//Configure DRV8305 to desired operation mode
// TODO: verify commands were sent correcty to DRV
void drv_init(){
  Serial.println("DAGOR: DRV8305 INIT");

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10010110);
  digitalWrite(cs, HIGH);
  //Serial.println(resp1, BIN);
  //Serial.println(resp2, BIN);

  //Clamp sense amplifier output to 3.3V - protect ESP32 adc
  digitalWrite(cs, LOW);
  byte resp3 = SPI.transfer(B01001100);
  byte resp4 = SPI.transfer(B10100000);
  digitalWrite(cs, HIGH);
  //Serial.println(resp1, BIN);
  //Serial.println(resp2, BIN);

  //Set DRV83045's amplifier gain to 80x
  digitalWrite(cs, LOW);
  byte resp7 = SPI.transfer(B01010000);
  byte resp8 = SPI.transfer(B00111111);
  digitalWrite(cs, HIGH);
  //Serial.println(resp7, BIN);
  //Serial.println(resp8, BIN);

  drv_enable(true);
  
}

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


// Fault status and manager for the DRV8305 -> Datahseet pages 37 and 38
void faultStatus(){
  static bool faultTrig = false;
  //Read nFault pin from DRV8305 - LOW == error / HIGH == normal operation
  int fault = digitalRead(nFault);
  
  if(fault == LOW && faultTrig == false){
    int is_error = 0;
    unsigned int faults = 0;

    Serial.println("DRV8305: Fault detected ->");
    faultTrig = true;

    //Check warning and watchdog reset (Address = 0x1)
    digitalWrite(cs, LOW);
    byte ft1 = SPI.transfer(0b10001000);
    byte ft2 = SPI.transfer(0b00000000);
    digitalWrite(cs, HIGH);
    //Serial.println("Address = 0x1");
    //Serial.println(ft1,BIN);
    //Serial.println(ft2,BIN);
    
    BYTE_CONCATENATE(ft2, (ft1 & 0b0000001111111111), faults);

    for(int i = 0; i<=10; i++){

      FIND_BIT(faults, i, is_error);

      if (is_error){
        switch(i){
          case 0:
            Serial.println("- Overtemperature warning");
            break;
          case 1:
            Serial.println("- Temperature flag setting for approximately 135°C");
            break;
          case 2:
            Serial.println("- Temperature flag setting for approximately 125°C");
            break;
          case 3:
            Serial.println("- Temperature flag setting for approximately 105°C");
            break;
          case 4:
            Serial.println("- Charge pump undervoltage flag warnin");
            break;
          case 5:
            Serial.println("- Real time OR of all VDS overcurrent monitors");
            break;
          case 6:
            Serial.println("- PVDD overvoltage flag warning");
            break;
          case 7:
            Serial.println("- PVDD undervoltage flag warning");
            break;
          case 8:
            Serial.println("- Temperature flag setting for approximately 175°C");
            break;
          case 9:
            Serial.println("-");
            break;
          case 10:
            Serial.println("- Fault indication");
            break;
        }
      }
    }

    //Check OV/VDS Faults (Address = 0x2)
    digitalWrite(cs, LOW);
    byte ft3 = SPI.transfer(0b10010000);
    byte ft4 = SPI.transfer(0b00000000);
    digitalWrite(cs, HIGH);
    //Serial.println("Address = 0x2");
    //Serial.println(ft3,BIN);
    //Serial.println(ft4,BIN);

    BYTE_CONCATENATE(ft4, (ft3 & 0b0000001111111111), faults);

    for(int i = 0; i<=10; i++){

      FIND_BIT(faults, i, is_error);

      if (is_error){
        switch(i){
          case 0:
            Serial.println("- Sense A overcurrent fault");
            break;
          case 1:
            Serial.println("- Sense B overcurrent fault");
            break;
          case 2:
            Serial.println("- Sense C overcurrent fault");
            break;
          case 3:
            Serial.println("-");
            break;
          case 4:
            Serial.println("-");
            break;
          case 5:
            Serial.println("- VDS overcurrent fault for low-side MOSFET C");
            break;
          case 6:
            Serial.println("- VDS overcurrent fault for high-side MOSFET C");
            break;
          case 7:
            Serial.println("- VDS overcurrent fault for low-side MOSFET B");
            break;
          case 8:
            Serial.println("- VDS overcurrent fault for high-side MOSFET B");
            break;
          case 9:
            Serial.println("- VDS overcurrent fault for low-side MOSFET A");
            break;
          case 10:
            Serial.println("- VDS overcurrent fault for high-side MOSFET A");
            break;
        }
      }
    }

    //Check IC Faults (Address = 0x3)
    digitalWrite(cs, LOW);
    byte ft5 = SPI.transfer(0b10011000);
    byte ft6 = SPI.transfer(0b00000000);
    digitalWrite(cs, HIGH);
    //Serial.println("Address = 0x3");
    //Serial.println(ft5,BIN);
    //Serial.println(ft6,BIN);

    BYTE_CONCATENATE(ft6, (ft5 & 0b0000001111111111), faults);

    for(int i = 0; i<=10; i++){

      FIND_BIT(faults, i, is_error);

      if (is_error){
        switch(i){
          case 0:
            Serial.println("- High-side charge pump overvoltage ABS fault");
            break;
          case 1:
            Serial.println("- High-side charge pump overvoltage fault");
            break;
          case 2:
            Serial.println("- High-side charge pump undervoltage 2 fault");
            break;
          case 3:
            Serial.println("-");
            break;
          case 4:
            Serial.println("- Low-side gate supply fault");
            break;
          case 5:
            Serial.println("- AVDD undervoltage fault");
            break;
          case 6:
            Serial.println("- VREG undervoltage fault");
            break;
          case 7:
            Serial.println("-");
            break;
          case 8:
            Serial.println("- Overtemperature fault");
            break;
          case 9:
            Serial.println("- Watchdog fault");
            break;
          case 10:
            Serial.println("- PVDD undervoltage 2 fault");
            break;
        }
      }
    }

    //Check VGS Faults (Address = 0x4)
    digitalWrite(cs, LOW);
    byte ft7 = SPI.transfer(0b10100000);
    byte ft8 = SPI.transfer(0b00000000);
    digitalWrite(cs, HIGH);
    //Serial.println("Address = 0x4");
    //Serial.println(ft7,BIN);
    //Serial.println(ft8,BIN);

    BYTE_CONCATENATE(ft8, (ft7 & 0b0000001111111111), faults);

    for(int i = 0; i<=10; i++){

      FIND_BIT(faults, i, is_error);

      if (is_error){
        switch(i){
          case 0:
            Serial.println("-");
            break;
          case 1:
            Serial.println("-");
            break;
          case 2:
            Serial.println("-");
            break;
          case 3:
            Serial.println("-");
            break;
          case 4:
            Serial.println("-");
            break;
          case 5:
            Serial.println("- VGS gate drive fault for low-side MOSFET C");
            break;
          case 6:
            Serial.println("- VGS gate drive fault for high-side MOSFET C");
            break;
          case 7:
            Serial.println("- VGS gate drive fault for low-side MOSFET B");
            break;
          case 8:
            Serial.println("- VGS gate drive fault for high-side MOSFET B");
            break;
          case 9:
            Serial.println("- VGS gate drive fault for low-side MOSFET A");
            break;
          case 10:
            Serial.println("- VGS gate drive fault for high-side MOSFET A");
            break;
        }
      }
    }

    if((ft1 & ft2 & ft3 & ft4 & ft5 & ft6 & ft7 & ft8) == 0) Serial.println("- Not responding.");

  }
}
