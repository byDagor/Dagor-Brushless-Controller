
//###########################################
// SETUP
//###########################################

void setup() {
  Serial.begin(115200);

  Serial.println("DAGOR: INIT");

  gpio_init();
  spi_init();

  delay(250);
  drv_init();

  SimpleFOCinit();
  
}
