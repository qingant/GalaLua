
all: libxml.so minixml.so

include ../../mk.rules

.PHONY:$(DES) static clean

libxml.so:pugixml.o
	$(G++) $(LNKFLAG) $(SYTMLIB) $(PLIBDIR) -o $@ $?
	mv $@  $(HOME)/lib

minixml.so:luaxml.o
	$(G++) $(LNKFLAG) $(SYTMLIB) $(PLIBDIR) -lxml -o $@ $?
	mv $@  $(HOME)/lib/lua
