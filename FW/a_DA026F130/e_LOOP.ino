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

    if(!motor.enabled) motor.target = motor.shaft_angle;

    #ifdef DEBUG_ADAPTER
      command.run();
      //motor.monitor();
    #endif 

  } else{

    static bool error_printed = false;

    if (!error_printed){
      error_printed = true;
      Serial.print("Dagor: Error -> State Machine: ");
      Serial.println(state_machine);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);

  }

}
