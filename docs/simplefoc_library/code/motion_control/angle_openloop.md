---
layout: default
title: Position Open-Loop
parent: Motion Control
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /angle_openloop
nav_order: 6
---

# Position open-loop control 
This control loop allows you to move your BLDC motor to the desired angle in real time without using the position sensor.  This mode is enabled by:
```cpp
// position control open-loop
motor.controller = ControlType::angle_openloop;
```
<img src="extras/Images/open_loop_angle.png">

You can test this algorithm by running the example `open_loop_position_example.ino`.

This control algorithm is very simple. User sets the target angle it wants to achieve <i>a<sub>d</sub></i>. The algorithm only subtracts the current angle <i>a<sub>c</sub></i> and the desired angle <i>a<sub>d</sub></i> to find the direction it needs to move and goes in that direction with the highest velocity possible `motor.velocity_limit`(max velocity). To set this velocity it uses the same algorithm as for [velocity open-loop control](velocity_openloop). It integrates the velocity it in time to find out what is the angle it needs to set to the motor <i>a<sub>c</sub></i> in order to achieve it. Then the maximal allowed voltage `motor.voltage_limit` is going to be applied in the direction of the <i>a<sub>c</sub></i> using Sinusoidal or SpaceVector modulation.

## Configuration
``` cpp
// choose FOC modulation (optional)
motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

//  maximal velocity of the position control
// default 20
motor.velocity_limit = 20;
// default voltage_power_supply
motor.voltage_limit = 3;
```

This type of motion control is highly inefficient therefore try not to use to high value for `motor.voltage_limit`. You can calculate the current the motor is going to be producing by checking the motor resistance `R` and evaluating:
```cpp
I = voltage_limit / R; // Amps
```

The maximal velocity `motor.velocity_limit` value is going to determine how fast your motor goes in between positions. The higher the value the faster the transition. But since we are turning the motor in open-loop we will not be able to know if the motor can follow the velocity. So make sure to put the `velocity_limit` value that is achievable for your motor. Also beware that for higher velocities and more holding torque you will need to increase the `motor.voltage_limit` variable as well.

Also, you can change the voltage limit `motor.voltage_limit` and transition velocity `motor.velocity_limit` in real-time if you need this kind of behavior in your application.
## Position open-loop control example
Here is one basic example of the velocity open-loop control with the complete configuration. The program will set the target position of `0 RAD` and maintain it, and the user cna change the target position using serial terminal.
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
  motor.velocity_limit = 20; // rad/s
  // open loop control config
  motor.controller = ControlType::angle_openloop;

  // init motor hardware
  motor.init();
  
  Serial.begin(115200);
  Serial.println("Motor ready!");
  _delay(1000);
}

float target_position = 0; // rad/s

void loop() {
  // open  loop angle movements 
  // using motor.voltage_limit and motor.velocity_limit
  motor.move(target_position);

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
      target_position = received_chars.toFloat();
      Serial.print("Target position: ");
      Serial.println(target_position);
      
      // reset the command buffer 
      received_chars = "";
    }
  }
}
```