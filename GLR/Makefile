all:res lib glr glr_s mkresx
lib:libGLR.so libGLR.a
include ../mk.rules
ROOT_DIR = ../..
.PHONY:res
GLR_OBJ = LuaNode.o Schedule.o GLR.o Bus.o Controllers.o SocketController.o BusController.o
libGLR.so:$(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
	mv $@ $(HOME)/lib/

libGLR.a: $(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	ar rcs $@ $(GLR_OBJ)
	mv $@ $(HOME)/lib/

res:
	echo "" > resource.h
	xxd -i glr.lua >> resource.h
ttt:ttt.o
	$(G++) $?  $(SYTMLIB) $(STUBLIB) -lluajit -o $@

glr:LuaTest.o resource/resxloader.c resource/resource.c resource/bswap.c resource/cmodulepack.c
	@echo $(PLATFORM)
	rm -f $@
	$(G++) -I./resource $(COMMINC) $^ -lluajit $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lGLR -lc -lm -ldl -lpthread -lrt -lluajit -o $@ 
	mv $@ $(HOME)/bin/

glr_s:LuaTest.o $(GLR_OBJ) resource/bswap.c resource/cmodulepack.c resource/resource.c resource/resxloader.c
	@echo $(PLATFORM)
	rm -f $@
	$(G++) -I./resource $(COMMINC) $^ $(HOME)/lib/libGalaxyStub.a $(HOME)/lib/libGalaxyRT.a $(SYTMLIB) -lc -lm -ldl -lpthread -lrt -lluajit -o $@
	mv $@ $(HOME)/bin/

mkresx: resource/mkresx.c resource/resource.c resource/bswap.c
	$(GCC) -o resource/mkresx -std=gnu99 -Wall -I./resource $^
	mv resource/mkresx $(HOME)/bin
