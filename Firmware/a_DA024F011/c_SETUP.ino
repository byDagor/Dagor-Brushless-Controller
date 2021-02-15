
//###########################################
// SETUP
//###########################################

void setup() {
  Serial.begin(115200);

  analogSetCycles(2);

  // Pinmode assignment
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
  pinMode(so1, INPUT);
  pinMode(so2, INPUT);
  pinMode(so3, INPUT);
  pinMode(nFault, INPUT);
  pinMode(enGate, OUTPUT);
  digitalWrite(enGate, LOW);

  // SPI start up
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);

  //Three phase driver init
  drv_init();

  //Field Oriented Control init
  SimpleFOCinit();

  Serial.println("DAGOR: Ready BLDC.");
}
