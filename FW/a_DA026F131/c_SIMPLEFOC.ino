//###########################################
//               SimpleFOC
//###########################################

// SimpleFOC special parameters, do not change unless you know what you're doing
const bool focModulation = false;       // Field oriented control modulation type: true -> Sine PWM
                                        //                                         false -> Space Vector PWM
const int motionDownSample = 4;         // Downsample the motion control loops with respect to the torque control loop [amount of loops]
const char motorID = 'M';               // Motor ID used for the commander interface, can be any character 
#undef   ENCODER                        // define -> ABI interface of magnetic sensor (incremental encoder) 
                                        // undef  -> SPI interface (absolute rotational position)
#undef   CALIBRATED_SENSOR              // Run sensor eccentricity calibration on start-up, NOT RECOMMENDED FOR NOW.
const float sensorOffset = 0.0;         // angular position offset, used to define a new MECHANICAL absolute 0 position on the motor's rotor [rads]

//#############_SIMPLEFOC INSTANCES_#################
BLDCMotor motor = BLDCMotor(pp);                                                        //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(INHC, INHB, INHA);                               //3PWM Driver instance

#ifdef CURRENT_SENSE
  LowsideCurrentSense current_sense = LowsideCurrentSense(0.002f, 80.0f, SO1, SO2);     //Current sensing instance
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
  Commander commandExt = Commander();
#endif


// Initialization of SimpleFOC, do NOT remove the delays in this function.
int SimpleFOCinit(float bus_v){
  
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
  // driver.voltage_limit = driver.voltage_power_supply;
  driver.init();
  motor.linkDriver(&driver);

  #ifdef CURRENT_SENSE
    current_sense.linkDriver(&driver);
  #endif
  
  if (focModulation) motor.foc_modulation = FOCModulationType::SinePWM;
  else motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set FOC loop to be used: MotionControlType::torque, velocity, angle, velocity_openloop, angle_openloop
  motor.controller = controlType;


  // link the current sense to the motor
  #ifdef CURRENT_SENSE
    motor.linkCurrentSense(&current_sense);
    if (trueTorque) {
      motor.torque_controller = TorqueControlType::foc_current;
      motor.voltage_limit = bus_v;
    }
    else {
      motor.torque_controller = TorqueControlType::voltage; 
      motor.voltage_limit = amp_limit*phaseRes;
    }
  #else
    motor.torque_controller = TorqueControlType::voltage; 
    motor.voltage_limit = amp_limit*phaseRes;
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

  // velocity PI controller parameterstorque
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
  
  // Position offset, used to define a MECHANICAL absolute 0 position [rads]
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

  #ifdef SKIP_SENSOR_CALIB
    motor.zero_electric_angle = zero_elec_angle;
    motor.sensor_direction = nat_dir;
  #endif
  
  initFOC_exit_code = motor.initFOC();

  if (initFOC_exit_code == 1) {
    _delay(500);

    // define the motor id
    command.add(motorID, onMotor, " PMSM");
    command.add('C', enableActiveComp, " Active Compliance");
    command.add('H', setNewMechanicalZero, " Set new mechanical Zero");
    //#ifndef DEBUG_ADAPTER
      if (!ext_command_debug) command.verbose = VerboseMode::nothing;  
    //#endif
  }

  #if defined(ESP_NOW) || defined(RS485)
    commandExt.add(motorID, onMotor, " PMSM");
    commandExt.add('C', enableActiveComp, " Active Compliance");
    commandExt.add('H', setNewMechanicalZero, " Set new mechanical Zero");
    if (!ext_command_debug) commandExt.verbose = VerboseMode::nothing;
  #endif

  return initFOC_exit_code;
}

