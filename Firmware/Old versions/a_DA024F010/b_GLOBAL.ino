
//###########################################
// GLOBAL TAB
//###########################################

// Libraries, pin number assignment and instance initialization

//SimpleFOC Version 2.0.1
#include <SimpleFOC.h>
#include <SPI.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

//#######_THREE PHASE DRIVER - DRV8305_########
// Datasheet: www.ti.com/lit/ds/symlink/drv8305.pdf
#define enGate 2       //Chip Enable
#define nFault 4       //Fault reading
#define cs 5           //DRV8305 Chip-select
#define so1 36
#define so2 35
#define so3 34
bool faultTrig = false;

//######_MAGNETIC SENSOR - AS5147_######
// Datasheet: https://ams.com/documents/20143/36005/AS5147_DS000307_2-00.pdf
#define sensorCS 16             //AS5147 Chip-select

//######_TEMPERATURE SENSOR - STLM20_######
// Datasheet: https://datasheet.lcsc.com/szlcsc/1810010411_STMicroelectronics-STLM20W87F_C129796.pdf
#define vTemp 39

//######_Voltage Monitor_#######
#define vMonitor 33

//#####_TIME MANAGEMENT_#####
float runTime, prevT = 0, timeDif, stateT;
int timeInterval = 1000, totalTempTime;

//####_SIMPLEFOC INSTANCES_####
BLDCMotor motor = BLDCMotor(pp);   //BLDCMotor instance
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27);     //3PWM Driver instance
MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, sensorCS);       //SPI Magnetic sensor instance

//######_SETUP FUNCTIONS INIT_######
void SimpleFOCinit();
static void IRAM_ATTR isr_handler(void*);
void drv_init();

//######_LOOP FUNCTIONS INIT_######
void timeManagement();
void tempStatus(bool debug = false);
void voltageMonitor(bool debug = false);
void rotorPosition();
void faultStatus();
String serialReceiveUserCommand();
