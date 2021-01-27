---
layout: default
title: STM32 Bluepill
parent: Setup examples
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 5
permalink: /bluepill_connection
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# Stm32 Bluepill using the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
The <span class="simple">Simple<span class="foc">FOC</span>Shield</span>, being a BLDC driver, can be used not just with boards with Arduino UNO headers but also as a stand alone driver board.

Here is an example how to connect Stm32 Bluepill with the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> :

<p><img src="extras/Images/bluepill_foc_shield_v13.png" class="width60"></p>
More information about the [Arduino Simple FOC Shield](arduino_simplefoc_shield_showcase).


<p><img src="extras/Images/bluepill_pinout.jpg" class="img400"></p>

<blockquote class="info"><p class="heading">Bluepill pinout</p> Make sure to consult the pinout before you assign the BLDC driver pins. They have to be PWM capable.</blockquote>


## Encoder 
- Channels `A` and `B` are connected to `PB9` and `PB8`.
- If your encoder has `index` signal you can connect it to the boards as well, for example on `PB7`.
<blockquote class="info"><p class="heading">NOTE</p> Any digital pin can be external interrupt pin on STM32 boards.</blockquote>

## <span class="simple">Simple<span class="foc">FOC</span>Shield</span> connection 
- Connect `PWMa`, `PWMb` and `PWMc` signal to `PA10`, `PA9` and `PA8` of the Bluepill.
- Connect the `enable` pin to any digital pin such as `PA11`
- Connect the common ground `GND`
- Connect the power-supply cable

## Motor
- Motor phases `a`, `b` and `c` are connected directly the motor terminal connector `TB_M1`


## Example connection
<p><img src="extras/Images/bluepill_foc_shield.jpg" class="width60"></p>
