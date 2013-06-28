
PLATFORM=$(shell uname)
LDFLG   = none
ifeq (${PLATFORM},Linux)
    LDFLG = -shared
	CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = 
else ifeq (${PLATFORM},AIX)
    LDFLG = -qmkshrobj
	CC=xlc_r -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lluajit -lm
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

OBJ=struct.o

default: $(OBJ)
	$(CC) $(LDFLG) -o struct.so  $(OBJ)

.PHONY: static

static: struct.a

struct.a: $(OBJ)
	$(AR) $(ARFLAGS) struct.a $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) struct.a
	cp struct.a $(HOME)/share/static/lua

clean:
	rm -f $(OBJ)
	rm -f struct.so struct.a

