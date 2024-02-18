//###########################################
//               SimpleFOC
//###########################################



//#############_SIMPLEFOC INSTANCES_#################
BLDCMotor motor = BLDCMotor(pp, phaseRes);                                              //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(INHC, INHB, INHA);                               //3PWM Driver instance

#ifdef CURRENT_SENSE
  LowsideCurrentSense current_sense = LowsideCurrentSense(0.002f, 40.0f, SO1, SO2);       //Current sensing instance
#endif

#ifdef ENCODER
  Encoder sensor = Encoder(32, 17, 512);       // Quadrature encoder instance
  // Interrupt routine intialisation -> channel A and B callbacks
  void doA(){sensor.handleA();}
  void doB(){sensor.handleB();}
#else
  MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, sensorCS);     //SPI Magnetic sensor instance
#endif

#ifdef CALIBRATED_SENSOR
  CalibratedSensor sensor_calibrated = CalibratedSensor(sensor);          // instantiate the calibrated sensor object
#endif

//#############_COMMANDER INTERFACE_#################
Commander command = Commander(Serial);
void onMotor(char* cmd){ command.motor(&motor, cmd); }

#if defined(ESP_NOW) || defined(RS485)
  Commander commandEspNow = Commander();
  void onMotorEspNow(char* cmd){ commandEspNow.motor(&motor, cmd); }
#endif


// Initialization of SimpleFOC
// Do NOT remove the delays in this function.
int SimpleFOCinit(float bus_v){

  Serial.println("DAGOR: Init SimpleFOC");
  //Motor driver initialization
  
  _delay(500);

  #ifdef ENCODER                          // If using ABI interface of magnetic sensor
    sensor.init();                        // Initialise magnetic sensor hardware
    sensor.enableInterrupts(doA, doB);    // Enable interrupts for quadrature signals
    motor.linkSensor(&sensor);            // Link sensor to motor instance  
  #else                                   // Default, SPI interface of magnetic sensor
    sensor.clock_speed = 10000000;        // Set SPI clock freq. to 10MHz, default is 1MHz
    sensor.init();                        // Initialise magnetic sensor hardware
    motor.linkSensor(&sensor);            // Link sensor to motor instance 
  #endif
  
  // driver config, power supply voltage [V]
  driver.voltage_power_supply = bus_v;
  driver.voltage_limit = driver.voltage_power_supply;
  driver.init();
  motor.linkDriver(&driver);

  #ifdef CURRENT_SENSE
    current_sense.linkDriver(&driver);
  #endif
  
  if (focModulation) motor.foc_modulation = FOCModulationType::SinePWM;
  else motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set FOC loop to be used: MotionControlType::torque, velocity, angle, velocity_openloop, angle_openloop
  if (controlType == "C0") motor.controller = MotionControlType::torque;
  else if (controlType == "C1") motor.controller = MotionControlType::velocity;
  else motor.controller = MotionControlType::angle;

  // link the current sense to the motor
  #ifdef CURRENT_SENSE
    motor.linkCurrentSense(&current_sense);
    if (trueTorque) motor.torque_controller = TorqueControlType::foc_current;
    else motor.torque_controller = TorqueControlType::voltage; 
    motor.voltage_limit = bus_v;
  #else
    motor.torque_controller = TorqueControlType::voltage; 
    motor.voltage_limit = bus_v;
    //motor.phase_resistance = phaseRes;          // Measured phase resistance [ohms]
    //motor.current_limit = maxPowersourceCurrent;
  #endif

  // Sensor aligning voltage
  motor.voltage_sensor_align = bus_v*alignStrength;
  Serial.print("MOT: Align voltage -> ");
  Serial.println(motor.voltage_sensor_align);

  // Current PI controller parameters - q_axis
  motor.PID_current_q.P = cp;
  motor.PID_current_q.I = ci;
  motor.PID_current_q.D = cd;
  motor.PID_current_q.limit = amp_limit;
  motor.PID_current_q.output_ramp = voltageRamp;
  motor.LPF_current_q.Tf = lpQDFilter;

  // Current PI controller parameters - d_axis
  motor.PID_current_d.P = cp;
  motor.PID_current_d.I = ci;
  motor.PID_current_d.D = cd;
  motor.PID_current_d.limit = amp_limit;
  motor.PID_current_d.output_ramp = voltageRamp;
  motor.LPF_current_d.Tf = lpQDFilter;

  // velocity PI controller parameters
  motor.PID_velocity.P = vp;
  motor.PID_velocity.I = vi;
  motor.PID_velocity.D = vd;
  motor.PID_velocity.output_ramp = voltageRamp;
  motor.LPF_velocity.Tf = lpVelFilter;
  motor.velocity_limit = velocity_limit;
  
  // angle P controller
  motor.P_angle.P = ap;
  motor.P_angle.I = ai;
  motor.P_angle.D = ad;
  motor.LPF_angle.Tf = lpPosFilter;       // maximal velocity of the poisition control
  
  // Position offset, used to define an absolute 0 position [rads]
  motor.sensor_offset = sensorOffset;

  // Downsampling value of the motion control loops with respect to torque loops
  motor.motion_downsample = motionDownSample; // - times (default 0 - disabled)

  motor.monitor_variables = _MON_VOLT_Q; 
  motor.monitor_downsample = 50; // downsampling, default 10

  motor.useMonitoring(Serial);      // use monitoring functionality
  motor.init();                     // initialise motor

  #ifdef CALIBRATED_SENSOR
    sensor_calibrated.voltage_calibration = bus_v*alignStrength;        // set voltage to run calibration
    sensor_calibrated.calibrate(motor);                                 // Running calibration
    //Serial.println("Calibrating Sensor Done.");
    motor.linkSensor(&sensor_calibrated);                               // Linking sensor to motor object
  #endif

  Serial.println("DAGOR: Init current sense");

  #ifdef CURRENT_SENSE
    current_sense.init();                           // current sense init hardware
    motor.linkCurrentSense(&current_sense);         // link motor and current sense
    current_sense.skip_align = true;                // skip alignment procedure
  #endif

  int initFOC_exit_code = -1;

  //if (skipCalibration && natDirection == "CW") initFOC_exit_code =  motor.initFOC(elecOffset,CW);              // start FOC skipping sensor calibration
  //else if (skipCalibration && natDirection == "CCW") initFOC_exit_code = motor.initFOC(elecOffset,CCW);        // start FOC skipping sensor calibration
  //else {
  //  initFOC_exit_code = motor.initFOC();                                                  // align sensor/ encoder and start FOC
  //}
  
  initFOC_exit_code = motor.initFOC();

  if (initFOC_exit_code == 1) {
    _delay(500);

    // define the motor id
    command.add(motorID, onMotor, " BLDC");
    if (!commandDebug) command.verbose = VerboseMode::nothing;  
  }

  #if defined(ESP_NOW) || defined(RS485)
      commandEspNow.add(motorID, onMotor, " BLDC");
      if (!commandDebug) commandEspNow.verbose = VerboseMode::nothing;
  #endif

  return initFOC_exit_code;
}

