
//###########################################
// SETUP FUNCTIONS
//###########################################


// Initialization of SimpleFOC
// Do NOT remove the delays in this function.
void SimpleFOCinit(){
  //Motor driver initialization
  
  _delay(500);

  sensor.init();                  // Initialise magnetic sensor hardware
  motor.linkSensor(&sensor);      // Link sensor to motor instance  
  
  
  // driver config, power supply voltage [V]
  driver.voltage_power_supply = sourceVoltage;
  driver.voltage_limit = sourceVoltage;
  driver.init();
  motor.linkDriver(&driver);

  if (focModulation) motor.foc_modulation = FOCModulationType::SinePWM;
  else motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set FOC loop to be used: MotionControlType::torque, velocity, angle, velocity_openloop, angle_openloop
  if (controlType == "C0") motor.controller = MotionControlType::torque;
  else if (controlType == "C1") motor.controller = MotionControlType::velocity;
  else motor.controller = MotionControlType::angle;
  
  // velocity PI controller parameters
  motor.PID_velocity.P = vp;
  motor.PID_velocity.I = vi;
  motor.PID_velocity.D = vd;
  motor.PID_velocity.output_ramp = voltageRamp;
  motor.LPF_velocity.Tf = lpVelFilter;
  
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

  motor.voltage_limit = phaseRes*maxCurrent; 
  // Measured phase resistance [ohms]
  //motor.phase_resistance = phaseRes;
  //motor.current_limit = maxCurrent; 

  // Sensor aligning voltage
  motor.voltage_sensor_align = (maxCurrent*phaseRes)*alignStrength;

  motor.useMonitoring(Serial);      // use monitoring functionality
  motor.init();                     // initialise motor
  
  if (skipCalibration && natDirection == "CW") motor.initFOC(elecOffset,CW);              // start FOC
  else if (skipCalibration && natDirection == "CCW") motor.initFOC(elecOffset,CCW);       // start FOC
  else motor.initFOC();                         // align sensor/ encoder and start FOC
  
  _delay(500);

  // define the motor id
  command.add(motorID, onMotor, " BLDC");
  Serial.println("DAGOR: Ready BLDC.");
  
}

//Configure DRV8305 to desired operation mode
void drv_init(){
  Serial.println("DRIVER: DRV8305 INIT");

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10000110);
  digitalWrite(cs, HIGH);
  //Serial.println(resp1, BIN);
  //Serial.println(resp2, BIN);
  
  _delay(500);

  Serial.println("DRIVER: enGate Enabled");
  digitalWrite(enGate, HIGH);
  
}
