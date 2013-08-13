
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
    LDFLG = -qmkshrobj -q64 -bM:SRE -bnoentry -brtl -brtllib -berok
	CC=xlc_r -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lm
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

OBJ=osdatetime.o

default: $(OBJ)
	$(CC) $(LDFLG) -o osdatetime.so  $(OBJ)
	cp -rf  osdatetime.so $(HOME)/lib/lua/
.PHONY: static

static: osdatetime.a

osdatetime.a: $(OBJ)
	$(AR) $(ARFLAGS) osdatetime.a $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) osdatetime.a
	cp osdatetime.a $(HOME)/share/static/lua

clean:
	rm -f $(OBJ)
	rm -f osdatetime.so osdatetime.a

