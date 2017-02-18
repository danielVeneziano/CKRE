
#include "Main.h"

#if _MAIN_DEBUG_
	static USART_obj *usart_obj_main;
#endif

static TIMER2_obj *timer2_main;															// Timer2 Object containing relevant settings and registers (see Timer0.h/.c)
static time_event *t_event_adc, *t_event_mc;											// a timed event (distinct from FSM events), triggers after so many ticks (see Timer0.h/.c)													// a timed event (distinct from FSM events), triggers after so many ticks (see Timer0.h/.c)
static mtr_controller *mtr_cntrl_main;
static dig_input *bumper_left, *bumper_front_l, *bumper_front_r, *bumper_right;
static ADC_obj *adc_obj_main;
static analog_in *prestr_main;															// a potentiometer for program c (see Potentiometer.h/.c)
static analog_in_array *ptstr_array_main;
static analog_in *ptstr_edge_left, *ptstr_edge_right;

static volatile uint8_t /*state_ids*/ state;											// state used by finite state machine
static uint8_t(*state_func[num_states])(void);											// array of function pointers used by finite state machine
static volatile uint8_t events[num_events];												// array of events used by finite state machine
static volatile uint8_t event_count;

static int16_t line_follow_error;
static uint8_t line_follow_dir;

// MAIN
int main(void) {

	program_setup();
	program_loop();
	program_end();
	return FALSE;
}

// SETUP OF STATIC GLOBAL VARIABLES, BOARD HARDWARE AND IO
void program_setup(void) {

#if _MAIN_DEBUG_
	usart_obj_main = NEW_USART();														// USART Object containing relevant settings and registers
	initialize_USART(usart_obj_main, /*"RXIE", USART_ENABLE_OPTION, "TXIE", USART_ENABLE_OPTION,*/ "END");									// setup USART, to do: make function variadic and properties structure
#endif

	timer2_main = NEW_TIMER2();
	initialize_TIMER2(timer2_main, "CIEA", TIMER2_ENABLE_OPTION, "END");

	t_event_adc = NEW_time_event();

	t_event_mc = NEW_time_event();

	mtr_cntrl_main = NEW_mtr_cntrl();
	initialize_mtr_cntrl(mtr_cntrl_main);

	bumper_left = NEW_dig_input();
	bumper_front_l = NEW_dig_input();															// Structure: Momentary Push Button (mpb) inputs for programs a & b
	bumper_front_r = NEW_dig_input();															// Structure: Momentary Push Button (mpb) inputs for programs a & b
	bumper_right = NEW_dig_input();
	dig_input_initialize(bumper_left, _PD2);
	dig_input_initialize(bumper_front_l, _PD3);
	dig_input_initialize(bumper_front_r, _PB3);
	dig_input_initialize(bumper_right, _PB1);

	adc_obj_main = NEW_ADC();																	// ADC Object containing relevant settings and registers
	initialize_ADC(adc_obj_main, "ADIE", ENABLE_ADC_OPTION, "END");

	prestr_main = NEW_analog_in();
	analog_in_initialize(prestr_main, LIGHT_NUM_SAMPLES, _ADC6);
	ptstr_array_main->elements[0]->calibration_min = 250;			// HIGHER MORE SENSITIVE TO FLASHLIGHT AT A DISTANCE

	ptstr_array_main = NEW_analog_in_array(4);													// Structure: Array of Phototransistors (ptstr) used for programs d through h
	analog_in_initialize(ptstr_array_main->elements[0], LINE_NUM_SAMPLES, _PC4);
	ptstr_array_main->elements[0]->calibration_min = 25;
	ptstr_array_main->elements[0]->calibration_max = 680;
	analog_in_initialize(ptstr_array_main->elements[1], LINE_NUM_SAMPLES, _PC3);
	ptstr_array_main->elements[1]->calibration_min = 25;
	ptstr_array_main->elements[1]->calibration_max = 700;
	analog_in_initialize(ptstr_array_main->elements[2], LINE_NUM_SAMPLES, _PC2);
	ptstr_array_main->elements[2]->calibration_min = 25;
	ptstr_array_main->elements[2]->calibration_max = 580; // outlier
	analog_in_initialize(ptstr_array_main->elements[3], LINE_NUM_SAMPLES, _PC1);
	ptstr_array_main->elements[3]->calibration_min = 25;
	ptstr_array_main->elements[3]->calibration_max = 660;
	
	ptstr_edge_left = NEW_analog_in();
	analog_in_initialize(ptstr_edge_left, EDGE_NUM_SAMPLES, _PC0);
	ptstr_edge_left->calibration_min = 25;
	ptstr_edge_left->calibration_max = 880;

	ptstr_edge_right = NEW_analog_in();
	analog_in_initialize(ptstr_edge_right, EDGE_NUM_SAMPLES, _ADC7);
	ptstr_edge_right->calibration_min = 25;
	ptstr_edge_right->calibration_max = 880;

	state = _wander;																			// initialize to wander

	state_func[_wander] = &wander_pgm;
	state_func[_obstcl_f] = &reverse_180_pgm;
	state_func[_obstcl_l] = &reverse_90_right_pgm;
	state_func[_obstcl_r] = &reverse_90_left_pgm;
	state_func[_spotlight] = &spotlight_pgm;
	state_func[_lights_off] = &lights_off_pgm;
	state_func[_found_line] = &found_line_pgm;
	state_func[_follow_line] = &follow_line_pgm;

	for (event_count = 0; event_count < _TRUE; event_count++) {		// re-using event_count here to initialize all events to FALSE (except _TRUE)
		events[event_count] = FALSE;
	}
	events[_TRUE] = TRUE;
	event_count = 0;												// initialize event count to zero

	reset_poll_ADC();

	reset_events();

	sei();															// enable interrupts globally
	
#if _MAIN_DEBUG_
	// insert asserts and check bounds here
#endif

	return;
}

// PROGRAM LOOP: PROGRAM FLOW CONTROLLED BY SIMPLE WHILE LOOP BELOW
void program_loop(void) {

#if _MAIN_DEBUG_
	// insert checks before main loop
#endif

	disable_time_event(t_event_mc);

	for (;;) {																			// loop until state dictates a program exit
		
		if (!t_event_adc->enable || t_event_adc->flag) {
			poll_ADC(ADC_BIT_MASK);
			initialize_time_event(t_event_adc, ANALOG_DISP_TIME);
			update_events();
		}

		if ((*state_func[state])() || event_count)									// check if a state change is required based on external event of an internal FSM decision
			state = FSM_transition(state, get_event());
	}

	return;
}

// CLEANS UP ALLOCATED MEMORY BEFORE PROGRAM ENDS (PREVENT MEMORY LEAKS)
void program_end(void) {

	destructor_TIMER2(timer2_main);
	destructor_time_event(t_event_adc);
	destructor_time_event(t_event_mc);
	destructor_mtr_cntrl(mtr_cntrl_main);
	dig_input_destructor(bumper_left);
	dig_input_destructor(bumper_front_l);
	dig_input_destructor(bumper_front_r);
	dig_input_destructor(bumper_right);
	ADC_destructor(adc_obj_main);
	analog_in_array_destructor(ptstr_array_main);
	analog_in_destructor(ptstr_edge_left);
	analog_in_destructor(ptstr_edge_right);

	return;
}

uint8_t wander_pgm(void) {

	drive_forward(mtr_cntrl_main, DRV_SPEED);

	return FALSE;
}


uint8_t reverse_180_pgm(void) {

	if (!t_event_mc->enable) {
		initialize_time_event(t_event_mc, REVERSE_TIME + TURN_180_DEG_TIME);
		drive_backward(mtr_cntrl_main, DRV_SPEED);
	}
	else if (t_event_mc->flag) {
		stop(mtr_cntrl_main);
		disable_time_event(t_event_mc);
		return TRUE;
	}
	else if (t_event_mc->tick > REVERSE_TIME) {
		turn_left(mtr_cntrl_main);
	}

	return FALSE;
}

uint8_t reverse_90_left_pgm(void) {

	if (!t_event_mc->enable) {
		initialize_time_event(t_event_mc, REVERSE_TIME + TURN_90_DEG_TIME);
		drive_backward(mtr_cntrl_main, DRV_SPEED);
	}
	else if (t_event_mc->flag) {
		stop(mtr_cntrl_main);
		disable_time_event(t_event_mc);
		return TRUE;
	}
	else if (t_event_mc->tick > REVERSE_TIME) {
		turn_left(mtr_cntrl_main);
	}

	return FALSE;
}

uint8_t reverse_90_right_pgm(void) {

	if (!t_event_mc->enable) {
		initialize_time_event(t_event_mc, REVERSE_TIME + TURN_90_DEG_TIME);
		drive_backward(mtr_cntrl_main, DRV_SPEED);
	}
	else if (t_event_mc->flag) {
		stop(mtr_cntrl_main);
		disable_time_event(t_event_mc);
		return TRUE;
	}
	else if (t_event_mc->tick > REVERSE_TIME) {
		turn_right(mtr_cntrl_main);
	}

	return FALSE;
}

uint8_t spotlight_pgm(void) {

	if (!t_event_mc->enable) {
		initialize_time_event(t_event_mc, SPOTLIGHT_JUKE_TIME);
		turn_right(mtr_cntrl_main);
	}
	else if (t_event_mc->flag) {
		disable_time_event(t_event_mc);
		return TRUE;
	}
	else if (t_event_mc->tick > TURN_180_DEG_TIME) {
		drive_forward(mtr_cntrl_main, DRV_SPEED);
	}

	return FALSE;
}

uint8_t lights_off_pgm(void) {

	if (!t_event_mc->enable) {
		initialize_time_event(t_event_mc, LIGHTS_OFF_TIME);
		stop(mtr_cntrl_main);
	}
	else if (t_event_mc->flag) {
		disable_time_event(t_event_mc);
		if (events[_CONTINUE] == FALSE) {
			events[_CONTINUE] = TRUE;
			event_count++;
		}
		return TRUE;
	}

	return FALSE;
}

uint8_t found_line_pgm(void) {

	//if (!t_event_mc->enable) {
	//	initialize_time_event(t_event_mc, FIND_LINE_TIME);
	//	if (line_follow_error > 0) {
	//		line_follow_dir = RIGHT;
	//		turn_right(mtr_cntrl_main);
	//	}
	//	else {
	//		line_follow_dir = LEFT;
	//		turn_left(mtr_cntrl_main);
	//	}
	//}
	//else if (t_event_mc->flag) {
	//	disable_time_event(t_event_mc);
	//	return TRUE;
	//}
	//else {
	//	if (line_follow_dir == RIGHT) {
	//		if (line_follow_error < 0) {
	//			disable_time_event(t_event_mc);
	//			return TRUE;
	//		}
	//	}
	//	else if (line_follow_dir == LEFT) {
	//		if (line_follow_error > 0) {
	//			disable_time_event(t_event_mc);
	//			return TRUE;
	//		}
	//	}
	//}

	return TRUE;
}

uint8_t follow_line_pgm(void) {

	if (line_follow_error > 0) {
		line_follow_dir = RIGHT;
		line_follow_error = (line_follow_error >> ERROR_DIVISOR);
	}
	else {
		line_follow_dir = LEFT;
		line_follow_error = ((-1*line_follow_error) >> ERROR_DIVISOR);
	}

	//if (line_follow_error > 93)
	//	line_follow_error = 93;

	P_Control(mtr_cntrl_main, (uint8_t)line_follow_error, line_follow_dir);

	return TRUE;
}

 //IDLE FUNCTION (RETURN IMMEDIATELY)
//uint8_t idle_function(void) { return TRUE; }

void reset_events(void) {

	int8_t i;

	for (i = 0; i < _TRUE; i++)
		events[i] = 0;

	event_count = 0;

	return;
}

// RETURNS THE EVENT OF THE HIGHEST PRIORITY CURRENTLY FLAGGED
uint8_t get_event(void) {

	uint8_t i;

	for (i = 0; i < _TRUE; i++) {

		if (events[i]) {
			if (FSM_preempt(state, i)) {
				events[i] = 0;
				event_count--;
			}
			return i;
		}
	}
	return _TRUE;
}

static void update_events(void) {			// DOES NOT SET EVENTS TRIGGERED IN/BY INTERRUPTS

	uint16_t val;
	uint8_t i;

	val = analog_in_get_calibrated_value(prestr_main);

	if (val > 975) {
		if (events[_LIGHTS_OFF] == FALSE) {
			events[_LIGHTS_OFF] = TRUE;
			event_count++;
		}
	}
	else if (val < 600) {
		if (events[_SPOTLIGHT] == FALSE) {
			events[_SPOTLIGHT] = TRUE;
			event_count++;
		}
	}

	if (events[_FOUND_LINE] == FALSE) {
		for (i = 0; i < ptstr_array_main->num_elements; i++) {
			if (analog_in_get_calibrated_value(ptstr_array_main->elements[i]) > 100) {
				events[_FOUND_LINE] = TRUE;
				event_count++;
				break;
			}
		}
	}

	if (events[_FOUND_LINE] == TRUE) {
		line_follow_error = (int16_t)analog_in_array_find_edge(ptstr_array_main) - 1500;		// offset so that center is zero, hint: - (num_elements - 1) * (1000/2)
	}

	if (events[_OBSTCL_F] == FALSE) {
		if (status_dig_input(bumper_front_l) || status_dig_input(bumper_front_r)) {
			events[_OBSTCL_F] = TRUE;
			event_count++;
		}
	}

	if (events[_OBSTCL_L] == FALSE) {
		if (status_dig_input(bumper_left)) {
			events[_OBSTCL_L] = TRUE;
			event_count++;
		}
	}

	if (events[_OBSTCL_R] == FALSE) {
		if (status_dig_input(bumper_right)) {
			events[_OBSTCL_R] = TRUE;
			event_count++;
		}
	}

#if _MAIN_DEBUG_
	append_home();
	append_decimal_uint16(analog_in_get_calibrated_value(ptstr_array_main->elements[0]));
	append_string(" ");
	append_decimal_uint16(analog_in_get_calibrated_value(ptstr_array_main->elements[1]));
	append_string(" ");
	append_decimal_uint16(analog_in_get_calibrated_value(ptstr_array_main->elements[2]));
	append_string(" ");
	append_decimal_uint16(analog_in_get_calibrated_value(ptstr_array_main->elements[3]));

	//append_decimal_uint8(events[0]);
	//append_string(" ");
	//append_decimal_uint8(events[1]);
	//append_string(" ");
	//append_decimal_uint8(events[2]);
	//append_string(" ");
	//append_decimal_uint8(events[3]);
	//append_string(" ");
	//append_decimal_uint8(events[4]);
	//append_string(" ");
	//append_decimal_uint8(events[5]);
	//append_string(" ");
	//append_decimal_uint8(events[6]);
	//append_string(" ");
	//append_decimal_uint8(events[7]);
	//append_string(" ");
	//append_decimal_uint8(events[8]);
	print_USART_now();
#endif


	return;
}

// SETS RTOS TIME EVENT FLAGS AND CONTAINS A SIGNIFICANT PORTION OF THE RTOS
ISR(TIMER2_COMPA_vect) {

	TCNT2 = 0;

	if (t_event_adc->enable && !t_event_adc->flag) {

		t_event_adc->tick++;

		if (t_event_adc->tick >= t_event_adc->period) {
			t_event_adc->tick = 0;
			t_event_adc->flag = 1;
		}
	}

	if (t_event_mc->enable && !t_event_mc->flag) {

		t_event_mc->tick++;

		if (t_event_mc->tick >= t_event_mc->period) {
			t_event_mc->tick = 0;
			t_event_mc->flag = 1;
		}
	}
}
