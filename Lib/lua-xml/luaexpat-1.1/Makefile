T= lxp
V= 1.1.0
CONFIG= ./config

sinclude $(CONFIG)

PLATFORM=$(shell uname)
LDFLAGS   = none
ifeq (${PLATFORM},Linux)
	LDFLAGS     = -shared
	CC          = gcc -ansi -Wall -O2 -I../../../LuaJIT-2.0.0/src/
	DYNCFLAG    = -fPIC
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
else ifeq (${PLATFORM},AIX)
	LDFLAGS     = -qmkshrobj
	CC          = xlc_r -q64 -O2 -I../../../lua-5.1.5/src/ -L $(HOME)/lib -lluajit -lm
	DYNCFLAG    =
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

ifeq "$(LUA_VERSION_NUM)" "500"
COMPAT_O= $(COMPAT_DIR)/compat-5.1.o
endif

OBJS= src/lxplib.o # $(COMPAT_O)
lib: src/$(LIBNAME) src/lxp.a

src/$(LIBNAME) : $(OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3";
	$(CC) -o src/lxplib.o -c $(DYNCFLAG) $(CFLAGS) src/lxplib.c
	$(CC) -o src/$(LIBNAME) $(LIB_OPTION) $(OBJS) -L$(HOME)/lib -lexpat

src/lxp.a: src/lxplib.c
	$(CC) -o src/lxplib.o -c $(CFLAGS) src/lxplib.c
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $@

$(COMPAT_DIR)/compat-5.1.o: $(COMPAT_DIR)/compat-5.1.c
	$(CC) -c $(CFLAGS) -o $(COMPAT_DIR)/compat-5.1.o $(COMPAT_DIR)/compat-5.1.c

install:
	mkdir -p $(LUA_LIBDIR)
	cp src/$(LIBNAME) $(LUA_LIBDIR)
	cd $(LUA_LIBDIR); ln -f -s $(LIBNAME) $T.so
	mkdir -p $(LUA_DIR)/$T
	cp src/$T/lom.lua $(LUA_DIR)/$T
	cp src/lxp.a $(HOME)/share/static/lua

.PHONY: clean static

static: src/lxp.a
	mkdir -p $(LUA_DIR)/$T
	cp src/$T/lom.lua $(LUA_DIR)/$T
	cp src/lxp.a $(HOME)/share/static/lua

clean:
	rm -f src/$(LIBNAME) $(OBJS) src/lxp.a

# $Id: makefile,v 1.33 2006/06/08 20:41:48 tomas Exp $
