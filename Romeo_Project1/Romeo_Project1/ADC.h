#pragma once

#ifndef _ADC_H_
#define _ADC_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"
#include "RTOS.h"

#define _ADC_DEBUG_	0

#define AREF_OFF	0
#define AREF_VCC	1
#define AREF_1_1V	3										// NOT CURRENTLY SUPPORTED

#define DISABLE_ADC_OPTION	0
#define ENABLE_ADC_OPTION	1

#define ADC_RIGHT_JUSTIFY	0
#define ADC_LEFT_JUSTIFY	1

// ADC PRESCALER (50kHz < ADC CLOCK < 250kHz)
#define ADC_PRSC_2		1									// ZERO IS ALSO A PRESCALER OF TWO
#define ADC_PRSC_4		2
#define ADC_PRSC_8		3
#define ADC_PRSC_16		4
#define ADC_PRSC_32		5
#define ADC_PRSC_64		6
#define ADC_PRSC_128	7

// ADC TRIGGER SELECTION
#define ADC_TRIG_FREE	0
#define ADC_TRIG_COMP	1
#define ADC_TRIG_EXI0	2
#define ADC_TRIG_T0CA	3
#define ADC_TRIG_T0OVF	4
#define ADC_TRIG_T1CB	5
#define ADC_TRIG_T1OVF	6
#define ADC_TRIG_T1CE	7

// ADMUX REGISTER
#define ADC_AREF_SEL_DEF		AREF_VCC					// AREF_OFF or AREF_VCC or AREF_1_1V
#define ADC_JUSTIFICATION_DEF	ADC_RIGHT_JUSTIFY			// ADC_LEFT_JUSTIFY or ADC_RIGHT_JUSTIFY

// ADCSRA REGISTER
#define ADC_ENABLE_DEF			ENABLE_ADC_OPTION			// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_START_DEF			ENABLE_ADC_OPTION			// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_TRIGGER_ON_DEF		DISABLE_ADC_OPTION			// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_INTERRUPT_ON_DEF	DISABLE_ADC_OPTION			// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_PRESCALER_DEF		ADC_PRSC_128				// ADC_PRSC_2/4/8/16/32/64/128

// ADCSRB REGISTER
#define ADC_TRIGGER_SRC_DEF		ADC_TRIG_FREE				// ADC_TRIG_FREE OR ADC_TRIG_COMP OR ADC_TRIG_EXI0 OR ADC_TRIG_T0CA OR ADC_TRIG_T0OVF OR ADC_TRIG_T1CB OR ADC_TRIG_T1OVF OR ADC_TRIG_T1CE					

// DIDR0 REGISTER (DISABLE DIGITAL IO SHARED WITH ANALOG INPUTS)
#define ADC_DIO_DISABLE_0	DISABLE_ADC_OPTION				// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_DIO_DISABLE_1	DISABLE_ADC_OPTION				// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_DIO_DISABLE_2	DISABLE_ADC_OPTION				// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_DIO_DISABLE_3	DISABLE_ADC_OPTION				// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_DIO_DISABLE_4	DISABLE_ADC_OPTION				// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION
#define ADC_DIO_DISABLE_5	DISABLE_ADC_OPTION				// DISABLE_ADC_OPTION or ENABLE_ADC_OPTION

// USED TO CLR CHANNEL SELECTION BITS IN ADMUX
#define ADMUX_CLR (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0)

#define ADC_NUM_SAMPLES 8									// NUMBER OF CONSECUTIVE VALUES STORED, USEFUL FOR CALC MOVING AVG (INDEPENDANT OF HOW MANY VALUES USED BY OTHER COMPILATION UNITS)
#define ADC_NUM_CHANNELS 8									// NUMBER OF ADC CHANNELS (8 FOR THE ATMEGA328P)

// STORES REFS TO ALL REGISTERS PERTAINING TO THE ADC HARDWARE
typedef struct ADC_registers_struct {

	volatile uint8_t * const r_mux;
	volatile uint8_t * const r_csa;
	volatile uint8_t * const r_csb;
	volatile uint8_t * const r_did;
	volatile uint16_t * const r_adc;
	volatile uint8_t * const r_adch;
	volatile uint8_t * const r_adcl;

}ADC_registers_obj;

// STORES CONECTUIVE ADC RESULTS
typedef struct ADC_results_struct {

	volatile uint8_t index : 4;										// POINTS TO THE NEXT ARRAY ELEMENT TO BE UPDATED WITH ADC RESULT (4 BITS TO CHECK FOR OVF IN LOOPS)
	volatile uint8_t channel : 4;									// SPECIFIES THE CHANNEL TO BE SAMPLED (4 BITS TO CHECK FOR OVF IN LOOPS)
	volatile uint8_t bit_mask;										// USED TO MASK WHICH CHANNELS SHOULD BE READ AND WHICH SHOULD BE IGNORED WHEN SAMPLING MULTIPLE CHANNELS (TRUE => TAKE SAMPLE)
	volatile uint16_t values[ADC_NUM_CHANNELS][ADC_NUM_SAMPLES];	// STORES CONSECUTIVE ADC RESULTS FOR EACH ADC CHANNEL (USED BY OTHER COMPILATION UNITS)

}ADC_results_obj;

// STORES THE ADC INI SETTINGS
typedef struct ADC_struct {
	
	// ADC register pointers
	ADC_registers_obj *registers;

	// ADMUX register members
	uint8_t aref_sel : 2;
	uint8_t justify : 1;

	// ADCSRA register members
	uint8_t adc_en : 1;
	uint8_t start : 1;
	uint8_t trig_en : 1;
	uint8_t intrpt_en : 1;
	uint8_t presclr : 3;

	// ADCRSB register members
	uint8_t trig_src : 3;

	// DIDR0 register members
	uint8_t ddio_0 : 1;
	uint8_t ddio_1 : 1;
	uint8_t ddio_2 : 1;
	uint8_t ddio_3 : 1;
	uint8_t ddio_4 : 1;
	uint8_t ddio_5 : 1;

}ADC_obj;

extern ADC_obj *NEW_ADC(void);
extern int8_t parse_ini_args_ADC(char *);
extern void initialize_ADC(ADC_obj *, ...);
extern void ini_default_ADC(ADC_obj *);
extern void ADC_destructor(ADC_obj *);

//static inline void start_ADC(uint8_t);

extern void reset_poll_ADC(void);
void clear_poll_ADC(void);

extern void poll_ADC_channel(uint8_t);
extern void poll_ADC(uint8_t);

extern void poll_ADC_channel_now(uint8_t);
extern void poll_ADC_now(uint8_t);

extern uint16_t get_raw_ADC(uint8_t);
extern uint16_t get_raw_ADC_now(uint8_t);

extern uint16_t get_mov_avg_ADC(uint8_t, uint8_t);
extern uint16_t get_clamped_value_ADC(uint8_t, uint16_t, uint16_t, uint8_t);
extern uint16_t get_scaled_value_ADC(uint8_t, uint16_t, uint16_t, uint8_t);

extern uint8_t get_raw_ADC_uint8_now(uint8_t);

#endif