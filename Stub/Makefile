include ../mk.rules

all: libGalaxyStub.so

libGalaxyStub.so: cstub.c
	$(GCC) $(CMPFLAG) -o cstub.o $<
	$(GCC) cstub.o $(LNKFLAG) -lpthread -lrt -ldl -o $@
	mv $@ $(HOME)/lib/
	mkdir -p $(HOME)/include/Stub/
	cp *.h $(HOME)/include/Stub/

.PHONY: static

static: libGalaxyStub.a
	
libGalaxyStub.a: cstub.c
	$(GCC) -o cstub.o $(CMPFLAG) $(COMMINC) -c -g -D_UNIX95_  $^
	$(AR) $(ARFLAGS) $@ cstub.o
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static

