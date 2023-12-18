#ifndef LOWSIDE_CS_LIB_H
#define LOWSIDE_CS_LIB_H

#include "Arduino.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/defaults.h"
#include "../common/base_classes/CurrentSense.h"
#include "../common/base_classes/FOCMotor.h"
#include "../common/lowpass_filter.h"
#include "hardware_api.h"


class LowsideCurrentSense: public CurrentSense{
  public:
    /**
      LowsideCurrentSense class constructor
      @param shunt_resistor shunt resistor value
      @param gain current-sense op-amp gain
      @param phA A phase adc pin
      @param phB B phase adc pin
      @param phC C phase adc pin (optional)
    */
    LowsideCurrentSense(float shunt_resistor, float gain, int pinA, int pinB, int pinC = _NC);

    // CurrentSense interface implementing functions
    int init() override;
    PhaseCurrent_s getPhaseCurrents() override;
    int driverAlign(float align_voltage) override;

    // ADC measuremnet gain for each phase
    // support for different gains for different phases of more commonly - inverted phase currents
    // this should be automated later
    float gain_a; //!< phase A gain
    float gain_b; //!< phase B gain
    float gain_c; //!< phase C gain

    // // per phase low pass fileters
    // LowPassFilter lpf_a{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf}; //!<  current A low pass filter
    // LowPassFilter lpf_b{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf}; //!<  current B low pass filter
    // LowPassFilter lpf_c{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf}; //!<  current C low pass filter

    float offset_ia; //!< zero current A voltage value (center of the adc reading)
    float offset_ib; //!< zero current B voltage value (center of the adc reading)
    float offset_ic; //!< zero current C voltage value (center of the adc reading)
  private:

    // hardware variables
  	int pinA; //!< pin A analog pin for current measurement
  	int pinB; //!< pin B analog pin for current measurement
  	int pinC; //!< pin C analog pin for current measurement

    // gain variables
    float shunt_resistor; //!< Shunt resistor value
    float amp_gain; //!< amp gain value
    float volts_to_amps_ratio; //!< Volts to amps ratio

    /**
     *  Function finding zero offsets of the ADC
     */
    void calibrateOffsets();

};

#endif
