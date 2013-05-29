all:libGalaxyRT.so libGalaxyRT.a
include ../mk.rules
OBJS = Object.o Exception.o Handle.o Socket.o Sync.o Pipe.o Utils.o Event.o Logger.o Buffer.o Thread.o File.o IPC.o Process.o Dll.o Options.o  XML.o PQueue.o Timer.o GalaxmlParser.o  Zlib.o Router.o

SUBCMPFLAG=$(CMPFLAG_)
CMPFLAG_:=$(CMPFLAG_)

libGalaxyRT.so:$(OBJS)
	@echo $(PLATFORM)
	rm -f $@
	$(G++) $? $(LNKFLAG)  $(SYTMLIB) $(STUBLIB) -lxml2 -o $@
	mv $@ $(HOME)/lib/
	mkdir -p $(HOME)/include/Runtime/
	cp *.hpp $(HOME)/include/Runtime/

libGalaxyRT.a: $(OBJS)
	@echo $(PLATFORM)
	rm -f $@
	ar -X64 rcs $@ $(OBJS)
	mv $@ $(HOME)/lib/

CMPFLAG_:=$(SUBCMPFLAG)
