#include <avr/io.h>

/**
 * Engine control.
 *
 * This module owns Timer0.
 */

void esc_engine_init() {

	TCCR0A =
		0 * _BV(COM0A1) |
		0 * _BV(COM0A0) |
		0 * _BV(COM0B1) |
		0 * _BV(COM0B0) |
		1 * _BV(WGM01) |
		0 * _BV(WGM00);

    OCR0A = 99;

	// Timer clock is Fosc/1
	TCCR0B =
		0 * _BV(FOC0A) |
		0 * _BV(FOC0B) |
		0 * _BV(WGM02) |
		0 * _BV(CS02) |
		0 * _BV(CS01) |
		1 * _BV(CS00);

	// Enable 
	TIMSK0 = 
		0 * _BV(OCIE0B) |
		1 * _BV(OCIE0A) |
		0 * _BV(TOIE0);
}

void esc_engine_tick() {
//    OCR0A = 99;

    PORTC = PORTC++;
}
