# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/$(SRC_PATH) \
 
OBJS += \
$(OUT_PATH)/$(SRC_PATH)/main.o \
$(OUT_PATH)/$(SRC_PATH)/epd_spi.o \
$(OUT_PATH)/$(SRC_PATH)/epd.o \
$(OUT_PATH)/$(SRC_PATH)/epd_bw_213.o \
$(OUT_PATH)/$(SRC_PATH)/epd_bwr_213.o \
$(OUT_PATH)/$(SRC_PATH)/epd_bwr_350.o \
$(OUT_PATH)/$(SRC_PATH)/epd_bwy_350.o \
$(OUT_PATH)/$(SRC_PATH)/epd_bw_213_ice.o \
$(OUT_PATH)/$(SRC_PATH)/epd_bwr_154.o \
$(OUT_PATH)/$(SRC_PATH)/led.o \
$(OUT_PATH)/$(SRC_PATH)/i2c.o \
$(OUT_PATH)/$(SRC_PATH)/battery.o \
$(OUT_PATH)/$(SRC_PATH)/nfc.o \
$(OUT_PATH)/$(SRC_PATH)/uart.o \
$(OUT_PATH)/$(SRC_PATH)/one_bit_display.o \
$(OUT_PATH)/$(SRC_PATH)/eeprom.o \
$(OUT_PATH)/$(SRC_PATH)/zigbee.o \
$(OUT_PATH)/$(SRC_PATH)/comms.o \
$(OUT_PATH)/$(SRC_PATH)/drawing.o \
$(OUT_PATH)/$(SRC_PATH)/syncedproto.o \
$(OUT_PATH)/$(SRC_PATH)/wdt.o \
$(OUT_PATH)/$(SRC_PATH)/powermgt.o \

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/$(SRC_PATH)/%.o: $(SRC_PATH)/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


