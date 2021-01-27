---
layout: default
title: <span class="simple">Simple<span class="foc">FOC</span>Shield</span> 
parent: Setup examples
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 1
permalink: /arduino_simplefoc_shield
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> example
The <span class="simple">Simple<span class="foc">FOC</span>Shield</span>, being Arduino UNO shield, is compatible with any type of board with the same headers.
Essentially, it is a L6234 chip breakout board made in a form of Arduino Shield.

<p><img src="extras/Images/foc_shield_v13.png" class="img400"></p>

## Connector types
 - Terminal connectors 
    - motor phase lines `a`,`b` and `c`
    - power supply cables (12V to 24V)
 - Encoder connector
    - Integrated configurable pull-ups

More information on this link: [Arduino Simple FOC Shield](arduino_simplefoc_shield_showcase).

## Encoder 
- Channels `A` and `B` are connected to the encoder connector `P_ENC`, terminals `A` and `B`. 
- If your encoder has `index` signal you can connect it to the encoder connector as well, terminal `I`.

## Motor
- Motor phases `a`, `b` and `c` are connected directly the motor terminal connector `TB_M1`


## Example connection
<p><img src="extras/Images/foc_shield_v13.jpg" class="width60"></p>
