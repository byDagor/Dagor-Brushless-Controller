//###########################################
//       LOOP FUNCTIONS DEFINITIONS
//###########################################

// (byte in which to find the 1s, position querying for a 1, binary result of query)
#define FIND_BIT(X, Y, Z)\        
  Z = (X>>Y) & 1;

#define BYTE_CONCATENATE(X, Y, Z)\
  Z = ((X<<8)+Y);


// Time management for fixed rate functions
unsigned long timeManagement(){
  static unsigned long prevT = 0;
  static unsigned long timeDif = 0;
  static unsigned long runTime = 0;

  runTime = micros();
  timeDif = runTime - prevT;
  prevT = runTime;
  //stateT += timeDif;

  return timeDif;
}

// Function to find the loop time, reports in microseconds. 
void loopPeriod(bool reset){
  static unsigned int loopCount = 1;

  if (reset){
    float lt = float(stateT)/float(loopCount);
    loopCount = 0;
    //Serial.println("Loop Time: );
    //Serial.print(", ");
    Serial.println(lt);
  }
  else{
    loopCount += 1;
  }
}

// Gravity compensation mode
void gravityComp(){

  #ifdef CURRENT_SENSE
    static float holdPosition = 0;         //moving position depending on external forces applied to the actuator's output
    //This inside the void loop
    if (gravityCompMode){

      float ea = motor.electricalAngle();
      DQCurrent_s current = current_sense.getFOCCurrents(ea);
    
      if (current.q > currentQthreshold && abs(sensor.getAngle() - holdPosition) > positionThreshold){
          holdPosition -= kgc * (current.q - currentQthreshold);
      }
      else if (current.q < -currentQthreshold && abs(sensor.getAngle() - holdPosition) > positionThreshold){
          holdPosition -= kgc * (current.q + currentQthreshold);
      }
      //Serial.println(current.q);
      //Serial.print(", ");
      //Serial.println}

      (holdPosition);
      motor.move(holdPosition);
      
    }
    else{
      motor.move();
    }
  #endif
}


// Temperature status and manager
void tempStatus(unsigned long timeDif, bool debug){
  static int tFlag;
  static unsigned int totalTempTime;

  //Read voltage from temperature sensor and transform it to °C
  float vOut = analogRead(vTemp);
  float temp = (((vOut*3.3)/4095)-1.8577)/-0.01177;
  
  if (debug == true) {
    Serial.print("Temp: ");
    Serial.println(temp,2);
  }
  
  if (temp >= maxTemp && tFlag == false){
    //unsigned long tempTime = micros();
    totalTempTime += timeDif;

    //If temperature is high for [overTempTime] seconds disable controller
    if(totalTempTime >= overTempTime*1000000){
      tFlag = true;
      digitalWrite(enGate, LOW);
      Serial.print("enGate Disabled - Temperature protection: ");
      Serial.println(temp);
    }
    
  }
  else if (temp <= maxTemp && tFlag == false){
    totalTempTime = 0;
  }
  
}

// Monitor the voltage of the power source (useful for battery powered devices).
void voltageMonitor(bool debug){
  
  float rawVoltage = analogRead(vMonitor);
  float bVoltage = rawVoltage * 36.3/4095;

  if (debug == true){
    Serial.print("Volate: ");
    Serial.println(bVoltage,2);
  }
}

// Print the rotor position in radians or rotor velocity in radians over second
void rotorData(bool rotorVelocity){
  if (rotorVelocity){
    float e = sensor.getAngle();
    Serial.print("Rads: ");
    Serial.print(e,4);  
    Serial.print("\t");  
  }
  else{
    //float e = sensor.getVelocity();
    float e = sensor.getMechanicalAngle();
    //Serial.print("Vel: ");
    Serial.print(e,4); 
    Serial.print("\t");
  }

}



// Print each phase current or the current in the DQ space
void printCurrents(bool DQspace){

  #ifdef CURRENT_SENSE
    if(DQspace){
      //float current_magnitude = current_sense.getDCCurrent();
      //Serial.println(current_magnitude); // milli Amps

      //DQCurrent_s current = current_sense.getFOCCurrents(motor.electrical_angle);
      //DQCurrent_s current = current_sense.getFOCCurrents( _electricalAngle(motor.shaft_angle, motor.pole_pairs) );
      Serial.print(motor.current.d);
      Serial.print("\t");
      Serial.print(motor.current.q);
      Serial.print("\t");    
    }
    else{
      PhaseCurrent_s currents = current_sense.getPhaseCurrents();
      Serial.print(currents.a); // [Amps]
      Serial.print("\t");
      Serial.print(currents.b); // [Amps]
      Serial.print("\t");
      Serial.print(currents.c); // [Amps]
      Serial.print("\t");
    }
  #else
    Serial.print("Current sense not initialized.");
  #endif
}

// Print each phase voltage or the equivalent in the DQ space
void phaseVoltages(bool DQspace){
  if (DQspace){
    //DQVoltage_s voltage;
    Serial.print(motor.voltage.d); // milli Amps
    Serial.print("\t");
    Serial.print(motor.voltage.q); // milli Amps
    Serial.print("\t");
  }
  else{
    Serial.print(motor.Ua); // [volts]
    Serial.print("\t");
    Serial.print(motor.Ub); // [volts]
    Serial.print("\t");
    Serial.print(motor.Uc); // [volts]
    Serial.print("\t");
  }
}
