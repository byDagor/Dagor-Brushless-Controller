
//###########################################
// LOOP FUNCTIONS
//###########################################

// Time management for fixed rate functions
void timeManagement(){
  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;
}

// Temperature status and manager
void tempStatus(bool debug){
  static int tFlag;

  //Read voltage from temperature sensor and transform it to °C
  float vOut = analogRead(vTemp);
  float temp = (((vOut*3.3)/4095)-1.8577)/-0.01177;
  
  if (debug == true) {
    Serial.print("Temp: ");
    Serial.println(temp,2);
  }
  
  if (temp >= maxTemp && tFlag == false){
    //unsigned long tempTime = micros();
    totalTempTime += timeDif;

    //If temperature is high for [overTempTime] seconds disable controller
    if(totalTempTime >= overTempTime*1000000){
      tFlag = true;
      digitalWrite(enGate, LOW);
      Serial.print("enGate Disabled - Temperature protection: ");
      Serial.println(temp);
    }
    
  }
  else if (temp <= maxTemp && tFlag == false){
    totalTempTime = 0;
  }
  
}

// Monitor the voltage of the power source. Mainly used for battery powered devices
void voltageMonitor(bool debug){
  
  int rawVoltage = analogRead(vMonitor);
  int bVoltage = rawVoltage * 36.3/4095;

  if (debug == true){
    Serial.print("Volate: ");
    Serial.println(bVoltage,2);
  }
}

// Print the rotor position in radians
void rotorPosition(){
  float e = sensor.getAngle();
  Serial.print("Rads: ");
  Serial.println(e,4);
}

// Print each phase current or the DC equivalent
void printCurrents(bool dcEquivalent){
  
  if(dcEquivalent){
    float current_magnitude = current_sense.getDCCurrent();
    Serial.println(current_magnitude); // milli Amps
  }else{
    PhaseCurrent_s currents = current_sense.getPhaseCurrents();
    Serial.print(currents.a); // milli Amps
    Serial.print("\t");
    Serial.print(currents.b); // milli Amps
    Serial.print("\t");
    Serial.println(currents.c); // milli Amps
  }

}

// Fault status and manager for the DRV8305
// Datahseet pages 37 and 38
void faultStatus(){
  //Read nFault pin from DRV8305 - LOW == error / HIGH == normal operation
  int fault = digitalRead(nFault);
  
  if(fault == LOW && faultTrig == false){
    Serial.println("Fault detected");
    faultTrig = true;
    //Check warning and watchdog reset (Address = 0x1)
    digitalWrite(cs, LOW);
    byte ft1 = SPI.transfer(B10001000);
    byte ft2 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x1");
    Serial.println(ft1,BIN);
    Serial.println(ft2,BIN);

    //Check OV/VDS Faults (Address = 0x2)
    digitalWrite(cs, LOW);
    byte ft3 = SPI.transfer(B10010000);
    byte ft4 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x2");
    Serial.println(ft3,BIN);
    Serial.println(ft4,BIN);

    //Check IC Faults (Address = 0x3)
    digitalWrite(cs, LOW);
    byte ft5 = SPI.transfer(B10011000);
    byte ft6 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x3");
    Serial.println(ft5,BIN);
    Serial.println(ft6,BIN);

    //Check VGS Faults (Address = 0x4)
    digitalWrite(cs, LOW);
    byte ft7 = SPI.transfer(B10100000);
    byte ft8 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x4");
    Serial.println(ft7,BIN);
    Serial.println(ft8,BIN);
  }
}
