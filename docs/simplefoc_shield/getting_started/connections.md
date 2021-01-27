---
layout: default
title: Connecting the hardware
parent: Getting Started
description: "Connecting SimpleFOCShield with your hardware."
nav_order: 2
permalink: /foc_shield_connect_hardware
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
---

# Connecting the hardware

<p>
<img src="extras/Images/connection.gif" class="width50">
</p>

Connecting the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> to the microcontroller, BLDC motor, power-supply and the sensors is very straight forward. 

## Microcontroller
- Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is designed to be easily stackable on all the devices featuring arduino headers, such as: [Arudino UNO](arduino_simplefoc_shield), Arudino MEGA, [Stm32 Nucleo](nucleo_connection) and similar.
- Bit it can also be used as a standalone BLDC driver as shown in combination with [Stm32 Bluepill](bluepill_connection).

## BLDC motor
- Motor phases `a`, `b` and `c` are connected directly the motor terminal connector `TB_M1`

<blockquote class="warning"><p class="heading">BEWARE: Power limitations</p>
Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is designed for gimbal motors with internal resistance higher than R>10Ohm. The absolute maximal current of this board is 5A. Please make sure when using this board in your projects that the BLDC motor used does comply with these limits.  <br>
If you still want to use this driver with the BLDC motors with very low resistance R < 1Ohm make sure to limit the voltage set to the board. <br>
For a bit more information about the choice of motors visit <a href="bldc_motors"> BLDC motor docs</a>
</blockquote>

## Power supply
- Power supply cables are connected directly to the terminal `TB_PWR` 
- Recommended power supply voltage is from 12V to 24V but in general, the board has not been tested for higher voltages, but there shouldn't be too much problems up to 30V.



## Encoder
- Channels `A` and `B` are connected to the encoder connector `P_ENC`, terminals `A` and `B`. 
- If your encoder has `index` signal you can connect it to the encoder connector as well, terminal `I`.

<img src="extras/Images/foc_shield_v13_enc.png" class="">

## Magnetic sensor SPI
- Magnetic sensor's SPI interface signals `SCK`, `MISO` and `MOSI` are connected to the Arduino's `SPI` pins (Arduino UNO `13`,`12` and `11`). 
  - If the application requires more than one sensor all of them are connected to the same pins of the Arduino.
- The `chip select` pin is connected to the desired pin. Each sensor connected to the same Arduino has to have unique chip select pin.

<img src="extras/Images/foc_shield_v13_magSPI.png" class="">

## Magnetic sensor I2C
- Magnetic sensor's I2C interface signals `SCL` and `SDA` are connected to the Arduino's `I2C` pins (Arduino UNO `A4` and `A5`). 
  - If the application requires more than one sensor all of them are connected to the same pins of the Arduino.
- It is possible that you will need to use additional pull-up resistors for the `SDA` and `SCL` lines.

<img src="extras/Images/foc_shield_v13_magI2C.png" class="">


## Magnetic sensor Analog output
- Magnetic sensor's Analog output is connected directly to any analog input pin, on a figure below we use `A0`
  - If the application requires more than one sensor each of them is connected to one of the analog input pins.

<img src="extras/Images/foc_shield_v13_analog.png" class="">


## Hall sensors
- Channels `A`, `B` and `C` ( `U`, `V` and `W` ) are connected to the encoder connector `P_ENC`. 

<img src="extras/Images/foc_shield_v13_hall.png" class="">

