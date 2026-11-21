// Status words and reason codes of the Dagor controller.
// The numbers are fixed forever: a host decodes any firmware version with them, a new reason takes a reserved bit.
// The meaning of every bit and worked examples are in Status-and-Commands-Reference.md of the planning notes.
#pragma once
#include <stdint.h>

// Status word, bits 0 to 2
enum DagorMode : uint8_t { MODE_BOOT = 0, MODE_READY = 1, MODE_RUNNING = 2, MODE_CALIBRATING = 3, MODE_FAULT = 4, MODE_UPDATE = 5 };
// Status word, bits 3 to 4. Follows from the masks: any fault bit is FAULT, otherwise any warning bit is WARNING
enum DagorHealth : uint8_t { HEALTH_GOOD = 0, HEALTH_WARNING = 1, HEALTH_FAULT = 2 };
// Status word, bits 5 to 7
enum DagorRunMode : uint8_t { RUN_NONE = 0, RUN_POSITION = 1, RUN_VELOCITY = 2, RUN_TORQUE = 3, RUN_ACTIVE_COMPLIANCE = 4, RUN_BRAKE = 5, RUN_COAST = 6 };

// Status word, bits 8 to 11
const uint32_t STATUS_GATES_ON = 1UL << 8;
const uint32_t STATUS_DERATED = 1UL << 9;
const uint32_t STATUS_COMMISSIONED = 1UL << 10;
const uint32_t STATUS_SENSOR_CALIBRATED = 1UL << 11;

// warningMask: one bit per warning reason. The comment is the ID of the fault catalog.
enum DagorWarning : uint8_t {
  W_DRV_FLAGS = 0,            // D1  gate driver warning flags
  W_DRV_TEMP = 1,             // D2  gate driver chip temperature flags
  W_BUS_LOW = 2,              // P1  bus voltage low, the stop request to the host
  W_BUS_HIGH = 3,             // P2  bus voltage high
  W_POWER_STAGE_TEMP = 4,     // T1  power stage temperature high
  W_MOTOR_TEMP = 5,           // T2  motor temperature high
  W_OVERLOAD = 6,             // M2  overload
  W_ENCODER_WEAK = 7,         // E1  encoder field weak
  W_SETTINGS_MISSING = 8,     // Y3  stored settings missing or corrupt
  W_UNEXPECTED_RESET = 9      // Y5  the last start was an unexpected reset
};
const uint8_t DAGOR_WARNING_COUNT = 10;

// faultMask: one bit per fault reason
enum DagorFault : uint8_t {
  F_DRV_LATCHED = 0,          // D3a gate driver hardware fault, latched (VDS overcurrent, gate drive fault)
  F_DRV_AUTO = 1,             // D3b gate driver hardware fault, clears by itself (chip over temperature, PVDD lockout, supply undervoltage)
  F_DRV_SETTINGS_LOST = 2,    // D4  the gate driver lost its settings
  F_DRV_NO_ANSWER = 3,        // D5  the gate driver does not answer on SPI
  F_DRV_WATCHDOG = 4,         // D7  the gate driver watchdog expired
  F_BUS_UNDER = 5,            // P1c bus voltage under the low fault level
  F_BUS_OVER = 6,             // P2c bus voltage over the high fault level
  F_POWER_STAGE_OVERTEMP = 7, // T1  power stage over temperature
  F_MOTOR_OVERTEMP = 8,       // T2  motor over temperature
  F_ROTOR_RANGE = 9,          // M1  rotor outside the allowed range
  F_OVERLOAD = 10,            // M2  overload
  F_OVERSPEED = 11,           // M4  overspeed
  F_TRACKING = 12,            // M5  tracking error
  F_ENCODER = 13,             // E1  encoder failure
  F_CURRENT_OFFSETS = 14,     // E2  current sense offsets implausible
  F_MOTOR_CONNECTION = 15,    // E3a motor not connected, a phase open or a short
  F_CALIBRATION_MISMATCH = 16,// E3b stored calibration does not match the rotor
  F_COMMS_TIMEOUT = 17,       // C1  communication timeout
  F_LOOP_HANG = 18,           // Y1  the control loop hangs
  F_SIMPLEFOC_INIT = 19,      // Y2  SimpleFOC initialisation failed
  F_FIRMWARE_CHECK = 20       // Y4  new firmware failed its checks
};
const uint8_t DAGOR_FAULT_COUNT = 21;

constexpr uint32_t dagorStatus(uint8_t mode, uint8_t health, uint8_t runMode, uint32_t flags){
  return (uint32_t)(mode & 0x7) | ((uint32_t)(health & 0x3) << 3) | ((uint32_t)(runMode & 0x7) << 5) | flags;
}

constexpr uint8_t dagorHealthOf(uint32_t warnings, uint32_t faults){
  return faults ? HEALTH_FAULT : (warnings ? HEALTH_WARNING : HEALTH_GOOD);
}

// The masks of the real events. A bit is set by the task that detects the event, one atomic operation, no lock.
extern volatile uint32_t dagorWarningMask;
extern volatile uint32_t dagorFaultMask;
#ifdef DEBUG_INJECT
// Bits set by the J command for tests, kept apart from the real ones so that they never hide or clear a real event
extern volatile uint32_t dagorInjectedWarning;
extern volatile uint32_t dagorInjectedFault;
#endif

inline void raiseWarning(uint8_t bit){ if (bit < 32) __atomic_fetch_or(&dagorWarningMask, 1UL << bit, __ATOMIC_RELAXED); }
inline void clearWarning(uint8_t bit){ if (bit < 32) __atomic_fetch_and(&dagorWarningMask, ~(1UL << bit), __ATOMIC_RELAXED); }
inline void raiseFault(uint8_t bit){ if (bit < 32) __atomic_fetch_or(&dagorFaultMask, 1UL << bit, __ATOMIC_RELAXED); }
inline void clearFault(uint8_t bit){ if (bit < 32) __atomic_fetch_and(&dagorFaultMask, ~(1UL << bit), __ATOMIC_RELAXED); }

// What is reported: the real events, plus the injected ones in a debug build
inline uint32_t dagorReportedWarnings(){
  uint32_t mask = dagorWarningMask;
#ifdef DEBUG_INJECT
  mask |= dagorInjectedWarning;
#endif
  return mask;
}
inline uint32_t dagorReportedFaults(){
  uint32_t mask = dagorFaultMask;
#ifdef DEBUG_INJECT
  mask |= dagorInjectedFault;
#endif
  return mask;
}
