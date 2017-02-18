
#include "FSM.h"

// ARRAY OF ALL POSSIBLE STATE TRANSITIONS BASED ON CURRENT STATE AND POSSIBLE EVENTS AND WHETHER TO CLEAR EVENT
static int8_t const transitions[PREEMPT + 1][num_events][num_states] = {

	// TRANSISTIONS
	{					//	_wander			_obstcl_f		_obstcl_r		_obstcl_l		_spotlight		_lights_off		_found_line		_follow_line
		[_CONTINUE]		= { _no_change,		_no_change,		_no_change,		_no_change,		_no_change,		_wander,		_no_change,		_no_change	},
		[_OBSTCL_F]		= { _obstcl_f,		_no_change,		_obstcl_f,		_obstcl_f,		_obstcl_f,		_obstcl_f,		_obstcl_f,		_obstcl_f	},
		[_OBSTCL_R]		= { _obstcl_r,		_obstcl_r,		_no_change,		_obstcl_r,		_obstcl_r,		_obstcl_r,		_obstcl_r,		_obstcl_r	},
		[_OBSTCL_L]		= { _obstcl_l,		_obstcl_l,		_obstcl_l,		_no_change,		_obstcl_l,		_obstcl_l,		_obstcl_l,		_obstcl_l	},
		[_SPOTLIGHT]	= { _spotlight,		_no_change,		_no_change,		_no_change,		_no_change,		_spotlight,		_spotlight,		_spotlight	},
		[_LIGHTS_OFF]	= { _lights_off,	_no_change,		_no_change,		_no_change,		_no_change,		_no_change,		_lights_off,	_lights_off },
		[_FOUND_LINE]	= { _found_line,	_no_change,		_no_change,		_no_change,		_no_change,		_no_change,		_no_change,		_no_change	},
		[_TRUE]			= { _no_change,		_wander,		_wander,		_wander,		_wander,		_wander,		_follow_line,	_wander		}
	},
	// PREEMPTIONS (IF A PARTICULAR EVENT OCCURS IN A PARTICULAR STATE, "NO" => DON'T CLEAR, SAVE THE EVENT FOR LATER)
	{					//	_wander	_obstcl_f	_obstcl_r	_obstcl_l	_spotlight	_lights_off	_found_line	_follow_line
		[_CONTINUE]		= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_OBSTCL_F]		= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_OBSTCL_R]		= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_OBSTCL_L]		= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_SPOTLIGHT]	= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_LIGHTS_OFF]	= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_FOUND_LINE]	= { YES,	YES,		YES,		YES,		YES,		YES,		YES,		YES },
		[_TRUE]			= { NO,		NO,			NO,			NO,			NO,			NO,			NO,			NO	}
	}
};

// RETURNS THE APPROPRIATE STATE CHANGE IF REQUIRED
uint8_t FSM_transition(uint8_t state_id, uint8_t event_id) {

	 int8_t val;

	 val = transitions[TRANSITION][event_id][state_id];

	 if (val == _no_change)
		 return state_id;
	 else
		 return (uint8_t)val;
}

uint8_t FSM_preempt(uint8_t state_id, uint8_t event_id) {

	return transitions[PREEMPT][event_id][state_id];
}