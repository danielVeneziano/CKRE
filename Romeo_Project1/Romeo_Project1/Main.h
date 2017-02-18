#pragma once

/*
Preprocessor Directives and Definitions
*/
#ifndef _MAIN_H_
#define _MAIN_H_

#define _MAIN_DEBUG_ 0

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "General_Definitions.h"
#include "Physical_Pins.h"
#include "Timer_2.h"
#include "RTOS.h"
#include "FSM.h"
#include "Motor_Controller.h"
#include "DIO.h"
#include "ADC.h"
#include "AIO.h"


#if _MAIN_DEBUG_
	#include "USART.h"
#endif

#define LIGHT_NUM_SAMPLES 6
#define LINE_NUM_SAMPLES 4
#define EDGE_NUM_SAMPLES 2

#define ADC_BIT_MASK 255
#define ANALOG_DISP_TIME 4

#define DRV_SPEED 160

#define REVERSE_TIME 320
#define TURN_90_DEG_TIME 180
#define TURN_180_DEG_TIME 300
#define SPOTLIGHT_JUKE_TIME 1600
#define LIGHTS_OFF_TIME 3000
#define FIND_LINE_TIME 350

#define ERROR_DIVISOR 6				// # of bits to right shift error val


// FUNCTION PROTOTYPES
static void program_setup(void);
static void program_loop(void);
static void program_end(void);

static uint8_t wander_pgm(void);
static uint8_t reverse_180_pgm(void);
static uint8_t reverse_90_left_pgm(void);
static uint8_t reverse_90_right_pgm(void);
static uint8_t spotlight_pgm(void);
static uint8_t lights_off_pgm(void);
static uint8_t found_line_pgm(void);
static uint8_t follow_line_pgm(void);

//static uint8_t idle_function(void);

static void reset_events(void);
static uint8_t get_event(void);
static void update_events(void);

#endif