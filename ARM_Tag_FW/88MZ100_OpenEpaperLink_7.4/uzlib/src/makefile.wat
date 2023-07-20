##
## tinflib  -  tiny inflate library (inflate, gzip, zlib)
##
## Watcom / OpenWatcom C/C++ makefile (GNU Make)
##
## Copyright (c) 2003 by Joergen Ibsen / Jibz
## All Rights Reserved
##
## http://www.ibsensoftware.com/
##

target  = ..\lib\tinf.lib
objects = tinflate.obj tinfgzip.obj tinfzlib.obj adler32.obj crc32.obj
system  = nt

cflags  = -bt=$(system) -d0 -obmlrs -s -zl

.PHONY: all clean

all: $(target)

$(target): $(objects)
	$(RM) $@
	echo $(patsubst %,+%,$(objects)) >> lib.cmd
	wlib -c -n -q -s -fo -io $@ @lib.cmd
	$(RM) lib.cmd

%.obj : %.c
	wcc386 $(cflags) $<

%.obj : %.nas
	nasm -o $@ -f obj -D_OBJ_ -O3 -Inasm/ $<

clean:
	$(RM) $(objects) $(target)
