OBJ=lua_minizip.o

#all:libzip.a minizip.a

AR          = ar
ARFLAGS     = -rcu $(ARFLAG)
RANLIB      = ranlib

PLATFORM=$(shell uname)
LDFLG   = none
ifeq (${PLATFORM},Linux)
	LDFLG = -shared
	CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
else ifeq (${PLATFORM},AIX)
	LDFLG = -qmkshrobj
	CC=xlc_r -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lluajit -lm
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

libzip:
	cd libzip-0.11.1 && ./configure && make -C lib && cp -af lib/.libs/libzip.so* $(HOME)/lib/

#
#minizip:
#	gcc -shared  -fPIC -ansi -fstack-check lua_minizip.c  -o minizip.so -lluajit -lzip -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib 
#	cp -rf minizip.so $(HOME)/lib/lua


%.o: %.c
	$(CC) -std=c99 -c $^ -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/
	
static: minizip.a libzip.a

minizip.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	cp -rf minizip.a $(HOME)/share/static/lua
	rm -f minizip.a

libzip.a:
	cd libzip-0.11.1 && ./configure && make -C lib && cp -af lib/.libs/libzip.a $(HOME)/share/static

clean:
	-rm -f *.o *.a *.so
	-rm -rf libzip-0.11.1/autom4te.cache
	@if [ -f libzip-0.11.1/Makefile ]; then make -C libzip-0.11.1 distclean; fi

