//###########################################
//             TASKS - Core 0
//###########################################


// Temperature status and manager  - TODO: Verify temperature formula
void tempStatus(void * print_temp){

  bool debug = *((bool *)print_temp);

  static int tFlag;
  static unsigned int totalTempTime;
  static unsigned long time_dif = 0, prev_time = 0, now = 0;

  while(1){

    now = micros();
    time_dif = now - prev_time;
    prev_time = now;

    //Read voltage from temperature sensor and transform it to °C
    float temp = powerStageTemperature();
    
    if (debug == true) {
      Serial.print("Temp: ");
      Serial.println(temp,2);
    }
    
    if (temp >= maxTemp && tFlag == false){
      //unsigned long tempTime = micros();
      totalTempTime += time_dif;

      //If temperature is high for [overTempTime] seconds disable the controller
      if(totalTempTime >= overTempTime*1000000){
        tFlag = true;
        driver.disableGate();
        Serial.print("Dagor: DRV8305 enGate Disabled - Temperature protection: ");
        Serial.println(temp);
      }
      
    }
    else if (temp <= maxTemp && tFlag == false){
      totalTempTime = 0;
    }

    vTaskDelay((1000/tempMonitorFreq) / portTICK_PERIOD_MS);

  }
  
}

// Monitor the voltage of the power source (useful for battery powered devices).
void busVoltageMonitor(void * print_voltage){
  
  bool debug = *((bool *)print_voltage);

  static float monitor_freq = 0.5; //[Hz]
  float update_wait_time = 30; //[s]
  unsigned int counter = 0;
  float voltage_sum = 0;

  while(1){

    float bVoltage = readBusVoltage();

    voltage_sum += bVoltage;
    counter++;

    if (counter >= update_wait_time/(1/monitor_freq)){
      float avg_voltage = voltage_sum/counter;
      voltage_sum = 0;
      counter = 0;

      // TODO: Update driver voltage with battery voltage
      //driver.voltage_power_supply = avg_voltage;
    }

    if (debug == true){
      Serial.print("Voltage: ");
      Serial.println(bVoltage,2);
    }

    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);
  }
}

// Print the rotor position in radians or rotor velocity in radians over second
void rotorMonitor(void * print_data){

  static float monitor_freq = 50; //[Hz]

  static bool enable = true;

  bool debug = *((bool *)print_data);

  while(1){
  
    if((motor.shaft_angle>max_rotor_position) || (motor.shaft_angle<min_rotor_position)){
      //enable = false;
      //state_machine = OUT_BOUNDS_ERROR;
      //command.run("MC0");
      //command.run("M0");
      //drv_enable(false);
      motor.disable();
      Serial.println("MOT: Disabled, motor position out of bounds.");
    }

    if(debug){
      Serial.print(motor.shaft_angle,4); 
      Serial.print("\t");  
      Serial.println(motor.shaft_velocity,4); 
    }
    
    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);
  }

}

// Print each phase current or the current in the DQ space
void printCurrents(void * print_type){

  bool DQspace = *((bool *)print_type);
  static float monitor_freq = 200; //[Hz]

  while(1){

  #ifdef CURRENT_SENSE
    if(DQspace){
      //DQCurrent_s current = current_sense.getFOCCurrents( _electricalAngle(sensor.getMechanicalAngle() + 0.1*sensor.getVelocity(), motor.pole_pairs) );
      //DQCurrent_s current = current_sense.getFOCCurrents(motor.electrical_angle);
      Serial.print(motor.current.d);
      Serial.print("\t");
      Serial.println(motor.current.q);
    }
    else{
      PhaseCurrent_s currents = current_sense.getPhaseCurrents();
      Serial.print(currents.a); // [Amps]
      Serial.print("\t");
      Serial.print(currents.b); // [Amps]
      Serial.print("\t");
      Serial.println(currents.c); // [Amps]
    }
  #else
    Serial.println("Current sense not initialized.");
  #endif

  vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);

  }
}

// Print each phase voltage or the equivalent in the DQ space
void printVoltages(void * print_type){

  bool DQspace = *((bool *)print_type);
  static float monitor_freq = 200; //[Hz]

  while(1){

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
      Serial.print(motor.Uc); // [volts]
      Serial.print("\t");
    }
    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);
  }
}

const int drvConfigCheckCycles = 4;     // The DRV task checks the chip settings every 4th run, once a second

void drv_task(void * no_param){
  static float monitor_freq = 4; //[Hz]
  int cycle = 0;

  while(1){
    drv_fault_status();
    if (++cycle >= drvConfigCheckCycles){
      cycle = 0;
      drv_config_monitor();
    }
    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);   
  }

}

void taskTempSensor(){
  xTaskCreatePinnedToCore(
    tempStatus, /* Function to implement the task */
    "Temperature Sensor Monitor", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_temp,
    5,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskBusVoltage(){
  xTaskCreatePinnedToCore(
    busVoltageMonitor, /* Function to implement the task */
    "Bus Voltage Monitor", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_bus_voltage,
    3,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskPrintCurrents(){
  xTaskCreatePinnedToCore(
    printCurrents, /* Function to implement the task */
    "Monitor Currents", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_dq_currents,  /* Task input parameter */
    0,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskPrintVoltages(){
  xTaskCreatePinnedToCore(
    printVoltages, /* Function to implement the task */
    "Monitor Voltages", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_dq_voltages,  /* Task input parameter */
    0,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskRotorMonitor(){
  xTaskCreatePinnedToCore(
    rotorMonitor, /* Function to implement the task */
    "Rotor Position and Veloity", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_rotor_data,
    2,  /* Priority of the task */
    NULL,  /* Task handle. */
    1); /* Core where the task should run */
}

void taskDRVfault(){
  xTaskCreatePinnedToCore(
    drv_task, /* Function to implement the task */
    "DRV Fault Status", /* Name of the task */
    2048,  /* Stack size in words */
    NULL,
    4,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}