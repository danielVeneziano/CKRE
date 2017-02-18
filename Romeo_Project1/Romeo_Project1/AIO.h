#pragma once

#ifndef _AIO_H_
#define _AIO_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"
#include "RTOS.h"

#define _AIO_DEBUG_	0

typedef struct ANALOG_IN {

	uint8_t channel : 3;
	uint8_t num_samples : 4;
	uint8_t : 0;
	uint8_t connected;
	uint16_t val;
	uint16_t calibration_min;
	uint16_t calibration_max;
}analog_in;

typedef struct ANALOG_IN_ARRAY {

	analog_in **elements;
	uint8_t num_elements;
	uint16_t position;
}analog_in_array;

extern analog_in *NEW_analog_in(void);
extern analog_in_array *NEW_analog_in_array(uint8_t array_size);
extern void analog_in_initialize(analog_in *, uint8_t, uint8_t);
extern void analog_in_destructor(analog_in *);
extern void analog_in_array_destructor(analog_in_array *);

extern void poll_analog_in(analog_in *);
extern void poll_analog_in_now(analog_in *);

extern uint16_t analog_in_get_raw_value(analog_in *);
extern uint8_t analog_in_get_raw_value_uint8(analog_in *);

extern uint16_t analog_in_get_value(analog_in *);

extern uint16_t analog_in_get_clamped_value(analog_in *, uint16_t, uint16_t);
extern uint16_t analog_in_get_scaled_value(analog_in *, uint16_t, uint16_t);

extern void analog_in_calibrate_high(analog_in *);
extern void analog_in_calibrate_low(analog_in *);

extern uint16_t analog_in_get_calibrated_value(analog_in *);
extern void analog_in_reset_calibration(analog_in *);

// array functions
extern void poll_analog_in_array(analog_in_array *);
extern void poll_analog_in_array_now(analog_in_array *);

extern void analog_in_array_get_raw_values(analog_in_array *);
extern void analog_in_array_get_raw_values_uint8(analog_in_array *);

extern void analog_in_array_get_values(analog_in_array *);

extern void analog_in_array_get_clamped_values(analog_in_array *, uint16_t, uint16_t);
extern void analog_in_array_get_scaled_values(analog_in_array *, uint16_t, uint16_t);

extern void analog_in_array_calibrate_high(analog_in_array *);
extern void analog_in_array_calibrate_low(analog_in_array *);
extern void analog_in_array_reset_calibration(analog_in_array *);

extern void analog_in_array_get_calibrated_values(analog_in_array *);

extern void analog_in_array_reset_calibration(analog_in_array *);

extern uint16_t analog_in_array_find_edge(analog_in_array *);

#endif
