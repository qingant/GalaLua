PREFIX      = /usr/local
LUA_INCLUDE = -I$(PREFIX)/include/luajit-2.0
LUA_STATIC  = $(PREFIX)/lib/libluajit-5.1.a
LUA_LIB     = $(PREFIX)/lib/libluajit-5.1.so

CC          = gcc
CFLAGS      = -std=gnu99 -g -Wall
INPATH      = -I. $(LUA_INCLUDE)
LDPATH      = -L.
LDLIBS      = -lc -lm -ldl
LDFLAGS     = -fPIC -shared

CXX         = g++
CXXFLAGS    = -std=gnu++98 -g -Wall

AR          = ar
ARFLAGS     = rcs

all: mkresx.out test.out test_lua.out test_lua2.out

%.o: %.c
	$(CC) -c $(CFLAGS) $(INPATH) -fPIC $^

mkresx.out: mkresx.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH)
	mv $@ $(HOME)/bin/mkresx

test.out: test.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH)

test_lua.out: test_lua.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) $(LUA_STATIC) $(LDLIBS)

test_lua2.out: test_lua2.o resource.o bswap.o
	$(CC) -o $@ $^ $(CFLAGS) $(INPATH) $(LDPATH) $(LUA_STATIC) $(LDLIBS)

.PHONY: install test test_lua test_lua2 clean

install:
	cp mkresx.out $(PREFIX)/bin

test:
	echo "grand regular: grand_regular_a.txt" > grand_regular_a.txt
	echo "grand regular: grand_regular_b.txt" > grand_regular_b.txt
	mkdir -p grand_directory/child_directory_a
	echo "grand_child_regular: grand_child_regular.txt" > grand_directory/child_directory_a/grand_child_regular.txt
	mkdir -p grand_directory/child_directory_b
	echo "child regular: child_regular.txt" > grand_directory/child_regular.txt
	./mkresx.out -o resx.rs grand_regular_a.txt grand_directory grand_regular_b.txt
	cat resx.rs >> test.out
	./test.out

test_lua:
	./mkresx.out -o resxlua.rs test.lua
	cat resxlua.rs >> test_lua.out
	./test_lua.out

test_lua2:
	./mkresx.out -o resxlua2.rs main.lua auxiliary.lua
	cat resxlua2.rs >> test_lua2.out
	./test_lua2.out test2.lua

clean:
	rm -f *.o *.so *.out *.rs
	rm -rf grand_regular_a.txt grand_regular_b.txt grand_directory

