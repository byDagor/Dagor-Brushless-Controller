---
layout: default
title: User parameters - tab a
parent: Firmware
nav_order: 1
permalink: /user_param
---

# User Parameters

There are a few parameters that have to be tweaked for each individual set-up, this parameter can be found on the tab *a* of the firmware folder. This section covers a break-down of this tab.

Bellow are the variables dependent on the user's set-up. If the phase resistance is unknown and can't be measured with a multimeter assume a low resistance and over-write the voltage limit to 1V. If this is too small increase the number until the desired torque is achieved.

```c++
//#######_USER VARIABLES_#######
byte pp = 7;                  //BLDC motor number of pole pairs
float phaseRes = 0.560;       //Phase winding resistance [ohms]
byte sourceVoltage = 12;      //Voltage of your power source [Volts]
byte maxCurrent = 2;          //Rough approximation of max current [Amps]
```

These are the control loops parameters, to obtain the desired response out of the controller they need to be tuned. The parameters can be tuned via the Serial Monitor sending a [command](https://docs.simplefoc.com/communication) which consists of a special character next to the desired value. For example, to change the Position control loop PROPORTIONAL gain from 25 to 10 the user writes in the Serial Monitor *K10*; to change the Velocity control loop INTEGRAL gain from 2.5 to 3 the user writes *I3*. To the right on the snip of code bellow there are the special characters that tune each corresponding parameter. Make sure to write the final values of the parameters to update the firmware and upload it again.

```c++
//#######_CONTROLLER PARAMETERS_#######
float ki = 0.002;             //Velocity control loop PROPORTIONAL gain value   - P_
float ti = 2.5;               //Velocity control loop INTEGRAL gain value       - I_
float lpFilter = 0.000;       //Velocity measurement filter                     - F_
float kp = 10;                //Position control loop PROPORTIONAL gain value   - K_
float voltageRamp = 25;       //Change in voltage allowed [Volts per sec]       - R_
float voltageLimit = phaseRes*maxCurrent;   //Voltage limit                     - L_
float velocityLimit = 2000;   //Velocity limit [rpm]                            - V_
```

The parameters bellow allow further personal configuration and are not necessary to be modified to obtain a proper operation out of the controller. If the application of the controller is yet unknown the default values will work.

```c++
//########_EXTRA CONFIGURATON_##########
byte maxTemp = 80;            //Maximum temperature [°C]
float overTempTime = 3;       //Time in an over-temperature scenario to disable the controller [seconds]
int fixedFreq = 5;            // Frequency of the fixed rate functions in loop [hertz]
```
