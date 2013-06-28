#all:zip.so
#zip.so:
# 
# 	mv $@ $(HOME)/lib/lua/
# 
#clean:
# 	rm -rf *.o
# 	rm -rf *.so
# 

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

all:libzip minizip

libzip:
	cd libzip-0.11.1 && ./configure && make -C lib && cp -af lib/.libs/libzip.so* $(HOME)/lib/

minizip:
	gcc -shared  -fPIC -ansi -fstack-check lua_minizip.c  -o minizip.so -lluajit -lzip -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib 
#	cp zloader.lua $(HOME)/lib/lua
	cp print_r.lua $(HOME)/lib/lua	
	cp -rf minizip.so $(HOME)/lib/lua

%.o: %.c
	gcc -std=gnu99 -c $^ -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/

.PHONY: static
	
static: minizip.a

minizip.a: lua_minizip.o
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	cp -rf minizip.a $(HOME)/share/static/lua

clean:
	rm -rf *.so
	rm -rf *.o
	rm -rf *.a

