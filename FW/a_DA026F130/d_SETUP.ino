//###########################################
//                  SETUP
//###########################################

void setup() {
  Serial.begin(115200);

  state_machine = LIFE_IS_GOOD;

  Serial.println("DAGOR: INIT");
  Serial.print("DAGOR: ACTUATOR ID -> ");
  Serial.println(ACT_ID);

  #ifdef ESP_NOW
    Serial.println("DAGOR: ESP-NOW init");
    espNowInit();
    _delay(500);
  #endif
 
  #ifdef RS485
    Serial1.begin(921600, SERIAL_8N1, UART_RX, UART_TX);
    myTransfer.begin(Serial1);
    pinMode(COMMS_DIR, OUTPUT);
    _delay(100);
  #endif

  gpio_init();
  spi_init();

  delay(200);
  state_machine = (Dagor_state)drv_init(); 
  delay(100);

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

      //#ifdef CURRENT_SENSE
      //  calibratePhaseZeroOffset();
      //  drv_init();
      //#endif
    } else { 
      Serial.println("DAGOR: Could not initialize SimpleFOC.");
      state_machine = SIMPLEFOC_ERROR;
      drv_enable(false);
      drv_fault_status();
    }

  } else{
    Serial.println("Dagor: ERROR - DRV could not init.");
  }

  taskDRVfault();

  #ifdef MONITOR_TEMP
    taskTempSensor();
  #endif

  #ifdef MONITOR_BUS_VOLTAGE
    taskBusVoltage();
  #endif

  if (print_currents) taskPrintCurrents();
  if (print_voltages) taskPrintVoltages();
  if (print_rotor_data) taskShaftData();

}
