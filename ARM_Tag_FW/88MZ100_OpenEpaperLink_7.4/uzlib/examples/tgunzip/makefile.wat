##
## tgunzip  -  gzip decompressor example
##
## Watcom / OpenWatcom C/C++ makefile (GNU Make)
##
## Copyright (c) 2003 by Joergen Ibsen / Jibz
## All Rights Reserved
##
## http://www.ibsensoftware.com/
##

target  = tgunzip.exe
objects = tgunzip.obj
libs    = ../../lib/tinf.lib
system  = nt

cflags  = -bt=$(system) -d0 -ox -I..\..\src
ldflags = system $(system)

.PHONY: all clean

all: $(target)

$(target): $(objects)
	wlink $(ldflags) name $@ file {$^} library {$(libs)}

%.obj : %.c
	wcc386 $(cflags) $<

clean:
	$(RM) $(objects) $(target)
