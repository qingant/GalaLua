CC          = gcc
CFLAGS      = -g -std=gnu99 -Wall
INPATH      = -I. -I/usr/include/lua5.1
LDPATH      = -L.
LDLIBS      = 
LDFLAGS     = -fPIC -shared

CXX         = g++
CXXFLAGS    = -g -std=gnu++98

all: mkresx.out libresource.so libresxloader.so test.out test_lua.out test_lua2.out

%.o: %.c
	$(CC) -c -fPIC $(CFLAGS) $(INPATH) $^
	
mkresx.out: mkresx.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH)

libresource.so: resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) $(LDLIBS) $(LDFLAGS)

libresxloader.so: resxloader.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) $(LDLIBS) $(LDFLAGS) -lresource

test.out: test.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH)

test_lua.out: test_lua.o resource.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) /usr/lib/i386-linux-gnu/liblua5.1.a -ldl -lm

test_lua2.out: test_lua2.o resource.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) /usr/lib/i386-linux-gnu/liblua5.1.a -ldl -lm

.PHONY: test test_lua test_lua2 clean

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
	rm -f resx.rs
	./test.out

test_lua:
	echo "local retval = os.date(\"*t\")" > test.lua
	echo "retval = os.date(\"%F %T\", os.time(retval))" >> test.lua
	echo "io.write(string.format(\"%s\\n\", retval))" >> test.lua
	./mkresx.out -o resx.rs test.lua
	cat resx.rs >> test_lua.out
	rm -f resx.rs
	./test_lua.out
	
test_lua2:
	./mkresx.out -o resx.rs hello.lua
	cat resx.rs >> test_lua2.out
	rm -f resx.rs
	./test_lua2.out test.lua

clean:
	rm -f *.o *.so *.out
