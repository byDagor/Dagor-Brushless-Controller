---
layout: default
title: BLDCDriver 6PWM
nav_order: 2
permalink: /bldcdriver6pwm
parent: BLDCDriver
grand_parent: Driver code
grand_grand_parent: Writing the Code
grand_grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# BLDC driver 6PWM - `BLDCDriver6PWM`

This is the class which provides an abstraction layer of most of the common 6PWM bldc drivers out there. Basically any BLDC driver board that can be run using 6PWM signals can be represented with this class.
Examples:
- DRV830x ( can be run in 3pwm or 6pwm mode )
- ST B-G431B
- X-NUCLEO-IHM08M1
- etc.


<img src="extras/Images/6pwm_driver.png" class="width40">

6 PWM control mode gives much more freedom for BLDC motor control than 3PWM control since each of the 6 half-bride mosfets can be controlled separately. 

## Step 1. Hardware setup
To create the interface to the BLDC driver you need to specify the 6 `pwm` pin numbers for each motor phase and optionally `enable` pin.
```cpp
//  BLDCDriver6PWM( int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int en)
//  - phA_h, phA_l - A phase pwm pin high/low pair 
//  - phB_h, phB_l - B phase pwm pin high/low pair
//  - phB_h, phC_l - C phase pwm pin high/low pair
//  - enable pin    - (optional input)
BLDCDriver6PWM motor = BLDCDriver6PWM(5,6, 9,10, 3,11, 8);
```
<blockquote class="warning">
⚠️ 6 PWM configuration is very hardware specific and please make sure to respect certain guidelines in order for it to work properly!
</blockquote>

### Arduino UNO support
Arduino UNO and all the atmega328 based boards have only 6 pwm pins and in order to use the `BLDCDrievr6PWM` we need to use all of them. Those are `3`,`5`,`6`,`9`,`10` and `11`. 
Furthermore in order for the algorithm to work well we need to use the pwm pins that belong to the same timer for each high/low side pair of each phase. 7
So Atmega328 pins belonging to the timers are:

`TIM0` |`TIM1` |`TIM2` 
--- | --- | ---
`5`,`6` | `9`,`10` |`3`,`11`

Therefore it is important that `phA_h` and `phA_l` belong to one timer, `phB_h` and `phB_l` to second timer and `phC_h` and `phC_l` to the last timer. If we decide that phase `A` belongs to the timer `TIM0` we can set `phA_h` either to pin `5` or pin `6`. 

### stm32 support

Stm32 boards have two possible 6pwm modes:
- Hardware 6pwm mode
- Software 6pwm mode

####  Hardware 6pwm mode
In hardware 6pwm mode the user uses only one timer, usually Timer 1 for all the 6PWM channels. Stm32 boards usually have at least one timer which has automatic complementary channels which avoids the need for a complicated channel inverting configuration. <span class="simple">Simple<span class="foc">FOC</span>library</span> automatically enables this control mode if you provide the pins that support this interface to the constructor of the `BLDCDriver6PWM` class. For example, both stm32 Bluepill and stm32 Nucleo boards have this interface supported by pins:

 `T1C1` | `T1C2` | `T1C3` | `T1C1N` | `T1C2N` | `T1C3N` 
 --- | --- | ---| ---| ---| ---
 `PA8` | `PA9` | `PA10` | `PB13` | `PB14` | `PB15`  

Where `T1Cx` are the Timer 1 channels and `T1CxN` are their complementary channels (inverted channels). Each pair of `T1Cx` and `T1CxN` is used for one pair of the high/low pwm pins. The library will configure the necessary timers and registers if you provide these pins to the constrictor of the `BLDCDriver6PWM` class. For example:
```cpp
//  BLDCDriver6PWM( int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int en)
BLDCDriver6PWM motor = BLDCDriver6PWM(PA8, PB13, PA9, PB14, PA10, PB15);
```

####  Software 6pwm mode
If it is not possible to use the hardware 6pwm mode with your board  <span class="simple">Simple<span class="foc">FOC</span>library</span> enables you to use any two channels of any of the timers as your high/low side pwm pair. Basically, the library will automatically configure the complementary channels on the provided low side pins. The only requirement for this code to work properly is exatcly the same as for the Arudino UNO, each phase high/low pwm pair needs to belong to the same timer. 
For example, if we take stm32 Nucleo F401RE board we can take for example:
```cpp
//  BLDCDriver6PWM( int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int en)
BLDCDriver6PWM motor = BLDCDriver6PWM(7, 2, 6, 3, 5, 4);
```
Where

 `T1C1` | `T1C3` | `T2C3` | `T2C2` | `T3C1` | `T3C2` 
 --- | --- | ---| ---| ---| ---
 `7` | `2` | `6` | `3` | `5` | `4`  

On Bluepill we could use:
```cpp
//  BLDCDriver6PWM( int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int en)
BLDCDriver6PWM motor = BLDCDriver6PWM(PA8, PA9, PB6, PB7, PB8, PB9);
```
Where

 `T1C1` | `T1C2` | `T4C1` | `T4C2` | `T4C3` | `T4C4` 
 --- | --- | ---| ---| ---| ---
 `PA8` | `PA9` | `PB6` | `PB7` | `PB8` | `PB9`  

### esp32 support
Esp32 boards support `MCPWM` interface that is intended for this kind of applications. Each ep32 board has two of the `MCPWM` channels and can support two 6PWM drivers. There is no pin specific requirements for the esp32, each pin can be used in pwm mode. But please make sure not to use the pins that have predefined states on boot because this could result malfunction. You can find this information online easily, here is a [YouTube video](https://www.youtube.com/watch?v=c0tMGlJVmkw) with more details. 

## Step 2.1 PWM Configuration
```cpp
// pwm frequency to be used [Hz]
// for atmega328 fixed to 32kHz
// esp32/stm32/teensy configurable
driver.pwm_frequency = 50000;
```
<blockquote class="warning">
⚠️ Arduino devices based on ATMega328 chips have fixed pwm frequency of 32kHz.
</blockquote>

Here is a list of different microcontrollers and their PWM frequency and resolution used with the  Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>.

MCU | default frequency | MAX frequency | PWM resolution | Center-aligned | Configurable freq
--- | --- | --- | --- | ---
Arduino UNO(Atmega328) | 32 kHz | 32 kHz | 8bit | yes | no
STM32 | 50kHz | 100kHz | 14bit | yes | yes
ESP32 | 40kHz | 100kHz | 10bit | yes | yes
Teensy | 50kHz | 100kHz | 8bit | yes | yes

All of these settings are defined in the `drivers/hardware_specific/x_mcu.cpp/h` of the library source. 


## Step 2.2 Dead zone (dead time) 


<img src="extras/Images/dead_zone.png" class="width30">

```cpp
// dead_zone [0,1] - default 0.02 - 2%
driver.dead_zone = 0.05;
```
The dead zone parameter is defined as the amount of teh duty cycle that is reserved in between changing the active mosfet. Each time the high/low side is deacitvated and low/high side is activated half of the `dead_zone` is injected. This parameter is equivalent to the dead time, dead_time can be calculated as:
```cpp
dead_time = 1/pwm_frequency*dead_zone
```

## Step 2.2 Voltages
Driver class is the one that handles setting the pwm duty cycles to the driver output pins and it is needs to know the DC power supply voltage it is plugged to.
Additionally driver class enables the user to set the absolute DC voltage limit the driver will be set to the output pins.  
```cpp
// power supply voltage [V]
driver.voltage_power_supply = 12;
// Max DC voltage allowed - default voltage_power_supply
driver.voltage_limit = 12;
```

<img src="extras/Images/limits.png" class="width60">

This parameter is used by the `BLDCMotor` class as well. As shown on the figure above the once the voltage limit `driver.voltage_limit` is set, it will be communicated to the FOC algorithm in `BLDCMotor` class and the phase voltages will be centered around the `driver.voltage_limit/2`.

Therefore this parameter is very important if there is concern of too high currents generated by the motor. In those cases this parameter can be used as a security feature. 

## Step 2.3 Initialisation
Once when all the necessary configuration parameters are set the driver function `init()` is called. This function uses the configuration parameters and configures all the necessary hardware and software for driver code execution.
```cpp
// driver init
driver.init();
```

## Step 3. Using encoder in real-time

BLDC driver class was developed to be used with the <span class="simple">Simple<span class="foc">FOC</span>library</span> and to provide the abstraction layer for FOC algorithm implemented in the `BLDCMotor` class. But the `BLDCDriver3PWM` class can used as a standalone class as well and once can choose to implement any other type of control algorithm using the bldc driver.  

## FOC algorithm support
In the context of the FOC control all the driver usage is done internally by the motion control algorithm and all that is needed to enable is is just link the driver to the `BLDCMotor` class.
```cpp
// linking the driver to the motor
motor.linkDriver(&driver)
```

## Standalone driver 
If you wish to use the bldc driver as a standalone device and implement your-own logic around it this can be easily done. Here is an example code of a very simple standalone application.
```cpp
// BLDC driver standalone example
#include <SimpleFOC.h>

// BLDC driver instance
BLDCDriver6PWM driver = BLDCDriver6PWM(5, 6, 9,10, 3, 11, 8);

void setup() {
  
  // pwm frequency to be used [Hz]
  driver.pwm_frequency = 50000;
  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  // Max DC voltage allowed - default voltage_power_supply
  driver.voltage_limit = 12;
  // daad_zone [0,1] - default 0.02 - 2%
  driver.dead_zone = 0.05;

  // driver init
  driver.init();

  // enable driver
  driver.enable();

  _delay(1000);
}

void loop() {
    // setting pwm
    // phase A: 3V, phase B: 6V, phase C: 5V
    driver.setPwm(3,6,5);
}
```