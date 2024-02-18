//###########################################
//                  SETUP
//###########################################

float offset_ia, offset_ib, offset_ic;

void setup() {
  Serial.begin(115200);
  
  Serial.println("DAGOR: INIT");
  Serial.print("DAGOR: ACTUATOR ID -> ");
  Serial.println(ACT_ID);
  
  float bus_v = busVoltage();

  #ifdef ESP_NOW
    Serial.println("DAGOR: ESP-NOW init");
    espNowInit();
    _delay(500);
  #endif
 
  #ifdef RS485
    Serial1.begin(921600, SERIAL_8N1, UART_RX, UART_TX);
    myTransfer.begin(Serial1);
    pinMode(COMMS_DIR, OUTPUT);
  #endif

  gpio_init();
  spi_init();

  delay(200);
  drv_init();

  int sfoc = SimpleFOCinit(12);
  //int sfoc = SimpleFOCinit(bus_v);

  if (sfoc){
    Serial.println("DAGOR: Ready BLDC.");
    state_machine = LIFE_IS_GOOD;

    #ifdef CURRENT_SENSE
      calibratePhaseZeroOffset();
      drv_init();
    #endif

  }
  else { 
    Serial.println("DAGOR: Could not initialize.");
    //state_machine = LIFE_IS_GOOD;
    state_machine = SIMPLEFOC_ERROR;
    drv_enable(false);
    faultStatus();
  }
}
