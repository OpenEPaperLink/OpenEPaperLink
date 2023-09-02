
MZ_FLASHER=python ../88MZ100_Flasher/88MZ100_Uart_flasher.py

ARMGCC=/usr/bin/
CC=$(ARMGCC)arm-none-eabi-gcc
AS=$(ARMGCC)arm-none-eabi-as
OBJCOPY=$(ARMGCC)arm-none-eabi-objcopy

#-Wall 
CC_WARNING_FLAGS=-Wall -Wformat=0 -Wattributes -Wstrict-aliasing=0
CC_FlAGS=-mcpu=cortex-m3 -g -O0 -mthumb -fdata-sections -ffunction-sections -std=c99
CC_END_FLAGS=-lc -lnosys -L. -T mz100.ld -fPIE --specs=nosys.specs -mcpu=cortex-m3 -mthumb -Wl,--gc-sections -O0 -flto -ffunction-sections -fdata-sections -DARM_GNU

C_SOURCES :=$(wildcard *.c)
C_EXECUTABLE :=$(C_SOURCES:.c=)

COMPORT = COM12
		

build: compile
only: clean compile create_ota_img
uart: clean compile flash_uart

#build: clean compile create_ota_img flash_uart_flash
#only: clean compile create_ota_img
#uart: clean compile flash_uart


compile:
	@mkdir -p build
	@$(AS) -mcpu=cortex-m3 --gdwarf-2 -mthumb-interwork -o build/startup.o startup.S	

	@$(CC) $(CC_FlAGS) -c core_cm3.c         -o build/core_cm3.o
	@$(CC) $(CC_FlAGS) -c mz100_ssp.c        -o build/mz100_ssp.o
	@$(CC) $(CC_FlAGS) -c mz100_wdt.c        -o build/mz100_wdt.o
	@$(CC) $(CC_FlAGS) -c mz100_gpio.c       -o build/mz100_gpio.o
	@$(CC) $(CC_FlAGS) -c mz100_driver.c     -o build/mz100_driver.o
	@$(CC) $(CC_FlAGS) -c mz100_adc.c        -o build/mz100_adc.o
	@$(CC) $(CC_FlAGS) -c mz100_flash.c      -o build/mz100_flash.o
	@$(CC) $(CC_FlAGS) -c mz100_clock.c      -o build/mz100_clock.o
	@$(CC) $(CC_FlAGS) -c mz100_rtc.c        -o build/mz100_rtc.o
	@$(CC) $(CC_FlAGS) -c mz100_pinmux.c     -o build/mz100_pinmux.o
	@$(CC) $(CC_FlAGS) -c mz100_pmu.c        -o build/mz100_pmu.o
	@$(CC) $(CC_FlAGS) -c mz100_qspi.c       -o build/mz100_qspi.o
	@$(CC) $(CC_FlAGS) -c mz100_aes.c        -o build/mz100_aes.o
	@$(CC) $(CC_FlAGS) -c mz100_gpt.c        -o build/mz100_gpt.o
	@$(CC) $(CC_FlAGS) -c mz100_sleep.c        -o build/mz100_sleep.o
	@$(CC) $(CC_FlAGS) -c mz100_uart.c       -o build/mz100_uart.o
	@$(CC) $(CC_FlAGS) -c mz100_aon_ram.c       -o build/mz100_aon_ram.o
	@$(CC) $(CC_FlAGS) -c printf.c		-o build/printf.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c zigbee.c        -o build/zigbee.o
#	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c ccm.c        -o build/ccm.o
#	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c chars.c        -o build/chars.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c drawing.c        -o build/drawing.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c powermgt.c        -o build/powermgt.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c syncedproto.c        -o build/syncedproto.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c comms.c        -o build/comms.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c settings.c        -o build/settings.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c timer.c        -o build/timer.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c util.c        -o build/util.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c gpio.c        -o build/gpio.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c epd.c        -o build/epd.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c userinterface.c	-o build/userinterface.o
	@$(CC) $(CC_FlAGS) $(CC_WARNING_FLAGS) -c main.c             -o build/main.o
	@$(CC) $(CC_END_FLAGS) $(CC_WARNING_FLAGS) build/main.o build/userinterface.o build/printf.o build/mz100_aon_ram.o build/zigbee.o build/drawing.o build/powermgt.o build/syncedproto.o build/comms.o build/settings.o build/timer.o build/util.o build/gpio.o build/epd.o build/mz100_sleep.o build/core_cm3.o build/mz100_ssp.o build/mz100_wdt.o build/mz100_gpio.o build/mz100_driver.o build/mz100_adc.o build/mz100_flash.o build/mz100_clock.o build/mz100_rtc.o build/mz100_pinmux.o build/mz100_pmu.o build/mz100_qspi.o build/mz100_aes.o build/mz100_gpt.o build/mz100_uart.o build/startup.o -o main.axf
	@$(OBJCOPY) -v -O binary main.axf main.bin
	
clean:
	@rm -rf build/*
	
flash_uart:
	@$(MZ_FLASHER) $(COMPORT) write main.bin
	
flash_uart_flash:
	@$(MZ_FLASHER) $(COMPORT) write_flash main.bin
	
flash_stock:
	@$(MZ_FLASHER) $(COMPORT) write_flash stock.bin
	
flash_dump:
	@$(MZ_FLASHER) $(COMPORT) read dump.bin
	
create_ota_img:
	@$(MZ_FLASHER) img main.bin UPDT0028.BIN
