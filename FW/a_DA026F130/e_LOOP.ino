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

  } 
  else{
    Serial.print("Dagor: Error -> State Machine: ");
    Serial.println(state_machine);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

}
