
all:router.so amq.so logger.so _dir.so xml.so osext.so hexdump.so platform.so cp

include ../mk.rules
amq:amq.so
platform.so:LuaPlatform.o
	$(G++) $? $(LNKFLAG)  $(SYTMLIB)  -o $@

router.so:LuaRouter.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -o $@
amq.so:LuaAMQ.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyQueue -o $@
logger.so:LuaLogger.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -o $@
# Configure.so:LuaConfigure.o
# 	@echo $(PLATFORM)
# 	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB)	-lGalaxyRT -lMTRCofigure -lxml2	-lluajit -o $@
_dir.so:LuaDir.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -o $@
xml.so:LuaXML.o
	@echo $(PLATFORM)
	$(G++) $?   $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -o $@

osext.so:LuaOS.o
	@echo $(PLATFORM)
	$(G++) $?   $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -o $@
cp:
	cp -rf *.so $(HOME)/lib/lua
hexdump.so:Luadump.o imembase.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -o $@
.PHONY: static clean


static: _dir.a xml.a


_dir.a: LuaDir.o
	echo $(PLATFORM)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	mv _dir.a $(HOME)/share/static/lua

xml.a: LuaXML.o
	echo $(PLATFORM)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	mv xml.a $(HOME)/share/static/lua
hexdump.a:Luadump.o imembase.o
	echo $(PLATFORM)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	mv hexdump.a $(HOME)/share/static/lua
clean:
	-rm -f *.o *.a *.so
