---
layout: default
title: Voltage step-down
parent: Hardware
nav_order: 3
permalink: /buck
---

# Voltage step-down converter

The Dagor Controller has a DC-DC stp-down (buck) converter that regulates the input voltage to 3.3V to power the ESP32 and the magnetic sensor and to be able to power external peripherals, such as: an external magnetic sensor, encoder, display, etc.

| :warning: WARNING          |
|:---------------------------|
| Touching this IC may fatally damage the controller (the IC is label as U3 on the board).   |
