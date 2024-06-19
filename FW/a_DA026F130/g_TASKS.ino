//###########################################
//             TASKS - Core 0
//###########################################

// Commander callback to enable Active Compliance
void enableActiveComp(char* cmd){
  if(atoi(cmd) && motor.controller == MotionControlType::angle) {
    active_comp_mode = true;
    Serial.println("Dagor: Active Compliance enabled.");
  } else {
    active_comp_mode = false;
    Serial.println("Dagor: Active Compliance disabled.");
  }
}

// Commander callback to register new mechanical zero, AKA new mechanical home
void setNewMechanicalZero(char* cmd){
  if(atoi(cmd) == 1){
    Serial.print("Defining new Mecahnical zero at: ");
    Serial.println( sensor.getAngle() );
    if (motor.sensor_direction == Direction::CW){
      motor.sensor_offset =  sensor.getAngle();
      motor.target = 0;
    }
    else if (motor.sensor_direction == Direction::CCW){
      motor.sensor_offset = -sensor.getAngle();
      motor.target = 0;
    }
  }
}

// Gravity compensation mode
void activeComp(){

  #ifdef CURRENT_SENSE                     // Only works if current sense is available
    static float holdPosition = 0;         // New calculated target depending on external forces applied to the actuator's output
    //This inside the void loop
    if (active_comp_mode && motor.controller == MotionControlType::angle){

      if (motor.current.q > currentQthreshold && abs(motor.shaft_angle - holdPosition) > positionThreshold){
          holdPosition -= kgc * (motor.current.q - currentQthreshold);
      }
      else if (motor.current.q < -currentQthreshold && abs(motor.shaft_angle - holdPosition) > positionThreshold){
          holdPosition -= kgc * (motor.current.q + currentQthreshold);
      }
      //Serial.println(motor.current.q);
      //Serial.print(", ");
      //Serial.println(holdPosition);
      
      motor.move(holdPosition);
    }
  #endif

}

// Function to find the loop time, reports in microseconds. 
void printFOCfreq(){

  static float monitor_freq = 1; //[Hz]

  static unsigned long prevT = 0;
  static unsigned long timeDif = 0;
  static unsigned long runTime = 0;
  static unsigned long stateT = 0;

  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;

  static unsigned long loopCount = 1;

  if (stateT >= 1000000/monitor_freq){
    
    float lt = float(stateT)/float(loopCount);    
    float freq = 1/(lt/1000);

    Serial.print(freq);    //[kHz]
    Serial.println("");

    loopCount = 0;
    stateT = 0;
    
  }
  else{
    loopCount += 1;
  }
}

// Temperature status and manager  - TODO: Verify temperature formula
void tempStatus(void * print_temp){

  bool debug = *((bool *)print_temp);

  static float monitor_freq = 4; //[Hz]
  static int tFlag;
  static unsigned int totalTempTime;
  static unsigned long time_dif = 0, prev_time = 0, now = 0;

  while(1){

    now = micros();
    time_dif = now - prev_time;
    prev_time = now;

    //Read voltage from temperature sensor and transform it to °C
    float vOut = analogRead(vTemp);
    float temp = (((vOut*3.3f)/4095.0f)-1.8577f)/-0.01177f;
    
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
        digitalWrite(enGate, LOW);
        Serial.print("Dagor: DRV8305 enGate Disabled - Temperature protection: ");
        Serial.println(temp);
      }
      
    }
    else if (temp <= maxTemp && tFlag == false){
      totalTempTime = 0;
    }

    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);

  }
  
}

// Monitor the voltage of the power source (useful for battery powered devices).
void busVoltageMonitor(void * print_voltage){
  
  bool debug = *((bool *)print_voltage);

  static float monitor_freq = 0.5; //[Hz]
  float update_wait_time = 30; //[s]
  unsigned int counter = 0;
  float avg_voltage = 0;

  while(1){

    float rawVoltage = analogRead(vMonitor);
    float bVoltage = rawVoltage * 36.3/4095;

    avg_voltage += rawVoltage;
    counter++;

    if (counter >= update_wait_time/(1/monitor_freq)){
      avg_voltage = avg_voltage/counter;
      counter = 0;

      // TODO: Update driver voltage with battery voltage
      //driver.voltage_power_supply = avg_voltage;
    }

    if (debug == true){
      Serial.print("Volate: ");
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

void drv_task(void * no_param){
  static float monitor_freq = 4; //[Hz]

  while(1){
    drv_fault_status();
    vTaskDelay((1000/monitor_freq) / portTICK_PERIOD_MS);   
  }

}

void printStackLeft(){
  unsigned int stack = uxTaskGetStackHighWaterMark(nullptr);
    Serial.println(stack);
}
