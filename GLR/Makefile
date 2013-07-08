all:res lib glr #mkresx glr_s
lib:libGLR.so #libGLR.a
include ../mk.rules
STATIC_LIB_PATH = $(HOME)/share/static
ROOT_DIR = ../..
.PHONY:res static
GLR_OBJ = LuaNode.o Schedule.o GLR.o Bus.o Controllers.o SocketController.o BusController.o
libGLR.so:$(GLR_OBJ)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG) $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
	mv $@ $(HOME)/lib/

static: res libGLR.a mkresx

#libGLR.a: $(GLR_OBJ)
libGLR.a: SensorFrm.o $(GLR_OBJ) resource/bswap.o resource/resource.o resource/resxloader.o
	@echo $(PLATFORM)
	rm -f $@
	$(AR) $(ARFLAGS) $@ SensorFrm.o $(GLR_OBJ) resource/bswap.o resource/resource.o resource/resxloader.o
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static
	mkdir -p $(HOME)/include/resource
	cp resource/resxloader.h $(HOME)/include/resource

res:
	echo "" > resource.h
	xxd -i glr.lua >> resource.h
ttt:ttt.o
	$(G++) $?  $(SYTMLIB) $(STUBLIB) -lluajit -o $@

glr:LuaTest.o
	@echo $(PLATFORM)
	rm -f $@
	$(G++) -I./resource $(COMMINC) $^ -lluajit $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lGLR -lc -lm -ldl -lpthread -lrt -lluajit -o $@
	mv $@ $(HOME)/bin/

glr_s:SensorFrm.cpp $(GLR_OBJ) resource/bswap.c resource/resource.c resource/resxloader.c resource/cmodulepack.c
	@echo $(PLATFORM)
	rm -f $@
	$(G++) -I./resource $(COMMINC) $^ $(STATIC_LIB_PATH)/lua/cjson.a $(STATIC_LIB_PATH)/lua/struct.a $(STATIC_LIB_PATH)/lua/iconv.a $(STATIC_LIB_PATH)/lua/minizip.a $(STATIC_LIB_PATH)/lua/_dir.a  $(STATIC_LIB_PATH)/lua/lxp.a $(STATIC_LIB_PATH)/libGalaxyStub.a $(STATIC_LIB_PATH)/libGalaxyRT.a $(SYTMLIB) $(STATIC_LIB_PATH)/libluajit.a -lc -lm -ldl -lpthread -lrt -lzip -lexpat -o $@
	mv $@ $(HOME)/bin/

mkresx: resource/mkresx.c resource/resource.c resource/bswap.c
	$(GCC) -o resource/mkresx -I./resource $^
	mv resource/mkresx $(HOME)/bin

