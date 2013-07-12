PLATFORM=$(shell uname)
LDFLG   = none
ifeq (${PLATFORM},Linux)
    LDFLG = -shared
	CC=gcc -ansi -Wall -O2 -fPIC -I../../LuaJIT-2.0.0/src/
else ifeq (${PLATFORM},AIX)
    LDFLG = -qmkshrobj
	CC=xlc_r -q64 -O2 -I../../lua-5.1.5/src/ -L $(HOME)/lib -lluajit -lm
endif

OBJ=lua_minizip.o

all:libzip minizip.a

AR          = ar
ARFLAGS     = -rcu $(ARFLAG)
RANLIB      = ranlib

libzip:
	cd libzip-0.11.1 && ./configure && make -C lib && cp -af lib/.libs/libzip.so* $(HOME)/lib/

#
#minizip:
#	gcc -shared  -fPIC -ansi -fstack-check lua_minizip.c  -o minizip.so -lluajit -lzip -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib 
#	cp -rf minizip.so $(HOME)/lib/lua

%.o: %.c
	#gcc -std=gnu99 -c $^ -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/
	$(CC) -std=c99 -c $^ -Ilibzip-0.11.1/lib -I../../LuaJIT-2.0.0/src/

minizip.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@
	cp -rf minizip.a $(HOME)/lib/lua

clean:
	rm -rf *.so
	rm -rf *.o

