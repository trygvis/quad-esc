#include <stdio.h>
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

int main()
{
	// this quits the simulator, since interupts are off
	// this is a "feature" that allows running tests cases and exit
	sleep_cpu();
}

