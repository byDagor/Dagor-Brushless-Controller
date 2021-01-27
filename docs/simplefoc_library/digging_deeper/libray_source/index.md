---
layout: default
title: Library Source
nav_order: 1
parent: Digging deeper
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /source_code
has_children: True
has_toc: false
---

# Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> source code [v2.0.2](https://github.com/simplefoc/Arduino-FOC/releases)
The arduino library code is organized into the standard [Arduino library structure](https://github.com/arduino/Arduino/wiki/Library-Manager-FAQ). 
The library contains FOC implementation for two types of BLDC motors, standard three phase BLDC motor in the class `BLDCMotor` and 2 phase stepper motors `StepperMotor`. The library implements nu;erous position sensors and they are all placed in the `senors` directory as well as drivers which are in the `drivers` directory. Finally all the utility functions and classes are placed in the `common` folder. 
## Arduino library source structure
```sh
| src
| ├─ SimpleFOC.h               # Main include file
| | 
| ├─ BLDCMotor.cpp/h           # BLDC motor handling class  
| ├─ StepperMotor.cpp/h        # Stepper motor handling class 
| |
│ ├─── common                  # Contains all the common utility classes and functions
| ├─── drivers                 # PWM setting and driver handling specific code
| ├─── sensors                 # Position sensor specific code
```

<blockquote class="info">For more info visit <a href="http://source.simplefoc.com/" target="_blank"> full source code documentation <i class="fa fa-external-link fa-sm"></i></a></blockquote>

## Motors
### `BLDCMotor.cpp/h`
BLDCMotor class implementation
- FOC algorithm implementation
- Motion control implementation

### `StepperMotor.cpp/h`
BLDCMotor class implementation
- FOC algorithm implementation
- Motion control implementation


<blockquote class="info"><a href="foc_implementation"><i class="fa fa-copy"></i> FOC implementation details</a> - Documentation of the procedures and detailed explanations of the code implementing FOC algorithm 
</blockquote>
<blockquote class="info">
     <a href="motion_control_implementation"><i class="fa fa-copy"></i> Motion control implementation details</a> - Documentation of the motion control algorithms and code implementation choices
</blockquote><blockquote class="info">
     <a href="commands_source"><i class="fa fa-copy"></i> Motor commands implementation </a> - Documentation of the motor commands functionality
</blockquote>

## Drivers
All the drivers that are supported in this library are placed in the drivers directory. 
```sh
| ├─── drivers  
| | ├─ BLDCDriver3PWM.cpp/h         # Implementation of generic 3PWM bldc driver
| | ├─ BLDCDriver6PWM.cpp/h         # Implementation of generic 6PWM bldc driver
| | ├─ StepperDriver2PWM.cpp/h      # Implementation of generic 2PWM stepper driver
| | ├─ StepperDriver4PWM.cpp/h      # Implementation of generic 4PWM stepper driver
| | |      
| | ├─ hardware_api.h               # common mcu specific api handling pwm setting and configuration
| | |
| | ├─── hardware_specific          # mcu specific hadrware_api.h implementations
| | | ├─ atmega2560_mcu.cpp         # ATMega 2560 implementation
| | | ├─ atmega328_mcu.cpp          # ATMega 328 (Arduino UNO) implementation
| | | ├─ esp32_mcu.cpp              # esp32 implementation
| | | ├─ stm32_mcu.cpp              # stm32 implementation
| | | ├─ teensy_mcu.cpp             # teensy implementation
| | | └─ generic_mcu./h             # generic implementation - if not nay of above (not complete)   
```
ALl BLDC drivers  implement the abstract class `BLDCDriver`. 
```cpp
class BLDCDriver{
    public:
        
        /** Initialise hardware */
        virtual int init();
        /** Enable hardware */
        virtual void enable();
        /** Disable hardware */
        virtual void disable();

        long pwm_frequency; //!< pwm frequency value in hertz
        float voltage_power_supply; //!< power supply voltage 
        float voltage_limit; //!< limiting voltage set to the motor
            
        /** 
         * Set phase voltages to the harware 
         * 
         * @param Ua - phase A voltage
         * @param Ub - phase B voltage
         * @param Uc - phase C voltage
        */
        virtual void setPwm(float Ua, float Ub, float Uc);
};
```
And all the stepper drivers implement the `StepperDriver` abstract class.
```cpp
class StepperDriver{
    public:
        
        /** Initialise hardware */
        virtual int init();
        /** Enable hardware */
        virtual void enable();
        /** Disable hardware */
        virtual void disable();

        long pwm_frequency; //!< pwm frequency value in hertz
        float voltage_power_supply; //!< power supply voltage 
        float voltage_limit; //!< limiting voltage set to the motor
            
        /** 
         * Set phase voltages to the harware 
         * 
         * @param Ua phase A voltage
         * @param Ub phase B voltage
        */
        virtual void setPwm(float Ua, float Ub);
};
```

Furthermore all the supported MCU architectures with the simplefoc library have to implement the header file `hardware_api.h`. The off-the-shelf supported architectures will have implementation of the `hardware_api.h` placed in the `hardware_specific` folder. If you wish to implement a new MCU please do create a new instance of the `my_new_mcu.cpp` and implement all the functions from the `hardware_api.h`, or at least the ones that you need.
## Sensors

```sh
| ├─── sensors 
| │ ├─ Encoder.cpp/h                # Encoder class implementing the Quadrature encoder operations
| │ ├─ MagneticSensorSPI.cpp/h      # class implementing SPI communication for Magnetic sensors
| │ ├─ MagneticSensorI2C.cpp/h      # class implementing I2C communication for Magnetic sensors
| │ ├─ MagneticSensorAnalog.cpp/h   # class implementing Analog output for Magnetic sensors
    └─ HallSensor.cpp/h             # class implementing Hall sensor
```
All position sensor classes implemented in this library are placed in this directory, and all of them will be implementing abstract sensor class `Sensor`. Every sensor needs to implement the `Sensor` class in order to be linkable to the motor (`BLDCMotor` and `StepperMotor` class). 
If you want to implement your own version of the sensor, jut extend this class and implement the virtual functions and you will be able to run the FOC algorithm with it.
You will be abele to link motor and the sensor by doing `motor.linkSensor(your sensor)`
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

## Common
```sh
│ ├─── common                  # Contains all the common utility classes and functions
| | |
| | ├─ defaults.h              # default motion control parameters
| | ├─ foc_utils.cpp./h        # utility functions of the FOC algorithm
| | ├─ time_utils.cpp/h        # utility functions for dealing with time measurements and delays
| | ├─ pid.cpp./h              # class implementing PID controller
| | ├─ lowpass_filter.cpp./h   # class implementing Low pass filter
| | |
| | ├─── base_classes
| | | ├─ FOCMotor.cpp./h        # common class for all implemented motors  
| | | ├─ BLDCDriver.h           # common class for all BLDC drivers  
| | | ├─ StepperDriver.h        # common class for all Stepper drivers
| | | └─ Sensor./h              # common class for all implemented sensors
| |
```
The common directory contains all the definitions and common utility functions for the  <span class="simple">Simple<span class="foc">FOC</span>library</span>.  It contains the definitions of the abstract classes for motors, sensors and drivers in the `base_classes` directory. It has two libraries of utility functions for time management `time_utils.cpp/h` and FOC helpers `foc_utils.cpp/h`. Finally it has definition and implementation of the two signal processing classes: pid controller `pid.cpp/h` and low pass filter `lowpass_filter.cpp/h`. It also contains the default configuration parameters of the library in the `defaults.h` header file.
Header file containing all the default configuration variables
```cpp
// default configuration values
// change this file to optimal values for your application

#define DEF_POWER_SUPPLY 12.0 //!< default power supply voltage
// velocity PI controller params
#define DEF_PID_VEL_P 0.5 //!< default PID controller P value
#define DEF_PID_VEL_I 10 //!<  default PID controller I value
#define DEF_PID_VEL_D 0 //!<  default PID controller D value
#define DEF_PID_VEL_U_RAMP 1000 //!< default PID controller voltage ramp value
// angle P params
#define DEF_P_ANGLE_P 20 //!< default P controller P value
#define DEF_VEL_LIM 20 //!< angle velocity limit default
// index search 
#define DEF_INDEX_SEARCH_TARGET_VELOCITY 1 //!< default index search velocity
// align voltage
#define DEF_VOLTAGE_SENSOR_ALIGN 6.0 //!< default voltage for sensor and motor zero alignemt
// low pass filter velocity
#define DEF_VEL_FILTER_Tf 0.005 //!< default velocity filter time constant
```



## Digging deeper

For more info about the FOC procedures and detailed explanations of the code implementing FOC algorithm please visit: <a href="foc_implementation"> FOC implementation details <i class="fa fa-external-link fa-sm"></i></a>

For documentation of the motion control algorithms and code implementation choices, visit <a href="motion_control_implementation"> Motion control implementation details <i class="fa fa-external-link fa-sm"></i></a>

The library comes with a lot of motor control examples for different microcontrollers. See more on [library examples <i class="fa fa-external-link"></i>](library_examples)

To dig deeper in the source code please visit <a href="http://source.simplefoc.com/" target="_blank"> Doxygen generated code documentation <i class="fa fa-external-link fa-sm"></i></a>

<div class="image_icon width80" >
    <a href="http://source.simplefoc.com/" target="_blank">
        <img src="extras/Images/source_docs.jpg" >
        <i class="fa fa-external-link-square fa-2x"></i>
    </a>
</div>

