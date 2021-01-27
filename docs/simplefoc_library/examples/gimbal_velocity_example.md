---
layout: default
title: Gimbal Controller example 
parent: Example projects
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 3
permalink: /gimbal_velocity_example
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> 
---


# Velocity control example<br>using HMBGC V2.2 board
This is a very simple and cool example of using the FOC algorithm using the gimbal controller board. They are not meant to be used with the closed loop position control but <span class="simple">Simple<span class="foc">FOC</span>library</span> makes it not just possible but also pretty simple. 

Here is the hardware we used for this project:

[HMBGC V2.2](https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ:rk:1:pf:1) | [AMT 103 encoder](https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduiAsBlScvLoAWHUnKz39jAIpNPVt58AQ0PVb84dpbt53g%3D%3D) | [IPower GBM8017-120T](https://fr.aliexpress.com/item/32483131130.html?spm=a2g0o.productlist.0.0.6ddd749fFd3u9E&algo_pvid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb&algo_expid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb-10&btsid=0b0a187915885172220541390e7eed&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
--- | --- | --- | --- 
<img src="extras/Images/pinout.jpg" class="imgtable150"> | <img src="extras/Images/enc1.png" class="imgtable150">  | <img src="extras/Images/big.jpg" class="imgtable150"> 


# Connecting everything together
For a bit more in depth explanation of HMBGC V2.2 connections please check the [connection examples](hmbgc).
<p><img src="extras/Images/hmbgc_v22.jpg" class="width60">
</p>
 
## Encoder
<blockquote class="warning"> <p class="heading">Pinout restriction</p>
HMBGC doesn't have access to the Arduino's external interrupt pins <code class="highlighter-rouge">2</code> and <code class="highlighter-rouge">3</code>, moreover the only pins we have access to are analog pins <code class="highlighter-rouge">A0-A7</code>. 
Therefore we need to read the encoder channels using the software interrupt library, please check the  encoder <a href="encoder">code implementation </a> for more information. </blockquote>

- Encoder channels `A` and `B` are connected to the pins `A0` and `A1`.

## Motor
- Motor phases `a`,`b` and `c` are connected directly to the driver outputs
- Motor terminal `M1` uses Arduino pins `9`,`10`,`11` and `M2` uses `3`,`5`,`6`



# Arduino code 
Let's go through the full code for this example and write it together.
First thing you need to do is include the `SimpleFOC` library:

```cpp
#include <SimpleFOC.h>
```
Make sure you have the library installed. If you still don't have it please check the [get started page](installation).

Also in the case of the gimbal controllers like the HMBGC we do not have access to the hardware interrupt pins so you will need to have a software interrupt library.
I would suggest using `PciManager` library. If you have not installed it yet, you can do it using the Arduino library manager directly. Please check the `Encoder` class [docs](encoder) for more info.
So once you have it please include it to the sketch:
```cpp
// software interrupt library
#include <PciManager.h>
#include <PciListenerImp.h>
```

## Encoder code
First, we define the `Encoder` class with the A and B channel pins and number of impulses per revolution.
```cpp
// define Encoder
Encoder encoder = Encoder(A0, A1, 2048);
```
Then we define the buffering callback functions.
```cpp
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}
```
Next we define the `PciManager` pin change listeners:
```cpp
// pin change listeners
PciListenerImp listenerA(encoder.pinA, doA);
PciListenerImp listenerB(encoder.pinB, doB);
``` 
In the `setup()` function first we initialize the encoder:
```cpp
// initialize encoder hardware
encoder.init();
```
And then instead of calling `encoder.enableInterrupt()` function we use the `PciManager` library interface to attach the interrupts.
```cpp
// interrupt initialization
PciManager.registerListener(&listenerA);
PciManager.registerListener(&listenerB);
```
And that is it, let's setup the motor.

<blockquote class="info">For more configuration parameters of the encoders please check the <code class="highlighter-rouge">Encoder</code> class <a href="encoder">docs</a>.</blockquote>


## Motor code
First we need to define the `BLDCMotor` class with the  number od pole pairs (`14`)
```cpp
// define BLDC motor
BLDCMotor motor = BLDCMotor(14);
```
<blockquote class="warning">If you are not sure what your pole pairs number is please check the  <code class="highlighter-rouge">find_pole_pairs.ino</code> example.</blockquote>


Next we need to define the `BLDCDriver3PWM` class with the PWM pin numbers of the motor
```cpp
// define BLDC driver
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 11);
```

Then in the `setup()` we configure first the voltage of the power supply if it is not `12` Volts and init the driver.
```cpp
// power supply voltage
// default 12V
driver.voltage_power_supply = 12;
driver.init();
```
Then we tell the motor which control loop to run by specifying the `motor.controller` variable.
```cpp
// set control loop type to be used
// ControlType::voltage
// ControlType::velocity
// ControlType::angle
motor.controller = ControlType::velocity;
```
Now we configure the PI controller parameters
```cpp
// velocity PI controller parameters
// default P=0.5 I = 10
motor.PID_velocity.P = 0.2;
motor.PID_velocity.I = 20;
// jerk control using voltage voltage ramp
// default value is 300 volts per sec  ~ 0.3V per millisecond
motor.PID_velocity.output_ramp = 1000;

//default voltage_power_supply
motor.voltage_limit = 6;
```
Additionally we can configure the Low pass filter time constant `Tf`
```cpp
// velocity low pass filtering
// default 5ms - try different values to see what is the best. 
// the lower the less filtered
motor.LPF_velocity.Tf = 0.01;
```
<blockquote class="info">For more information about the velocity control loop parameters please check the <a href="velocity_loop">doc</a>.</blockquote>

And finally we connect the encoder and the driver to the motor, do the hardware init  and init of the Field Oriented Control.
```cpp  
// link the motor to the sensor
motor.linkSensor(&encoder);
// link driver
motor.linkDriver(&driver);

// initialize motor
motor.init();
// align encoder and start FOC
motor.initFOC();
```
The last peace of code important for the motor is of course the FOC routine in the `loop` function.
```cpp
void loop() {
// iterative FOC function
motor.loopFOC();

// iterative function setting and calculating the velocity loop
// this function can be run at much lower frequency than loopFOC function
motor.move(target_velocity);
}
```
That is it, let's see the full code now!
<blockquote class="info">For more configuration parameters and control loops please check the <code class="highlighter-rouge">BLDCMotor</code> class <a href="motors_config">doc</a>.</blockquote>

## Full Arduino code
To the full code I have added a small serial communication code in the `serialEvent()` function,  to be able to change velocity target value in real time.
```cpp
#include <SimpleFOC.h>
// software interrupt library
#include <PciManager.h>
#include <PciListenerImp.h>


// define BLDC motor
BLDCMotor motor = BLDCMotor( 14 );
// define driver
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 11);
//  define Encoder
Encoder encoder = Encoder(A0, A1, 8192);
// interrupt routine initialization
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}

// encoder interrupt init
PciListenerImp listenerA(encoder.pinA, doA);
PciListenerImp listenerB(encoder.pinB, doB);

void setup() {
  // initialize encoder hardware
  encoder.init();
  // interrupt initialization
  PciManager.registerListener(&listenerA);
  PciManager.registerListener(&listenerB);
  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // power supply voltage
  // default 12V
  driver.voltage_power_supply = 12;
  driver.init();
  // link the motor to the driver
  motor.linkDriver(&driver);

  // set FOC loop to be used
  // ControlType::voltage
  // ControlType::velocity
  // ControlType::angle
  motor.controller = ControlType::velocity;

  // controller configuration based on the control type 
  // velocity PI controller parameters
  // default P=0.5 I = 10
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motor.PID_velocity.output_ramp = 1000;

  // velocity low pass filtering
  // default 5ms - try different values to see what is the best. 
  // the lower the less filtered
  motor.LPF_velocity.Tf = 0.01;

  //default voltage_power_supply
  motor.voltage_limit = 6;

  // initialize motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // monitoring port
  Serial.begin(115200);
  Serial.println("Motor ready.");
  Serial.println("Set the target velocity using serial terminal:");
  _delay(1000);
}

float target_velocity=0;

void loop() {
  // iterative FOC function
  motor.loopFOC();

  // 0.5 hertz sine wave
  //target_velocity = sin( micros()*1e-6 *2*M_PI * 0.5 );

  // iterative function setting the velocity target
  motor.move(target_velocity);

}

// Serial communication callback function
// gets the target value from the user
void serialEvent() {
  // a string to hold incoming data
  static String inputString; 
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline
    // end of input
    if (inChar == '\n') {
      target_velocity = inputString.toFloat();
      Serial.print("Target velocity: ");
      Serial.println(target_velocity);
      inputString = "";
    }
  }
}
```
