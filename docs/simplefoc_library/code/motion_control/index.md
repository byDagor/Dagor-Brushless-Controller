---
layout: default
title: Motion Control
parent: Writing the Code
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /motion_control
nav_order: 4
has_children: True
has_toc: False
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Motion control
<span class="simple">Simple<span class="foc">FOC</span>library</span> gives you the choice of using 3 different closed-loop control strategies: 
- [torque control loop using voltage](voltage_loop)
- [position/angle motion control](angle_loop)
- [velocity motion control](velocity_loop)
<blockquote class="info"> Index search uses also uses velocity control loop, but has some additional parameters, see <a href="index_search_loop">index search</a></blockquote>

You set it by changing the `motor.controller` variable. If you want to control the motor angle you will set the `controller` to `ControlType::angle`, if you seek the DC motor behavior behavior by controlling the voltage use `ControlType::voltage`, if you wish to control motor angular velocity `ControlType::velocity`. 

```cpp
// set FOC loop to be used
// ControlType::voltage
// ControlType::velocity
// ControlType::angle
motor.controller = ControlType::angle;
```

Additionally you can run the motor in the open-loop as well:
- [position open-loop control](angle_openloop)
- [velocity open-loop control](velocity_openloop)

```cpp
// ControlType::velocity_openloop    - velocity open-loop control
// ControlType::angle_openloop       - position open-loop control
motor.controller = ControlType::angle_openloop;
```

For more information about the source code implementation of the motion control strategies check the [library source code documentation](motion_control_implementation)