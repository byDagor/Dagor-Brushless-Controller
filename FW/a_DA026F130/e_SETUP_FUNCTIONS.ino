//###########################################
//      SETUP FUNCTIONS DEFINITIONS
//###########################################





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
