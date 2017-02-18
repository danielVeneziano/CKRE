#pragma once

#ifndef _PINS_H_
#define _PINS_H_
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include "General_Definitions.h"

#define _PINS_DEBUG_ 0

#endif

// THIS OBJECT DEFINES THE PROPERTIES OF THE ACTUAL PHYSICAL PORTS OF THE ROMEO BOARD
typedef struct port_struct {

	volatile uint8_t * const r_ddr;
	volatile uint8_t * const r_port;
	volatile uint8_t * const r_pin;
	volatile uint8_t * const r_pcmsk;
	volatile uint8_t * const r_pcicir;
	uint8_t pcie_channel;
}port_obj;

// THIS OBJECT DEFINES THE PROPERTIES OF THE ACTUAL PHYSICAL PORTS OF THE ROMEO BOARD
typedef struct pin_struct {

	port_obj *registers;

	uint8_t channel : 3;
	uint8_t purpose : 2;
	uint8_t pcie : 1;
	uint8_t prev_state : 1;

	uint8_t *connected;
}pin_obj;

extern pin_obj physical_pins[];

extern void connect_PIN(pin_obj *, uint8_t *);
extern void disconnect_PIN(pin_obj *);
extern uint8_t check_connection_PIN(pin_obj *);
extern void assign_purpose_PIN(pin_obj *, uint8_t);
extern uint8_t get_purpose_PIN(pin_obj *);
extern void enable_pin_change_int_PIN(pin_obj *);
extern void disable_pin_change_int_PIN(pin_obj *);