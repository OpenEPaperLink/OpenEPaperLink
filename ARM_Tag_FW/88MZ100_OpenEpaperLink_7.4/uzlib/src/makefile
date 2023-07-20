##
## tinflib  -  tiny inflate library (inflate, gzip, zlib)
##
## GCC makefile (Linux, FreeBSD, BeOS and QNX)
##
## Copyright (c) 2003 by Joergen Ibsen / Jibz
## All Rights Reserved
##
## http://www.ibsensoftware.com/
##

target  = ../lib/libtinf.a
objects = tinflate.o tinfgzip.o tinfzlib.o adler32.o crc32.o \
    defl_static.o genlz77.o

COPT    = -Os
CFLAGS  = -Wall $(COPT)
LDFLAGS = $(CFLAGS) -s

.PHONY: all clean

all: $(target)

$(target): $(objects)
	$(RM) $@
	ar -frs $@ $^
	ranlib $@

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.nas
	nasm -o $@ -f elf -D_ELF_ -O3 -Inasm/ $<

clean:
	$(RM) $(objects) $(target)
