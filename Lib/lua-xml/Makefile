OS=$(shell uname)
ifeq ($(OS),AIX)
MAKE = make
else
MAKE = gmake
endif
all: etree luaiconv luaxpath luaexpat

etree:
	$(MAKE) -C etree-0.1.1
	$(MAKE) -C etree-0.1.1 install

luaiconv:
	$(MAKE) -C lua-iconv-7
	$(MAKE) -C lua-iconv-7 install

luaxpath:
	$(MAKE) -C luaxpath
	$(MAKE) -C luaxpath install

luaexpat:
	$(MAKE) -C luaexpat-1.1
	$(MAKE) -C luaexpat-1.1 install

.PHONY: clean
	$(MAKE) -C etree-0.1.1 clean
	$(MAKE) -C lua-iconv-7 clean
	$(MAKE) -C luaxpath clean
	$(MAKE) -C luaexpat clean
