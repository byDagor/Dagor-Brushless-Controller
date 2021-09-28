
//###########################################
//      SETUP FUNCTIONS DEFINITIONS
//###########################################


// Initialization of SimpleFOC
// Do NOT remove the delays in this function.
void SimpleFOCinit(){

  Serial.println("DAGOR: Init SimpleFOC");
  //Motor driver initialization
  
  _delay(500);

  #ifdef ENCODER                        // If using ABI interface of magnetic sensor
    sensor.init();                      // Initialise magnetic sensor hardware
    sensor.enableInterrupts(doA, doB);  // Enable interrupts for quadrature signals
    motor.linkSensor(&sensor);          // Link sensor to motor instance  
  #else                                 // Default, SPI interface of magnetic sensor
    sensor.init();                      // Initialise magnetic sensor hardware
    motor.linkSensor(&sensor);          // Link sensor to motor instance 
  #endif
  
  // driver config, power supply voltage [V]
  driver.voltage_power_supply = sourceVoltage;
  //driver.voltage_limit = sourceVoltage;
  driver.init();
  motor.linkDriver(&driver);

  Serial.println("DAGOR: Init current sense");
  // current sense init hardware
  current_sense.init();
  current_sense.driverSync(&driver);
  
  // link the current sense to the motor
  if (trueTorque){
    motor.linkCurrentSense(&current_sense);
    motor.torque_controller = TorqueControlType::foc_current; 
    motor.voltage_limit = phaseRes*maxPowersourceCurrent; 
  }
  else{
    motor.torque_controller = TorqueControlType::voltage; 

    motor.voltage_limit = phaseRes*maxPowersourceCurrent; 
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
  motor.voltage_sensor_align = (maxPowersourceCurrent*phaseRes)*alignStrength;
  Serial.print("MOT: Align voltage -> ");
  Serial.println(motor.voltage_sensor_align);

  // Current PI controller parameters
  motor.PID_current_q.P = cp;
  motor.PID_current_q.I = ci;
  motor.PID_current_q.D = cd;
  motor.PID_current_q.limit = phaseRes*maxPowersourceCurrent;
  motor.PID_current_q.output_ramp = voltageRamp;
  motor.LPF_current_q.Tf = lpQDFilter;

  motor.PID_current_d.P = cp;
  motor.PID_current_d.I = ci;
  motor.PID_current_d.D = cd;
  motor.PID_current_d.limit = phaseRes*maxPowersourceCurrent;
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
  
  if (skipCalibration && natDirection == "CW") motor.initFOC(elecOffset,CW);              // start FOC
  else if (skipCalibration && natDirection == "CCW") motor.initFOC(elecOffset,CCW);       // start FOC
  else motor.initFOC();                                         // align sensor/ encoder and start FOC
  
  _delay(500);

  // define the motor id
  command.add(motorID, onMotor, " BLDC");
  if (!commandDebug) command.verbose = VerboseMode::nothing;
  commandEspNow.add(motorID, onMotor, " BLDC");
  //if (!commandDebug) commandEspNow.verbose = VerboseMode::nothing;

  motor.PID_current_q.limit = phaseRes*maxPowersourceCurrent;   //[volts]
  motor.PID_current_d.limit = phaseRes*maxPowersourceCurrent;   //[volts]

  motor.monitor_variables = _MON_VOLT_Q; 
    // downsampling
  motor.monitor_downsample = 50; // default 10
  
  Serial.println("DAGOR: Ready BLDC.");
  
}

//Configure DRV8305 to desired operation mode
void drv_init(){
  Serial.println("DAGOR: DRV8305 INIT");

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10000110);
  digitalWrite(cs, HIGH);
  //Serial.println(resp1, BIN);
  //Serial.println(resp2, BIN);

  //Clamp sense amplifier output to 3.3V - security measure
  digitalWrite(cs, LOW);
  byte resp3 = SPI.transfer(B01001100);
  byte resp4 = SPI.transfer(B10100000);
  digitalWrite(cs, HIGH);
  //Serial.println(resp1, BIN);
  //Serial.println(resp2, BIN);

  //Set DRV83045's amplifier gain to 20x
  digitalWrite(cs, LOW);
  byte resp7 = SPI.transfer(B01010000);
  byte resp8 = SPI.transfer(B00101010);
  digitalWrite(cs, HIGH);
  //Serial.println(resp7, BIN);
  //Serial.println(resp8, BIN);

  Serial.println("DAGOR: enGate Enabled");
  digitalWrite(enGate, HIGH);
  _delay(500);
  
}

void current_dc_calib(bool activate){
  //Activate DC calibration mode on DRV8305
  if (activate){
    digitalWrite(cs, LOW);
    byte resp5 = SPI.transfer(B01010111);
    byte resp6 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    //Serial.println(resp5, BIN);
    //Serial.println(resp6, BIN);
  }
  //Deactivate DC calibration mode on DRV8305
}

void spi_init(){
  //SPI start up
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
}

void gpio_init(){
  //Pinmodes assignment
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
  pinMode(so1, INPUT);
  pinMode(so2, INPUT);
  pinMode(so3, INPUT);
  pinMode(nFault, INPUT);
  pinMode(enGate, OUTPUT);
  digitalWrite(enGate, LOW);
}
