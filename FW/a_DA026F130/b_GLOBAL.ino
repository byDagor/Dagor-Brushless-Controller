//#################################################
//                     GLOBAL
//#################################################

// Libraries, pin number assignment and instance initialization

//SimpleFOC Version 2.1
#include <SimpleFOC.h>
#include <SPI.h>

#ifdef ESP_NOW
  #include <esp_now.h>
  #include <WiFi.h>
  #include <esp_private/wifi.h>
#endif

enum Dagor_state {
  LIFE_IS_GOOD,
  ACTIVE_COMP,
  SIMPLEFOC_ERROR,
  DRV_WARNING,
  DRV_FAULT,
  FETS_TEMP_WARNING,
  FETS_TEMP_FAULT
} state_machine;


//#############_THREE PHASE DRIVER - DRV8305_########
// Datasheet: www.ti.com/lit/ds/symlink/drv8305.pdf
#define enGate 2            // Chip Enable
#define nFault 4            // Fault reading
#define cs 5                // DRV8305 SPI Chip-select
#define SO1 36              // Current sense phase A
#define SO2 35              // Current sense phase B
#define SO3 34              // Current sense phase C
#define INHA 27             // PWM input signal for bridge A high side
#define INHB 26             // PWM input signal for bridge B high side
#define INHC 25             // PWM input signal for bridge C high side

//#############_MAGNETIC SENSOR - AS5147_############
// Datasheet: https://ams.com/documents/20143/36005/AS5147_DS000307_2-00.pdf
#define sensorCS 16             //AS5147 Chip-select

//#############_TEMPERATURE SENSOR - STLM20_#########
// Datasheet: https://datasheet.lcsc.com/szlcsc/1810010411_STMicroelectronics-STLM20W87F_C129796.pdf
#define vTemp 39

//#############_Voltage Monitor_#####################
#define vMonitor 33

//#############_TIME MANAGEMENT_#####################
unsigned long stateT = 0;

//#############_SIMPLEFOC INSTANCES_#################
BLDCMotor motor = BLDCMotor(pp);                                                  //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(INHC, INHB, INHA);                               //3PWM Driver instance
LowsideCurrentSense current_sense = LowsideCurrentSense(0.002, 80.0, SO1, SO2);   //Current sensing instance

#ifdef ENCODER
  Encoder sensor = Encoder(32, 17, 512);       // Quadrature encoder instance
  // Interrupt routine intialisation -> channel A and B callbacks
  void doA(){sensor.handleA();}
  void doB(){sensor.handleB();}
#else
  MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, sensorCS);     //SPI Magnetic sensor instance
#endif

//#############_COMMANDER INTERFACE_#################
Commander command = Commander(Serial);
void onMotor(char* cmd){ command.motor(&motor, cmd); }

#ifdef ESP_NOW
  Commander commandEspNow = Commander();
  void onMotorEspNow(char* cmd){ commandEspNow.motor(&motor, cmd); }
#endif

//#############_SETUP FUNCTIONS DECLARATION_################
int SimpleFOCinit();
void drv_init();
void spi_init();
void gpio_init();
void current_dc_calib(bool activate);

//#############_LOOP FUNCTIONS DECLARATION_#################
unsigned long timeManagement();
void gravityComp();
void tempStatus(unsigned long timeDif, bool debug = false);  
void voltageMonitor(bool debug = false);
void rotorData(bool rotorVelocity = false);
void faultStatus();
void phaseCurrents(bool DQspace = false);
void loopPeriod(bool reset = false);
void phaseVoltages(bool DQspace = false);

//#############_ESP-NOW FUNCTIONS DECLARATION_#############################
#ifdef ESP_NOW
  
  typedef struct input_message {
      int leg_id;
      String kneeFunc;
      float kneeValue;
      String hipFunc;
      float hipValue;
      String shoulderFunc;
      float shoulderValue;
  } input_message;

  typedef struct output_message {
      int leg_id;
      float actPosition;
      float actVelocity;
      float actQCurrent;
  } output_message;
  
  input_message inputData;
  output_message outputData;
  esp_now_peer_info_t peerInfo;
  
  void espNowInit();
  void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
  void sendData();
  void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  
#endif
