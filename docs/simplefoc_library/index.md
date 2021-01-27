---
layout: default
title: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 3
permalink: /arduino_simplefoc_library_showcase
has_children: True
has_toc: False
---


# Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>

![Library Compile](https://github.com/simplefoc/Arduino-FOC/workflows/Library%20Compile/badge.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![arduino-library-badge](https://www.ardu-badge.com/badge/Simple%20FOC.svg?)

This Arduino library implements Field Oriented Control (FOC) algorithm for BLDC and Stepper motors. FOC algorithm produces incomparably smooth operation and high degree of torque, velocity and position control.
The library is intended both for: 
- Beginners searching for a simple and *user-friendly* way to learn how to control BLDC and Stepper motors 
- Advanced users ready to dig deeper into the FOC algorithm and optimize the code for their particular application/hardware.


## Features
- **Arduino compatible**: 
   - Arduino library code
  - Arduino Library Manager integration
- **Open-Source**: Full code and documentation available on github
- **Easy to setup and configure**: 
  - Easy hardware configuration
  - Easy [tuning the control loops](motion_control)
- **Modular**:
  - Supports multiple motors:
     - BLDC motors
     - Stepper motors
  - Support as many [sensors and  driver boards](supported_hardware) as possible
  - Supports multiple [MCU architectures](microcontrollers):
     - Arduino: UNO, MEGA, any board with ATMega328 chips
     - STM32 boards: [Nucleo](https://www.st.com/en/evaluation-tools/stm32-nucleo-boards.html), [Bluepill](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html) ...
     - ESP32
     - Teensy boards
- **Plug & play**: Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> 

<blockquote class="warning">📢
Up to this moment (<a href="https://github.com/simplefoc/Arduino-FOC/releases">current release <i class="fa fa-tag"></i></a>) the library doesn't implement the current control loop. The motor torque is controlled via voltage directly. <a href="voltage_torque_control"> More info.</a>
</blockquote>


## YouTube demonstration videos
<iframe class="youtube" src="https://www.youtube.com/embed/Y5kLeqTc6Zk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

This video demonstrates <span class="simple">Simple<span class="foc">FOC</span>library</span> basic usage, electronic connections and shows its basic capabilities.

**The video hardware setup consists of:**
<ul class="width60">
<li> 
<b>HMBGC V2.2 board </b> <a class="pull-right" href="https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ">Ebay link <span class="fa fa-link"></span></a>
</li>
<li>
AMT 103 CUI Encoder 2048ppr <a class="pull-right" href="https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduivK%252B0pk7%2Fn5JVYn0KI22hXp9BVM%2FOAA64YDfmI%2FUQlRWDW0CMgz3WfQ6GDou4mx58%3D">Mouser link <span class="fa fa-link"></span></a>
</li>
<li>
BLDC Gimbal Motor  <a class="pull-right" href="https://fr.aliexpress.com/item/32483131130.html?spm=a2g0o.productlist.0.0.6ddd749fFd3u9E&algo_pvid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb&algo_expid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb-10&btsid=0b0a187915885172220541390e7eed&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_">Aliexpress link <span class="fa fa-link"></span></a>
</li>
</ul> 

<iframe class="youtube" src="https://www.youtube.com/embed/RI4nNMF608I" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

This video demonstrates <span class="simple">Simple<span class="foc">FOC</span>library</span> support for high-performance BLDC drivers such as DRV8302 and the support for Arudino and STM32 MCU architectures. 

**The video hardware setup consists of:**
<ul class="width60">
<li>
<b>DRV8302 driver board</b> <a class="pull-right" href="https://bit.ly/2BZZ5fG">Aliexpress link <span class="fa fa-link"></span></a>
</li>
<li> 
Arduino UNO <a class="pull-right" href="https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ">Ebay link <span class="fa fa-link"></span></a>
</li>
<li> 
Nucleo-64 F401RE<a class="pull-right" href="https://www.mouser.fr/ProductDetail/STMicroelectronics/NUCLEO-F401RE?qs=fK8dlpkaUMvGeToFJ6rzdA%3D%3D">Mouser link <span class="fa fa-link"></span></a>
</li>
<li>
USA-DIGITAL E3-8192 Encoder 8192ppr  <a class="pull-right" href="https://www.usdigital.com/products/encoders/incremental/kit/E3">USA Digital link <span class="fa fa-link"></span></a>
</li>
<li>
BLDC Gimbal Motor GBM5108-120T <a class="pull-right" href="https://www.onedrone.com/store/ipower-gbm5108-120t-gimbal-motor.html">iPower store <span class="fa fa-link"></span></a>
</li>
</ul> 

<iframe class="youtube" src="https://www.youtube.com/embed/xTlv1rPEqv4" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

This video demonstrates <span class="simple">Simple<span class="foc">FOC</span>library</span> support for Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> and using multiple motors with Arduino UNO. The project examples are based on bidirectional haptic control. 

**The video hardware setup consists of:**
<ul class="width60">
<li>
<b>Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span></b> <a class="pull-right" href="arduino_simplefoc_shield_showcase">More info <span class="fa fa-link"></span></a>
</li>
<li> 
Arduino UNO <a class="pull-right" href="https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ">Ebay link <span class="fa fa-link"></span></a>
</li>
<li> 
Nucleo-64 F401RE<a class="pull-right" href="https://www.mouser.fr/ProductDetail/STMicroelectronics/NUCLEO-F401RE?qs=fK8dlpkaUMvGeToFJ6rzdA%3D%3D">Mouser link <span class="fa fa-link"></span></a>
</li>
<li> 
AS5600  <a class="pull-right" href="https://www.ebay.com/itm/1PC-New-AS5600-magnetic-encoder-sensor-module-12bit-high-precision/303401254431?hash=item46a41fbe1f:g:nVwAAOSwTJJd8zRK">Ebay link <span class="fa fa-link"></span></a>
</li>
<li>
AMT 103 CUI <a class="pull-right" href="https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduivK%252B0pk7%2Fn5JVYn0KI22hXp9BVM%2FOAA64YDfmI%2FUQlRWDW0CMgz3WfQ6GDou4mx58%3D">Mouser link <span class="fa fa-link"></span></a>
</li>
<li>
USA-DIGITAL E3-8192  <a class="pull-right" href="https://www.usdigital.com/products/encoders/incremental/kit/E3">USA Digital link <span class="fa fa-link"></span></a>
</li>
<li>
GBM5108-120T <a class="pull-right" href="https://www.onedrone.com/store/ipower-gbm5108-120t-gimbal-motor.html">iPower store <span class="fa fa-link"></span></a>
</li>
<li>
GBM4108-120T <a class="pull-right" href="https://www.robotshop.com/en/ipower-gbm4108h-120t-gimbal-motor.html">Robotshop <span class="fa fa-link"></span></a>
</li>
</ul> 


## Installation
There are two ways to install this Arduino library, depending of your plans with the code and your particular application.
- [Full library installation](library_download) - for plug & play functionality with many examples 
- [Minimal project builder](minimal_download) - Arduino sketch with integrated complete library
    - Easier experimentation and interaction with the library code directly

<img src="extras/Images/alm.gif" class="width50">

[Find out more <i class="fa  fa-external-link"></i>](installation)


## Supported hardware
Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> is made modular and will support most of the motor + sensor combinations out there, as well as many low-cost bldc driver boards.
<p><img src="extras/Images/connection.gif" class="width50"></p>

[Find out more <i class="fa  fa-external-link"></i>](supported_hardware)

## Writing the Code
This library code is written in a way to be as simple sa possible to use and to leave a lot of space for configuration and fine tuning for specific applications. 

[Find out more <i class="fa  fa-external-link"></i>](code)


## Project Examples 
We are very happy to provide you with several projects multiple hardware configurations and fully documented and explained code!
Go see the [examples](examples)!
<p style="width:100%">
<a href="position_control_example"><img src="extras/Images/position_control_example.jpg" class="img200 img_half"></a><a href="velocity_control_example"><img src="extras/Images/hmbgc_v22_velocity_control.jpg" class="img200 img_half"></a><a href="simplefoc_pendulum"><img src="extras/Images/foc_pendulum.jpg" class="img200 img_half"></a>
</p>