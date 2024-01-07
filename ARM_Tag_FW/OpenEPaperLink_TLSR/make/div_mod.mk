# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/platform/tc32
 
OBJS += \
$(OUT_PATH)/platform/tc32/div_mod.o


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/platform/tc32/%.o: $(SDK_PATH)/platform/tc32/%.S
	@echo 'Building file: $<'
	@$(CC) $(BOOT_FLAG) $(INCLUDE_PATHS) -c -o"$@" "$<"

