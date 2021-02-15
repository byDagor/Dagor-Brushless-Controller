
//###########################################
// LOOP
//###########################################

// Do not add delay inside the loop void.

void loop() {
  timeManagement();
  motor.loopFOC();
  motor.move();

  //Allows commands to be send through the serial monitor
  motor.command(serialReceiveUserCommand());

  // Fixed rate functions
  // Functions inside this "if" will execute at a fixed rate.
  if(stateT >= 1000000/fixedFreq){
    stateT = 0;

    // Un/ comment the functions bellow that you wish to use.
    tempStatus();
    faultStatus();
    voltageMonitor();
    //rotorPosition();
  }
  
}
