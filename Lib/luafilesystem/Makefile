# $Id: Makefile,v 1.36 2009/09/21 17:02:44 mascarenhas Exp $

T= lfs

#CONFIG= ./config

include ../mk.rules

SRCS= src/$T.c
OBJS= src/$T.o

lib: src/lfs.so

src/lfs.so: $(OBJS)
	MACOSX_DEPLOYMENT_TARGET="10.3"; export MACOSX_DEPLOYMENT_TARGET; $(GCC) $(LNKFLAG) $(LIBDIR) $(SYTMLIB) -lluajit -o src/lfs.so $(OBJS)

install:
	#mkdir -p $(LUA_LIBDIR)
	cp src/lfs.so $(HOME)/lib/lua

