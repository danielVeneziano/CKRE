#include "AIO.h"
#include "ADC.h"

analog_in *NEW_analog_in(void) {

	analog_in *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	}

#if _AIO_DEBUG_
	// insert checks here
#endif

	me->channel = 0;
	me->num_samples = 0;
	me->val = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->calibration_min = 0;
		me->calibration_max = 1023;
	}

	return me;
}

analog_in_array *NEW_analog_in_array(uint8_t array_size) {

	int8_t i;
	analog_in_array *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	}

#if _AIO_DEBUG_
	// insert checks here
#endif

	me->num_elements = array_size;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->elements = malloc(me->num_elements*sizeof(me->elements));
	}

#if _AIO_DEBUG_
	// insert checks here
#endif

	for (i = 0; i < me->num_elements; i++)
		me->elements[i] = NEW_analog_in();

	me->position = 0;

	return me;
}


void analog_in_initialize(analog_in *me, uint8_t num_samples, uint8_t pin_number) {

	connect_PIN(&physical_pins[pin_number], &me->connected);

	assign_purpose_PIN(&physical_pins[pin_number], _ALT1);

	*physical_pins[pin_number].registers->r_ddr &= ~(1 << physical_pins[pin_number].channel);		// set pin as input
	*physical_pins[pin_number].registers->r_port &= ~(1 << physical_pins[pin_number].channel);		// disable pull up resistor

	physical_pins[pin_number].prev_state = 0;

	me->channel = physical_pins[pin_number].channel;
	me->num_samples = num_samples;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->val = analog_in_get_value(me);
	}

	return;
}

void analog_in_destructor(analog_in *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me);
	}

	return;
}

void analog_in_array_destructor(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		analog_in_destructor(me->elements[i]);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me->elements);
		free(me);
	}

	return;
}

void poll_analog_in(analog_in *me) {

	poll_ADC_channel(me->channel);

	return;
}

void poll_analog_in_now(analog_in *me) {


	poll_ADC_channel_now(me->channel);

	return;
}

uint16_t analog_in_get_raw_value(analog_in *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->val = get_raw_ADC(me->channel);
	}

	return me->val;
}

uint8_t analog_in_get_raw_value_uint8(analog_in *me) {

	me->val = get_raw_ADC_uint8_now(me->channel);

	return me->val;
}

uint16_t analog_in_get_value(analog_in *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->val = get_mov_avg_ADC(me->channel, me->num_samples);
	}

	return me->val;
}

uint16_t analog_in_get_clamped_value(analog_in *me, uint16_t min, uint16_t max) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->val = get_clamped_value_ADC(me->channel, min, max, me->num_samples);
	}

	return me->val;
}

uint16_t analog_in_get_scaled_value(analog_in *me, uint16_t min, uint16_t max) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->val = get_scaled_value_ADC(me->channel, min, max, me->num_samples);
	}

	return me->val;
}

void analog_in_calibrate_high(analog_in *me) {

	int8_t i;
	uint16_t next_val;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->calibration_max = 0;
	}

	for (i = 0; i < 10; i++) {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			next_val = analog_in_get_value(me);
			if (me->calibration_max < next_val)
				me->calibration_max = next_val;
		}
	}

	return;
}

void analog_in_calibrate_low(analog_in *me) {

	int8_t i;
	uint16_t next_val;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->calibration_min = 1023;
	}

	for (i = 0; i < 10; i++) {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			next_val = analog_in_get_value(me);
			if (me->calibration_min > next_val)
				me->calibration_min = next_val;
		}

	}

	return;
}

uint16_t analog_in_get_calibrated_value(analog_in *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->val = analog_in_get_scaled_value(me, me->calibration_min, me->calibration_max);
	}

	return me->val;
}

uint8_t analog_in_get_calibrated_value_uint8(analog_in *me) {

	return (uint8_t)(analog_in_get_scaled_value(me, me->calibration_min, me->calibration_max) >> 2);
}

void analog_in_reset_calibration(analog_in * me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->calibration_max = 1023;
		me->calibration_min = 0;
	}

	return;
}

void poll_analog_in_array(analog_in_array *me) {

	int8_t i;
	uint8_t bit_mask = 0;

	for (i = 0; i < me->num_elements; i++)
		bit_mask |= (1 << me->elements[i]->channel);

	poll_ADC(bit_mask);

	return;
}

void poll_analog_in_array_now(analog_in_array *me) {

	int8_t i;
	uint8_t bit_mask = 0;

	for (i = 0; i < me->num_elements; i++)
		bit_mask |= (1 << me->elements[i]->channel);

	poll_ADC_now(bit_mask);

	return;
}

// array functions
void analog_in_array_get_raw_values(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			me->elements[i]->val = analog_in_get_raw_value(me->elements[i]);
		}
	}

	return;
}

void analog_in_array_get_raw_values_uint8(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		me->elements[i]->val = analog_in_get_raw_value_uint8(me->elements[i]);

	return;
}

void analog_in_array_get_values(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			me->elements[i]->val = analog_in_get_value(me->elements[i]);
		}
	}

	return;
}

void analog_in_array_get_clamped_values(analog_in_array *me, uint16_t min, uint16_t max) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			me->elements[i]->val = analog_in_get_clamped_value(me->elements[i], min, max);
		}
	}

	return;
}

void analog_in_array_get_scaled_values(analog_in_array *me, uint16_t min, uint16_t max) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			me->elements[i]->val = analog_in_get_scaled_value(me->elements[i], min, max);
		}
	}

	return;
}

void analog_in_array_calibrate_high(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		analog_in_calibrate_high(me->elements[i]);

	return;
}

void analog_in_array_calibrate_low(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		analog_in_calibrate_low(me->elements[i]);

	return;
}

void analog_in_array_reset_calibration(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		analog_in_reset_calibration(me->elements[i]);

	return;
}

void analog_in_array_get_calibrated_values(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			me->elements[i]->val = analog_in_get_calibrated_value(me->elements[i]);
		}
	}

	return;
}

void analog_in_array_get_calibrated_values_uint8(analog_in_array *me) {

	int8_t i;

	for (i = 0; i < me->num_elements; i++)
		me->elements[i]->val = analog_in_get_calibrated_value_uint8(me->elements[i]);

	return;
}

// FUNCTION SPECIFIC TO PHOTOTRANSISTOR ARRAYS: RETURNS THE POSITION, REALTIVE TO THE ARRAY, OF A REFLECTIVE OBJECT AGAINST A NON-REFLECTIVE BACKGROUND (VALUE BETWEEN 0 AND 7000)
uint16_t analog_in_array_find_edge(analog_in_array *me) {

	int8_t i;
	uint32_t numerator = 0;
	uint16_t denominator = 0;

	analog_in_array_get_calibrated_values(me);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		for (i = 0; i < me->num_elements; i++) {

			if (me->elements[i]->val > 5) {														// filters out low end noise, necessary for retaining previous position but sensor must be well calibrated
				numerator += (uint32_t)(me->elements[i]->val) * i * 1000;
				denominator += (me->elements[i]->val);
			}
		}

		if (denominator)																		// protects against div by zero error, and maintains previous valid value if there no input
			me->position = (uint16_t)(numerator / denominator);
	}

	return me->position;
}