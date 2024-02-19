//####################################################################################
//               Dagor Controller Alpha 2.6 Firmware Version 1.3.0
//####################################################################################

// The firmware is separated into 7 tabs (a - g), make sure all files are stored in the folder with the same name as tab a_DAXXXFXXX.
// This tab cointains the parameters to customize the controller. Learn more at https://docs.dagor.dev/

//TODO:
//      stall detection
//      RS-485-based protocol
//      Position control runaway protection
//      General runaway protection

//#######_USER SET-UP PARAMETERS_#######
#define ACT_ID 100                        // Actuator ID, used for ESP-NOW or RS-485 comms
const byte pp = 7;                        // BLDC motor number of pole pairs
const float phaseRes = 0.25 ;             // Phase winding resistance [ohms]
const float sourceVoltage = 11.1;         // Voltage of your power source [Volts]
const float amp_limit = 2.5;              // IQ current limit [amps]                              
const float alignStrength = 0.05;         // Percentage of available voltage used to calibrate the sensor on start-up
const String controlType = "C0";          // control type: C0 -> torque (voltage)
                                          //               C1 -> velocity
                                          //               C2 -> angular position


// Below are the control loops start-up parameters, to obtain the desired response out of the  
// controller they need to be tuned. These parameters can be tuned via the SimpleFOC's "Commander"  
// interface, which is based on ASCII character command ids. There are two types of commands, a 
// fetch and a set. A fetch command will have the following structure:
//                               motor ID ->  MLC <- Current
//                                             ^
//                                             |
//                                           Limits
//
// These commands can be sent through the Serial Monitor, make sure to set the baud rate to 115200 and 
// "Newline". To the right of the variables below there are the characters that would be used to fetch
// or set each individual parameter. There is a cheat-sheet of the commands at the bottom of this tab.
// Make sure to write the final values of the parameters after tuning to update the firmware and 
// upload it again.

//#######_START-UP CONTROLLER PARAMETERS_#######                                         //  Commander IDs:
const float cp = 0.025 ;                  // QD current loops PROPORTONAL gain value           - MQP & MDP
const float ci = 50.0;                   // QD current loops INTEGRAL gain value              - MQI & MDI
const float cd = 0.0;                     // QD current loops DERIVATIVE gain value            - MQD & MDD
const float lpQDFilter = 0.001;           // QD current loops measurement low-pass filter      - QF & DF
const float vp = 0.1;                     // Velocity control loop PROPORTIONAL gain value     - VP
const float vi = 1;                       // Velocity control loop INTEGRAL gain value         - VI
const float vd = 0;                       // Velocity control loop DERIVATIVE gain value       - VD
const float lpVelFilter = 0.000;          // Velocity measurement low-pass filter              - VF
const float velocity_limit = 2.5;         // Velocity limit [rpm]                              - LV
const float ap = 5.0;                     // Position control loop PROPORTIONAL gain value     - AP
const float ai = 0;                       // Position control loop INTEGRAL gain value         - AI
const float ad = 0.25;                    // Position control loop DERIVATIVE gain value       - AD
const float lpPosFilter = 0.000;          // Position measurment low-pass filter               - AF
const float voltageRamp = 5000;           // Change in voltage allowed [Volts per sec]         - VR


//########_ADVANCED PARAMETERS_##########
#define CURRENT_SENSE                 // Define if using current sense, difference between current control or voltage control mode.
bool trueTorque = true;               // Even if using current sense, mode can be voltage torque mode if set to false
bool focModulation = true;            // Field oriented control modulation type: true -> Sine PWM
                                                                             // false -> Space Vector PWM
#define MONITOR_TEMP                   // define -> monitor and evaluate controllers temperature
const bool print_temp = true;         // Monitor temperature through serial terminal
const int maxTemp = 75;               // Maximum operating temperature of the power-stage [°C]
const float overTempTime = 1.0;       // Time to elapse in an over-temperature state to disable the controller [seconds]
#define MONITOR_BUS_VOLTAGE
const float sensorOffset = 0.0;       // Position offset, used to define a new absolute 0 position on the motor's rotor [rads]
const int motionDownSample = 4;       // Downsample the motion control loops with respect to the torque control loop [amount of loops]
const int callerFixedFreq = 4;        // Frequency of the fixed rate function caller in void loop [hertz]
char motorID = 'M';                   // Motor ID used for the commander interface, can be any character 
                                      // (a unique ID is useful for multi-board proyects)
bool commandDebug = true;             // Enable/ disable commander serial print for commander 
bool skipCalibration = false;         // Skip the calibration on start-up
                                      // electric angle offset and natural direction printed on start-up
const float elecOffset = 0.00;        // Printed as: "MOT: Zero elec. angle: X.XX"
String natDirection = "CW";           // Can be either CW or CCW
#undef   CALIBRATED_SENSOR            // Run sensor eccentricity calibration on start-up
#define  MONITORING                   // define if using monitoring (usb to ttl connected to board)
#undef   ENCODER                      // define -> ABI interface of magnetic sensor (incremental encoder) 
                                      // undef  -> SPI interface (absolute rotational position)

//#######_WIRED/WIRELESS COMMUNICATION_###########
#undef ESP_NOW                        // define -> enable ESP_NOW
#undef RS485                          // define -> enable RS-485

//#######_ADMITTANCE MODE_#########
bool gravityCompMode = false;         // enable compliance mode at start-up
const float kgc = 0.01;               // Proportional gain of the gravity compensation controller
int currentQthreshold = 0.02;         // minimum current that defines a force applied on the actuator's output [A]
float positionThreshold = 0.014;      // minimum angular position to move holding position


//#######_LIST OF COMMANDER IDs_#######
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
