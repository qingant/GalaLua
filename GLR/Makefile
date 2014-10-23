all: lib glr
lib:libGLR.so amq.so #libGLR.a
include ../mk.rules

STATIC_LIB_PATH = $(HOME)/share/static
.PHONY:static clean
GLR_OBJ = LuaNode.o Schedule.o GLR.o Bus.o Controllers/Clock.o Controllers/SocketController.o Controllers/BusController.o NamedProcessTable.o Loader.o

header:
	cp *.hpp $(HOME)/include

libGLR.so:$(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
	mv $@ $(HOME)/lib/

static: libGLR.a header

libGLR.a: $(GLR_OBJ) lua_file
	@echo $(PLATFORM)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(GLR_OBJ)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static

glr:LuaTest.o lua_file
	@echo $(PLATFORM)
	rm -f $@
	$(G++)  $(COMMINC) $< -lluajit $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lGLR -lc -lm -ldl -lpthread -lrt -lluajit -o $@
	mv $@ $(HOME)/bin/

lua_file:
	cp glr.lua $(HOME)/lib/lua/
	cp -rf wrappers $(HOME)/lib/lua/

amq.so: Controllers/AMQController.o
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -lGalaxyMQ -o $@
	mv $@ $(HOME)/lib/

clean::
	-rm -f glr glr_s *.o *.a Controllers/*.o

