---
layout: default
title: Velocity Control example
parent: Example projects
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 2
permalink: /velocity_control_example
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> 
---

# Velocity control example<br>using Drotek's L6234 driver
[Drotek's L6234 breakout board](https://store-drotek.com/212-brushless-gimbal-controller-l6234.html) is a very minimalistic 3-phase BLDC motor driver and is very suitable for jump-starting your FOC experience. Here we the velocity control example project using the <span class="simple">Simple<span class="foc">FOC</span>library</span> and this hardware:

[Arduino UNO](https://store.arduino.cc/arduino-uno-rev3) | [Drotek L6234 breakout board](https://store-drotek.com/212-brushless-gimbal-controller-l6234.html) | [AMT 103 encoder](https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduiAsBlScvLoAWHUnKz39jAIpNPVt58AQ0PVb84dpbt53g%3D%3D) | [IPower GBM8017-120T](https://fr.aliexpress.com/item/32483131130.html?spm=a2g0o.productlist.0.0.6ddd749fFd3u9E&algo_pvid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb&algo_expid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb-10&btsid=0b0a187915885172220541390e7eed&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
--- | --- | --- | --- 
<img src="extras/Images/arduino_uno.jpg" class="imgtable150"> |  <img src="extras/Images/l6234.jpg" style="width:140px">  | <img src="extras/Images/enc1.png" class="imgtable150">  | <img src="extras/Images/big.jpg" class="imgtable150"> 


# Connecting everything together
For a bit more in depth explanation of Arduino UNO and L6234 connection please check the [connection examples](arduino_l6234).
<p><img src="extras/Images/uno_l6234.jpg" class="width60"></p>

### Encoder
- Encoder channels `A` and `B` are connected to the Arduino's external interrupt pins `2` and `3`. 

### L6234 breakout board 
- Connected to the arduino pins `9`,`10` and `11` (you can use also pins `5` and `6`).  
- Additionally you can connect the `enable` pin to the any digital pin of the arduino the picture shows pin `8` but this is optional. You can connect the driver enable directly to 5v. 
- Make sure you connect the common ground of the power supply and your Arduino

### Motor
- Motor phases `a`, `b` and `c` are connected directly to the driver outputs



# Arduino code 
Let's go through the full code for this example and write it together.
First thing you need to do is include the `SimpleFOC` library:

```cpp
#include <SimpleFOC.h>
```
Make sure you have the library installed. If you still don't have it please check the [get started page](installation)

## Encoder code
First we define the `Encoder` class with the A and B channel pins and number of impulses per revolution.
```cpp
// define Encoder
Encoder encoder = Encoder(2, 3, 2048);
```
Then we define the buffering callback functions.
```cpp
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}
```
In the `setup()` function we initialize the encoder and enable interrupts:
```cpp
// initialize encoder hardware
encoder.init();
// hardware interrupt enable
encoder.enableInterrupts(doA, doB);
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


Next we need to define the `BLDCDriver3PWM` class with the PWM pin numbers of the motor and the driver enable pin
```cpp
// define BLDC driver
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 11, 8);
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

// define BLDC motor
BLDCMotor motor = BLDCMotor( 14 );
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 11, 8);

// define Encoder
Encoder encoder = Encoder(2, 3, 2048);
// Interrupt routine initialization
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}


void setup() {
  
  // initialize encoder hardware
  encoder.init();
  // hardware interrupt enable
  encoder.enableInterrupts(doA, doB);

  // power supply voltage
  // default 12V
  driver.voltage_power_supply = 12;
  driver.init();
  // link the motor to the driver
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = ControlType::velocity;

  // velocity PI controller parameters
  // default P=0.5 I = 10
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  //default voltage_power_supply
  motor.voltage_limit = 6;
  
  // velocity low pass filtering
  // default 5ms - try different values to see what is the best. 
  // the lower the less filtered
  motor.LPF_velocity.Tf = 0.01;
  

  // link the motor to the sensor
  motor.linkSensor(&encoder);

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

// velocity set point variable
float target_velocity = 0;

void loop() {
  // iterative foc function 
  motor.loopFOC();

  // iterative function setting and calculating the velocity loop
  // this function can be run at much lower frequency than loopFOC function
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
