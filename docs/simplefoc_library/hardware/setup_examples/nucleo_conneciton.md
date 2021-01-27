---
layout: default
title: STM32 Nucelo-64
parent: Setup examples
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 4
permalink: /nucleo_connection
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# Stm32 Nucleo-64 using the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
The <span class="simple">Simple<span class="foc">FOC</span>Shield</span>, being Arduino UNO shield, is compatible with any type of board with the same headers and one of them are STM32 Nucleo-64 boards.

Here is the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> connection scheme:

<p><img src="extras/Images/foc_shield_v13_nucleo.png" class="img400"></p>

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

<blockquote class="info"><p class="heading">NOTE</p> Any digital pin can be external interrupt pin on STM32 boards.</blockquote>

## Motor
- Motor phases `a`, `b` and `c` are connected directly the motor terminal connector `TB_M1`


## Example connection
<p><img src="extras/Images/nucleo_foc_shield_connection.jpg" class="width60"></p>
