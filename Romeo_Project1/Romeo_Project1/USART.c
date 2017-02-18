
#include "USART.h"

static USART_registers_obj USART_registers = {

	.r_data = &UDR0,
	.r_csa = &UCSR0A,
	.r_csb = &UCSR0B,
	.r_csc = &UCSR0C,
	.r_brrh = &UBRR0H,
	.r_brrl = &UBRR0L,
};

static USART_output_obj USART_output = { 0 };

typedef enum INI_ARGS {

	_U2X0,
	_MCPM,
	_RXIE,
	_TXIE,
	_DRIE,
	_RXEN,
	_TXEN,
	_USZ9,
	_MSEL,
	_UPM0,
	_USBS,
	_UCSZ,
	_CPOL,
	_CDIR,
	_FOSC,
	_BAUD,
	NUM_USART_ARGS
}ini_args;

static const char *ini_arg_lst[NUM_USART_ARGS + 1] = {

	"U2X0",
	"MCPM",
	"RXIE",
	"TXIE",
	"DRIE",
	"RXEN",
	"TXEN",
	"USZ9",
	"MSEL",
	"UPM0",
	"USBS",
	"UCSZ"
	"CPOL",
	"CDIR",
	"FOSC",
	"BAUD",
	"END"
};

USART_obj *NEW_USART(void) {

	USART_obj *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	
		me->registers = &USART_registers;
	}

#if _USART_DEBUG_
	// ASSERT MALLOC
#endif

	// UCSR0A register members
	me->dbl_mode = USART_DBL_MODE_DEF;
	me->multi_proccessor = USART_MLTI_PRCR_MODE_DEF;

	// UCSR0B register members
	me->rx_int_en = USART_RX_IE_DEF;
	me->tx_int_en = USART_TX_IE_DEF;
	me->data_reg_int_en = USART_DR_IE_DEF;
	me->receive_en = USART_RX_EN_DEF;
	me->transmit_en = USART_TX_EN_DEF;
	me->data_9_bits = USART_DATA_9_BITS_DEF;

	// UCSR0C register members
	me->mode = USART_MODE_DEF;
	me->parity = USART_PARITY_DEF;
	me->two_stop_bits = USART_TWO_STOP_BITS_DEF;
	me->data_frame_len = USART_DATA_LEN_DEF;
	me->clk_polarity = USART_FLIP_CLK_POLARITY_DEF;

	// UBRR0_ registers members (take careful notice of units! So we can use 16 bit ints)
	me->xck_dir = USART_XCK_DIR_DEF;

	// This commented block is for ref only, describes how external clock is handled in ini function
	//if (me->xck_dir)
	//	me->fosc = fxck; // must specify external clock freq = fxck (note: fxck < fosc/4)
	//else
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->fosc = USART_FOSC_DEF / 100;						// Clock Speed in Hz * 100 (MHz * 100 000)
		me->baud = USART_BAUD_DEF / 100;						// Baud Rate in Hz * 100
		me->ubrr = USART_UBRR_DEF;								// Calculated in header file
	}

	return me;
}

int8_t parse_ini_args_USART(char *arg) {

	int8_t i;

	for (i = 0; i < NUM_USART_ARGS; i++) {
		if (!strcmp(ini_arg_lst[i], arg))
			return i;
	}

	return -1;
}

void initialize_USART(USART_obj *me, ...) {

	int8_t jmp = 0;
	char *str;
	uint8_t val = 0;
	uint32_t val_32 = 0;

	va_list ap;

	// DEFAULT VALUES LOADED BY NEW_USART, THIS JUST UPDATES

	va_start(ap, me);

	while (jmp >= 0) {

		str = va_arg(ap, char *);

		if (strlen(str) != 4) {
			jmp = -1;
		}
		else {
			jmp = parse_ini_args_USART(str);
		}

		switch (jmp) {

		case _U2X0:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->dbl_mode = val;
			break;
		case _MCPM:
			val = (uint8_t)va_arg(ap, int);
			//if ((val >= 0) && (val <= 1))					// NOT CURRENTLY SUPPORTED
			//	me->multi_proccessor = val;
			break;
		case _RXIE:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->rx_int_en = val;
			break;
		case _TXIE:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->tx_int_en = val;
			break;
		case _DRIE:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->data_reg_int_en = val;
			break;
		case _RXEN:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->receive_en = val;
			break;
		case _TXEN:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->transmit_en = val;
			break;
		case _USZ9:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->data_9_bits = val;
			break;
		case _MSEL:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))					// MASTER SPI MODE NOT CURRENTLY SUPPOTED, IF ADDED CHECK BOUNDS WILL EXTEND TO <= 3 (2 IS INVALID)
				me->mode = val;
			break;
		case _UPM0:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->parity = val;
			break;
		case _USBS:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->two_stop_bits = val;
			break;
		case _UCSZ:
			val = (uint8_t)va_arg(ap, int);
			if (me->data_9_bits == USART_ENABLE_OPTION)
				me->data_frame_len = 3;
			else if ((val >= 0) && (val <= 3))
				me->data_frame_len = val;
			break;
		case _CPOL:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->clk_polarity = val;
			break;
		case _CDIR:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->xck_dir = val;
			break;
		case _FOSC:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				val_32 = (uint32_t)va_arg(ap, long);
				if ((me->xck_dir == USART_XCK_INPUT) && (me->mode == USART_SYNCHRONOUS))
					me->fosc = val_32 / 100;															// must specify external clock freq = fxck (note: fxck < fosc/4)
				else
					me->fosc = USART_FOSC_DEF / 100;
			}
			break;
		case _BAUD:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				val_32 = (uint32_t)va_arg(ap, long);
				me->baud = (uint16_t)(val_32 / 100);
			}
			break;
		default:
			//debug stuff here
			break;
		}
	}

	// Set UBRR value
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (me->mode == USART_ASYNCHRONOUS) {
			me->ubrr = (uint16_t)((me->fosc / me->baud / (16 >> me->dbl_mode)) - 1);
		}
		else {
			me->ubrr = (uint16_t)((me->fosc / me->baud / 2) - 1);
			assign_purpose_PIN(&physical_pins[USART_XCK_CHANNEL], _ALT1);
			if (me->xck_dir == USART_XCK_INPUT)
				*physical_pins[USART_XCK_CHANNEL].registers->r_ddr &= ~(1 << physical_pins[USART_XCK_CHANNEL].channel);
			else
				*physical_pins[USART_XCK_CHANNEL].registers->r_ddr |= (1 << physical_pins[USART_XCK_CHANNEL].channel);
		}
	
		*me->registers->r_brrh = (uint8_t)(me->ubrr >> 8);
		*me->registers->r_brrl = (uint8_t)me->ubrr;
	}

	*me->registers->r_csa = (me->dbl_mode << U2X0) | (me->multi_proccessor << MPCM0);
	*me->registers->r_csb = (me->rx_int_en << RXCIE0) | (me->tx_int_en << TXCIE0) | (me->data_reg_int_en << UDRIE0) | (me->receive_en << RXEN0) | (me->transmit_en << TXEN0);
	*me->registers->r_csc = (me->mode << UMSEL00) | (me->parity << UPM00) | (me->two_stop_bits << USBS0) | (me->data_frame_len << UCSZ00) | (me->data_9_bits << UCSZ02) | (me->clk_polarity << UCPOL0);

	va_end(ap);

	return;
}

void initialize_USART_def(USART_obj *me) {

	// UCSR0A register members
	me->dbl_mode = USART_DBL_MODE_DEF;
	me->multi_proccessor = USART_MLTI_PRCR_MODE_DEF;

	// UCSR0B register members
	me->rx_int_en = USART_RX_IE_DEF;
	me->tx_int_en = USART_TX_IE_DEF;
	me->data_reg_int_en = USART_DR_IE_DEF;
	me->receive_en = USART_RX_EN_DEF;
	me->transmit_en = USART_TX_EN_DEF;
	me->data_9_bits = USART_DATA_9_BITS_DEF;

	// UCSR0C register members
	me->mode = USART_MODE_DEF;
	me->parity = USART_PARITY_DEF;
	me->two_stop_bits = USART_TWO_STOP_BITS_DEF;
	me->data_frame_len = USART_DATA_LEN_DEF;
	me->clk_polarity = USART_FLIP_CLK_POLARITY_DEF;

	// UBRR0_ registers members (take careful notice of units! So we can use 16 bit ints)
	me->xck_dir = USART_XCK_DIR_DEF;

	// This commented block is for ref only, describes how external clock is handled in ini function
	//if (me->xck_dir)
	//	me->fosc = fxck; // must specify external clock freq = fxck (note: fxck < fosc/4)
	//else
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->fosc = USART_FOSC_DEF / 100;						// Clock Speed in Hz * 100 (MHz * 100 000)
		me->baud = USART_BAUD_DEF / 100;						// Baud Rate in Hz * 100
		me->ubrr = USART_UBRR_DEF;								// Calculated in header file
	
		// Set UBRR value
		if (me->mode == USART_ASYNCHRONOUS) {
			me->ubrr = (uint16_t)((me->fosc / me->baud / (16 >> me->dbl_mode)) - 1);
		}
		else {
			me->ubrr = (uint16_t)((me->fosc / me->baud / 2) - 1);
			assign_purpose_PIN(&physical_pins[USART_XCK_CHANNEL], _ALT1);
			if (me->xck_dir == USART_XCK_INPUT)
				*physical_pins[USART_XCK_CHANNEL].registers->r_ddr &= ~(1 << physical_pins[USART_XCK_CHANNEL].channel);
			else
				*physical_pins[USART_XCK_CHANNEL].registers->r_ddr |= (1 << physical_pins[USART_XCK_CHANNEL].channel);
		}
	
		*me->registers->r_brrh = (uint8_t)(me->ubrr >> 8);
		*me->registers->r_brrl = (uint8_t)me->ubrr;
	}

	*me->registers->r_csa = (me->dbl_mode << U2X0) | (me->multi_proccessor << MPCM0);
	*me->registers->r_csb = (me->rx_int_en << RXCIE0) | (me->tx_int_en << TXCIE0) | (me->data_reg_int_en << UDRIE0) | (me->receive_en << RXEN0) | (me->transmit_en << TXEN0);
	*me->registers->r_csc = (me->mode << UMSEL00) | (me->parity << UPM00) | (me->two_stop_bits << USBS0) | (me->data_frame_len << UCSZ00) | (me->data_9_bits << UCSZ02) | (me->clk_polarity << UCPOL0);

	return;
}

void USART_destructor(USART_obj *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me);
	}

	return;
}

static inline void transmit_USART_generic_now(uint16_t data) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (*USART_registers.r_csa & (1 << UCSZ02))
			*USART_registers.r_csb |= (((uint8_t)(data >> 8) & (1 << 0)) << TXB80);
	}

	while (!(*USART_registers.r_csa & (1 << UDRE0)));

	*USART_registers.r_data = (uint8_t)data;

	return;
}

static inline uint16_t receive_USART_generic_now(void) {

	uint16_t data = 0;

	while (!(*USART_registers.r_csa) & (1 << RXC0));  // wait until previous data has been received 

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (*USART_registers.r_csa & (1 << UCSZ02))
			data = ((uint16_t)((*USART_registers.r_csb >> RXB80) & (1 << 0)) << 8);
	}

	data |= (uint16_t)*USART_registers.r_data;

	return data;
}

static inline void transmit_USART_char_now(char data) {

	while (!(*USART_registers.r_csa & (1 << UDRE0)));

	*USART_registers.r_data = data;

	return;
}

static inline char receive_USART_char_now(void) {

	while (!(*USART_registers.r_csa) & (1 << RXC0));  // wait until previous data has been received 

	return *USART_registers.r_data;
}

void print_USART(void) {

	USART_output.index = 0;

	RTOS_running = 1;

	if ((USART_output.text[USART_output.index] != '\0') && (USART_output.index <= USART_output.len))
		transmit_USART_char_now(USART_output.text[USART_output.index]);

	return;
}

void print_USART_now(void) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		for (USART_output.index = 0; (USART_output.text[USART_output.index] != '\0') && (USART_output.index <= USART_output.len); USART_output.index++)
			transmit_USART_char_now(USART_output.text[USART_output.index]);

		USART_output.text[0] = '\0';
		USART_output.index = 0;
		USART_output.len = 0;
	}

	return;
}

void clear_output_USART(void) {

	USART_output.text[0] = '\0';
	USART_output.index = 0;
	USART_output.len = 0;
	RTOS_running = 0;

	return;
}

void clear_buffer(void) {

	*USART_registers.r_csb &= ~(USART_RX_EN_DEF << RXEN0);
	*USART_registers.r_csb |= (USART_RX_EN_DEF << RXEN0);

	return;
}

void print_string_now(char *str) {

	int8_t i;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		for (i = 0; str[i] != '\0'; i++)
			transmit_USART_char_now(str[i]);
	}

	return;
}

void print_home_now(void) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		transmit_USART_char_now('\t');
		transmit_USART_char_now('\r');
	}

	return;
}

void print_new_line_now(void) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		transmit_USART_char_now('\n');
		transmit_USART_char_now('\r');
	}

	return;
}

void print_byte_now(uint8_t val) {

	int8_t i;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		for (i = 7; i >= 0; i--) {
			if (val & (1 << i))
				transmit_USART_char_now('1');
			else
				transmit_USART_char_now('0');
		}
	}

	return;
}

void append_char(char ascii_char) {

	if ((255 - USART_output.len) > 1) {							// strictly greater than to leave one space for a NULL character

		USART_output.text[USART_output.len] = ascii_char;
		USART_output.len++;
		USART_output.text[USART_output.len] = '\0';
	}

	return;
}

void append_string(char *str) {
	
	uint8_t i;

	if ((255 - USART_output.len) > strlen(str)) {						// strictly greater than to leave one space for a NULL character

		for (i = 0; str[i] != '\0'; i++) {

			USART_output.text[USART_output.len] = str[i];
			USART_output.len++;
		}

		USART_output.text[USART_output.len] = '\0';
	}

	return;
}

void append_byte(uint8_t val) {

	int8_t i;
	char ascii_characters[8 + 1] = { '\0' };

	for (i = 7; i >= 0; i--) {
		if (val & (1 << i))
			ascii_characters[i] = '1';
		else
			ascii_characters[i] = '0';;
	}

	ascii_characters[8] = '\0';

	append_string(ascii_characters);

	return;
}

void append_home(void) {

	append_string("\t\r");

	return;
}

void append_new_line(void) {

	append_string("\n\r");

	return;
}

void append_decimal_uint8(uint8_t val) {

	char ascii_characters[4] = { '\0' };

	sprintf(ascii_characters, "%4u", val);

	append_string(ascii_characters);

	return;
}

void append_decimal_int8(int8_t val) {

	char ascii_characters[5] = { '\0' };

	sprintf(ascii_characters, "%5d", val);

	append_string(ascii_characters);

	return;
}

void append_decimal_uint16(uint16_t val) {

	char ascii_characters[6] = { '\0' };

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		sprintf(ascii_characters, "%6u", val);
	}

	append_string(ascii_characters);

	return;
}

void append_decimal_int16(int16_t val) {

	char ascii_characters[7] = { '\0' };

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		sprintf(ascii_characters, "%7d", val);
	}

	append_string(ascii_characters);

	return;
}

void append_fixed_point_uint16(uint16_t val, uint8_t point_loc) {			// point_loc is the number of decimal places

	char ascii_characters[6+1] = { '\0' };
	int8_t i;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		sprintf(ascii_characters, "%6u", val);									// strlen is always 6 from sprintf. Val is always <= 5 chars. Add 1 for decimal point (added later) and 1 for NULL
	}

	if (point_loc >= 1) {

		for (i = 1; ascii_characters[i] == ' '; i++)						// find the leading digit and shift characters to the left
			ascii_characters[i - 1] = ascii_characters[i];

		for (; (i < 6 - point_loc) && (ascii_characters[i] != '\0'); i++)	// find the location in the string for the decimal point (if in string)
			ascii_characters[i - 1] = ascii_characters[i];

		ascii_characters[i - 1] = '.';										// add the decimal point in the correct position if required
	}

	ascii_characters[6] = '\0';												// double insurance that the NULL is not missing

	append_string(ascii_characters);

	return;
}

ISR(USART_TX_vect) {

	if (USART_output.len)
		USART_output.index++;

	if ((USART_output.text[USART_output.index] != '\0') && (USART_output.index <= USART_output.len)) {
		*USART_registers.r_data = USART_output.text[USART_output.index];
	}
	else {
		RTOS_running = 0;
		USART_output.text[0] = '\0';
		USART_output.index = 0;
		USART_output.len = 0;
	}
}