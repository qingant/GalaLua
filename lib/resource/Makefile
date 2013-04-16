CC          = gcc
CFLAGS      = -g -std=gnu99 -Wall
INPATH      = -I.
LDPATH      = -L.
LDLIBS      = 
LDFLAGS     = -fPIC -shared

CXX         = g++
CXXFLAGS    = -g -std=gnu++98

all: mkresx.out libresource.so test.out

mkresx.out: mkresx.c resource.c
	$(CC) $(CFLAGS) $(INPATH) $^ -o $@

libresource.so: resource.c
	$(CC) $(CFLAGS) $(INPATH) $(LDPATH) $(LDLIBS) $(LDFLAGS) $^ -o $@

test.out: test.c
	$(CC) $(CFLAGS) $(INPATH) $(LDPATH) $^ -lresource -o $@
	./mkresx.out -o res.rs res/a res/h res/o
	cat res.rs >> test.out

.PHONY: clean
clean:
	rm -f fusion.out libresource.so test.out res.rs

