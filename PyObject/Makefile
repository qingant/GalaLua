all:PyGalaxy.so
include ../mk.rules
OBJS = PyGalaxy.o #pythonAPI_wapper.o

SUBCMPFLAG=$(CMPFLAG_)
CMPFLAG_:=$(CMPFLAG_) -I ~/python/include/python2.7

PyGalaxy.so:$(OBJS)
	@echo $(PLATFORM)
	rm -f $@ 
	$(G++) $(LNKFLAG)  $^ -lMTRCofigure -lxml2 $(STUBLIB)  $(SYTMLIB)  -o $@
	mkdir -p $(HOME)/lib/gala/
	mv $@ $(HOME)/lib/gala/

CMPFLAG:=$(SUBCMPFLAG)
