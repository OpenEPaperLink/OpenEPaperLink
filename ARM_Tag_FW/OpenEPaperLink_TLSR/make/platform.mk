# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/platform/chip_8258 \
/platform/chip_8258/flash \
/platform/services/b85m
 
 
OBJS += \
$(OUT_PATH)/platform/chip_8258/flash.o \
$(OUT_PATH)/platform/chip_8258/adc.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid011460c8.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid1060c8.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid13325e.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid134051.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid136085.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid1360c8.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid1360eb.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid14325e.o \
$(OUT_PATH)/platform/chip_8258/flash/flash_mid1460c8.o \
$(OUT_PATH)/platform/services/b85m/irq_handler.o 



# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/platform/%.o: $(SDK_PATH)/platform/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

