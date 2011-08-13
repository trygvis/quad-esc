include Makefile.common

AVR_PART=atmega48
AVR_CFLAGS=\
	-Wall -gdwarf-2 -Os -std=gnu99 \
	-mmcu=$(AVR_PART) \
	-DSIMAVR_AVR_MCU='"$(AVR_PART)"' \
	-DF_CPU=8000000 \
	-fno-inline-small-functions \
	-ffunction-sections -fdata-sections \
	-Wl,--relax,--gc-sections \
	-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \
	-I$(SIMAVR)/include

AVR_CC=avr-gcc
AVR_OBJCOPY=avr-objcopy

CFLAGS=\
	--std=c99 \
	-DSIMAVR_AVR_MCU=atmega48 \
	-I$(SIMAVR)/include \
	-I$(SIMAVR)/simavr/sim

LDFLAGS=\
	-lsimavr \
	-L$(SIMAVR)/simavr/$(OBJ)

OBJ = obj-${shell $(CC) -dumpmachine}

.PHONY: all test

all: main.axf tests

main.axf: main.c esc_spi.o.axf
#esc_spi.c esc_spi.h
esc_api.o.axf: esc_spi.h

test_1_target.axf: esc_spi.o.axf
#test_%_target.axf: test_%_target.c

tests: $(patsubst %.c,%,$(wildcard test_*_host.c))
tests: $(patsubst %.c,%.axf,$(wildcard test_*_target.c))

test: tests
	@ls test_*_host | \
		while read f; do LD_LIBRARY_PATH=$(SIMAVR)/simavr/$(OBJ) ./$$f; done

clean:
	@rm -rf $(wildcard *.axf)
	@rm -rf $(wildcard *.o)
	@rm -rf $(wildcard *.vcd)
	@rm -rf $(wildcard test_*_host)

%: %.c
	@echo CC $@
	@$(CC) \
		$(CFLAGS) \
		$(LDFLAGS) \
		-DTEST_NAME='"$(patsubst test_%_host.c,test_%,$<)"' \
		$< -o $@

%.hex: %.axf
	@$(AVR_OBJCOPY) -j .text -j .data -j .eeprom -O ihex $(<) $(@)
                                                                                         
%.s: %.axf
	@$(AVR_OBJDUMP) -j .text -j .data -j .bss -d  $(<) > $(@)

%.axf: %.c
	@echo avr-gcc $(<)
	$(AVR_CC) $(AVR_CFLAGS) $(^) -o $(@)

%.axf: %.c
	@echo avr-gcc $(<)
	@$(AVR_CC) \
		$(AVR_CFLAGS) \
		-DF_CPU=1000000 \
		-DTEST_NAME='"$(patsubst test_%_target.c,test_%,$<)"' \
		$(^) -o $(@)

%.o.axf: %.c
	@echo avr-gcc $(<)
	@$(AVR_CC) -c $(AVR_CFLAGS) $(^) -o $(@)
