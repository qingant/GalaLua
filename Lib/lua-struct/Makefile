PLATFORM=$(shell uname)
LDFLG   = none
ifeq (${PLATFORM},Linux)
    LDFLG = -shared
	CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
else ifeq (${PLATFORM},AIX)
    LDFLG = -qmkshrobj
	CC=xlc_r -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lluajit -lm
endif
OBJ=struct.o

AR          = ar
ARFLAGS     = -rcu $(ARFLAG)
RANLIB      = ranlib

default: $(OBJ)
	$(CC) $(LDFLG) -o struct.so  $(OBJ)
	$(AR) $(ARFLAGS)   struct.a $(OBJ)
	$(RANLIB) struct.a

clean:
	rm -f $(OBJ)
	rm -f struct.so struct.a

