---
layout: default
title: Stepper motors
nav_order: 2
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /stepper_motors
parent: Motors
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Stepper motors 

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> supports most of common stepper motors out there. 

Stepper motors are a subset of BLDC motors, and these are some of their characteristics:
 - 2 phase motors - 4 wires
 - Relatively high number of pole pairs >50
 - Internal resistance generally >5Ω
 - Currents generally under <5A

Stepper motors are one of the most common hobby motors. They are everywhere. The main benefits of using stepper motors are :
- Price - *both motors and drivers are quiet cheap*
- Simplicity of usage - *direction and step interface*
- Good performance - *high holding torque, velocity up to 1000rpm*

## Benefits of using Field Oriented Control with stepper motors
The main problem of using the stepper motors in open-loop (without position sensor) control mode is that we are obliged to neglect all the dynamics the motor and its load have. We assume that the motor is capable of obeying any command we might have. Therefore, when using the stepper motor we often underestimate the motor's abilities in order to be sure that the motor can produce all the movements we ask it to do.
This can be easily seen in 3D printers for example, if you set the speed of the printer's movements to be too high, the motor will start skipping (missing steps or not being able to follow) and it will ruin your print. Therefore printers often have a feed-rate factor, which enables you to slow down your instructions and avoid the movements that your motor will not be able to produce. 

Additional problem of the open-loop control of stepper motors is not knowing where the motor is, what makes the control algorithm unable to respond to the disturbances from the environment. Again in 3D printing this problem is well known as losing steps. If the nozzle of the 3D printer gets stuck for a second, the 3d printer will not know it and it will continue sending the commands to the motor as if the motor has been following the whole time. This often results by the ruined print. 

So, some of the benefits of using the FOC algorithm with the stepper motors are:
- Better dynamics
  - Proper choice of stepper driver + position sensor + microcontroller necessary
- No missing steps
  - Disturbance rejection
- Much more energy efficient 
  - Voltage set to the motor is modulated to minimize the positioning error - (usually fixed )
- **Backdrivable**

## Some supported motors 

Here are some examples of the different price range gimbal motors that have been successfully tested with this library.

Examples | Description | Specifications | Link | Price 
---- | ---- | ---- | ---- | ----
[<img src="extras/Images/nema14.jpg" style="height:100px">](https://www.ebay.com/itm/New-Geeetech-Nema14-35-BYGHW-stepper-motor-for-3d-printer-Reprap-Prusa/272847009701) | NEMA14 BYGHW |  - 50PP (200 steps) <br> - 18N.cm  <br> - 12V/1.2A <br> - 8.8Ω <br> - 35x35x35mm| [Ebay](https://www.ebay.com/itm/New-Geeetech-Nema14-35-BYGHW-stepper-motor-for-3d-printer-Reprap-Prusa/272847009701) | 10$
[<img src="extras/Images/nema17_1.jpg" style="height:100px">](https://www.ebay.com/itm/NEMA-17-Stepper-Motor-12V-0-4A-for-CNC-Reprap-3D-Printer-Extruder-36oz-in-26Ncm/401853894019?hash=item5d905bcd83:g:u04AAOSwRBFdp-IP) | NEMA17 42BYGH34-0400A |  - 50PP (200 steps) <br> - 26N.cm  <br> - 12V/0.4A  <br> - 30Ω <br> - 42x42x34mm| [Ebay](https://www.ebay.com/itm/NEMA-17-Stepper-Motor-12V-0-4A-for-CNC-Reprap-3D-Printer-Extruder-36oz-in-26Ncm/401853894019?hash=item5d905bcd83:g:u04AAOSwRBFdp-IP) | 12$
 [<img src="extras/Images/nema17_2.jpg" style="height:100px">](https://www.ebay.com/itm/Nema-17-Stepper-Motor-Bipolar-2A-59Ncm-83-6oz-in-48mm-Body-4-lead-3D-Printer-CNC/282285186801?hash=item41b9821ef1:g:7dUAAOSwEzxYSl25) | NEMA 17HS19-2004S1 |  - 50PP (200 steps) <br> - 59N.cm  <br> - 12V/2A  <br> - 4Ω<br> - 42x42x48mm| [Ebay](https://www.ebay.com/itm/Nema-17-Stepper-Motor-Bipolar-2A-59Ncm-83-6oz-in-48mm-Body-4-lead-3D-Printer-CNC/282285186801?hash=item41b9821ef1:g:7dUAAOSwEzxYSl25) | 17$
 [<img src="extras/Images/nema23.jpg" style="height:100px">](https://www.ebay.com/itm/Nema-23-Stepper-Motor-4-Wire-Bipolar-2-Phase-1-8-57BYGH-for-CNC-3D-Printer/372619819064) | NEMA23 57BYGH  | - 50PP (200 steps) <br> - >100N.cm <br> - 12V/2.5A  <br> - 1.2Ω <br> - 56x56x56mm| [Ebay](https://www.ebay.com/itm/Nema-23-Stepper-Motor-4-Wire-Bipolar-2-Phase-1-8-57BYGH-for-CNC-3D-Printer/372619819064) | 30$
