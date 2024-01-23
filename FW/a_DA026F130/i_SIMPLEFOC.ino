//###########################################
//               SimpleFOC
//###########################################


// Initialization of SimpleFOC
// Do NOT remove the delays in this function.
int SimpleFOCinit(){

  Serial.println("DAGOR: Init SimpleFOC");
  //Motor driver initialization
  
  _delay(500);

  #ifdef ENCODER                          // If using ABI interface of magnetic sensor
    sensor.init();                        // Initialise magnetic sensor hardware
    sensor.enableInterrupts(doA, doB);    // Enable interrupts for quadrature signals
    motor.linkSensor(&sensor);            // Link sensor to motor instance  
  #else                                   // Default, SPI interface of magnetic sensor
    sensor.init();                        // Initialise magnetic sensor hardware
    motor.linkSensor(&sensor);            // Link sensor to motor instance 
  #endif
  
  // driver config, power supply voltage [V]
  driver.voltage_power_supply = sourceVoltage;
  driver.voltage_limit = sourceVoltage;
  motor.linkDriver(&driver);
  driver.init();
  current_sense.linkDriver(&driver);
  
  // link the current sense to the motor
  if (trueTorque){
    motor.linkCurrentSense(&current_sense);
    motor.torque_controller = TorqueControlType::foc_current; 
    motor.voltage_limit = sourceVoltage; //phaseRes*maxPowersourceCurrent; 
  }
  else{
    motor.torque_controller = TorqueControlType::voltage; 
    //driver.voltage_limit = sourceVoltage;
    motor.voltage_limit = sourceVoltage; //phaseRes*maxPowersourceCurrent; 
    // Measured phase resistance [ohms]
    //motor.phase_resistance = phaseRes;
    //motor.current_limit = maxPowersourceCurrent;
  }
  

  if (focModulation) motor.foc_modulation = FOCModulationType::SinePWM;
  else motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set FOC loop to be used: MotionControlType::torque, velocity, angle, velocity_openloop, angle_openloop
  if (controlType == "C0") motor.controller = MotionControlType::torque;
  else if (controlType == "C1") motor.controller = MotionControlType::velocity;
  else motor.controller = MotionControlType::angle;

  // Sensor aligning voltage
  motor.voltage_sensor_align = sourceVoltage*alignStrength; //(maxPowersourceCurrent*phaseRes)*alignStrength;
  Serial.print("MOT: Align voltage -> ");
  Serial.println(motor.voltage_sensor_align);

  // Current PI controller parameters
  motor.PID_current_q.P = cp;
  motor.PID_current_q.I = ci;
  motor.PID_current_q.D = cd;
  motor.PID_current_q.limit = sourceVoltage; //phaseRes*maxPowersourceCurrent;
  motor.PID_current_q.output_ramp = voltageRamp;
  motor.LPF_current_q.Tf = lpQDFilter;

  motor.PID_current_d.P = cp;
  motor.PID_current_d.I = ci;
  motor.PID_current_d.D = cd;
  motor.PID_current_d.limit = sourceVoltage; //phaseRes*maxPowersourceCurrent;
  motor.PID_current_d.output_ramp = voltageRamp;
  motor.LPF_current_d.Tf = lpQDFilter;

  /*
  // velocity PI controller parameters
  motor.PID_velocity.P = vp;
  motor.PID_velocity.I = vi;
  motor.PID_velocity.D = vd;
  motor.PID_velocity.output_ramp = voltageRamp;
  motor.LPF_velocity.Tf = lpVelFilter;
  */
  
  // angle P controller
  motor.P_angle.P = ap;
  motor.P_angle.I = ai;
  motor.P_angle.D = ad;
  // maximal velocity of the poisition control
  motor.velocity_limit = velocityLimit;
  motor.LPF_angle.Tf = lpPosFilter;
  
  
  // Position offset, used to define an absolute 0 position [rads]
  motor.sensor_offset = sensorOffset;

  // Downsampling value of the motion control loops with respect to torque loops
  motor.motion_downsample = motionDownSample; // - times (default 0 - disabled)

  motor.useMonitoring(Serial);      // use monitoring functionality
  motor.init();                     // initialise motor
  
  motor.monitor_variables = _MON_VOLT_Q; 
  motor.monitor_downsample = 50; // downsampling, default 10

  Serial.println("DAGOR: Init current sense");
  // current sense init hardware
  current_sense.init();
  // link motor and current sense
  motor.linkCurrentSense(&current_sense);
  // skip alignment procedure
  current_sense.skip_align = true;

  int initFOC_exit_code = 999;

  if (skipCalibration && natDirection == "CW") initFOC_exit_code =  motor.initFOC(elecOffset,CW);              // start FOC skipping sensor calibration
  else if (skipCalibration && natDirection == "CCW") initFOC_exit_code = motor.initFOC(elecOffset,CCW);        // start FOC skipping sensor calibration
  else {
    initFOC_exit_code = motor.initFOC();                                                  // align sensor/ encoder and start FOC
  }
  
  if (initFOC_exit_code == 1) {
    _delay(500);

    // define the motor id
    command.add(motorID, onMotor, " BLDC");
    if (!commandDebug) command.verbose = VerboseMode::nothing;
        
    Serial.println("DAGOR: Ready BLDC.");    
  }
  else {
    drv_deinit();
    Serial.println("DAGOR: Could not initialize.");
    
  }

  #ifdef ESP_NOW
      commandEspNow.add(motorID, onMotor, " BLDC");
      if (!commandDebug) commandEspNow.verbose = VerboseMode::nothing;
  #endif

  return initFOC_exit_code;
}