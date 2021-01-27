---
layout: default
title: BLDCDriver
nav_order: 1
permalink: /bldcdriver
parent: Driver code
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
has_children: True
has_toc: False
---

# BLDC Driver configuration

<div class="width60">
<img src="extras/Images/drv8302.png" style="width:25%;display:inline"><img src="extras/Images/bgc_30.jpg" style="width:25%;display:inline"><img src="extras/Images/l6234.jpg" style="width:25%;display:inline">
</div>

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> implements support for two types of BLDC driver control interfaces:<br>
- [3PWM <i class="fa fa-external-link"></i>](bldcdriver3pwm) - class `BLDCMotor3PWM`
- [6PWM <i class="fa fa-external-link"></i>](bldcdriver6pwm) - class `BLDCMotor6PWM`

Classes `BLDCMotor3PWM` and  `BLDCMotor6PWM` provide an abstraction layer of all the hardware/platform specific code for all the supported platforms: atmega328, esp32, stm32 and teensy. 
They implement:
- PWM configuration
    - PWM frequency
    - PWM center-alignment 
    - Complementary channel  (6PWM)
    - Dead zone/dead time  (6PWM)
- PWM duty cycle setting 
- Voltage limiting

These classes can be used as stand-alone classes and they can be used to set certain PWM value to the BLDC driver outputs, see example codes in `utils > driver_standalone_test`.
In order for FOC algorithm to work the `BLDCDriverxPWM` classes are linked to a `BLDCMotor` class which uses the driver to set the appropriate phase voltages.   

The driver code is written in a way to support as many different drivers out there as possible and in a way to be fully interchangeable. 

## Digging deeper
For more theoretical explanations and source code implementations of the FOC algorithm and the motion control approaches check out the [digging deeper section](digging_deeper).
