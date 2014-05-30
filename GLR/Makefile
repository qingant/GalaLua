all: lib glr
lib:libGLR.so #libGLR.a
include ../mk.rules

STATIC_LIB_PATH = $(HOME)/share/static
ROOT_DIR = ../..
.PHONY:static clean
GLR_OBJ = LuaNode.o Schedule.o GLR.o Bus.o Controllers/Clock.o Controllers/SocketController.o Controllers/BusController.o NamedProcessTable.o Loader.o

libGLR.so:$(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
	mv $@ $(HOME)/lib/

static: libGLR.a

libGLR.a: $(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(GLR_OBJ)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static

glr:LuaTest.o
	@echo $(PLATFORM)
	rm -f $@
	$(G++)  $(COMMINC) $^ -lluajit $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lGLR -lc -lm -ldl -lpthread -lrt -lluajit -o $@
	cp glr.lua $(HOME)/lib/lua/
	cp -rf wrappers $(HOME)/lib/lua/
	mv $@ $(HOME)/bin/

clean::
	-rm -f glr glr_s *.o *.a

