---
layout: default
title: Magnetic sensor SPI
parent: Magnetic sensor
grand_parent: Position Sensors
grand_grand_parent: Writing the Code
grand_grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 1
permalink: /magnetic_sensor_spi
---


# SPI Magnetic sensor setup

In order to use your SPI magnetic position sensor with <span class="simple">Simple<span class="foc">FOC</span>library</span> first create an instance of the `MagneticSensorSPI` class:
```cpp
// MagneticSensorSPI(int cs, float _cpr, int _angle_register)
//  cs              - SPI chip select pin 
//  bit_resolution - magnetic sensor resolution
//  angle_register  - (optional) angle read register - default 0x3FFF
MagneticSensorSPI sensor = MagneticSensorSPI(10, 14, 0x3FFF);
```
The parameters of the class are
- `chip_select` - pin number you connected your sensor to be used with the SPI communication, 
- `bit_resolution` - resolution of your sensor (number of bits of the sensor internal counter register) and your
- `angle register` - register number containing angle value. <br>The default `angle_register` number is set to `0x3FFF` as it is the angle register for most of the low cost AS5048/AS5047 sensors. 

Additionally the library allows you to configure SPI communication clock speed and SPI mode by setting the variables:
```cpp
// spi mode (phase/polarity of read/writes) i.e one of SPI_MODE0 | SPI_MODE1 (default) | SPI_MODE2 | SPI_MODE3
sensor.spi_mode = SPI_MODE0;
// speed of the SPI clock signal - default 1MHz
sensor.clock_speed = 500000;
```

Finally after the configuration the only thing you need to do is to call the `init()` function. This function prepares the SPI interface and initializes the sensor hardware. So your magnetic sensor initialization code will look like:
```cpp
MagneticSensorSPI sensor = MagneticSensorSPI(10, 14, 0x3FFF);

void loop(){
  ...
  sensor.spi_mode = SPI_MODE0; // spi mode - OPTIONAL
  sensor.clock_speed = 500000; // spi clock frequency - OPTIONAL
  sensor.init();
  ...
}
```

If you wish to use more than one magnetic sensor, make sure you connect their `chip_select` pins to different arduino pins and follow the same idea as above, here is a simple example:
```cpp
MagneticSensorSPI sensor1 = MagneticSensorSPI(10, 14, 0x3FFF);
MagneticSensorSPI sensor1 = MagneticSensorSPI(9, 14, 0x3FFF);

void loop(){
  ...
  sensor1.init();
  sensor2.init();
  ...
}
```

### Multiple SPI busses 
For the microcontrollers where that is possible, library provides you a way to provide it the SPI bus you would like it to use instead of default `SPI` class.
```cpp
// these are valid pins (mosi, miso, sclk) for 2nd SPI bus on storm32 board (stm32f107rc)
SPIClass SPI_2(PB15, PB14, PB13);
void setup(){
  // init magnetic sensor   
  sensor.init(&SPI_2);
}
```

### Quick configuration for common sensors

For the most common SPI magnetic sensors, the library provides the simplified configuration constructor. Namely for AS5047/AS5147 14-bit SPI sensors and MA70 14-bit SSI sensor.
```cpp
// instance of AS5047/AS5147 sensor
MagneticSensorSPI sensor = MagneticSensorSPI(10, AS5147_SPI);
// instance of MA730 sensor
MagneticSensorSPI sensor = MagneticSensorSPI(10, MA730_SPI);
```
If wish to implement your own quick configuration structure, you will need to create an instance of the structure:
```cpp
struct MagneticSensorSPIConfig_s  {
  int bit_resolution;
  int angle_register;
  int spi_mode; // default SPI_MODE0
  long clock_speed;  // default 1MHz
  int data_start_bit; // default 13
  int command_rw_bit; // default 14
  int command_parity_bit; // default 15
};
```
and provide it to the constructor, here is an example:
```cpp
// 12 bit, 0x3FFF angle register, spi mode 1, 1MHz, default start bit, rw command, and parity bit 
MagneticSensorSPIConfig_s MySensorConfig = {
  .bit_resolution=12, 
  .angle_register=0x3FFF, 
  .spi_mode=SPI_MODE1, 
  .clock_speed=1000000, 
  .data_start_bit=13,
  .command_rw_bit=14,
  .command_parity_bit=15
  }; 

// the sensor class with desired sensor configuration
MagneticSensorSPI sensor = MagneticSensorSPI(10, MySensorConfig);
void setup(){
  sensor.init();
  ...
}
```

Please check the `magnetic_sensor_spi_example.ino` example for a quick test of your sensor. All the features of SPI magnetic sensors are implemented in the `MagneticSensorSPI.cpp/h` files. 


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
class MagneticSensorSPI{
 public:
    // shaft velocity getter
    float getVelocity();
  	// shaft angle getter
    float getAngle();
}
```

Here is a quick example for the AS5047U magnetic sensor with SPI communication:
```cpp
#include <SimpleFOC.h>

// MagneticSensorSPI(int cs, float _cpr, int _angle_register)
// cs              - SPI chip select pin 
// bit_resolution  - sensor resolution
// angle_register  - (optional) angle read register - default 0x3FFF
MagneticSensorSPI as5047u = MagneticSensorSPI(10, 14, 0x3FFF);
// or quick config
MagneticSensorSPI as5047u = MagneticSensorSPI(10, AS4147_SPI);

void setup() {
  // monitoring port
  Serial.begin(115200);

  // initialise magnetic sensor hardware
  as5047u.init();

  Serial.println("as5047u ready");
  _delay(1000);
}

void loop() {
  // display the angle and the angular velocity to the terminal
  Serial.print(as5047u.getAngle());
  Serial.print("\t");
  Serial.println(as5047u.getVelocity());
}
```