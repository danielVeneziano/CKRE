#pragma once

#ifndef _FSM_H_
#define _FSM_H_
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "General_Definitions.h"

#define _FSM_DEBUG_ 0
#define TRANSITION 0
#define PREEMPT 1

//LIST OF ALL POSSIBLE STATES ENCODED INTO ENUM
typedef enum STATE_IDS {

	_wander,
	_obstcl_f,
	_obstcl_r,
	_obstcl_l,
	_spotlight,
	_lights_off,
	_found_line,
	_follow_line,
	num_states
}state_ids;

//LIST OF ALL POSSIBLE EVENTS ENCODED INTO ENUM
typedef enum EVENT_IDS {

	_OBSTCL_F,
	_OBSTCL_R,
	_OBSTCL_L,
	_SPOTLIGHT,
	_LIGHTS_OFF,
	_CONTINUE,
	_FOUND_LINE,
	_TRUE,
	num_events
}event_ids;

#define _no_change -1												// USED TO IDENTIFY WHEN NO STATE CHANGE IS REQUIRED (CHECK IF < 0)

extern uint8_t FSM_transition(uint8_t, uint8_t);
uint8_t FSM_preempt(uint8_t, uint8_t);

#endif