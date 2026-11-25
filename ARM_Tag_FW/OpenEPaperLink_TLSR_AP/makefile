# OpenEPaperLink TLSR825x access point radio firmware (802.15.4, AP type 0xF8)
#
#   make                           build out/OEPL_TLSR_AP.bin (with .elf, .lst and .map beside it)
#   make AP_DEBUG=1                instrumented build, in out/debug: diagnostics as LOG frames whether or
#                                  not the S3 asks for them, with per-ping, per-join and per-check-in
#                                  detail; what goes on the air is unchanged
#   make TC32_PATH=/opt/tc32/bin/  use a tc32 toolchain elsewhere (with the trailing slash)
#   make RAMCODE_SPILL_OK=1        link even when RAM code ends just past a 256-byte boundary, at a
#                                  cost of 256 bytes of SRAM (see _ramcode_tail_ in boot_8258_ap.link)
#   make STACK_ANALYSIS=0          link once with the fixed 2048/512-byte stacks instead of sizing them
#   make PYTHON=python3            the Python 3 that runs tools/stackdepth.py
#   make size                      print the section sizes
#   make clean
#
# Toolchain: Telink's tc32-elf-gcc (GCC 4.5.1). On Linux it comes from the submodule
# ../OpenEPaperLink_TLSR/tc32_linux (a git worktree uses the main checkout's copy), on Windows from
# ../OpenEPaperLink_TLSR/tc32_windows.
#
# The image is linked twice: tools/stackdepth.py reads the first link's listing and sizes the main and
# IRQ stacks for the second (out/stack.defsym). The SRAM between .bss and the main stack is the cache
# arena, from which the firmware derives every maximum at boot (boot_8258_ap.link, README.md "RAM"),
# so RAM added anywhere shrinks the arena instead of failing the link. To see a build's arena:
#   tc32-elf-nm out/OEPL_TLSR_AP.elf | grep _cache_arena_size_
# There are no size options; the few fixed sizes are constants in the source: CACHE_BLOCKS and
# MAX_ALLOWED_MACS in src/blockcache.h, URX_RING in src/tlsr/hal_tlsr.c.

PROJECT_NAME := OEPL_TLSR_AP

ifeq ($(OS), Windows_NT)
	TC32_PATH ?= ..\\OpenEPaperLink_TLSR\\tc32_windows\\bin\\
	MKDIR = $(TC32_PATH)mkdir -p
	RM = $(TC32_PATH)rm -rf
	CAT = $(TC32_PATH)cat
	PYTHON ?= python
else
	# The tc32 toolchain submodule. A git worktree does not get submodule contents, so fall back to
	# the one in the main checkout (needs git 2.31 or later; with an older git, set TC32_PATH).
	MAIN_CHECKOUT := $(patsubst %/.git,%,$(shell git rev-parse --path-format=absolute --git-common-dir 2>/dev/null))
	TC32_PATH ?= $(firstword \
		$(dir $(wildcard ../OpenEPaperLink_TLSR/tc32_linux/bin/tc32-elf-gcc)) \
		$(dir $(wildcard $(MAIN_CHECKOUT)/ARM_Tag_FW/OpenEPaperLink_TLSR/tc32_linux/bin/tc32-elf-gcc)))
	MKDIR = mkdir -p
	RM = rm -rf
	CAT = cat
	PYTHON ?= python3
endif

CC      = $(TC32_PATH)tc32-elf-gcc
LD      = $(TC32_PATH)tc32-elf-ld
OBJCOPY = $(TC32_PATH)tc32-elf-objcopy
OBJDUMP = $(TC32_PATH)tc32-elf-objdump
SIZE    = $(TC32_PATH)tc32-elf-size

SDK_PATH := ../OpenEPaperLink_TLSR/tl_zigbee_sdk
# 1 compiles in the diagnostics (src/ap.c); 0 is the production image.
AP_DEBUG ?= 0
# AP_DEBUG changes every object, so each kind of image keeps its own: switching can't reuse the other's
OUT_PATH := $(if $(filter 1,$(AP_DEBUG)),out/debug,out)
LINK_SCRIPT := boot_8258_ap.link

# RAM code that ends just past a 256-byte boundary costs 256 bytes of SRAM for a few bytes of code,
# and boot_8258_ap.link refuses to link it (see _ramcode_tail_ there). 1 accepts the spill.
RAMCODE_SPILL_OK ?= 0
# The stack analysis needs Python 3 (standard library only). Without it the image links once with the
# fixed 2048- and 512-byte stacks of boot_8258_ap.link, which costs the arena about 1 KB, and says so.
ifeq ($(origin STACK_ANALYSIS), undefined)
  STACK_ANALYSIS := $(if $(filter ok,$(shell $(PYTHON) -c "import sys; print('ok' if sys.version_info[0] >= 3 else 'no')")),1,0)
endif

INCLUDE_PATHS := \
	-Isrc/tlsr \
	-Isrc \
	-I$(SDK_PATH)/platform \
	-I$(SDK_PATH)/proj/common \
	-I$(SDK_PATH)/proj

# as the TLSR tag firmware and the SDK library are built
GCC_FLAGS := \
	-ffunction-sections \
	-fdata-sections \
	-Wall \
	-O2 \
	-fpack-struct \
	-fshort-enums \
	-finline-small-functions \
	-std=gnu99 \
	-fshort-wchar \
	-fms-extensions \
	-DEND_DEVICE=1 \
	-DMCU_CORE_8258=1 \
	-D__PROJECT_TL_SWITCH__=1 \
	-DAP_DEBUG=$(AP_DEBUG)

BOOT_FLAG := -DMCU_CORE_8258 -DMCU_STARTUP_8258
# a .d beside each object, so that a header edit rebuilds what includes it
DEP_FLAGS := -MMD -MP

APP_SRCS := \
	src/ap.c \
	src/v2link.c \
	src/blockcache.c \
	src/tlsr/main.c \
	src/tlsr/hal_tlsr.c \
	src/tlsr/string_min.c

# the SDK sources the tag firmware builds (make/*.mk there), without the SDK's irq_handler.c.
# proj/os/ev_timer.c is left out on purpose: its 684-byte `ev_timer` pool is never used, and
# --gc-sections cannot drop it (a COMMON symbol reached from kept .text), so the two entry points
# drv_platform_init needs are stubbed in src/tlsr/hal_tlsr.c. Add the file back if an SDK timer or
# a sleep mode is ever used.
SDK_C_SRCS := \
	proj/common/list.c \
	proj/common/mempool.c \
	proj/common/tlPrintf.c \
	proj/common/utility.c \
	proj/drivers/drv_adc.c \
	proj/drivers/drv_calibration.c \
	proj/drivers/drv_flash.c \
	proj/drivers/drv_gpio.c \
	proj/drivers/drv_hw.c \
	proj/drivers/drv_i2c.c \
	proj/drivers/drv_keyboard.c \
	proj/drivers/drv_nv.c \
	proj/drivers/drv_pm.c \
	proj/drivers/drv_pwm.c \
	proj/drivers/drv_spi.c \
	proj/drivers/drv_timer.c \
	proj/drivers/drv_uart.c \
	proj/os/ev.c \
	proj/os/ev_buffer.c \
	proj/os/ev_poll.c \
	proj/os/ev_queue.c \
	platform/chip_8258/flash.c \
	platform/chip_8258/adc.c \
	platform/chip_8258/flash/flash_mid011460c8.c \
	platform/chip_8258/flash/flash_mid1060c8.c \
	platform/chip_8258/flash/flash_mid13325e.c \
	platform/chip_8258/flash/flash_mid134051.c \
	platform/chip_8258/flash/flash_mid136085.c \
	platform/chip_8258/flash/flash_mid1360c8.c \
	platform/chip_8258/flash/flash_mid1360eb.c \
	platform/chip_8258/flash/flash_mid14325e.c \
	platform/chip_8258/flash/flash_mid1460c8.c

SDK_S_SRCS := \
	platform/tc32/div_mod.S \
	platform/boot/link_cfg.S \
	platform/boot/8258/cstartup_8258.S

APP_OBJS := $(patsubst %.c,$(OUT_PATH)/%.o,$(APP_SRCS))
SDK_OBJS := $(patsubst %.c,$(OUT_PATH)/sdk/%.o,$(SDK_C_SRCS)) $(patsubst %.S,$(OUT_PATH)/sdk/%.o,$(SDK_S_SRCS))
OBJS := $(APP_OBJS) $(SDK_OBJS)

ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf
BIN_FILE := $(OUT_PATH)/$(PROJECT_NAME).bin
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
# the first link, which only the stack analysis reads, and what it found
PASS1 := $(OUT_PATH)/pass1/$(PROJECT_NAME)
STACK_DEFSYM := $(OUT_PATH)/stack.defsym
# -T last: ld reads the script where it stands on the command line, and the script's DEFINED() only
# sees a --defsym that came before it
LD_COMMON = --gc-sections --defsym=__RAMCODE_SPILL_OK__=$(RAMCODE_SPILL_OK) -L $(SDK_PATH)/platform/lib
LD_SCRIPT = -T $(LINK_SCRIPT)

ifeq ($(wildcard $(TC32_PATH)tc32-elf-gcc),)
  ifeq ($(filter clean,$(MAKECMDGOALS)),)
    $(info )
    $(info No tc32-elf-gcc found. It is a submodule of this repository:)
    $(info   git submodule update --init ARM_Tag_FW/OpenEPaperLink_TLSR/tc32_linux)
    $(info or point TC32_PATH at a copy, with the trailing slash: make TC32_PATH=~/tc32/bin/)
    $(info (no spaces in that path or in the checkout's))
    $(info )
    $(error no tc32 toolchain)
  endif
endif

all: $(BIN_FILE) $(LST_FILE) size

$(OUT_PATH)/%.o: %.c
	@$(MKDIR) $(dir $@)
	@echo 'CC $<'
	@$(CC) $(GCC_FLAGS) $(DEP_FLAGS) $(INCLUDE_PATHS) -c -o "$@" "$<"

$(OUT_PATH)/sdk/%.o: $(SDK_PATH)/%.c
	@$(MKDIR) $(dir $@)
	@echo 'CC $<'
	@$(CC) $(GCC_FLAGS) $(DEP_FLAGS) $(INCLUDE_PATHS) -c -o "$@" "$<"

# The IRQ stack's bytes are reserved by the link, at the size the analysis gives it, so cstartup's own
# .lcomm of it is made empty: irq_stk becomes the label at the top of that reserve (boot_8258_ap.link).
$(OUT_PATH)/sdk/platform/boot/8258/cstartup_8258.o: BOOT_FLAG += -D__IRQ_STK_SIZE__=0

$(OUT_PATH)/sdk/%.o: $(SDK_PATH)/%.S
	@$(MKDIR) $(dir $@)
	@echo 'AS $<'
	@$(CC) $(BOOT_FLAG) $(DEP_FLAGS) $(INCLUDE_PATHS) -c -o "$@" "$<"

ifeq ($(STACK_ANALYSIS),1)
# Link 1: the fallback stacks, for the analysis to read. Its code is the final image's code: the second
# link changes only the stacks' sizes, which move .bss and the arena and no instruction.
$(PASS1).elf: $(OBJS) $(LINK_SCRIPT)
	@$(MKDIR) $(dir $@)
	@echo 'LD $@ (for the stack analysis)'
	@$(LD) $(LD_COMMON) $(LD_SCRIPT) -Map $(PASS1).map -o "$@" $(OBJS) -ldrivers_8258

$(PASS1).lst: $(PASS1).elf
	@$(OBJDUMP) -x -D $< > $@

# Fails the build, saying why, on anything that would make the bound a guess (see the tool)
$(STACK_DEFSYM): $(PASS1).lst tools/stackdepth.py
	@echo 'STACK $<'
	@$(PYTHON) tools/stackdepth.py $< --defsym $@

# Link 2, with the stacks the analysis asked for; then the analysis again on what was linked, which
# refuses the image (and deletes it, .DELETE_ON_ERROR) if either bound came out higher than the sizes.
$(ELF_FILE): $(OBJS) $(LINK_SCRIPT) $(STACK_DEFSYM)
	@echo 'LD $@ ($(shell $(CAT) $(STACK_DEFSYM)))'
	@$(LD) $(LD_COMMON) $(shell $(CAT) $(STACK_DEFSYM)) $(LD_SCRIPT) -Map $(OUT_PATH)/$(PROJECT_NAME).map -o "$@" $(OBJS) -ldrivers_8258
	@$(OBJDUMP) -x -D -l -S $@ > $(LST_FILE)
	@$(PYTHON) tools/stackdepth.py $(LST_FILE) --verify --quiet
else
$(ELF_FILE): $(OBJS) $(LINK_SCRIPT)
	@echo 'LD $@'
	@echo 'No stack analysis (no Python 3 as "$(PYTHON)", or STACK_ANALYSIS=0): the stacks keep the fixed 2048 and 512 bytes of boot_8258_ap.link, and the arena is that much smaller.'
	@$(LD) $(LD_COMMON) $(LD_SCRIPT) -Map $(OUT_PATH)/$(PROJECT_NAME).map -o "$@" $(OBJS) -ldrivers_8258
	@$(OBJDUMP) -x -D -l -S $@ > $(LST_FILE)
endif

$(BIN_FILE): $(ELF_FILE)
	@$(OBJCOPY) -O binary $< $@
	@echo 'BIN $@'

# made by the link's recipe, beside the image it lists
$(LST_FILE): $(ELF_FILE) ;

size: $(ELF_FILE)
	@$(SIZE) -t $<

clean:
	-$(RM) $(OUT_PATH)

-include $(OBJS:.o=.d)

.PHONY: all clean size
# a recipe that fails leaves no target behind - above all an image the stack check refused
.DELETE_ON_ERROR:
