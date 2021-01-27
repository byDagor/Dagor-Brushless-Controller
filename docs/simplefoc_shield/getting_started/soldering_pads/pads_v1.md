---
layout: default
title: v1.3.3
parent: Soldering pads
grand_parent: Getting Started
description: "Configuring your SimpleFOCShield by soldering the pads."
nav_order: 1
permalink: /pads_soldering_v1
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
---
# Hardware configuration using soldering pads <br> <span class="simple">Simple<span class="foc">FOC</span>Shield</span> <small>v1.3</small>
One of the very important features of the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is the hardware configuration. 

<img src="extras/Images/shield_bot_v131_pinout.gif" class="width40">

Each board has a set of solder pads on the bottom side which are used for configuration. These solder pads enable board to:

- Enable / Disable pull-up resistors for encoder A,B and Index channel
- Configure BLDC driver pinout (PWM pins A,B,C and enable pin)

<blockquote class="info"> <p class="heading">BEWARE 📢: Conductive ink </p>
All the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> boards will be initially tested and they will be shipped with initial configuration. The testing configuration will be done with use of conductive ink instead of soldering connections. Therefore once you have the board, if you wish to change configuration all you need to do is remove the ink with some wet paper wipe.
</blockquote>

## Enabling encoder/hall sensor pull-up resistors

<img src="extras/Images/shield_bot_v131_pullup_enable.png" class="width30">

Each board has integrated set of three 3.3KOhm pull-up resistors for encoder channels A,B and Index (or Hall sensor U, V, W). The picture above shows how solder the pads in order to enable the Pull-up resistors.
Not all encoders need the pull-up resistors, or better said, in general, most of them don't need them.  For those of us who are looking for price optimization :slight_smile: , a lot of cheap Ebay/Aliexpress encoders will need them such as [600P ebay encoder <i class="fa fa-external-link"></i>](https://www.ebay.com/itm/360-600P-R-Photoelectric-Incremental-Rotary-Encoder-5V-24V-AB-Two-Phases-Shaft/254214673272?hash=item3b30601378:g:AZsAAOSwu~lcxosc) and similar.

## Enabling I2C pull-up resistors

<img src="extras/Images/shield_bot_v132_i2c_pullup_enable.png" class="width30">

From the shield [<i class="fa fa-tag"></i>version 1.3.2](https://github.com/simplefoc/Arduino-SimpleFOCShield/releases) the boards come with with 4.7KOhm pull-up resistors for I2C communication pins. The picture above shows how solder the pads in order to enable the Pull-up resistors.
Not all I2C devices (especially Magnetic sensors) need the pull-up resistors, or better said, in general, most of them don't need them, especially with Arduino UNO. But it is very common to have problems interfacing these sensors with STM32 boards such as Nucleo-64. There you will need to enable the pullups or provide them yourself externally.  
<blockquote class="warning"><p class="heading">BEWARE: Stacking</p>
If you are stacking the shields and you wish to use the I2C pull-ups, make sure you solder these pads on only board at a time!
</blockquote>

## Customizing pinout

Pinout customization of the <span class="simple">Simple<span class="foc">FOC</span>Shield</span>  enables the board to be very flexible with using different sensors and additional arduino modules. But more importantly it enables the board to be stackable. 

Here is a table of the configurable signals and their possible pin assignments:

Signal | Possible pins
--- | ---
Pwm A | 3, 9 
Pwm B | 6, 11, **13***
Pwm C | 5, 10
Enable | 7, 8
Encoder A | 3, A2, A3
Encoder B | 2, A1 
Encoder I | 4, A0

<small>*From <span class="simple">Simple<span class="foc">FOC</span>Shield</span> <a href="https://github.com/simplefoc/Arduino-SimpleFOCShield/releases">version <i>v1.3.1</i></a></small>

Now, there is a lot of possible pin configurations and not all of them are possible depending on the microcontroller and sensor you are using. 
For example, Arduino UNO only has 2 external interrupt pins, and they are pin  `2` and `3`. Therefore when using the board with Arduino UNO and Encoder we will try to use pin `3` for encoder channel A and not for pwm A. 

Another example is when stacking two boards with the STM32 Nucleo. Nucleo board cannot have pwm generation on pins `11` and `6` therefore you cannot combine those pins at the same time. Therefore whn using Nucleo board, the rule of thumb is to avoid using pin `11` but use pin `13` instead.

Therefore in the following text you can find the suggested pinout configuration based on the stacking necessity and the microcontroller used.

### Suggested pinout: Single board
When using only one board with only one motor, it is much easier to choose the pinout. Basically you only need to be careful if you are using the encoder to use pin `3` with encoder channel A and not withe the driver's pin pwm A. And as well, if you are using the SPI magnetic sensor you should avoid using pins `10` and `11` because they are used with the SPI communication.  

<img src="extras/Images/shield_bot_v131_config_single.png" class="width30">

With all of this in mind, probably the best pinout you can have when using one board will be:

Signal | Pwm A | Pwm B | Pwm C | Enable | Encoder A | Encoder B | Encoder I
--- | --- | ---- | --- | --- | --- | --- | ---
Pin number | 9 | 5 | 6 | 8 | 3 | 2 | A0 

On the image above you can see which soldering pads you would need to solder in order to obtain this configuration.
```cpp
// driver instance configuration based on pinout above
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);
```

### Suggested pinout: Stacking with Arduino UNO

Arduino UNO has only 6 pwm pins which means when we stack two boards we do not have much choice which ones to use, we need all of them. It is nto important how we organize the pwm A,B,C,enable and encoder A,B,I signals as log as we use pin `3` for pwm A and as long as we don't use pin `13` for pwm C.

<img src="extras/Images/shield_bot_v131_config_double.png" class="width30">
<img src="extras/Images/shield_bot_v131_config_double_ard.png" class="width30">

Here is one example of a pin assignment compatible with Arduino UNO:
 
Signal | Pwm A | Pwm B | Pwm C | Enable | Encoder A | Encoder B | Encoder I
--- | --- | ---- | --- | --- | --- | --- | ---
Board #1 | 9 | 10 | 11 | 8 | A2 | A1 | A0 
Board #2 | 3 | 5 | 6 | 7 | A3 | 2 | 4 

The figure above shows how to solder the pads on both boards to obtain the desired pinout.  
```cpp
// motor instances configuration based on pinout above
BLDCDriver3PWM driver1 = BLDCDriver3PWM(9, 10, 11, 8);
BLDCDriver3PWM driver2 = BLDCDriver3PWM(3, 5, 6, 7);
```

### Suggested pinout: Stacking with Stm32 Nucleo

When using stacked <span class="simple">Simple<span class="foc">FOC</span>Shield</span> with stm32 Nucleo board, we just have to make sure not to use pin `11` for pwm C, but to use pin `13` instead. And as for Arduino UNO, we should not use pin `3` for encoder A, but for pwm A. But if we respect these constraints we can choose the other pins as we wish.

<img src="extras/Images/shield_bot_v131_config_double.png" class="width30">
<img src="extras/Images/shield_bot_v131_config_double_nucleo.png" class="width30">

Here is an example of chosen pinout configuration valid for stacking with Nucleo board.

Signal | Pwm A | Pwm B | Pwm C | Enable | Encoder A | Encoder B | Encoder I
--- | --- | ---- | --- | --- | --- | --- | ---
Board #1 | 9 | 10 | 6 | 8 | A2 | A1 | A0 
Board #2 | 3 | 5 | 13 | 7 | A3 | 2 | 4 

See the figure above to find how to solder the pads to obtain this configuration.

```cpp
// motor instances configuration based on pinout above
BLDCDriver3PWM driver1 = BLDCDriver3PWM(9, 10, 6, 8);
BLDCDriver3PWM driver2 = BLDCDriver3PWM(3, 5, 13, 7);
```

<blockquote class="info"><p class="heading">Pin 13 or pin 11</p> We advise you to try first with the  pin 13 configured and then with pin 11 if this does not work. Tested boards are Nucleo-F401RE - pin 13 works / pin 11 doesn't and  Nucleo-F466RE pin 11 works/ pin 13 doesnt.</blockquote>