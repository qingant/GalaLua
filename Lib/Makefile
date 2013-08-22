
MakeMe:all

include ../mk.rules

OS=$(shell uname)
ifeq ($(OS),AIX)
	MAKE = gmake
else
	MAKE = make
endif

SQLITE_HOME = $(HOME)/
DES = json struct libzip lfs linenoise md5 sigar time #luaxml cp sqlite dba
all:$(DES)
.PHONY:$(DES) static clean

cp:
	#cp -rf *.lua $(HOME)/lib/lua/
	cp -rf luadbi.0.5/dbaccess.lua $(HOME)/lib/lua/
sqlite:
	cd sqlite-src-3070900/sqlite-src-3070900 && ./configure --prefix=$(SQLITE_HOME) --disable-tcl --enable-shared --enable-threadsafe --enable-cross-thread-connections && $(MAKE) && $(MAKE) install && $(MAKE) clean
json:
	cd lua-cjson-2.1.0 && $(MAKE) all && mv cjson.so $(HOME)/lib/lua/ && $(MAKE) clean
dba:
	cd luasql/src && $(MAKE) all && mv *.so $(HOME)/lib/lua/ && $(MAKE) clean
struct:
	cd lua-struct && $(MAKE) && cp struct.so structX.lua $(HOME)/lib/lua/ && $(MAKE) clean
libzip:
	cd lua-zip && $(MAKE) && $(MAKE) clean
luazip:
	cd lua-zip && $(MAKE)
lfs:
	cd luafilesystem && $(MAKE) lib && $(MAKE) install
luaxml:
	$(MAKE) -C lua-xml all
encryption:
	cd lua-encryption && $(MAKE) all
linenoise:
	cd lua-linenoise && $(MAKE) all && mv  linenoise.so $(HOME)/lib/lua

md5:
	cd lua-encryption && $(MAKE) all
resource:
	cd resource && $(MAKE) all
sigar:
	cd lua-sigar/src && make && cd ../bindings/lua/ && make
time:
	cd lua-time/ && make
static:
	cd lua-cjson-2.1.0 && $(MAKE) static && $(MAKE) clean
	cd lua-struct && $(MAKE) static && $(MAKE) clean
	cd lua-zip && $(MAKE) static
	cd lua-sigar/src && make static && cd ../bindings/lua/ && make static
	cd lua-time/ && make static
	$(MAKE) -C lua-time static
#	$(MAKE) -C lua-xml/lua-iconv-7 static
#	$(MAKE) -C lua-xml static

clean:
	$(MAKE) -C lua-cjson-2.1.0 clean
	$(MAKE) -C lua-struct clean
	$(MAKE) -C lua-zip clean
	$(MAKE) -C luafilesystem clean
	$(MAKE) -C lua-xml clean
