---
layout: default
title: Position Sensors
nav_order: 3
parent: Supported Hardware
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /position_sensors
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Position sensors
The library currently supports ([releases <i class="fa fa-tag"></i>](https://github.com/simplefoc/Arduino-FOC/releases)) types of position sensors: 
- [Encoder](#encoders) 
- [Magnetic sensor](#magnetic-sensors).
- [Hall sensors](#hall-sensors)
- *IMU (in development)*

## Encoders
Encoders are by far most popular position sensors, both in industry and in hobby community. The main benefits are the precision, standardization and very low noise level. The <span class="simple">Simple<span class="foc">FOC</span>library</span> support just about any type of encoder you can find.  Here is nice short video which explains the main working principles of encoders: [YouTube video](https://www.youtube.com/watch?v=qT6FdvcEsMs)

The encoder position tracking (pulse counting) algorithm code efficiency, and especially its implementation on Arduino devices, is one of the main limitations for both performance and smoothness of the FOC algorithm. There is a clear tradeoff in between encoder precision and execution efficiency, therefore it very is important to find a suitable encoder to your application.

Examples  | Description | Link | Price
---- | ---- | ---- | ----
[<img src="extras/Images/enc.jpg"  style="height:100px">](https://www.ebay.com/itm/360-600P-R-Photoelectric-Incremental-Rotary-Encoder-5V-24V-AB-Two-Phases-Shaft/254214673272?hash=item3b30601378:m:mDiuW1F2qXINSH51TqAjhTg)  | Optical encoder<br>2400cpr | [Ebay](https://www.ebay.fr/itm/L6234-Breakout-Board-/153204519965) | 10$
[<img src="extras/Images/enc1.png" style="height:100px">](https://www.ebay.com/itm/HMBGC-V2-0-3-Axle-Gimbal-Controller-Control-Plate-Board-Module-with-Sensor/351497840990?hash=item51d6e7695e:g:BAsAAOSw0QFXBxrZ:rk:1:pf:1) | Optical encoder<br>AMT103 <br> configurable cpr 48-8192 |  [Mouser](https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduiAsBlScvLoAWHUnKz39jAIpNPVt58AQ0PVb84dpbt53g%3D%3D)  | 20$
[<img src="extras/Images/mag.jpg"  style="height:100px">](hhttps://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D) | Margetic encoder <br> AS5047U <br> 16384cpr |  [Mouser](https://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D)<br> [Youtube demo](https://www.youtube.com/watch?v=Gl-DiOqXXJ8)   | 15$



<blockquote class="warning">
<p class="heading">Encoder CPR: Rule of thumb for Arduino UNO</p>
For Arduino UNO, the maximum number of pulses/second should not exceed 20,000. After this value it start to have execution issues. 
Please take this in consideration when choosing the encoder and especially if using more than one motor.<br>
<p class="heading">Example</p>
If your CPR value is 10000, you will be able to spin your motor with max velocity 120rpm - 2 rotations/second 
</blockquote>

      
## Magnetic sensors
Magnetic position sensor has many benefits over the encoders:
- Very efficient position calculation ( no counting ) 
- Time of execution doesn't depend on velocity or number of sensors
- No need for interrupt hardware
- Absolute position value
- Lower price then encoders
- Simple to mount

Magnetic sensors usually come with several different communication protocols:
- ABI (✔️ supported) - *exactly the same as the encoder interface* 
- SPI (✔️ supported)
- I2C (✔️ supported)
- Analog (✔️ supported)
- UVW (✔️ supported)  - *exactly the same as the Hall sensor interface* 
- PWM
- SSI 

PWM and SSI protocol will be tested for their performance and implemented in following steps. 
Please [let us know](contact) if you would be interested in having some other communication implemented!

Here are some of the supported magnetic sensors:

Examples  | Description | Link | Price
---- | ---- | ---- | ----
[<img src="extras/Images/mag.jpg"  style="height:100px">](https://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D) | AS5047<br> SPI/ABI/PWM/UVW <br> 14bit |  [Mouser](https://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D) | 15$
[<img src="extras/Images/mag2.jpg"  style="height:100px">](https://www.ebay.com/itm/AS5048-Magnetic-Encoder-PWM-SPI-Interface-14-Bit-Precision-For-Brushless-Motor/153636871434?hash=item23c5789d0a:g:oOMAAOSwd-5ddaWQ) | AS5048A<br> SPI/PWM <br> absolute <br> 14bit |  [Ebay](https://www.ebay.com/itm/AS5048-Magnetic-Encoder-PWM-SPI-Interface-14-Bit-Precision-For-Brushless-Motor/153636871434?hash=item23c5789d0a:g:oOMAAOSwd-5ddaWQ) | 10$
[<img src="extras/Images/as5600.jpg"  style="height:100px">](https://www.ebay.com/itm/1PC-New-AS5600-magnetic-encoder-sensor-module-12bit-high-precision/303401254431?hash=item46a41fbe1f:g:nVwAAOSwTJJd8zRK) | AS5600 <br> I2C/PWM <br> 12bit | [Ebay](https://www.ebay.com/itm/1PC-New-AS5600-magnetic-encoder-sensor-module-12bit-high-precision/303401254431?hash=item46a41fbe1f:g:nVwAAOSwTJJd8zRK) | 5$ 


<blockquote class="warning"><p class="heading">BEWARE: I2C Pull-ups</p>
Not all the I2C buses need pull-ups. But in some cases they might be necessary. Especially when using STM32 boards. In those cases, you will not be able to communicate with the sensor. 
To enable communication usually 4.7kOhm resistors will be needed in between 5V/3.3V and SCL and SDA channels. Here is a good stack overflow question about this issue :<a href="https://electronics.stackexchange.com/questions/102611/what-happens-if-i-omit-the-pullup-resistors-on-i2c-lines"> link </a>. 
<br>
In the future versions of the <span class="simple">Simple<span class="foc">FOC</span>Shield</span> I will try to include those pullups as well.
</blockquote>

<blockquote class="warning"><p class="heading">BEWARE: UVW interface</p>
If you desire to use UVW interface of the magnetic sensor please be sure to configure your magnetic sensor to have the same pole pair number as the motor. Also beware that the alignment of the magnetic sensor and the motor phases is very important and will probably take some time to achieve. 
</blockquote>
      
## Hall sensors

Hall sensors for FOC control used to be very common in past due to their commutation simplicity. Motor is usually fitted with 3 hall sensors which read the rotor magnet position and can determine electrical angle of the motor with the resolution of 60 degrees. 

Sector | Electrical angle | Hall A | Hall B | Hall C
--- | ---| -- | - | - 
 1| 0-60 | 1 | 0 | 1
 2| 60-120 | 1 | 0 | 0  
 3| 120-180 | 1 | 1 | 0
 4| 180-240 | 0 | 1 | 0 
 5| 240-300 | 0 | 1 | 1  
 3| 300-360 | 0 | 0 | 1

The main benefit of these sensors is that they are really cheap and they can be added on just about any BLDC motor. But nowdays, with emergence of magnetic sensors they are less and less used. One of the main disadvantages of these sensors is that the low speed operation is not smooth due to the relatively high quantization of the angle measurement. 

The main area of application of these senors are electrical motors in all kinds of transportation vehicles. Their motors are often very high-power and relaying on any kind of communication (I2C, SPI, SSI ... ) with such high currents and magnetic fieds is just not an option. Another reason why use hall sensors is if you cannot access the shaft of the motor, or if you cannot mount the sensor coaxial to the motor shaft. Hall sensors are mounted around the motor rotor, which makes them very non-intrusive and easy to integrate. 

Hall sensors ICs you can find at any electronics supplier, all the way from Ebay and Aliexpress to Mouser and Digikey but also in most of local electronics shops.

Examples  | Description | Link | Price
---- | ---- | ---- | ----
[<img src="extras/Images/hall.png"  style="height:100px">](https://fr.aliexpress.com/item/32590021901.html?spm=a2g0o.productlist.0.0.6eec671cZA32JT&algo_pvid=5729f98b-72a0-4cf8-b80a-adac9ecbbd2a&algo_expid=5729f98b-72a0-4cf8-b80a-adac9ecbbd2a-58&btsid=0b8b035915993735716435630eb78b&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) |  49E Hall Sensor (10x) |  [Aliexpress](https://fr.aliexpress.com/item/32590021901.html?spm=a2g0o.productlist.0.0.6eec671cZA32JT&algo_pvid=5729f98b-72a0-4cf8-b80a-adac9ecbbd2a&algo_expid=5729f98b-72a0-4cf8-b80a-adac9ecbbd2a-58&btsid=0b8b035915993735716435630eb78b&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | 1$

You can also consider buying a BLDC motor with integrated Hall effect sensors if this suites your application. 

Examples  | Description | Link | Price
---- | ---- | ---- | ----
[<img src="extras/Images/hall1.png"  style="height:100px">](https://fr.aliexpress.com/item/4000086664014.html?spm=a2g0o.productlist.0.0.338073065g29WW&s=p&ad_pvid=20200905233621305169369584280003211148_6&algo_pvid=e2271fc5-6c48-4ca9-9961-ed620ada16d6&algo_expid=e2271fc5-6c48-4ca9-9961-ed620ada16d6-29&btsid=0b8b034515993741819075226e8e8e&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) |  BLDC motor <br> hall sensors integrated <br> 100W |  [Aliexpress](https://fr.aliexpress.com/item/4000086664014.html?spm=a2g0o.productlist.0.0.338073065g29WW&s=p&ad_pvid=20200905233621305169369584280003211148_6&algo_pvid=e2271fc5-6c48-4ca9-9961-ed620ada16d6&algo_expid=e2271fc5-6c48-4ca9-9961-ed620ada16d6-29&btsid=0b8b034515993741819075226e8e8e&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | 40$
[<img src="extras/Images/hallw.png"  style="height:100px">](https://fr.aliexpress.com/item/4000242695485.html?spm=a2g0o.productlist.0.0.338073065g29WW&algo_pvid=e2271fc5-6c48-4ca9-9961-ed620ada16d6&algo_expid=e2271fc5-6c48-4ca9-9961-ed620ada16d6-17&btsid=0b8b034515993741819075226e8e8e&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) |  BLDC motor <br> hall sensors integrated <br> 30W |  [Aliexpress](https://fr.aliexpress.com/item/4000242695485.html?spm=a2g0o.productlist.0.0.338073065g29WW&algo_pvid=e2271fc5-6c48-4ca9-9961-ed620ada16d6&algo_expid=e2271fc5-6c48-4ca9-9961-ed620ada16d6-17&btsid=0b8b034515993741819075226e8e8e&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | 25$

<blockquote class="warning"><p class="heading">Motor choice</p>
Make sure to read <a href="motors">supported motor docs</a> before you buy your BLDC motor.
</blockquote>
      