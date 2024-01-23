//###########################################
//                  SETUP
//###########################################

void setup() {
  Serial.begin(115200);
  
  Serial.println("DAGOR: INIT");
  Serial.print("DAGOR: ACTUATOR ID -> ");
  Serial.println(ACT_ID);
  
  #ifdef ESP_NOW
    Serial.println("DAGOR: ESP-NOW init");
    espNowInit();
    _delay(500);
  #endif
 
  gpio_init();
  spi_init();

  delay(250);
  drv_init();

  int sfoc = SimpleFOCinit();

  if (sfoc){
    state_machine = LIFE_IS_GOOD;
  }
  else { 
    state_machine = SIMPLEFOC_ERROR;
    faultStatus();
  }
  
}
