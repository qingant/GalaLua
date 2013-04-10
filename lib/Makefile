SQLITE_HOME = $(HOME)/Sqlite3
DES = cp sqlite json dba struct zip
all:$(DES)
.PHONY:$(DES)

cp:
	cp -rf *.lua $(HOME)/lib/lua/
	cp -rf luadbi.0.5/dbaccess.lua $(HOME)/lib/lua/
sqlite:
	cd sqlite-src-3070900/sqlite-src-3070900 && ./configure --prefix=$(SQLITE_HOME) --disable-tcl --enable-shared --enable-threadsafe --enable-cross-thread-connections && make && make install && make clean
json:
	cd lua-cjson-2.1.0 && make && mv cjson.so $(HOME)/lib/lua/ && make clean
dba:
	cd luadbi.0.5 && make sqlite3 && mv *.so $(HOME)/lib/lua/ && make clean
struct:
	cd lua-struct && make && mv struct.so $(HOME)/lib/lua/ && make clean
libzip:
	cd libzip-0.11 && ./configure --prefix=$(HOME)/lib --enable-shared=no && make && make install
luazip:
	cd lua-zip && make
