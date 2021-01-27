---
layout: default
title: Magnetic sensor I2C
parent: Magnetic sensor
grand_parent: Position Sensors
grand_grand_parent: Writing the Code
grand_grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 2
permalink: /magnetic_sensor_i2c
---


# I2C Magnetic sensor setup

Now in order to use your I2C magnetic position sensor with <span class="simple">Simple<span class="foc">FOC</span>library</span> first create an instance of the `MagneticSensorI2C` class:
```cpp
// MagneticSensorI2C(uint8_t _chip_address, float _cpr, uint8_t _angle_register_msb)
//  chip_address         - I2C chip address
//  bit_resolution       - resolution of the sensor
//  angle_register_msb   - angle read register msb
//  bits_used_msb        - number of used bits in msb register
MagneticSensorI2C sensor = MagneticSensorI2C(0x36, 12, 0x0E, 4);
```

The parameters of the class are:
 - `chip_address` - I2C address of the magnetic sensor 
 - `bit_resolution` - resolution of your sensor (number of bits of the sensor internal counter register) and your
 - `angle_register_msb` - register number containing the MSB part of the angle value. ( ex. AS5600 - `0x0E`, AS5048 - `0xFE` ) 
 - `bits_used_msb` - number of used bits in MSB register

<blockquote class="info"> <p class="heading">How to find the MSB register values and used bits value?</p>

Since I2C registers are Byte(8-bit) registers they contain the 12-14 bit angle representation separated in two Byte addresses,  MSB(most significant byte) and LSB(least significant byte).
To instantiate the I2C magnetic sensor you need to provide the MSB register address and tell the library how is the angle value separated in the two registers. By specifying the number of bits used in the MSB register and specifying the <code class="highlighter-rouge">bit_resolution</code> value the library can calculate how many bits are used in the LSB register and can reconstruct the complete angle value.
<br>
Example MSB/LSB division:
<ul>
<li>AS5600 (12 bit) - MSB (4bit), LSB (8bit)</li>
<li>AS5048B (14bit): - MSB (8bit), LSB(6bit)</li>
</ul>
</blockquote>
<blockquote class="info">
<p class="heading">AS5600 example</p>
<ul>
  <li> First we open AS5600 sensor's <a href="https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf" target="_blank"> datasheet <i class="fa fa-external-link"></i></a>.</li>
  <li> In the datasheet we find the table of I2C registers (page 18)</li>
  <li> The <code class="highlighter-rouge">angle_register_msb</code> value is the first (upper in datasheet) register value - <code class="highlighter-rouge">0x0E</code></li>
  <li> The <code class="highlighter-rouge">bits_used_msb</code> value is number of bits in the MSB register - <code class="highlighter-rouge">4</code></li>
</ul>
</blockquote>

Finally, after the configuration, you just call the `init()` function. This function prepares the I2C interface and configures the sensor hardware. So your magnetic sensor initialization code will look like:
```cpp
MagneticSensorI2C sensor = MagneticSensorI2C(0x36, 12, 0x0E, 4);

void loop(){
  ...
  sensor.init();
  ...
}
```

If you wish to use more than one magnetic sensor using SPI interface, make sure your sensors have different addresses, here is a simple example:
```cpp
MagneticSensorI2C sensor1 = MagneticSensorI2C(0x36, 12, 0x0E, 4);
MagneticSensorI2C sensor2 = MagneticSensorI2C(0x37, 12, 0x0E, 4);

void loop(){
  ...
  sensor1.init();
  sensor2.init();
  ...
}
```


### Multiple I2C busses 

Additionally, the library enables you to already configured I2C bus to the sensor using the `init()` function.
```cpp
Wire.setClock(400000);
Wire.begin();

// providing 
sensor.init(&Wire)
```
For microcontrollers where that is possible (not for Arduino UNO) you can use different I2C busses for multiple I2C sensors. See the examples in `utils > sensor_test > magnetic_sensors > magnetic_sensor_i2c_dual_bus_examples`. 

###  Quick configuration for common sensors

For the most common I2C magnetic sensors, the library provides the simplified configuration constructor. Namely for AS5600 12-bit sensor and AS5048 14-bit sensor.
```cpp
// instance of AS5600 sensor
MagneticSensorI2C sensor2 = MagneticSensorI2C(AS5600_I2C);
// instance of AS5048B sensor
MagneticSensorI2C sensor2 = MagneticSensorI2C(AS5048_I2C);
```
If wish to implement your own quick configuration structure, you will need to create an instance of the structure:
```cpp
struct MagneticSensorI2CConfig_s  {
  int chip_address;
  int bit_resolution; 
  int angle_register;
  int data_start_bit; 
};
```
and provide it to the constructor, here is an example:
```cpp
// configuration for AS5600 sensor
MagneticSensorI2CConfig_s MySensorConfig = {
  .chip_address = 0x36, 
  .bit_resolution = 12, 
  .angle_register=0x0E, 
  .data_start_bit=11
  }; 

// the sensor class with desired sensor configuration
MagneticSensorI2C sensor = MagneticSensorI2C(MySensorConfig);
void setup(){
  sensor.init();
  ...
}
```

Please check the `magnetic_sensor_i2_example.ino` example for a quick test of your sensor. All the features of I2C magnetic sensors are implemented in the `MagneticSensorI2C.cpp/h` files. 

## Using magnetic sensor in real-time

There are two ways to use magnetic sensor implemented within this library:
- As motor position sensor for FOC algorithm
- As standalone position sensor

### Position sensor for FOC algorithm

To use the ensor with the FOC algorithm implemented in this library, once when you have initialized `sensor.init()` you just need to link it to the BLDC motor by executing:
```cpp
motor.linkSensor(&sensor);
```

### Standalone sensor 

To get the magnetic sensor angle and velocity at any given time you can use the public methods:
```cpp
class MagneticSensorI2C{
 public:
    // shaft velocity getter
    float getVelocity();
  	// shaft angle getter
    float getAngle();
}
```

Here is a quick example for AS5600 magnetic sensor with I2C communication:
```cpp
#include <SimpleFOC.h>

// MagneticSensorI2C(uint8_t _chip_address, float _cpr, uint8_t _angle_register_msb)
//  chip_address         - I2C chip address
//  bit_resolution       - resolution of the sensor
//  angle_register_msb   - angle read register msb
//  bits_used_msb        - number of used bits in msb register
MagneticSensorI2C as5600 = MagneticSensorI2C(0x36, 12, 0x0E, 4);
// or quick config
MagneticSensorI2C as5600 = MagneticSensorI2C(AS5600_I2C);

void setup() {
  // monitoring port
  Serial.begin(115200);

  // init magnetic sensor hardware
  as5600.init();

  Serial.println("AS5600 ready");
  _delay(1000);
}

void loop() {
  // display the angle and the angular velocity to the terminal
  Serial.print(as5600.getAngle());
  Serial.print("\t");
  Serial.println(as5600.getVelocity());
}
```