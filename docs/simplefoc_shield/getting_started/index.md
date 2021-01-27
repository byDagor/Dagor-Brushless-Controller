---
layout: default
title: Getting Started
description: "Getting started with Arduino SimpleFOCShield"
parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
nav_order: 1
permalink: /arduino_simplefoc_shield_installation
has_children: true
has_toc: false
---
# Getting Started with Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>  

<img src="extras/Images/simple_foc_shield_v13_small.gif" class="width50">

## Step 1: [Hardware configuration](pads_soldering) 
One of the very important features of the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is the hardware configuration. 

<img src="extras/Images/shield_bot_v131_pinout.gif" class="width40">

Each board has a set of solder pads on the bottom side which are used for configuration. These solder pads enable board to:
- Enable / Disable pull-up resistors for encoder A,B and Index channel
- Configure BLDC driver pinout (PWM pins A,B,C and enable pin)

[Read more](pads_soldering)



## Step 2: [Connecting the hardware](foc_shield_connect_hardware)

<img src="extras/Images/connection.gif" class="width50">

Connecting the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> to the microcontroller, BLDC motor, power-supply and the sensors is very straight forward. 

[Read more](foc_shield_connect_hardware)

## Step 3: [Writing the code](foc_shield_code)

Once when you have decided the appropriate [hardware configuration](pads_soldering) of yous board and once you have all the hardware [ready to be connected](foc_shield_connect_hardware), we can start the most exciting part, coding!

[Read more](foc_shield_code)