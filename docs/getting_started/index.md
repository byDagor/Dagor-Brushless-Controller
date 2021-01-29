---
layout: default
title: Getting started
nav_order: 2
permalink: /started
---

# Getting started

There's a few thing to do before controlling a brushless motor with the Dagor Controller. Make sure to read this section carefully before trying to use the controller. 

## Step 1. Materials

The first step is to collect all the necesarry materials together:

- Dagor Controller Alpha
- Diametrically polarized magnet (included with controller)
- Brushless motor
- Motor/ controller base
- Power source
- USB to TTL converter
- Connectors (optional)

## Step 2. The motor

This section includes the material to prepare to use a particular brushless motor.

### 2.1 Phase resistance:

If there is no information regarding the phase resistance of the motor from the manufacturer measure the phase resistance with a multimeter. It doesn't matter if the multimeter doesn't have a lot of precision, what matters is getting a rough estimate of its resistance. Usually gimbal brushless motors have a phase resistance of around 10 ohms, which is considered a big number, and drone motors usually have a resistance bellow 1 ohm, wich is considered a small number. 

### 2.2 Pole pair number

Ususally the manufacturer of the motor will 

### 2.3 Attaching the magnet

The Dagor Controller has an on-board magnetic sensor to measure the angular position of the motor's rotor, this is critical for the FOC algorithm. At this point the magnet provided with the controller has to be attached to the rotor. There are two good ways to do this: design and 3D print a base for the magnet and hot glue it to the motor's shaft (very easy to remove) or super glue the magnet directly to the motor's shaft (not so easy to remove).

## Step 3. Arduino IDE and SimpleFOC

## Step 4. Update the firmware

With the information from the previous steps there's a few parameters that must be changed in the firmware, such as: power supply voltage, 
