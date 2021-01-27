---
layout: default
title: Drivers
parent: Supported Hardware
nav_order: 2
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /drivers
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
has_children: true
has_toc: false
---

# Supported driver boards

<div class="width60">
<img src="extras/Images/drv8302.png" style="width:25%;display:inline"><img src="extras/Images/bgc_30.jpg" style="width:25%;display:inline"><img src="extras/Images/l6234.jpg" style="width:25%;display:inline"><img src="extras/Images/l298n.jpg" style="width:25%;display:inline">
</div>

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> has a goal to support as many BLDC and stepper motor drivers as possible. Till this moment there are two kinds of motor drivers supported by this library:

- [BLDC motor driver <i class="fa fa-external-link"></i>](bldc_drivers)
    - **3 PWM signals** ( 3 phase )
    - **6 PWM signals** ( 3 phase )
    - gimbal motor drivers or high-performance boards
- [Stepper drivers <i class="fa fa-external-link"></i>](stepper_drivers)
    - **4 PWM signals** ( 2 phase )
    - Stepper drivers or double DC motor drivers

## 📢 Make sure to read this before settling for a driver!
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