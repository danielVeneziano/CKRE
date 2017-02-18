#pragma once

#ifndef _USART_H_
#define _USART_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "Physical_Pins.h"
#include "General_Definitions.h"
#include "RTOS.h"

#define _USART_DEBUG_	0

#define USART_ASYNCHRONOUS	0
#define USART_SYNCHRONOUS	1
#define USART_MASTER_SPI	3								// NOT CURRENTLY SUPPORTED

#define USART_DISABLE_OPTION	0
#define USART_ENABLE_OPTION		1

#define USART_EVEN_PARITY	2
#define USART_ODD_PARITY	3

#define USART_XCK_OUTPUT	0
#define USART_XCK_INPUT		1

#define USART_FIVE_BITS		0
#define USART_SIX_BITS		1
#define USART_SEVEN_BITS	2
#define USART_EIGHT_BITS	3

//DEFAULT SETTINGS
#define USART_DBL_MODE_DEF			USART_ENABLE_OPTION		// USART_DISABLE_OPTION or USART_ENABLE_OPTION
#define USART_MLTI_PRCR_MODE_DEF	USART_DISABLE_OPTION	// USART_DISABLE_OPTION or USART_ENABLE_OPTION										(USART_MLTI_PRCR_MODE_DEF not currently supported)

#define USART_RX_IE_DEF				USART_DISABLE_OPTION	// DISABLE or ENABLE
#define USART_TX_IE_DEF				USART_DISABLE_OPTION	// DISABLE or ENABLE
#define USART_DR_IE_DEF				USART_DISABLE_OPTION	// DISABLE or ENABLE
#define USART_RX_EN_DEF				USART_ENABLE_OPTION		// DISABLE or ENABLE
#define USART_TX_EN_DEF				USART_ENABLE_OPTION		// DISABLE or ENABLE
#define USART_DATA_LEN_DEF			USART_EIGHT_BITS		// USART_FIVE_BITS or USART_SIX_BITS or USART_SEVEN_BITS or USART_EIGHT_BITS		(9 BITS DETERMINED BY SEPARATE BOOL)
#define USART_DATA_9_BITS_DEF		USART_DISABLE_OPTION	// DISABLE or ENABLE																(OVERRIDES THE DATA LEN MEMBER)

#define USART_MODE_DEF				USART_ASYNCHRONOUS		// ASYNCHRNOUS or USART_SYNCHRONOUS or USART_MASTER_SPI								(USART_MASTER_SPI not currently supported)
#define USART_PARITY_DEF			USART_DISABLE_OPTION	// USART_DISABLE_OPTION or USART_EVEN_PARITY or USART_ODD_PARITY
#define USART_TWO_STOP_BITS_DEF		USART_DISABLE_OPTION	// USART_DISABLE_OPTION or USART_ENABLE_OPTION					
#define USART_FLIP_CLK_POLARITY_DEF	USART_DISABLE_OPTION	// USART_DISABLE_OPTION or USART_ENABLE_OPTION

#define USART_XCK_CHANNEL	_PD4							// CHANNEL OF EXTERNAL CLOCK: PORT D, PIN 4
#define USART_XCK_DIR_DEF	USART_XCK_OUTPUT				// USART CLOCK FREQUENCY
#define USART_FOSC_DEF		F_CPU							// USART CLOCK FREQUENCY
#define USART_BAUD_DEF		115200							// DEFAULT BAUD RATE

#if USART_MODE_DEF == USART_ASYNCHRONOUS
#define USART_BAUD_DIVISOR (16 >> USART_DBL_MODE_DEF)
#elif USART_MODE_DEF == USART_SYNCHRONOUS
#define USART_BAUD_DIVISOR 2
#endif

#define USART_UBRR_DEF (USART_FOSC_DEF/USART_BAUD_DIVISOR/USART_BAUD_DEF) - 1

typedef struct USART_registers_struct {

	volatile uint8_t * const r_data;
	volatile uint8_t * const r_csa;
	volatile uint8_t * const r_csb;
	volatile uint8_t * const r_csc;
	volatile uint8_t * const r_brrh;
	volatile uint8_t * const r_brrl;

}USART_registers_obj;

typedef struct USART_output_struct {

	volatile uint8_t len;
	volatile uint8_t index;
	volatile char text[255];

}USART_output_obj;

typedef struct USART_struct {

	// USART register pointers
	USART_registers_obj *registers;

	// UCSR0A register members
	uint8_t dbl_mode : 2;
	uint8_t multi_proccessor : 1;

	// UCSR0B register members
	uint8_t rx_int_en : 1;
	uint8_t tx_int_en : 1;
	uint8_t data_reg_int_en : 1;
	uint8_t receive_en : 1;
	uint8_t transmit_en : 1;
	uint8_t data_frame_len : 2;

	// UCSR0C register members
	uint8_t mode : 2;
	uint8_t parity : 2;
	uint8_t two_stop_bits : 1;
	uint8_t data_9_bits : 1;
	uint8_t clk_polarity : 1;

	// UBRR0_ registers members (take careful notice of units! So we can use 16 bit ints)
	uint8_t xck_dir : 1;								// for Synchronous communication specifies clock direction (0 input = slave, 1 output = master)
	uint32_t fosc;										// pass freq to ini in Hz, stored as Hz / 100
	uint16_t baud;										// pass freq to ini in Hz, stored as Hz / 100
	uint16_t ubrr;										// calculated in ini function

}USART_obj;

extern USART_obj *NEW_USART(void);
extern int8_t parse_ini_args_USART(char *);
extern void initialize_USART(USART_obj *, ...);
extern void initialize_USART_def(USART_obj *);
extern void USART_destructor(USART_obj *);

//static inline void transmit_USART_generic_now(uint16_t);
//static inline uint16_t receive_USART_generic_now(void);
//static inline void transmit_USART_char_now(char);
//static inline char receive_USART_char_now(void);

void print_USART(void);
void print_USART_now(void);
void clear_output_USART(void);

extern void clear_buffer(void);

void print_string_now(char *);
void print_home_now(void);
void print_new_line_now(void);
void print_byte_now(uint8_t);

extern void append_char(char);
extern void append_string(char *);
extern void append_home(void);
extern void append_new_line(void);
extern void append_byte(uint8_t);
extern void append_decimal_uint8(uint8_t);
extern void append_decimal_int8(int8_t);
extern void append_decimal_uint16(uint16_t);
extern void append_decimal_int16(int16_t);
extern void append_fixed_point_uint16(uint16_t, uint8_t);

#endif