---
layout: default
title: Writing the Code
nav_order: 6
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /code
has_children: True
has_toc: False
parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> 
---

# Let's write the code

Once when you have your <span class="simple">Simple<span class="foc">FOC</span>library</span> [installed](installation) and you have all the necessary [hardware](supported_hardware),we can finally start the fun part, let's white the code and move the motor!

## Step 0. Include the library

Let's start by including the library header file:
```cpp
#include <SimpleFOC.h>
```
## Step 1. <a href="sensors" class="remove_dec">Position sensor setup</a>

First step when writing the code is initializing and configuring the position sensor.
The library supports these position sensors:
 - [Encoders](encoder): Optical, Capacitive, Magnetic encoders (ABI)
 - [Magnetic sensors](magnetic_sensor): SPI, I2C or Analog
 - [Hall sensors](hall_sensors): 3xHall sonde, Magnetic sensor (UVW interface) 

Choose position sensor to use with this example:

<a href ="javascript:showMagnetic();" id="mag" class="btn btn-primary">Magnetic sensor</a> <a href="javascript:showEncoder();" id="enc" class="btn">Encoder</a> 

```c
#include <SimpleFOC.h>

// Encoder(pin_A, pin_B, PPR)
Encoder sensor = Encoder(2, 3, 2048);
// channel A and B callbacks
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}

 
void setup() {  
  // initialize encoder hardware
  sensor.init();
  // hardware interrupt enable
  sensor.enableInterrupts(doA, doB);

}

void loop() {
  
}
```

```c++
#include <SimpleFOC.h>

// SPI example
// MagneticSensorSPI(int cs, float bit_resolution, int angle_register)
MagneticSensorSPI sensor = MagneticSensorSPI(10, 14, 0x3FFF);

void setup() {
  // initialize magnetic sensor hardware
  sensor.init();
}

void loop() {

}
```

<div id="enc_p" class="hide_p">
Encoders as position sensors are implemented in the class <code class="highlighter-rouge">Encoder</code> and are defined by its:
  <ul>
    <li> <code class="highlighter-rouge">A</code> and <code class="highlighter-rouge">B</code> channel pin numbers: <code class="highlighter-rouge">2</code> and <code class="highlighter-rouge">3</code></li>
    <li> Encoder  <code class="highlighter-rouge">PPR</code> (impulses per revolution number): <code class="highlighter-rouge">2048</code></li>
    <li> <code class="highlighter-rouge">Index</code> pin number <i>(optional)</i> </li>
  </ul> 
</div>

<div id="mag_p" class="hide_p">
In this example we will be using teh setup of a 14 bit magnetic sensor such as <a href="https://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D">AS5047u <i class="fa fa-external-link"></i></a>, connected to the pin <code class="highlighter-rouge">10</code>.<br>
Magnetic sensors using the SPI communication are implemented in the class <code class="highlighter-rouge">MagneticSensorSPI</code>and are defined by its
  <ul>
    <li><code class="highlighter-rouge">chip_select</code> pin: <code class="highlighter-rouge">10</code> </li>
    <li> the overall <code class="highlighter-rouge">CPR</code> of the sensor:   <code class="highlighter-rouge">CPR = 2^14bit =16384</code></li>
    <li> <code class="highlighter-rouge">angle</code> SPI register: <code class="highlighter-rouge">0x3FFF</code></li> 
  </ul>
</div>

Sensor is initialized hardware pins by running `sensor.init()`.

For full documentation of the setup and all configuration parameters please visit the <a href="sensors"> position sensors docs <i class="fa fa-external-link"></i></a>.


## Step 2. <a href="drivers_config" class="remove_dec"> Driver setup</a>
After the position sensor we proceed to initializing and configuring the driver. The library supports BLDC drivers handled by `BLDCDriver3PWM` and `BLDCDriver6PWM`  classes as well as the stepper drivers handled by `StepperDriver4PWM` class. 

`BLDCDriver3PWM` class instantiated by providing:
- phase `A`, `B` and `C` pin number
- `enable` pin number *(optional)*

For example:
```cpp
#include <SimpleFOC.h>

//  BLDCDriver3PWM( pin_pwmA, pin_pwmB, pin_pwmC, enable (optional))
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);

// instantiate sensor 

void setup() {  

  // init sensor

  // power supply voltage
  driver.voltage_power_supply = 12;
  // driver init
  driver.init();

}

void loop() {

}
```


For full documentation of the setup and all configuration parameters please visit the <a href="drivers_config"> driver docs <i class="fa fa-external-link"></i></a>.



## Step 2. <a href="motors_config" class="remove_dec"> Motor setup </a>
After the position sensor and the driver we proceed to initializing and configuring the motor. The library supports BLDC motors handled by `BLDCMotor` class as well as the stepper motors handled by `StepperMotor` class. Both classes are instantiated by providing just the `pole_pairs` number of the motor

```cpp
// StepperMotor(int pole_pairs)
StepperMotor motor = StepperMotor(50);
```
```cpp 
// BLDCMotor(int pole_pairs)
BLDCMotor motor = BLDCMotor(11);
```


In this example we will use BLDC motor:
```cpp
#include <SimpleFOC.h>

//  BLDCMotor( int pole_pairs )
BLDCMotor motor = BLDCMotor( 11);
 
// instantiate driver

// instantiate sensor   

void setup() {  
  // init sensor
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // init driver
  // link the motor to the driver
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = ControlType::velocity;
  // initialize motor
  motor.init();

}

void loop() {

}
```

After the instance of the motor `motor` has been created we need to link the motor with the sensor `motor.linkSensor()` and link the motor class to the driver it is connected to `motor.linkDriver()`.  <br>
The next step is the configuration step, for the sake of this example we will configure only the motion control loop we will be using:
```cpp
// set control loop type to be used
motor.controller = ControlType::velocity;
```
And to finish the `motor` setup we run the `motor.init()` function.

For full documentation of the setup and all configuration parameters please visit the <a href="motors_config"> motor docs <i class="fa fa-external-link"></i></a>.


## Step 3. FOC routine and real-time motion control
Once when we have initialized the position sensor, driver and the motor, and before we can run the FOC algorithm we need to align the motor and sensor. This is done by calling `motor.initFOC()`. 
After this step we have a functional position sensor, we have configured motor and our FOC algorithm knows how to set the appropriate voltages based on position sensor measurements.

For the real-time routine of the FOC algorithm we need to add the `motor.loopFOC()` and `motor.move(target)` functions in the Arduino `loop()`.
- `motor.loopFOC()`:  FOC algorithm execution - should be executed as fast as possible `> 1kHz`
- `motor.move(target)`: motion control routine - depends of the `motor.controller` parameter

Here is how it looks in code:

```cpp
#include <SimpleFOC.h>

// instantiate motor
// instantiate driver
// instantiate senor

void setup() {  
  
  // init sensor
  // link motor and sensor

  // init driver
  // link motor and driver

  // configure motor
  // init motor

  // align encoder and start FOC
  motor.initFOC();
}

void loop() {
  // FOC algorithm function
  motor.loopFOC();

  // velocity control loop function
  // setting the target velocity or 2rad/s
  motor.move(2);
}
```

For full documentation of the setup and all configuration parameters for BLDC motors please visit the <a href="bldcmotor"> BLDCMotor docs  <i class="fa fa-external-link"></i></a>, and for Stepper motors please visit the <a href="steppermotor"> StepperMotor docs  <i class="fa fa-external-link"></i></a>


## Step 4. <a href="monitoring" class="remove_dec"> Monitoring</a>

`BLDCMotor` and `StepperMotor` classes provide monitoring functionality. For enabling the monitoring feature make sure you call `motor.useMonitoring()` with the `Serial` port instance you want to output to. It uses `Serial` class to output motor initialization status during the `motor.init()` function, as well as in `motor.initFOC()` function.

If you are interested to output motors state variables in real-time (even though it will impact the performance - writing the Serial port is slow!) add the `motor.monitor()` function call to the Arduino `loop()` function. 

```cpp
#include <SimpleFOC.h>

// instantiate motor
// instantiate driver
// instantiate senor

void setup() {  
  
  // init sensor
  // link motor and sensor

  // init driver
  // link motor and driver

  // use monitoring with the BLDCMotor
  Serial.begin(115200);
  // monitoring port
  motor.useMonitoring(Serial);
  
  // configure motor
  // init motor
  
  // align encoder and start FOC
}

void loop() {
  
  // FOC execution
  // motion control loop

  // monitoring function outputting motor variables to the serial terminal 
  motor.monitor();
}
```
For full documentation of the setup and all configuration parameters please visit the <a href="monitoring"> Monitoring docs</a>.


## Step 4. <a href="communication" class="remove_dec"> Motor Commands</a>

Finally in order to configure the control algorithm, set the target values and get the state variables in the user-friendly way (not just dumping as using `motor.monitor()`)  Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>  provides you with the communication interface in a form of motor commands.Very similar to G code.

The communication interface is implemented in the `motor.command()` function. It receives `String` with the user command. 

The following code is one basic implementations of the full communication interface with the user:

```cpp
#include <SimpleFOC.h>

// instantiate motor
// instantiate senor

void setup() {  
  
  // init senor

  // link motor and sensor
  // configure motor

  // use monitoring
  Serial.begin(115200);
  // monitoring port
  motor.useMonitoring(Serial);
  
  // init motor
  
  // align encoder and start FOC
}

void loop() {
  
  // FOC execution
  // motion control loop
  
  
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
For full documentation of the setup and all configuration parameters please visit the <a href="communication"> Communication docs</a>. 


<script type="text/javascript">
    hideClass('language-c');
    document.getElementById("enc_p").style.display = "none";

    function showMagnetic(){
        document.getElementById("enc").classList.remove("btn-primary");
        document.getElementById("mag").classList.add("btn-primary");
        hideClass('language-c');
        showClass('language-c++');
        hideClass('hide_p');
        document.getElementById("mag_p").style.display = "block";


        return 0;
    }

    function showEncoder(){
        document.getElementById("mag").classList.remove("btn-primary");
        document.getElementById("enc").classList.add("btn-primary");
        showClass('language-c');
        hideClass('language-c++');
        hideClass('hide_p');
        document.getElementById("enc_p").style.display = "block";

        return 0;
    }

  function hideClass(class_name){
    var elems = document.getElementsByClassName(class_name);
    for (i = 0; i < elems.length; i++) {
        elems[i].style.display = "none";
    }
  }
  function showClass(class_name){
    var elems = document.getElementsByClassName(class_name);
    for (i = 0; i < elems.length; i++) {
        elems[i].style.display = "block";
    }
  }

</script>


## 🎨 Full Arduino code of the example 

Now when you have learned what are all the parts of the Arduino program and what are they for, here is the full code example with some additional configuration. Please go through the code to better understand how to integrate all previously introduced parts together. This is the code of the library example `motor_full_control_serial_examples/magnetic_sensor/full_control_serial.ino`. 

```cpp
#include <SimpleFOC.h>

// magnetic sensor instance - SPI
MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, 10);

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);

void setup() {

  // initialise magnetic sensor hardware
  sensor.init();
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link driver
  motor.linkDriver(&driver);



  // motor config
  // set control loop type to be used
  motor.controller = ControlType::velocity;

  // contoller configuration based on the control type 
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
  motor.PID_velocity.D = 0;
  // default voltage_power_supply
  motor.voltage_limit = 12;

  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle loop controller
  motor.P_angle.P = 20;
  // angle loop velocity limit
  motor.velocity_limit = 50;

  // use monitoring with serial for motor init
  // monitoring port
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // motor init
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // Run user commands to configure and the motor (find the full command list in docs.simplefoc.com)
  Serial.println("Motor commands sketch | Initial motion control > velocity : target 0 rad/s.");
  
  _delay(1000);
}


void loop() {
  // iterative setting FOC phase voltage
  motor.loopFOC();

  // iterative function setting the outer loop target
  // velocity, position or voltage
  // if tatget not set in parameter uses motor.target variable
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

<h2> Library source code</h2>
If you are interested in extending and adapting the <span class="simple">Simple<span class="foc">FOC</span>library</span> source code you can find full documentation on <a href="source_code">library source docs</a>