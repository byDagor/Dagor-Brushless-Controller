
//###########################################
//       LOOP FUNCTIONS DEFINITIONS
//###########################################

// Time management for fixed rate functions
void timeManagement(){
  loopCount += 1;
  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;
}


// Gravity compensation mode
void gravityComp(){
  static float holdPosition = 0;         //moving position depending on external forces applied to the actuator's output
  //This inside the void loop
  if (gravityCompMode){

    float ea = motor.electricalAngle();
    DQCurrent_s current = current_sense.getFOCCurrents(ea);
  
    if (current.q > currentQthreshold && abs(sensor.getAngle() - holdPosition) > positionThreshold){
        holdPosition -= kgc * (current.q - currentQthreshold);
    }
    else if (current.q < -currentQthreshold && abs(sensor.getAngle() - holdPosition) > positionThreshold){
        holdPosition -= kgc * (current.q + currentQthreshold);
    }
    //Serial.println(current.q);
    //Serial.print(", ");
    //Serial.println}

    (holdPosition);
    motor.move(holdPosition);
    
  }
  else{
    motor.move();
  }
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

// Monitor the voltage of the power source (useful for battery powered devices).
void voltageMonitor(bool debug){
  
  int rawVoltage = analogRead(vMonitor);
  int bVoltage = rawVoltage * 36.3/4095;

  if (debug == true){
    Serial.print("Volate: ");
    Serial.println(bVoltage,2);
  }
}

// Print the rotor position in radians or rotor velocity in radians over second
void rotorData(bool rotorVelocity){
  if (!rotorVelocity){
    float e = sensor.getAngle();
    Serial.print("Rads: ");
    Serial.print(e,4);    
  }
  else{
    float e = sensor.getVelocity();
    Serial.print("Vel: ");
    Serial.print(e,4); 
  }

}

// Function to find the loop time, reports in microseconds. 
void loopTime(){
  float lt = float(stateT)/float(loopCount);
  loopCount = 0;
  //Serial.println("Loop Time: );
  //Serial.print(", ");
  Serial.println(lt);
}

// Print each phase current or the current in the DQ space
void phaseCurrents(bool DQspace){
  
  if(DQspace){
    //float current_magnitude = current_sense.getDCCurrent();
    //Serial.println(current_magnitude); // milli Amps

    DQCurrent_s current = current_sense.getFOCCurrents(motor.electricalAngle());
    Serial.print(current.d);
    Serial.print("\t");
    Serial.print(current.q);
    Serial.print("\t");    
  }
  else{
    PhaseCurrent_s currents = current_sense.getPhaseCurrents();
    Serial.print(currents.a); // [Amps]
    Serial.print("\t");
    Serial.print(currents.b); // [Amps]
    Serial.print("\t");
    Serial.print(currents.c); // [Amps]
  }
}

// Print each phase voltage or the equivalent in the DQ space
void phaseVoltages(bool DQspace){
  if (DQspace){
    //DQVoltage_s voltage;
    Serial.print(motor.voltage.d); // milli Amps
    Serial.print("\t");
    Serial.print(motor.voltage.q); // milli Amps
    Serial.print("\t");
  }
  else{
    Serial.print(motor.Ua); // [volts]
    Serial.print("\t");
    Serial.print(motor.Ub); // [volts]
    Serial.print("\t");
    Serial.println(motor.Uc); // [volts]
  }
}

// Fault status and manager for the DRV8305 -> Datahseet pages 37 and 38
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
