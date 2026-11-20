// DRV8305 status registers 0x1 to 0x4, datasheet SLVSCX2B sections 7.3.8.1 and 7.6.1.
// Reading the registers and sampling nFAULT is done by the DRV8305 library, this file only names the bits.

#pragma once
#include <stdint.h>

const int DRV_STATUS_REGISTERS = 4;                 // Registers 0x1 to 0x4
const int DRV_STATUS_BITS = 11;                     // Data bits D0 to D10
const uint16_t DRV_FAULT_FLAG = 1 << 10;            // Register 0x1, D10: a latched fault is present
const uint16_t DRV_WARNING_BITS = 0x1FF;            // Register 0x1, D0 to D8: report-only warnings
const uint16_t DRV_ALL_BITS = 0x7FF;

// Registers 0x2 to 0x4 only hold faults, register 0x1 holds the warnings and the fault flag.
constexpr bool drvBitIsWarning(int registerIndex, int bit){
  return registerIndex == 0 && bit <= 8;
}

// All four registers reading 0 or all ones means the DRV8305 did not answer: nFAULT is active, so some status bit must be set.
constexpr bool drvStatusLooksValid(const uint16_t* status){
  bool allZero = true;
  bool allOnes = true;
  for (int i = 0; i < DRV_STATUS_REGISTERS; i++){
    if (status[i] != 0) allZero = false;
    if (status[i] != DRV_ALL_BITS) allOnes = false;
  }
  return !allZero && !allOnes;
}

// Bit names as in the datasheet, indexed [register - 1][bit]. nullptr is a reserved bit.
const char* const DRV_STATUS_NAMES[DRV_STATUS_REGISTERS][DRV_STATUS_BITS] = {
  { // 0x1 Warning and watchdog reset
    "Overtemperature warning",
    "Temperature flag setting for approximately 135 degC",
    "Temperature flag setting for approximately 125 degC",
    "Temperature flag setting for approximately 105 degC",
    "Charge pump undervoltage flag warning",
    "Real time OR of all VDS overcurrent monitors",
    "PVDD overvoltage flag warning",
    "PVDD undervoltage flag warning",
    "Temperature flag setting for approximately 175 degC",
    nullptr,
    "Fault indication"
  },
  { // 0x2 OV/VDS faults
    "Sense A overcurrent fault",
    "Sense B overcurrent fault",
    "Sense C overcurrent fault",
    nullptr,
    nullptr,
    "VDS overcurrent fault for low-side MOSFET C",
    "VDS overcurrent fault for high-side MOSFET C",
    "VDS overcurrent fault for low-side MOSFET B",
    "VDS overcurrent fault for high-side MOSFET B",
    "VDS overcurrent fault for low-side MOSFET A",
    "VDS overcurrent fault for high-side MOSFET A"
  },
  { // 0x3 IC faults
    "High-side charge pump overvoltage ABS fault",
    "High-side charge pump overvoltage fault",
    "High-side charge pump undervoltage 2 fault",
    nullptr,
    "Low-side gate supply fault",
    "AVDD undervoltage fault",
    "VREG undervoltage fault",
    nullptr,
    "Overtemperature fault",
    "Watchdog fault",
    "PVDD undervoltage 2 fault"
  },
  { // 0x4 VGS faults
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "VGS gate drive fault for low-side MOSFET C",
    "VGS gate drive fault for high-side MOSFET C",
    "VGS gate drive fault for low-side MOSFET B",
    "VGS gate drive fault for high-side MOSFET B",
    "VGS gate drive fault for low-side MOSFET A",
    "VGS gate drive fault for high-side MOSFET A"
  }
};
