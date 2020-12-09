//Dagor 2.1 Brushless Controller Firmware 022 (Includes current sensing, not yet used for control)

//SimpleFOC Version 2.0.1
#include <SimpleFOC.h>
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

//#######_USER VARIABLES_#######
byte pp = 7;                  //BLDC motor number of pole pairs
float phaseRes = 0.560;       //Phase winding resistance [ohms]
byte sourceVoltage = 12;      //Voltage of your power source [Volts]
byte maxCurrent = 2;          //Rough approximation of max current [Amps]

//#######_CONTROLLER PARAMETERS_#######
float ki = 0.002;
float ti = 2.5; 
float lpFilter = 0.000;
float kp = 10;
float voltageRamp = 25;
float voltageLimit = phaseRes*maxCurrent;
float velocityLimit = 2000;

//######_ESPNOW_######
typedef struct struct_message {
    String function;
    float value;
} struct_message;

struct_message dagorData;

//#######_DRV8305_########
//Datasheet: www.ti.com/lit/ds/symlink/drv8305.pdf
#define enGate 2       //Chip Enable
#define nFault 4       //Fault reading
#define cs 5           //DRV8305 Chip-select
#define so1 36
#define so2 35
#define so3 34
bool faultTrig = false;

//####_CURRENT READING_####
static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1};
static portMUX_TYPE mcpwm_spinlock = portMUX_INITIALIZER_UNLOCKED;
int a1, a2, a3;         //Current readings from internal current sensor amplifiers
static void IRAM_ATTR isr_handler(void*);
byte currentState = 1;

//######_AS5147_######
#define sensorCS 16             //AS5147 Chip-select
//A and B encoder inputs
//#define arduinoInt1 32             // interrupt 0
//#define arduinoInt2 33             // interrupt 1

//######_TEMPERATURE SENSOR_######
#define vTemp 39
byte maxTemp = 80;      //Maximum temperature [°C]

//#####_TIME MANAGEMENT_#####
float runTime, prevT = 0, timeDif, stateT;
int timeInterval = 1000, totalTempTime;

//####_SIMPLEFOC INSTANCES_####
BLDCMotor motor = BLDCMotor(pp);   //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27);     //3PWM Driver instance
MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, sensorCS);       //SPI Magnetic sensor instance
//Encoder sensor = Encoder(4, 2, 1024);       // Quadrature encoder instance

// Interrupt routine intialisation
// channel A and B callbacks
//void doA(){sensor.handleA();}
//void doB(){sensor.handleB();}

//--------------------------------------------Set-up--------------------------------------------
void setup() {
  Serial.begin(115200);

  analogSetCycles(2);

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
  _delay(500);

  Serial.println("enGate Enabled");
  digitalWrite(enGate, HIGH);
  _delay(500);

  Serial.println("ESP-NOW init");
  espNowInit();
   _delay(500);

  //Initialise magnetic sensor hardware
  sensor.init();

  //If using encoder
  //sensor.enableInterrupts(doA, doB); 
  
  //Link the motor to the sensor
  motor.linkSensor(&sensor);
  
  // driver config, power supply voltage [V]
  driver.voltage_power_supply = sourceVoltage;
  driver.init();
  motor.linkDriver(&driver);

  // set FOC loop to be used: ControlType::voltage, velocity, angle
  motor.controller = ControlType::angle;

  // Sensor aligning voltage
  motor.voltage_sensor_align = voltageLimit/2;
  
  // velocity PI controller parameters, default K=0.5 Ti = 0.01
  motor.PID_velocity.P = ki;
  motor.PID_velocity.I = ti;
  motor.PID_velocity.D = 0;
  motor.voltage_limit = voltageLimit;
  motor.LPF_velocity.Tf = lpFilter;
  
  // angle P controller, default K=70
  motor.P_angle.P = kp;
  // maximal velocity of the poisition control, default 20
  motor.velocity_limit = velocityLimit;

  motor.useMonitoring(Serial);      // use monitoring functionality
  motor.init();                     // initialise motor
  motor.initFOC(4.5,CW);            // align sensor/ encoder and start FOC
  //motor.initFOC();
  Serial.println("Ready BLDC.");
  
  _delay(500);
  Serial.println(motor.zero_electric_angle);

  MCPWM[MCPWM_UNIT_0]->int_ena.timer0_tep_int_ena = true;//A PWM timer 0 TEP event will trigger this interrupt
  MCPWM[MCPWM_UNIT_0]->int_ena.timer1_tep_int_ena = true;//A PWM timer 1 TEP event will trigger this interrupt
  MCPWM[MCPWM_UNIT_0]->int_ena.timer2_tep_int_ena = true;//A PWM timer 2 TEP event will trigger this interrupt
  mcpwm_isr_register(MCPWM_UNIT_0, isr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);  //Set ISR Handler
}

//--------------------------------------------Loop--------------------------------------------
void loop() {
  motor.loopFOC();
  motor.move(); // target set by the motor.command()  - or by variable motor.target

  float e = sensor.getAngle();
  //Serial.println(e,4);
  
  motor.command(serialReceiveUserCommand());        //user communication

  //Time managment
  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;

  //Functions inside this "if" will execute at a 10hz rate
  if(stateT >= 200000){
    tempStatus();
    faultStatus();
    voltageMonitor();
    Serial.print(a1);
  Serial.print(", ");
  Serial.print(a2);
  Serial.print(", ");
  Serial.println(a3);
  }
  
}

//Temperature status and manager
void tempStatus(){
  static int tFlag;

  //Read voltage from temperature sensor and transform it to °C
  float vOut = analogRead(vTemp);
  float temp = (((vOut*3.3)/4095)-1.8577)/-0.01177;
  //Serial.println(temp,2);
  
  if (temp >= maxTemp && tFlag == false){
    int tempTime = micros();
    totalTempTime += tempTime;

    //If temperature is high for 3 seconds disable DRV
    if(totalTempTime >= 3000000){
      tFlag = true;
      digitalWrite(enGate, LOW);
      Serial.print("enGate Disabled - Temperature protection: ");
      Serial.println(temp);
    }
    
  }
  else if (temp <= maxTemp && tFlag == false){
    totalTempTime = 0;
  }
  
}

void voltageMonitor(){
  int rawVoltage = analogRead(17);
  int bVoltage = rawVoltage * 24/4095;
}

static void IRAM_ATTR isr_handler(void*){
  uint32_t mcpwm_intr_status_0;
  uint32_t mcpwm_intr_status_1;
  uint32_t mcpwm_intr_status_2;
  mcpwm_intr_status_0 = MCPWM[MCPWM_UNIT_0]->int_st.timer0_tep_int_st;
  mcpwm_intr_status_1 = MCPWM[MCPWM_UNIT_0]->int_st.timer1_tep_int_st;
  mcpwm_intr_status_2 = MCPWM[MCPWM_UNIT_0]->int_st.timer2_tep_int_st;
  if(mcpwm_intr_status_0 > 0 && currentState == 1){
    a1 = analogRead(so1);
    currentState = 2; 
  }
  else if(mcpwm_intr_status_1 > 0 && currentState == 2){
    a2 = analogRead(so2);
    currentState = 3;
  }
  else if(mcpwm_intr_status_2 > 0 && currentState == 3){
    a3 = analogRead(so3);  
    currentState = 1;
  }
  MCPWM[MCPWM_UNIT_0]->int_clr.timer0_tep_int_clr = mcpwm_intr_status_0;
  MCPWM[MCPWM_UNIT_0]->int_clr.timer1_tep_int_clr = mcpwm_intr_status_1;
  MCPWM[MCPWM_UNIT_0]->int_clr.timer2_tep_int_clr = mcpwm_intr_status_2;
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

  //Adds delay on the current sense signals
  /*
  digitalWrite(cs, LOW);
  int resp5 = SPI.transfer(B01010000);
  int resp6 = SPI.transfer(B11000000);
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

void espNowInit(){
  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&dagorData, incomingData, sizeof(dagorData));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  //Serial.print("Function: ");
  //Serial.println(dagorData.function);
  //Serial.print("Value: ");
  //Serial.println(dagorData.value);

  String val = String(dagorData.value,3);
  if(dagorData.function == "Target"){
    motor.command(val);
  }
  else if(dagorData.function == "C1"){
    //action("C1");
    motor.command("0");
    motor.command("C1");
  }
  else if(dagorData.function == "C0"){
    //action("C0");
    motor.command("0");
    motor.command("C0");
  }
  else if(dagorData.function == "C2"){
    //action("C2");
    motor.command("0");
    motor.command("C2");
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
