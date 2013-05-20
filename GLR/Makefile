all:res lib glr mkresx
lib:libGLR.so
include ../mk.rules
ROOT_DIR = ../..
.PHONY:res
GLR_OBJ = LuaNode.o Schedule.o GLR.o Bus.o Controllers.o SocketController.o BusController.o bswap.o cmodulepack.o resource.o resxloader.o
libGLR.so:$(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
	mv $@ $(HOME)/lib/	

res:
	echo "" > resource.h
	xxd -i glr.lua >> resource.h
ttt:ttt.o
	$(G++) $?  $(SYTMLIB) $(STUBLIB)  -lluajit -o $@
glr:LuaTest.o
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $?  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lGLR -o $@
	mv $@ $(HOME)/bin/

glr_s:luaTest.o $(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(G++)  ../Stub/libGalaxyStub.a ../Runtime/libGalaxyRT.a  $(SYTMLIB) -lluajit  -o $@ $?
	mv $@ $(HOME)/bin/

mkresx： mkresx.c resource.c bswap.c
	$(GCC) -o $@ -std=gnu99 -Wall $^
	mv $@ $(HOME)/bin/

