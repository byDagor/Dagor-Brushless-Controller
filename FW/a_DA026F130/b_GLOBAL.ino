//#################################################
//                     GLOBAL
//#################################################

// Libraries, pin number assignment and instance initialization

//SimpleFOC Version 2.3
//#include <SimpleFOC.h>
#include <SPI.h>

#ifdef CALIBRATED_SENSOR
  #include <SimpleFOCDrivers.h>
  #include <encoders/calibrated/CalibratedSensor.h>
#endif

#ifdef ESP_NOW
  #include <esp_now.h>
  #include <WiFi.h>
  #include <esp_private/wifi.h>
#endif

enum Dagor_state {
  LIFE_IS_GOOD,
  ACTIVE_COMP,
  DRV_WARNING,
  FETS_TEMP_WARNING,
  MOTOR_TEMP_WARNING,
  SIMPLEFOC_WARNING,
  DRV_ERROR,
  FETS_TEMP_ERROR,
  MOTOR_TEMP_ERROR,
  SIMPLEFOC_ERROR
} state_machine = LIFE_IS_GOOD;


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
#define sensorCS 16             //AS5147 SPI Chip-select

//#############_TEMPERATURE SENSOR - STLM20_#########
// Datasheet: https://datasheet.lcsc.com/szlcsc/1810010411_STMicroelectronics-STLM20W87F_C129796.pdf
#define vTemp 39

//#############_Voltage Monitor_#####################
#define vMonitor 33

//#############_FUNCTIONS DECLARATION_################
//void drv_enable();
void spi_init();
void gpio_init();
void current_dc_calib(bool activate);
void calibratePhaseZeroOffset();
//unsigned long timeManagement();
void gravityComp();


//#############_ESP-NOW FUNCTIONS DECLARATION_#############################
#ifdef ESP_NOW
  
  typedef struct input_message {
      int act_id;
      char act_commander1;
      char act_commander2;
      char act_commander3;
      float act_target_value;
  } input_message;

  typedef struct output_message {
      int act_id;
      float act_position;
      float act_velocity;
      float act_current;
      float act_commander_value;
  } output_message;
  
  input_message inputData;
  output_message outputData;
  esp_now_peer_info_t peerInfo;
  
  void espNowInit();
  void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
  void sendData();
  void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  
#endif



#ifdef RS485

  #define COMMS_DIR 12
  #define UART_RX 22
  #define UART_TX 21

  #include "SerialTransfer.h"

  SerialTransfer myTransfer;

  struct __attribute__((packed)) input_message {
      int act_id;
      char act_commander1;
      char act_commander2;
      char act_commander3;
      float act_target_value;
  } input_message;

  struct __attribute__((packed)) output_message {
      int act_id;
      char act_state;
      float act_position;
      float act_velocity;
      float act_current;
      float act_commander_value;
  } output_message;

  //input_message inputData;
  //output_message outputData;

#endif
