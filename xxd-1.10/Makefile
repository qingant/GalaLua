#
# Makefile for xxd
#

OS=$(shell uname)
ifeq ($(OS),AIX)
	MAKE = gmake
	CC   = xlc_r
	CFLAGS=-O
else
	MAKE = make
	CC   = gcc
	CFLAGS=-O
endif

all: xxd

xxd: xxd.c
	$(CC) $(CFLAGS) -o xxd xxd.c

.PHONY: clean install uninstall

install:
	cp xxd $(HOME)/bin

uninstall:
	rm $(HOME)/bin/xxd

clean:
	-rm -f xxd *.o
