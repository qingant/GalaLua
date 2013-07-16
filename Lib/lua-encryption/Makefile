#Makefile
#
# Created on: 2013-5-9
#     Author: ocaml
#
#
all:libmd5.so md5.so test_md5

include ../mk.rules

libmd5.so:md5.o
	$(GCC) $? $(LNKFLAG) -o $@
	mv $@ $(HOME)/lib
md5.so:lua_md5.o
	$(GCC) $? $(LNKFLAG) $(SYTMLIB) $(LIBDIR) -lmd5 -o $@
	mv $@ $(HOME)/lib/lua
	cp -rf md5.lua $(HOME)/lib/lua
test_md5:test_md5.o
	$(GCC) $? $(LNKFLAG) $(SYTMLIB) $(LIBDIR) -lmd5 -o $@
