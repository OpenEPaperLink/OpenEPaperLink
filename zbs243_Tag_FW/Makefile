
BUILD		?= zbs29_ssd1619

#file containing main() must be first!
SOURCES		+= main.c eeprom.c drawing.c
#SOURCES		+= ccm.c comms.c
SOURCES		+= comms.c
SOURCES		+= syncedproto.c userinterface.c
SOURCES		+= powermgt.c barcode.c i2cdevices.c settings.c



all:	#make sure it is the first target

include board/$(BUILD)/make.mk
include soc/$(SOC)/make.mk
include cpu/$(CPU)/make.mk
FLAGS += -Iboard/$(BUILD)
FLAGS += -Isoc/$(SOC)
FLAGS += -Icpu/$(CPU)

SOURCES += cpu/$(CPU)/cpu.c
SOURCES += board/$(BUILD)/board.c
SOURCES += board/$(BUILD)/screen.c

SOURCES += md5.c


EEPROMDRV ?= eeprom.c



SOURCES += $(EEPROMDRV)
FLAGS += -I.


OBJS = $(patsubst %.c,%.$(OBJFILEEXT),$(SOURCES))

all: $(TARGETS)


%.$(OBJFILEEXT): %.c
	$(CC) -c $^ $(FLAGS) -o $@

main.ihx: $(OBJS)
	rm -f $@
	$(CC) $^ $(FLAGS)

main.elf: $(OBJS)
	$(CC) $(FLAGS) -o $@ $^

%.bin: %.ihx
	objcopy $^ $@ -I ihex -O binary

.PHONY: clean

SOURCES += board/zbs29_ssd1619/screen.c
SOURCES += board/zbs42_ssd1619/screen.c
SOURCES += board/zbs154_ssd1619/screen.c
SOURCES += board/zbs16_ssd1619/screen.c
SOURCES += board/zbs29_uc8151/screen.c
SOURCES += board/zbs27_ucvar/screen.c


clean:
	rm -f $(patsubst %.c,%.asm,$(SOURCES))
	rm -f $(patsubst %.c,%.lst,$(SOURCES))
	rm -f $(patsubst %.c,%.rst,$(SOURCES))
	rm -f $(patsubst %.c,%.sym,$(SOURCES))
	rm -f $(patsubst %.c,%.map,$(SOURCES))
	rm -f $(patsubst %.c,%.mem,$(SOURCES))
	rm -f $(patsubst %.c,%.ihx,$(SOURCES))
	rm -f $(patsubst %.c,%.adb,$(SOURCES))
	rm -f $(patsubst %.c,%.adb,$(SOURCES))
	rm -f $(patsubst %.c,%.omf,$(SOURCES))
	rm -f $(patsubst %.c,%.bin,$(SOURCES))
	rm -f $(OBJS)
