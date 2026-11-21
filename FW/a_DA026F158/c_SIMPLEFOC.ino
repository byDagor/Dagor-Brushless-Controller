//###########################################
//               SimpleFOC
//###########################################

// SimpleFOC special parameters, do not change unless you know what you're doing
const bool focModulation = false;       // Field oriented control modulation type: true -> Sine PWM
                                        //                                         false -> Space Vector PWM
const int motionDownSample = 4;         // Downsample the motion control loops with respect to the torque control loop [amount of loops]
const float pwmFrequency = 20000;       // Power-stage PWM frequency [Hz]
const float svpwmVoltageFactor = _1_SQRT3;      // Largest undistorted phase voltage amplitude, as a fraction of the driver voltage limit, for Space Vector PWM
const float sinePwmVoltageFactor = 0.5;         // Same, for Sine PWM
const bool lockLoopToCurrentSamples = true;     // Start each loop iteration right after a pair of current samples completes: a fixed 10 kHz at 20 kHz PWM
const uint32_t cpuCyclesPerUs = 240;            // CPU clock [MHz], the loop is timed with the CPU cycle counter
const uint32_t currentPairCycles = uint32_t(2000000.0f / pwmFrequency * cpuCyclesPerUs);   // Time between two complete pairs of current samples: two PWM periods [CPU cycles]
const uint32_t currentPairGraceCycles = 3000;   // How long a late sample interrupt is waited for [CPU cycles] (12.5 us)
const uint32_t loopTimeoutCycles = 30000;       // Longest wait for the samples [CPU cycles] (125 us = 8 kHz, the minimum loop rate)
const uint8_t commandPollDivider = 32;          // The serial command interface is polled once every this many loop iterations
const uint32_t sensorCsSetupCycles = 100;       // Wait between chip select falling and the first clock edge in CPU cycles at 240 MHz (0.42 us), the AS5147 needs 350 ns
const uint32_t sensorFrameGapCycles = 120;      // Wait between the two AS5147 SPI frames in CPU cycles at 240 MHz (0.5 us), the chip needs 350 ns of CSn high time
const int sensorFrameGapUs = 1;                 // Same wait for the slow path [us]
const uint16_t as5147ReadAngleCommand = 0xFFFF; // Read of register 0x3FFF (angle with dynamic angle error compensation), including read bit and even parity bit
const uint16_t as5147DataMask = 0x3FFF;         // The angle is the lower 14 bits of the response frame
const float as5147CountsPerRev = 16384;         // 2^14 angle counts per mechanical revolution
const char motorID = 'M';               // Motor ID used for the commander interface, can be any character 
#undef   ENCODER                        // define -> ABI interface of magnetic sensor (incremental encoder) 
                                        // undef  -> SPI interface (absolute rotational position)
#undef   CALIBRATED_SENSOR              // Run sensor eccentricity calibration on start-up, NOT RECOMMENDED FOR NOW.
const float sensorOffset = 0.0;         // angular position offset, used to define a new MECHANICAL absolute 0 position on the motor's rotor [rads]

// Must stay below the CALIBRATED_SENSOR flag: tabs are concatenated alphabetically, so a tab above this one cannot see it.
#ifdef CALIBRATED_SENSOR
  #include <SimpleFOCDrivers.h>
  #include <encoders/calibrated/CalibratedSensor.h>
#endif

//#############_SIMPLEFOC INSTANCES_#################
BLDCMotor motor = BLDCMotor(pp);                                                        //BLDCMotor instance
DRV8305Driver3PWM driver = DRV8305Driver3PWM(INHC, INHB, INHA, cs, enGate, nFault);     //DRV8305 3PWM Driver instance

#ifdef CURRENT_SENSE
  LowsideCurrentSense current_sense = LowsideCurrentSense(0.002f, 80.0f, SO1, SO2);     //Current sensing instance
#endif

#ifdef ENCODER
  Encoder sensor = Encoder(32, 17, 512);       // Quadrature encoder instance
  // Interrupt routine intialisation -> channel A and B callbacks
  void doA(){sensor.handleA();}
  void doB(){sensor.handleB();}
#else
  // SimpleFOC waits 50 us between the two SPI frames of every ESP32 read and opens a full Arduino SPI transaction twice. That costs
  // more than the whole rest of the control loop, so this class reads the same register (0x3FFF) with the bus held only once and
  // the chip select driven directly.
  class DagorAS5147 : public MagneticSensorSPI {
    public:
      DagorAS5147(int chipSelect) : MagneticSensorSPI(AS5147_SPI, chipSelect), csPin(chipSelect) {}

      // The bus lock is the one the DRV8305 task takes in its transactions, so the two cannot overlap. The bus clock register is checked
      // on every read: if the DRV8305 task has set its own clock, the full Arduino transaction sets the encoder's again.
      float getSensorAngle() override;

    private:
      int csPin;
      uint32_t csMask = 0;
      uint32_t encoderClockDiv = 0;   // Bus clock register as the encoder needs it, 0 until the first read
      float lastAngle = 0;

      // The read as the Arduino SPI class does it, all inside one transaction. It also leaves the bus set for the encoder,
      // and remembers the clock register so the next reads can take the short path.
      float readWithFullTransaction();

      // Inline cycle counter: ESP.getCycleCount() is a function in flash and would pull the cache back into the IRAM read
      static inline __attribute__((always_inline)) void waitCycles(uint32_t cycles){
        uint32_t start = esp_cpu_get_cycle_count();
        while (esp_cpu_get_cycle_count() - start < cycles);
      }
  };

  float FOC_HOT DagorAS5147::getSensorAngle(){
        spi_t* bus = SPI.bus();
        spiSimpleTransaction(bus);
        if (spiGetClockDiv(bus) != encoderClockDiv){
          spiEndTransaction(bus);
          return readWithFullTransaction();
        }
        GPIO.out_w1tc = csMask;
        waitCycles(sensorCsSetupCycles);
        spiTransferShortNL(bus, as5147ReadAngleCommand);
        GPIO.out_w1ts = csMask;
        waitCycles(sensorFrameGapCycles);
        GPIO.out_w1tc = csMask;
        waitCycles(sensorCsSetupCycles);
        uint16_t response = spiTransferShortNL(bus, 0x0000);
        GPIO.out_w1ts = csMask;
        spiEndTransaction(bus);
        // A glitch on the shared bus reads as all zeros (seen only under heavy DRV8305 traffic): keep the previous angle
        if (response == 0x0000) return lastAngle;
        lastAngle = ((response & as5147DataMask) / as5147CountsPerRev) * _2PI;
        return lastAngle;
  }

  float FOC_HOT DagorAS5147::readWithFullTransaction(){
        csMask = 1UL << csPin;
        SPI.beginTransaction(SPISettings(clock_speed, MSBFIRST, spi_mode));
        encoderClockDiv = spiGetClockDiv(SPI.bus());
        digitalWrite(csPin, LOW);
        SPI.transfer16(as5147ReadAngleCommand);
        digitalWrite(csPin, HIGH);
        delayMicroseconds(sensorFrameGapUs);
        digitalWrite(csPin, LOW);
        uint16_t response = SPI.transfer16(0x0000);
        digitalWrite(csPin, HIGH);
        SPI.endTransaction();
        lastAngle = ((response & as5147DataMask) / as5147CountsPerRev) * _2PI;
        return lastAngle;
  }

  DagorAS5147 sensor = DagorAS5147(sensorCS);                             //SPI Magnetic sensor instance
#endif

#ifdef CALIBRATED_SENSOR
  CalibratedSensor sensor_calibrated = CalibratedSensor(sensor);          // instantiate the calibrated sensor object
#endif


//#############_COMMANDER INTERFACE_#################
Commander command = Commander(Serial);
Commander commandSystem = Commander(Serial);   // Only the system commands, still read after an error when the motor commands are not
void onMotor(char* cmd){ noteCommand('M', cmd); command.motor(&motor, cmd); }

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
  driver.pwm_frequency = pwmFrequency;
  driver.init();
  if (!drvConfigure()){
    Serial.println("DAGOR: DRV8305 did not accept its settings, check the SPI wiring and the supply.");
    return 0;
  }
  Serial.println("DAGOR: DRV8305 settings verified.");
  #ifdef PROFILE_FOC
    profileBusyReport("busy work after driver.init");
  #endif
  motor.linkDriver(&driver);

  #ifdef CURRENT_SENSE
    current_sense.linkDriver(&driver);
  #endif
  
  if (focModulation) motor.foc_modulation = FOCModulationType::SinePWM;
  else motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set FOC loop to be used: MotionControlType::torque, velocity, angle, angle_nocascade, velocity_openloop, angle_openloop
  motor.controller = controlType;


  // link the current sense to the motor
  #ifdef CURRENT_SENSE
    motor.linkCurrentSense(&current_sense);
    if (trueTorque) {
      motor.torque_controller = TorqueControlType::foc_current;
      motor.voltage_limit = driver.voltage_limit * (focModulation ? sinePwmVoltageFactor : svpwmVoltageFactor);
    }
    else {
      motor.torque_controller = TorqueControlType::voltage; 
      motor.voltage_limit = amp_limit*phaseRes;
    }
  #else
    motor.torque_controller = TorqueControlType::voltage; 
    motor.voltage_limit = amp_limit*phaseRes;
    //motor.phase_resistance = phaseRes;          // Measured phase resistance [ohms]
  #endif

  motor.current_limit = amp_limit;

  // Sensor aligning voltage
  motor.voltage_sensor_align = bus_v*alignStrength;
  Serial.print("MOT: Align voltage -> ");
  Serial.println(motor.voltage_sensor_align);

  // Current PI controller parameters - q_axis
  motor.PID_current_q.P = cp;
  motor.PID_current_q.I = ci;
  motor.PID_current_q.D = cd;
  motor.PID_current_q.output_ramp = voltageRamp;
  motor.LPF_current_q.Tf = lpQDFilter;

  // Current PI controller parameters - d_axis
  motor.PID_current_d.P = cp;
  motor.PID_current_d.I = ci;
  motor.PID_current_d.D = cd;
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
    if (lockLoopToCurrentSamples && current_sense.params) currentSampleIndex = &((ESP32CurrentSenseParams*)current_sense.params)->buffer_index;
    #ifdef PROFILE_FOC
      profileBusyReport("busy work after current_sense.init");
    #endif
  #endif

  int initFOC_exit_code = -1;

  #ifdef SKIP_SENSOR_CALIB
    motor.zero_electric_angle = zero_elec_angle;
    motor.sensor_direction = nat_dir;
  #endif
  
  initFOC_exit_code = motor.initFOC();

  // SimpleFOC only warns on a pole-pair mismatch; a wrong pole-pair count would commutate badly, so fail the init.
  // The check only runs when the sensor direction was found by the calibration.
  #ifndef SKIP_SENSOR_CALIB
    if (initFOC_exit_code == 1 && !motor.pp_check_result) {
      Serial.println("MOT: Pole-pair check failed, check the pp setting.");
      initFOC_exit_code = 0;
    }
  #endif

  if (initFOC_exit_code == 1) {
    // The target is 0 after boot, hold the current position instead of driving to it.
    motor.sensor->update();
    motor.target = motor.shaftAngle();
    bootTarget = motor.target;

    _delay(500);

    // define the motor id
    command.add(motorID, onMotor, " PMSM");
    command.add('C', enableActiveComp, " Active Compliance");
    command.add('H', setNewMechanicalZero, " Set new mechanical Zero");
    command.add('L', printLoopRate, " Loop rate");
    command.add('Q', onStatsCommand, " Hold statistics");
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


// ---- Motor helpers: control mode, active compliance, mechanical zero ----

bool isAngleControl(){
  return motor.controller == MotionControlType::angle || motor.controller == MotionControlType::angle_nocascade;
}

// Commander callback to enable Active Compliance
void enableActiveComp(char* cmd){
  noteCommand('C', cmd);
  if(atoi(cmd) && isAngleControl()) {
    active_comp_mode = true;
    Serial.println("Dagor: Active Compliance enabled.");
  } else {
    active_comp_mode = false;
    Serial.println("Dagor: Active Compliance disabled.");
  }
}

// Commander callback to register new mechanical zero, AKA new mechanical home
void setNewMechanicalZero(char* cmd){
  noteCommand('H', cmd);
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

// Gravity compensation mode. Returns true if it already ran motor.move() this cycle.
bool FOC_HOT activeComp(){

  #ifdef CURRENT_SENSE                     // Only works if current sense is available
    static float holdPosition = 0;         // New calculated target depending on external forces applied to the actuator's output
    static bool wasActive = false;

    if (active_comp_mode && isAngleControl()){

      if (!wasActive){
        holdPosition = motor.shaft_angle;
        wasActive = true;
      }

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
      return true;
    }

    // Normal control resumes from the last hold position instead of the pre-compliance target.
    if (wasActive){
      motor.target = holdPosition;
      wasActive = false;
    }
  #endif

  return false;
}
