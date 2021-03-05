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

- Pole pair number: the number of pole pairs the brushless motor to use has. To learn how to obtain this number visit the [getting started section](https://docs.dagor.dev/getting_started#22-pole-pair-number).
- Phase winding resistance: the resistance in the phase winding in the brushless motor. This number is used to set an appropriate voltage limit to limit the maximum current with SimpleFOC. If the phase resistance is unknown and can't be measured with a multimeter assume a low resistance and over-write the voltage limit to 1V. If this voltage limit is too small (motor fails to move) increase the number until the desired torque is achieved.
- Voltage of the power source: state the voltage of the power source, whether it's a battery, a PSU or equivalent.
- Maximum current: alongside the phase resistance the maximum current allowed is used to calculate a rough voltage limit to stay within safe limits.

Code snippet:

```c++
//#######_USER VARIABLES_#######
byte pp = 7;                  //BLDC motor number of pole pairs
float phaseRes = 0.560;       //Phase winding resistance [ohms]
byte sourceVoltage = 12;      //Voltage of your power source [Volts]
byte maxCurrent = 2;          //Rough approximation of max current [Amps]
```

Second there are the control loops parameters, to obtain the desired response out of the controller these need to be tuned. The parameters can be tuned via the Serial Monitor sending a **command** which consists of a special character next to the desired value. For example, to change the Position control loop PROPORTIONAL gain from 25 to 10 the user writes in the Serial Monitor *K10*; to change the Velocity control loop INTEGRAL gain from 2.5 to 3 the user writes *I3*. To the right on the snip of code bellow there are the special characters that tune each corresponding parameter. Make sure to write the final values of the parameters to update the firmware and upload it again.

Code snippet:

```c++
//#######_CONTROLLER PARAMETERS_#######
float ki = 0.002;             //Velocity control loop PROPORTIONAL gain value   - P_
float ti = 2.5;               //Velocity control loop INTEGRAL gain value       - I_
float lpFilter = 0.000;       //Velocity measurement filter                     - F_
float kp = 10;                //Position control loop PROPORTIONAL gain value   - K_
float voltageRamp = 25;       //Change in voltage allowed [Volts per sec]       - R_
float velocityLimit = 2000;   //Velocity limit [rpm]                            - V_
float voltageLimit = phaseRes*maxCurrent;   //Voltage limit                     - L_
```

Third there are more parameters for a more specific configuration and are not necessary to be modified to obtain a proper operation out of the controller. If the application of the controller is yet unknown the default values will work.

- Maximum temperature: set the threshold that defines an over-temperature condition for the temperature of the [power stage](power_stage).
- Over temperature time span: number of seconds an over-temperature condition has to maintain to trigger an over-temperature shutdown. Sometimes inrush current can make the temperature sensor detect an over-temperature condition for a few milliseconds, but after that the temperature drops. This parameter makes sure the temperature is actually high and the sensor didn't detect an inrush current.
- Fixed frequency of loop functions: the void loop of the firmware includes some functions that run at a slower rate that the general loop. This parameter allows to change the frequency these functions are called. For the default loop functions a slow rate of 5 hertz is more than enough, if the user defines its own functions maybe a faster or slower call rate is needed.

Code snippet:

```c++
//########_EXTRA CONFIGURATON_##########
byte maxTemp = 80;            //Maximum temperature [°C]
float overTempTime = 3;       //Time in an over-temperature scenario to disable the controller [seconds]
int fixedFreq = 5;            // Frequency of the fixed rate functions in loop [hertz]
```
