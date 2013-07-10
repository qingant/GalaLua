include ../mk.rules
OS=$(shell uname)
ifeq ($(OS),AIX)
	MAKE = gmake
else
	MAKE = make
endif

all: etree luaiconv luaxpath luaexpat

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
	cd expat-2.1.0 && ./configure --prefix=$(HOME) --libdir=$(HOME)/lib --includedir=$(HOME)/include/luajit-2.0
	cd expat-2.1.0 && cp expat_config.h lib/expat_external.h lib/expat.h $(HOME)/include
	make -C expat-2.1.0
	cp expat-2.1.0/.libs/libexpat.so* $(HOME)/lib
	cp expat-2.1.0/.libs/libexpat.a $(HOME)/share/static

luaexpat: libexpat
	$(MAKE) -C luaexpat-1.1
	$(MAKE) -C luaexpat-1.1 install

.PHONY: clean
	$(MAKE) -C etree-0.1.1 clean
	$(MAKE) -C lua-iconv-7 clean
	$(MAKE) -C luaxpath clean
	$(MAKE) -C luaexpat clean

