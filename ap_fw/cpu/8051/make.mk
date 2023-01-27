FLAGS += -Icpu/8051

FLAGS += -mmcs51 --std-sdcc2x --opt-code-size --peep-file cpu/8051/peep.def --fomit-frame-pointer
SOURCES += cpu/8051/asmUtil.c
CC = sdcc

TARGETS	= main.ihx main.bin
OBJFILEEXT = rel
