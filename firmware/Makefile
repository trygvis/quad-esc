include Makefile.common

# FreeRTOS stuff
FREERTOS_PORT=ATMegaX8
FREERTOS_CFLAGS= \
	-Ifreertos/Source/include \
	-Ifreertos/Source/portable/GCC/$(FREERTOS_PORT)

AVR_PART=atmega48
F_CPU=1000000
AVR_CFLAGS=\
	-Wall -gdwarf-2 -Os -std=gnu99 \
	-mmcu=$(AVR_PART) \
	-DSIMAVR_AVR_MCU='"$(AVR_PART)"' \
	-DF_CPU=$(F_CPU) \
	-fno-inline-small-functions \
	-ffunction-sections -fdata-sections \
	-Wl,--relax,--gc-sections \
	-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \
	-I$(SIMAVR)/include \
	-I. \
	$(FREERTOS_CFLAGS)

AVR_CC=avr-gcc
AVR_LD=avr-gcc
AVR_OBJCOPY=avr-objcopy

CFLAGS=\
	--std=c99 \
	-Wall -Werror \
	-ggdb \
	-DSIMAVR_AVR_MCU=atmega48 \
	-I$(SIMAVR)/include \
	-I$(SIMAVR)/simavr/sim

LDFLAGS=\
	-lsimavr \
	-L$(SIMAVR)/simavr/$(OBJ) \
	-Wl,-rpath,$(SIMAVR)/simavr/$(OBJ)

OBJ = obj-${shell $(CC) -dumpmachine}

.PHONY: all test deps

TARGETS=main.axf tests
all: $(TARGETS)

# FREERTOS_SOURCES = $(wildcard freertos/Source/*.c)
FREERTOS_SOURCES = \
	freertos/Source/list.c \
	freertos/Source/queue.c \
	freertos/Source/tasks.c \
	freertos/Source/portable/GCC/$(FREERTOS_PORT)/port.c \
	freertos/Source/portable/MemMang/heap_1.c

FREERTOS_OBJECTS = $(patsubst %.c,%.o.axf, $(FREERTOS_SOURCES))
FREERTOS_HEADERS = \
	$(wildcard freertos/Source/include/*.h) \
	freertos/Source/portable/GCC/$(FREERTOS_PORT)/portmacro.h \
	FreeRTOSConfig.h

main.axf: main.o.axf esc_engine.o.axf esc_spi.o.axf
main.axf: $(FREERTOS_OBJECTS)

test_1_target.axf: esc_engine.o.axf esc_spi.o.axf

tests: $(patsubst %.c,%,$(wildcard test_*_host.c))
tests: $(patsubst %.c,%.axf,$(wildcard test_*_target.c))

test: tests
	@ls test_*_host | \
		while read f; do ./$$f; done

clean:
	@rm -rf $(wildcard *.axf)
	@rm -rf $(wildcard *.o)
	@rm -rf $(wildcard *.vcd)
	@rm -rf $(wildcard test_*_host)
	@rm -rf $(FREERTOS_OBJECTS)
	@find . -name \*.d|xargs rm -f

##############################################################
# Documentation

docs: mkdir-docs docs/doc2545.pdf docs/00857a.pdf docs/doc8012.pdf

docs/00857a.pdf:
	(cd docs; wget http://ww1.microchip.com/downloads/en/AppNotes/00857a.pdf)

docs/doc2545.pdf:
	(cd docs; wget http://www.atmel.com/dyn/resources/prod_documents/doc2545.pdf)

docs/doc8012.pdf:
	(cd docs; wget http://atmel.com/dyn/resources/prod_documents/doc8012.pdf)

##############################################################
# Build targets

mkdir-%:
	@mkdir -p $(patsubst mkdir-%,%,$@)

%: %.c
	@echo "  HOST: CC $@"
	@$(CC) \
		$(CFLAGS) \
		$(LDFLAGS) \
		-DTEST_NAME='"$(patsubst test_%_host.c,test_%,$<)"' \
		$< -MD -o $@

%.hex: %.axf
	@$(AVR_OBJCOPY) -j .text -j .data -j .eeprom -O ihex $(<) $(@)
                                                                                         
%.s: %.axf
	@$(AVR_OBJDUMP) -j .text -j .data -j .bss -d  $(<) > $(@)

%.axf: %.o.axf
	@echo "TARGET: LD" -o $(@)
	@$(AVR_LD) \
		$(AVR_CFLAGS) \
		-Wl,-Map=$(basename $@).map \
		$(^) -MD -o $(@)

%.o.axf: %.c
	@echo "TARGET: CC" -o $(@) $(<)
	@$(AVR_CC) \
		$(AVR_CFLAGS) \
		$(filter -DTEST_NAME=%,$(patsubst test_%_target.c,-DTEST_NAME='"test_%"',$<)) \
		-MD -c $(<) -o $(@)

-include $(patsubst %.c,%.o.d,$(FREERTOS_SOURCES))
-include $(patsubst %.c,%.o.d,$(wildcard *.c))
