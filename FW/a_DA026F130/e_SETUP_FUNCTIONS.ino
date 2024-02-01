//###########################################
//      SETUP FUNCTIONS DEFINITIONS
//###########################################



float busVoltage(){

  float bus_v;

  for(int i = 0; i<=50; i++){
    //Serial.println(analogRead(vMonitor));
    bus_v += analogRead(vMonitor) * 28.7/4095; //36.3
  }

  bus_v = bus_v/50;

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
