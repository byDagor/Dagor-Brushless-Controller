---
layout: default
title: Home
nav_order: 1
permalink: /
---

# Dagor Brushless Controller


The Dagor Controller is a fully integrated tiny solution for brushless-based actuators with wireless capabilities. ESP32-based brushless controller that has an on-board magnetic encoder, a three-phase MOSFET driver, three MOSFET half-bridges, a temperature sensor and current sensing resistors.

The project is currently on it's Alpha stage and with the help of the Alpha testers the project will move forward faster.

![DagorBoard](Images/DagorAlpha.png)

## What makes this Controller different?

The Dagor Brushless Controller was designed as a simple platform for hobbyist and students to power brushless-based projects. The firmware for this board was developed in the Arduino development environment as part of the SimpleFOCprject. This firmware is merely a starting point as it's encouraged to modify it to fit specific projects. The firmware being very easy to modify and understand gives some key advantages:
- great platform for single board projects 
- specific communication scheme design for multi-board projects
- implementation of unique control laws
- endless amount of compatible libraries


## Key specs
| Specification    | Rating          |
| ------------- |-------------|
| Dimensions      | 44 x 44mm |
| Mass    | 12g |
| Power source voltage      | 5-24V |
| Peak current   | up to 40A |
| Magnetic sensor resolution | 14 bits |
| Temperature sensor range | -10 ~ 120°C |
