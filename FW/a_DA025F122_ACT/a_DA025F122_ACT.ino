
//####################################################################################
//      Dagor Controller Alpha 2.5 Firmware Version 1.2.2 for the DAGR Actuator
//####################################################################################

//#undef   QM5006          // define if this motor is used in the actuator
//#define    BDUAV           // define if this motor is used in the actuator

#define LEG_ID 0        // LEG ID number:
                        // Front Right LEG -> 0
                        // Front Left LEG -> 1
                        // Back Right LEG -> 2
                        // Back Left LEG -> 3
#define KNEE             // define if the actuator is knee, hip or shoulder
#undef HIP
#undef SHOULDER

// The firmware is separated into 7 tabs (a - g), make sure all files are stored in the folder 
// named a_DA025F122_ACT.

// This tab cointains the parameters to customize the controller. Learn more at https://docs.dagor.dev/

//############################################################################################################

// Below are the variables dependent on the user's set-up. 
// If the phase resistance is unknown and can't be meausered with a multimeter assume a low resistance.

//#######_USER VARIABLES_#######
//const byte pp = 7;                  //BLDC motor number of pole pairs
//const float phaseRes = 0.560;                  //Phase winding resistance [ohms]
const float sourceVoltage = 12;                //Voltage of your power source [Volts]
//const float maxPowersourceCurrent = 3.6;       //Very rough approximation of max current draw from the power source [Amps]
                                               //This is not the phase current through the motor.
const String controlType = "C2";               //control type: C0 -> torque (voltage)
                                                      // C1 -> velocity
                                                      // C2 -> angular position

// Below are the control loops parameters, to obtain the desired response out of the controller 
// they need to be tuned. These parameters can be tuned via the "Commander" interface, which is 
// based on ASCII character command ids. There are two types of commands, a fetch and a set.
// A fetch command will have the following structure:
//                               motor ID ->  MLC <- Current
//                                             ^
//                                             |
//                                           Limits
//
// The example above will show the current limit set to the controller 
//
// A set command will have the following structure:
//                               motor ID ->  MVI3.5 <- value
//                                             ^^
//                                            /  \
//                          Velocity controller  Integral gain
//
// The example above will set the Integral gain value of the velocity control loop to 3.5
//
// Changing the set-point (target) looks like this:
//                               motor ID ->  M6.3  <- value
//
// The example above will rotate the motor's rotor one full turn, given angular position control. 
//
// These commands can be sent through the Serial Monitor, make sure to set the baud rate to 115200 and 
// "Newline". To the right of the variables below there are the characters that would be used to fetch
// or set each individual parameter. There is a cheat-sheet of the commands at the bottom of this tab.
// Make sure to write the final values of the parameters after tuning to update the firmware and 
// upload it again.

//#######_CONTROLLER PARAMETERS_#######
//const float vp = 0.1;               //Velocity control loop PROPORTIONAL gain value   - VP
//const float vi = 1;                 //Velocity control loop INTEGRAL gain value       - VI
//const float vd = 0;                 //Velocity control loop DERIVATIVE gain value     - VD
//const float lpVelFilter = 0.000;    //Velocity measurement low-pass filter            - VF
//#ifdef QM5006
#ifdef KNEE
  const float phaseRes = 0.560;
  const float maxPowersourceCurrent = 3.4;
  const byte pp = 14;                 //BLDC motor number of pole pairs
  const float alignStrength = 1.30;   // Percentage of power used to calibrate the sensor on start-up
  
  const float cp = 0.05;              //QD current loops PROPORTONAL gain value         - MQP & MDP
  const float ci = 10;                //QD current loops INTEGRAL gain value            - MQI & MDI
  const float cd = 0;                 //QD current loops DERIVATIVE gain value          - MQD & MDD
  const float lpQDFilter = 0.000;     //QD current loops measurement low-pass filter    - QF & DF
  const float ap = 10;                //Position control loop PROPORTIONAL gain value   - AP
  const float ai = 0;                 //Position control loop INTEGRAL gain value       - AI
  const float ad = 0.50;              //Position control loop DERIVATIVE gain value     - AD
  const float lpPosFilter = 0.000;    //Position measurment low-pass filter             - AF
  const float voltageRamp = 1000;     //Change in voltage allowed [Volts per sec]       - VR
  const float velocityLimit = 2000;   //Velocity limit [rpm]                            - LV
  float kgc = 0.004;  
#endif

//#ifdef BDUAV
#ifndef KNEE
  const float phaseRes = 0.560;
  const float maxPowersourceCurrent = 3.6;
  const byte pp = 7;                  //BLDC motor number of pole pairs
  const float alignStrength = 1.30;   // Percentage of power used to calibrate the sensor on start-up
  
  const float cp = 0.05;              //QD current loops PROPORTONAL gain value         - MQP & MDP
  const float ci = 10;                //QD current loops INTEGRAL gain value            - MQI & MDI
  const float cd = 0;                 //QD current loops DERIVATIVE gain value          - MQD & MDD
  const float lpQDFilter = 0.000;     //QD current loops measurement low-pass filter    - QF & DF
  const float ap = 10;                //Position control loop PROPORTIONAL gain value   - AP
  const float ai = 0;                 //Position control loop INTEGRAL gain value       - AI
  const float ad = 0.75;              //Position control loop DERIVATIVE gain value     - AD
  const float lpPosFilter = 0.000;    //Position measurment low-pass filter             - AF
  const float voltageRamp = 350;      //Change in voltage allowed [Volts per sec]       - VR
  const float velocityLimit = 2000;   //Velocity limit [rpm]                            - LV
  float kgc = 0.004;  
#endif

// Below are extra parameters that can be further configured for the desired application or project,
// if the application is unknown the default values will work. It is recommended to change these 
// parameters AFTER feeling comfortable using the controller.

//########_EXTRA CONFIGURATON_##########
const byte maxTemp = 80;            // Maximum operating temperature of the power-stage [°C]
const float overTempTime = 3.5;     // Time in an over-temperature senario to disable the controller [seconds]
const float sensorOffset = 0.0;     // Position offset, used to define a new absolute 0 position on the motor's rotor [rads]
const int motionDownSample = 5;     // Downsample the motion control loops with respect to the torque control loop [amount of loops]
const int callerFixedFreq = 5;      // Frequency of the fixed rate function caller in void loop [hertz]
//const float alignStrength = 1.20;   // Percentage of power used to calibrate the sensor on start-up
char motorID = 'M';                 // Motor ID used for the commander interface, can be any character 
                                    // (a unique ID is useful for multi-board proyects)
bool trueTorque = true;             // True torque mode, current control or voltage control mode.
bool commandDebug = false;           // Enable/ disable commander serial print 
bool focModulation = true;          // Field oriented control modulation type: true -> Sine PWM
                                                                           // false -> Space Vector PWM
bool skipCalibration = false;       // Skip the calibration on start-up
                                    // electric angle offset and natural direction printed on start-up
const float elecOffset = 6.05;      // Printed as: "MOT: Zero elec. angle: X.XX"
String natDirection = "CW";         // Can be either CW or CCW
#undef   MONITORING                 // define if using monitoring (usb to ttl connected to board)
#undef   ENCODER                    // define -> ABI interface of magnetic sensor (incremental encoder) 
                                    // undef  -> SPI interface (absolute rotational position)

//#######_ESP-NOW_###########
#define ESP_NOW                                                            // define -> enable ESP_NOW
//const uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0x51, 0xED, 0xA4};   // Master Controller MAC Address
const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//#######_ACTIVE COMPLIANCE_#########
bool gravityCompMode = true;
int currentQthreshold = 5;      //minimum current that defines a force applied on the actuator's output [A]
float positionThreshold = 0.08; 
//float kgc = 0.004;                //Proportional gain of the gravity compensation controller

//#######_LIST OF COMMANDS_#######

/*
    V - Velocity PID controller
      P - proportional gain
      I - integral gain
      D - derivative gain
    A - Angle PID controller
      P - proportional gain
      I - integral gain
      D - derivative gain
    L - Limits
      C - Current
      U - Voltage
      V - Velocity
    C - Motion control type config
      D - downsample motion loop
      0 - torque
      1 - velocity
      2 - angle
    T - Torque control type
      0 - voltage
      1 - dc_current
      2 - foc_current
    E - Motor status (enable/disable)
      0 - disable
      1 - enable
    R - Motor phase resistance
    S - Sensor offsets
      M - sensor offset
      E - sensor electrical zero
    M - Monitoring control
      D - downsample monitoring
      C - clear monitor
      S - set monitoring variables
      G - get variable value
    ’’ - Target get/set
*/

// Visit https://docs.simplefoc.com/commander_interface to learn more about the Commander Interface.
