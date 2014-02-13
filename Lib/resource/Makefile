include ../../mk.rules
AR          = ar
ARFLAGS     = rcs

all: mkresx.out libresource.so libresxloader.so test.out test_lua.out test_lua2.out

mkresx.out: mkresx.o resource.o bswap.o
	$(GCC) -o $@ $^ $(SYTMLIB)
	mv $@ $(HOME)/bin

libresource.so: resource.o bswap.o
	$(GCC) -o $@ $^ $(LNKFLAG) $(SYTMLIB)
	mv $@ $(HOME)/lib

libresxloader.so: resxloader.o
	$(GCC) -o $@ $^ $(LNKFLAG) $(SYTMLIB) $(LIBDIR) -lresource
	mv $@ $(HOME)/lib

test.out: test.o resource.o bswap.o
	$(GCC) -o $@ $^ $(SYTMLIB)

test_lua.out: test_lua.o
	$(GCC) -o $@ $^ $(SYTMLIB) $(LIBDIR) -lresource

test_lua2.out: test_lua2.o
	$(GCC) -o $@ $^ $(SYTMLIB) $(LIBDIR) -lresource

.PHONY: test test_lua test_lua2 #clean

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

test_lua:
	echo "local retval = os.date(\"*t\")" > test_lua
	echo "retval = os.date(\"%F %T\", os.time(retval))" >> test_lua
	echo "io.write(string.format(\"%s\", retval))" >> test_lua
	./mkresx.out -o resx.rs test_lua
	cat resx.rs >> test_lua.out
	./test_lua.out

test_lua2:
	./mkresx.out -o resx.rs hello_lua
	cat resx.rs >> test_lua2.out
	./test_lua2.out test_lua

#clean:
#	rm -f *.o *.so *.out *.rs
