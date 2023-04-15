
BUILD		?= zbs29v033

#file containing main() must be first!
SOURCES         += main.c
SOURCES		+= comms.c
SOURCES		+= APDisplay.c
SOURCES		+= barcode.c
SOURCES		+= emulateTag.c

all:	#make sure it is the first target

FLAGS += -DAP_FW=1

include board/$(BUILD)/make.mk
include soc/$(SOC)/make.mk
include cpu/$(CPU)/make.mk
FLAGS += -Iboard/$(BUILD)
FLAGS += -Isoc/$(SOC)
FLAGS += -Icpu/$(CPU)

SOURCES += cpu/$(CPU)/cpu.c
SOURCES += board/$(BUILD)/board.c
SOURCES += board/$(BUILD)/screen.c
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
