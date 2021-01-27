---
layout: default
title: Work roadmap
parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
nav_order: 8
permalink: /roadmap
---

#  Roadmap

For more info about the features of library releases visit the [github releases <i class="fa fa-tag"></i>](https://github.com/simplefoc/Arduino-FOC/releases).

## Motion Control and FOC algorithm
- [ ] Motor control: Support for current control
- [ ] Motor control: FOC using IMU - camera gimbal and similar applications
- [x] **Motor control: Support for stepper motors**
- [x] Motor control: Support for Hall sensor commutation 
- [x] Motor control: Support for fully open-loop operation (no sensor)
- [x] Add support for acceleration ramping
- [x] Velocity Low pass filter
- [x] Timer interrupt execution rather than in the `loop()`: ❌ No real performance improvement
- [x] Sine wave lookup table implementation
- [X] Implement Space Vector Modulation method : Pure SVM
- [x] Implement Space Vector Modulation method : PWM SVM

## MCU support
- [x] Hardware specific code separation : easier porting in between devices `hardware_utils.cpp/.h`
- [x] Teensy support
- [x] ESP32 support
- [x] STM32 Nucleo support
- [x] STM32 BLuepill support

## Driver support
- [ ] Driver support: Implement support for MOSFET control low and high pairs
- [x] Driver support: DRV8302 borads

## Sensor support
- [ ] IMU as position sensor
- [ ] Back-EMF support
- [ ] Make support for magnetic encoder  SSI
- [ ] Make support for magnetic encoder  PWM
- [x] Make support for magnetic encoder  Analog
- [x] Make support for magnetic encoder  I2C
- [x] Make support for magnetic encoder  ABI
- [x] Make support for magnetic encoder  SPI
- [x] Hall sensor support
- [x] Encoder index proper implementation

## User interaction
- [x] Implement motor commands 
- [x] Support monitoring 

## Usability 
- [ ] <span class="simple">Simple<span class="foc">FOC</span>library</span> getting started page
- [x] <span class="simple">Simple<span class="foc">FOC</span>Shield</span> getting started page
- [x] Make the library accessible in the Arduino Library Manager 
- [x] Make minimal version of the arduino code - all in one arduino file
- [x] Documentation separation from README
- [x] Proper introduction of the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>

## Videos  
- [ ] VIDEO: Publish a video tutorial for using the library and the samples  
- [ ] VIDEO: Coding setup and procedure video
- [x] VIDEO: Two motors running on HMBGC example
- [x] VIDEO: Initial video with simple demonstration




