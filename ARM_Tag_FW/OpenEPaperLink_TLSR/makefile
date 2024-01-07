PROJECT_NAME := TLSR_OpenEPaperLink


ifeq ($(OS), Windows_NT)
	TC32_COMPILER_PATH := "tc32_windows\\bin\\"
	WINDOWS_PREFIX :=$(TC32_COMPILER_PATH)
	LINUX_CHMOD := 
else
	TC32_COMPILER_PATH := ./tc32_linux/bin/
	WINDOWS_PREFIX :=
	LINUX_CHMOD := chmod 777 -R ./tc32_linux/
endif

AS      = $(TC32_COMPILER_PATH)tc32-elf-as
CC      = $(TC32_COMPILER_PATH)tc32-elf-gcc
LD      = $(TC32_COMPILER_PATH)tc32-elf-ld
NM      = $(TC32_COMPILER_PATH)tc32-elf-nm
OBJCOPY = $(TC32_COMPILER_PATH)tc32-elf-objcopy
OBJDUMP = $(TC32_COMPILER_PATH)tc32-elf-objdump
ARCH	= $(TC32_COMPILER_PATH)tc32-elf-ar
SIZE	= $(TC32_COMPILER_PATH)tc32-elf-size

LIBS := -ldrivers_8258

DEVICE_TYPE = -DEND_DEVICE=1
MCU_TYPE = -DMCU_CORE_8258=1
BOOT_FLAG = -DMCU_CORE_8258 -DMCU_STARTUP_8258

SDK_PATH := ./tl_zigbee_sdk
SRC_PATH := ./src
OUT_PATH := ./out
MAKE_INCLUDES := ./make

INCLUDE_PATHS := \
-I$(SDK_PATH)/platform \
-I$(SDK_PATH)/proj/common \
-I$(SDK_PATH)/proj \
-I$(SDK_PATH)/zbhci \
-I$(SRC_PATH) \
-I./common
 

LS_FLAGS := $(SDK_PATH)/platform/boot/8258/boot_8258.link

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
-fms-extensions

GCC_FLAGS += \
$(DEVICE_TYPE) \
$(MCU_TYPE) \
-D__PROJECT_TL_SWITCH__=1

OBJ_SRCS := 
S_SRCS := 
ASM_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
FLASH_IMAGE := 
ELFS := 
OBJS := 
LST := 
SIZEDUMMY := 


RM := $(WINDOWS_PREFIX)rm -rf

# All of the sources participating in the build are defined here
-include $(MAKE_INCLUDES)/proj.mk
-include $(MAKE_INCLUDES)/platformS.mk
-include $(MAKE_INCLUDES)/div_mod.mk
-include $(MAKE_INCLUDES)/platform.mk
-include ./project.mk

# Add inputs and outputs from these tool invocations to the build variables 
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(OUT_PATH)/../$(PROJECT_NAME).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf

SIZEDUMMY += \
sizedummy \


# All Target
all: chmod_all pre-build main-build

# Main-build Target
main-build: $(ELF_FILE) secondary-outputs

# Tool invocations
$(ELF_FILE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: TC32 C Linker'
	$(LD) --gc-sections -L $(SDK_PATH)/zigbee/lib/tc32 -L $(SDK_PATH)/platform/lib -T $(LS_FLAGS) -o "$(ELF_FILE)" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	$(OBJDUMP) -x -D -l -S $(ELF_FILE)  > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '

$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	@$(OBJCOPY) -v -O binary $(ELF_FILE)  $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '


sizedummy: $(ELF_FILE)
	@echo 'Invoking: Print Size'
	@$(SIZE) -t $(ELF_FILE)
	@echo 'Finished building: $@'
	@echo ' '

clean:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(SIZEDUMMY) $(LST_FILE) $(ELF_FILE) $(BIN_FILE)
	-@echo ' '
	
pre-build:
	$(WINDOWS_PREFIX)mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
	-@echo ' '

chmod_all:
	-@$(LINUX_CHMOD)
	-@echo 'chmod_all'
	
secondary-outputs: $(BIN_FILE) $(LST_FILE) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents pre-build
.SECONDARY: main-build pre-build

