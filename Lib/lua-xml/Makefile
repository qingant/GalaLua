
all: etree luaiconv luaxpath luaexpat

etree:
	make -C etree-0.1.1
	make -C etree-0.1.1 install

luaiconv:
	make -C lua-iconv-7
	make -C lua-iconv-7 install

luaxpath:
	make -C luaxpath
	make -C luaxpath install

luaexpat:
	make -C luaexpat-1.1
	make -C luaexpat-1.1 install

.PHONY: clean
	make -C etree-0.1.1 clean
	make -C lua-iconv-7 clean
	make -C luaxpath clean
	make -C luaexpat clean
