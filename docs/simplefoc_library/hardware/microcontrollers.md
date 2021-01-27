---
layout: default
title: Microcontrollers
parent: Supported Hardware
nav_order: 4
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /microcontrollers
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Supported microcontrollers


Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> supports Arduino,  STM32 and ESP32 devices *off-the-shelf*, using Arduino IDE, and with small modifications many more...<br>
This library has been ported to Maple boards, Arduino DUEs and even to *strom32 BGC v1.31*, so I hope to be able to support more boards in future. 😃

## Arduino support

 Board | Name | Specifications | Link | Price
---- | --- | --- | --- | --- | ---
[<img src="extras/Images/arduino_uno.jpg" class="imgtable150">](https://store.arduino.cc/arduino-uno-rev3-smd) | Arduino UNO | ATMega328 <br>- 5V logic<br> - 6 PWMs<br>- 2 interrupts<br>- 16Mhz| [Ebay](https://www.ebay.com/itm/NEW-Arduino-UNO-R3-ATMEGA328P-CH340G-Microcontroller-Board-Bootloader-USB/323962801627?hash=item4b6db00ddb:g:ihUAAOSwLc1dNWCd)<br> [Arudino Store](https://store.arduino.cc/arduino-uno-rev3-smd) | 5€<br>25€ 
[<img src="extras/Images/mega.png" class="imgtable150">](https://store.arduino.cc/arduino-mega-2560-rev3) | Arduino Mega |  ATMega2560 <br>- 5V logic<br> - 15 PWMs<br>- 6 interrupts<br>- 16Mhz | [Ebay](https://www.ebay.com/itm/MEGA-2560-R3-Development-Board-CH340G-ATMEGA-2560-Kit-USB-Cable-For-Arduino-New/253764643649?epid=25019988960&_trkparms=ispr%3D1&hash=item3b158d2b41:g:C44AAOSwomJapjfF&enc=AQAEAAACYIQvEcHUrT7nmUC3yY5qbPyaBN1nJEDYW8MyypsJPgXK3AqiNsU0sSphPu4g6Qid31UfuUmxbibd03S6nwGFOtPRQtA6b7fwyQa%2BlHjHz58lNHKPszpYYTTo0kkJEDqmhf4Wiz0dmrGPE5aFjKQswyzpK0%2FagGHK8e518kkvgI15vEk3BEXEnW%2BgpNypJKacwMKe1INf06jl%2BrC%2FW50ef2gL1FPUQyUq9fK4Rm4tPSr28E52usHYczBDbdMdghFUExt3Ge%2B0iSj4t%2BcsyM2NGC%2BjCDDA8FBe3W5K8wg80e2DQwtM1R8Bpxrt6qJdyWZWigo8m4dpWLS%2Brmys9YJWASnU6mnFZoy4SLPUBLFK560rONYnB7aPohtZNJ%2BjCJPDLOQISm6tmGZVF5fMNu6iLYwYG8WG7J3c7rGVeUiDnJdf%2Fz68BNLAvth%2FnSoI9w7Jau%2Fd19gx3WYQbxgipDAmxgrVhGYlPrEvTSqCLIno1u3W%2FTI7FhSpNW%2Bgzw94kG%2FFgR9ieLRyv9p0w%2BYY3rrJepqCtlCJNkKflPpj4WAcXxBHHaQLJOr2mr7E2wRdwTBstIdCtoMmIp%2BjTRJFApIoT7fnEEIcMZyfLvbQZtemIQKHxpuibWKjQZU99awWsbMBlE0SRWaxRhML5YGORIjmgbIUyFdy9fiWHDcRpoKQJPsV6N3HUkRg9yU9cZ0m5w4ywXpiv7vHA8JFZg7hy5INiPGWwcxTRabpanq%2FWCB%2Bb4AN6%2BFP4%2Bqes86XVNe0YemDM1cBQWiHHjGxAHbw8gWuCZvXoc7XsJE5lrmQTsB1%2F%2FG6&checksum=253764643649687cd36869924033b58b75e082ef568d)<br>[Arduino Store](https://store.arduino.cc/arduino-mega-2560-rev3) | 10€<br>35€ 
[<img src="extras/Images/nano.png" class="imgtable150">](https://www.ebay.com/itm/Nano-V3-0-USB-ATmega328P-AU-16MHz-5V-CH340G-Micro-Controller-Board-For-Arduino/223471184608?hash=item3407ebaae0:g:-gMAAOSwdzBcpfIA) | Arduino NANO |  ATMega328<br> -5V logic<br> - 6 PWMs <br>- 2 interrupts<br> - 16Mhz  | [Ebay](https://www.ebay.com/itm/Nano-V3-0-USB-ATmega328P-AU-16MHz-5V-CH340G-Micro-Controller-Board-For-Arduino/223471184608?hash=item3407ebaae0:g:-gMAAOSwdzBcpfIA) | 3€

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> has stared as an Arduino UNO library and it has been optimized to be used with these types of devices. But it is not restricted to the Arduino UNOs, you will be able to run it on Arduino MEGA and basically any board with ATMega328 chip or ATMega2560. Such as Arduino Nano, Arduino pro-mini and similar.

Board | Name | Specifications | Link | Price
---- | --- | --- | --- | --- | ---
[<img src="extras/Images/pinout.jpg" class="imgtable150">](https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ:rk:1:pf:1) | HMBGC V2.2 |  ATMega328<br> - 5V logic<br> - 6 PWMs <br>- 0 interrupts<br> - No SPI<br>- 16Mhz <br> - 2x BLDC driver | [Ebay](https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ:rk:1:pf:1) | 20€
[<img src="extras/Images/bgc_30.jpg" class="imgtable150">](https://fr.aliexpress.com/item/4000411471994.html?spm=a2g0o.productlist.0.0.5d047d57y4zGC4&algo_pvid=861ada4b-b12f-4019-be84-fae9870a12ed&algo_expid=861ada4b-b12f-4019-be84-fae9870a12ed-1&btsid=0ab6f83a15906954691168349e30d7&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | BGC 3.0 |  ATMega328<br>- 5V logic<br> - 6 PWMs <br>- 0 interrupts<br> - 16Mhz <br> - 2x BLDC driver | [Aliexpress](https://fr.aliexpress.com/item/4000411471994.html?spm=a2g0o.productlist.0.0.5d047d57y4zGC4&algo_pvid=861ada4b-b12f-4019-be84-fae9870a12ed&algo_expid=861ada4b-b12f-4019-be84-fae9870a12ed-1&btsid=0ab6f83a15906954691168349e30d7&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | 10€

One of the interesting example boards for beginners are the gimbal controller boards based on the ATMega328 chips that usually have integrated BLDC driver hardware for running two motors. Their performance is limited but they are great initial setup.

## STM32 boards support

 Board | Name | Specifications | Link | Price
---- | --- | --- | --- | --- | ---
[<img src="extras/Images/nucleo.jpg" class="imgtable150">](https://www.mouser.fr/ProductDetail/STMicroelectronics/NUCLEO-F446RE?qs=%2Fha2pyFaduj0LE%252BzmDN2WNd7nDNNMR7%2Fr%2FThuKnpWrd0IvwHkOHrpg%3D%3D) | Nucleo-64 boards | (ex. Nucleo F446RE)<br>- 3.3V logic<br> - 20 PWMs <br>- all pins interrupts<br>- 180Mhz | [Mouser](https://www.mouser.fr/ProductDetail/STMicroelectronics/NUCLEO-F446RE?qs=%2Fha2pyFaduj0LE%252BzmDN2WNd7nDNNMR7%2Fr%2FThuKnpWrd0IvwHkOHrpg%3D%3D) | 15€
[<img src="extras/Images/bluepill.jpg" class="imgtable150">](https://www.ebay.com/itm/STM32F103C8T6-ARM-STM32-Dev-Development-Board-Module-Blue-Pill-BluePill/292145343898?hash=item4405382d9a:g:nZoAAOSwH-dZ6oaf) | Bluepill | (ex. STM32F103C8)<br>- 3.3V logic<br> - 15 PWMs <br>- all pins interrupts<br>- 72Mhz | [Ebay](https://www.ebay.com/itm/STM32F103C8T6-ARM-STM32-Dev-Development-Board-Module-Blue-Pill-BluePill/292145343898?hash=item4405382d9a:g:nZoAAOSwH-dZ6oaf) | 5€

The STM32 boards are supported using the [STM32Duino package](https://github.com/stm32duino), it is completely open-source and can be installed directly through the `Arduino Board Manager`.
Please check the [STM32Duino wiki](https://github.com/stm32duino/wiki/wiki/Getting-Started) to see a detailed guide how to install the package and all its functionalities.


## ESP32 boards support

 Board | Name | Specifications | Link | Price
---- | --- | --- | --- | --- | ---
[<img src="extras/Images/esp32.jpg" class="imgtable150">](https://www.ebay.com/itm/Espressif-ESP32-WLAN-Dev-Kit-Board-Development-Bluetooth-Wifi-v1-WROOM32-NodeMCU/253059783728?hash=item3aeb89dc30:g:5-8AAOSwAThb3MaZ) | ESP32 | (ex. NodeMCU)<br>- 3,3V / 5V logic<br> - 16 PWMs <br>- all pins interrupts <br>- 240MHz <br> - Wifi + Bluetooth | [Ebay](https://www.ebay.com/itm/Espressif-ESP32-WLAN-Dev-Kit-Board-Development-Bluetooth-Wifi-v1-WROOM32-NodeMCU/253059783728?hash=item3aeb89dc30:g:5-8AAOSwAThb3MaZ) | 10€

ESP32 boards are supported using [arduino-esp32](https://github.com/espressif/arduino-esp32) package, it is open source software provided by the espressif. You can download the support package through the `Arduino Board Manager` by searching for `esp32` or follow the instruction of their webpage [package installation](https://github.com/espressif/arduino-esp32#installation-instructions).

There is just one more thing you need to do in order to s setup the ESP32 boards with the <span class="simple">Simple<span class="foc">FOC</span>library</span>. Since ESP32 support in Arduino IDE is quiet recent, there are some known bugs. You can find the github issue [here](https://github.com/espressif/arduino-esp32/issues/3743). One of them is closely related to the `MCPWM` class which we need to use to have smooth motor control with the ESP32. In order to resolve this bug you just need to replace one header file in the arduino-esp32 package, file `mcpwm.h`. 

This file is usually placed in (Windows):

`C:\Users\(you user name)\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\tools\sdk\include\driver\driver`

Navigate to this directory and replace the file `mcpwm.h` with the file that you can download beneath:

- <b><a class="docs_link" href="extras/esp32/mcpwm.h" download="mcpwm.h"><i class="fa fa-file"></i> mcpwm.h</a></b> - New fixed header file

Now your ESP32 code should be compiling and you are ready to go!


# Choosing the microcontroller
Even though all the MCUs from the table above (and many more) are supported in the library and all of the will work with most of the BLDC motors+BLDC driver+sensor combinations, their performance will not be the same. So here is a quick guide how to choose which MCU to use.

MCU | `loopFOC() + move()` - encoder | `loopFOC() + move()` - magnetic sensor SPI | `loopFOC() + move()` - magnetic sensor I2C
--- | --- | --- | --- |
HMBGC V2.2 | 800us (ipr = 0),  ~10ms (ipr > 10000) | (doesn't support SPI) | 1100us
Arduino UNO | 800us (ipr = 0),  ~10ms (ipr > 20000) | 1200us | 1100us
Stm32 Bluepill | 200us (ipr = 0), ~1ms (ipr > 50000) | 300us | to be tested
Stm32 Nucleo-64 | 100us (ipr = 0), ~500us (ipr > 50000) | 200us | to be tested
ESP32 | to be tested | to be tested | to be tested

*ipr = interrupt callbacks per second.


In the table above you can a comparison of the execution times of the FOC loop for different MCUs. When you are deciding which MCU to use with your project please make sure that your loop execution time `loopFOC() + move()`, in the worst case, will not be greater 3-4ms. And for optimal performance your loop time should be under 2ms. Make sure to account for multiple motors.

## Gimbal controllers
Gimbal controllers are the most simple and surely the cheapest solution for running FOC algorithm with your gimbal motor. They are perfect for smooth position/velocity controlling two BLDC motors with sensors if you don't have high constraints on dynamics. Their main disadvantage is that they use all the external interrupt pins for PWM signals and therefore you cannot access them from outside. That would mean that even if you only need one motor (3PWMs) you will still not be able to use pin `2` and `3` for encoder `A` and `B` signals. This means, if you are planing to use encoders with these boards you will need to use software interrupts. The good news is that this will work, the bad news is that the performance of counting encoder signals will be impaired. So I would suggest you to use Magnetic sensors with communication interface (SPI, I2C...) with these boards if possible.  

Don't let this discourage you from using the gimbal controllers with FOC, just be aware of possible side-effects when deciding which motor and sensor to use. 

<blockquote class="warning"> Make sure your gimbal controller has communication interface pins you need, available before buying it. </blockquote>

## Arduino MCUs
Arduino devices, such as UNO,MEGA,NANO and similar, are probably the most commonly used microcontrollers there is, and therefore probably with this library as well. The simplicity of using these boards si incomparable. If you are planning to run this library with the Arduino device I would certainly suggest you to think about using Magnetic sensors instead of encoders. Encoders are highly inefficient sensors (at least their implementation for Arduino UNO & MEGA) and due to constant counting the interrupt signals of the encoder produces a large difference of execution time depending on velocities you are driving your motor. 

<blockquote class="warning">
<p class="heading">Encoder CPR: Rule of thumb for Arduino UNO/MEGA</p>
For Arduino UNO, the maximum number of pulses/second should not exceed 20,000. After this value it start to have execution issues. 
Please take this in consideration when choosing the encoder and especially if using more than one motor.<br>
<p class="heading">Example</p>
If your CPR value is 10000, you will be able to spin your motor with max velocity 120rpm - 2 rotations/second 
</blockquote>

Additionally Arduino UNO has only two encoder interrupt pins and if you are running two motors with encoders on your Arduino UNO you will be forced to use software interrupt callback which introduce additional augmentation in execution times. Arduino MEGA has 6 interrupts and you should not have this problem.

This library will enable you to use Arduino UNO/MEGA as your FOC brain and you can still do a lot of cool stuff with it even with two motors. Just be aware of the role of thumb if you are using the encoders. 

## Stm32 devices
Stm32 devices are probably the best choice for the FOC implementation MCU. They are very powerful and have many external interrupt pins. They don't loose too much performance due to counting and have much lower loop times making the FOC algorithm much smoother. Stm32 Bluepill can run up to 4 BLDC motors without a problem and Nucleo-64 can run 6+.

The biggest problem of using the STM32 boards in the community is the complexity of programming these devices. But since they have been integrated into the Arduino IDE even this is not a problem any more. Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> code for stm32 devices is exactly the same as for Arduino UNO except the different pin numbers. Therefore I strongly urge you to consider using these devices in your projects because the results are awesome. 😃

<blockquote class="info"> Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is completely compatible with Nucleo-64 devices. And from the shield version 1.3.1 you will be able to stack 2 of the shields with the Nucleo boards as well.</blockquote>

## ESP32 devices
ESP32 devices are very interesting choice for running this library. They have superb communication interfaces and they move the user interaction with the motor  to the next level. ESP32 devices are, in theory, capable of running 4 BLDC motors at the same time with this library. ANd their performance will be much better than the one of the Arduino devices. Especially since they don't have problems with external interrupt limitations.

There are two main problems when using ESP32 boards with for real time motor control.

- ESP32 boards were not originally designed for precise real-time control tasks, they have exceptional communication capabilities, therefore sometimes you can have some strange problems due to this fact. But in 99% of situations this board will perform exceptionally well, it is just if you were planning bring it to its limits when the strange things can happen.
- The other problem of this board is the pinout limitations. Make sure, if you are new with the ESP32, that you watch this [YouTube video](https://www.youtube.com/watch?v=c0tMGlJVmkw). On startup some of the GPIOs have to be in specific states in order to the ESP32 to boot normally. But this is not a big problem once you get used to it!

This board has a lot of benefits and it seems like we will see much more of it in the domain of real-time motor control in future.



<h2><i class="fa fa-lg"><svg id="fab-discourse" style="width:20px;fill:#44a8fa" viewBox="0 0 448 512"><path d="M225.9 32C103.3 32 0 130.5 0 252.1 0 256 .1 480 .1 480l225.8-.2c122.7 0 222.1-102.3 222.1-223.9C448 134.3 348.6 32 225.9 32zM224 384c-19.4 0-37.9-4.3-54.4-12.1L88.5 392l22.9-75c-9.8-18.1-15.4-38.9-15.4-61 0-70.7 57.3-128 128-128s128 57.3 128 128-57.3 128-128 128z"></path> </svg></i> <span class="simple">Simple<span class="foc">FOC</span> Community</span></h2>

If you have ported the library to another device or you are searching for help to port it to some specific device don't hesitate to post in [community forum](https://community.simplefoc.com) 

It is always helpful to hear the stories/problems/suggestions of people implementing the code and you might find a lot of answered questions there already! 

<div class="image_icon width80" >
    <a href="https://community.simplefoc.com" target="_blank">
        <img src="extras/Images/community.png" >
        <i class="fa fa-external-link-square fa-2x"></i>
    </a>
</div>
