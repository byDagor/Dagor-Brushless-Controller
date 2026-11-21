//###########################################
//         SUPER LOOP - Core 1
//###########################################

// Do NOT add delay inside the loop void.

void FOC_HOT loop() {

  loopIterations++;

  #ifdef OTA_UPDATE
    if (updateRequested) enterUpdateMode();
  #endif

  if (imagePending) validateImageWhenHealthy();

  if (state_machine < DRV_WARNING){
     // These functions have to run as fast as possible
    waitForCurrentSamples();
    motor.loopFOC();
    // activeComp() returns true when it already ran motor.move() with its own target.
    if (!activeComp()) motor.move();

    if (print_foc_freq) printFOCfreq();

    #if defined(CURRENT_OUTLIER_TEST) && defined(CURRENT_SENSE)
      currentOutlierMonitor();
    #endif

    if(!motor.enabled) motor.target = motor.shaft_angle;

    if (statsActive) statsAccumulate();


    #ifdef DEBUG_ADAPTER
      // The serial poll costs more than the rest of the loop, a few milliseconds of command latency do not matter
      static uint8_t commandDivider = 0;
      if (++commandDivider >= commandPollDivider){
        commandDivider = 0;
        command.run();
      }
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


// The current sense interrupt samples one phase per PWM period, alternating, so a fresh pair of phase currents completes every second
// period (10 kHz at 20 kHz PWM). This starts each loop iteration right after a pair completes, so every iteration works on a pair it
// has not used yet and the loop runs at exactly that rate.
// The PWM and the CPU cycle counter run from the same crystal, so pairs complete at an exact period. The first completion is found by
// polling the sample index, then the loop is paced by time, which keeps it locked whatever it was doing when it arrived. The index
// only confirms that the pair is complete. Every wait is bounded by the 8 kHz limit, so a loop without samples (sensing stopped,
// gates off) still runs.
void FOC_HOT waitForCurrentSamples(){
  static uint32_t lastStart = 0;         // CPU cycle count when the previous iteration started
  static uint32_t nextPair = 0;          // CPU cycle count when the next pair completes
  static bool locked = false;
  if (currentSampleIndex){
    if (locked){
      if ((int32_t)(esp_cpu_get_cycle_count() - nextPair) >= 0) loopLateStarts++;
      while ((int32_t)(esp_cpu_get_cycle_count() - nextPair) < 0);
      // The pair is complete when the index is back at 0. A late interrupt gets a short grace time, then the edge is searched again.
      while (*currentSampleIndex != 0 && esp_cpu_get_cycle_count() - nextPair < currentPairGraceCycles);
      if (*currentSampleIndex != 0){ locked = false; loopLockLosses++; }
      // The next completion, skipping the ones a long iteration made us miss
      uint32_t now = esp_cpu_get_cycle_count();
      nextPair += currentPairCycles;
      if ((int32_t)(now - nextPair) >= 0) nextPair += currentPairCycles * ((now - nextPair) / currentPairCycles + 1);
    } else {
      // Index 1: the first phase was just sampled. Index 0 afterwards: the second phase is done, the pair is complete.
      while (*currentSampleIndex != 1 && esp_cpu_get_cycle_count() - lastStart < loopTimeoutCycles);
      while (*currentSampleIndex != 0 && esp_cpu_get_cycle_count() - lastStart < loopTimeoutCycles);
      nextPair = esp_cpu_get_cycle_count() + currentPairCycles;
      locked = (esp_cpu_get_cycle_count() - lastStart < loopTimeoutCycles);
      if (!locked) loopTimeouts++;
    }
  }
  lastStart = esp_cpu_get_cycle_count();
}
