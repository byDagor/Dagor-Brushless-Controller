---
layout: default
title: Global - tab b
parent: Firmware
nav_order: 2
permalink: /global
---

# Global

This tab contains the libraries, functions, pins, variables and instances needed for the normal operation of the basic firmware of the **Dagor Controller**. It is not recommended to change anything in this tab, but it is encouraged to add code if the user is doing something specific such as using the free-to-use GPIOs of the controller, setting up a communication protocol, or using a library.

Needed libraries. The firmware is powered by SimpeFOC; make sure to use the correct SimpleFOC version. IF another library is needed it must be included here.

```c++
//SimpleFOC Version 2.0.1
#include <SimpleFOC.h>
#include <SPI.h>
```

Three phase driver (power stage)[/power_stage] pin definition.

```c++
//#######_THREE PHASE DRIVER - DRV8305_########
// Datasheet: www.ti.com/lit/ds/symlink/drv8305.pdf
#define enGate 2       //Chip Enable
#define nFault 4       //Fault reading
#define cs 5           //DRV8305 Chip-select
#define so1 36
#define so2 35
#define so3 34
bool faultTrig = false;


On-board magnetic sensor SPI chip select. The SimpleFOC library manages the SPI communication with the sensor.

```c++
//######_MAGNETIC SENSOR - AS5147_######
// Datasheet: https://ams.com/documents/20143/36005/AS5147_DS000307_2-00.pdf
#define sensorCS 16             //AS5147 Chip-select
```

The temperature sensor has an analog signal 0 - 3.3V that is read with this ESP32's pin.

```c++
//######_TEMPERATURE SENSOR - STLM20_######
// Datasheet: https://datasheet.lcsc.com/szlcsc/1810010411_STMicroelectronics-STLM20W87F_C129796.pdf
#define vTemp 39
```

Analog read pin for the power source voltage monitor.

```c++
//######_Voltage Monitor_#######
#define vMonitor 33
```

Time management variables. Add here a variable named something like "stateT2" to add another fixed rate function caller, if needed.

```c++
//#####_TIME MANAGEMENT_#####
float runTime, prevT = 0, timeDif, stateT;
int timeInterval = 1000, totalTempTime;
```

SimpleFOC motor, driver and sensor instances. Even though the on-board sensor has SPI and ABI connected, the SPI communication is preferred to have absolute position of the motor's rotor.

```c++
//####_SIMPLEFOC INSTANCES_####
BLDCMotor motor = BLDCMotor(pp);   //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27);     //3PWM Driver instance
MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, sensorCS);       //SPI Magnetic sensor instance
```

Declaration of functions that are used on the void setup, if a new function is added it has to be declared here, called on the void setup function (tab c) and added in the Setup functions tab (tab e).

```c++
//######_SETUP FUNCTIONS INIT_######
void SimpleFOCinit();
static void IRAM_ATTR isr_handler(void*);
void drv_init();
```

Declaration of functions that are used on the void loop, if a new function is added it has to be declared here, called on the void loop (tab D) in either the main loop, the fixed rate caller or a new fixed rate caller, and added in the Loop functions tab (tab f).

```c++
//######_LOOP FUNCTIONS INIT_######
void timeManagement();
void tempStatus(bool debug = false);
void voltageMonitor(bool debug = false);
void rotorPosition();
void faultStatus();
String serialReceiveUserCommand();
```
