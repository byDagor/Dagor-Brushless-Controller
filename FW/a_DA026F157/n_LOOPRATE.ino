//###########################################
//   LOOP RATE
//###########################################

#include <esp_system.h>

// Records a command for the L command: its letter and the first five characters of its argument
void noteCommand(char id, const char* argument){
  commandCount++;
  lastCommand[0] = id;
  int n = 1;
  for (int i = 0; argument && argument[i] && i < 5 && argument[i] != 13 && argument[i] != 10; i++) lastCommand[n++] = argument[i];
  lastCommand[n] = 0;
  lastCommandAtS = (uint32_t)(esp_timer_get_time() / 1000000);
}

// L: loop rate since the previous L, the uptime and the reset reason of the last start (1 power on, 3 software, 4 panic, 5 to 7 watchdog, 9 brownout), counted by the loop itself, so it measures the image that is running, plus the state and the
// hold values, so a quick look shows that the controller is healthy.
void printLoopRate(char* cmd){
  static uint32_t lastCount = 0;
  static int64_t lastUs = 0;
  uint32_t count = loopIterations;
  int64_t nowUs = esp_timer_get_time();
  Serial.print("LOOP: ");
  if (lastUs == 0){
    Serial.print("first call, call again to measure");
  } else {
    Serial.print((count - lastCount) * 1000.0f / (nowUs - lastUs), 3);
    Serial.print(" kHz since the previous call");
  }
  Serial.print(" | late ");
  Serial.print(loopLateStarts);
  Serial.print(" lost ");
  Serial.print(loopLockLosses);
  Serial.print(" timeouts ");
  Serial.print(loopTimeouts);
  Serial.print(" | up ");
  Serial.print((uint32_t)(nowUs / 1000000));
  Serial.print(" s reset ");
  Serial.print((int)esp_reset_reason());
  Serial.print(" | cmds ");
  Serial.print(commandCount);
  Serial.print(" last ");
  Serial.print(lastCommand);
  Serial.print(" at ");
  Serial.print(lastCommandAtS);
  Serial.print(" s | offset ");
  Serial.print(motor.sensor_offset, 3);
  Serial.print(" en ");
  Serial.print((int)motor.enabled);
  Serial.print(" boot target ");
  Serial.print(bootTarget, 3);
  Serial.print(" | state ");
  Serial.print((int)state_machine);
  Serial.print(" angle ");
  Serial.print(motor.shaft_angle, 3);
  Serial.print(" target ");
  Serial.print(motor.target, 3);
  Serial.print(" iq ");
  Serial.print(motor.current.q, 3);
  Serial.print(" uq ");
  Serial.println(motor.voltage.q, 3);
  lastCount = count;
  lastUs = nowUs;
}
