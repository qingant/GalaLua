
include ../mk.rules

OS=$(shell uname)
ifeq ($(OS),AIX)
	MAKE = gmake
else
	MAKE = make
endif

SQLITE_HOME = $(HOME)/
DES = json struct libzip lfs #luaxml cp sqlite dba
all:$(DES)
.PHONY:$(DES) static

cp:
	#cp -rf *.lua $(HOME)/lib/lua/
	cp -rf luadbi.0.5/dbaccess.lua $(HOME)/lib/lua/
sqlite:
	cd sqlite-src-3070900/sqlite-src-3070900 && ./configure --prefix=$(SQLITE_HOME) --disable-tcl --enable-shared --enable-threadsafe --enable-cross-thread-connections && $(MAKE) && $(MAKE) install && $(MAKE) clean
json:
	cd lua-cjson-2.1.0 && $(MAKE) && mv cjson.so $(HOME)/lib/lua/ && $(MAKE) clean
dba:
	cd luadbi.0.5 && $(MAKE) sqlite3 && mv *.so $(HOME)/lib/lua/ && $(MAKE) clean
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
	cd lua-linenoise && $(MAKE) all
resource:
	cd resource && $(MAKE) all

static:
	cd lua-cjson-2.1.0 && $(MAKE) static && $(MAKE) clean
	cd lua-struct && $(MAKE) static && $(MAKE) clean
	cd lua-zip && $(MAKE) static
	$(MAKE) -C lua-xml

