#pragma once

#ifndef _GEN_DEFS_H_
#define _GEN_DEFS_H_
#include <stdlib.h>
#include <inttypes.h>

#define TRUE 1
#define FALSE 0

#define YES 1
#define NO 0

#define RIGHT 1
#define LEFT 0

typedef enum PHYSICAL_PINS_LIST {

	_PD3,
	_PD4,
	_GND1,
	_VCC1,
	_GND2,
	_VCC2,
	_PB6,
	_PB7,
	_PD5,
	_PD6,
	_PD7,
	_PB0,
	_PB1,
	_PB2,
	_PB3,
	_PB4,
	_PB5,
	_AVCC,
	_ADC6,
	_AREF,
	_GND3,
	_ADC7,
	_PC0,
	_PC1,
	_PC2,
	_PC3,
	_PC4,
	_PC5,
	_PC6,
	_PD0,
	_PD1,
	_PD2,
	NUM_PHYSICAL_PINS
}physical_pins_list;

typedef enum PIN_PURPOSES {

	_INPUT,
	_OUTPUT,
	_ALT1,
	_ALT2,
	NUM_OF_PURPOSES
}pin_purpose;

#endif