CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
OBJ=struct.o

default: $(OBJ)
	$(CC) -shared -o struct.so  $(OBJ)

clean:
	rm -f $(OBJ)
	rm -f struct.so

