---
layout: default
title: Monitoring
parent: Writing the Code
nav_order: 5
permalink: /monitoring
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> 
---


# Monitoring functionality

Both `BLDCMotor` and `StepperMotor` classes support monitoring using `Serial` port which is enabled by:
```cpp
motor.useMonitoring(Serial);
```

The `motor` will output to the serial its status during the initialization `motor.init()` of the motor and the FOC `motor.initFOC()`. Enabling this functionality will not directly influence the real-time performance because there is no predefined monitoring in real time-loop in the functions `motor.loopFOC()` and `motor.move()`.

This is an example of the `motor` initialization monitoring output:
```sh
MOT: Monitor enabled!
MOT: Init pins.
MOT: PWM config.
MOT: Enable.
MOT: Align sensor.
MOT: Absolute zero align.
MOT: Success!
MOT: Motor ready.
```

To monitor the real-time execution you can use `motor.monitor()` function which will output the motor variables to the serial port. The function outputs different variables relevant to the motion control loops you are running. 

Here is the list of the outputted variables:
- torque control (`ControlType::voltage`): <br>
`motor.voltage_q`, `motor.shaft_angle` and `motor.shaft_velocity`  
- velocity control (`ControlType::velocity`): <br> `motor.voltage_q`, `motor.target` and `motor.shaft_velocity`
- position control (`ControlType::angle`):  <br>`motor.voltage_q`, `motor.target` and `motor.shaft_angle`

This function is intended to be used for real-time visualization, particularly suitable for Arduino IDE's `Serial Plotter`.

<img class="width60" src="extras/Images/plotter.jpg">

Or in `Serial Terminal`
```sh
...
voltage,target,velocity
1.17	2.00	2.29
1.23	2.00	1.96
1.30	2.00	1.65
1.28	2.00	1.80
1.20	2.00	2.20
1.07	2.00	2.70
0.91	2.00	3.22
0.69	2.00	3.74
0.40	2.00	4.34
0.18	2.00	4.57
0.09	2.00	4.38
0.06	2.00	4.04
0.08	2.00	3.58
0.11	2.00	3.14
0.18	2.00	2.65
0.27	2.00	2.13
0.37	2.00	1.65
0.47	2.00	1.26
0.55	2.00	0.99
0.64	2.00	0.77
0.71	2.00	0.67
...
```

<blockquote class="warning"><p class="heading"> Execution time impairment</p>
The intention of this method is to be called in main loop function along the <code class="highlighter-rouge">loopFOC()</code> and <code class="highlighter-rouge">move()</code> function. Therefore, <code class="highlighter-rouge">motor.monitor()</code> is going to impair the execution performance and reduce the sampling frequency of the FOC algorithm so therefore take it in consideration when running the code.  </blockquote>

If you wish to implement you own monitoring functions or just output the motor variables to the `Serial` terminal here are the public variables of the `BLDCMotor` class that you can access at any time.
```cpp
// current target value
float target;
// current motor angle
float shaft_angle;
// current motor velocity 
float shaft_velocity;
// current target velocity
float shaft_velocity_sp;
// current target angle
float shaft_angle_sp;

// current voltage u_q set to the motor
float voltage_q;
// current voltage u_d set to the motor
float voltage_d;
// phase voltages 
float Ua, Ub, Uc;

```
You can access any of these variables by adding `motor.` before it. For example:
```cpp
Serial.println(motor.shaft_angle);// print current motor position to the serial terminal
// or
Serial.println(motor.Ua); // print phase voltage Ua to the serial terminal
```

As you can see monitoring works only in one direction and it assumes you will implement the user communication on your own.

## Real-time user communication using motor commands
  
For two-way communication in between user and the motor the Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>  provides you with the [Motor commands interface](communication).