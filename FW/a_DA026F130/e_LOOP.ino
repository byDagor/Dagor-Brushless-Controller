//###########################################
//         SUPER LOOP - Core 1
//###########################################

// Do NOT add delay inside the loop void.

void loop() {

  if (state_machine < DRV_WARNING){
     // These functions have to run as fast as possible
    motor.loopFOC();
    motor.move();
    activeComp();

    if (print_foc_freq) printFOCfreq();

    #ifdef DEBUG_ADAPTER
      command.run();
      //motor.monitor();
    #endif 

    #ifdef RS485
      digitalWrite(COMMS_DIR, LOW);
      checkRS485();
    #endif

  } else{
    Serial.print("Error, current state: ");
    Serial.println();
    _delay(1000);
  }

}
