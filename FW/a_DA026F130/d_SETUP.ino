//###########################################
//                  SETUP
//###########################################

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

  delay(250);
  drv_init();

  int sfoc = SimpleFOCinit(bus_v);

  if (sfoc){
    state_machine = LIFE_IS_GOOD;
    Serial.println("DAGOR: Ready BLDC.");  
  }
  else { 
    Serial.println("DAGOR: Could not initialize.");
    state_machine = SIMPLEFOC_ERROR;
    faultStatus();
  }
  
}
