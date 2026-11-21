//###########################################
//   STATUS WORDS AND THEIR COMMANDS
//###########################################

// The masks that dagor_state.h declares. Reasons are listed in Status-and-Commands-Reference.md.
volatile uint32_t dagorWarningMask = 0;
volatile uint32_t dagorFaultMask = 0;
#ifdef DEBUG_INJECT
volatile uint32_t dagorInjectedWarning = 0;
volatile uint32_t dagorInjectedFault = 0;
#endif

// Short names for the S command, the long texts are in the reference document
static const char* const warningNames[DAGOR_WARNING_COUNT] = {
  "D1 driver warning", "D2 driver temperature flag", "P1 bus voltage low", "P2 bus voltage high", "T1 power stage hot",
  "T2 motor hot", "M2 overload", "E1 encoder field weak", "Y3 stored settings missing", "Y5 unexpected reset"
};
static const char* const faultNames[DAGOR_FAULT_COUNT] = {
  "D3a driver fault latched", "D3b driver fault auto", "D4 driver settings lost", "D5 driver not answering", "D7 driver watchdog",
  "P1c bus under fault level", "P2c bus over fault level", "T1 power stage overtemp", "T2 motor overtemp", "M1 rotor out of range",
  "M2 overload fault", "M4 overspeed", "M5 tracking error", "E1 encoder failure", "E2 current offsets",
  "E3a motor connection", "E3b calibration mismatch", "C1 comms timeout", "Y1 loop hang", "Y2 SimpleFOC init failed",
  "Y4 firmware check failed"
};

// Power stage temperature [C], the STLM20 next to the MOSFETs
float powerStageTemperature(){
  float vOut = adcRead(vTemp);
  return (((vOut * 3.3f) / 4095.0f) - 1.8577f) / -0.01177f;
}

static uint8_t dagorRunMode(){
  if (!motor.enabled) return RUN_NONE;
  if (active_comp_mode) return RUN_ACTIVE_COMPLIANCE;
  switch (motor.controller){
    case MotionControlType::angle:
    case MotionControlType::angle_nocascade: return RUN_POSITION;
    case MotionControlType::velocity: return RUN_VELOCITY;
    case MotionControlType::torque: return RUN_TORQUE;
    default: return RUN_NONE;
  }
}

static void printHex32(uint32_t value){
  char text[11];
  snprintf(text, sizeof(text), "0x%08lX", (unsigned long)value);
  Serial.print(text);
}

static void printReasons(uint32_t mask, const char* const* names, uint8_t count){
  if (!mask){ Serial.print("none"); return; }
  bool first = true;
  for (uint8_t bit = 0; bit < 32; bit++){
    if (!((mask >> bit) & 1)) continue;
    if (!first) Serial.print("; ");
    first = false;
    if (bit < count){
      Serial.print(names[bit]);
    } else {
      Serial.print("bit ");
      Serial.print(bit);
    }
  }
}

// S: two lines, the state in words and the three status words, then the active reasons and live values
void onStatusCommand(char* cmd){
  noteCommand('S', cmd);
  static const char* const modes[] = {"Boot", "Ready", "Running", "Calibrating", "Fault", "Update"};
  static const char* const healths[] = {"LIFE_IS_GOOD", "WARNING", "FAULT"};
  static const char* const runModes[] = {"none", "Position", "Velocity", "Torque", "ActiveCompliance", "Brake", "Coast"};
  static uint32_t lastCount = 0;
  static int64_t lastUs = 0;

  uint32_t warnings = dagorReportedWarnings();
  uint32_t faults = dagorReportedFaults();
  uint8_t health = dagorHealthOf(warnings, faults);
  uint8_t mode = (faults || state_machine >= DRV_WARNING) ? MODE_FAULT : MODE_READY;
  uint8_t runMode = dagorRunMode();
  uint32_t status = dagorStatus(mode, health, runMode, motor.enabled ? STATUS_GATES_ON : 0);

  Serial.print("S: mode ");
  Serial.print(modes[mode]);
  Serial.print(", health ");
  Serial.print(healths[health]);
  Serial.print(", run ");
  Serial.print(runModes[runMode]);
  Serial.print(", gates ");
  Serial.print(motor.enabled ? "on" : "off");
  Serial.print(" | status ");
  printHex32(status);
  Serial.print(" warn ");
  printHex32(warnings);
  Serial.print(" fault ");
  printHex32(faults);
#ifdef DEBUG_INJECT
  Serial.print(" | injected warn ");
  printHex32(dagorInjectedWarning);
  Serial.print(" fault ");
  printHex32(dagorInjectedFault);
#endif
  Serial.println();

  Serial.print("S: faults ");
  printReasons(faults, faultNames, DAGOR_FAULT_COUNT);
  Serial.print(", warnings ");
  printReasons(warnings, warningNames, DAGOR_WARNING_COUNT);
  Serial.print(" | angle ");
  Serial.print(motor.shaft_angle, 3);
  Serial.print(" rad vel ");
  Serial.print(motor.shaft_velocity, 2);
  Serial.print(" rad/s iq ");
  Serial.print(motor.current.q, 3);
  Serial.print(" A vbus ");
  Serial.print(readBusVoltage(), 1);
  Serial.print(" V temp ");
  Serial.print(powerStageTemperature(), 0);
  Serial.print(" C loop ");
  uint32_t count = loopIterations;
  int64_t nowUs = esp_timer_get_time();
  if (lastUs == 0) Serial.print("first call");
  else {
    Serial.print((count - lastCount) * 1000.0f / (nowUs - lastUs), 3);
    Serial.print(" kHz");
  }
  lastCount = count;
  lastUs = nowUs;
  Serial.print(" | legacy state ");
  Serial.println((int)state_machine);
}

// R: software restart. The gate driver is switched off first.
void onRestartCommand(char* cmd){
  noteCommand('R', cmd);
  Serial.println("R: restarting");
  Serial.flush();
  driver.disableGate();
  delay(20);
  ESP.restart();
}

#ifdef DEBUG_INJECT
// J: sets a reason bit for tests, JW<bit> a warning, JF<bit> a fault, J0 clears every injected bit.
// It touches only the injected masks: not the gate driver, not the control loop, not the old state machine.
void onInjectCommand(char* cmd){
  noteCommand('J', cmd);
  if (cmd[0] == '0'){
    dagorInjectedWarning = 0;
    dagorInjectedFault = 0;
    Serial.println("J: injected bits cleared");
    return;
  }
  bool warning = cmd[0] == 'W';
  bool fault = cmd[0] == 'F';
  int bit = atoi(cmd + 1);
  if ((!warning && !fault) || cmd[1] < '0' || cmd[1] > '9' || bit < 0 || bit > 31){
    Serial.println("J: use JW<bit>, JF<bit> or J0");
    return;
  }
  const char* name = "reserved";
  if (warning){
    dagorInjectedWarning |= 1UL << bit;
    if (bit < DAGOR_WARNING_COUNT) name = warningNames[bit];
  } else {
    dagorInjectedFault |= 1UL << bit;
    if (bit < DAGOR_FAULT_COUNT) name = faultNames[bit];
  }
  Serial.print("J: ");
  Serial.print(warning ? "warning" : "fault");
  Serial.print(" bit ");
  Serial.print(bit);
  Serial.print(" (");
  Serial.print(name);
  Serial.println(") raised");
}
#endif
