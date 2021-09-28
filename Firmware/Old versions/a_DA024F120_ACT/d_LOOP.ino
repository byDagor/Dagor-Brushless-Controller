
//###########################################
// LOOP
//###########################################

// Do not add delay inside the loop void.


void loop() {
  // These functions have to run as fast as possible
  timeManagement();
  motor.loopFOC();
  //command.run();
  motor.move();

  // Fixed rate functions
  // Functions inside this "if" will execute at a 5hz rate. Un/ comment the functions that you wish to use.
  if(stateT >= 1000000/callerFixedFreq){
    stateT = 0;

    // Un/ comment the functions bellow that you wish to use.
    tempStatus();
    faultStatus();
    //voltageMonitor(true);
    //rotorPosition();
    //printCurrents(true);
  
  }
  
}
