CC          = gcc
CFLAGS      = -g -std=gnu99 -Wall
INPATH      = -I. -I/usr/include/lua5.1
LDPATH      = -L.
LDLIBS      = 
LDFLAGS     = -fPIC -shared

CXX         = g++
CXXFLAGS    = -g -std=gnu++98

all: mkresx.out libresource.so test.out test_lua.out

%.o: %.c
	$(CC) $^ -c $(CFLAGS) $(INPATH)
	
mkresx.out: mkresx.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH)

libresource.so: resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) $(LDLIBS) $(LDFLAGS)

test.out: test.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH)

test_lua.out: test_lua.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) -llua5.1 -lresource

.PHONY: test clean

test:
	echo "hello hello" > hello.txt
	echo "world world" > world.txt
	mkdir -p resx
	mkdir -p resx/abc
	echo "abcdefg" > resx/abc/abc.txt
	mkdir -p resx/hij
	echo "uvw xyz" > resx/uvw.txt
	./mkresx.out -o resx.rs hello.txt resx world.txt
	cat resx.rs >> test.out
	./test.out

clean:
	rm -f *.o *.so *.out
