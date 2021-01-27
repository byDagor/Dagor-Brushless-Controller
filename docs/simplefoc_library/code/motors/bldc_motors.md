---
layout: default
title: BLDCMotor
nav_order: 1
permalink: /bldcmotor
parent: Motor code
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# BLDC Motor configuration

<div class="width60">
<img src="extras/Images/mot2.jpg" style="width:30%;display:inline"><img src="extras/Images/bigger.jpg" style="width:30%;display:inline"><img src="extras/Images/mot.jpg" style="width:30%;display:inline">
</div>

All BLDC motors are handled with the `BLDCMotor` class. This class implements:
- BLDC FOC algorithm
- Motion control loops
- Monitoring
- User communication interface

## Step 1. Creating the instance of the BLDC motor
To instantiate the BLDC motor we need to create an instance of the `BLDCMotor` class and provide it the number of `pole pairs` of the motor.
```cpp
//  BLDCMotor(int pp)
//  - pp  - pole pair number
BLDCMotor motor = BLDCMotor( 11 );
```
<blockquote class="info"><p class="heading">Pole pair number </p>
If you are not sure what your <code class="highlighter-rouge">pole_paris</code> number is. The library provides an example code to estimate your <code class="highlighter-rouge">pole_paris</code> number in the examples <code class="highlighter-rouge">find_pole_pairs_number.ino</code>.
 </blockquote>

## Step 2. Linking the sensor 
Once when you have the `motor` defined and the sensor initialized you need to link the `motor` and the `sensor` by executing:    
```cpp
// link the sensor to the motor
motor.linkSensor(&sensor);
```
Method `linkSensor` is able to link the motor to any sensor implemented in this library. The `sensor` will be used to determine electrical position of the motor for the FOC algorithm as well as for the motion control loops of velocity and position. See the [position sensor docs](sensors) for more info!

<blockquote class="info">Linking is not necessary when using the openloop motion control.</blockquote>

## Step 3. Linking the driver 
Once when you have the `motor` defined and the driver initialized you need to link the `motor` and the `driver` by executing:    
```cpp
// link the driver to the motor
motor.linkDriver(&driver);
```

The `BLDCMotor` class expect to receive a `BLDCDriver` class instance, implemented by default with classes `BLDCDriver3PWM` and `BLDCDriver6PWM`. The `driver` deals with all the hardware specific operations related to specific microcontroller architecture and driver hardware. See the [bldc driver docs](bldcdriver) for more info!

## Step 4. Configuration

If you choose not to set some of the configuration parameters they will take values defined in the `defaults.h` file.
Check the [library source code](source_code) to dig deeper.

### Step 4.1 Modulation type

There are four types of Field Oriented Control modulation types implemented for BLDC motors:
- Sinusoidal PWM modulation
- Space Vector PWM modulation
- Block commutation - *beneficial for current control applications - **future release***
    - Trapesoidal 120
    - Trapesoidal 150

You can set them by changing the `motor.foc_modulation` variable:
```cpp
// choose FOC modulation
// FOCModulationType::SinePWM; (default)
// FOCModulationType::SpaceVectorPWM;
// FOCModulationType::Trapezoid_120;
// FOCModulationType::Trapezoid_150;
motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
```
<blockquote class="info"> <p class="heading">Choosing modulation/commutation</p> Sinusoidal PWM and Space vector commutation patters will produce very smooth operation but block commutation will be faster to execute, therefore more suitable for higher velocities. It is suggested to use the Trapesoidal 120 commutation with Hall sensors. Other commutation patterns will work as well but this one will have the best performance. </blockquote>

For more information about the theory of these approaches please and source code implementation check the [FOC implementation docs](foc_implementation) or visit the [digging deeper section](digging_deeper).


### Step 4.2 Sensor and motor aligning parameters
The voltage used for the motor and sensor alignment set the variable `motor.voltage_sensor_align`:
```cpp
// aligning voltage [V]
// default 6V
motor.voltage_sensor_align = 3;
```

### Step 4.3 Motion control parameters  

There are 3 different closed loop control strategies implemented in the Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>: 
- [torque control loop using voltage](voltage_loop)
- [position/angle motion control](angle_loop)
- [velocity motion control](velocity_loop)

Additionally <span class="simple">Simple<span class="foc">FOC</span>library</span> implements two open loop control strategies as well:
- [position open-loop control](angle_openloop)
- [velocity open-loop control](velocity_openloop)

You set it by changing the `motor.controller` variable. 
```cpp
// set motion control loop to be used
// ControlType::voltage     - torque control loop using voltage
// ControlType::velocity    - velocity motion control
// ControlType::angle       - position/angle motion control
// ControlType::velocity_openloop    - velocity open-loop control
// ControlType::angle_openloop       - position open-loop control
motor.controller = ControlType::angle;
```
<blockquote class="warning"><p class="heading"> Important!</p>This parameter doesn't have a default value and it has to be set before real-time execution starts.</blockquote>

Each motion control strategy has its own parameters and you can find more about them on [motion control docs](motion_control). 

Here is the list of all the motion control configuration parameters:
```cpp

// incremental encoder index search velocity [rad/s]
motor.velocity_index_search = 3;

// set control loop type to be used
motor.controller = ControlType::voltage;

// controller configuration based on the control type 
motor.PID_velocity.P = 0.2;
motor.PID_velocity.I = 20;
motor.PID_velocity.D = 0.001;

// velocity low pass filtering time constant
motor.LPF_velocity.Tf = 0.01;

// angle loop controller
motor.P_angle.P = 20;

// angle loop velocity limit
motor.velocity_limit = 50;
// default value the same as driver.voltage_limit
motor.voltage_limit = 12;
```

### Step 4.4 Configuration done - `motor.init()`
Finally the configuration is terminated by running `init()` function which prepares all the hardware and software motor components using the configured values.
```cpp
// initialize motor
motor.init();
```

## Step 5. Field Oriented Control initialization

After the sensor, driver and the motor are configured, and before we can start the motion control we need to initialize the FOC algorithm. 
```cpp
// align sensor and start FOC
motor.initFOC();
```
<blockquote class="danger"><p class="heading"> Skip for openloop control!</p>This function should not be called if the motor is run in open loop! </blockquote>

This function aligns sensor and motor zero positions and initializes FOC variables. It is intended to be run in the `setup` function of the Arduino. After the call of this function FOC is ready and our setup is done! 

If you are using absolute sensors such as magnetic sensors, in the `initFOC()` you can provide the sensor offset `zero_electric_offset` and sensor direction `sensor_direction` to avoid alignment procedure:
```cpp
// align sensor and start FOC
//motor.initFOC(zero_electric_offset, sensor_direction);
motor.initFOC(2.15, Direction::CW);
```
You can find these values by running the `find_sensor_offset_and_direction.ino` example.

For more info about what really happens in `initFOC()` check the [FOC source code implementation](foc_implementation).

## Step 6. Real-time motion control

The real-time motion control of theArduino <span class="simple">Simple<span class="foc">FOC</span>library</span> is realized with two functions: `motor.loopFOC()` and `motor.move(float target)`.
```cpp
// Function running FOC algorithm in real-time
// it calculates the gets motor angle and sets the appropriate voltages 
// to the phase pwm signals
// - the faster you can run it the better Arduino UNO ~1ms, Bluepill ~ 100us
motor.loopFOC();
```
<blockquote class="danger"><p class="heading"> Skip for openloop control!</p>This function should not be called if the motor is run in open loop! </blockquote>

The function `loopFOC()` gets the current motor angle from the sensor, turns it into the electrical angle and transforms the quadrature <i>U<sub>q</sub></i> voltage `motor.voltage_q` to the appropriate phase voltages <i>u<sub>a</sub></i>, <i>u<sub>b</sub></i> and <i>u<sub>c</sub></i> which are set then set to the motor. 

The transformation of the <i>U<sub>q</sub></i> to <i>u<sub>a</sub></i>, <i>u<sub>b</sub></i> and <i>u<sub>c</sub></i>  is defined with the configuration parameter `motor.foc_modulation` and it can be either: ***Sinusoidal PWM***, ***Space Vector PWM*** or the block commutation ***Trapesoid 120*** and ***Trapesoid 150***. 
<blockquote class="info"><p class="heading"> New Feature - <i>U<sub>d</sub></i> direct voltage support!</p><i><b>Sinusoidal PWM</b></i> supports the direct voltage setting <i>U<sub>d</sub></i> setting. Direct voltage can be set by changing the variable <code class="highlighter-rouge">motor.voltage_d</code>! This is a new feature and it will be exploited more in the feature releases with the current control.</blockquote>

This function is execution time is critical, therefore it is very important that the `motor.loopFOC()` function is executed as fast as possible!

<blockquote class="warning"><p class="heading">Rule od thumb: execution time</p>

The faster you can run this function the better!
<ul style="margin-bottom:0em">
    <li> Arduino UNO <code class="highlighter-rouge">loop()</code> ~ 1ms </li>
    <li> Esp32 <code class="highlighter-rouge">loop()</code> ~ 500us</li>
    <li> Bluepill <code class="highlighter-rouge">loop()</code> ~ 200us</li>
    <li> Nucleo <code class="highlighter-rouge">loop()</code> ~ 100us</li>
</ul>
</blockquote>

Finally, once we have a way to set the <i>U<sub>q</sub></i> (and <i>U<sub>d</sub></i>) to the motor using the FOC algorithm we can proceed to the motion control. And this is done with `motor.move()` function.
```cpp
// Function executing the control loops configured by the motor.controller parameter of the motor. 
// - This function doesn't need to be run upon each loop execution - depends of the use case
//
// target  Either voltage, angle or velocity based on the motor.controller
//         If it is not set the motor will use the target set in its variable motor.target
motor.move(target);
```

The `move()` method executes the motion control loops of the algorithm. If is governed by the `motor.controller` variable. It executes either pure voltage loop, velocity loop or angle loop.

It receives one parameter `float target` which is current user defined target value.
- If the user runs [velocity loop](velocity_loop) or [velocity open-loop](velocity_openloop), `move` function will interpret `target` as the target velocity <i>v<sub>d</sub></i>.
- If the user runs [angle loop](angle_loop) or [angle open-loop](angle_openloop), `move` will interpret `target` parameter as the target angle <i>a<sub>d</sub></i>. 
- If the user runs the [voltage loop](voltage_loop), `move` function will interpret the `target` parameter as voltage <i>u<sub>d</sub></i>.

The `target` parameter is optional and if it is not set, the target value will be set by the public motor variable `motor.target`. The equivalent code would be:

```cpp
motor.target = 2;
motor.move();
```

And that is it, you have your complete Field Oriented Controlled BLDC motor with motion control. 

## User interaction

<span class="simple">Simple<span class="foc">FOC</span>library</span> implements two types of real-time user interaction:
- [Monitoring functionality](monitoring)
- [Motor commands](communication)


## Digging deeper
For more theoretical explanations and source code implementations of the FOC algorithm and the motion control approaches check out the [digging deeper section](digging_deeper).

## Example code
A simple BLDC motor torque control using voltage based on the FOC algorithm.
```cpp
/**
 * Torque control example using voltage control loop.
 */
#include <SimpleFOC.h>

// BLDC motor instance
BLDCMotor motor = BLDCMotor(11);
// driver instance
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);
// sensor instance
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

void setup() { 
  
  // initialize encoder sensor hardware
  sensor.init();
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link driver
  motor.linkDriver(&driver);

  // aligning voltage
  motor.voltage_sensor_align = 3;
  
  // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set motion control loop to be used
  motor.controller = ControlType::voltage;

  // initialize motor
  motor.init();
  // align sensor and start FOC
  motor.initFOC();

  _delay(1000);
}

// target voltage to be set to the motor
float target_voltage = 2;

void loop() {

  // main FOC algorithm function
  motor.loopFOC();

  // Motion control function
  motor.move(target_voltage);
}
```
