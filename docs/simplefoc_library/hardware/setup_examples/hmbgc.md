---
layout: default
title: HMBGC V2.2
parent: Setup examples
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 3
permalink: /hmbgc
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# HMBGC V2.2 example
To use HMBGC controller for vector control (FOC) you need to connect motor to one of the motor terminals and connect the encoder to the analog pins. Following pictures show the schematic of the HMBGC board necessary connections and the one real example of the connection.

<p>	<img src="extras/Images/hmbgc_connection.png" class="width50"> </p>
 
 
## Encoder
<blockquote class="warning"> <p class="heading">Pinout restriction</p>
HMBGC doesn't have access to the Arduino's external interrupt pins <code class="highlighter-rouge">2</code> and <code class="highlighter-rouge">3</code>, moreover the only pins we have access to are analog pins <code class="highlighter-rouge">A0-A7</code>. 
Therefore we need to read the encoder channels using the software interrupt library, please check the  encoder <a href="encoder">code implementation </a> for more information. </blockquote>

Please see the HMBGC code example (`HMBGC_example.ino`) to test all the functionalities.

- Encoder channels `A` and `B` are connected to the pins `A0` and `A1`.
- Optionally if your encoder has `index` signal you can connect it to any available pin, figure shows pin `A2`.  

## Motor
- Motor phases `a`,`b` and `c` are connected directly to the driver outputs
- Motor terminal `M1` uses Arduino pins `9`,`10`,`11` and `M2` uses `3`,`5`,`6`



<blockquote class="danger"> HMBGC board doesn't support magnetic sensors because it doesn't have necessary SPI infrastructure.</blockquote>

## Example connection
<p><img src="extras/Images/hmbgc_v22.jpg" class="width60">
</p>