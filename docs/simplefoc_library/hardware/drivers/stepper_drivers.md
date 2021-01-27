---
layout: default
title: Stepper drivers
nav_order: 2
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /stepper_drivers
parent: Drivers
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Stepper motor drivers
This library will be compatible with most of the 2 phase stepper motor driver boards that feature 2 full H-bridges or 4 half-bridges such as [<i class="fa fa-file"></i> MC33926](https://www.nxp.com/docs/en/data-sheet/MC33926.pdf), [<i class="fa fa-file"></i> L298](https://www.st.com/resource/en/datasheet/l298.pdf), [<i class="fa fa-file"></i> L293](http://www.ti.com/lit/ds/symlink/l293.pdf) and many more. **In order for the driver board to work with the library it needs to be controllable using 4 pwm signals.**

<blockquote class="warning"><p class="heading">⚠️ DIR/STEP stepper drivers not supported!</p>
This library does not support stepper drivers with DIR+STEP (step and direction) interface such as A4988, DRV8825, TB6600, TB6560 and similar.
</blockquote>

The choice of stepper driver depends directly of the stepper motor you are using, basically make sure the stepper driver can provide the current the motor needs to perform. 



## Supported board examples
Here are some stepper driver boards that are supported by this library. 

Examples | Description | Specifications | Link | Price
---- | ---- | ---- | ---
[<img src="extras/Images/ms1508.jpg" style="height:100px">](https://www.ebay.com/itm/Dual-Channel-DC-Motor-Driver-Mini-Module-PWM-Speed-Control-Beyond-L298N-S2U/124342998274?hash=item1cf36b9502:g:zJoAAOSwFuZbSF25)| Stepper driver MX1508| - MX1508  chip <br> - 1 motor <br>- 5V-10V <br> - 2.5A | [Ebay](https://www.ebay.com/itm/Dual-Channel-DC-Motor-Driver-Mini-Module-PWM-Speed-Control-Beyond-L298N-S2U/124342998274?hash=item1cf36b9502:g:zJoAAOSwFuZbSF25) | 1€
[<img src="extras/Images/l298n.jpg" style="height:100px">](https://www.ebay.com/itm/L298N-DC-Stepper-Motor-Driver-Module-Dual-H-Bridge-Control-Board-for-Arduino/362863436137?hash=item547c58a169:g:gkYAAOSwe6FaJ5Df)| Stepper driver L298N| - L298N  chip <br> - 1 motor <br>- 5V-35V <br> - 2A(MAX single bridge) | [Ebay](https://www.ebay.com/itm/L298N-DC-Stepper-Motor-Driver-Module-Dual-H-Bridge-Control-Board-for-Arduino/362863436137?hash=item547c58a169:g:gkYAAOSwe6FaJ5Df) | 2€
[<img src="extras/Images/shiled_stepper.jpg" style="height:100px">](https://www.ebay.com/itm/L298P-Shield-R3-DC-Motor-Driver-Module-2A-H-Bridge-2-way-For-Arduino-UNO-2560/310787745501?hash=item485c64a6dd:g:m0sAAOSwXwdfMo5O)| Shield R3 DC Motor Driver Module| - L298P  chip <br> - 1 motor <br>- 5V-35V <br> - 2A(MAX single bridge) | [Ebay](https://www.ebay.com/itm/L298P-Shield-R3-DC-Motor-Driver-Module-2A-H-Bridge-2-way-For-Arduino-UNO-2560/310787745501?hash=item485c64a6dd:g:m0sAAOSwXwdfMo5O) | 6€
[<img src="extras/Images/shiled_stepper1.jpg" style="height:100px">](https://www.ebay.com/itm/L298P-Shield-R3-DC-Motor-Driver-Module-2A-H-Bridge-2-way-For-Arduino-UNO-2560/310787745501?hash=item485c64a6dd:g:m0sAAOSwXwdfMo5O)| Arduino Motor Drive Shield V1| - L293D chip <br> - 1 motor <br>- 5V-35V <br> - 0.6A( 1.2 peak) | [Ebay](https://www.ebay.com/itm/L298P-Shield-R3-DC-Motor-Driver-Module-2A-H-Bridge-2-way-For-Arduino-UNO-2560/310787745501?hash=item485c64a6dd:g:m0sAAOSwXwdfMo5O) | 6€
[<img src="extras/Images/shiled_stepper3.jpg" style="height:100px">](https://www.ebay.com/itm/Motor-Stepper-Servo-Robot-Shield-for-Arduino-I2C-v2-Kit-w-PWM-Driver-TOP/201415058167?hash=item2ee545e2f7:g:IkgAAOSwJ-5aTI4Q)| Arduino Motor Drive Shield V2| - TB6612 chip <br> - 1 motor <br>- 5V-35V <br> - 1.2A (3A peak) | [Ebay](https://www.ebay.com/itm/Motor-Stepper-Servo-Robot-Shield-for-Arduino-I2C-v2-Kit-w-PWM-Driver-TOP/201415058167?hash=item2ee545e2f7:g:IkgAAOSwJ-5aTI4Q) | 25€
[<img src="extras/Images/shield_monster.jpg" style="height:100px">](https://www.ebay.com/itm/L298N-Dual-VNH2SP30-Stepper-Motor-Driver-Module-30A-Monster-Moto-Shield-Replace/112031018900?hash=item1a1591af94:g:R4YAAOSwEaBaTafh)| Arduino Monster motor shield| - VNH2SP30  chip <br> - 1 motor <br>- 5V-16V <br> - 14A (30A peak) | [Ebay](https://www.ebay.com/itm/L298N-Dual-VNH2SP30-Stepper-Motor-Driver-Module-30A-Monster-Moto-Shield-Replace/112031018900?hash=item1a1591af94:g:R4YAAOSwEaBaTafh) | 8€


Alternatively, you can use two DC motor driver boards, one for each phase of the motor. Here are some examples:

Examples | Description | Specifications | Link | Price
---- | ---- | ---- | ---
[<img src="extras/Images/pololu.jpg" style="height:100px">](https://www.pololu.com/product/1212) | Pololu MC33926 | - MC33926 chip <br> - 1 motor <br>- 5V-28V <br> - 5A MAX| [Drotek](https://store-drotek.com/212-brushless-gimbal-controller-l6234.html)<br> [Ebay](https://www.pololu.com/product/1212) | 18€
[<img src="extras/Images/BTS7960B.jpg" style="height:100px">](https://fr.aliexpress.com/item/32965904058.html)| Aideepen BTS7960B| - BTS7960B  chip <br> - 1 motor <br>- 5V-30V <br> - 43A MAX| [Aliexpress](https://fr.aliexpress.com/item/32965904058.html) | 10€


Finally you can also use two 3-phase [BLDC motor drivers](bldc_drivers). Each of the BLDC drivers has 3 half-bridges, from which you will need to use 2. When you combine two of them you will be using 4 output pins out of 6 they have.  
