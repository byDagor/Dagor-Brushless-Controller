---
layout: default
title: Motors
nav_order: 1
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /motors
parent: Supported Hardware
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
has_children: true
has_toc: false
---

# Supported BLDC motors

<div class="width60">
<img src="extras/Images/mot2.jpg" style="width:20%;display:inline"><img src="extras/Images/bigger.jpg" style="width:20%;display:inline"><img src="extras/Images/mot.jpg" style="width:20%;display:inline"><img src="extras/Images/nema17_2.jpg" style="width:20%;display:inline"><img src="extras/Images/nema17_1.jpg" style="width:20%;display:inline">
</div>

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> supports two types of BLDC motors:

- [BLDC motors <i class="fa fa-external-link"></i>](bldc_motors) 
  - 3 phase (3 wire):
  - Gimbal and High-performance BLDC motors
- [Stepper motors <i class="fa fa-external-link"></i>](stepper_motors) 
  - 2 phase (4 wire)

## 📢 Make sure to read this before running any motor!
Before running any BLDC motor with the <span class="simple">Simple<span class="foc">FOC</span>library</span> please make sure your hardware can handle the currents your motor requires. 

The simplest way to do it is by checking the motor phase resistance `R`. Either check the datasheet of your motor and search for the resistance value or measure it yourself using a multimeter. Then check the value of your power supply voltage `V_dc` and once when you have the values you can find the maximum current `I_max` value by calculating:
```cpp
I_max = V_dc/R
```
Finally check the value of the maximum current `I_max` with the datasheet of your driver board. If the `I_max` is too high you can lower the power supply voltage `V_dc` in order prevent too high peaks of the current. If you are not able to change your power supply voltage you can limit the voltage set to motor in software. 
<blockquote class="warning">
    <p class="heading">NOTE</p>
    The equation above calculates the worst case maximum current <code class="highlighter-rouge">I_max</code> and in most cases calculated <code class="highlighter-rouge">I_max</code> is higher than the actual value. Maximum current depends both of the motor hardware such as winding configuration and the control algorithm.  
</blockquote>

