# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/proj/common \
/proj/drivers \
/proj/drivers/usb \
/proj/os
 
OBJS += \
$(OUT_PATH)/proj/common/list.o \
$(OUT_PATH)/proj/common/mempool.o \
$(OUT_PATH)/proj/common/string.o \
$(OUT_PATH)/proj/common/tlPrintf.o \
$(OUT_PATH)/proj/common/utility.o \
$(OUT_PATH)/proj/drivers/drv_adc.o \
$(OUT_PATH)/proj/drivers/drv_calibration.o \
$(OUT_PATH)/proj/drivers/drv_flash.o \
$(OUT_PATH)/proj/drivers/drv_gpio.o \
$(OUT_PATH)/proj/drivers/drv_hw.o \
$(OUT_PATH)/proj/drivers/drv_i2c.o \
$(OUT_PATH)/proj/drivers/drv_keyboard.o \
$(OUT_PATH)/proj/drivers/drv_nv.o \
$(OUT_PATH)/proj/drivers/drv_pm.o \
$(OUT_PATH)/proj/drivers/drv_pwm.o \
$(OUT_PATH)/proj/drivers/drv_spi.o \
$(OUT_PATH)/proj/drivers/drv_timer.o \
$(OUT_PATH)/proj/drivers/drv_uart.o \
$(OUT_PATH)/proj/os/ev.o \
$(OUT_PATH)/proj/os/ev_buffer.o \
$(OUT_PATH)/proj/os/ev_poll.o \
$(OUT_PATH)/proj/os/ev_queue.o \
$(OUT_PATH)/proj/os/ev_timer.o 



# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/proj/%.o: $(SDK_PATH)/proj/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


