---
layout: default
title: User parameters - tab a
parent: Firmware
nav_order: 1
permalink: /user_param
---

# User Parameters

There are a few parameters that have to be tweaked for each individual set-up. These parameters can be found in the tab *a* of the firmware folder and this section breaks-down the contents of this tab.

First there are the variables dependent on the user's set-up:

- pp: the number of pole pairs the brushless motor to use has. To learn how to obtain this number visit the [getting started section](https://docs.dagor.dev/getting_started#22-pole-pair-number).
- phaseRes: the resistance in the phase winding of the brushless motor. If the phase resistance is unknown and can't be measured with a multimeter assume a low resistance, 0.1 ohms, if the motor fails to move increase the number until the motor passes the calibration and the desired torque is achieved.
- sourceVoltage: state the voltage of the power source, whether it's a battery, a PSU or equivalent.
- maxCurrent: alongside the phase resistance the maximum current allowed is used to calculate a rough voltage limit to stay within safe limits.
- controlType: select the control type, C0 -> torque (voltage), C1 -> velocity, C2 -> angular position.

Code snippet:

```c++
//#######_USER VARIABLES_#######
byte pp = 7;                  //BLDC motor number of pole pairs
float phaseRes = 0.560;       //Phase winding resistance [ohms]
float sourceVoltage = 12;      //Voltage of your power source [Volts]
float maxCurrent = 1.25;        //Very rough approximation of max current [Amps]
String controlType = "C2";    //control type: C0 -> torque (voltage)
                                           // C1 -> velocity
                                           // C2 -> angular position
```

Second there are the control loops parameters, to obtain the desired response out of the controller these need to be tuned. The parameters can be tuned via the Serial Monitor sending a **command** via the Commander interface. To the right on the snip of code bellow there are the special characters that tune each corresponding parameter. Make sure to write the final values of the parameters to update the firmware and upload it again.

Second there are the control loops parameters, to obtain the desired response out of the controller they need to be tuned. These parameters can be tuned via the "Commander" interface, which is based on ASCII character command ids. There are two types of commands, a fetch and a set.

A fetch command will have the following structure:
                               motor ID ->  MLC <- Current
                                             ^
                                             |
                                           Limits

The example above will show the current limit set to the controller by sending the command *MLC*

A set command will have the following structure:
                               motor ID ->  MVI3.5 <- value
                                             ^^
                                            /  \
                          Velocity controller  Integral gain
The example above will set the Integral gain value of the velocity control loop to 3.5 by sending the command *MVI3.5*

Changing the set-point (target) looks like this:
                               motor ID ->  M6.28  <- value

The example above will rotate the motor's rotor one full turn, given angular position control, by sending the command *M6.28*

These commands can be sent through the Serial Monitor, make sure to set the baud rate to 115200 and "Newline". To the right of the variables bellow there are the characters that would be used to fetch or set each individual parameter. Make sure to write the final values of the parameters after tuning to update the firmware and upload it again.

Code snippet:

```c++
//#######_CONTROLLER PARAMETERS_#######
float vp = 0.002;             //Velocity control loop PROPORTIONAL gain value   - VP
float vi = 3.0;               //Velocity control loop INTEGRAL gain value       - VI
float vd = 0;                 //Velocity control loop DERIVATIVE gain value     - VD
float lpVelFilter = 0.000;    //Velocity measurement low-pass filter            - VF
float ap = 10;                //Position control loop PROPORTIONAL gain value   - AP
float ai = 0;                 //Position control loop INTEGRAL gain value       - AI
float ad = 0;                 //Position control loop DERIVATIVE gain value     - AD
float lpPosFilter = 0.000;    //Position measurement low-pass filter             - AF
float voltageRamp = 300;      //Change in voltage allowed [Volts per sec]       - VR
float velocityLimit = 2000;   //Velocity limit [rpm]                            - LV
```

Third there are more parameters for a more specific configuration and are not necessary to be modified to obtain a proper operation out of the controller. If the application of the controller is yet unknown the default values will work. It is recommended to change these parameters *after* feeling comfortable using the controller.

- maxTemp: set the threshold that defines an over-temperature condition for the temperature of the [power stage](power_stage).
- overTempTime: number of seconds an over-temperature condition has to maintain to trigger an over-temperature shutdown. Sometimes inrush current can make the temperature sensor detect an over-temperature condition for a few milliseconds, but after that the temperature drops. This parameter makes sure the temperature is actually high and the sensor didn't detect a big inrush current.
- sensorOffset: Offset the position that is consideres 0 radians by the amount of this variable.
- callerFixedFreq: the void loop of the firmware includes some functions that run at a slower rate that the general loop. This parameter allows to change the frequency these functions are called. For the default loop functions a slow rate of 5 hertz is more than enough, if the user defines its own functions maybe a faster or slower call rate is needed.
- alignStrength: This is a multiplier used to select how strong the magnetic field is during the calibration on start-up. Generally it's not necessary the full strength and half should be enough.
- motorID: Identifier attached to the motor for the *Commander* commands. Different IDs are useful for multi-board projects.
- focModulation: SimpleFOC implements two types of modulation: Sine PWM and Space Vector PWM.
- skipCalibration: The calibration the motor does on start-up can be skipped. After the boot capture the natural direction of movement and the electric angle offset that are printed.
- elecOffset: Relation of the magnet and the motor's rotor. Printed as: "MOT: Zero elec. angle: X.XX"
- natDirection: Natural direction of movement, give then U, V and W phases, can be either CW or CCW.

Code snippet:

```c++
//########_EXTRA CONFIGURATON_##########
byte maxTemp = 80;            // Maximum temperature if the power-stage [°C]
float overTempTime = 3;       // Time in an over-temperature scenario to disable the controller [seconds]
float sensorOffset = 0.0;     // Position offset, used to define an absolute 0 position on the motor's rotor [rads]
int motionDownSample = 3;     // Downsample the motion control loops with respect to the torque control loop [amount of loops]
int callerFixedFreq = 5;      // Frequency of the fixed rate function caller in void loop [hertz]
float alignStrength = 0.5;    // Percentage of power used to calibrate the sensor on start-up
char motorID = 'M';           // Motor ID used for command can be any character, useful for multi-board projects
bool focModulation = true;    // Field oriented control modulation type: true -> Sine PWM
                                                                     // false -> Space Vector PWM
bool skipCalibration = false; //Skip the calibration if the set-up won't change in the future
                              //electric angle offset and natural direction printed on start-up
const float elecOffset = 0.0; //Printed as: "MOT: Zero elec. angle: X.XX"
String natDirection = "CW";   //can be either CW or CCW   
```
