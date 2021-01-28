---
layout: default
title: Three-phase driver
parent: Hardware
nav_order: 2
permalink: /driver
---

# Three-phase motor driver

The DRV8305 is a three-phase gate driver that can drive high and low-side N-channel MOSFETS. What makes this driver special is the amount of programmable parameters, the protection features and the fault diagnostics; all of which take place through an SPI bus.
The driver has two types of faults: *warning* and *fault*. If either occur the red indicator LED on the Dagor board will turn on; if a *fault* occurs the the output MOSFETs will be placed in their high impedance state, if a *warning* occurs operation will continue normally.
A few of the faults that the driver reports are the following: high temperature flags, source under or over-voltage, VDS over current monitors, gate drive fault, etc.
The picture below shows the simplified schematic of the driver that can be found in the [DRV8305 datasheet](https://www.ti.com/lit/ds/symlink/drv8305.pdf?ts=1593641896221&ref_url=https%253A%252F%252Fwww.google.com%252F). Please refer to it if you want to learn more about all the features of this driver IC.

![DRV8305](Images/DRV8305Schematic.PNG)
