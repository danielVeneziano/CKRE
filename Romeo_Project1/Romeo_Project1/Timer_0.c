
#include "Timer_0.h"

static TIMER0_registers_obj TIMER0_registers = {

	.r_cnt = &TCNT0,
	.r_cra = &TCCR0A,
	.r_crb = &TCCR0B,
	.r_compa = &OCR0A,
	.r_compb = &OCR0B,
	.r_msk = &TIMSK0,
	.r_ifr = &TIFR0
};

typedef enum INI_ARGS {

	_CMOA,
	_CMOB,
	_WGM0,
	_FOCA,
	_FOCB,
	_WGM2,
	_CLKD,
	_CIEB,
	_CIEA,
	_OVIE,
	_CNT0,
	_CNTA,
	_CNTB,
	_CAEN,
	_CBEN,
	NUM_TIMER0_ARGS
}ini_args;

static const char *ini_arg_lst[NUM_TIMER0_ARGS + 1] = {

	"CMOA",
	"CMOB",
	"WGM0",
	"FOCA",
	"FOCB",
	"WGM2",
	"CLKD",
	"CIEB",
	"CIEA",
	"OVIE",
	"CNT0",
	"CNTA",
	"CNTB",
	"CAEN",
	"CBEN",
	"END"
};

TIMER0_obj *NEW_TIMER0(void) {

	TIMER0_obj *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	}

	me->registers = &TIMER0_registers;

#if _TIMER0_DEBUG_
	// insert checks here
#endif

	// TCCR0A register members
	me->compare_out_A = TIMER0_OUTPUT_A_MODE_DEF;
	me->compare_out_B = TIMER0_OUTPUT_B_MODE_DEF;
	me->waveform_mode = TIMER0_WAVEFORM_MODE_DEF;

	// TCCR0B register members
	me->force_output_A = TIMER0_FORCE_OUT_A_DEF;
	me->force_output_B = TIMER0_FORCE_OUT_B_DEF;
	me->waveform_top_ocrA = TIMER0_TOP_OCRA_DEF;				// WGM02
	me->clock_sel = TIMER0_CLK_DIV_DEF;

	// TIMSK0 register members
	me->int_en_compB = TIMER0_COMP_B_IE_DEF;
	me->int_en_compA = TIMER0_COMP_A_IE_DEF;
	me->int_en_ovf = TIMER0_OVF_IE_DEF;

	// TIFR0 register members
	me->int_flag_compB = 0;
	me->int_flag_compA = 0;
	me->int_flag_ovf = 0;

	me->ocra_en = TIMER0_DISABLE_OPTION;
	me->ocrb_en = TIMER0_DISABLE_OPTION;

	*me->registers->r_cnt = 0;
	*me->registers->r_compa = TIMER0_OCRA_DEF;
	*me->registers->r_compb = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->tick = 0;
	}

	return me;
}

int8_t parse_ini_args_TIMER0(char *arg) {

	int8_t i;

	for (i = 0; i < NUM_TIMER0_ARGS; i++) {
		if (!strcmp(ini_arg_lst[i], arg))
			return i;
	}

	return -1;
}

void initialize_TIMER0(TIMER0_obj *me, ...) {

	int8_t jmp = 0;
	char *str;
	uint8_t val = 0;

	va_list ap;

	// DEFAULT VALUES LOADED BY NEW_TIMER0, THIS JUST UPDATES

	va_start(ap, me);

	while (jmp >= 0) {

		str = va_arg(ap, char *);

		if (strlen(str) != 4) {
			jmp = -1;
		}
		else {
			jmp = parse_ini_args_TIMER0(str);
		}

		switch (jmp) {

		case _CMOA:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 3))
				me->compare_out_A = val;
			break;
		case _CMOB:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 3))
				me->compare_out_B = val;
			break;
		case _WGM0:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 3))
				me->waveform_mode = val;
			break;
		case _FOCA:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->force_output_A = val;
			break;
		case _FOCB:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->force_output_B = val;
			break;
		case _WGM2:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->waveform_top_ocrA = val;				// WGM02
			break;
		case _CLKD:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 7))
				me->clock_sel = val;
			break;
		case _CIEB:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->int_en_compB = val;
			break;
		case _CIEA:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->int_en_compA = val;
			break;
		case _OVIE:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->int_en_ovf = val;
			break;
		case _CNT0:
			val = (uint8_t)va_arg(ap, int);
			*me->registers->r_cnt = val;
			break;
		case _CNTA:
			val = (uint8_t)va_arg(ap, int);
			*me->registers->r_compa = val;
			break;
		case _CNTB:
			val = (uint8_t)va_arg(ap, int);
			*me->registers->r_compb = val;
			break;
		case _CAEN:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ocra_en = val;
			break;
		case _CBEN:
			val = (uint8_t)va_arg(ap, int);
			if ((val >= 0) && (val <= 1))
				me->ocrb_en = val;
			break;
		default:
			//debug stuff here
			break;
		}
	}

	*me->registers->r_cra = (me->compare_out_A << COM0A0) | (me->compare_out_B << COM0B0) | (me->waveform_mode << WGM00);
	*me->registers->r_crb = (me->force_output_A << FOC0A) | (me->force_output_B << FOC0B) | (me->waveform_top_ocrA << WGM02) | (me->clock_sel << CS00);
	*me->registers->r_msk = (me->int_en_compB << OCIE0B) | (me->int_en_compA << OCIE0A) | (me->int_en_ovf << TOIE0);

	if (me->ocra_en)
		if ((me->compare_out_A) && !((me->waveform_top_ocrA == TIMER0_DISABLE_OPTION) && (me->waveform_mode != TIMER0_NORMAL_WAVE) && (me->compare_out_A == TIMER0_TOGGLE_OUTPUT))) {
			assign_purpose_PIN(&physical_pins[TIMER0_OCA_CHANNEL], _ALT1);
			*physical_pins[TIMER0_OCA_CHANNEL].registers->r_ddr |= (1 << physical_pins[TIMER0_OCA_CHANNEL].channel);
		}

	if (me->ocrb_en)
		if ((me->compare_out_B >= 2) || ((me->waveform_mode == TIMER0_NORMAL_WAVE) && (me->compare_out_B == TIMER0_TOGGLE_OUTPUT))) {
			assign_purpose_PIN(&physical_pins[TIMER0_OCB_CHANNEL], _ALT1);
			*physical_pins[TIMER0_OCB_CHANNEL].registers->r_ddr |= (1 << physical_pins[TIMER0_OCB_CHANNEL].channel);
		}

	if ((me->waveform_top_ocrA == TIMER0_ENABLE_OPTION) || (me->waveform_mode == TIMER0_CTC_WAVE))
		me->top = *me->registers->r_compa;
	else
		me->top = 255;

	va_end(ap);

	return;
}

void initialize_TIMER0_def(TIMER0_obj *me) {

	// TCCR0A register members
	me->compare_out_A = TIMER0_OUTPUT_A_MODE_DEF;
	me->compare_out_B = TIMER0_OUTPUT_B_MODE_DEF;
	me->waveform_mode = TIMER0_WAVEFORM_MODE_DEF;

	// TCCR0B register members
	me->force_output_A = TIMER0_FORCE_OUT_A_DEF;
	me->force_output_B = TIMER0_FORCE_OUT_B_DEF;
	me->waveform_top_ocrA = TIMER0_TOP_OCRA_DEF;				// WGM02
	me->clock_sel = TIMER0_CLK_DIV_DEF;

	// TIMSK0 register members
	me->int_en_compB = TIMER0_COMP_B_IE_DEF;
	me->int_en_compA = TIMER0_COMP_A_IE_DEF;
	me->int_en_ovf = TIMER0_OVF_IE_DEF;

	// TIFR0 register members
	me->int_flag_compB = 0;
	me->int_flag_compA = 0;
	me->int_flag_ovf = 0;

	me->ocra_en = TIMER0_DISABLE_OPTION;
	me->ocrb_en = TIMER0_DISABLE_OPTION;

	*me->registers->r_cra = (me->compare_out_A << COM0A0) | (me->compare_out_B << COM0B0) | (me->waveform_mode << WGM00);
	*me->registers->r_crb = (me->force_output_A << FOC0A) | (me->force_output_B << FOC0B) | (me->waveform_top_ocrA << WGM02) | (me->clock_sel << CS00);
	*me->registers->r_msk = (me->int_en_compB << OCIE0B) | (me->int_en_compA << OCIE0A) | (me->int_en_ovf << TOIE0);

	*me->registers->r_cnt = 0;
	*me->registers->r_compa = TIMER0_OCRA_DEF;
	*me->registers->r_compb = 0;

	if ((me->waveform_top_ocrA == TIMER0_ENABLE_OPTION) || (me->waveform_mode == TIMER0_CTC_WAVE))
		me->top = *me->registers->r_compa;
	else
		me->top = 255;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->tick = 0;
	}

	return;
}

void destructor_TIMER0(TIMER0_obj *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me);
	}

	return;
}

void update_duty_A_TIMER0(TIMER0_obj *me, uint16_t new_duty_cnt) {

	if (new_duty_cnt > me->top)
		new_duty_cnt = me->top;

	if ((me->waveform_top_ocrA == TIMER0_DISABLE_OPTION) && (me->waveform_mode != TIMER0_CTC_WAVE))
		*me->registers->r_compa = new_duty_cnt;

	return;
}

void update_duty_B_TIMER0(TIMER0_obj *me, uint16_t new_duty_cnt) {

	if (new_duty_cnt > me->top)
		new_duty_cnt = me->top;

	*me->registers->r_compb = new_duty_cnt;

	return;
}

// Updates the period (frequency) if in an applicable mode and keeps the duty cycle as similar as possible
void update_period_TIMER0(TIMER0_obj *me, uint16_t new_top) {

	uint16_t updated_duty_cnt = 0;
	uint8_t updated_compB = 0;

	if ((me->waveform_top_ocrA == TIMER0_ENABLE_OPTION) || (me->waveform_mode == TIMER0_CTC_WAVE)) {

		if (new_top < 3)												// min resolution of timer1 is 2 (top = 3), also prevent div 0 error below
			new_top = 3;

		updated_duty_cnt = (*me->registers->r_compb) * new_top;
		updated_compB = (uint8_t)(updated_duty_cnt / me->top);

		me->top = new_top;

		*me->registers->r_compa = new_top;

		update_duty_B_TIMER0(me, updated_compB);
	}

	return;
}


// Updates the period (frequency) and the duty cycle simultaneously
void update_period_and_duty_TIMER0(TIMER0_obj *me, uint16_t new_top, uint16_t new_duty_cnt) {

	if ((me->waveform_top_ocrA == TIMER0_ENABLE_OPTION) || (me->waveform_mode == TIMER0_CTC_WAVE)) {

		if (new_top < 3)												// min resolution of timer1 is 2 (top = 3), also prevent div 0 error below
			new_top = 3;

		me->top = new_top;

		*me->registers->r_compa = new_top;

		update_duty_B_TIMER0(me, new_duty_cnt);
	}

	return;
}

void disable_output_TIMER0(void) {

	*physical_pins[TIMER0_OCA_CHANNEL].registers->r_ddr &= ~(1 << physical_pins[TIMER0_OCA_CHANNEL].channel);
	*physical_pins[TIMER0_OCB_CHANNEL].registers->r_ddr &= ~(1 << physical_pins[TIMER0_OCA_CHANNEL].channel);


	return;
}

uint16_t select_divisor_ms_TIMER0(uint16_t cnt) {

	// ranges based on cnt < F_CPU / 2 / 256 / 1000 / N (for each N, 1000 term for milliseconds)

	if (cnt <= 2)
		return TIMER0_CLK_DIV_64;
	else if (cnt <= 8)
		return TIMER0_CLK_DIV_256;
	else if (cnt <= 32)
		return TIMER0_CLK_DIV_1024;
	else
		return 0;
}

uint16_t select_divisor_us_TIMER0(uint16_t cnt) {

	// ranges based on cnt < F_CPU / 2 / 256 / 1000000 / N (for each N, 1000000 term for microseconds)

	if (cnt <= 32)
		return TIMER0_CLK_DIV_1;
	else if (cnt <= 255)
		return TIMER0_CLK_DIV_8;
	else
		return 0;
}

uint16_t calculate_top_ms_TIMER0(uint16_t cnt) {

	uint32_t calc = 0;
	uint8_t divisor;

	if (cnt <= 32) {

		divisor = select_divisor_ms_TIMER0(cnt);

		switch (divisor) {
		case TIMER0_CLK_DIV_64:
			calc = (F_CPU / 2 / cnt / 64) - 1;
			break;
		case TIMER0_CLK_DIV_256:
			calc = (F_CPU / 2 / cnt / 256) - 1;
			break;
		case TIMER0_CLK_DIV_1024:
			calc = (F_CPU / 2 / cnt / 1024) - 1;
			break;
		default:
			// debug stuff
			break;
		}
	}

	return (uint16_t)calc;
}

uint16_t calculate_top_us_TIMER0(uint16_t cnt) {

	uint32_t calc = 0;
	uint8_t divisor;

	// No need for an if statement to check cnt since the max allowable value is the max of a uint16_t

	divisor = select_divisor_us_TIMER0(cnt);

	switch (divisor) {
	case TIMER0_CLK_DIV_1:
		calc = (F_CPU / 2 / cnt / 1) - 1;
		break;
	case TIMER0_CLK_DIV_8:
		calc = (F_CPU / 2 / cnt / 8) - 1;
		break;
	default:
		// debug stuff
		break;
	}

	return (uint16_t)calc;
}

void update_tick_TIMER0(TIMER0_obj *me, uint16_t new_tick) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->tick = new_tick;
	}

	return;
}