all:PyGalaxy.so
include ../mk.rules
OBJS = PyGalaxy.o #pythonAPI_wapper.o

PyGalaxy.so:$(OBJS)
	@echo $(PLATFORM)
	$(G++) $(LNKFLAG) $^ -lMTRCofigure -lxml2 $(STUBLIB)  $(SYTMLIB)  -o $@
	mkdir -p $(HOME)/lib/gala/
	mv $@ $(HOME)/lib/gala/

$(OBJS):PyGalaxy.cpp
	g++ -c -fPIC -pthread -Wall -Wextra -Woverloaded-virtual -I../Common -I../Stub $(shell python-config --include) -I../Config/include   -I../Runtime  -I.. -I/usr/include/libxml2  -D_UNIX95_   -Wno-write-strings  -fno-strict-aliasing  -Wno-missing-field-initializers -D_DEBUG_ -oPyGalaxy.o PyGalaxy.cpp

