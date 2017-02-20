#include "DIO.h"

/*
	> ASSUMES THAT INPUTS ARE CONNECTED TO NORMALLY OPEN CONTACTS WITH PULL UP RESISTORS. ALL LOGIC BASED ON THIS ASSUMPTION.
	> TO DO:
	> Add property to flip logic for inputs connected to Normally Closed contacts with external pull down resistors
	> Add checks on the valid pin numbers
*/

// CREATES A NEW INSTANCE OF A DIG INPUT OBJECT (CALL THIS BEFORE INI)
dig_input *NEW_dig_input(void) {

	dig_input *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
		me->pin = malloc(sizeof(*me->pin));
	}

#if _DIO_DEBUG_
	// insert checks here
#endif

	me->pin = NULL;
	me->connected = 0;

	return me;
}
// CREATES A NEW INSTANCE OF A DIG INPUT ARRAY OBJECT (CALL THIS BEFORE INI)
dig_input_array *NEW_dig_input_array(uint8_t ini_size) {

	int8_t i;

	dig_input_array *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	}

#if _DIO_DEBUG_
	// insert checks here
#endif

	me->num_elements = ini_size;

	me->elements = malloc(me->num_elements*sizeof(me->elements));

#if _DIO_DEBUG_
	// insert checks here
#endif

	for (i = 0; i < me->num_elements; i++)
		me->elements[i] = NEW_dig_input();

	me->prev_val = 0;

	return me;
}
// THIS FUNCTION SETS UP THE PIN AS A DIGITAL INPUT AND SETS THE PURPOSE OF THE SOFTWARE REPRESENTATION OF THIS PIN ACCORDINGLY
void dig_input_initialize(dig_input *me, uint8_t pin_number) {

	connect_PIN(&physical_pins[pin_number], &me->connected);

	assign_purpose_PIN(&physical_pins[pin_number], _INPUT);

	*physical_pins[pin_number].registers->r_ddr &= ~(1 << physical_pins[pin_number].channel);		// set pin as input
	*physical_pins[pin_number].registers->r_port |= (1 << physical_pins[pin_number].channel);		// enable pull up resistor

	physical_pins[pin_number].prev_state = status_dig_input(me);

	me->pin = &physical_pins[pin_number];

	return;
}
// FREES MEMEORY ALLOCATED FOR DIG_INPUT (PREVENTS LEAKS)
void dig_input_destructor(dig_input *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me->pin);
		free(me);
	}

	return;
}
// FREES MEMEORY ALLOCATED FOR DIG INPUT ARRAY (PREVENTS LEAKS)
void dig_input_array_destructor(dig_input_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		dig_input_destructor(me->elements[i]);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me->elements);
		free(me);
	}

	return;
}
// CREATES A NEW INSTANCE OF A DIG OUTPUT OBJECT (CALL THIS BEFORE INI)
dig_output *NEW_dig_output(void) {

	dig_output *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
		me->pin = malloc(sizeof(*me->pin));
	}

#if _DIO_DEBUG_
	// insert checks here
#endif

	me->pin = NULL;
	me->connected = 0;

	return me;
}
// CREATES A NEW INSTANCE OF A DIG OUTPUT ARRAY OBJECT (CALL THIS BEFORE INI)
dig_output_array *NEW_dig_output_array(uint8_t array_size) {

	int8_t i;
	dig_output_array *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	}

#if _DIO_DEBUG_
	// insert checks here
#endif

	me->num_elements = array_size;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->elements = malloc(me->num_elements*sizeof(me->elements));
	}

#if _DIO_DEBUG_
	// insert checks here
#endif

	for (i = 0; i < me->num_elements; i++)
		me->elements[i] = NEW_dig_output();

	me->val = 0;

	return me;
}
// THIS FUNCTION SETS UP THE PIN AS A DIGITAL OUTPUT AND SETS THE PURPOSE OF THE SOFTWARE REPRESENTATION OF THIS PIN ACCORDINGLY
void dig_output_initialize(dig_output *me, uint8_t pin_number) {

	connect_PIN(&physical_pins[pin_number], &me->connected);

	assign_purpose_PIN(&physical_pins[pin_number], _OUTPUT);

	*physical_pins[pin_number].registers->r_ddr |= (1 << physical_pins[pin_number].channel);		// set pin as output
	*physical_pins[pin_number].registers->r_port &= ~(1 << physical_pins[pin_number].channel);		// clear any value currently on pin

	me->pin = &physical_pins[pin_number];

	return;
}
// FREES MEMEORY ALLOCATED FOR DIG OUTPUT (PREVENTS LEAKS)
void dig_output_destructor(dig_output *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me->pin);
		free(me);
	}

	return;
}
// FREES MEMEORY ALLOCATED FOR DIG OUTPUT ARRAY (PREVENTS LEAKS)
void dig_output_array_destructor(dig_output_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		dig_output_destructor(me->elements[i]);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me->elements);
		free(me);
	}

	return;
}
// RETURNS THE STATUS OF THE SPECIFIED PIN (ASSUMES PIN IS INPUT)
uint8_t status_dig_input(dig_input *me) {

	return (uint8_t)((*me->pin->registers->r_pin & (1 << me->pin->channel)) == 0);			// return 0 if high (no input) and 1 if low (input) because of pull up
}
// CHECKS FOR RISING EDGE AND RETURNS BOOLEAN
uint8_t rising_edge_dig_input(dig_input *me) {

	if (me->pin->prev_state == 0) {

		if (status_dig_input(me) == 1) {

			me->pin->prev_state = status_dig_input(me);

			return 1;											// return 1 (true) if dig_input state when from high to low
		}
	}

	me->pin->prev_state = status_dig_input(me);

	return 0;													// return 0 (false) if dig_input state if state did not go from high to low
}
// CHECKS FOR FALLING EDGE AND RETURNS BOOLEAN
uint8_t falling_edge_dig_input(dig_input *me) {

	if (me->pin->prev_state == 1) {

		if (status_dig_input(me) == 0) {

			me->pin->prev_state = status_dig_input(me);

			return 1;											// return 1 (true) if dig_input state when from low to high
		}
	}

	me->pin->prev_state = status_dig_input(me);

	return 0;													// return 0 (false) if dig_input state if state did not go from low to high
}
// CHECKS FOR ANY CHANGE IN THE SPECIFIED PINS STATUS AND RETURNS BOOLEAN
uint8_t on_change_dig_input(dig_input *me) {

	if (me->pin->prev_state != status_dig_input(me)) {

		me->pin->prev_state = status_dig_input(me);

		return 1;												// return 1 (true) if dig_input state when from low to high
	}

	me->pin->prev_state = status_dig_input(me);

	return 0;													// return 0 (false) if dig_input state if state did not go from low to high
}
// CHECKS STATUS OF DIG INPUT ARRAY AND RETURNS 8-BIT CODED RESULT
uint8_t status_dig_input_array(dig_input_array *me) {

	int8_t i;
	uint8_t status = 0;

	for (i = 0; i < me->num_elements; i++) {

		status |= (status_dig_input(me->elements[i]) << i);			// status_dig_input returns bool so shifting is necessary
	}

	return status;
}
// CHECKS DIG INPUT ARRAY FOR ANY CHANGES IN STATUS OF DIG INPUT ELEMENTS
uint8_t on_change_dig_input_array(dig_input_array *me) {

	uint8_t val = status_dig_input_array(me);

	if (me->prev_val != val) {

		me->prev_val = val;

		return 1;												// return 1 (true) if dig input array val changed
	}

	me->prev_val = val;

	return 0;													// return 0 (false) if dig input array val did not change
}
// Encodes the binary number read in from the dig input switches into a binary number that will display that many leds
uint8_t encode_dig_input_array(dig_input_array *me) {

	int8_t i, status, result;

	status = status_dig_input_array(me);

	result = 0;

	for (i = 0; i < status; i++) {

		result |= (1 << i);										// status_dig_input returns bool so shifting is necessary
	}

	return result;
}
// TOGGLES THE OUTPUT OF THE SPECIFIED PIN
void toggle_dig_output(dig_output *me) {

	*me->pin->registers->r_port ^= (1 << me->pin->channel);

	return;
}
// SETS THE OUTPUT OF THE SPECIFIED PIN
void set_dig_output(dig_output *me) {

	*me->pin->registers->r_port |= (1 << me->pin->channel);

	return;
}
// CLEARS THE OUTPUT OF THE SPECIFIED PIN
void clr_dig_output(dig_output *me) {

	*me->pin->registers->r_port &= ~(1 << me->pin->channel);

	return;
}
// RETURNS THE STATUS OF THE SPECIFIED PIN (ASSUMES PIN IS OUTPUT)
uint8_t status_dig_output(dig_output *me) {

	return (uint8_t)((*me->pin->registers->r_port & (1 << me->pin->channel)) != 0);			//return 0 if low 2^channel if high (evaluates true or false)
}
// SETS ALL OF THE DIG OUTPUTS IN THE DIG OUTPUT ARRAY HIGH
void set_dig_output_array(dig_output_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		set_dig_output(me->elements[i]);

	return;
}
// CLEARS ALL THE BITS OF THE DIGITAL OUTPUT ARRAY
void clr_dig_output_array(dig_output_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		clr_dig_output(me->elements[i]);

	return;
}
// INCREMENTS THE VAL PROPERTY OF THE DIGITAL OUTPUT ARRAY AND UPDATES THE HARDWARE IO TO MATCH
void increment_dig_output_array(dig_output_array *me) {

	me->val += (1 << 0);

	update_dig_output_array(me);

	return;
}
// DECREMENTS THE VAL PROPERTY OF THE DIGITAL OUTPUT ARRAY AND UPDATES THE HARDWARE IO TO MATCH
void decrement_dig_output_array(dig_output_array *me) {

	me->val -= (1 << 0);

	update_dig_output_array(me);

	return;
}
// RETURNS THE ACTUAL HARDWARE IO STATES IN BINARY AS A PACKED 8 BIT INTEGER
uint8_t status_dig_output_array(dig_output_array *me) {

	int8_t i;
	uint8_t status;

	status = 0;

	for (i = 0; i < me->num_elements; i++) {

		status |= (status_dig_output(me->elements[i]) << i);				//status_dig_output returns bool so re-shift is needed
	}

	return status;
}
// UPDATES THE VAL PROPERTY OF THE OUTPUT ARRAY AND UPDATES THE HARDWARE IO
void configure_dig_output_array(dig_output_array *me, uint8_t new_val) {

	me->val = new_val;
	update_dig_output_array(me);

	return;
}
// UPDATES THE HARDWARE IO TO MATCH THE CURRENT VAL PROPERTY
void update_dig_output_array(dig_output_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++) {
		if ((me->val) & (1 << i))
			set_dig_output(me->elements[i]);
		else
			clr_dig_output(me->elements[i]);
	}

	return;
}