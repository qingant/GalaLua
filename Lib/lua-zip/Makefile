OBJ=lua_minizip.o

all:libzip minizip

AR          = ar
ARFLAGS     = -rcu $(ARFLAG)
RANLIB      = ranlib
PLATFORM=$(shell uname)
LDFLG   =
CFLAGS  =
ifeq (${PLATFORM},Linux)
	MAKE  = make
	LDFLG = -shared
	CFLAGS = -c -ansi -std=gnu99 -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
	CC=gcc
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
else ifeq (${PLATFORM},AIX)
	LDFLG = -qmkshrobj -q64 -bM:SRE -bnoentry -brtl -brtllib -berok
	CFLAGS = -c -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lm
	CC=xlc_r
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
	MAKE        = gmake
endif

libzip:
	cd libzip-0.11.1 && chmod +x configure && ./configure CC=$(CC) && OBJECT_MODE=64 && $(MAKE) -C lib && cp -f lib/.libs/libzip.so* $(HOME)/lib/


minizip:
	$(CC) $(LDFLG) $(CFLAGS) lua_minizip.c  -o minizip.so -lzip -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib
	cp -rf minizip.so $(HOME)/lib/lua


%.o: %.c
	$(CC) $(CFLAGS) -c $< -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/

static: libzip.a minizip.a

minizip.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	cp -rf minizip.a $(HOME)/share/static/lua
	rm -f minizip.a

libzip.a:
	cd libzip-0.11.1 && chmod +x configure && ./configure CC=$(CC) && OBJECT_MODE=64 && $(MAKE) -C lib && cp -f lib/.libs/libzip.a $(HOME)/share/static

clean:
	-rm -f *.o *.a *.so
	-rm -rf libzip-0.11.1/autom4te.cache
	@if [ -f libzip-0.11.1/Makefile ]; then \
		$(MAKE) -C libzip-0.11.1 clean; \
		$(MAKE) -C libzip-0.11.1 distclean; \
	fi

