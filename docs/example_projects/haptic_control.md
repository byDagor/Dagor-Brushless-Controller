---
layout: default
title: Haptic control
parent: Example Projects
nav_order: 1
permalink: /haptic_control
---

# Haptic Control

This example shows a type of haptic control called steer by wire(less); this is done by creating a wireless virtual link between the angular position of two brushless motors. The video shows practical demonstrates of this haptic control example.

<iframe width="560" height="315" src="https://www.youtube.com/embed/DDW3rwI5KEI" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## Hardware

- 2x Dagor Controller
- 2x Brushless motor
- 2x Diametrically polarized magnet
- 2x u.fl antenna

## Software

Wire each set-up on its own test station (3D printed base) as described in the [*getting started* section](https://bydagor.github.io/Dagor-Brushless-Controller/getting_started). In the project's[Github repository](https://github.com/byDagor/Dagor-Brushless-Controller) navigate to the [examples folder](https://github.com/byDagor/Dagor-Brushless-Controller/tree/master/Examples) and find the Haptic example, download both codes and open each one on the Arduino IDE. The code for each controller is in essence the basic firmware with two modifications:

- ESP-NOW communication between the two controllers; each controller is sending a packet with its rotor angular position to the other.
- Control law that takes its motor's angular position and the other's angular position and tries to maintain the same angular position between the two motors.

```c++
setPoint = gain*(thatMotor - thisMotor);
if (setPoint >= voltageLimit) setPoint = voltageLimit;
else if(setPoint <= -voltageLimit) setPoint = -voltageLimit;
motor.move(setPoint);
```

Where:

- Gain: multiplier of the error, the bigger the stronger the virtual link. This value doesn't need to be the same between controllers, if they're different you create a virtual gear reduction within the virtual link.
- thatMotor: the other's motor angular position.
- thisMotor: this motor's angular position.
- voltageLimit: limits the voltage to a maximum and minimum number to make sure to stay within safe current limits.
- setPoint: calculated voltage from the gain times the error.

Follow the next steps to get the example running:

1. Upload code **A** to one controller and run it once; write its MAC address, printed on the Serial Monitor and leave this controller for now.
2. Upload code **B** to one controller and run it once; write its MAC address, printed on the Serial Monitor.
3. On code **B** update the boardcast Address by writing the MAC address of the controller **A**, and on code **A** update the boardcast Address by writing the MAC address of the controller **B**.

```c++
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
```

4. Upload code **A** to controller **A** and code **B** to controller **B**.
5. Run both at once and the example should run as shown in the video.
