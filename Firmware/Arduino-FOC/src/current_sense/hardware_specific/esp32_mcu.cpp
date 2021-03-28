#include "../hardware_api.h"

#if defined(ESP_H)

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "../../common/lowpass_filter.h"

#define _ADC_VOLTAGE 3.3
#define _ADC_RESOLUTION 4095.0

LowPassFilter LPF_current_a{0.002};
LowPassFilter LPF_current_b{0.002};
LowPassFilter LPF_current_c{0.002};

static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1};
int a1, a2, a3;         //Current readings from internal current sensor amplifiers
int _pinA, _pinB, _pinC;
static void IRAM_ATTR isr_handler(void*);
byte currentState = 1;

// adc counts to voltage conversion ratio
// some optimizing for faster execution
#define _ADC_CONV ( (_ADC_VOLTAGE) / (_ADC_RESOLUTION) )

// function reading an ADC value and returning the read voltage
float _readADCVoltage(const int pin){
  uint32_t raw_adc;
  /*
  if (pin == _pinA) raw_adc = LPF_current_a(a1);
  else if (pin == _pinB) raw_adc = LPF_current_b(a2);
  else if (pin == _pinC) raw_adc = LPF_current_c(a3);
  */

  if (pin == _pinA) raw_adc = a1;
  else if (pin == _pinB) raw_adc = a2;
  else if (pin == _pinC) raw_adc = a3;

  return raw_adc * _ADC_CONV;
}

/*
// function reading an ADC value and returning the read voltage
float _readADCVoltage(const int pinA){
  uint32_t raw_adc
  if (pinA == _pinA) raw_adc = analogRead(pinA);
  return raw_adc * _ADC_CONV;
}
*/

// function reading an ADC value and returning the read voltage
void _configureADC(const int pinA,const int pinB,const int pinC){
  _pinA = pinA;
  _pinB = pinB;
  if( _isset(pinC) ) _pinC = pinC;
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  if( _isset(pinC) ) pinMode(pinC, INPUT);

}

void _driverSync(){
  MCPWM[MCPWM_UNIT_0]->int_ena.timer0_tep_int_ena = true;//A PWM timer 0 TEP event will trigger this interrupt
  MCPWM[MCPWM_UNIT_0]->int_ena.timer1_tep_int_ena = true;//A PWM timer 1 TEP event will trigger this interrupt
  //MCPWM[MCPWM_UNIT_0]->int_ena.timer2_tep_int_ena = true;//A PWM timer 2 TEP event will trigger this interrupt
  mcpwm_isr_register(MCPWM_UNIT_0, isr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);  //Set ISR Handler
}

/*
// function reading an ADC value and returning the read voltage
void _configureADC(const int pinA,const int pinB,const int pinC){
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  if( _isset(pinC) ) pinMode(pinC, INPUT);

  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328PB__) || defined(__AVR_ATmega2560__) //  if mcu is atmega328 or atmega2560
    // set hight frequency adc - ADPS2,ADPS1,ADPS0 | 001 (16mhz/2) | 010 ( 16mhz/4 ) | 011 (16mhz/8) | 100(16mhz/16) | 101 (16mhz/32) | 110 (16mhz/64) | 111 (16mhz/128 default)
    // set divisor to 8 - adc frequency 16mhz/8 = 2 mhz
    // arduino takes 25 conversions per sample so - 2mhz/25 = 80k samples per second - 12.5us per sample
    cbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
  #endif
}
*/

// Read currents when interrupt is triggered
static void IRAM_ATTR isr_handler(void*){
  uint32_t mcpwm_intr_status_0 = MCPWM[MCPWM_UNIT_0]->int_st.timer0_tep_int_st;
  uint32_t mcpwm_intr_status_1 = MCPWM[MCPWM_UNIT_0]->int_st.timer1_tep_int_st;
  //uint32_t mcpwm_intr_status_2 = MCPWM[MCPWM_UNIT_0]->int_st.timer2_tep_int_st;

  if(mcpwm_intr_status_0 > 0 && currentState == 1){
    a1 = analogRead(_pinA);
    //a2 = analogRead(_pinB);
    currentState = 2;
  }
  else if(mcpwm_intr_status_1 > 0 && currentState == 2){
    a2 = analogRead(_pinB);
    //a3 = analogRead(_pinC);
    currentState = 1;
  }
  /*
  else if(mcpwm_intr_status_2 > 0 && currentState == 3){
    a3 = analogRead(_pinC);
    //a1 = analogRead(_pinA);
    currentState = 1;
  }*/

  MCPWM[MCPWM_UNIT_0]->int_clr.timer0_tep_int_clr = mcpwm_intr_status_0;
  MCPWM[MCPWM_UNIT_0]->int_clr.timer1_tep_int_clr = mcpwm_intr_status_1;
  //MCPWM[MCPWM_UNIT_0]->int_clr.timer2_tep_int_clr = mcpwm_intr_status_2;
}

#endif
