OBJ=lua_minizip.o

all:zip minizip

AR          = ar
ARFLAGS     = -rcu $(ARFLAG)
RANLIB      = ranlib
PLATFORM=$(shell uname)
LDFLG   =
CFLAGS  =
ifeq (${PLATFORM},Linux)
	MAKE        = make
	LDFLG       = -shared
	COMMINC     =  -I. -I./libzip -I./zlib -I$(HOME)/include
	CFLAGS      = -ansi -std=gnu99 -Wall -O2 -fPIC $(COMMINC)
	PLIBDIR     = -L$(HOME)/lib/
	CC          = gcc
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
	LIBZIP_ZLIB =
endif
ifeq (${PLATFORM},AIX)
	LDFLG = -qmkshrobj -q64 -bM:SRE -bnoentry -brtl -brtllib -berok
	COMMINC     =  -I. -I./libzip -I./zlib -I$(HOME)/include
	CFLAGS =  -q64 -O2 -lm $(COMMINC)
	PLIBDIR     = -L$(HOME)/lib/
	CC=xlc_r
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
	MAKE        = gmake
	LIBZIP_ZLIB =
endif

zip:
	$(MAKE) -C zlib all
	$(MAKE) -Clibzip all


minizip:
	$(CC) $(LDFLG) $(CFLAGS) lua_minizip.c  -o minizip.so -lzip $(PLIBDIR)
	cp -rf minizip.so $(HOME)/lib/lua


%.o: %.c
	$(CC) $(CFLAGS) -c $<

static: libzip.a minizip.a

minizip.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	cp -rf minizip.a $(HOME)/share/static/lua
	rm -f minizip.a

libzip.a:
	$(MAKE) -C zlib static
	$(MAKE) -C libzip static

clean:
	-rm -f *.o *.a *.so
	-rm -rf libzip-0.11.1/autom4te.cache
	@if [ -f libzip-0.11.1/Makefile ]; then \
		$(MAKE) -C libzip-0.11.1 clean; \
		$(MAKE) -C libzip-0.11.1 distclean; \
	fi

