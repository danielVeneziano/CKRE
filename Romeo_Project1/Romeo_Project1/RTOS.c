
#include "RTOS.h"

// GLOBAL VARIABLE TO PREVENT COLLISION OF RTOS TASKS
volatile uint8_t RTOS_running = 0;

time_event *NEW_time_event(void) {

	time_event *me;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me = malloc(sizeof(*me));
	}

#if _RTOS_DEBUG_
	// insert checks here
#endif

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->period = 0;
		me->enable = 0;
	}

	return me;
}

void initialize_time_event(time_event *me, uint16_t new_period) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		me->tick = 0;
		me->period = new_period;
	}
	me->enable = 1;
	me->flag = 0;

	return;
}

void destructor_time_event(time_event *me) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		free(me);
	}

	return;
}

void disable_time_event(time_event *me) {

	me->enable = 0;
	me->flag = 0;

	return;
}