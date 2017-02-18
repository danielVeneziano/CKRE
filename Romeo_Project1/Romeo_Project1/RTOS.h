#pragma once

#ifndef _RTOS_H_
#define _RTOS_H_
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "General_Definitions.h"

#define _RTOS_DEBUG_	0

extern volatile uint8_t RTOS_running;

typedef struct time_event_struct {

	volatile uint16_t tick;
	volatile uint16_t period;
	volatile uint8_t enable : 1;
	volatile uint8_t flag : 1;

}time_event;


extern time_event *NEW_time_event(void);
extern void initialize_time_event(time_event *, uint16_t);
extern void destructor_time_event(time_event *);
extern void disable_time_event(time_event *);


#endif