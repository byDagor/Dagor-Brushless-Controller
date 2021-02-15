---
layout: default
title: Setup functions - tab e
parent: Firmware
nav_order: 5
permalink: /setup_func
---

# Setup functions

Initialization of (SimpleFOC)[https://simplefoc.com]. Visit the ("Writing the code")[https://docs.simplefoc.com/code] section to learn how to configure this function.

```c++
// Do NOT remove the delays in this function.
void SimpleFOCinit(){
  delay(500);

  //Initialise magnetic sensor hardware
  sensor.init();

  //Link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config, power supply voltage [V]
  driver.voltage_power_supply = sourceVoltage;
  driver.init();
  motor.linkDriver(&driver);

  // set FOC loop to be used: ControlType::voltage, velocity, angle
  motor.controller = ControlType::angle;

  // Sensor aligning voltage
  motor.voltage_sensor_align = voltageLimit/2;

  // velocity PI controller parameters, default K=0.5 Ti = 0.01
  motor.PID_velocity.P = ki;
  motor.PID_velocity.I = ti;
  motor.PID_velocity.D = 0;
  motor.voltage_limit = voltageLimit;
  motor.LPF_velocity.Tf = lpFilter;

  // angle P controller, default K=70
  motor.P_angle.P = kp;
  // maximal velocity of the poisition control, default 20
  motor.velocity_limit = velocityLimit;

  motor.useMonitoring(Serial);      // use monitoring functionality
  motor.init();                     // initialise motor
  motor.initFOC();                  // align sensor/ encoder and start FOC
  Serial.print("Electric Angle: ");
  Serial.println(motor.zero_electric_angle);
  //motor.initFOC(4.5,CW);

  _delay(1000);
}

```

Configuration of the DRV8305 driver IC. The first thing to do before applying the FOC algorithm is to configure the driver to work with 3 PWM signals
```c++
//Configure DRV8305 to desired operation mode
void drv_init(){
  delay(250);
  Serial.println("DRIVER: DRV8305 INIT");

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10000110);
  digitalWrite(cs, HIGH);
  //010 10010110
  //010 00000110
  Serial.println(resp1, BIN);
  Serial.println(resp2, BIN);

  delay(500);
  Serial.println("DRIVER: enGate Enabled");
  digitalWrite(enGate, HIGH);
}

```
