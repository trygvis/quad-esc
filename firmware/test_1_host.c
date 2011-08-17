#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <unistd.h>

#include "avr_ioport.h"
#include "avr_spi.h"
#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"

// #define TEST_NAME "test_1"
#define MAX_TIME_US (10 *1000)

avr_t * avr = NULL;
avr_vcd_t vcd_file;

enum {
  RUNNING,
  SPECIAL_DEINIT,
  TIMEOUT
} next_state = RUNNING;

// static next_state next_state = 0;
/*
*/

#define fail(s, ...) _fail(__FILE__, __LINE__, s, ## __VA_ARGS__)

#define fatal(s, ...) _fail(__FILE__, __LINE__, s, ## __VA_ARGS__); exit(EXIT_FAILURE);
        
void _fail(const char *filename, int linenum, const char *fmt, ...) {
    if (filename)
        fprintf(stderr, "%s:%d: ", filename, linenum);

    fprintf(stderr, "Test " TEST_NAME " FAILED.\n");

    if (filename)
        fprintf(stderr, "%s:%d: ", filename, linenum);

    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    putc('\n', stderr);

//    finished = 1;
    _exit(1);
}

avr_cycle_count_t tests_cycle_count = 0;
static avr_cycle_count_t timeout_cb(struct avr_t *avr, avr_cycle_count_t when, void *param) {
    next_state = TIMEOUT;
    return 0;
}

static void special_deinit_cb(struct avr_t *avr) {
    next_state = SPECIAL_DEINIT;
}

uint16_t avr_run_one(avr_t *);

static int my_avr_run(avr_t * avr)
{
//    printf("avr->pc=%02x avr->cycle=% 10d\n", avr->pc, avr->cycle);
    if (avr->state == cpu_Stopped)
        return avr->state;

    uint16_t new_pc = avr->pc;

    if (avr->state == cpu_Running)
        new_pc = avr_run_one(avr);

    // if we just re-enabled the interrupts...
    // double buffer the I flag, to detect that edge
    if (avr->sreg[S_I] && !avr->i_shadow)
        avr->pending_wait++;
    avr->i_shadow = avr->sreg[S_I];

    // run the cycle timers, get the suggested sleep time
    // until the next timer is due
    avr_cycle_count_t sleep = avr_cycle_timer_process(avr);

    avr->pc = new_pc;

    if (avr->state == cpu_Sleeping) {
        if (!avr->sreg[S_I]) {
//            printf("simavr: sleeping with interrupts off, quitting gracefully\n");
            avr_terminate(avr);
            next_state = SPECIAL_DEINIT;
            return avr->state;
        }
        /*
         * try to sleep for as long as we can (?)
         */
        // uint32_t usec = avr_cycles_to_usec(avr, sleep);
        // printf("sleep usec %d cycles %d\n", usec, sleep);
        // usleep(usec);
        avr->cycle += 1 + sleep;
    }
    // Interrupt servicing might change the PC too, during 'sleep'
    if (avr->state == cpu_Running || avr->state == cpu_Sleeping)
        avr_service_interrupts(avr);

    // if we were stepping, use this state to inform remote gdb

    return avr->state;
}

int main(int argc, char *argv[])
{
    elf_firmware_t f;
    const char * fname = TEST_NAME "_target.axf";
    elf_read_firmware(fname, &f);

//    printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);

    avr = avr_make_mcu_by_name(f.mmcu);
    if (!avr) {
        fprintf(stderr, "%s: Unknown AVR '%s'\n", argv[0], f.mmcu);
        exit(1);
    }
    avr_init(avr);
    avr_load_firmware(avr, &f);

    /*
    // initialize our 'peripheral'
    button_init(avr, &button, "button");
    // "connect" the output irw of the button to the port pin of the AVR
    avr_connect_irq(
        button.irq + IRQ_BUTTON_OUT,
        avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 0));
    */

    /*
    // connect all the pins on port B to our callback
    for (int i = 0; i < 8; i++)
        avr_irq_register_notify(
            avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), i),
            pin_changed_hook, 
            NULL);
    */

    // even if not setup at startup, activate gdb if crashing
    avr->gdb_port = 1234;
    if (0) {
        //avr->state = cpu_Stopped;
        avr_gdb_init(avr);
    }

    avr_vcd_init(
            avr,
            TEST_NAME "_host.vcd",
            &vcd_file,
            100000 /* flush time in usec */);
    avr_vcd_add_signal(
            &vcd_file,
            avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'),
            IOPORT_IRQ_PIN_ALL),
            8 /* bits */ ,
            "portb" );

    avr_irq_t * i_mosi = avr_io_getirq(
            avr,
            AVR_IOCTL_SPI_GETIRQ(0),
            SPI_IRQ_OUTPUT);

    avr_vcd_add_signal(
            &vcd_file,
            avr_get_interrupt_irq(avr, 17),
            1 /* bit */,
            "SPI_INT" );
    avr_vcd_add_signal(
            &vcd_file,
            i_mosi,
            8 /* bits */,
            "MOSI" );

    avr_vcd_start(&vcd_file);

    avr->special_deinit = special_deinit_cb;
    avr_cycle_timer_register_usec(avr, MAX_TIME_US, timeout_cb, NULL);

    while (next_state == RUNNING) {
        my_avr_run(avr);
    }

    avr_cycle_count_t cycle = avr->cycle - 1;
    printf("Stopping at %lu cycles, %0.03fs\n", cycle, cycle * (1.0f / avr->frequency));

	// Stop the trace that's running from within the AVR
    if(avr->vcd) {
        avr_vcd_stop(avr->vcd);
    }
    avr_vcd_stop(&vcd_file);
    avr_vcd_close(&vcd_file);

    switch(next_state) {
    case SPECIAL_DEINIT:
        // returned from special deinit, avr stopped
        return EXIT_SUCCESS;
    case TIMEOUT:
        fail("timeout");
        return EXIT_FAILURE;
    default:
        fatal("error");
    }
}
