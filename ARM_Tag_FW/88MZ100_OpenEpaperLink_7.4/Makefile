
MZ_FLASHER=python ../88MZ100_Flasher/88MZ100_Uart_flasher.py

ARMGCC=/usr/bin/
CC=$(ARMGCC)arm-none-eabi-gcc
GPLUSPLUS=$(ARMGCC)arm-none-eabi-g++
AS=$(ARMGCC)arm-none-eabi-as
OBJCOPY=$(ARMGCC)arm-none-eabi-objcopy

#-Wall
CC_WARNING_FLAGS=-Wall -Wformat=0 -Wattributes -Wstrict-aliasing=0
CPP_FLAGS=-lstdc++ -mcpu=cortex-m3 -g -O3 -mthumb -fdata-sections -ffunction-sections -std=c++98 -std=gnu++0x
CPP0_FLAGS=-lstdc++ -mcpu=cortex-m3 -g -O0 -mthumb -fdata-sections -ffunction-sections -std=c++98 -std=gnu++0x 
CC_FLAGS=-mcpu=cortex-m3 -g -O0 -mthumb -fdata-sections -ffunction-sections -std=c99
CC3_FLAGS=-mcpu=cortex-m3 -g -O3 -mthumb -fdata-sections -ffunction-sections -std=c99
CC_END_FLAGS=-lstdc++ -lc -lnosys -L. -T mz100/mz100.ld -fPIE --specs=nosys.specs -mcpu=cortex-m3 -mthumb -Wl,--gc-sections -O0 -flto -ffunction-sections -fdata-sections -DARM_GNU

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
#	@mkdir -p build
	@$(AS) -mcpu=cortex-m3 --gdwarf-2 -mthumb-interwork -o build/startup.o startup.S
	@$(CC) $(CC3_FLAGS) -c mz100/core_cm3.c         -o build/core_cm3.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_ssp.c        -o build/mz100_ssp.o
	@$(CC) $(CC_FLAGS) -c mz100/mz100_wdt.c        -o build/mz100_wdt.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_gpio.c       -o build/mz100_gpio.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_driver.c     -o build/mz100_driver.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_adc.c        -o build/mz100_adc.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_flash.c      -o build/mz100_flash.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_clock.c      -o build/mz100_clock.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_rtc.c        -o build/mz100_rtc.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_pinmux.c     -o build/mz100_pinmux.o
	@$(CC) $(CC_FLAGS) -c mz100/mz100_pmu.c        -o build/mz100_pmu.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_qspi.c       -o build/mz100_qspi.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_aes.c        -o build/mz100_aes.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_gpt.c        -o build/mz100_gpt.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_sleep.c        -o build/mz100_sleep.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_uart.c       -o build/mz100_uart.o
	@$(CC) $(CC3_FLAGS) -c mz100/mz100_aon_ram.c       -o build/mz100_aon_ram.o
	@$(CC) $(CC3_FLAGS) -c mz100/printf.c		-o build/printf.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c zigbee.c        -o build/zigbee.o
#	@$(CC) $(CC_FLAGS) $(CC_WARNING_FLAGS) -c ccm.c        -o build/ccm.o

	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/adler32.c      -o build/uz-adler32.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/crc32.c        -o build/uz-crc32.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/defl_static.c  -o build/uz-defl_static.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/genlz77.c      -o build/uz-genlz77.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/tinfgzip.c     -o build/uz-tinfgzip.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/tinflate.c     -o build/uz-tinflate.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/uzlib/src/tinfzlib.c     -o build/uz-tinfzlib.o

	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -Wno-unknown-pragmas -c ../common/QRCode/src/qrcode.c      -o build/qrcode.o

	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c compression.cpp        -o build/compression.o

	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c drawing.cpp        -o build/drawing.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c epd_interface.cpp	-o build/epd_interface.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c powermgt.cpp        -o build/powermgt.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLASGS) -c oepl-protocol.cpp -o build/oepl-protocol.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c comms.c        -o build/comms.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c ../common/md5.c        -o build/md5.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c settings.cpp        -o build/settings.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c mz100/timer.c        -o build/timer.o
	@$(CC) $(CC3_FLAGS) $(CC_WARNING_FLAGS) -c mz100/util.c        -o build/util.o
	@$(CC) $(CC_FLAGS) $(CC_WARNING_FLAGS) -c mz100/gpio.c        -o build/gpio.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS)  -c uc8159-var-m2.cpp        -o build/uc8159-var-m2.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS)  -c uc8176-var-m2.cpp        -o build/uc8176-var-m2.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c userinterface.cpp	-o build/userinterface.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c main.cpp             -o build/main.o
	@$(GPLUSPLUS) $(CPP_FLAGS) $(CC_WARNING_FLAGS) -c oepl_fs.cpp             -o build/oepl_fs.o
	@$(GPLUSPLUS) $(CC_END_FLAGS) $(CC_WARNING_FLAGS) \
		build/main.o \
		build/epd_interface.o \
		build/oepl_fs.o\
		build/userinterface.o \
		build/printf.o \
		build/mz100_aon_ram.o \
		build/zigbee.o \
		build/compression.o \
		build/drawing.o \
		build/powermgt.o \
		build/oepl-protocol.o \
		build/comms.o \
		build/settings.o \
		build/timer.o \
		build/util.o \
		build/md5.o \
		build/gpio.o \
		build/uc8176-var-m2.o \
                build/uc8159-var-m2.o \
		build/mz100_sleep.o \
		build/core_cm3.o \
		build/qrcode.o \
		build/uz-adler32.o  build/uz-crc32.o build/uz-defl_static.o build/uz-genlz77.o build/uz-tinfgzip.o build/uz-tinflate.o build/uz-tinfzlib.o \
		build/mz100_ssp.o build/mz100_wdt.o build/mz100_gpio.o build/mz100_driver.o build/mz100_adc.o build/mz100_flash.o build/mz100_clock.o build/mz100_rtc.o build/mz100_pinmux.o build/mz100_pmu.o build/mz100_qspi.o build/mz100_aes.o build/mz100_gpt.o build/mz100_uart.o \
		build/startup.o -o main.axf
	@$(OBJCOPY) -v -O binary main.axf main.bin
	@cat build/fs.img >> main.bin
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
