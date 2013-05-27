AR          = ar
ARFLAGS     = rcu
RANLIB      = ranlib

all:router.so amq.so  logger.so  _dir.so _dir.a cp

include ../mk.rules
amq:amq.so
router.so:LuaRouter.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lxml2 -o $@
amq.so:LuaAMQ.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyQueue -lxml2 -o $@
logger.so:LuaLogger.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -lxml2 -o $@
# Configure.so:LuaConfigure.o
# 	@echo $(PLATFORM)
# 	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB)	-lGalaxyRT -lMTRCofigure -lxml2	-o $@
_dir.so:LuaDir.o
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lGalaxyRT -o $@
_dir.a: LuaDir.o
	@echo $(PLATFORM)
	$(AR) $(ARFLAGS) $@ $^
cp:
	cp -rf *.so *.a $(HOME)/lib/lua
