---
layout: default
title: Hall sensors
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /hall_sensors
nav_order: 2
parent: Position Sensors
grand_parent: Writing the Code
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# Hall sensors setup
<div class="width60">
<img src="extras/Images/hall_schema.jpg" style="width:48.5%;display:inline"><img src="extras/Images/hall.png" style="width:48.5%;display:inline">
</div>

## Step 1. Instantiate `HallSensor` class
To initialize the hall sensors you need to provide the pin numbers for `A`, `B` and `C` (sometimes called `U`,`V` and`W`) channel and the number of pole pairs `pp` of the motor.
```cpp
// Hall sensor instance
// HallSensor(int hallA, int hallB , int cpr, int index)
//  - hallA, hallB, hallC    - HallSensor A, B and C pins
//  - pp                     - pole pairs
HallSensor sensor = HallSensor(2, 3, 4, 11);
```

## Step 2. Configuration

Additionally the hall senso has one more optional parameter you may set, the pullup location. Hall sensors usually require pullups and in cases when your sensor needs one and you don't have one on your hands you can use Arduino pullups. That is set by changing the value of the `sensor.pullup` variable. The default value is set to `Pullup::EXTERN` but if you would like to change it to use the MCU ones do:
```cpp
// use internal pullups
sensor.pullup = Pullup::INTERN;
```
<blockquote class="warning"><p class="heading">Arduino Pullup 20kΩ</p> Be careful when using internal pullups, Arduino has relatively high valued pullups around 20kΩ, which means that you might have some problems for higher velocities (for shorted impulse durations). Recommended pull-up values are in between 1kΩ and 5kΩ.</blockquote>

## Step 3. Interrupt setup
There are two ways you can run hall sensors with Simple FOC library.
- Using [hardware external interrupt](#hardware-external-interrupt) 
   - Arduino UNO(Atmega328) pins `2` and `3`
   - STM32 boards any pin
   - ESP32 any pin
- Using [software pin change interrupt](#software-pin-change-interrupt) by using a library such as [PciManager library](https://github.com/prampec/arduino-pcimanager)
   - Only for Arduino devices (Atmga328 and Atmage2560)

<blockquote class="warning"><p class="heading">Software interrupts</p> Using the hardware external interrupts usually results in better and more reliable performance but software interrupts will work very well for lower velocities. Especially on boards that just don't have enough hardware interrupt pins, having this functionality basically enables FOC on these boards.</blockquote>

### Hardware external interrupt
Arduino UNO has two hardware external interrupt pins, pin `2` and `3`,  Arduino Mega has 6 interrupt pins, pins `2`, `3`, `18`, `19`, `20`and `2` whereas ESP32 and STM32 boards can use all their digital pins as interrupt pins, which makes implementation much easier.

Simple FOC `HallSensor` class already has implemented initialization and sensor `A`, `B` and `C` channel callbacks. 
All you need to do is define two functions `doA()`, `doB()` and `doC()`, the buffering functions of sensor callback functions `sensor.handleA()`, `sensor.handleB()` and  `sensor.handleC()`. 
```cpp
// interrupt routine initialization
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}
void doC(){sensor.handleC();}
```
And provide those functions to the hall sensor interrupt init function `sensor.enableInterrupts()`
```cpp
// enable hall sensor hardware interrupts
sensor.enableInterrupts(doA, doB, doC)
```
You can name the buffering functions as you wish. It is just important to provide them to the `sensor.enableInterrupts()` function. This procedure is a tradeoff in between scalability and simplicity. This allows you to have more than one sensor connected to the same MCU. All you need to do is to instantiate new `HallSensor` class and create new buffer functions. For example:
```cpp
// sensor 1
HallSensor sensor1 = HallSensor(...);
void doA1(){sensor1.handleA();}
void doB1(){sensor1.handleB();}
void doC1(){sensor1.handleC();}
// sensor 2
HallSensor sensor2 = HallSensor(...);
void doA2(){sensor2.handleA();}
void doB2(){sensor2.handleB();}
void doC2(){sensor2.handleC();}

void setup(){
...
  sensor1.init();
  sensor1.enableInterrupts(doA1,doB1,doC1);
  sensor2.init();
  sensor2.enableInterrupts(doA2,doB2,doC2);
...
}
```

### Software pin change interrupt

For Arduino Uno and boards using Atmega328 chipd, we will have to use the software interrupt library to use Hall senors with this library, because we will need three interrupt pins and Atmega328 has only 2.  
I suggest using the [PciManager library](https://github.com/prampec/arduino-pcimanager).

The steps of using this library in code are very similar to [hardware interrupt](#arduino-hardware-external-interrupt).
The SimpleFOC `HallSensor` class still provides you with all the callbacks `A`, `B` and `C` channels but the Simple FOC library will not initialize the interrupts for you. 

In order to use the `PCIManager` library you will need to include it in your code:
```cpp
#include <PciManager.h>
#include <PciListenerImp.h>
```
Next step is the same as before, you will just initialize the new `HallSensor` instance.
```cpp
HallSensor sensor = HallSensor(2, 3, 4, 11);
// A, B and C interrupt callback buffers
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}
void doC(){sensor.handleC();}
```
Then you declare listeners `PciListenerImp `:
```cpp
// sensor interrupt init
PciListenerImp listenA(sensor.pinA, doA);
PciListenerImp listenB(sensor.pinB, doB);
PciListenerImp listenC(sensor.pinC, doC);
```
Finally, after running `sensor.init()` you skip the call of the `sensor.enableInterrupts()` and call the `PCIManager` library to register the interrupts for all the sensor channels.
```cpp
// initialize sensor hardware
sensor.init();
// interrupt initialization
PciManager.registerListener(&listenA);
PciManager.registerListener(&listenB);
PciManager.registerListener(&listenC);
```
And that is it, it is very simple. It if you want more than one sensor, you just initialize the new class instance, create the new `A`, `B` and `C` callbacks, initialize the new listeners. Here is a quick example:
```cpp
// sensor 1
HallSensor sensor1 = HallSensor(2, 3, 4, 11);
void doA1(){sensor1.handleA();}
void doB1(){sensor1.handleB();}
void doC1(){sensor1.handleC();}
PciListenerImp listenC1(sensor1.pinC, doC1);

// sensor 2
HallSensor sensor2 = HallSensor(5, 6, 7, 11);
void doA2(){sensor2.handleA();}
void doB2(){sensor2.handleB();}
void doC2(){sensor2.handleC();}
PciListenerImp listenA2(sensor2.pinA, doA2);
PciListenerImp listenB2(sensor2.pinB, doB2);
PciListenerImp listenC2(sensor2.pinC, doC2);

void setup(){
...
  // sensor 1
  sensor1.init();
  sensor1.enableInterrupts(doA1,doB1); // two hardware interrupts
  PciManager.registerListener(&listenC1); // one software interrupt

  // sensor 2
  sensor2.init();
  PciManager.registerListener(&listenA2);
  PciManager.registerListener(&listenB2);
  PciManager.registerListener(&listenC2);
...
}
```

## Step 4. Using hall sensors in real-time

There are two ways to use hall sensor implemented within this library:
- As motor position sensor for FOC algorithm
- As standalone position sensor

### Position sensor for FOC algorithm

To use the hall sensor with the foc algorithm implemented in this library, once when you have initialized `sensor.init()` it and enabled interrupts `sensor.enableInterrupts(...)` you just have to link it to the BLDC motor by executing:
```cpp
motor.linkSensor(&sensor);
```

### Standalone sensor 

To get the hall sensor angle and velocity at any given time you can use the public methods:
```cpp
class HallSensor{
 public:
    // shaft velocity getter
    float getVelocity();
	  // shaft angle getter
    float getAngle();
}
```

Here is a quick example using only hardware interrupts:
```cpp
#include <SimpleFOC.h>

// Hall sensor instance
// HallSensor(int hallA, int hallB , int cpr, int index)
//  - hallA, hallB, hallC    - HallSensor A, B and C pins
//  - pp                     - pole pairs
HallSensor sensor = HallSensor(2, 3, 4, 11);

// Interrupt routine initialization
// channel A and B callbacks
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}
void doC(){sensor.handleC();}

void setup() {
  // monitoring port
  Serial.begin(115200);

  // check if you need internal pullups
  sensor.pullup = Pullup::EXTERN;
  
  // initialize sensor hardware
  sensor.init();
  // hardware interrupt enable
  sensor.enableInterrupts(doA, doB, doC);

  Serial.println("Sensor ready");
  _delay(1000);
}

void loop() {
  // display the angle and the angular velocity to the terminal
  Serial.print(sensor.getAngle());
  Serial.print("\t");
  Serial.println(sensor.getVelocity());
}
```


Here is a quick example using software interrupts:
```cpp
#include <SimpleFOC.h>

// Hall sensor instance
// HallSensor(int hallA, int hallB , int cpr, int index)
//  - hallA, hallB, hallC    - HallSensor A, B and C pins
//  - pp                     - pole pairs
HallSensor sensor = HallSensor(2, 3, 4, 11);

// Interrupt routine initialization
// channel A and B callbacks
void doA(){sensor.handleA();}
void doB(){sensor.handleB();}
void doC(){sensor.handleC();}

// sensor interrupt init
PciListenerImp listenA(sensor.pinA, doA);
PciListenerImp listenB(sensor.pinB, doB);
PciListenerImp listenC(sensor.pinC, doC);

void setup() {
  // monitoring port
  Serial.begin(115200);

  // check if you need internal pullups
  sensor.pullup = Pullup::EXTERN;
  
  // initialize sensor hardware
  sensor.init();
  // interrupt initialization
  PciManager.registerListener(&listenA);
  PciManager.registerListener(&listenB);
  PciManager.registerListener(&listenC);

  Serial.println("Sensor ready");
  _delay(1000);
}

void loop() {
  // display the angle and the angular velocity to the terminal
  Serial.print(sensor.getAngle());
  Serial.print("\t");
  Serial.println(sensor.getVelocity());
}
```
