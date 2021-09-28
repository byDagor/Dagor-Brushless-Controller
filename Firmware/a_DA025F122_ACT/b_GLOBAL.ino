
//#################################################
//                    GLOBAL
//#################################################

// Libraries, pin number assignment and instance initialization

//SimpleFOC Version 2.1
#include <SimpleFOC.h>
#include <SPI.h>

#ifdef ESP_NOW
  #include <esp_now.h>
  #include <WiFi.h>
  #include <esp_wifi_internal.h>
#endif

//#############_THREE PHASE DRIVER - DRV8305_########
// Datasheet: www.ti.com/lit/ds/symlink/drv8305.pdf
#define enGate 2            // Chip Enable
#define nFault 4            // Fault reading
#define cs 5                // DRV8305 SPI Chip-select
#define so1 36              // Current sense phase A
#define so2 35              // Current sense phase B
#define so3 34              // Current sense phase C
bool faultTrig = false;

//#############_MAGNETIC SENSOR - AS5147_############
// Datasheet: https://ams.com/documents/20143/36005/AS5147_DS000307_2-00.pdf
#define sensorCS 16             //AS5147 Chip-select

//#############_TEMPERATURE SENSOR - STLM20_#########
// Datasheet: https://datasheet.lcsc.com/szlcsc/1810010411_STMicroelectronics-STLM20W87F_C129796.pdf
#define vTemp 39

//#############_Voltage Monitor_#####################
#define vMonitor 33

//#############_TIME MANAGEMENT_#####################
unsigned long runTime, prevT = 0, timeDif, stateT, loopCount;
int timeInterval = 1000, totalTempTime;

//#############_SIMPLEFOC INSTANCES_#################
BLDCMotor motor = BLDCMotor(pp);                                                  //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27);                               //3PWM Driver instance
LowsideCurrentSense current_sense = LowsideCurrentSense(0.002, 40.0, so1, so2);   //Current sensing instance

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
Commander commandEspNow = Commander();
void onMotorEspNow(char* cmd){ commandEspNow.motor(&motor, cmd); }


//#############_SETUP FUNCTIONS DECLARATION_################
void SimpleFOCinit();
void drv_init();
void spi_init();
void gpio_init();
void current_dc_calib(bool activate);

//#############_LOOP FUNCTIONS DECLARATION_#################
void timeManagement();
void gravityComp();
void tempStatus(bool debug = false);  
void voltageMonitor(bool debug = false);
void rotorData(bool rotorVelocity = false);
void faultStatus();
void phaseCurrents(bool DQspace = false);
void loopTime();
void phaseVoltages(bool DQspace = false);
void sineDemo();

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
