#pragma once

#ifndef _DIO_H_
#define _DIO_H_
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"

#define _DIO_DEBUG_ 0

// THIS STRUCT STORES A REFERENCE TO A PIN OBJECT AND A CONNECTED BOOLEAN (PIN OBJECT CONTAINS ALL RELEVANT DIGIAL IO PROPERTIES)
typedef struct DIG_INPUT {

	pin_obj *pin;
	uint8_t connected;
}dig_input;

// THIS STRUCT CONTAINS AN ARRAY OF DIGITAL INPUT ELEMENTS
typedef struct DIG_INPUT_ARRAY {

	dig_input **elements;
	uint8_t num_elements;
	uint8_t prev_val;
}dig_input_array;

// THIS STRUCT STORES A REFERENCE TO A PIN OBJECT AND A CONNECTED BOOLEAN (PIN OBJECT CONTAINS ALL RELEVANT DIGIAL IO PROPERTIES)
typedef struct DIG_OUTPUT {

	pin_obj *pin;
	uint8_t connected;
}dig_output;

// THIS STRUCT CONTAINS AN ARRAY OF DIP ELEMENTS
typedef struct DIG_OUTPUT_ARRAY {

	dig_output **elements;
	uint8_t num_elements;
	uint8_t val;
}dig_output_array;

extern dig_input *NEW_dig_input(void);
extern dig_input_array *NEW_dig_input_array(uint8_t);
extern dig_output *NEW_dig_output(void);
extern dig_output_array *NEW_dig_output_array(uint8_t);
extern void dig_input_initialize(dig_input *, uint8_t);
extern void dig_output_initialize(dig_output *, uint8_t);
extern void dig_input_destructor(dig_input *);
extern void dig_input_array_destructor(dig_input_array *);
extern void dig_output_destructor(dig_output *);
extern void dig_output_array_destructor(dig_output_array *);

extern uint8_t status_dig_input(dig_input *);
extern uint8_t rising_edge_dig_input(dig_input *);
extern uint8_t falling_edge_dig_input(dig_input *);
extern uint8_t on_change_dig_input(dig_input *);

extern uint8_t status_dig_input_array(dig_input_array *);
extern uint8_t on_change_dig_input_array(dig_input_array *);
extern uint8_t encode_dig_input_array(dig_input_array *);

extern void toggle_dig_output(dig_output *);
extern void set_dig_output(dig_output *);
extern void clr_dig_output(dig_output *);
extern uint8_t status_dig_output(dig_output *);

extern void set_dig_output_array(dig_output_array *);
extern void clr_dig_output_array(dig_output_array *);
extern void increment_dig_output_array(dig_output_array *);
extern void decrement_dig_output_array(dig_output_array *);
extern uint8_t status_dig_output_array(dig_output_array *);
extern void configure_dig_output_array(dig_output_array *, uint8_t);
extern void update_dig_output_array(dig_output_array *);

#endif

