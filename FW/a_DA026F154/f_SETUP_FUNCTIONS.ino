//###########################################
//       SETUP FUNCTIONS DEFINITIONS
//###########################################


adc_cali_handle_t busAdcCalibration = nullptr;

// Temperature and bus voltage use SimpleFOC's ADC read, analogRead() does not share its lock with the current sense interrupt.
// Must run before the current sense interrupt starts.
void adcMonitorInit(){
  adcInit(vTemp);
  adcInit(vMonitor);

  adc_cali_line_fitting_config_t calibration = {};
  calibration.unit_id = ADC_UNIT_1;
  calibration.atten = ADC_ATTEN_DB_12;
  calibration.bitwidth = ADC_BITWIDTH_12;
  if (adc_cali_create_scheme_line_fitting(&calibration, &busAdcCalibration) != ESP_OK){
    busAdcCalibration = nullptr;
    Serial.println("Dagor: ADC factory calibration not available, bus voltage is less accurate.");
  }
}

// Single bus voltage reading [V]
float readBusVoltage(){
  int raw = adcRead(vMonitor);
  int millivolts = raw * ADC_FULL_SCALE_MV / ADC_MAX_COUNT;
  if (busAdcCalibration) adc_cali_raw_to_voltage(busAdcCalibration, raw, &millivolts);
  return millivolts / 1000.0f * BUS_DIVIDER_RATIO;
}

float busVoltage(){

  float bus_v = 0;

  for(int i = 0; i < BUS_VOLTAGE_SAMPLES; i++){
    bus_v += readBusVoltage();
  }

  bus_v = bus_v/BUS_VOLTAGE_SAMPLES;

  Serial.print("Dagor: Bus Voltage ->");
  Serial.println(bus_v);
  
  return bus_v;

}

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
  pinMode(SO1, INPUT);
  pinMode(SO2, INPUT);
  pinMode(SO3, INPUT);
  pinMode(nFault, INPUT);
  pinMode(enGate, OUTPUT);
  digitalWrite(enGate, LOW);
}

void taskTempSensor(){
  xTaskCreatePinnedToCore(
    tempStatus, /* Function to implement the task */
    "Temperature Sensor Monitor", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_temp,
    5,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskBusVoltage(){
  xTaskCreatePinnedToCore(
    busVoltageMonitor, /* Function to implement the task */
    "Bus Voltage Monitor", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_bus_voltage,
    3,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskPrintCurrents(){
  xTaskCreatePinnedToCore(
    printCurrents, /* Function to implement the task */
    "Monitor Currents", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_dq_currents,  /* Task input parameter */
    0,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskPrintVoltages(){
  xTaskCreatePinnedToCore(
    printVoltages, /* Function to implement the task */
    "Monitor Voltages", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_dq_voltages,  /* Task input parameter */
    0,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}

void taskRotorMonitor(){
  xTaskCreatePinnedToCore(
    rotorMonitor, /* Function to implement the task */
    "Rotor Position and Veloity", /* Name of the task */
    2048,  /* Stack size in words */
    (void*)&print_rotor_data,
    2,  /* Priority of the task */
    NULL,  /* Task handle. */
    1); /* Core where the task should run */
}

void taskDRVfault(){
  xTaskCreatePinnedToCore(
    drv_task, /* Function to implement the task */
    "DRV Fault Status", /* Name of the task */
    2048,  /* Stack size in words */
    NULL,
    4,  /* Priority of the task */
    NULL,  /* Task handle. */
    0); /* Core where the task should run */
}