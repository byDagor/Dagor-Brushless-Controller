//###########################################
//         SUPER LOOP - Core 1
//###########################################

// Do NOT add delay inside the loop void.

void loop() {

  #ifdef OTA_UPDATE
    if (updateRequested) enterUpdateMode();
  #endif

  if (imagePending) validateImageWhenHealthy();

  if (state_machine < DRV_WARNING){
     // These functions have to run as fast as possible
    motor.loopFOC();
    // activeComp() returns true when it already ran motor.move() with its own target.
    if (!activeComp()) motor.move();

    if (print_foc_freq) printFOCfreq();

    #if defined(CURRENT_OUTLIER_TEST) && defined(CURRENT_SENSE)
      currentOutlierMonitor();
    #endif

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
    #ifdef DEBUG_ADAPTER
      commandSystem.run();    // so a controller in error can still be updated wirelessly (U1)
    #endif

    vTaskDelay(1 / portTICK_PERIOD_MS);

  }

}
