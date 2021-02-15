
//###########################################
// SETUP FUNCTIONS
//###########################################


// Initialization of SimpleFOC
// Do NOT remove the delays in this function.
void SimpleFOCinit(){
  delay(500);

  if (sensorType == "ABI"){
    encoder.init();
    // hardware interrupt enable
    encoder.enableInterrupts(doA, doB);
    motor.linkSensor(&encoder);
  }
  // Initialise magnetic sensor hardware
  else{
    sensor.init();
    motor.linkSensor(&sensor);
  }
  
  
  // Driver configuration
  driver.voltage_power_supply = sourceVoltage;    //[Volts]
  driver.init();
  motor.linkDriver(&driver);

  // set FOC loop to be used: MotionControlType::torque, velocity, angle
  if (controlType == "C0") motor.controller = MotionControlType::torque;
  else if (controlType == "C1") motor.controller = MotionControlType::velocity;
  else motor.controller = MotionControlType::angle;
  
  // Voltage applied to sensor align routine
  motor.voltage_sensor_align = voltageLimit/2;
  
  // Velocity PI gains
  motor.PID_velocity.P = ki;
  motor.PID_velocity.I = ti;
  motor.PID_velocity.D = 0;
  motor.voltage_limit = voltageLimit;
  motor.LPF_velocity.Tf = lpFilter;
  
  // Position P gain
  motor.P_angle.P = kp;
  motor.velocity_limit = velocityLimit;

  motor.useMonitoring(Serial);          // use monitoring functionality
  motor.init();                         // initialise motor
  motor.initFOC();                      // align sensor and start FOC
  
  // Once the electric angle and rotation direction is known, the sensor align routine can be skipped
  // by writing the electric angle and the direction to the initFOC function like shown bellow.
  // motor.initFOC(4.5,CW);
  
  //Serial.print("Electric Angle: ");
  //Serial.println(motor.zero_electric_angle);
  
  _delay(1000);
}

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
