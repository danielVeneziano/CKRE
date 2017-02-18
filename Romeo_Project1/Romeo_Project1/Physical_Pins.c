#include "Physical_Pins.h"

// THIS IS A DEDICATED INSTANCIATED STRUCT THAT CONATINS THE PROPERTIES OF PORT B
static port_obj portB = {

	.r_ddr = &DDRB,
	.r_port = &PORTB,
	.r_pin = &PINB,
	.r_pcmsk = &PCMSK0,
	.r_pcicir = &PCICR,
	.pcie_channel = 0
};

// THIS IS A DEDICATED INSTANCIATED STRUCT THAT CONATINS THE PROPERTIES OF PORT C
static port_obj portC = {

	.r_ddr = &DDRC,
	.r_port = &PORTC,
	.r_pin = &PINC,
	.r_pcmsk = &PCMSK1,
	.r_pcicir = &PCICR,
	.pcie_channel = 1
};

// THIS IS A DEDICATED INSTANCIATED STRUCT THAT CONATINS THE PROPERTIES OF PORT D
static port_obj portD = {

	.r_ddr = &DDRD,
	.r_port = &PORTD,
	.r_pin = &PIND,
	.r_pcmsk = &PCMSK2,
	.r_pcicir = &PCICR,
	.pcie_channel = 2
};

// THIS IS A DEDICATED INSTANCIATED STRUCT THAT CONATINS THE PROPERTIES OF EACH PIN
pin_obj physical_pins[] = {

[_PD3] = { .registers = &portD, .channel = PD3, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD4] = { .registers = &portD, .channel = PD4, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_GND1] = { .registers = &portD, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_VCC1] = { .registers = NULL, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_GND2] = { .registers = NULL, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_VCC2] = { .registers = NULL, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB6] = { .registers = &portB, .channel = PB6, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB7] = { .registers = &portB, .channel = PB7, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD5] = { .registers = &portD, .channel = PD5, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD6] = { .registers = &portD, .channel = PD6, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD7] = { .registers = &portD, .channel = PD7, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB0] = { .registers = &portB, .channel = PB0, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB1] = { .registers = &portB, .channel = PB1, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB2] = { .registers = &portB, .channel = PB2, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB3] = { .registers = &portB, .channel = PB3, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB4] = { .registers = &portB, .channel = PB4, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PB5] = { .registers = &portB, .channel = PB5, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_AVCC] = { .registers = NULL, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_ADC6] = { .registers = NULL, .channel = 6, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_AREF] = { .registers = NULL, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_GND3] = { .registers = NULL, .channel = 0, .purpose = 0, .pcie = 0, .prev_state = 0, .connected = NULL },
[_ADC7] = { .registers = NULL, .channel = 7, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC0] = { .registers = &portC, .channel = PC0, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC1] = { .registers = &portC, .channel = PC1, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC2] = { .registers = &portC, .channel = PC2, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC3] = { .registers = &portC, .channel = PC3, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC4] = { .registers = &portC, .channel = PC4, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC5] = { .registers = &portC, .channel = PC5, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PC6] = { .registers = &portC, .channel = PC6, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD0] = { .registers = &portD, .channel = PD0, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD1] = { .registers = &portD, .channel = PD1, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL },
[_PD2] = { .registers = &portD, .channel = PD2, .purpose = _INPUT, .pcie = 0, .prev_state = 0, .connected = NULL }
};

void connect_PIN(pin_obj *me, uint8_t *connect_ptr) {

	if (me->connected != NULL)
		disconnect_PIN(me);

	me->connected = connect_ptr;

	*me->connected = 1;

	return;
}

void disconnect_PIN(pin_obj *me) {

	if (me->connected != NULL) {
		*me->connected = 0;
		me->connected = NULL;
	}

	return;
}

uint8_t check_connection_PIN(pin_obj *me) {

	if (me->connected != NULL)
		return *me->connected;

	return 0;
}

void assign_purpose_PIN(pin_obj *me, uint8_t purpose) {

	if (me->connected != NULL)
		disconnect_PIN(me);

	if (purpose <= _ALT2)
		me->purpose = purpose;

	return;
}

uint8_t get_purpose_PIN(pin_obj *me) {

	return me->purpose;
}

void enable_pin_change_int_PIN(pin_obj *me) {

	*me->registers->r_pcicir |= (1 << me->registers->pcie_channel);
	*me->registers->r_pcmsk |= (1 << me->channel);
	me->pcie = 1;

	return;
}

void disable_pin_change_int_PIN(pin_obj *me) {

	*me->registers->r_pcmsk &= ~(1 << me->channel);
	me->pcie = 0;

	if (!(*me->registers->r_pcmsk))
		*me->registers->r_pcicir &= ~(1 << me->registers->pcie_channel);

	return;
}