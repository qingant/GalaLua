all:libGalaxyRT.so
include ../mk.rules
OBJS = Object.o Exception.o Handle.o Socket.o Sync.o Pipe.o Utils.o Event.o Buffer.o Thread.o Process.o XML.o PQueue.o Timer.o

SUBCMPFLAG=$(CMPFLAG_)
CMPFLAG_:=$(CMPFLAG_)

libGalaxyRT.so:$(OBJS)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -o $@
	mv $@ $(HOME)/lib/
	mkdir -p $(HOME)/include/Runtime/
	cp *.hpp $(HOME)/include/Runtime/

.PHONY: static clean

static: libGalaxyRT.a


libGalaxyRT.a: $(OBJS)
	@echo $(PLATFORM)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static

CMPFLAG_:=$(SUBCMPFLAG)
