//###########################################
//                  SETUP
//###########################################

void setup() {
  Serial.begin(115200);

  settingsInit();

  #ifdef OTA_UPDATE
    if (updateModeRequested()) runUpdateMode();
  #endif

  registerSystemCommands();

  #ifdef PROFILE_FOC
    profileBusyReport("busy work before PWM");
  #endif

  state_machine = LIFE_IS_GOOD;

  Serial.println("DAGOR: INIT");
  Serial.print("DAGOR: FW Version -> ");
  Serial.println(FW_VERSION);
  Serial.print("DAGOR: ACTUATOR ID -> ");
  Serial.println(actId);

  #ifdef ESP_NOW
    Serial.println("DAGOR: ESP-NOW init");
    espNowInit();
    _delay(500);
  #endif
 
  gpio_init();
  spi_init();

  #if defined(MONITOR_TEMP) || defined(MONITOR_BUS_VOLTAGE)
    adcMonitorInit();
  #endif

  _delay(500);

  if (state_machine == LIFE_IS_GOOD){
    Serial.println("DAGOR: Init SimpleFOC");

    #ifdef MONITOR_BUS_VOLTAGE
      float bus_v = busVoltage();
      int sfoc = SimpleFOCinit(bus_v);
    #else
      int sfoc = SimpleFOCinit(voltageOverride);
    #endif

    if (sfoc){
      Serial.println("DAGOR: Ready PMSM.");
      state_machine = LIFE_IS_GOOD;
    } else { 
      Serial.println("DAGOR: Could not initialize SimpleFOC.");
      raiseFault(F_SIMPLEFOC_INIT);
      state_machine = SIMPLEFOC_ERROR;
      drv_fault_status();
      drv_enable(false);
    }

  }

  checkImageAtBoot();

  taskDRVfault();

  #ifdef MONITOR_TEMP
    taskTempSensor();
  #endif

  #ifdef MONITOR_BUS_VOLTAGE
    taskBusVoltage();
  #endif

  #ifdef MONITOR_ROTOR
    Serial.println("DAGOR: Rotor Monitor Task Enabled.");
    taskRotorMonitor();
  #endif

  #ifdef RS485
    Serial.println("DAGOR: RS485 Task Enabled.");
    taskRS485();
  #endif

  if (print_currents) taskPrintCurrents();
  if (print_voltages) taskPrintVoltages();

  #ifdef PROFILE_FOC
    if (state_machine == LIFE_IS_GOOD) profileFOC();
  #endif

}

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

