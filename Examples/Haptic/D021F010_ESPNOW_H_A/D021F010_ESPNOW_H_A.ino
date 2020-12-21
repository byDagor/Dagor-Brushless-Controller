//Dagor 2.1 Brushless Controller Firmware 010 with ESP-NOW communication for steer by wire(less).

//SimpleFOC Version 1.6
#include "SimpleFOC.h"
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>

//######_ESPNOW_######
typedef struct struct_message {
    float value;
} struct_message;

struct_message inputPosition;
struct_message outputPosition;
esp_now_peer_info_t peerInfo;

uint8_t broadcastAddress[] = {0xAC, 0x67, 0xB2, 0x59, 0xAB, 0xB8};

float thisMotor = 0, thatMotor = 0, setPoint = 0;
 
//#######_DRV8305_########.
//Datasheet: www.ti.com/lit/ds/symlink/drv8305.pdf
#define enGate 2       //Chip Enable
#define nFault 4       //Fault reading
#define so1 36
#define so2 35
#define so3 34
int a1, a2, a3;         //Current readings from internal current sensor amplifiers
bool faultTrig = false;

//#####_SPI_#######
#define cs 5      //Chip-select

//######_AS5147_######
//A and B encoder inputs
//#define arduinoInt1 32             // interrupt 0
//#define arduinoInt2 33             // interrupt 1

//######_TEMPERATURE_######
#define vTemp 39

//#####_TIME MANAGEMENT_#####
float runTime, prevT = 0, timeDif, stateT, stateNow;
int timeInterval = 1000;

//####_BLDC_MOTOR_####
//  BLDCMotor instance
BLDCMotor motor = BLDCMotor(25, 26, 27, 7);

//####_ENCODER_####
// SPI Magnetic sensor instance (AS5147P example)
// MISO 12, MOSI 9, SCK 14
MagneticSensorSPI sensor = MagneticSensorSPI(16, 14, 0x3FFF);

// encoder instance
//Encoder sensor = Encoder(4, 2, 1024);

// Interrupt routine intialisation
// channel A and B callbacks
//void doA(){sensor.handleA();}
//void doB(){sensor.handleB();}

//#####_System Gains_#####
float ki = 0.0015; //0.075;
float ti = 2.0; //0.0075;
float lpFilter = 0.000;
float kp = 10; //15;
float voltageRamp = 20;
//float voltageLimit = 0.8;
float voltageLimit = 1;
float velocityLimit = 2000;

void setup() {
  Serial.begin(115200);

 //Initialise magnetic sensor hardware
  sensor.init();

  //If using encoder
  //sensor.enableInterrupts(doA, doB); 
  
  //Link the motor to the sensor
  motor.linkSensor(&sensor);

  //Pinmodes
  pinMode(15,OUTPUT);
  digitalWrite(15,HIGH);
  pinMode(so1, INPUT);
  pinMode(so2, INPUT);
  pinMode(so3, INPUT);
  pinMode(nFault, INPUT);
  pinMode(enGate, OUTPUT);
  digitalWrite(enGate, LOW);

  //SPI start up
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);

  //Motor driver initialization
  delay(250);
  Serial.println("DRV8305 INIT");
  drv_init();
  delay(500);

  Serial.println("enGate Enabled");
  digitalWrite(enGate, HIGH);
  delay(500);

  //BLDC motor initialization
  motor.voltage_power_supply = 12;
  // aligning voltage
  motor.voltage_sensor_align = 0.5;

  // set FOC loop to be used: ControlType::voltage, velocity, angle
  motor.controller = ControlType::voltage;
  
  // velocity PI controller parameters, default K=0.5 Ti = 0.01
  motor.PID_velocity.P = ki;
  motor.PID_velocity.I = ti;
  motor.voltage_limit = voltageLimit;
  motor.LPF_velocity.Tf = lpFilter;
  
  // angle P controller, default K=70
  motor.P_angle.P = kp;
  // maximal velocity of the poisition control, default 20
  motor.velocity_limit = velocityLimit;

  // use monitoring functionality
  motor.useMonitoring(Serial);
  // initialise motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC(3.99,CW);
  Serial.println("Ready BLDC.");
  
  _delay(1000);
  Serial.println(motor.zero_electric_angle);
  espNowInit();
}


void loop() {
  motor.loopFOC();

  setPoint = 1*(thatMotor - thisMotor);
  if (setPoint >= voltageLimit) setPoint = voltageLimit;
  else if(setPoint <= -voltageLimit) setPoint = -voltageLimit;
  motor.move(setPoint); // target set by the motor.command()  - or by variable motor.target

  thisMotor = sensor.getAngle();
  //Serial.println(e,4);
  
  //motor.command(serialReceiveUserCommand());
  // user communication
  serialReceiveUserCommand();

  //Time Managment
  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;
  stateNow += timeDif;
  
  //Send rotor angle at 300Hz
  if(stateNow >= 3333){
    sendData(thisMotor);
    stateNow = 0;
    //Serial.println(setPoint);
  }
  

  //Call functions inside if at 1 Hz
  if(stateT >= 1000000){
    tempStatus();
    faultStatus();
    stateT = 0;
  }
  
}

void sendData(float val){
  outputPosition.value = val;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outputPosition, sizeof(outputPosition));
}

void espNowInit(){
  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  //esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&inputPosition, incomingData, sizeof(inputPosition));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  //Serial.print("Value: ");
  //Serial.println(inputPosition.value);

  thatMotor = inputPosition.value;  
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  /*if (status == 0){
    String success = "Delivery Success :)";
  }
  else{
    String success = "Delivery Fail :(";
  }*/
}

//Temperature status and manager
void tempStatus(){
  static int tFlag;

  //Read voltage from temperature sensor and transform it to °C
  float vOut = analogRead(vTemp);
  float temp = (((vOut*3.3)/4095)-1.8577)/-0.01177;
  //Serial.println(temp,2);
  
  if (temp >= 75 && tFlag == false){
    digitalWrite(enGate, LOW);
    tFlag = true;
    Serial.print("enGate Disabled - Temperature protection: ");
    Serial.println(temp);
  }
  else if (tFlag == true && temp <= 75){
    digitalWrite(enGate, HIGH);
    tFlag = false;
    Serial.println("enGate Enabled - Temperature low");
  }
  
}

//Configure DRV8305 to desired operation mode
void drv_init(){

  //Set to three PWM inputs mode
  digitalWrite(cs, LOW);
  byte resp1 = SPI.transfer(B00111010);
  byte resp2 = SPI.transfer(B10000110);
  digitalWrite(cs, HIGH);
  //010 10010110
  //010 00000110
  Serial.println(resp1, BIN);
  Serial.println(resp2, BIN);
  
  //DC Calibration for current sensing
/*
  digitalWrite(cs, LOW);
  int resp3 = SPI.transfer(B01010111);
  int resp4 = SPI.transfer(B00000000);
  digitalWrite(cs, HIGH);
  Serial.println(resp3);
  Serial.println(resp4);

  digitalWrite(cs, LOW);
  int resp5 = SPI.transfer(B01010000);
  int resp6 = SPI.transfer(B00000000);
  digitalWrite(cs, HIGH);
  Serial.println(resp5);
  Serial.println(resp6);
*/

  /*
  digitalWrite(cs, LOW);
  int resp3 = SPI.transfer(B00101000);
  int resp4 = SPI.transfer(B10001000);
  digitalWrite(cs, HIGH);
  Serial.println(resp3);
  Serial.println(resp4);

  digitalWrite(cs, LOW);
  int resp5 = SPI.transfer(B00110000);
  int resp6 = SPI.transfer(B10001000);
  digitalWrite(cs, HIGH);
  Serial.println(resp5);
  Serial.println(resp6);
  */
  
}

//Fault status and manager for the DRV8305
//Datahseet pages 37 and 38
void faultStatus(){
  //Read nFault pin from DRV8305 - LOW == error / HIGH == normal operation
  int fault = digitalRead(nFault);
  
  if(fault == LOW && faultTrig == false){
    Serial.println("Fault detected");
    faultTrig = true;
    //Check warning and watchdog reset (Address = 0x1)
    digitalWrite(cs, LOW);
    byte ft1 = SPI.transfer(B10001000);
    byte ft2 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x1");
    Serial.println(ft1,BIN);
    Serial.println(ft2,BIN);

    //Check OV/VDS Faults (Address = 0x2)
    digitalWrite(cs, LOW);
    byte ft3 = SPI.transfer(B10010000);
    byte ft4 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x2");
    Serial.println(ft3,BIN);
    Serial.println(ft4,BIN);

    //Check IC Faults (Address = 0x3)
    digitalWrite(cs, LOW);
    byte ft5 = SPI.transfer(B10011000);
    byte ft6 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x3");
    Serial.println(ft5,BIN);
    Serial.println(ft6,BIN);

    //Check VGS Faults (Address = 0x4)
    digitalWrite(cs, LOW);
    byte ft7 = SPI.transfer(B10100000);
    byte ft8 = SPI.transfer(B00000000);
    digitalWrite(cs, HIGH);
    Serial.println("Address = 0x4");
    Serial.println(ft7,BIN);
    Serial.println(ft8,BIN);
  }
}


//Utility function enabling serial communication the user
String serialReceiveUserCommand() {
  // a string to hold incoming data
  static String received_chars;
  String command = "";

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the string buffer:
    received_chars += inChar;
    // end of user input
    if (inChar == '\n') {
      // execute the user command
      command = received_chars;
      // reset the command buffer 
      received_chars = "";
    }
  }
  return command;
}
