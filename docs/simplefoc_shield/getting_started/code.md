---
layout: default
title: Writing the code
parent: Getting Started
description: "Writing the Arduino program for your SimpleFOCShield."
nav_order: 3
permalink: /foc_shield_code
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
---

# Writing the code
Once when you have decided the appropriate [hardware configuration](pads_soldering) of yous board and once you have all the hardware [ready to be connected](foc_shield_connect_hardware): 
- Microcontroller
- BLDC motor
- Position sensor
- Power supply

we can start the most exciting part, coding!

Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is fully supported by Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>, therefore please make sure you have the newest version of the  <span class="simple">Simple<span class="foc">FOC</span>library</span> installed. If you still did not get your owm version of the library please follow the [installation instructions](installation). 

Suggested approach when starting coding for the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> is:

- [Test the sensor](#step-1-testing-the-sensor)
- [Test the motor](#step-2-testing-the-motor)
- [Voltage motion control](#step-3-voltage-motion-control)
- [More complex control strategies](#step-4-more-complex-control-strategies) - position and velocity

## Step 1. Testing the sensor
First make sure your sensor works properly. Run one of the library examples specific to your sensors. You can find the library examples in 
```sh
utils >
    sensor_test >
            encoder >
             - encoder_example
             - encoder_software_interrupts_example
            magnetic_sensors >
             - magnetic_sensor_i2c_example
             - magnetic_sensor_spi_example
             - magnetic_sensor_analog_example
            hall_sensors >
             - hall_sensor_example
             - hall_sensor_software_interrupts_example
```
Once you have your sensor reading the good values you can continue the testing the combination of the motor and the sensor.

<blockquote class="warning"> <p class="heading">Update the example pinout</p> 
When testing the sensor make sure to update the pinout that you have chosen in the <a href="pads_soldering">hardware configuration</a>.</blockquote>

## Step 2. Testing the motor
To test the BLDC motor before running FOC algorithm we suggest to run the open loop examples!
```sh
motion_control >
    open_loop_motor_control >
             - open_loop_position_example
             - open_loop_velocity_example
```
<blockquote class="warning"> <p class="heading">Update the example pinout</p> 
When testing the motor make sure to update the pinout that you have chosen in the <a href="pads_soldering">hardware configuration</a>.</blockquote>

If you are not sure what is the number of pole pairs your motor has, please check the example code:
```sh
utils >
    find_pole_pair_number >
             - encoder
             - magnetic_sensor
```
This code will estimate the pole pairs number that your motor has. Please run this code several times to get a good estimation. The code will, generally show a good reading 7/10 times.


## Step 3. Voltage motion control 
Once you have your sensor working, and you have the right number of pole pairs of your motor you can start using FOC algorithm. The best practice is to start with an example of the Voltage control:
```sh
motion_control > 
        torque_voltage_control > 
                       - encoder
                       - magnetic_sensor
                       - hall_sensors
```

## Step 4. More complex control strategies
Now when you have your torque control using voltage ready you can continue to the position and velocity control algorithms. They will take a bit more time to tune but they will give you a chance to have great results. You can find the library examples for motion control loops in by navigating the examples:

```sh
motion_control > 
        position_motion_control > 
                       - encoder
                       - magnetic_sensor
                       - hall_sensors
        torque_voltage_control > 
                       - encoder
                       - magnetic_sensor
                       - hall_sensors
        velocity_motion_control > 
                       - encoder
                       - magnetic_sensor
                       - hall_sensors
```
For more information about the possible <span class="simple">Simple<span class="foc">FOC</span>Shield</span> projects visit [example projects](examples)
