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
