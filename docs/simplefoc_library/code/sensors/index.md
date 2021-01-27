---
layout: default
title: Position Sensors
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /sensors
nav_order: 3
parent: Writing the Code
has_children: True
has_toc: False
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Position Sensors 
This library supports *off-the-shelf* these types of position sensors: 
- [Encoder](encoder): 
    - Sensor that counts `A`, `B` and `index` channels impulses to estimate the position.
    - Examples: 
        - optical: [OMRON 1000P <i class="fa fa-external-link"></i>](https://www.ebay.com/itm/OMRON-1000P-Incremental-Rotary-Encoder-1000p-r-E6B2-CWZ1X-Differential-Signal/303247826877?hash=item469afa9fbd:g:BsYAAOSwb2hdTCQB)
        - capacitive: [AMT103 CUI <i class="fa fa-external-link"></i>](https://www.mouser.fr/ProductDetail/CUI-Devices/AMT103-V?qs=%2Fha2pyFaduiAsBlScvLoAWHUnKz39jAIpNPVt58AQ0PVb84dpbt53g%3D%3D)
        - magnetic: [AS5047U <i class="fa fa-external-link"></i>](https://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D) - using ABI
- [Magnetic sensor](magnetic_sensor): 
    - Sensor that uses precise magnetic field measurement to estimate the position.
        - They come with multitude of different communication standards such as: SPI, SSI, I2C, ABI, UVW, PWM...
        - **Supported communications:** ([releases <i class="fa fa-tag"></i>](https://github.com/simplefoc/Arduino-FOC/releases)) 
            - SPI, I2C, Analog
            - UVW (*using the hall sensor interface*)
            - ABI (*using the encoder interface*)
    - Examples :  [AS5048A <i class="fa fa-external-link"></i>](https://www.ebay.com/itm/AS5048-Magnetic-Encoder-PWM-SPI-Interface-14-Bit-Precision-For-Brushless-Motor/153636871434?hash=item23c5789d0a:g:oOMAAOSwd-5ddaWQ), [AS5047U <i class="fa fa-external-link"></i>](https://www.mouser.fr/ProductDetail/ams/AS5X47U-TS_EK_AB?qs=sGAEpiMZZMve4%2FbfQkoj%252BBDLPCj82ZLyYIPEtADg0FE%3D), [AS5600 <i class="fa fa-external-link"></i>](https://www.ebay.com/itm/1PC-New-AS5600-magnetic-encoder-sensor-module-12bit-high-precision/303401254431?hash=item46a41fbe1f:g:nVwAAOSwTJJd8zRK)
- [Hall sensors](hall_sensors): 
    - Sensor that estimates rotor position by reading magnet positions on the rotor.
    - Examples :  [49E Hall sonde <i class="fa fa-external-link"></i>](https://fr.aliexpress.com/item/32590021901.html?spm=a2g0o.productlist.0.0.6eec671cZA32JT&algo_pvid=5729f98b-72a0-4cf8-b80a-adac9ecbbd2a&algo_expid=5729f98b-72a0-4cf8-b80a-adac9ecbbd2a-58&btsid=0b8b035915993735716435630eb78b&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) ,   [105 Hall sensor <i class="fa fa-external-link"></i>](https://fr.aliexpress.com/item/32968973849.html?spm=a2g0o.productlist.0.0.2727671c1QF3Xc&algo_pvid=701cd77d-e484-49ca-8ee8-35a76ed246a1&algo_expid=701cd77d-e484-49ca-8ee8-35a76ed246a1-12&btsid=0b8b034e15993753711202685ed51b&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) 

All classes of sensors are implemented in generic way in order to support as many of their versions as possible.

<div class="image_icon width30" >
    <a href="encoder" class="text-center">
        <img src="extras/Images/enc0.jpg" style="width:32%;display:inline"><img src="extras/Images/enc.jpg" style="width:32%;display:inline"><img src="extras/Images/enc1.png" style="width:32%;display:inline">
        <i class="fa fa-external-link-square fa-2x"></i>
        <p >Encoders</p>
    </a>
</div>
<div class="image_icon width30" >
    <a href="magnetic_sensor" class="text-center">
        <img src="extras/Images/mag0.jpg" style="width:32%;display:inline"><img src="extras/Images/mag2.jpg" style="width:32%;display:inline"><img src="extras/Images/mag.jpg" style="width:32%;display:inline">
        <i class="fa fa-external-link-square fa-2x"></i>
        <p >Magnetic sensors</p>
    </a>
</div>
<div class="image_icon width30" >
    <a href="hall_sensors" class="text-center">
        <img src="extras/Images/hall_schema.jpg" style="width:48.5%;display:inline"><img src="extras/Images/hall.png" style="width:48.5%;display:inline">
        <i class="fa fa-external-link-square fa-2x"></i>
        <p >Hall sensors</p>
    </a>
</div>

## Supporting additional sensors
If you are interested to see what does it take to enable your code to run with different type of sensor or with the communication interface that has nto been implemented in the Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> yet, check out the short example [how to implement new sensor source](sensor_support). For more details about the theoretical parts of the code and the source code documentation visit [digging deeper section](digging_deeper).


<h2><i class="fa fa-lg"><svg id="fab-discourse" style="width:20px;fill:#44a8fa" viewBox="0 0 448 512"><path d="M225.9 32C103.3 32 0 130.5 0 252.1 0 256 .1 480 .1 480l225.8-.2c122.7 0 222.1-102.3 222.1-223.9C448 134.3 348.6 32 225.9 32zM224 384c-19.4 0-37.9-4.3-54.4-12.1L88.5 392l22.9-75c-9.8-18.1-15.4-38.9-15.4-61 0-70.7 57.3-128 128-128s128 57.3 128 128-57.3 128-128 128z"></path> </svg></i> <span class="simple">Simple<span class="foc">FOC</span> Community</span></h2>

Finally, if you have implemented a new type od sensor to the library or you are searching for help to implement some specific sensor type don't hesitate to post in [community forum](https://community.simplefoc.com) 

It is always helpful to hear the stories/problems/suggestions of people implementing the code and you might find a lot of answered questions there already! 

<div class="image_icon width80" >
    <a href="https://community.simplefoc.com" target="_blank">
        <img src="extras/Images/community.png" >
        <i class="fa fa-external-link-square fa-2x"></i>
    </a>
</div>
    

