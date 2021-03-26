---
layout: default
title: Setup functions - tab e
parent: Firmware
nav_order: 5
permalink: /setup_func
---

# Setup functions

Initialization of [SimpleFOC](https://simplefoc.com). Visit the ["Writing the code"](https://docs.simplefoc.com/code) section to learn how to configure this function.

```c++
// Do NOT remove the delays in this function.
void SimpleFOCinit(){
  //Motor driver initialization

  _delay(500);

  sensor.init();                  // Initialise magnetic sensor hardware
  motor.linkSensor(&sensor);      // Link sensor to motor instance  


  // driver config, power supply voltage [V]
  driver.voltage_power_supply = sourceVoltage;
  driver.voltage_limit = sourceVoltage;
  driver.init();
  motor.linkDriver(&driver);

  if (focModulation) motor.foc_modulation = FOCModulationType::SinePWM;
  else motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // set FOC loop to be used: MotionControlType::torque, velocity, angle, velocity_openloop, angle_openloop
  if (controlType == "C0") motor.controller = MotionControlType::torque;
  else if (controlType == "C1") motor.controller = MotionControlType::velocity;
  else motor.controller = MotionControlType::angle;

  // velocity PI controller parameters
  motor.PID_velocity.P = vp;
  motor.PID_velocity.I = vi;
  motor.PID_velocity.D = vd;
  motor.PID_velocity.output_ramp = voltageRamp;
  motor.LPF_velocity.Tf = lpVelFilter;

  // angle P controller
  motor.P_angle.P = ap;
  motor.P_angle.I = ai;
  motor.P_angle.D = ad;
  // maximal velocity of the poisition control
  motor.velocity_limit = velocityLimit;
  motor.LPF_angle.Tf = lpPosFilter;


  // Position offset, used to define an absolute 0 position [rads]
  motor.sensor_offset = sensorOffset;

  // Downsampling value of the motion control loops with respect to torque loops
  motor.motion_downsample = motionDownSample; // - times (default 0 - disabled)

  //motor.voltage_limit = phaseRes*maxCurrent;
  // Measured phase resistance [ohms]
  motor.phase_resistance = phaseRes;
  motor.current_limit = maxCurrent;

  // Sensor aligning voltage
  motor.voltage_sensor_align = (maxCurrent*phaseRes)*alignStrength;

  motor.useMonitoring(Serial);      // use monitoring functionality
  motor.init();                     // initialise motor

  if (skipCalibration && natDirection == "CW") motor.initFOC(elecOffset,CW);              // start FOC
  else if (skipCalibration && natDirection == "CCW") motor.initFOC(elecOffset,CCW);       // start FOC
  else motor.initFOC();                         // align sensor/ encoder and start FOC

  _delay(500);

  // define the motor id
  command.add(motorID, onMotor, " BLDC");
  if (!commandDebug) command.verbose = VerboseMode::nothing;

}

```

Configuration of the DRV8305 driver IC. The first thing to do before applying the FOC algorithm is to configure the driver to work with 3 PWM signals
```c++
//Configure DRV8305 to desired operation mode
void drv_init(){
  _delay(250);
  Serial.println("DRIVER: DRV8305 INIT");

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10000110);
  digitalWrite(cs, HIGH);

  _delay(500);

  Serial.println("DRIVER: enGate Enabled");
  digitalWrite(enGate, HIGH);
}

```

Initialise SPI and GPIOs

```c++
void spi_init(){
  //SPI start up
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
}

void gpio_init(){
  //Pinmodes assignment
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
  pinMode(so1, INPUT);
  pinMode(so2, INPUT);
  pinMode(so3, INPUT);
  pinMode(nFault, INPUT);
  pinMode(enGate, OUTPUT);
  digitalWrite(enGate, LOW);
}
```
