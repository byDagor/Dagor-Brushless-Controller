
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
    int tempTime = micros();
    totalTempTime += tempTime;

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

// Monitor the voltage of the power source. Relevant when using battery powered devices.
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

// Fault status and manager for the DRV8305
// Datahseet pages 37 and 38
void faultStatus(){
  // Read nFault pin from DRV8305 - LOW == error / HIGH == normal operation
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

// Utility function enabling serial communication
String serialReceiveUserCommand() {
  // A string to hold incoming data
  static String received_chars;
  String command = "";

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;
    // end of user input
    if (inChar == '\n') {
      // execute the user command
      command = received_chars;
      // reset the command buffer 
      received_chars = "";
    }
  }
  return command;
}
