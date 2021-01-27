---
layout: default
title:  Position Control Nucleo-64
parent: Example projects
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 5
permalink: /position_control_nucleo_example
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> 
---


# Position control example<br>using <span class="simple">Simple<span class="foc">FOC</span>Shield</span> and Stm32 Nucleo-64
For this BLDC motor position control example we are going to be using this hardware:

[Stm32 Nucleo-64](https://www.mouser.fr/ProductDetail/STMicroelectronics/NUCLEO-F446RE?qs=%2Fha2pyFaduj0LE%252BzmDN2WNd7nDNNMR7%2Fr%2FThuKnpWrd0IvwHkOHrpg%3D%3D) | [Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>](arduino_simplefoc_shield_showcase) | [AMT 103 encoder](https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduiAsBlScvLoAWHUnKz39jAIpNPVt58AQ0PVb84dpbt53g%3D%3D) | [GBM5108-120T](https://www.onedrone.com/store/ipower-gbm5108-120t-gimbal-motor.html)
--- | --- | --- | --- 
<img src="extras/Images/nucleo.jpg" class="imgtable150"> |  <img src="extras/Images/shield_to_v13.jpg" class="imgtable150">  | <img src="extras/Images/enc1.png" class="imgtable150">  | <img src="extras/Images/bigger.jpg" class="imgtable150"> 


# Connecting everything together
For a bit more in depth explanation of how to connect Nucleo board and <span class="simple">Simple<span class="foc">FOC</span>Shield</span> connection please check the [connection examples](nucleo_connection).
<p><img src="extras/Images/nucleo_foc_shield_connection.jpg" class="width60"></p>

For more information about the <span class="simple">Simple<span class="foc">FOC</span>Shield</span> check the [docs](arduino_simplefoc_shield_showcase).

## Encoder 
- Channels `A` and `B` are connected to the encoder connector `P_ENC`, terminals `A` and `B`. 
- Index channel you can connect directly to the `P_ENC` as well to the terminal `I`
## Motor
- Motor phases `a`, `b` and `c` are connected directly the motor terminal connector `TB_M1`


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
Encoder encoder = Encoder(A1, A2, 2048, A0);
```
Then we define the buffering callback functions.
```cpp
// channel A, B and index callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}
void doI(){encoder.handleIndex();}
```
In the `setup()` function we initialize the encoder and enable interrupts:
```cpp
// initialize encoder hardware
encoder.init();
// hardware interrupt enable
encoder.enableInterrupts(doA, doB, doI);
```
And that is it, let's setup the motor.

<blockquote class="info">For more configuration parameters of the encoders please check the <code class="highlighter-rouge">Encoder</code> class <a href="encoder">docs</a>.</blockquote>


## Motor code
First we need to define the `BLDCMotor` class with the  number od pole pairs (`11`)
```cpp
// define BLDC motor
BLDCMotor motor = BLDCMotor(11);
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
```oltage_power_supply = 12;
```
Next thing we can change is the index search velocity:
```cpp
// index search velocity
// default 1 rad/s
motor.velocity_index_search = 3;
```

Then we tell the motor which control loop to run by specifying the `motor.controller` variable.
```cpp
// set control loop type to be used
// ControlType::voltage
// ControlType::velocity
// ControlType::angle
motor.controller = ControlType::angle;
```
Now we configure the velocity PI controller parameters
```cpp
// velocity PI controller parameters
// default P=0.5 I = 10
motor.PID_velocity.P = 0.2;
motor.PID_velocity.I = 20;
```
Additionally we can configure the Low pass filter time constant `Tf`
```cpp
// velocity low pass filtering
// default 5ms - try different values to see what is the best. 
// the lower the less filtered
motor.LPF_velocity.Tf = 0.01;
```
Finally we configure position P controller gain and the velocity limit variable.
```cpp
// angle P controller 
// default P=20
motor.P_angle.P = 20;
//  maximal velocity of the position control
// default 20
motor.velocity_limit = 20;
```
<blockquote class="info">For more information about the angle control loop parameters please check the  <a href="angle_loop">doc</a>.</blockquote>

Next we connect the encoder and the driver to the motor, do the hardware init and init of the Field Oriented Control.
```cpp  
// link the motor to the sensor
motor.linkSensor(&encoder);
// link the motor to the driver
motor.linkDriver(&driver);

// initialize motor
motor.init();
// align encoder and start FOC
motor.initFOC();

// initial target value
motor.target = 0;
```
The last peace of code important for the motor is of course the FOC routine in the `loop` function.
```cpp
void loop() {
// iterative FOC function
motor.loopFOC();

// iterative function setting and calculating the angle/position loop
// this function can be run at much lower frequency than loopFOC function
motor.move();
}
```
That is it, that is the full code for the motor, FOC and motion control initialization and configuration. Let's enable user communication now.
<blockquote class="info">For more configuration parameters and control loops please check the <code class="highlighter-rouge">BLDCMotor</code> class <a href="motors_config">doc</a>.</blockquote>

## User communication
At the end the Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> enables you to change all the configuration parameters in real-time as well as read the motor state variables, and set the target values by using [motor commands interface](communication).

In order to enable it we need to first enable monitoring:
```cpp  
Serial.begin(115200);
// enable monitoring functionality
motor.useMonitoring(Serial);
```
Then we need a small function to parse user input from the serial terminal such as:
```cpp
// utility function enabling serial communication the user
String serialReceiveUserCommand() {
  
  // a string to hold incoming data
  static String received_chars;
  
  String command = "";

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;

    // end of user input
    if (inChar == '\n') {
      
      // execute the user command
      command = received_chars;

      // reset the command buffer 
      received_chars = "";
    }
  }
  return command;
}
```
And at the end we can make motor follow the user commands by adding this line to the `loop()`:
```cpp
// user communication
motor.command(serialReceiveUserCommand());
``` 
And that is it, we everything configured and ready to go, let's see the full code!

For more info about the [monitoring](monitoring) and [motor commands](communicaiton) visit the [Writing the Code section](code).

## Full Arduino code

```cpp
#include <SimpleFOC.h>

// init BLDC motor
BLDCMotor motor = BLDCMotor( 11 );
// init driver
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 11, 8);
//  init encoder
Encoder encoder = Encoder(2, 3, 2048);
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}

void setup() {

  // initialize encoder hardware
  encoder.init();
  // hardware interrupt enable
  encoder.enableInterrupts(doA, doB);
  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // power supply voltage
  // default 12V
  driver.voltage_power_supply = 12;
  driver.init();
  // link the motor to the driver
  motor.linkDriver(&driver);

  // index search velocity
  // default 1 rad/s
  motor.velocity_index_search = 3;

  // set control loop to be used
  motor.controller = ControlType::angle;
  
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

  // angle P controller 
  // default P=20
  motor.P_angle.P = 20;
  //  maximal velocity of the position control
  // default 20
  motor.velocity_limit = 20;
  
  // monitoring port
  Serial.begin(115200);
  // enable monitoring functionality
  motor.useMonitoring(Serial);

  // initialize motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // initial angle target
  // it will be changed by the motor.command() function
  motor.target = 0;

  Serial.println("Motor ready.");
  Serial.println("Set the target angle using serial terminal:");
  _delay(1000);
}

void loop() {
  // iterative FOC function
  motor.loopFOC();

  // position motion control loop
  motor.move();
    
  // user communication
  motor.command(serialReceiveUserCommand());

}

// utility function enabling serial communication the user
String serialReceiveUserCommand() {
  
  // a string to hold incoming data
  static String received_chars;
  
  String command = "";

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;

    // end of user input
    if (inChar == '\n') {
      
      // execute the user command
      command = received_chars;

      // reset the command buffer 
      received_chars = "";
    }
  }
  return command;
}
```