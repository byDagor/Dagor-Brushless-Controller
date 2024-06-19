//####################################################################################
//                         Dagor Controller Alpha Firmware
//                   Compatible with Alpha versions 2.4 and above
//####################################################################################

#include <SimpleFOC.h>
#define FW_VERSION "1.3.0"

// The firmware is separated into 7 tabs (a - g), make sure all files are stored in the folder with the same name as tab a_DAXXXFXXX.
// This tab cointains the parameters to customize the controller. Learn more at https://docs.dagor.dev/

// Refactoring to use both cores leveraging freeRTOS
//TODO:
//    Stall detection algorithm
//    Finish RS-485-based protocol, directional comms working
//    Virtual Walls algo
//    Detect DRV warnings vs. errors
//    Make nicer out of bounds behavior
//    Finish implementing state machine
//    Change "H" homing command to accept home as arbitrary number or current position

//#######_USER SET-UP PARAMETERS_#######
#define ACT_ID 100                                              // Actuator ID, used for ESP-NOW or RS-485 comms
const int pp = 7;                                               // BLDC motor number of pole pairs
const float phaseRes = 0.200;                                   // Phase winding resistance [ohms]
const float amp_limit = 2.5;                                    // IQ current limit [amps] - requires trueTorque mode                           
const float alignStrength = 0.05;                               // Percentage of available bus voltage used to calibrate the sensor on start-up
MotionControlType controlType = MotionControlType::angle;       // control types: angle, velocity, torque


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
const float cp = 0.015;                   // QD current loops PROPORTONAL gain value           - MQP & MDP
const float ci = 40.0;                    // QD current loops INTEGRAL gain value              - MQI & MDI
const float cd = 0.0;                     // QD current loops DERIVATIVE gain value            - MQD & MDD
const float lpQDFilter = 0.001;           // QD current loops measurement low-pass filter      - QF & DF

const float vp = 0.1;                     // Velocity control loop PROPORTIONAL gain value     - VP
const float vi = 1.0;                     // Velocity control loop INTEGRAL gain value         - VI
const float vd = 0.0;                     // Velocity control loop DERIVATIVE gain value       - VD
const float lpVelFilter = 0.000;          // Velocity measurement low-pass filter              - VF
const float velocity_limit = 2.5;         // Velocity limit [rpm]                              - LV

const float ap = 5.0;                     // Position control loop PROPORTIONAL gain value     - AP
const float ai = 0.0;                     // Position control loop INTEGRAL gain value         - AI
const float ad = 0.30;                    // Position control loop DERIVATIVE gain value       - AD
const float lpPosFilter = 0.000;          // Position measurment low-pass filter               - AF
const float voltageRamp = 2000;           // Change in voltage allowed [Volts per sec]         - VR


//########_ADVANCED PARAMETERS_##########
#define CURRENT_SENSE                     // Define if using current sense, difference between current control or voltage control mode.  
const bool trueTorque = true;             // Even if using current sense, torque mode can be voltage torque mode if set to false
const bool print_currents = false;        // Monitor currents through the serial terminal 
const bool print_dq_currents = false;     // true-> print DQ currents, false -> print phase currents

#define MONITOR_TEMP                      // define -> monitor and evaluate controllers temperature, parameters below ignored if undef
const bool print_temp = false;            // Monitor temperature through the serial terminal
const int maxTemp = 75;                   // Maximum operating temperature of the power-stage [°C]
const float overTempTime = 1.0;           // Time to elapse in an over-temperature state to disable the controller [seconds]

#undef MONITOR_BUS_VOLTAGE                // Monitor bus voltage, automatically adjust to the lowering battery voltage
const bool print_bus_voltage = false;     // Monitor the bus voltage through the serial terminal
const float voltageOverride = 11.1;       // Voltage of your power source [Volts], overrides source voltage if MONITOR_BUS_VOLTAGE is undefined

#define MONITOR_ROTOR                     // Monitor rotor position and velocity, parameters below ignored if undef
const bool print_rotor_data = false;      // Monitor roto's position and velocity (respectively) through the serial terminal
const float max_rotor_position = 10;      // Max position [rad] the rotor is allowed to spin to; above this value, motor will freewheel 
const float min_rotor_position = -150;    // Min position [rad] the rotor is allowed to spin to; below this value, motor will freewheel 

#undef SKIP_SENSOR_CALIB                  // Skip the position sensor calibration on start-up
                                          // electric angle offset and natural direction printed on start-up
const float zero_elec_angle = 0.00;       // Printed as: "MOT: Zero elec. angle: X.XX"
Direction nat_dir = Direction::CW;        // Can be either CW or CCW

#define DEBUG_ADAPTER                     // define if using USB programming/debug adapter (usb to ttl connected to board)
                                          // Recommended to disable when adapter will not be connected to computer

const bool print_voltages = false;        // Monitor voltages through the serial terminal
const bool print_dq_voltages = false;     // If print_voltage is set to true, then true-> print DQ voltages, false -> print phase voltages
const bool print_foc_freq = false;        // Monitor FOC current loop bandwidth [Hz]


//#######_WIRED/WIRELESS COMMUNICATION_###########
#undef ESP_NOW                            // define -> enable ESP_NOW
#define RS485                             // define -> enable RS-485
const bool ext_command_debug = false;     // Enable/ disable monitoring of inputs from external comms through the USB adapter (serial terminal)
                                          // Recommended to disable when adapter will not be connected to computer

//#######_ADMITTANCE MODE_#########
bool active_comp_mode = false;            // Enable active compliance mode at start-up
const float kgc = 0.01;                   // Proportional gain of the gravity compensation controller
const int currentQthreshold = 0.02;       // Minimum current that defines a force applied on the actuator's output [A]
const float positionThreshold = 0.014;    // Minimum angular position to move from target to comply (change position to new target) [rad]



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
