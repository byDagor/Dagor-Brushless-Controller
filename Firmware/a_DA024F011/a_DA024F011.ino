
//########################################################
// Dagor Controller Alpha 2.4 Firmware Version 1.1
//########################################################

// The firmware is separated into 6 tabs (a - f), make sure all files are stored in the folder 
// named a_DA024F010.

// This tab cointains the variables to customize the controller.

// Bellow are the variables dependent on the user's set-up. 
// If the phase resistance is unknown and can't be meausered with a multimeter assume a low resistance
// and over-write the voltage limit to 1V. If this limit is too small increase the number until the desired
// torque is achieved. 

//#######_USER VARIABLES_#######
byte pp = 7;                  //BLDC motor number of pole pairs
float phaseRes = 0.560;       //Phase winding resistance [ohms]
byte sourceVoltage = 12;      //Voltage of your power source [Volts]
byte maxCurrent = 2;          //Rough approximation of max current [Amps]
String controlType = "C2";    //control type: C2 -> position, C1 -> velocity, C0 -> torque (voltage)

// Bellow are the control loops parameters, to obtain the desired response out of the controller 
// they need to be tuned. These parameters can be tuned via the Serial Monitor sending a special 
// character next to the desired value. For example, to change the Position control loop PROPORTIONAL 
// gain from 25 to 10 the user writes in the Serial Monitor K10; to change the Velocity control loop 
// INTEGRAL gain from 2.5 to 3 the user writes I3. To the right of the variables bellow there are 
// the special characters that tune each corresponding parameter. Make sure to write the final values 
// of the parameters after tuning to update the firmware and upload it again.

//#######_CONTROLLER PARAMETERS_#######
float ki = 0.002;             //Velocity control loop PROPORTIONAL gain value   - P_
float ti = 2.5;               //Velocity control loop INTEGRAL gain value       - I_
float lpFilter = 0.000;       //Velocity measurement low-pass filter            - F_
float kp = 10;                //Position control loop PROPORTIONAL gain value   - K_
float voltageRamp = 25;       //Change in voltage allowed [Volts per sec]       - R_
float voltageLimit = phaseRes*maxCurrent;   //Voltage limit [Volts]             - L_
float velocityLimit = 2000;   //Velocity limit [rpm]                            - V_

// Bellow are extra parameters that can be easily configured for the desired application of the board,
// if the application is unknown the default values will work.

//########_EXTRA CONFIGURATON_##########
byte maxTemp = 80;            //Maximum temperature [°C]
float overTempTime = 3;       //Time in an over-temperature senario to disable the controller [seconds]
int fixedFreq = 5;            //Frequency of the fixed rate functions in loop [hertz]
String sensorType = "SPI";    //There are two possible position sensors:
                              //SPI -> absolute position sensor (14-bits)
                              //ABI -> quadrature incremental econder (11-bits)
