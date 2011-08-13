#include "esc.h"
#include "esc_spi.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

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

void spi_send(char cData)
{
    SPDR = cData;
    PORTC = 1 << 4;
    PORTC = 1 << 4;
    PORTC = 0x00;
    while(!(SPSR & (1<<SPIF))) {
//        PORTC = 1 << 4;
//        PORTC = 0x00;
    }

    PORTC = 1 << 2;
    PORTC = 1 << 2;
    PORTC = 1 << 2;
    PORTC = 1 << 2;
    PORTC = 0x00;
}

int main()
{
	// this tell simavr to put the UART in loopback mode
//	GPIOR0 = SIMAVR_CMD_UART_LOOPBACK;

	// this tells simavr to start the trace
	GPIOR0 = SIMAVR_CMD_VCD_START_TRACE;

    esc_spi_init();

    spi_send(0xa5);

    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 0x00;

    spi_send(0x5a);

    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 1 << 5;
    PORTC = 0x00;

    cli();
	GPIOR0 = SIMAVR_CMD_VCD_STOP_TRACE;
    sleep_cpu();
}
