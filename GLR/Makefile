all:lib glr
lib:libGLR.so
include ../mk.rules
ROOT_DIR = ../..


libGLR.so:LuaNode.o Schedule.o GLR.o Bus.o Controllers.o SocketController.o BusController.o
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
	mv $@ $(HOME)/lib/	


ttt:ttt.o
	$(G++) $?  $(SYTMLIB) $(STUBLIB)  -lluajit -o $@
glr:LuaTest.o
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $?  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lGLR -o $@
	mv $@ $(HOME)/bin/
