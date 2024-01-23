
//###########################################
//                SETUP
//###########################################

void setup() {
  Serial.begin(115200);
  
  Serial.println("DAGOR: INIT");
  Serial.print("DAGOR: LEG ID -> ");
  Serial.println(LEG_ID);
  #ifdef KNEE
    Serial.println("DAGOR: ACTUATOR -> KNEE ");
  #endif 
   
  #ifdef HIP
    Serial.println("DAGOR: ACTUATOR -> HIP ");
  #endif
 
  #ifdef SHOULDER
    Serial.println("DAGOR: ACTUATOR -> SHOULDER ");
  #endif
  
  #ifdef ESP_NOW
    Serial.println("DAGOR: ESP-NOW init");
    espNowInit();
    _delay(500);
  #endif
 
  gpio_init();
  spi_init();

  delay(250);
  drv_init();

  SimpleFOCinit();
  
}
