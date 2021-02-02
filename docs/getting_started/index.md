---
layout: default
title: Getting started
nav_order: 2
permalink: /getting_started
---

# Getting started

There's a few thing to do before controlling a brushless motor with the Dagor Controller. Make sure to read this section carefully before trying to use the controller.

## Step 1. Materials

The first step is to collect all the necessary materials together:

- Dagor Controller Alpha
- Diametrically polarized magnet (included with controller)
- Brushless motor
- Motor/ controller base
- Power source
- USB to TTL converter
- Connectors (optional)

## Step 2. The motor

This section includes the material to prepare to use a particular brushless motor. Visit the [BLDC motor's page](https://docs.simplefoc.com/bldc_motors) to learn more about the type of brushless motors and their uses.

### 2.1 Phase resistance:

If there is no information regarding the phase resistance of the motor from the manufacturer measure the phase resistance with a multimeter. It doesn't matter if the multimeter doesn't have a lot of precision, what matters is getting a rough estimate of its resistance. Usually gimbal brushless motors have a phase resistance of around or above 10 ohms, which is considered a big resistance, and drone motors usually have a resistance bellow 1 ohm, which is considered a small resistance.

### 2.2 Pole pair number

Usually the manufacturer of the motor will rate the motor as the following **24N22P** where N means the number of coils and P means the number of poles (magnets). To know the number of pole pairs divide P (poles) by two, in the case of this example 22/2 = 11 pole pairs. If the manufacturer fails to provide this rating and if the magnets are exposed, the pole pair number can be found by counting the magnets on the motor's rotor and dividing by two.

### 2.3 Attaching the magnet

The Dagor Controller has an on-board magnetic sensor to measure the angular position of the motor's rotor, this is critical for the FOC algorithm. At this point the magnet provided with the controller has to be attached to the rotor. There are two good ways to do this: design and 3D print a base for the magnet and hot glue it to the motor's shaft (very easy to remove) or super glue the magnet directly to the motor's shaft (not so easy to remove).

## Step 3. Arduino IDE and SimpleFOC

The firmware was developed in the Arduino development environment as part of the SimpleFOC project. Setting up the Arduino IDE properly will assure an effortless way to work on a project with the Dagor Controller. Follow the [installation](https://docs.simplefoc.com/installation) to install the SimpleFOC library and follow the [ESP32 microcontroller set-up page](https://docs.simplefoc.com/microcontrollers#esp32-boards-support) to configure the Arduino IDE to support ESP32 boards. It is recommended to update the board's firmware **before** connecting the motor to the board.

## Step 4. Update the firmware

This section includes the relevant steps to update the firmware on a brand new Dagor Controller.

### 4.1 Downloading the firmware

Download the most recent firmware version and open it on the Arduino IDE.

### 4.2 Changing the user parameters

With the information from the previous steps there's a few parameters that must be changed in the firmware, such as: power supply voltage, phase resistance, maximum estimated current, etc.

### 4.3 Controller flash mode

It is required a USB to TTL adapter to flash the Dagor board; I recommend using the CP2102 module. Once the adapter is connected to the board (as shown in the picture bellow), to put the Dagor board in flash mode press and hold the Reset Button, press once the Boot Button and then release the Reset Button. You should be able to read on the Serial monitor that the board is *waiting for download*. Press the upload button on the Arduino IDE and the ESP32 will begin flashing, after it's done press the Reset Button once and the code should start running.

![CP2102](Images/DagorCP2102.png)

## Step 5. Motor and controller base
