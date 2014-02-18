MAKE          = make
PLATFORM      = $(shell uname)
DES           = libxml.so minixml.so
all:$(DES)
include ./mk.rules
.PHONY:$(DES) static clean

ifeq ($(PLATFORM),AIX)
    MAKE=gmake
endif

libxml.so:pugixml.o
	$(G++) $(LNKFLAG) $(SYTMLIB) $(PLIBDIR) -o $@ $?
	mv $@  $(HOME)/lib

minixml.so:luaxml.o
	$(G++) $(LNKFLAG) $(SYTMLIB) $(PLIBDIR) -lxml -o $@ $?
	mv $@  $(HOME)/lib/lua
clean:
	rm -rf *.o