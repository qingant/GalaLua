all:libGalaxyStub.so

include ../mk.rules

libGalaxyStub.so:cstub.c
	$(GCC) $(CMPFLAG) -ocstub.o $<
	$(GCC) cstub.o $(LNKFLAG) -lpthread -lrt -ldl -o $@
	mv $@ $(HOME)/lib/