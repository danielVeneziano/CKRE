
#include "Motor_Driver.h"

mtr_drv *NEW_mtr_drv(void) {

	mtr_drv *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
		me->timer_pwm = malloc(sizeof(*me->timer_pwm));
		me->left_mtr = malloc(sizeof(*me->left_mtr));
		me->left_mtr->pin = malloc(sizeof(*me->left_mtr->pin));
		me->right_mtr = malloc(sizeof(*me->right_mtr));
		me->right_mtr->pin = malloc(sizeof(*me->right_mtr->pin));
	}

#if _MTR_DRV_DEBUG_
	// insert checks here
#endif

	me->left_mtr->speed = 0;
	me->left_mtr->dir = FWD;
	me->right_mtr->speed = 0;
	me->right_mtr->dir = FWD;

	return me;
}

void initialize_mtr_drv(mtr_drv *me) {

	me->timer_pwm = NEW_TIMER0();
	initialize_TIMER0(me->timer_pwm, "CMOA", TIMER0_CLEAR_OUTPUT, "CMOB", TIMER0_CLEAR_OUTPUT, "WGM0", TIMER0_FAST_PWM_WAVE, "CLKD", TIMER0_CLK_DIV_1, "CAEN", TIMER0_ENABLE_OPTION, "CBEN", TIMER0_ENABLE_OPTION, "END");
	dig_output_initialize(me->left_mtr->pin, LEFT_MTR_PIN);
	dig_output_initialize(me->right_mtr->pin, RIGHT_MTR_PIN);

	left_motor_control(me, 0, 0);
	right_motor_control(me, 0, 0);

	return;
}

void destructor_mtr_drv(mtr_drv *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		destructor_TIMER0(me->timer_pwm);
		free(me->timer_pwm);
		free(me->left_mtr->pin);
		free(me->left_mtr);
		free(me->right_mtr->pin);
		free(me->right_mtr);
		free(me);
	}

	return;
}

void left_motor_control(mtr_drv *me, uint8_t speed, uint8_t dir) {

	me->left_mtr->dir = dir;
	if (me->left_mtr->dir) {
		set_dig_output(me->left_mtr->pin);
	}
	else {
		clr_dig_output(me->left_mtr->pin);
	}

	me->left_mtr->speed = speed;
	update_duty_B_TIMER0(me->timer_pwm, me->left_mtr->speed);

	return;
}

void right_motor_control(mtr_drv *me, uint8_t speed, uint8_t dir) {

	me->right_mtr->dir = dir;
	if (me->right_mtr->dir) {
		set_dig_output(me->right_mtr->pin);
	}
	else {
		clr_dig_output(me->right_mtr->pin);
	}

	me->right_mtr->speed = speed;
	update_duty_A_TIMER0(me->timer_pwm, me->right_mtr->speed);

	return;
}