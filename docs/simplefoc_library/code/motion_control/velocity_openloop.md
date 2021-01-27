---
layout: default
title: Velocity Open-Loop
parent: Motion Control
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /velocity_openloop
nav_order: 5
---

# Velocity open-loop control 
This control loop allows you to spin your BLDC motor with desired velocity without using position sensor. This mode is enabled by:
```cpp
// velocity control open-loop
motor.controller = ControlType::velocity_openloop;
```

<img src="extras/Images/open_loop_velocity.png" >

You can test this algorithm by running the example `open_loop_velocity_example.ino`.

This control algorithm is very simple. User can set the target velocity it wants to achieve <i>v<sub>d</sub></i>, the algorithm is going to integrate it in time to find out what is the angle it needs to set to the motor <i>a<sub>c</sub></i> in order to achieve it. Then the maximal allowed voltage `motor.voltage_limit` is going to be applied in the direction of the <i>a<sub>c</sub></i> using SinusoidalPWM or SpaceVectorPWM modulation.

This is the simplified version of the calculation of the next angle to set to the motor: 
```cpp
next_angle = past_angle + target_velocity*d_time;
```
You need to know the  `target_velocity`, sample time `d_time` and past value of the angle `past_angle` you have set to the motor.

## Configuration
```cpp
// choose FOC modulation (optional) - default SinePWM
motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

// limiting voltage 
motor.voltage_limit = 3;   // rad/s
```

This type of motion control is highly inefficient therefore try not to use to high value for `motor.voltage_limit`. You can calculate the current the motor is going to be producing by checking the motor resistance `R` and evaluating:
```cpp
I = voltage_limit / R; // Amps
```

Also, you can change the voltage limit in real-time if you need this kind of behavior in your application.

## Velocity open-loop control example

Here is one basic example of the velocity open-loop control with the complete configuration. The program will set the target velocity of `2 RAD/s` and maintain it, and the user cna change the target velocity using serial terminal.

```cpp
// Open loop motor control example 
#include <SimpleFOC.h>

// motor instance
BLDCMotor motor = BLDCMotor( 11 );
// driver instance
BLDCDriver3PWM driver = BLDCDriver3PWM(3, 10, 6, 7);

void setup() {

  // driver config
  driver.init();
  motor.linkDriver(&driver);

  // limiting motor movements
  motor.voltage_limit = 3;   // rad/s

  // open loop control config
  motor.controller = ControlType::velocity_openloop;

  // init motor hardware
  motor.init();
  
  Serial.begin(115200);
  Serial.println("Motor ready!");
  _delay(1000);
}

float target_velocity = 2; // rad/s

void loop() {
  // open loop velocity movement 
  // using motor.voltage_limit and motor.velocity_limit
  motor.move(target_velocity);

  // receive the used commands from serial
  serialReceiveUserCommand();
}

// utility function enabling serial communication with the user to set the target values
// this function can be implemented in serialEvent function as well
void serialReceiveUserCommand() {
  
  // a string to hold incoming data
  static String received_chars;
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;
    // end of user input
    if (inChar == '\n') {
      
      // change the motor target
      target_velocity = received_chars.toFloat();
      Serial.print("Target velocity ");
      Serial.println(target_velocity);
      
      // reset the command buffer 
      received_chars = "";
    }
  }
}
```
