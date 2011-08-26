#include "FreeRTOS.h"
#include "task.h"

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
 
volatile uint8_t done = 0;

static void vBlink1(void * parameters) {
    while(1) {
        PORTC |= 0x01;
        PORTC &= ~0x01;
    }
}

static void vBlink2(void * parameters) {
	done = 1;
    for(int i = 0; i < 100; i++) {
        PORTC |= 0x02;
        PORTC &= ~0x02;
    }

	done = 1;
}

const signed char blink_name[] = "blink";

volatile portBASE_TYPE fail = 0;
int main()
{
    fail = xTaskCreate(vBlink1, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);
//    fail = xTaskCreate(vBlink2, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);
//    if(xTaskCreate(vBlink2, blink_name, configMINIMAL_STACK_SIZE, NULL, 0, NULL) != pdPASS) {
//		fail = 1;
//	}

    vTaskStartScheduler();

	while(!done) {
	}

    // this quits the simulator, since interupts are off
    // this is a "feature" that allows running tests cases and exit
    sleep_cpu();
}
