all:libGalaxyStub.so libGalaxyStub.a

include ../mk.rules

libGalaxyStub.so:cstub.c
	$(GCC) $(CMPFLAG) -o cstub.o $<
	$(GCC) cstub.o $(LNKFLAG) -lpthread -lrt -ldl -o $@
	mv $@ $(HOME)/lib/
	mkdir -p $(HOME)/include/Stub/
	cp *.h $(HOME)/include/Stub/

libGalaxyStub.a: cstub.c
	$(GCC) -o cstub.o $(CMPFLAG) $(COMMINC) -c -g -D_UNIX95_  $^
	ar $(ARFLAG) rcs $@ cstub.o
	mv $@ $(HOME)/lib
