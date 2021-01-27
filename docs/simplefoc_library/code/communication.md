---
layout: default
title: Motor commands
parent: Writing the Code
nav_order: 6
permalink: /communication
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# Motor commands

Motor commands interface is implemented in the function `motor.command()`, and it should receive a `String` with the desired user command:
```cpp
motor.command(user_command);
```
Using this function, user can set the configuration parameters of the motor, target value of the control loop and read them by outputting them to the monitoring port( if available ) :
- configure PID controller constants
- change motion control loops
- monitor motor variables
- set target values
- check all the configuration values 

## List of motor commands:
- **P**: velocity PID controller P gain
- **I**: velocity PID controller I gain
- **D**: velocity PID controller D gain
- **R**: velocity PID controller voltage ramp
- **F**: velocity Low pass filter time constant
- **K**: angle P controller P gain
- **N**: global velocity limit
- **L**: global voltage limit
- **C**: control loop 
  - **0**: voltage 
  - **1**: velocity 
  - **2**: angle
- **V**: get motor variables
  - **0**: currently set voltage
  - **1**: current velocity
  - **2**: current angle
  - **3**: current target value

Here is an example of user communication: 
```sh
$ P
PI velocity P: 0.20
$ P1.2
PI velocity P: 1.20
$ I
PI velocity I: 20.00
$ N
P angle velocity limit: 50.00
$ C
Controller type: voltage
$ C2
Controller type: angle
$ C1
Controller type: velocity
$ 2
Target : 2.00
$ V2 
Velocity: 2.12
$ V1
Velocity: 1.76
$ V1
Velocity: 2.22
$ V1
Velocity: 1.97
$ V2 
Angle: 15.52
```
To read a configuration value just enter the command letter:
For example: 
- to read velocity PI controller P gain run: `P`
- to read velocity PI controller I gain run: `I`
- to read current control loop run: `C`

To set the configuration value just enter first the command letter and value:
For example: 
- to set velocity PI controller P gain to 1.2 run: `P1.2`
- to set velocity PI controller voltage limit to 1.5V run: `L1.5`
- to set the control loop to velocity run: `C1`

To change the target value just enter a number in the terminal:
For example: 
- to change the target value to -0.1453 enter: `-0.1453`

To read the motor state variables enter `V` and the number:
For example:
  - to read current voltage set to motor write: `V0`
  - to read current velocity run: `V1`
  - to read current angle run: `V2`
  - to read current target value: `V3`


## Serial communication implementation example
One of the ways to get the user command is by using the `Serial` communication for example:
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

Then you need to do two things: first add enable monitoring in `setup()` function:
```cpp
motor.useMonitoring(Serial);
```
and second add to the Arduino `loop()` function:
```cpp
// user communication
motor.command(serialReceiveUserCommand());
```



## Example code using the motor commands

```cpp
#include <SimpleFOC.h>

// magnetic sensor instance
MagneticSensorSPI AS5x4x = MagneticSensorSPI(10, 14, 0x3FFF);

// motor instance
StepperMotor motor = StepperMotor(50);
// driver instance
StepperDriver4PWM driver = StepperDriver4PWM(9, 10, 5, 6);

void setup() {

  // initialize magnetic sensor hardware
  AS5x4x.init();
  // link the motor to the sensor
  motor.linkSensor(&AS5x4x);

  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = ControlType::voltage;

  // controller configuration based on the control type 
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 20;
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

  // initialize motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // set the initial target value
  // this value will be changed by the motor.command() function
  motor.target = 2;


  Serial.println("Full control example: ");
  Serial.println("Run user commands to configure and the motor (find the full command list in docs.simplefoc.com) \n ");
  Serial.println("Initial motion control loop is voltage loop.");
  Serial.println("Initial target voltage 2V.");
  
  _delay(1000);
}


void loop() {
  // setting FOC phase voltage
  motor.loopFOC();

  // motion control execution
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

## Extending the command list

Check the library source code to find out how to extend the command list [ library source code](source_code) and [motor commands source](commands_source)