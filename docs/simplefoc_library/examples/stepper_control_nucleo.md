---
layout: default
title:  Stepper Motor Control Nucleo-64
parent: Example projects
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 8
permalink: /stepper_control_nucleo
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> 
---


# Stepper motor control example<br>using L298N and Stm32 Nucleo-64
For this stepper motor control example we are going to be using this hardware:

[Stm32 Nucleo-64](https://www.mouser.fr/ProductDetail/STMicroelectronics/NUCLEO-F446RE?qs=%2Fha2pyFaduj0LE%252BzmDN2WNd7nDNNMR7%2Fr%2FThuKnpWrd0IvwHkOHrpg%3D%3D) | [L298N driver](https://www.ebay.com/itm/L298N-DC-Stepper-Motor-Driver-Module-Dual-H-Bridge-Control-Board-for-Arduino/362863436137?hash=item547c58a169:g:gkYAAOSwe6FaJ5Df) | [AMT 103 encoder](https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduiAsBlScvLoAWHUnKz39jAIpNPVt58AQ0PVb84dpbt53g%3D%3D) | [NEMA 17](https://www.ebay.com/itm/Nema-17-Stepper-Motor-Bipolar-2A-59Ncm-83-6oz-in-48mm-Body-4-lead-3D-Printer-CNC/282285186801?hash=item41b9821ef1:g:7dUAAOSwEzxYSl25)
--- | --- | --- | --- 
<img src="extras/Images/nucleo.jpg" class="imgtable150"> |  <img src="extras/Images/l298n.jpg" class="imgtable150">  | <img src="extras/Images/enc1.png" class="imgtable150">  | <img src="extras/Images/nema17_2.jpg" class="imgtable150"> 

# Connecting everything together

Here is an example of the connection scheme using the L298N and Nucleo-64: 

<p><img src="extras/Images/stepper_connection.png" class="img400"></p>

## L298N
- Channels `ENA` and `ENB` are connected to pins `7` and `8`
- Channels `IN1`, `IN2`, `IN3` and `IN4` are connected to the pins `5`, `6`, `9`, `10`
- Common ground is connected in between nucleo and L298N
- 12V power-supply is connected directly to the driver

## Encoder 
- Channels `A` and `B` are connected to the pins `A0` qnd `A1` 
- Index channel is not used in this example but you cqn easily modify this example to support it

## Motor
- Motor phases `A1`, `A2`, `B1`and `B2` are connected directly the motor connectors of the L298N chip.



# Full Arduino code

```cpp
#include <SimpleFOC.h>

// Stepper motor instance
StepperMotor motor = StepperMotor(50);
// Stepper driver instance
StepperDriver4PWM driver = StepperDriver4PWM(5, 6, 9, 10,  8, 7);

// encoder instance
Encoder encoder = Encoder(A1, A2, 2048);

// Interrupt routine intialisation
// channel A and B callbacks
void doA(){encoder.handleA();}
void doB(){encoder.handleB();}

void setup() {

  // initialize encoder sensor hardware
  encoder.init();
  encoder.enableInterrupts(doA, doB); 
  // link the motor to the sensor
  motor.linkSensor(&encoder);

  // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  driver.init();
  // link the motor to the sensor
  motor.linkDriver(&driver);

  // set control loop type to be used
  motor.controller = ControlType::voltage;

  // contoller configuration based on the controll type 
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

  // initialise motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();

  // set the inital target value
  motor.target = 2;


  // Run user commands to configure and the motor (find the full command list in docs.simplefoc.com)
  Serial.println("Motor commands sketch | Initial motion control > torque/voltage : target 2V.");
  
  _delay(1000);
}


void loop() {
  // iterative setting FOC phase voltage
  motor.loopFOC();

  // iterative function setting the outter loop target
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