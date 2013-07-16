# makefile for lightning library for Lua
# based on lpack's

# change these to reflect your Lua installation

include ../mk.rules

PLATFORM=$(shell uname)
ifeq ($(PLATFORM),Darwin)
  LDFLAGS += -undefined dynamic_lookup
endif

all:lightningmdb.so

lightningmdb.so:lightningmdb.o ../lmdb/mdb.o ../lmdb/midl.o
	$(GCC) $(LNKFLAG) -L$(HOME)/lib -lluajit -lm -o $@ $?
	mv $@  $(HOME)/lib/lua

clean:
	rm -rf *.so
	rm -rf *.o

.PHONY: all clean
