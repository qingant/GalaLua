
MakeMe:all

include ../mk.rules

SQLITE_HOME = $(HOME)/
DES = json struct libzip lfs linenoise md5 sigar time snappy codec 
all:$(DES)
.PHONY:$(DES) static clean

json:
	cd lua-cjson-2.1.0 && $(MAKE) all && mv cjson.so $(HOME)/lib/lua/ && $(MAKE) clean
struct:
	cd lua-struct && $(MAKE) && cp struct.so structX.lua $(HOME)/lib/lua/ && $(MAKE) clean
libzip:
	cd lua-zip && $(MAKE) && $(MAKE) clean
luazip:
	cd lua-zip && $(MAKE)
lfs:
	cd luafilesystem && $(MAKE) lib && $(MAKE) install
encryption:
	cd lua-encryption && $(MAKE) all
linenoise:
	cd lua-linenoise && $(MAKE) all && mv  linenoise.so $(HOME)/lib/lua

md5:
	cd lua-encryption && $(MAKE) all
sigar:
	$(MAKE) -C lua-sigar/src all
	$(MAKE) -C lua-sigar/bindings/lua/ all
codec:
	$(MAKE) -C lua-codec/ all
time:
	$(MAKE) -C lua-time default
snappy:
	$(MAKE) -C lua-snappy/snappy all
	$(MAKE) -C lua-snappy all
static:
	cd lua-cjson-2.1.0 && $(MAKE) static && $(MAKE) clean
	cd lua-struct && $(MAKE) static && $(MAKE) clean
	cd lua-zip && $(MAKE) static
	cd lua-sigar/src && $(MAKE) static && cd ../bindings/lua/ && $(MAKE) static
	cd lua-time/ && $(MAKE) static
	cd lua-snappy/snappy/ && $(MAKE) static
	cd lua-snappy && $(MAKE) static
	cd lua-aimagtpls && $(MAKE) static
	$(MAKE) -C lua-time static
	$(MAKE) -C lua-codec static
#	$(MAKE) -C lua-xml/lua-iconv-7 static
#	$(MAKE) -C lua-xml static

clean::
	$(MAKE) -C lua-cjson-2.1.0 clean
	$(MAKE) -C lua-struct clean
	$(MAKE) -C lua-zip clean
	$(MAKE) -C luafilesystem clean
#	$(MAKE) -C lua-xml clean
	$(MAKE) -C lua-codec clean
	$(MAKE) -C lua-linenoise clean
	$(MAKE) -C lua-encryption clean
	$(MAKE) -C lua-snappy clean
	$(MAKE) -C lua-sigar/src clean
	$(MAKE) -C lua-sigar/bindings/lua/ clean
	$(MAKE) -C lua-time/ clean
	$(MAKE) -C lua-aimagtpls clean
