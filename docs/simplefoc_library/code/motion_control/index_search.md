---
layout: default
title: Index Search loop 
parent: Motion Control
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 4
permalink: /index_search_loop
---

# Index search routine
Finding the encoder index is performed only if the constructor of the `Encoder` class has been provided with the `index` pin. The search is performed by setting a constant velocity of the motor until it reaches the index pin. To set the desired searching velocity alter the parameter:
```cpp
// index search velocity - default 1rad/s
motor.velocity_index_search = 2;
```
The index search is executed in the `motor.initFOC()` function. 

This velocity control loop is implemented exactly the same as [velocity control loop](/velocity_loop) and has the same controller parameters which are set in the configuration of the  [velocity control loop](/velocity_loop).

## Example of code using Index search

This is an example of a motion control program which uses encoder as position sensor and particularly, encoder with `index` signal. The index search velocity is set to be `3 RAD/s`:
```cpp
// index search velocity [rad/s]
motor.velocity_index_search = 3;
```

After the motor and the position senor have been aligned in `motor.initFOC()` by performing index search. The motor will start spinning with angular velocity `2 RAD/s` and maintain this value.

```cpp
#include <SimpleFOC.h>

// motor instance
BLDCMotor motor = BLDCMotor(11);
// driver instance
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 11, 8);

// encoder instance
Encoder encoder = Encoder(2, 3, 8192, A0);

// Interrupt routine initialization
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}
void doIndex(){encoder.handleIndex();}


void setup() {
  
  // initialize encoder sensor hardware
  encoder.init();
  encoder.enableInterrupts(doA, doB,doIndex); 

  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // driver config
  driver.init();
  motor.linkDriver(&driver);

  // index search velocity [rad/s]
  motor.velocity_index_search = 3;

  // set motion control loop to be used
  motor.controller = ControlType::velocity;

  // controller configuration 
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  // default voltage_power_supply
  motor.voltage_limit = 6;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motor.PID_velocity.output_ramp = 1000;
 
  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;


  // use monitoring with serial 
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);
  
  // initialize motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();


  Serial.println("Motor ready.");
  _delay(1000);
}

// angle set point variable
float target_velocity = 2;

void loop() {
  // main FOC algorithm function
  // the faster you run this function the better
  // Arduino UNO loop  ~1kHz
  // Bluepill loop ~10kHz 
  motor.loopFOC();

  // Motion control function
  // velocity, position or voltage (defined in motor.controller)
  // this function can be run at much lower frequency than loopFOC() function
  // You can also use motor.move() and set the motor.target in the code
  motor.move(target_velocity);

}

```
