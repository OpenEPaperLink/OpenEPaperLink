FLAGS += -Isoc/zbs243

FLAGS += -DSOC_ZBS243 --xram-loc 0xe000 --xram-size 0x2000 --model-large

SOURCES += soc/zbs243/soc.c soc/zbs243/wdt.c soc/zbs243/sleep.c soc/zbs243/spi.c soc/zbs243/uart.c soc/zbs243/timer.c soc/zbs243/radio.c
SOURCES += soc/zbs243/flash.c soc/zbs243/temperature.c cpu/8051/random.c cpu/8051/aes.c cpu/8051/printf.c
SOURCES += soc/zbs243/i2c.c
CPU = 8051
