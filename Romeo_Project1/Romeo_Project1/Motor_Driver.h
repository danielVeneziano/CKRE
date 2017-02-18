#pragma once

#ifndef _MTR_DRV_H_
#define _MTR_DRV_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util\atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"
#include "Timer_0.h"
#include "DIO.h"

#define _MTR_DRV_DEBUG_	0

#define LEFT_MTR_PIN _PD4
#define RIGHT_MTR_PIN _PD7

#define FWD 1
#define BKWD 0

typedef struct MTR_CHANNEL_struct {

	dig_output *pin;
	uint8_t speed;
	uint8_t dir;

}mtr_channel;

typedef struct MTR_DRV_struct {

	TIMER0_obj *timer_pwm;
	mtr_channel *left_mtr;
	mtr_channel *right_mtr;
	
}mtr_drv;

extern mtr_drv *NEW_mtr_drv(void);
extern void initialize_mtr_drv(mtr_drv *);
extern void destructor_mtr_drv(mtr_drv *);
extern void left_motor_control(mtr_drv *, uint8_t, uint8_t);
extern void right_motor_control(mtr_drv *, uint8_t, uint8_t);

#endif
