---
layout: default
title: Torque Control
parent: Motion Control
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /voltage_loop
nav_order: 1
---

# Torque control loop using voltage 
This control loop allows you to run the BLDC motor as it is simple DC motor, where you set the target voltage <i>U<sub>q</sub></i> to be set to the motor and the FOC algorithm calculates the necessary phase voltages <i>u<sub>a</sub></i> ,<i>u<sub>b</sub></i> and <i>u<sub>c</sub></i>  for smooth operation. This mode is enabled by:
```cpp
// voltage control loop
motor.controller = ControlType::voltage;
```
This control loop is very basic and it doesn't really have any configuration parameters. 

## How does it work exactly
 <a name="foc_image"></a><img src="extras/Images/voltage_loop.png">

The voltage control algorithm reads the angle <i>a</i> from the position sensor and the gets target <i>U<sub>q</sub></i> voltage value from the user and using the FOC algorithm sets the appropriate <i>u<sub>a</sub></i>, <i>u<sub>b</sub></i> and <i>u<sub>c</sub></i> voltages to the motor. FOC algorithm ensures that these voltages generate the magnetic force in the motor rotor exactly with <i>90 degree</i> offset from its permanent magnetic field, which guarantees maximal torque, this is called commutation.

The torque generated in the motor is proportional the voltage as <i>U<sub>q</sub></i> set buy user. Maximal torque corresponds to the maximal <i>U<sub>q</sub></i> which conditioned by the power supply voltage avaible, and the minimal torque is of course for <i>U<sub>q</sub></i> = 0.

For more info about the theory of the torque control check the section [Digging deeper section](digging_deeper) or go directly to [torque control theory](voltage_torque_control).

Now, lets see one example!

## Voltage control example 
You can test this algorithm by running the example `voltage_control.ino`.

Here we provide an example of a toque/voltage control program with full motion control configuration.  The program sets target <i>U<sub>q</sub></i> voltage of 2V to the motor using the FOC algorithm. 

```cpp
#include <SimpleFOC.h>

// motor instance
BLDCMotor motor = BLDCMotor( pole_pairs );
// driver instance
BLDCDriver3PWM driver = BLDCDriver3PWM(pwmA, pwmB, pwmC, enable);

// encoder instance
Encoder encoder = Encoder(chA, chB, ppr);

// Interrupt routine initialization
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}

void setup() { 
  
  // initialize encoder sensor hardware
  encoder.init();
  encoder.enableInterrupts(doA, doB); 
  // link the motor to the sensor
  motor.linkSensor(&encoder);
  
  // driver config
  driver.init();
  motor.linkDriver(&driver);
  
  // set motion control loop to be used
  motor.controller = ControlType::voltage;

  // use monitoring with serial for motor init
  // comment out if not needed
  motor.useMonitoring(Serial);

  // use monitoring with serial 
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // initialize motor
  motor.init();
  // align sensor and start FOC
  motor.initFOC();

  Serial.println("Motor ready.");
  _delay(1000);
}

// target voltage to be set to the motor
float target_voltage = 2;

void loop() {

  // main FOC algorithm function
  // the faster you run this function the better
  // Arduino UNO loop  ~1kHz
  // Bluepill loop ~10kHz 
  motor.loopFOC();

  // Motion control function
  // setting the target voltage
  motor.move(target_voltage);
}
```
## Project examples
Here is one very cool project example which uses torque control and describes the full hardware + software setup needed.

<div class="image_icon width30">
    <a href="simplefoc_pendulum">
        <img src="extras/Images/foc_pendulum.jpg" >
        <i class="fa fa-external-link-square fa-2x"></i>
    </a>
</div>

Find more projects in the [example projects](example_projects) section.