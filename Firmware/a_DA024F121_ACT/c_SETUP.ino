
//###########################################
// SETUP
//###########################################

void setup() {
  Serial.begin(115200);
  
  Serial.println("DAGOR: INIT");
  
  Serial.println("DAGOR: ESP-NOW init");
  espNowInit();
  _delay(500);
 
  gpio_init();
  spi_init();

  delay(250);
  drv_init();

  SimpleFOCinit();
  
}
