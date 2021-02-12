---
layout: default
title: Void loop - tab d
parent: Firmware
nav_order: 4
permalink: /void_loop
---

# Void setup

```c++
void loop() {
  timeManagement();
  motor.loopFOC();
  motor.move();

  //Allows commands to be send through the serial monitor
  motor.command(serialReceiveUserCommand());

  // Fixed rate functions
  // Functions inside this "if" will execute at a 5hz rate. Un/ comment the functions that you wish to use.
  if(stateT >= 1000000/fixedFreq){
    stateT = 0;

    // Un/ comment the functions bellow that you wish to use.
    tempStatus();
    faultStatus();
    voltageMonitor();
    //rotorPosition();
  }

}

```
