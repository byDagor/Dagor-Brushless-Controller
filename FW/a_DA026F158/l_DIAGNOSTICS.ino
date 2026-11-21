//###########################################
//   DIAGNOSTICS: loop rate (L) and control statistics (Q)
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

// ---- Control statistics (Q) ----
// Q<seconds>[,<step>]: statistics of the control loop, accumulated by the loop itself and printed as one line when Q is sent again after
// the time has passed. Nothing is stored, nothing else is printed, so the measurement does not disturb the control loop.
//   Q2        quiet hold: nothing moves. Mean and standard deviation of angle, velocity, iq and uq. The standard deviations are the
//             regression numbers, they must not grow between builds.
//   Q6,3.1416 position step of pi rad after 0.3 s (at most statsMaxStep): rise time 10 to 90 %, overshoot, settling time (5 % band),
//             steady state error, peak iq. Only with someone at the bench, the motor moves, and it moves back after the measurement.
const float statsMaxStep = 3.2;                 // Largest step the command accepts [rad], a half turn is the usual test
const int64_t statsStepAtUs = 300000;           // The step is applied this long after the start [us]
const int64_t statsBaselineUs = 250000;         // Angle averaged over this long before the step [us]
const int64_t statsFinalUs = 250000;            // Steady state averaged over this long at the end [us]

static float statsA0 = 0, statsSumA = 0, statsSumA2 = 0, statsSumV = 0, statsSumV2 = 0, statsSumI = 0, statsSumI2 = 0, statsSumU = 0, statsSumU2 = 0;
static uint32_t statsCount = 0;
static int64_t statsStartUs = 0, statsEndUs = 0;
static volatile bool statsDone = false;
static float statsStep = 0, statsBase = 0;
static float stepA0 = 0, stepBaseSum = 0, stepFinalSum = 0, stepMaxProgress = 0, stepIqPeak = 0, stepT10 = -1, stepT90 = -1, stepSettle = 0;
static uint32_t stepBaseN = 0, stepFinalN = 0;
static bool stepApplied = false;
static int64_t stepAppliedUs = 0;

// Called by the loop on every iteration while the statistics run. Sums are of deviations from the first angle, to keep the float precise.
void statsAccumulate(){
  float angle = motor.shaft_angle, v = motor.shaft_velocity, i = motor.current.q, u = motor.voltage.q;
  int64_t now = esp_timer_get_time();
  if (statsCount == 0) statsA0 = angle;
  float a = angle - statsA0;
  statsSumA += a; statsSumA2 += a * a;
  statsSumV += v; statsSumV2 += v * v;
  statsSumI += i; statsSumI2 += i * i;
  statsSumU += u; statsSumU2 += u * u;
  statsCount++;
  if (statsStep != 0){
    int64_t t = now - statsStartUs;
    if (t < statsBaselineUs){ stepBaseSum += angle; stepBaseN++; }
    else if (!stepApplied && t >= statsStepAtUs){
      stepA0 = stepBaseSum / stepBaseN;
      motor.target = statsBase + statsStep;
      stepApplied = true;
      stepAppliedUs = t;
    }
    if (stepApplied){
      float progress = (angle - stepA0) / statsStep;
      float ms = (t - stepAppliedUs) / 1000.0f;
      if (progress > stepMaxProgress) stepMaxProgress = progress;
      if (stepT10 < 0 && progress >= 0.1f) stepT10 = ms;
      if (stepT90 < 0 && progress >= 0.9f) stepT90 = ms;
      if (fabsf(progress - 1.0f) > 0.05f) stepSettle = ms;
      if (fabsf(i) > stepIqPeak) stepIqPeak = fabsf(i);
      if (now >= statsEndUs - statsFinalUs){ stepFinalSum += angle; stepFinalN++; }
    }
  }
  if (now >= statsEndUs){
    if (statsStep != 0) motor.target = statsBase;
    statsActive = false;
    statsDone = true;
  }
}

static void printStat(const char* name, float sum, float sum2, uint32_t n){
  float mean = sum / n;
  float variance = sum2 / n - mean * mean;
  Serial.print(" ");
  Serial.print(name);
  Serial.print(" ");
  Serial.print(mean, 4);
  Serial.print("/");
  Serial.print(sqrtf(variance > 0 ? variance : 0), 4);
}

void onStatsCommand(char* cmd){
  noteCommand('Q', cmd);
  bool hasArgument = cmd[0] != 0 && cmd[0] != '\r' && cmd[0] != '\n' && cmd[0] != ' ';
  if (!hasArgument){
    // A bare Q reads the result of the last measurement
    if (statsActive){ Serial.println("QSTAT: running"); return; }
    if (!statsDone){ Serial.println("QSTAT: no result, start one with Q<seconds>"); return; }
    statsDone = false;
    float seconds = (statsEndUs - statsStartUs) / 1e6f;
    Serial.print("QSTAT: n ");
    Serial.print(statsCount);
    Serial.print(" rate ");
    Serial.print(statsCount / seconds / 1000.0f, 3);
    Serial.print(" kHz | mean/std:");
    printStat("angle(rel)", statsSumA, statsSumA2, statsCount);
    printStat("vel", statsSumV, statsSumV2, statsCount);
    printStat("iq", statsSumI, statsSumI2, statsCount);
    printStat("uq", statsSumU, statsSumU2, statsCount);
    if (statsStep != 0 && stepFinalN > 0){
      Serial.print(" | step ");
      Serial.print(statsStep, 3);
      Serial.print(" rise10-90 ");
      Serial.print(stepT10 >= 0 && stepT90 >= 0 ? stepT90 - stepT10 : -1.0f, 1);
      Serial.print(" ms overshoot ");
      Serial.print(max(0.0f, (stepMaxProgress - 1.0f) * 100.0f), 1);
      Serial.print(" % settle5% ");
      Serial.print(stepSettle, 1);
      Serial.print(" ms ss_error ");
      Serial.print(statsBase + statsStep - stepFinalSum / stepFinalN, 4);
      Serial.print(" rad iq_peak ");
      Serial.print(stepIqPeak, 3);
    }
    Serial.println();
    return;
  }
  // Q<seconds>[,<step>] starts a new measurement and drops an unread result
  if (statsActive){ Serial.println("QSTAT: busy"); return; }
  statsDone = false;
  if (state_machine != LIFE_IS_GOOD || !motor.enabled){ Serial.println("QSTAT: needs a healthy enabled motor"); return; }
  float seconds = atof(cmd);
  float step = 0;
  const char* comma = strchr(cmd, ',');
  if (comma) step = atof(comma + 1);
  if (seconds <= 0) seconds = 2;
  if (seconds > 10) seconds = 10;
  if (step != 0){
    if (!isAngleControl()){ Serial.println("QSTAT: a step needs angle control"); return; }
    if (fabsf(step) > statsMaxStep){ Serial.print("QSTAT: the step must not exceed "); Serial.print(statsMaxStep, 1); Serial.println(" rad"); return; }
    if (seconds < 1.0f) seconds = 1.0f;
  }
  statsSumA = statsSumA2 = statsSumV = statsSumV2 = statsSumI = statsSumI2 = statsSumU = statsSumU2 = 0;
  statsCount = 0;
  statsStep = step;
  statsBase = motor.target;
  stepBaseSum = stepFinalSum = stepMaxProgress = stepIqPeak = stepSettle = 0;
  stepT10 = stepT90 = -1;
  stepBaseN = stepFinalN = 0;
  stepApplied = false;
  statsStartUs = esp_timer_get_time();
  statsEndUs = statsStartUs + (int64_t)(seconds * 1e6f);
  statsActive = true;
  Serial.println("QSTAT: running, send Q again after the time has passed");
}


// ---- Other diagnosis helpers: loop frequency print, stack, current outliers, profiling ----

// Function to find the loop time, reports in microseconds. 
void printFOCfreq(){

  static float monitor_freq = 1; //[Hz]

  static unsigned long prevT = 0;
  static unsigned long timeDif = 0;
  static unsigned long runTime = 0;
  static unsigned long stateT = 0;

  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  stateT += timeDif;

  static unsigned long loopCount = 1;

  if (stateT >= 1000000/monitor_freq){
    
    float lt = float(stateT)/float(loopCount);    
    float freq = 1/(lt/1000);

    Serial.print(freq);    //[kHz]
    Serial.println("");

    #ifdef PROFILE_FOC
      static int reportCount = 0;
      if (++reportCount % profile_report_period == 0) profilePrintReport();
    #endif

    loopCount = 0;
    stateT = 0;
    
  }
  else{
    loopCount += 1;
  }
}

void printStackLeft(){
  unsigned int stack = uxTaskGetStackHighWaterMark(nullptr);
    Serial.println(stack);
}

#if defined(CURRENT_OUTLIER_TEST) && defined(CURRENT_SENSE)

// A bad ADC sample shows up as a phase current far from its running average.
void currentOutlierMonitor(){
  static float mean_a = 0, mean_b = 0, max_deviation = 0, sum_squares = 0;
  static unsigned long samples = 0, outliers = 0, windowStart = 0;
  static bool started = false;

  PhaseCurrent_s current = current_sense.getPhaseCurrents();

  if (!started){
    mean_a = current.a;
    mean_b = current.b;
    windowStart = micros();
    started = true;
  }

  float deviation_a = fabs(current.a - mean_a);
  float deviation_b = fabs(current.b - mean_b);
  float deviation = deviation_a > deviation_b ? deviation_a : deviation_b;

  samples++;
  if (deviation > outlier_threshold) outliers++;
  if (deviation > max_deviation) max_deviation = deviation;
  sum_squares += deviation_a * deviation_a + deviation_b * deviation_b;

  mean_a += outlier_average_filter * (current.a - mean_a);
  mean_b += outlier_average_filter * (current.b - mean_b);

  if (micros() - windowStart >= 1000000){
    Serial.print("OUTLIERS: cycles ");
    Serial.print(samples);
    Serial.print(", outliers ");
    Serial.print(outliers);
    Serial.print(", largest deviation [A] ");
    Serial.print(max_deviation);
    Serial.print(", rms deviation [A] ");
    Serial.println(sqrt(sum_squares / (2.0f * samples)), 4);
    samples = 0;
    outliers = 0;
    max_deviation = 0;
    sum_squares = 0;
    windowStart = micros();
  }
}

#endif

#ifdef PROFILE_FOC

void profileSensor(){ sensor.update(); }
void profileLoopFOC(){ motor.loopFOC(); }
void profileMove(){ motor.move(); }
void profileCommand(){ command.run(); }

#ifdef CURRENT_SENSE
void profileCurrentSense(){ current_sense.getPhaseCurrents(); }
#endif

// Fixed CPU work, it takes longer the more time interrupts steal from the core.
void profileBusyWork(){ for (volatile int i = 0; i < profile_busy_iterations; i++){} }

float averageMicros(void (*section)()){
  unsigned long start = micros();
  for (int i = 0; i < profile_runs; i++) section();
  return float(micros() - start) / profile_runs;
}

void profileCpuInfo(){
  Serial.print("PROFILE: CPU MHz ");
  Serial.print(getCpuFrequencyMhz());
  Serial.print(", APB Hz ");
  Serial.println(getApbFrequency());
}

const char* profileNames[profile_max_entries];
float profileUs[profile_max_entries];
int profileCount = 0;

void profileStore(const char* name, float us){
  if (profileCount >= profile_max_entries) return;
  profileNames[profileCount] = name;
  profileUs[profileCount++] = us;
}

void profileBusyReport(const char* stage){
  profileStore(stage, averageMicros(profileBusyWork));
}

void profilePrintReport(){
  profileCpuInfo();
  for (int i = 0; i < profileCount; i++){
    Serial.print("PROFILE: ");
    Serial.print(profileNames[i]);
    Serial.print(" [us]: ");
    Serial.println(profileUs[i]);
  }
}

void profileFOC(){
  profileStore("sensor.update", averageMicros(profileSensor));
  #ifdef CURRENT_SENSE
    profileStore("getPhaseCurrents", averageMicros(profileCurrentSense));
  #endif
  profileStore("motor.loopFOC", averageMicros(profileLoopFOC));
  profileStore("motor.move", averageMicros(profileMove));
  profileStore("command.run", averageMicros(profileCommand));
  profileStore("busy work after full init", averageMicros(profileBusyWork));
  profilePrintReport();
}

#endif
