---
layout: default
title: Motor code
nav_order: 1
parent: Writing the Code
permalink: /motors_config
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
has_children: True
has_toc: False
---

# Motor configuration code

<div class="width60">
<img src="extras/Images/mot2.jpg" style="width:20%;display:inline"><img src="extras/Images/bigger.jpg" style="width:20%;display:inline"><img src="extras/Images/mot.jpg" style="width:20%;display:inline"><img src="extras/Images/nema17_2.jpg" style="width:20%;display:inline"><img src="extras/Images/nema17_1.jpg" style="width:20%;display:inline">
</div>

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> supports two types of BLDC motors:

- [BLDC motors <i class="fa fa-external-link"></i>](bldcmotor) 
  - 3 phase (3 wire):
  - Gimbal and High-performance BLDC motors
- [Stepper motors <i class="fa fa-external-link"></i>](steppermotor) 
  - 2 phase (4 wire)


The motor code is written in a way to support as many different motors out there as possible and in a way to be fully interchangeable. 

## Digging deeper
For more theoretical explanations and source code implementations of the FOC algorithm and the motion control approaches check out the [digging deeper section](digging_deeper).