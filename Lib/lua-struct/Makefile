

PLATFORM=$(shell uname)
LDFLG   = none
ifeq (${PLATFORM},Linux)
    LDFLG = -shared
	CC=gcc -ansi -Wall -O2 -fPIC -I$(HOME)/include
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
else ifeq (${PLATFORM},AIX)
    LDFLG = -qmkshrobj -q64 -bM:SRE -bnoentry -brtl -brtllib -berok
	CC=xlc_r -q64 -O2 -I$(HOME)/include -L $(HOME)/lib -lm
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

OBJ=struct.o

default: $(OBJ)
	$(CC) $(LDFLG) -o struct.so  $(OBJ)
	cp -rf  struct.so structX.lua stringop.lua $(HOME)/lib/lua/
.PHONY: static

static: struct.a

struct.a: $(OBJ)
	$(AR) $(ARFLAGS) struct.a $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) struct.a
	cp struct.a $(HOME)/share/static/lua
	cp structX.lua stringop.lua $(HOME)/share/static/lua

clean:
	rm -f $(OBJ)
	rm -f struct.so struct.a

