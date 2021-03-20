
//########################################################
// Dagor Controller Alpha 2.4 Firmware Version 1.2.0
//########################################################

// The firmware is separated into 6 tabs (a - f), make sure all files are stored in the folder 
// named a_DA024F111.

// This tab cointains the parameters to customize the controller. Learn more at https://docs.dagor.dev/

//########################################################

// Below are the variables dependent on the user's set-up. 
// If the phase resistance is unknown and can't be meausered with a multimeter assume a low resistance.

//#######_USER VARIABLES_#######
byte pp = 7;                  //BLDC motor number of pole pairs
float phaseRes = 0.560;       //Phase winding resistance [ohms]
float sourceVoltage = 12;      //Voltage of your power source [Volts]
float maxCurrent = 1.25;        //Very rough approximation of max current [Amps]
String controlType = "C0";    //control type: C0 -> torque (voltage)
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
float voltageLimit = phaseRes*maxCurrent;   //Voltage limit [Volts]             - LU
float vp = 0.002;             //Velocity control loop PROPORTIONAL gain value   - VP
float vi = 3.0;               //Velocity control loop INTEGRAL gain value       - VI
float vd = 0;                 //Velocity control loop DERIVATIVE gain value     - VD
float lpVelFilter = 0.000;    //Velocity measurement low-pass filter            - VF
float ap = 10;                //Position control loop PROPORTIONAL gain value   - AP
float ai = 0;                 //Position control loop INTEGRAL gain value       - AI
float ad = 0;                 //Position control loop DERIVATIVE gain value     - AD
float lpPosFilter = 0.000;    //Position measurment low-pass filter             - AF
float voltageRamp = 300;      //Change in voltage allowed [Volts per sec]       - VR
float velocityLimit = 2000;   //Velocity limit [rpm]                            - LV

// Below are extra parameters that can be further configured for the desired application or project,
// if the application is unknown the default values will work. It is recommended to change these 
// parameters AFTER feeling comfortable using the controller.

//########_EXTRA CONFIGURATON_##########
byte maxTemp = 80;            // Maximum temperature if the power-stage [°C]
float overTempTime = 3;       // Time in an over-temperature senario to disable the controller [seconds]
float sensorOffset = 0.0;     // Position offset, used to define an absolute 0 position on the motor's rotor [rads]
int motionDownSample = 3;     // Downsample the motion control loops with respect to the torque control loop [amount of loops]
int callerFixedFreq = 5;      // Frequency of the fixed rate function caller in void loop [hertz]
float alignStrength = 0.5;    // Percentage of power used to calibrate the sensor on start-up
char motorID = 'M';           // Motor ID used for the commander interface, can be any character 
                              // (a unique ID is useful for multi-board proyects)
bool commandDebug = true;     // Enable/ disable commander serial print 
bool focModulation = true;    // Field oriented control modulation type: true -> Sine PWM
                                                                     // false -> Space Vector PWM
bool skipCalibration = false; // Skip the calibration on start-up
                              // electric angle offset and natural direction printed on start-up
const float elecOffset = 0.0; // Printed as: "MOT: Zero elec. angle: X.XX"
String natDirection = "CW";   // Can be either CW or CCW   


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
    E - Motor status (enable/disable)
      0 - enable
      1 - disable
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
