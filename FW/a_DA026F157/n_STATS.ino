//###########################################
//   CONTROL STATISTICS
//###########################################

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
