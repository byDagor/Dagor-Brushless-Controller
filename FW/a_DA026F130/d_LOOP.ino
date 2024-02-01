//###########################################
//                  LOOP
//###########################################

// Do NOT add delay inside the loop void.

void loop() {
  if (state_machine == LIFE_IS_GOOD){

    // These functions have to run as fast as possible
    //motor.move();
    gravityComp();

    motor.loopFOC();
  
    #ifdef MONITORING
      command.run();
      //motor.monitor();
    #endif 

  }
  
  unsigned long timeDif = timeManagement();
  stateT += timeDif;
  
  //loopPeriod(false);

  // Fixed rate functions
  // Functions inside this "if" will execute at a 5hz rate (by default).
  // Uncomment/ comment the functions that you wish to use.
  if(stateT >= 1000000/callerFixedFreq){
    //loopPeriod(true);
    
    // Un/ comment the functions bellow that you wish to use.
    faultStatus();          // monitors faults and warnings of the DRV8305
    tempStatus(stateT);     // monitors operating temperature, true argument to print temprature
    
    
    stateT = 0;

    // Print for monitoring -> Uncomment to use.
    //voltageMonitor(true);
    //rotorData(false);          // true -> rotor velocity, false -> rotor position
    //phaseCurrents(false);        // true -> DQspace currents, false -> phase currents
    //phaseVoltages(true);    // true -> DQspace voltages, false -> phase voltages
    //Serial.println();       // if printing to serial monitor uncomment to insert a line break
  }
  
}