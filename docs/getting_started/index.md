---
layout: default
title: Getting started
nav_order: 2
permalink: /getting_started
---

# Getting started

There's a few things to do before controlling a brushless motor with the Dagor Controller. Make sure to read this section carefully before trying to use the controller.

## Step 1. Materials

The first step is to collect all the necessary materials together:

- Dagor Controller Alpha
- Diametrically polarized magnet (included with controller)
- Brushless motor
- Motor/ controller base
- Adequate power source
- USB to TTL converter
- Connectors (optional)

## Step 2. The motor

This section includes the resources to prepare to use a particular brushless motor. Visit the [BLDC motor's](https://docs.simplefoc.com/bldc_motors) page to learn more about the type of brushless motors and their uses.

### 2.1 Phase resistance:

If there is no information regarding the phase resistance of the motor from the manufacturer measure the phase resistance with a multimeter. It doesn't matter if the multimeter isn't extremely accurate, what matters is getting a rough estimate of its resistance. Usually gimbal brushless motors have a phase resistance of around or above 10 ohms, which is considered a big resistance, and drone motors usually have a resistance bellow 1 ohm, which is considered a small resistance. Write down the phase resistance as it will be needed later.

### 2.2 Pole pair number

Usually the manufacturer of the motor will rate the motor as the following **24N22P** where N means the number of coils and P means the number of poles (magnets). To know the number of pole pairs divide **P** (poles) by two, in the case of this example 22/2 = 11 pole pairs. If the manufacturer fails to provide this rating and if the magnets are exposed, the pole pair number can be found by counting the magnets on the motor's rotor and dividing by two.

### 2.3 Attaching the magnet

The Dagor Controller has an on-board magnetic sensor to measure the angular position of the motor's rotor, this is critical for the [FOC (field oriented control) algorithm](https://docs.simplefoc.com/foc_theory). At this point the magnet provided with the controller has to be attached firmly to the motor's rotor. There are two easy ways to do this: design and 3D print a base for the magnet and hot glue it to the motor's shaft (very easy to remove) or super glue the magnet directly to the motor's shaft (not so easy to remove).

## Step 3. Arduino IDE and SimpleFOC

The firmware was developed in the Arduino development environment and is powered by the [SimpleFOC project](https://www.simplefoc.com/). Setting up the Arduino IDE properly will assure an effortless way to work on a project with the Dagor Controller. Follow the [installation instructions](https://docs.simplefoc.com/installation) to install the SimpleFOC library and follow the [ESP32 microcontroller set-up page](https://docs.simplefoc.com/microcontrollers#esp32-boards-support) to configure the Arduino IDE to support ESP32 boards. It is recommended to update the board's firmware **before** connecting the motor to the board.

## Step 4. Update the firmware

This section includes the relevant steps to update the firmware on a brand new Dagor Controller.

### 4.1 Downloading the firmware and change user parameters

Download the most recent firmware version from the [Github repository](https://github.com/byDagor/Dagor-Brushless-Controller/tree/master/Firmware) and open it on the Arduino IDE. Visit the [firmware section](https://bydagor.github.io/Dagor-Brushless-Controller/Firmware) to learn about the firmware structure and version name.

Given each individual set-up there's a few parameters that must be changed in the firmware. By this point the pole pair number, the phase resistance, the power supply voltage should be known. Visit the [user parameters section](https://bydagor.github.io/Dagor-Brushless-Controller/user_param) to learn how to update these parameters in the firmware.

### 4.2 Controller flash mode

It is required a USB to TTL adapter to flash the Dagor board; it is recommended to use the CP2102 module. Solder (optionally) any type of headers style to the board to be able to easily connect and disconnect the module. Once the adapter is connected to the board (as shown in the picture bellow) and the board is powered, to put the Dagor board in flash mode press and hold the *Reset* Button, press once the *Boot* Button and then release the *Reset* Button. If done correctly the Serial monitor of the Arduino IDE should print that the board is *waiting for download*. After updating the firmware to the desired parameters, press the upload button on the Arduino IDE and the ESP32 will begin flashing, after it's done press the *Reset* Button once and the code should start running.

![CP2102](Images/DagorCP2102.png)

## Step 5. Motor and controller base

The Dagor controller has to be firmly attached to the motor's stator, fixing the center of the magnetic sensor to the center of the magnet. One easy way to do this is by designing and 3D printing a base (or test station) that fixes together a motor (with its magnet) and a controller. Here you can find a CAD to use as reference for a 3D printed base, like seen in the picture bellow.

## Step 6. Testing

By this point, the only thing left to do is test the set-up and tune the controller.

### 6.1 Connecting everything

Connect the Dagor controller to the motor and USB to TTL module, power the controller and the motor will start position controlling the motor. If the controller is very badly tuned (as is expected) the position control might work very badly and tuning will be needed. Make sure to start with a very modest voltage limit so that the response isn't very bad while tuning the controller's parameters.

### 6.2 Tuning the controller

Tuning the controller is essential to obtain the desired response out of the motors movements, avoid oscillations and avoid vibrations. The serial monitor is used to tune the controller by sending [commands](https://docs.simplefoc.com/communication) to it. These [commands](https://docs.simplefoc.com/communication) are implemented directly into SimpleFOC, and each one consists of a special character next to the desired value. For example, to change the Position control loop PROPORTIONAL gain from 25 to 10 the user writes in the Serial Monitor *K10*; to change the Velocity control loop INTEGRAL gain from 2.5 to 3 the user writes *I3*. To check the actual value of the Position control loop PROPORTIONAL gain the user writes *K* in the serial monitor. Please refer to the [Motor commands](https://docs.simplefoc.com/communication) to learn about all the different commands and please visit the [motion control implementation](https://docs.simplefoc.com/motion_control_implementation) to learn about the control loops and control theory.

### 6.3 Share

Share with us your set-up! [SimpleFOC forums](https://community.simplefoc.com/)
