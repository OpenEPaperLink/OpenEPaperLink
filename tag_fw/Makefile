
BUILD		?= zbs29v033

#file containing main() must be first!
SOURCES		+= main.c eeprom.c drawing.c
SOURCES		+= comms.c chars.c
SOURCES		+= syncedproto.c


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


ifeq ($(BARCODE),barcode)
  FLAGS += -DBARCODE
  SOURCES += barcode.c
else ifeq ($(BARCODE),datamatrix)
 # FLAGS += -DDATAMATRIX
 # SOURCES += datamatrix.c
 #i might release this later, but for now, just use barcodes :P
  FLAGS += -DBARCODE
  SOURCES += barcode.c
else
  SOURCES += $(warning "no barcode type set")
endif


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
