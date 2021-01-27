---
layout: default
title: Library Examples
parent: Library Source
nav_order: 5
permalink: /library_examples
grand_parent: Digging deeper
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---


# Library Examples

This library comes with `32` documented examples showing the basic usage of the library with:
- **different microcontroller architectures**: 
    - Arduino UNO, Nucleo: everything except `hardware_specific_examples`
    - Bluepill: `bluepill_position_control.ino`
    - HMBGC gimbal controller: `position_control.ino`, `voltage_control.ino`
    - ESP32 controller: `position_control.ino`, `voltage_control.ino`
- **different position sensors**: 
  - encoders, 
  - magnetic sensors (SPI, I2C, Analog), 
  - hall sensors 
- **different BLDC drivers**: 
  - <span class="simple">Simple<span class="foc">FOC</span>Shield</span> 
  - HMBGC gimbal controller: `HMBGC_example`
  - DRV8302: `DRV8305_driver`
- **different motion control**: 
  - torque/voltage control: `torque_voltage_control.ino`
  - velocity control:  `velocity_motion_control.ino`
  - position/angle control: `position_motion_control.ino`
  - velocity open-loop:  `open_loop_velocity_example.ino`
  - position/angle open-loop: `open_loop_position_example.ino`
- **a lot of utility functions**:
  - finding pole pair number: `find_pole_pair_number.ino`
  - finding zero offset and sensor orientation: `find_sensor_offset_and_direction.ino`
  - sensor alignment and motor cogging test: `alignment_and_cogging_test.ino`

## Examples folder structure
```shell
> examples
├───hardware_specific_examples
│   ├───Bluepill_examples                         # example of STM32 Bluepill code
│   │   ├───encoder
│   │   └───magnetic_sensor
│   ├───DRV8305_driver                            # example of code with DRV8302 config
│   │   └───motor_full_control_serial_examples
│   ├───ESP32                                     # example of ESP32 controller code
│   │   ├───encoder 
│   │   └───magnetic_sensor
│   └───HMBGC_example                             # example of HMBGC controller code
│       ├───position_control
│       └───voltage_control
├───motion_control
│   ├───open_loop_motor_control                   # example of open-loop motor control
│   │   ├───open_loop_position_example
│   │   └───open_loop_velocity_example
│   ├─── position_motion_control                  # example of angle/position motion control loop with configuration
│   ├─── torque_voltage_control                   # example of the voltage/torque control loop with configuration
│   └─── velocity_motion_control                  # example of velocity motion control loop with configuration
|
├───motor_commands_serial_examples
│   ├───encoder
│   ├───hall_sensor
│   └───magnetic_sensor
└───utils
    ├───alignment_and_cogging_test                # example estimating alignment and cogging performance
    ├───find_pole_pair_number                     # example estimating pole pair number of the motor
    ├───find_sensor_offset_and_direction          # example determining sensor zero_electrical_offset and natural direction
    └───sensor_test                               # examples for sensor testing
        ├───encoder
        │   ├───encoder_example
        │   └───encoder_software_interrupts_example
        ├───hall_sensors
        │   ├───hall_sensor_example
        │   └───hall_sensor_software_interrupts_example
        └───magnetic_sensors
            ├───magnetic_sensor_analog_example
            ├───magnetic_sensor_i2c_example
            └───magnetic_sensor_spi_example
```
