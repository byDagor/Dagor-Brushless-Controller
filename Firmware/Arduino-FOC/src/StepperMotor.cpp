#include "StepperMotor.h"
#include "./communication/SimpleFOCDebug.h"


// StepperMotor(int pp)
// - pp            - pole pair number
// - R             - motor phase resistance
// - KV            - motor kv rating (rmp/v)
StepperMotor::StepperMotor(int pp, float _R, float _KV)
: FOCMotor()
{
  // number od pole pairs
  pole_pairs = pp;
  // save phase resistance number
  phase_resistance = _R;
  // save back emf constant KV = 1/K_bemf
  KV_rating = _KV;

  // torque control type is voltage by default
  // current and foc_current not supported yet
  torque_controller = TorqueControlType::voltage;
}

/**
	Link the driver which controls the motor
*/
void StepperMotor::linkDriver(StepperDriver* _driver) {
  driver = _driver;
}

// init hardware pins
void StepperMotor::init() {
  if (!driver || !driver->initialized) {
    motor_status = FOCMotorStatus::motor_init_failed;
    SIMPLEFOC_DEBUG("MOT: Init not possible, driver not initialized");
    return;
  }
  motor_status = FOCMotorStatus::motor_initializing;
  SIMPLEFOC_DEBUG("MOT: Init");

  // sanity check for the voltage limit configuration
  if(voltage_limit > driver->voltage_limit) voltage_limit =  driver->voltage_limit;
  // constrain voltage for sensor alignment
  if(voltage_sensor_align > voltage_limit) voltage_sensor_align = voltage_limit;

  // update the controller limits
  if(_isset(phase_resistance)){
    // velocity control loop controls current
    PID_velocity.limit = current_limit;
  }else{
    PID_velocity.limit = voltage_limit;
  }
  P_angle.limit = velocity_limit;

  _delay(500);
  // enable motor
  SIMPLEFOC_DEBUG("MOT: Enable driver.");
  enable();
  _delay(500);

  motor_status = FOCMotorStatus::motor_uncalibrated;
}


// disable motor driver
void StepperMotor::disable()
{
  // set zero to PWM
  driver->setPwm(0, 0);
  // disable driver
  driver->disable();
  // motor status update
  enabled = 0;
}
// enable motor driver
void StepperMotor::enable()
{
  // disable enable
  driver->enable();
  // set zero to PWM
  driver->setPwm(0, 0);
  // motor status update
  enabled = 1;
}


/**
  FOC functions
*/
// FOC initialization function
int  StepperMotor::initFOC( float zero_electric_offset, Direction _sensor_direction ) {
  int exit_flag = 1;
  
  motor_status = FOCMotorStatus::motor_calibrating;

  // align motor if necessary
  // alignment necessary for encoders!
  if(_isset(zero_electric_offset)){
    // abosolute zero offset provided - no need to align
    zero_electric_angle = zero_electric_offset;
    // set the sensor direction - default CW
    sensor_direction = _sensor_direction;
  }

  // sensor and motor alignment - can be skipped
  // by setting motor.sensor_direction and motor.zero_electric_angle
  _delay(500);
  if(sensor){
    exit_flag *= alignSensor();
    // added the shaft_angle update
    sensor->update();
    shaft_angle = sensor->getAngle();
  }else SIMPLEFOC_DEBUG("MOT: No sensor.");

  if(exit_flag){
    SIMPLEFOC_DEBUG("MOT: Ready.");
    motor_status = FOCMotorStatus::motor_ready;
  }else{
    SIMPLEFOC_DEBUG("MOT: Init FOC failed.");
    motor_status = FOCMotorStatus::motor_calib_failed;
    disable();
  }

  return exit_flag;
}

// Encoder alignment to electrical 0 angle
int StepperMotor::alignSensor() {
  int exit_flag = 1; //success
  SIMPLEFOC_DEBUG("MOT: Align sensor.");

  // if unknown natural direction
  if(!_isset(sensor_direction)){
    // check if sensor needs zero search
    if(sensor->needsSearch()) exit_flag = absoluteZeroSearch();
    // stop init if not found index
    if(!exit_flag) return exit_flag;

    // find natural direction
    // move one electrical revolution forward
    for (int i = 0; i <=500; i++ ) {
      float angle = _3PI_2 + _2PI * i / 500.0f;
      setPhaseVoltage(voltage_sensor_align, 0,  angle);
      _delay(2);
    }
    // take and angle in the middle
    sensor->update();
    float mid_angle = sensor->getAngle();
    // move one electrical revolution backwards
    for (int i = 500; i >=0; i-- ) {
      float angle = _3PI_2 + _2PI * i / 500.0f ;
      setPhaseVoltage(voltage_sensor_align, 0,  angle);
      _delay(2);
    }
    sensor->update();
    float end_angle = sensor->getAngle();
    setPhaseVoltage(0, 0, 0);
    _delay(200);
    // determine the direction the sensor moved
    if (mid_angle == end_angle) {
      SIMPLEFOC_DEBUG("MOT: Failed to notice movement");
      return 0; // failed calibration
    } else if (mid_angle < end_angle) {
      SIMPLEFOC_DEBUG("MOT: sensor_direction==CCW");
      sensor_direction = Direction::CCW;
    } else{
      SIMPLEFOC_DEBUG("MOT: sensor_direction==CW");
      sensor_direction = Direction::CW;
    }
    // check pole pair number
    float moved =  fabs(mid_angle - end_angle);
    if( fabs(moved*pole_pairs - _2PI) > 0.5f ) { // 0.5f is arbitrary number it can be lower or higher!
      SIMPLEFOC_DEBUG("MOT: PP check: fail - estimated pp: ", _2PI/moved);
    } else 
      SIMPLEFOC_DEBUG("MOT: PP check: OK!");

  }else SIMPLEFOC_DEBUG("MOT: Skip dir calib.");

  // zero electric angle not known
  if(!_isset(zero_electric_angle)){
    // align the electrical phases of the motor and sensor
    // set angle -90(270 = 3PI/2) degrees
    setPhaseVoltage(voltage_sensor_align, 0,  _3PI_2);
    _delay(700);
    // read the sensor
    sensor->update();
    // get the current zero electric angle
    zero_electric_angle = 0;
    zero_electric_angle = electricalAngle();
    _delay(20);
    if(monitor_port){
      SIMPLEFOC_DEBUG("MOT: Zero elec. angle: ", zero_electric_angle);
    }
    // stop everything
    setPhaseVoltage(0, 0, 0);
    _delay(200);
  }else SIMPLEFOC_DEBUG("MOT: Skip offset calib.");
  return exit_flag;
}

// Encoder alignment the absolute zero angle
// - to the index
int StepperMotor::absoluteZeroSearch() {

  SIMPLEFOC_DEBUG("MOT: Index search...");
  // search the absolute zero with small velocity
  float limit_vel = velocity_limit;
  float limit_volt = voltage_limit;
  velocity_limit = velocity_index_search;
  voltage_limit = voltage_sensor_align;
  shaft_angle = 0;
  while(sensor->needsSearch() && shaft_angle < _2PI){
    angleOpenloop(1.5f*_2PI);
    // call important for some sensors not to loose count
    // not needed for the search
    sensor->update();
  }
  // disable motor
  setPhaseVoltage(0, 0, 0);
  // reinit the limits
  velocity_limit = limit_vel;
  voltage_limit = limit_volt;
  // check if the zero found
  if(monitor_port){
    if(sensor->needsSearch()) SIMPLEFOC_DEBUG("MOT: Error: Not found!");
    else SIMPLEFOC_DEBUG("MOT: Success!");
  }
  return !sensor->needsSearch();
}


// Iterative function looping FOC algorithm, setting Uq on the Motor
// The faster it can be run the better
void StepperMotor::loopFOC() {
  
  // update sensor - do this even in open-loop mode, as user may be switching between modes and we could lose track
  //                 of full rotations otherwise.
  if (sensor) sensor->update();

  // if open-loop do nothing
  if( controller==MotionControlType::angle_openloop || controller==MotionControlType::velocity_openloop ) return;
  // shaft angle
  shaft_angle = shaftAngle();

  // if disabled do nothing
  if(!enabled) return;

  // Needs the update() to be called first
  // This function will not have numerical issues because it uses Sensor::getMechanicalAngle() 
  // which is in range 0-2PI
  electrical_angle = electricalAngle();

  // set the phase voltage - FOC heart function :)
  setPhaseVoltage(voltage.q, voltage.d, electrical_angle);
}

// Iterative function running outer loop of the FOC algorithm
// Behavior of this function is determined by the motor.controller variable
// It runs either angle, velocity or voltage loop
// - needs to be called iteratively it is asynchronous function
// - if target is not set it uses motor.target value
void StepperMotor::move(float new_target) {

  // downsampling (optional)
  if(motion_cnt++ < motion_downsample) return;
  motion_cnt = 0;

  // shaft angle/velocity need the update() to be called first
  // get shaft angle
  // TODO sensor precision: the shaft_angle actually stores the complete position, including full rotations, as a float
  //                        For this reason it is NOT precise when the angles become large.
  //                        Additionally, the way LPF works on angle is a precision issue, and the angle-LPF is a problem
  //                        when switching to a 2-component representation.
  if( controller!=MotionControlType::angle_openloop && controller!=MotionControlType::velocity_openloop ) 
    shaft_angle = shaftAngle(); // read value even if motor is disabled to keep the monitoring updated but not in openloop mode
  // get angular velocity 
  shaft_velocity = shaftVelocity(); // read value even if motor is disabled to keep the monitoring updated

  // if disabled do nothing
  if(!enabled) return;

  // set internal target variable
  if(_isset(new_target) ) target = new_target;

  // calculate the back-emf voltage if KV_rating available U_bemf = vel*(1/KV)
  if (_isset(KV_rating)) voltage_bemf = shaft_velocity/KV_rating/_RPM_TO_RADS;
  // estimate the motor current if phase reistance available and current_sense not available
  if(!current_sense && _isset(phase_resistance)) current.q = (voltage.q - voltage_bemf)/phase_resistance;

  // choose control loop
  switch (controller) {
    case MotionControlType::torque:
      if(!_isset(phase_resistance))  voltage.q = target; // if voltage torque control
      else  voltage.q =  target*phase_resistance + voltage_bemf;
      voltage.q = _constrain(voltage.q, -voltage_limit, voltage_limit);
      voltage.d = 0;
      break;
    case MotionControlType::angle:
      // angle set point
      shaft_angle_sp = target;
      // calculate velocity set point
      shaft_velocity_sp = P_angle( shaft_angle_sp - shaft_angle );
      // calculate the torque command
      current_sp = PID_velocity(shaft_velocity_sp - shaft_velocity); // if voltage torque control
      // if torque controlled through voltage
      // use voltage if phase-resistance not provided
      if(!_isset(phase_resistance))  voltage.q = current_sp;
      else  voltage.q =  _constrain( current_sp*phase_resistance + voltage_bemf , -voltage_limit, voltage_limit);
        voltage.d = 0;
      break;
    case MotionControlType::velocity:
      // velocity set point
      shaft_velocity_sp = target;
      // calculate the torque command
      current_sp = PID_velocity(shaft_velocity_sp - shaft_velocity); // if current/foc_current torque control
      // if torque controlled through voltage control
      // use voltage if phase-resistance not provided
      if(!_isset(phase_resistance))  voltage.q = current_sp;
      else  voltage.q = _constrain( current_sp*phase_resistance + voltage_bemf , -voltage_limit, voltage_limit);
      voltage.d = 0;
      break;
    case MotionControlType::velocity_openloop:
      // velocity control in open loop
      shaft_velocity_sp = target;
      voltage.q = velocityOpenloop(shaft_velocity_sp); // returns the voltage that is set to the motor
      voltage.d = 0;
      break;
    case MotionControlType::angle_openloop:
      // angle control in open loop
      shaft_angle_sp = target;
      voltage.q = angleOpenloop(shaft_angle_sp); // returns the voltage that is set to the motor
      voltage.d = 0;
      break;
  }
}


// Method using FOC to set Uq and Ud to the motor at the optimal angle
// Function implementing Sine PWM algorithms
// - space vector not implemented yet
//
// Function using sine approximation
// regular sin + cos ~300us    (no memory usaage)
// approx  _sin + _cos ~110us  (400Byte ~ 20% of memory)
void StepperMotor::setPhaseVoltage(float Uq, float Ud, float angle_el) {
  // Sinusoidal PWM modulation
  // Inverse Park transformation

  // angle normalization in between 0 and 2pi
  // only necessary if using _sin and _cos - approximation functions
  angle_el = _normalizeAngle(angle_el);
  float _ca = _cos(angle_el);
  float _sa = _sin(angle_el);
  // Inverse park transform
  Ualpha =  _ca * Ud - _sa * Uq;  // -sin(angle) * Uq;
  Ubeta =  _sa * Ud + _ca * Uq;    //  cos(angle) * Uq;

  // set the voltages in hardware
  driver->setPwm(Ualpha, Ubeta);
}

// Function (iterative) generating open loop movement for target velocity
// - target_velocity - rad/s
// it uses voltage_limit variable
float StepperMotor::velocityOpenloop(float target_velocity){
  // get current timestamp
  unsigned long now_us = _micros();
  // calculate the sample time from last call
  float Ts = (now_us - open_loop_timestamp) * 1e-6f;
  // quick fix for strange cases (micros overflow + timestamp not defined)
  if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

  // calculate the necessary angle to achieve target velocity
  shaft_angle = _normalizeAngle(shaft_angle + target_velocity*Ts);
  // for display purposes
  shaft_velocity = target_velocity;

  // use voltage limit or current limit
  float Uq = voltage_limit;
  if(_isset(phase_resistance)) 
    Uq = _constrain(current_limit*phase_resistance + voltage_bemf,-voltage_limit, voltage_limit);

  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  setPhaseVoltage(Uq,  0, _electricalAngle(shaft_angle, pole_pairs));

  // save timestamp for next call
  open_loop_timestamp = now_us;

  return Uq;
}

// Function (iterative) generating open loop movement towards the target angle
// - target_angle - rad
// it uses voltage_limit and velocity_limit variables
float StepperMotor::angleOpenloop(float target_angle){
  // get current timestamp
  unsigned long now_us = _micros();
  // calculate the sample time from last call
  float Ts = (now_us - open_loop_timestamp) * 1e-6f;
  // quick fix for strange cases (micros overflow + timestamp not defined)
  if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

  // calculate the necessary angle to move from current position towards target angle
  // with maximal velocity (velocity_limit)
  if(abs( target_angle - shaft_angle ) > abs(velocity_limit*Ts)){
    shaft_angle += _sign(target_angle - shaft_angle) * abs( velocity_limit )*Ts;
    shaft_velocity = velocity_limit;
  }else{
    shaft_angle = target_angle;
    shaft_velocity = 0;
  }

  // use voltage limit or current limit
  float Uq = voltage_limit;
  if(_isset(phase_resistance)) 
    Uq = _constrain(current_limit*phase_resistance + voltage_bemf,-voltage_limit, voltage_limit);
  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  setPhaseVoltage(Uq,  0, _electricalAngle((shaft_angle), pole_pairs));

  // save timestamp for next call
  open_loop_timestamp = now_us;

  return Uq;
}