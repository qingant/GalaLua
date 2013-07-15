
all:router.so amq.so logger.so _dir.so xml.so cp

include ../mk.rules
amq:amq.so
router.so:LuaRouter.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lxml2 -lluajit -o $@
amq.so:LuaAMQ.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyQueue -lxml2 -lluajit -o $@
logger.so:LuaLogger.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lxml2 -lluajit -o $@
# Configure.so:LuaConfigure.o
# 	@echo $(PLATFORM)
# 	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB)	-lGalaxyRT -lMTRCofigure -lxml2	-lluajit -o $@
_dir.so:LuaDir.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@
xml.so:LuaXML.o
	@echo $(PLATFORM)
	$(G++) $?   $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lluajit -o $@

cp:
	cp -rf *.so $(HOME)/lib/lua

.PHONY: static clean


static: _dir.a


_dir.a: LuaDir.o
	echo $(PLATFORM)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	mv _dir.a $(HOME)/share/static/lua

clean:
	-rm -f *.o *.a *.so
