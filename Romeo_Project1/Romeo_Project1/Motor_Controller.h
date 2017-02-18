#pragma once

#ifndef _MTR_CNTRL_H_
#define _MTR_CNTRL_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util\atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"
#include "RTOS.h"
#include "Motor_Driver.h"

#define _MTR_CNTRL_DEBUG_	0

#define TURN_SPEED 180
#define MAX_SPEED 190
#define MIN_SPEED 25
#define P_CNTRL_SPEED 190

#define SPEED_SCALE 121			// DIVIDED BY 128 (7 BIT SHIFTS RIGHT)

#define RAMP_TIME 100			// MUST BE LESS THAN 256 TO PREVENT OVERFLOW IN SCALING CALC
#define START_OFFSET 30			// CORRECTS FOR DELAY IN RIGHT MOTOR AND ADJUSTS FOR SCALING AT SMALL TICK VALS, WHICH WOULD EQUAL ZERO DUE TO TRUNCATION

#define Kp 1

typedef struct MTR_CONTROLLER_struct {

	mtr_drv *drive;

}mtr_controller;

extern mtr_controller *NEW_mtr_cntrl(void);
extern void initialize_mtr_cntrl(mtr_controller *);
extern void destructor_mtr_cntrl(mtr_controller *);

uint8_t speed_correction(uint8_t);

extern void drive_forward(mtr_controller *, uint8_t);
extern void drive_backward(mtr_controller *, uint8_t);
extern void turn_right(mtr_controller *);
extern void turn_left(mtr_controller *);
extern void stop(mtr_controller *);

extern uint8_t drive_forward_ms(mtr_controller *, uint8_t, time_event *, uint16_t);
extern uint8_t drive_backward_ms(mtr_controller *, uint8_t, time_event *, uint16_t);
extern uint8_t turn_right_ms(mtr_controller *, time_event *, uint16_t);
extern uint8_t turn_left_ms(mtr_controller *, time_event *, uint16_t);

extern void P_Control(mtr_controller *, uint16_t, uint8_t);

#endif

