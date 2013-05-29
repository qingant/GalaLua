PLATFORM=$(shell uname)

ifeq (${PLATFORM},Linux)
	CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
else ifeq (${PLATFORM},AIX)
	CC=gcc -ansi -Wall -O2 -fPIC -I../../lua-5.1.5/src/ -L $(HOME)/lua5.1.5/lib -llua -lm
endif
OBJ=struct.o

AR          = ar
ARFLAGS     = rcu
RANLIB      = ranlib

default: $(OBJ)
	$(CC) -shared -o struct.so  $(OBJ)
	$(AR) $(ARFLAGS) struct.a $(OBJ)

clean:
	rm -f $(OBJ)
	rm -f struct.so struct.a

