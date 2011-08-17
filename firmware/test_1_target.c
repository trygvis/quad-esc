#include "esc.h"
#include "esc_spi.h"
#include "esc_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

/*
 * This demonstrate how to use the avr_mcu_section.h file
 * The macro adds a section to the ELF file with useful
 * information for the simulator
 */
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, SIMAVR_AVR_MCU);
// tell simavr to listen to commands written in this (unused) register
AVR_MCU_SIMAVR_COMMAND(&GPIOR0);

/*
 * This small section tells simavr to generate a VCD trace dump with changes to these
 * registers.
 * Opening it with gtkwave will show you the data being pumped out into the data register
 * UDR0, and the UDRE0 bit being set, then cleared
 */
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
	{ AVR_MCU_VCD_SYMBOL("PORTC"), .what = (void*)&PORTC, },
	{ AVR_MCU_VCD_SYMBOL("DDRC"), .what = (void*)&DDRC, },

	{ AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
	{ AVR_MCU_VCD_SYMBOL("DDRB"), .what = (void*)&DDRB, },

	{ AVR_MCU_VCD_SYMBOL("UDR0"), .what = (void*)&UDR0, },
	{ AVR_MCU_VCD_SYMBOL("UDRE0"), .mask = (1 << UDRE0), .what = (void*)&UCSR0A, },
	{ AVR_MCU_VCD_SYMBOL("GPIOR1"), .what = (void*)&GPIOR1, },

	{ AVR_MCU_VCD_SYMBOL("SPCR"), .what = (void*)&SPCR, },
	{ AVR_MCU_VCD_SYMBOL("SPDR"), .what = (void*)&SPDR, },
	{ AVR_MCU_VCD_SYMBOL("SPSR"), .what = (void*)&SPSR, },
};
AVR_MCU_VCD_FILE(TEST_NAME "_target.vcd", 1);

ISR(SPI_STC_vect) {
    PORTC = 1 << 7;
}

volatile uint8_t done = 0;

ISR(TIMER0_COMPA_vect) {
    // esc_engine_tick();
    done = 1;
}

void spi_send(char cData)
{
    SPDR = cData;
    PORTC = 1 << 0;
    PORTC = 0x00;
    while(!(SPSR & (1<<SPIF))) {
    }

    PORTC = 1 << 1;
    PORTC = 0x00;
}

int main()
{
	// this tells simavr to start the trace
	GPIOR0 = SIMAVR_CMD_VCD_START_TRACE;
    PORTC=0xff;

    esc_spi_init();
    // esc_engine_init();

    PORTC=0x00;
    sei();

    spi_send(0xaa);
    spi_send(0x55);
    /*
    */

    /*
    for(int i = 0; i < 10; i++) {
        do {} while(!done);
        done = 0;
    }
    */

    cli();
    sleep_cpu();
}
