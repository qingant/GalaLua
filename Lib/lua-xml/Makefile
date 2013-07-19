include ../mk.rules
OS=$(shell uname)
ifeq ($(OS),AIX)
	MAKE = gmake
else
	MAKE = make
endif

all: etree luaiconv luaxpath libexpat luaexpat

etree:
	$(MAKE) -C etree-0.1.1
	$(MAKE) -C etree-0.1.1 install

luaiconv:
	$(MAKE) -C lua-iconv-7 all
	$(MAKE) -C lua-iconv-7 install

luaxpath:
	$(MAKE) -C luaxpath
	$(MAKE) -C luaxpath install

libexpat:
	rm -f expat-2.1.0/Makefile
#ifeq ($(OS), Linux)
	cd expat-2.1.0 && ./configure --prefix=$(HOME) --libdir=$(HOME)/lib --includedir=$(HOME)/include/luajit-2.0
#else
	cd expat-2.1.0 && ./configure --prefix=$(HOME) --libdir=$(HOME)/lib --includedir=$(HOME)/include/luajit
#endif
	cd expat-2.1.0 && cp expat_config.h lib/expat_external.h lib/expat.h $(HOME)/include
	make -C expat-2.1.0
	cp expat-2.1.0/.libs/libexpat.so* $(HOME)/lib
	cp expat-2.1.0/.libs/libexpat.a $(HOME)/share/static

luaexpat: libexpat
	$(MAKE) -C luaexpat-1.1
	$(MAKE) -C luaexpat-1.1 install

.PHONY: clean static

static: etree luaxpath libexpat
	$(MAKE) -C lua-iconv-7 static
	$(MAKE) -C lua-iconv-7 install
	$(MAKE) -C luaexpat-1.1 static

clean:
	$(MAKE) -C lua-iconv-7 clean
	$(MAKE) -C luaexpat-1.1 clean
	@if [ -f expat-2.1.0/Makefile ]; then $(MAKE) -C expat-2.1.0 distclean; fi

