
PLATFORM=$(shell uname)
LDFLG   = none
ifeq (${PLATFORM},Linux)
    LDFLG = -shared
	CC=g++ -ansi -Wall -O2 -I../../LuaJIT-2.0.0/src
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
endif
ifeq (${PLATFORM},AIX)
    LDFLG = -qmkshrobj -q64 -bM:SRE -bnoentry -brtl -brtllib -berok
	CC=xlc++_r -q64 -O2 -I../../lua-5.1.5/src -L $(HOME)/lib -lm
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

OBJ=aimagtpls.o

default: $(OBJ)
	$(CC) $(LDFLG) -o aimagtpls.so  $(OBJ)
	cp -rf  aimagtpls.so $(HOME)/lib/lua/
.PHONY: static

static: aimagtpls.a

$(OBJ): aimagtpls.cpp
	$(CC) -o $@ -c aimagtpls.cpp

aimagtpls.a: $(OBJ)
	$(AR) $(ARFLAGS) aimagtpls.a $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) aimagtpls.a
	mv aimagtpls.a $(HOME)/share/static/lua

clean:
	rm -f $(OBJ)
	rm -f aimagtpls.so aimagtpls.a

