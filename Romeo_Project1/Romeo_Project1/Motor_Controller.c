#include "Motor_Controller.h"

mtr_controller *NEW_mtr_cntrl(void) {

	mtr_controller *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
		me->drive = malloc(sizeof(*me->drive));
	}

#if _MTR_CNTRL_DEBUG_
	// insert checks here
#endif

	return me;
}

void initialize_mtr_cntrl(mtr_controller *me) {

	me->drive = NEW_mtr_drv();
	initialize_mtr_drv(me->drive);

	return;
}

void destructor_mtr_cntrl(mtr_controller *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		destructor_mtr_drv(me->drive);
		free(me);
	}

	return;
}

uint8_t speed_correction(uint8_t speed) {

	return (uint8_t)(((uint16_t)speed * SPEED_SCALE) >> 7);
}

void drive_forward(mtr_controller *me, uint8_t speed) {

	if (speed < MIN_SPEED) {
		speed = MIN_SPEED;
	}
	else if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	left_motor_control(me->drive, speed, FWD);
	right_motor_control(me->drive, speed_correction(speed), FWD);
}

void drive_backward(mtr_controller *me, uint8_t speed) {

	if (speed < MIN_SPEED) {
		speed = MIN_SPEED;
	}
	else if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	left_motor_control(me->drive, speed, BKWD);
	right_motor_control(me->drive, speed_correction(speed), BKWD);
}

void turn_right(mtr_controller *me) {

	left_motor_control(me->drive, TURN_SPEED, FWD);
	right_motor_control(me->drive, speed_correction(TURN_SPEED), BKWD);
}

void turn_left(mtr_controller *me) {

	left_motor_control(me->drive, TURN_SPEED, BKWD);
	right_motor_control(me->drive, speed_correction(TURN_SPEED), FWD);
}

void stop(mtr_controller *me) {

	left_motor_control(me->drive, 0, FWD);
	right_motor_control(me->drive, 0, FWD);
}

uint8_t drive_forward_ms(mtr_controller *me, uint8_t speed, time_event *t_me, uint16_t time) {

	uint16_t ramp_speed = 0;

	if (speed < MIN_SPEED) {
		speed = MIN_SPEED;
	}
	else if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	if (!t_me->enable || t_me->flag) {
		initialize_time_event(t_me, time);
		left_motor_control(me->drive, MIN_SPEED, FWD);
		right_motor_control(me->drive, MAX_SPEED, FWD);
	}
	else if ((t_me->tick > START_OFFSET) && (t_me->tick < RAMP_TIME) && (t_me->tick < (t_me->period - 10))) {
		ramp_speed = (t_me->tick*(uint16_t)speed) / RAMP_TIME;
		left_motor_control(me->drive, (uint8_t)ramp_speed, FWD);
		right_motor_control(me->drive, speed_correction((uint8_t)ramp_speed), FWD);
	}
	else if (t_me->tick > START_OFFSET) {
		left_motor_control(me->drive, speed, FWD);
		right_motor_control(me->drive, speed_correction(speed), FWD);
		return TRUE;
	}

	return FALSE;
}

uint8_t drive_backward_ms(mtr_controller *me, uint8_t speed, time_event *t_me, uint16_t time) {

	uint16_t ramp_speed = 0;

	if (speed < MIN_SPEED) {
		speed = MIN_SPEED;
	}
	else if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	if (!t_me->enable || t_me->flag) {
		initialize_time_event(t_me, time);
		left_motor_control(me->drive, 10, BKWD);
		right_motor_control(me->drive, 10, BKWD);
	}
	else if ((t_me->tick > 10) && (t_me->tick < RAMP_TIME) && (t_me->tick < (t_me->period - 10))) {
		ramp_speed = (t_me->tick*speed) / RAMP_TIME;
		left_motor_control(me->drive, (uint8_t)ramp_speed, FWD);
		right_motor_control(me->drive, speed_correction((uint8_t)ramp_speed), FWD);
	}
	else {
		left_motor_control(me->drive, speed, FWD);
		right_motor_control(me->drive, speed_correction(speed), FWD);		return TRUE;
	}

	return FALSE;
}

uint8_t turn_right_ms(mtr_controller *me, time_event *t_me, uint16_t time) {

	if (!t_me->enable || t_me->flag) {
		initialize_time_event(t_me, time);
		right_motor_control(me->drive, speed_correction(TURN_SPEED), BKWD);
	}
	else if (t_me->tick >= 20) {
		left_motor_control(me->drive, TURN_SPEED, FWD);
		return TRUE;
	}

	return FALSE;
}

uint8_t turn_left_ms(mtr_controller *me, time_event *t_me, uint16_t time) {

	if (!t_me->enable || t_me->flag) {
		initialize_time_event(t_me, time);
		right_motor_control(me->drive, speed_correction(TURN_SPEED), FWD);
	}
	else if (t_me->tick >= 20) {
		left_motor_control(me->drive, TURN_SPEED, BKWD);
		return TRUE;
	}

	return FALSE;
}

void P_Control(mtr_controller *me, uint16_t err, uint8_t dir) {

	if (dir == LEFT) {
		right_motor_control(me->drive, speed_correction(P_CNTRL_SPEED), FWD);
		left_motor_control(me->drive, P_CNTRL_SPEED - err*Kp, FWD);
	}
	else {
		right_motor_control(me->drive, speed_correction(P_CNTRL_SPEED) - err*Kp, FWD);
		left_motor_control(me->drive, P_CNTRL_SPEED, FWD);
	}

	return;
}
