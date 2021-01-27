---
layout: default
title: L6234 Breakout Board
parent: Setup examples
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 2
permalink: /arduino_l6234
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Arduino UNO + L6234 driver
[Drotek's L6234 breakout board](https://store-drotek.com/212-brushless-gimbal-controller-l6234.html) is a very minimalistic 3-phase BLDC motor driver and is very suitable for jump-starting your FOC experience. Here we show two example connections using this board and Arduino UNO:
- [Arduino UNO + Drotek L6134 + Encoder](#encoder-example)
- [Arduino UNO + Drotek L6234 + Margentic sensor AS5048A](#magnetic-sensor-as5048-example)

## Encoder example
<p> <img src="extras/Images/arduino_connection.png" class="width60"></p>  

### Encoder
- Encoder channels `A` and `B` are connected to the Arduino's external interrupt pins `2` and `3`. 
- Optionally if your encoder has `index` signal you can connect it to any available pin, figure shows pin `4`.
  - For Arduino UNO and similar broads which don't have 3 hardware interrupts, if you can choose, preferably connect index pin to pins `A0-A5` due to the interrupt routine, it will have better performance (but any other pin will work as well).  
  - Otherwise if you are using different board and have 3 hardware interrupt pins connect the index pin to one of them.

### L6234 breakout board 
- Connected to the arduino pins `9`,`10` and `11` (you can use also pins `5` and `6`).  
- Additionally you can connect the `enable` pin to the any digital pin of the arduino the picture shows pin `8` but this is optional. You can connect the driver enable directly to 5v. 
- Make sure you connect the common ground of the power supply and your Arduino

### Motor
- Motor phases `a`, `b` and `c` are connected directly to the driver outputs

### Example connection
<p><img src="extras/Images/uno_l6234.jpg" class="width60"></p>

## Magnetic sensor AS5048 example

<p>
 <img src="extras/Images/arduino_connection_magnetic.png" class="width50">
</p>  

### Magnetic sensor
- Magnetic sensor's (AS5048) SPI interface signals `SCK`, `MISO` and `MOSI` are connected to the Arduino's `SPI` pins (Arduino UNO `13`,`12` and `11`). 
  - If the application requires more than one sensor all of them are connected to the same pins of the Arduino.
- The `chip select` pin is connected to the desired pin. Each sensor connected to the same Arduino has to have unique chip select pin.

### L6234 breakout board 
- Connected to the arduino pins `3`,`5` and `6` (you can use also pin `9` and `10`, but pin `11` is taken by the SPI interface).  
- Additionally you can connect the `enable` pin to the any digital pin of the arduino the picture shows pin `2` but this is optional. You can connect the driver enable directly to 5v. 
- Make sure you connect the common ground of the power supply and your Arduino

### Motor
- Motor phases `a`, `b` and `c` are connected directly to the driver outputs

<blockquote class="info"> <p class="heading">Alignment</p>
Motor phases <code class="highlighter-rouge">a</code>,<code class="highlighter-rouge">b</code>,<code class="highlighter-rouge">c</code> and encoder channels <code class="highlighter-rouge">A</code> and <code class="highlighter-rouge">B</code> and the magnetic sensor counting direction have to be oriented right for the algorithm to work. But don't worry about it too much. Connect it initially as you wish and then if the motor locks in place inverse <code class="highlighter-rouge">a</code> and <code class="highlighter-rouge">b</code> of the motor, that should be enough.
</blockquote>


