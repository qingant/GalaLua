G++			= g++
GCC			= gcc
PLATFORM      =  $(shell uname)
ifeq ($(PLATFORM),AIX)
G++ = xlc++_r
endif
COMMINC     = -I. -I../include -I../../GalaxyKit/Stub -I../../GalaxyKit -I/usr/include/libxml2
LNKFLAG 	= -shared -pthread
CMPFLAG 	= -c -g -fPIC -pthread -Wall $(COMMINC)  -D_UNIX95_ -D_DEBUG_ -Wno-write-strings -rdynamic
PLIBDIR		= -lGalaxyStub -lGalaxyRT -L$(HOME)/lib/
STUBLIB     = -lGalaxyStub
SYTMLIB 	= -lc -lm -ldl -lpthread  -lrt -L$(HOME)/lib/

OBJ = test.o

all:cfgctl

.cpp.o:
	$(G++) $(CMPFLAG) -o$@ $<

clean:
	cd $(HOME)/lib/ && rm -f $@
	cd $(HOME)/bin/ && rm -f $@
	rm -rf *.o
rf:clean all

cfgctl:$(OBJ)
	$(G++) -pthread $(PLIBDIR) $(SYTMLIB) -lMTRCofigure -lxml2 -o$@ $?
	#mv $@ $(HOME)/bin/
	rm -rf *.o

