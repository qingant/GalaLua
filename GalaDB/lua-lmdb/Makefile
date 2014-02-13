# makefile for lightning library for Lua
# based on lpack's

# change these to reflect your Lua installation

all:lightningmdb.so
include ../../mk.rules

PLATFORM=$(shell uname)
ifeq ($(PLATFORM),Darwin)
  LDFLAGS += -undefined dynamic_lookup
endif
COMMINC+=-I../lmdb

lightningmdb.so:lightningmdb.o ../lmdb/mdb.o ../lmdb/midl.o
	$(GCC) $(LNKFLAG) -L$(HOME)/lib -lm -o $@ $?
	mv $@  $(HOME)/lib/lua

.PHONY: all 
