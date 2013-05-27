CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
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

