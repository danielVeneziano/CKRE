
#include "ADC.h"

/* TO DO
	> TEST FUNCTIONS BELOW WHEN ADC IS AUTOTRIGGERED

*/

// INITIALIZE TO ADC REGISTERS
static ADC_registers_obj ADC_registers = {

	.r_mux = &ADMUX,
	.r_csa = &ADCSRA,
	.r_csb = &ADCSRB,
	.r_did = &DIDR0,
	.r_adc = &ADC,
	.r_adch = &ADCH,
	.r_adcl = &ADCL
};

// INITIALIZE ADC RESULTS OBJECT (ALL MEMBERS ZERO)
static ADC_results_obj ADC_results = { 0 };

// ENUM USED IN VARIADIC INI FUNCTION
typedef enum INI_ARGS {
	_VREF,
	_JUST,
	_ADEN,
	_ADSC,
	_TRGE,
	_ADIE,
	_PRSC,
	_TRGS,
	_DIO0,
	_DIO1,
	_DIO2,
	_DIO3,
	_DIO4,
	_DIO5,
	NUM_ADC_ARGS
}ini_args;

// STRINGS READ IN BY VARIADIC INI FUNCTION
static const char *ini_arg_lst[NUM_ADC_ARGS + 1] = {
	"AREF",
	"JUST",
	"ADEN",
	"ADSC",
	"TRGE",
	"ADIE",
	"PRSC",
	"TRGS",
	"DIO0",
	"DIO1",
	"DIO2",
	"DIO3",
	"DIO4",
	"DIO5",
	"END"
};

// CREATES A NEW INSTANCE OF AN ADC OBJECT (CALL THIS BEFORE INI)
ADC_obj *NEW_ADC(void) {

	ADC_obj *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));

		me->registers = &ADC_registers;
	}

#if _ADC_DEBUG_
	// insert checks here
#endif

	me->aref_sel = AREF_VCC;					// ADMUX register members
	me->justify = ADC_RIGHT_JUSTIFY;

	me->adc_en = ENABLE_ADC_OPTION;				// ADCSRA register members
	me->start = ENABLE_ADC_OPTION;
	me->trig_en = DISABLE_ADC_OPTION;
	me->intrpt_en = DISABLE_ADC_OPTION;
	me->presclr = ADC_PRSC_128;

	me->trig_src = ADC_TRIG_FREE;				// ADCRSB register members

	me->ddio_0 = DISABLE_ADC_OPTION;			// DIDR0 register members
	me->ddio_1 = DISABLE_ADC_OPTION;
	me->ddio_2 = DISABLE_ADC_OPTION;
	me->ddio_3 = DISABLE_ADC_OPTION;
	me->ddio_4 = DISABLE_ADC_OPTION;
	me->ddio_5 = DISABLE_ADC_OPTION;

	return me;
}

// PARSES OPTIONAL ARGUMENTS ENTERED INTO VARIADIC INI FUNCTION 
int8_t parse_ini_args_ADC(char *arg) {

	int8_t i;

	for (i = 0; i < NUM_ADC_ARGS; i++) {
		if (!strcmp(ini_arg_lst[i], arg))
			return i;
	}

	return -1;
}

// VARIADIC INI FUNCTION TO SETUP ADC OBJECT (CONTAINS ALL ADC SETUP INFO)
void initialize_ADC(ADC_obj *me, ...) {

	int8_t jmp = 0;
	char *str;
	uint8_t val = 0;

	va_list ap;

	// DEFAULT VALUES LOADED BY NEW_ADC, THIS JUST UPDATES

	va_start(ap, me);

	while (jmp >= 0) {

		str = va_arg(ap, char *);

		if (strlen(str) != 4) {
			jmp = -1;
		}
		else {
			jmp = parse_ini_args_ADC(str);
		}

		switch (jmp) {

		case _VREF:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 3))
				me->aref_sel = val;
		break;
		case _JUST:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->justify = val;
		break;
		case _ADEN:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->adc_en = val;
		break;
		case _ADSC:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->start = val;
		break;
		case _TRGE:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->trig_en = val;
			break;
		case _ADIE:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->intrpt_en = val;
		break;
		case _PRSC:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 7))
				me->presclr = val;
		break;
		case _TRGS:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 7))
				me->trig_src = val;
		break;
		case _DIO0:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ddio_0 = val;
		break;
		case _DIO1:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ddio_1 = val;
		break;
		case _DIO2:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ddio_2 = val;
		break;
		case _DIO3:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ddio_3 = val;
		break;
		case _DIO4:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ddio_4 = val;
		break;
		case _DIO5:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ddio_5 = val;
		break;
		default:
			//debug stuff here
		break;
		}
	}


	*me->registers->r_mux = (me->aref_sel << REFS0) | (me->justify << ADLAR);
	*me->registers->r_csa = (me->adc_en << ADEN) | (me->trig_en << ADATE) | (me->intrpt_en << ADIE) | (me->presclr << ADPS0);
	*me->registers->r_csb |= (me->trig_src << ADTS0);																				// OR'D BECAUSE REGISTER SHARES A BIT WITH ANALOG COMPARATOR INI
	*me->registers->r_did = (me->ddio_5 << PC5) | (me->ddio_4 << PC4) | (me->ddio_3 << PC3) | (me->ddio_2 << PC2) | (me->ddio_1 << PC1) | (me->ddio_0 << PC0);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		*me->registers->r_csa |= (me->start << ADSC);
		while (*ADC_registers.r_csa & (1 << ADSC));
		*ADC_registers.r_csa |= (1 << ADIF);
	}

	va_end(ap);

	return;
}

// SETS ALL INI TO DEFAULT (ALTERNATIVE TO VARIADIC INI FUNCTION ABOVE)
void ini_default_ADC(ADC_obj *me) {

	// LOAD DEFAULT VALUES

	me->aref_sel = AREF_VCC;					// ADMUX register members
	me->justify = ADC_RIGHT_JUSTIFY;

	me->adc_en = ENABLE_ADC_OPTION;				// ADCSRA register members
	me->start = ENABLE_ADC_OPTION;
	me->trig_en = DISABLE_ADC_OPTION;
	me->intrpt_en = DISABLE_ADC_OPTION;
	me->presclr = ADC_PRSC_128;

	me->trig_src = ADC_TRIG_FREE;				// ADCRSB register members

	me->ddio_0 = DISABLE_ADC_OPTION;			// DIDR0 register members
	me->ddio_1 = DISABLE_ADC_OPTION;
	me->ddio_2 = DISABLE_ADC_OPTION;
	me->ddio_3 = DISABLE_ADC_OPTION;
	me->ddio_4 = DISABLE_ADC_OPTION;
	me->ddio_5 = DISABLE_ADC_OPTION;

	*me->registers->r_mux = (me->aref_sel << REFS0) | (me->justify << ADLAR);
	*me->registers->r_csa = (me->adc_en << ADEN) | (me->trig_en << ADATE) | (me->intrpt_en << ADIE) | (me->presclr << ADPS0);
	*me->registers->r_csb |= (me->trig_src << ADTS0);																				// OR'D BECAUSE REGISTER SHARES A BIT WITH ANALOG COMPARATOR INI
	*me->registers->r_did = (me->ddio_5 << PC5) | (me->ddio_4 << PC4) | (me->ddio_3 << PC3) | (me->ddio_2 << PC2) | (me->ddio_1 << PC1) | (me->ddio_0 << PC0);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		*me->registers->r_csa |= (me->start << ADSC);
		while (*ADC_registers.r_csa & (1 << ADSC));
		*ADC_registers.r_csa |= (1 << ADIF);
	}

	return;
}

// FREES ADC OBJECT AND PREVENT MEMORY LEAKS
void ADC_destructor(ADC_obj *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me);
	}

	return;
}

// START THE ADC OF A SPECIFIED CHANNEL
static inline void start_ADC(uint8_t channel) {

	*ADC_registers.r_mux &= ~(ADMUX_CLR);				// clears bits 0 through 3
	*ADC_registers.r_mux |= channel;					// specifies channel

	if (~(*ADC_registers.r_csa) & (1 << ADATE))			// ????????? might need to reconsider this logic, if ADC is auto triggered would you ever want to force ADC and if would writing ADSC accomplish that and if so what happens when trigger occurs ???????
		*ADC_registers.r_csa |= (1 << ADSC);

	return;
}

// FILLS THE ADC RESULTS OBJECT WITH ONE SAMPLED RESULT (USEFUL TO ELIMINATE ERRORS WHEN CALCULATING THE FIRST FEW MOVING AVERAGES)
void reset_poll_ADC(void) {

	uint8_t channel, index;

	for (channel = 0; channel < ADC_NUM_CHANNELS; channel++) {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			start_ADC(channel);
			while (*ADC_registers.r_csa & (1 << ADSC));								// wait for conversion to finish
			*ADC_registers.r_csa |= (1 << ADIF);
		}

		for (index = 0; index < ADC_NUM_SAMPLES; index++) {

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				ADC_results.values[channel][index] = *ADC_registers.r_adc;
			}
		}
	}

	ADC_results.bit_mask = 0;
	ADC_results.index = 0;

	return;
}

// CLEARS ALL PREVIOUS RESULTS SAVED IN THE ADC RESULTS OBJECT
void clear_poll_ADC(void) {

	uint8_t channel, index;

	for (channel = 0; channel < ADC_NUM_CHANNELS; channel++) {

		for (index = 0; index < ADC_NUM_SAMPLES; index++) {

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				ADC_results.values[channel][index] = 0;	
			}	
		}
	}

	ADC_results.bit_mask = 0;
	ADC_results.index = 0;
	RTOS_running = 0;

	return;
}

// UPDATE THE VALUE FROM A SPECIFIC CHANNEL SAVED INTO THE ADC RESULTS OBJECT - NON-BLOCKING VERSION (NOTE WILL INCREMENT THE RESULTS INDEX SO SHOULD NOT BE USED IF MULTIPLE CHANNELS NEED TO BE AVERAGED SIMULTANEOUSLY)
void poll_ADC_channel(uint8_t channel) {

	ADC_results.channel = channel;

	ADC_results.bit_mask = (1 << channel);

	RTOS_running = 1;

	start_ADC(channel);

	return;
}

// UPDATES THE VALUES SAVED INTO THE ADC RESULTS OBJECT - NON-BLOCKING VERSION
void poll_ADC(uint8_t bit_mask) {

	ADC_results.bit_mask = bit_mask;

	ADC_results.channel = 0;

	RTOS_running = 1;

	while (ADC_results.channel < ADC_NUM_CHANNELS) {

		if (ADC_results.bit_mask & (1 << ADC_results.channel)) {

			start_ADC(ADC_results.channel);

			break;
		}

		ADC_results.channel++;
	}

	return;
}

// UPDATE THE VALUE FROM A SPECIFIC CHANNEL SAVED INTO THE ADC RESULTS OBJECT - NON-BLOCKING VERSION (NOTE WILL INCREMENT THE RESULTS INDEX SO SHOULD NOT BE USED IF MULTIPLE CHANNELS NEED TO BE AVERAGED SIMULTANEOUSLY)
void poll_ADC_channel_now(uint8_t channel) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		start_ADC(channel);
		while (*ADC_registers.r_csa & (1 << ADSC));									// wait for conversion to finish
		*ADC_registers.r_csa |= (1 << ADIF);

		ADC_results.values[channel][ADC_results.index] = *ADC_registers.r_adc;		// note that ADC could be left justified but I assume if someone set it up that way then they want their result that way too
	}

	ADC_results.index++;

	return;
}

// UPDATES THE VALUES SAVED INTO THE ADC RESULTS OBJECT - BLOCKING VERSION
void poll_ADC_now(uint8_t bit_mask) {

	uint8_t channel;

	for (channel = 0; channel < ADC_NUM_CHANNELS; channel++) {

		if (bit_mask & (1 << channel)) {

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				start_ADC(channel);
				while (*ADC_registers.r_csa & (1 << ADSC));									// wait for conversion to finish
				*ADC_registers.r_csa |= (1 << ADIF);

				ADC_results.values[channel][ADC_results.index] = *ADC_registers.r_adc;		// note that ADC could be left justified but I assume if someone set it up that way then they want their result that way too
			}
		}
	}

	ADC_results.index++;

	if (!(ADC_results.index < ADC_NUM_SAMPLES))
		ADC_results.index = 0;

	return;
}

// RETURNS THE PREVIOUS ADC RESULT OF A SPECIFIC CHANNEL
uint16_t get_raw_ADC(uint8_t channel) {

	uint16_t result;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = ADC_results.values[channel][ADC_results.index];
	}

	return result;
}

// UPDATES AND RETURNS A NEW ADC RESULT OF A SPECIFIC CHANNEL - BLOCKING
uint16_t get_raw_ADC_now(uint8_t channel) {										

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		start_ADC(channel);
		while (*ADC_registers.r_csa & (1 << ADSC));								// wait for conversion to finish
		*ADC_registers.r_csa |= (1 << ADIF);
	}
	
	return *ADC_registers.r_adc;												// note that ADC could be left justified but I assume if someone set it up that way then they want their result that way too
}

// CALCULATES A MOVING AVERAGE OF THE PREVIOUS 'num_samples' OF A SPECIFIC CHANNEL
uint16_t get_mov_avg_ADC(uint8_t channel, uint8_t num_samples) {				

	int8_t i;

	uint16_t result = 0;													// Because ADC result is only 10 bit the arithmetic below can fit in a 16 bit integer (up to 8 samples)

	if ((num_samples == 0) || (num_samples > ADC_NUM_SAMPLES)) {
		return 0;
	}

	for (i = 0; i < num_samples; i++) {		// This loop sums the ADC_result values starting with the most recently updated index moving backward. If i exceeds the index the sum continues backwards from the max number of samples (more recently updated).

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (i <= ADC_results.index) 		
				result += ADC_results.values[channel][ADC_results.index - i];
			else
				result += ADC_results.values[channel][ADC_NUM_SAMPLES - i];
		}
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = (( (result << 3)/num_samples ) + (1 << 2)) >> 3;				// maximum in process value is 2^16 - 7
	}

	return result;
}

// RETURNS A CLAMPED VALUE OF A MOVING AVERAGE OF THE PREVIOUS 'num_samples' A SPECIFIED CHANNEL 
uint16_t get_clamped_value_ADC(uint8_t channel, uint16_t min, uint16_t max, uint8_t num_samples) {

	uint16_t result = 0;

	if ((max > min) && (max < 1024)) {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			result = get_mov_avg_ADC(channel, num_samples);
	
			if (result > max)
				result = max;
			else if (result < min)
				result = min;
		}
	}

	return result;
}

// RETURNS A SCALED VALUE OF A MOVING AVERAGE OF THE PREVIOUS 'num_samples' A SPECIFIED CHANNEL 
uint16_t get_scaled_value_ADC(uint8_t channel, uint16_t min, uint16_t max, uint8_t num_samples) {

	uint32_t result;

	if ((max > min) && (max < 1024)) {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			result = (uint32_t)get_clamped_value_ADC(channel, min, max, num_samples);
			result = ((1000 * (result - min)) / (max - min));
		}

		return (uint16_t)result;
	}

	return 0;
}

// UPDATES AND RETURNS A NEW 8-BIT ADC RESULT OF A SPECIFIC CHANNEL - BLOCKING
uint8_t get_raw_ADC_uint8_now(uint8_t channel) {

	uint8_t result;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		start_ADC(channel);
		while (*ADC_registers.r_csa & (1 << ADSC));				// wait for conversion to finish
		*ADC_registers.r_csa |= (1 << ADIF);
	}

	if (*ADC_registers.r_mux & (1 << ADLAR)) {
		result = *ADC_registers.r_adch;							// return the 8 bit MSB
	}
	else {

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			result = (uint8_t)(*ADC_registers.r_adc >> 2);		// shift MSB into ADCL (right shift 2 bits)
		}
	}

	return result;
}

// THIS ISR IS PART OF THE NON-BLOCKING POLLING FUNCTIONS, UPDATES THE VALUES IN THE ADC RESULTS OBJECT OF THE CHANNELS ACCORDING TO THE CURRENT MASK
ISR(ADC_vect) {

#if _ADC_DEBUG_
	// Insert checks here
#endif

	if (ADC_results.bit_mask & (1 << ADC_results.channel)) {
		ADC_results.values[ADC_results.channel][ADC_results.index] = *ADC_registers.r_adc;		// note that ADC could be left justified but I assume if someone set it up that way then they want their result that way too
	}

	ADC_results.channel++;

	while (ADC_results.channel < ADC_NUM_CHANNELS) {

		if (ADC_results.bit_mask & (1 << ADC_results.channel)) {

			*ADC_registers.r_mux &= ~(ADMUX_CLR);				// clears bits 0 through 3
			*ADC_registers.r_mux |= ADC_results.channel;		// specifies channel

			*ADC_registers.r_csa |= (1 << ADSC);

			break;
		}

		ADC_results.channel++;
	}

	if (!(ADC_results.channel < ADC_NUM_CHANNELS)) {

		RTOS_running = 0;										// CLEAR THE RTOS FLAG
		ADC_results.channel = 0;
		ADC_results.bit_mask = 0;
		ADC_results.index++;
		if (!(ADC_results.index < ADC_NUM_SAMPLES))
			ADC_results.index = 0;
	}
}
