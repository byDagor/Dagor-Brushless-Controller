---
layout: default
title: Sensor support
parent: Library Source
grand_parent: Digging deeper
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
nav_order: 3
permalink: /sensor_support
---

# Supporting additional sensors

In order to be able to use a new type of sensor with the FOC algorithm implemented with Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>, it needs to be wrapped in a class which extends the `Sensor` class. And it needs to implement just few generic functions.
```cpp
class Sensor{
public:
    // get current angle (rad) 
    virtual float getAngle();
    // get current angular velocity (rad/s)
    virtual float getVelocity();
    // set current angle as zero angle 
    // return the angle [rad] difference
    virtual float initRelativeZero();
    // set absolute zero angle as zero angle
    // return the angle [rad] difference
    virtual float initAbsoluteZero();

    // returns 0 if it has no absolute 0 measurement
    // 0 - incremental encoder without index
    // 1 - encoder with index & magnetic sensors
    virtual int hasAbsoluteZero();
    // returns 0 if it does need search for absolute zero
    // 0 - magnetic sensor (& encoder with index which is found)
    // 1 - encoder with index (with index not found yet)
    virtual int needsAbsoluteZeroSearch();
}
```

## Step 1. Header file `MySensor.h `
Lets make a mockup example of the new sensor implementation. We start with the `MySensor.h ` file: 

```cpp
#include <SimpleFOC.h>

class MySensor: public Sensor{
 public:
    MySensor(...);

    // initialize the sensor hardware
    void init();

    // angle offset
    float zero_offset;
    // angle
    float current_angle;

    // Abstract functions of the Sensor class implementation
    // get current angle (rad) 
    float getAngle();
    // get current angular velocity (rad/s) 
    float getVelocity();
    // set current angle as zero angle 
    float initRelativeZero();
    // set absolute zero angle as zero angle ( if available )
    float initAbsoluteZero();
    // returns 0 if it has no absolute 0 measurement
    int hasAbsoluteZero();
    // returns 0 if it does need search for absolute zero
    int needsAbsoluteZeroSearch();
 private:
};
```

## Step 2. Class implementation file `MySensor.cpp`
Now let's implement the `MySensor.cpp` file:
```cpp
#include "MySensor.h"

MySensor::MySensor(...){
    // define all the necessary sensor variables
    // for example
    current_angle = 0;
    zero_offset = 0;
}
MySensor::init(){
    // setup all the needed sensor hardware 
    // for example
    sensor.hardwareInit();
}

// Abstract functions of the Sensor class implementation
// get current angle (rad) 
MySensor::getAngle(){
    // get the position value directly from the sensor
    // for example
    current_angle = sensor.angle() - zero_offset;
    return current_angle;
}
// get current angular velocity (rad/s) 
MySensor::getVelocity(){
    // calculate the velocity value)
    // - get it directly from the sensor 
    // for example
    float velocity = sensor.velocity();
    // - or calculate it for example 
    // for example
    float velocity = (current_angle - previous_angle)/T;
    return velocity;
}
// set current angle as zero angle 
// return offset
MySensor::initRelativeZero(){
    zero_offset = current_angle;
    return -zero_offset;
}
// set absolute zero angle as zero angle 
// return the offset from relative zero
MySensor::initAbsoluteZero(){
    return -zero_offset;
}
// return 1 - absolute sensor
MySensor::hasAbsoluteZero(){
    return 1;
}
// return 0 - doesn't need search
MySensor::needsAbsoluteZeroSearch(){
    return 0;
}
```

## Step 3. Arduino program
Finally we will be able to use it in Arduino code:
```cpp
#include <SimpleFOC.h>
#include "MySensor.h"

// instantiate the MySensor
MySensor my_sensor = MySensor(...);

// instantiate the motor
BLDCMotor motor = BLDCMotor(...)

// driver
BLDCDriver3PWM driver = BLDCDriver3PWM(...)

void setup(){
    // init MySensor position tracking
    my_sensor.init();

    // link MySensor with the motor
    motor.linkSensor(&my_sensor);

    // driver config
    driver.init();
    motor.linkDriver(&driver);

    // get ready for FOC
    motor.init();
    motor.initFOC();
}
void loop(){
    // do FOC
    motor.loopFOC();
    motor.move(target);
}

```




