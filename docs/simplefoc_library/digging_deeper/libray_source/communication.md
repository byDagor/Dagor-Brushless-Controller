---
layout: default
title: Motor commands
parent: Library Source
nav_order: 4
permalink: /commands_source
grand_parent: Digging deeper
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Motor commands

This is the full source implementation of the motor commands. You can see it is pretty simple.  

Here is the code, and beneath the code see an [example of adding a new command](#adding-a-new-command-example--disable-motor---d).
```cpp
/**
 * Function setting the configuration parameters  of the motor, target value of the control loop
 * and outputting them to the monitoring port( if available ) :
 * - configure PID controller constants
 * - change motion control loops
 * - monitor motor variables
 * - set target values
 * - check all the configuration values 
 * 
   ...
 * 
 * @param command String containing the user command
 * 
 * returns 0 for error or 1 for executed command
 */
int BLDCMotor::command(String user_command) {
  // error flag
  int errorFlag = 1;
  // if empty string
  if(user_command.length() < 1) return errorFlag;

  // parse command letter
  char cmd = user_command.charAt(0);
  // check if get command
  char GET = user_command.charAt(1) == '\n';
  // parse command values
  float value = user_command.substring(1).toFloat();

  // a bit of optimisation of variable memory for Arduino UNO (atmega328)
  switch(cmd){
    case 'P':      // velocity P gain change
    case 'I':      // velocity I gain change
    case 'D':      // velocity D gain change
    case 'R':      // velocity voltage ramp change
      if(monitor_port) monitor_port->print(" PID velocity| ");
      break;
    case 'F':      // velocity Tf low pass filter change
      if(monitor_port) monitor_port->print(" LPF velocity| ");
      break;
    case 'K':      // angle loop gain P change
      if(monitor_port) monitor_port->print(" P angle| ");
      break;
    case 'L':      // velocity voltage limit change
    case 'N':      // angle loop gain velocity_limit change
      if(monitor_port) monitor_port->print(" Limits| ");
      break;

  }

  // apply the the command
  switch(cmd){
    case 'P':      // velocity P gain change
      if(monitor_port) monitor_port->print("P: ");
      if(!GET) PID_velocity.P = value;
      if(monitor_port) monitor_port->println(PID_velocity.P);
      break;
    case 'I':      // velocity I gain change
      if(monitor_port) monitor_port->print("I: ");
      if(!GET) PID_velocity.I = value;
      if(monitor_port) monitor_port->println(PID_velocity.I);
      break;
    case 'D':      // velocity D gain change
      if(monitor_port) monitor_port->print("D: ");
      if(!GET) PID_velocity.D = value;
      if(monitor_port) monitor_port->println(PID_velocity.D);
      break;
    case 'R':      // velocity voltage ramp change
      if(monitor_port) monitor_port->print("volt_ramp: ");
      if(!GET) PID_velocity.output_ramp = value;
      if(monitor_port) monitor_port->println(PID_velocity.output_ramp);
      break;
    case 'L':      // velocity voltage limit change
      if(monitor_port) monitor_port->print("volt_limit: ");
      if(!GET)voltage_limit = value;
      if(monitor_port) monitor_port->println(voltage_limit);
      break;
    case 'F':      // velocity Tf low pass filter change
      if(monitor_port) monitor_port->print("Tf: ");
      if(!GET) LPF_velocity.Tf = value;
      if(monitor_port) monitor_port->println(LPF_velocity.Tf);
      break;
    case 'K':      // angle loop gain P change
      if(monitor_port) monitor_port->print(" P: ");
      if(!GET) P_angle.P = value;
      if(monitor_port) monitor_port->println(P_angle.P);
      break;
    case 'N':      // angle loop gain velocity_limit change
      if(monitor_port) monitor_port->print("vel_limit: ");
      if(!GET) velocity_limit = value;
      if(monitor_port) monitor_port->println(velocity_limit);
      break;
    case 'C':
      // change control type
      if(monitor_port) monitor_port->print("Control: ");
      
      if(GET){ // if get command
        switch(controller){
          case ControlType::voltage:
            if(monitor_port) monitor_port->println("voltage");
            break;
          case ControlType::velocity:
            if(monitor_port) monitor_port->println("velocity");
            break;
          case ControlType::angle:
            if(monitor_port) monitor_port->println("angle");
            break;
          default:
            if(monitor_port) monitor_port->println("open loop");
        }
      }else{ // if set command
        switch((int)value){
          case 0:
            if(monitor_port) monitor_port->println("voltage");
            controller = ControlType::voltage;
            break;
          case 1:
            if(monitor_port) monitor_port->println("velocity");
            controller = ControlType::velocity;
            break;
          case 2:
            if(monitor_port) monitor_port->println("angle");
            controller = ControlType::angle;
            break;
          default: // not valid command
            errorFlag = 0;
        }
      }
      break;
    case 'V':     // get current values of the state variables
        switch((int)value){
          case 0: // get voltage
            if(monitor_port) monitor_port->print("Uq: ");
            if(monitor_port) monitor_port->println(voltage_q);
            break;
          case 1: // get velocity
            if(monitor_port) monitor_port->print("Velocity: ");
            if(monitor_port) monitor_port->println(shaft_velocity);
            break;
          case 2: // get angle
            if(monitor_port) monitor_port->print("Angle: ");
            if(monitor_port) monitor_port->println(shaft_angle);
            break;
          case 3: // get angle
            if(monitor_port) monitor_port->print("Target: ");
            if(monitor_port) monitor_port->println(target);
            break;
          default: // not valid command
            errorFlag = 0;
        }
      break;
    default:  // target change
      if(monitor_port) monitor_port->print("Target : ");
      target = user_command.toFloat();
      if(monitor_port) monitor_port->println(target);
  }
  // return 0 if error and 1 if ok
  return errorFlag;
}
```

## Adding a new command example : disable motor - `D`.

We will start by adding a new command just create a new case entry before `default:` entry. 

```cpp
  ....
case 'D':

  break;
default:
   ...  
```

Now let's add the disabling functionality:
```cpp
  ....
case 'D':
  disable();
  break;
default:
   ...  
```

And we can add a monitoring output to notify the user that the motor is disabled:
```cpp
  ....
case 'D':
  if(monitor_port) monitor_port->print("Motor disabled!");
  disable();
  break;
default:
   ...  
```
