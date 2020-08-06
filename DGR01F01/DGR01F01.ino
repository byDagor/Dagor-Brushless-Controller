//#####_Libraries_#####
#include <SPI.h>
#include <SimpleFOC.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include <esp_now.h>
#include <WiFi.h>

//######_ESPNOW_######
typedef struct struct_message {
    String function;
    float value;
} struct_message;

struct_message myData;

//######_MCPWM Pins_#######
#define GPIO_PWM0A_OUT 25   //Set GPIO 25 as PWM0A
#define GPIO_PWM1A_OUT 26   //Set GPIO 26 as PWM1A
#define GPIO_PWM2A_OUT 27   //Set GPIO 27 as PWM2A

//######_DRV8305_######
#define enGate 17       //Chip Enable
#define nFault 14       //Fault reading
#define so1 36          //Pins connected to DRV's current sensors, one for each phase
#define so2 35
#define so3 34
int a1, a2, a3;         //Current readings from internal current sensor amplifiers

//######_MA730_######
//A and B encoder inputs
#define arduinoInt1 4             // interrupt 0
#define arduinoInt2 2             // interrupt 1

//######_TEMPERATURE_######
#define vTemp 39
float vOut, temp;
bool tFlag = false;

//########_BLDC Motor_########
//  BLDCMotor( int phA, int phB, int phC, int pp, int en)
//  - phA, phB, phC - motor A,B,C phase pwm pins
//  - pp            - pole pair number
//  - enable pin    - (optional input)
//  - power percentage
BLDCMotor motor = BLDCMotor(25, 26, 27, 7, 0.1);

//  Encoder(int encA, int encB , int cpr, int index)
//  - encA, encB    - encoder A and B pins
//  - ppr           - impulses per rotation  (cpr=ppr*4)
//  - index pin     - (optional input)
Encoder encoder = Encoder(arduinoInt1, arduinoInt2, 1024);
// interrupt ruotine intialisation
void doA1(){encoder.handleA();}
void doB1(){encoder.handleB();}

//#####_SPI_#######
#define cs 5      //Chip-select

//#####_System Gains_#####
float ki = 0.03;
float ti = 1.0;
float lpFilter = 0.000;
float kp = 15;
float voltageRamp = 100;
float voltageLimit = 1.4/2;
float velocityLimit = 2000;

//Time managing variables
float runTime, prevT = 0, timeDif, stateT;
int timeInterval = 1000;

//Target variable
float target = 0;
// loop stats variables
unsigned long t = 0;
long timestamp = micros();

static void IRAM_ATTR setup_MCPWM();

void setup() {
  //Serial communication to virtual COM
  Serial.begin(115200);
  
  _delay(350);
  setup_MCPWM();
  _delay(350);

  //  Quadrature::ENABLE - CPR = 4xPPR  - default
  //  Quadrature::DISABLE - CPR = PPR
  encoder.quadrature = Quadrature::ENABLE;
  // check if you need internal pullups
  // Pullup::EXTERN - external pullup added - dafault
  // Pullup::INTERN - needs internal arduino pullup
  encoder.pullup = Pullup::EXTERN;
  // initialise encoder hardware
  encoder.init();
  encoder.enableInterrupts(doA1,doB1);

  //Pin modes for every pin
  pinMode(vTemp, INPUT);
  pinMode(enGate, OUTPUT);
  digitalWrite(enGate, LOW);
  pinMode(nFault, INPUT);
  pinMode(so1, INPUT);
  pinMode(so2, INPUT);
  pinMode(so3, INPUT);

  //SPI start up
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  //Serial.println(MOSI);
  //Serial.println(MISO);
  //Serial.println(SCK);

  //Motor driver initialization
  delay(500);
  Serial.println("DRV8305 INIT");
  drv_init();
  delay(500);

  Serial.println("enGate Enabled");
  digitalWrite(enGate, HIGH);
  delay(500);

  //BLDC motor initialization
  motor.voltage_power_supply = 12;

  // set FOC loop to be used: ControlType::voltage, velocity, angle
  motor.controller = ControlType::angle;
  
  // velocity PI controller parameters, default K=0.5 Ti = 0.01
  motor.PI_velocity.P = ki;
  motor.PI_velocity.I = ti;
  motor.PI_velocity.voltage_limit = voltageLimit;
  motor.PI_velocity.voltage_ramp = voltageRamp;
  motor.LPF_velocity.Tf = lpFilter;
  
  // angle P controller, default K=70
  motor.P_angle.P = kp;
  // maximal velocity of the poisiiton control, default 20
  motor.P_angle.velocity_limit = velocityLimit;
  
  // link the motor to the sensor
  motor.linkSensor(&encoder);
  // intialise motor
  motor.init();
  // align encoder and start FOC
  motor.initFOC();
  Serial.println("Ready BLDC.");
  _delay(1000);
  printGains();

  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
  motor.loopFOC();
  serialEvent();

  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;

  if(stateT >= 1000){
    motor.move(target);
    stateT = 0;
    //Serial.println(sp);
  }
  
  //Read voltage from temperature sensor and transform it to °C
  vOut = analogRead(vTemp);
  temp = ((vOut*(3.3)/4095)-0.4)/0.0195;
  /*if (temp >= 75 && tFlag == false){
    digitalWrite(enGate, LOW);
    tFlag = true;
    Serial.print("enGate Disabled - Temperature protection: ");
    Serial.println(temp);
  }
  else if (tFlag == true && temp <= 75){
    digitalWrite(enGate, HIGH);
    tFlag = false;
    Serial.println("enGate Enabled - Temperature low");
  }*/
  //Serial.println(temp);

  //Read nFault pin from DRV8305 - LOW == error / HIGH == normal operation
  int fault = digitalRead(nFault);
  //Serial.println(fault);

  //Read voltages from the DRV8305 current sensors and sense resistors
  a1 = analogRead(so1);
  a2 = analogRead(so2);
  a3 = analogRead(so3);

  //Serial.print(a1);
  //Serial.print(",");
  //Serial.print(a2);
  //Serial.print(",");
  //Serial.println(a3);
}

//Set DRV8305 to three PWM inputs mode
void drv_init(){
  digitalWrite(cs, LOW);
  int resp = SPI.transfer(B00111010);
  int resp2 = SPI.transfer(B10000110);
  digitalWrite(cs, HIGH);
  //010 10010110
  //010 00000110
  Serial.println(resp);
  Serial.println(resp2);
}

// Serial communication callback
void serialEvent() {
  // a string to hold incoming data
  static String inputString;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline
    // end of input
    if (inChar == '\n') {
      if(inputString.charAt(0) == 'P'){
        motor.PI_velocity.P = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'I'){
        motor.PI_velocity.I = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'F'){
        motor.LPF_velocity.Tf = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'K'){
        motor.P_angle.P = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'R'){
        motor.PI_velocity.voltage_ramp = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'L'){
        motor.PI_velocity.voltage_limit = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'V'){
        motor.P_angle.velocity_limit = inputString.substring(1).toFloat();
        printGains();
      }else if(inputString.charAt(0) == 'T'){
        Serial.print("Average loop time is (microseconds): ");
        Serial.println((micros() - timestamp)/t);
        t = 0;
        timestamp = micros();
      }else if(inputString.charAt(0) == 'C'){
        Serial.print("Contoller type: ");
        int cnt = inputString.substring(1).toFloat();
        if(cnt == 0){
          Serial.println("angle!");
          motor.controller = ControlType::angle;
        }else if(cnt == 1){
          Serial.println("velocity!");
          motor.controller = ControlType::velocity;
        }else if(cnt == 2){
          Serial.println("volatge!");
          motor.controller = ControlType::voltage;
        }
        Serial.println();
        t = 0;
        timestamp = micros();
      }else{
        target = inputString.toFloat();
        Serial.print("Target : ");
        Serial.println(target);
        inputString = "";
      }
      inputString = "";
    }
  }
}

//Function that prints the value of the system control gains.
void printGains(){
  Serial.print("PI velocity P: ");
  Serial.print(motor.PI_velocity.P,4);
  Serial.print(",\t I: ");
  Serial.print(motor.PI_velocity.I,4);
  Serial.print(",\t Low passs filter Tf: ");
  Serial.print(motor.LPF_velocity.Tf,4);
  Serial.print(",\t Kp angle: ");
  Serial.println(motor.P_angle.P);
  Serial.print("Voltage ramp: ");
  Serial.print(motor.PI_velocity.voltage_ramp);
  Serial.print(",\t Voltage limit: ");
  Serial.print(motor.PI_velocity.voltage_limit);
  Serial.print(",\t Velocity limit: ");
  Serial.println(motor.P_angle.velocity_limit);
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Function: ");
  Serial.println(myData.function);
  Serial.print("Value: ");
  Serial.println(myData.value);
  Serial.println();
  target = myData.value;
}

static void setup_MCPWM_pins(){
    Serial.println("Initializing MCPWM control GPIOs...");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, GPIO_PWM1A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM2A, GPIO_PWM2A_OUT);
}

#define TIMER_CLK_PRESCALE 1
#define MCPWM_CLK_PRESCALE 0
#define MCPWM_PERIOD_PRESCALE 4
#define MCPWM_PERIOD_PERIOD 2048

static void IRAM_ATTR setup_MCPWM(){
     setup_MCPWM_pins();

     mcpwm_config_t pwm_config;
     pwm_config.frequency = 4000000;  //frequency = 20000Hz
     pwm_config.cmpr_a = 0;      //duty cycle of PWMxA = 50.0%
     pwm_config.cmpr_b = 0;      //duty cycle of PWMxB = 50.0%
     pwm_config.counter_mode = MCPWM_UP_DOWN_COUNTER; // Up-down counter (triangle wave)
     pwm_config.duty_mode = MCPWM_DUTY_MODE_0; // Active HIGH
     mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
     mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);    //Configure PWM0A & PWM0B with above settings
     mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &pwm_config);    //Configure PWM0A & PWM0B with above settings
     Serial.print("clk_cfg.prescale = ");
     Serial.println( MCPWM0.clk_cfg.prescale);
     Serial.print("timer[0].period.prescale = ");
     Serial.println( MCPWM0.timer[0].period.prescale);
     Serial.print("timer[0].period.period = ");
     Serial.println( MCPWM0.timer[0].period.period);
     Serial.print("timer[0].period.upmethod = ");
     Serial.println( MCPWM0.timer[0].period.upmethod);

     mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
     mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
     mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_2);
     MCPWM0.clk_cfg.prescale = MCPWM_CLK_PRESCALE;
  
     MCPWM0.timer[0].period.prescale = MCPWM_PERIOD_PRESCALE;
     MCPWM0.timer[1].period.prescale = MCPWM_PERIOD_PRESCALE;
     MCPWM0.timer[2].period.prescale = MCPWM_PERIOD_PRESCALE;    
     delay(1);
     MCPWM0.timer[0].period.period = MCPWM_PERIOD_PERIOD;
     MCPWM0.timer[1].period.period = MCPWM_PERIOD_PERIOD;
     MCPWM0.timer[2].period.period = MCPWM_PERIOD_PERIOD;
     delay(1);
     MCPWM0.timer[0].period.upmethod =0;
     MCPWM0.timer[1].period.upmethod =0;
     MCPWM0.timer[2].period.upmethod =0;
     delay(1); 
     Serial.print("clk_cfg.prescale = ");
     Serial.println( MCPWM0.clk_cfg.prescale);
     Serial.print("timer[0].period.prescale = ");
     Serial.println( MCPWM0.timer[0].period.prescale);
     Serial.print("timer[0].period.period = ");
     Serial.println( MCPWM0.timer[0].period.period);
     mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
     mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_1);
     mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_2);
   
     mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_SELECT_SYNC_INT0, 0);
     mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_SELECT_SYNC_INT0, 0);
     mcpwm_sync_enable(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_SELECT_SYNC_INT0, 0);
     delayMicroseconds(1000);
     MCPWM0.timer[0].sync.out_sel = 1;
     delayMicroseconds(1000);
     MCPWM0.timer[0].sync.out_sel = 0;

} // setup_mcpwm
