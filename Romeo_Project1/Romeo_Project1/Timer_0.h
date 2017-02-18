#pragma once

#ifndef _TIMER0_H_
#define _TIMER0_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util\atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"

#define _TIMER0_DEBUG_	0

#define TIMER0_OCA_CHANNEL _PD6
#define TIMER0_OCB_CHANNEL _PD5

#define TIMER0_DISABLE_OPTION	0
#define TIMER0_ENABLE_OPTION	1

#define TIMER0_NORMAL_OUTPUT	0
#define TIMER0_TOGGLE_OUTPUT	1								// NORMAL OPERATION IF WGM02 = 0
#define TIMER0_CLEAR_OUTPUT		2
#define TIMER0_SET_OUTPUT		3

#define TIMER0_NORMAL_WAVE		0
#define TIMER0_PWM_WAVE			1
#define TIMER0_CTC_WAVE			2
#define TIMER0_FAST_PWM_WAVE	3

#define TIMER0_NO_CLK		0
#define TIMER0_CLK_DIV_1	1
#define TIMER0_CLK_DIV_8	2
#define TIMER0_CLK_DIV_64	3
#define TIMER0_CLK_DIV_256	4
#define TIMER0_CLK_DIV_1024	5
#define TIMER0_XCLK_FALLING	6
#define TIMER0_XCLK_RISING	7

//DEFAULT SETTINGS
#define TIMER0_OUTPUT_A_MODE_DEF	TIMER0_NORMAL_OUTPUT		// TIMER0_NORMAL_OUTPUT or TIMER0_TOGGLE_OUTPUT or TIMER0_CLEAR_OUTPUT or TIMER0_SET_OUTPUT
#define TIMER0_OUTPUT_B_MODE_DEF	TIMER0_NORMAL_OUTPUT		// TIMER0_NORMAL_OUTPUT or TIMER0_TOGGLE_OUTPUT or TIMER0_CLEAR_OUTPUT or TIMER0_SET_OUTPUT

#define TIMER0_WAVEFORM_MODE_DEF	TIMER0_CTC_WAVE				// TIMER0_NORMAL_WAVE or TIMER0_PWM_WAVE or TIMER0_CTC_WAVE or TIMER0_FAST_PWM_WAVE

#define TIMER0_FORCE_OUT_A_DEF		TIMER0_DISABLE_OPTION		// TIMER0_DISABLE_OPTION or TIMER0_ENABLE_OPTION	(NO AFFECT IN PWM MODE)
#define TIMER0_FORCE_OUT_B_DEF		TIMER0_DISABLE_OPTION		// TIMER0_DISABLE_OPTION or TIMER0_ENABLE_OPTION	(NO AFFECT IN PWM MODE)

#define TIMER0_TOP_OCRA_DEF			TIMER0_DISABLE_OPTION		// TIMER0_DISABLE_OPTION or TIMER0_ENABLE_OPTION	(WGM02 BIT, NO AFFECT IN NORMAL MODE)

#define TIMER0_CLK_DIV_DEF			TIMER0_CLK_DIV_64			// TIMER0_NO_CLK or TIMER0_CLK_DIV_1 or TIMER0_CLK_DIV_8 or TIMER0_CLK_DIV_64 or TIMER0_CLK_DIV_256 or TIMER0_CLK_DIV_1024 or TIMER0_XCLK_FALLING or TIMER0_XCLK_RISING

#define TIMER0_COMP_B_IE_DEF		TIMER0_DISABLE_OPTION		// TIMER0_DISABLE_OPTION or TIMER0_ENABLE_OPTION
#define TIMER0_COMP_A_IE_DEF		TIMER0_DISABLE_OPTION		// TIMER0_DISABLE_OPTION or TIMER0_ENABLE_OPTION
#define TIMER0_OVF_IE_DEF			TIMER0_DISABLE_OPTION		// TIMER0_DISABLE_OPTION or TIMER0_ENABLE_OPTION

#define TIMER0_OCRA_DEF				(F_CPU / 64 / 1000) - 1			// = 249, for period of 1 ms

typedef struct TIMER0_registers_struct {

	volatile uint8_t * const r_cnt;
	volatile uint8_t * const r_cra;
	volatile uint8_t * const r_crb;
	volatile uint8_t * const r_compa;
	volatile uint8_t * const r_compb;
	volatile uint8_t * const r_msk;
	volatile uint8_t * const r_ifr;

}TIMER0_registers_obj;

typedef struct TIMER0_struct {

	// TIMER0 register pointers
	TIMER0_registers_obj *registers;

	// TCCR0A register members
	uint8_t compare_out_A : 2;
	uint8_t compare_out_B : 2;
	uint8_t waveform_mode : 2;

	// TCCR0B register members
	uint8_t force_output_A : 1;
	uint8_t force_output_B : 1;
	uint8_t waveform_top_ocrA : 1;				// WGM02
	uint8_t clock_sel : 3;

	// TIMSK0 register members
	uint8_t int_en_compB : 1;
	uint8_t int_en_compA : 1;
	uint8_t int_en_ovf : 1;

	// TIFR0 register members
	uint8_t int_flag_compB : 1;
	uint8_t int_flag_compA : 1;
	uint8_t int_flag_ovf : 1;

	uint8_t ocra_en : 1;
	uint8_t ocrb_en : 1;

	volatile uint16_t tick;
	volatile uint8_t top;

}TIMER0_obj;

extern TIMER0_obj *NEW_TIMER0(void);
extern int8_t parse_ini_args_TIMER0(char *);
extern void initialize_TIMER0(TIMER0_obj *, ...);
extern void initialize_TIMER0_def(TIMER0_obj *);
extern void destructor_TIMER0(TIMER0_obj *);

extern void update_duty_A_TIMER0(TIMER0_obj *, uint16_t);
extern void update_duty_B_TIMER0(TIMER0_obj *, uint16_t);
extern void update_period_TIMER0(TIMER0_obj *, uint16_t);
extern void update_period_and_duty_TIMER0(TIMER0_obj *, uint16_t, uint16_t);

extern void disable_output_TIMER0(void);

extern uint16_t select_divisor_ms_TIMER0(uint16_t);
extern uint16_t select_divisor_us_TIMER0(uint16_t);
extern uint16_t calculate_top_ms_TIMER0(uint16_t);
extern uint16_t calculate_top_us_TIMER0(uint16_t);

extern void update_tick_TIMER0(TIMER0_obj *, uint16_t);

#endif