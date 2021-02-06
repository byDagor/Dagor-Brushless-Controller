---
layout: default
title: Sensors
parent: Hardware
nav_order: 4
permalink: /sensors
---

# Magnetic sensor

The **Dagor Controller** has a high resolution absolute magnetic sensor (U7) for its precise control. The sensor makes use of a diametrically polarized magnet (included with a controller purchase) attached to the motor's rotor, as shown in the picture. To learn more about this sensor refer to the [AS5147 datasheet](https://ams.com/documents/20143/36005/AS5147_DS000307_2-00.pdf).

![AS5147](Images/Magnet.PNG)

<img src="Images/as5147.png" width=400>

# Temperature sensor

The Maximum continuous current of the **Dagor Controller** depends on the quality of heat-sinking and cooling. A small heat-sink or a cooling fan can have great impacts on the maximum continuous current the board can handle. The on-board temperature sensor (U5) is a great way to assure the user stays within the safe range of operating temperature. The temperature sensor is found near the MOSFETs to obtain a more accurate measurement of the temperature of the controller's power stage. To learn more about this sensor please refer to the [STLM20W87F datasheet](https://datasheet.lcsc.com/szlcsc/1810010411_STMicroelectronics-STLM20W87F_C129796.pdf).

![temp_sensor_dagor](Images/temp_sensor_dagor.png)

## Temperature vs. current curve

This section studies the steady state temperature versus constant current draw with the controller without any type of cooling or heat-sinking.

>Graph coming soon, if you wish to help with this measurements shoot me an email :)

# Voltage monitor

The Dagor controller has a resistor-based voltage divider that can be used to monitor the power source/ battery voltage.

![volt_monitor](Images/volt_monitor.png)
