SQLITE_HOME = $(HOME)/
DES =  json struct libzip lfs cp sqlite dba luaxml
all:$(DES)
.PHONY:$(DES)

cp:
	#cp -rf *.lua $(HOME)/lib/lua/
	cp -rf luadbi.0.5/dbaccess.lua $(HOME)/lib/lua/
sqlite:
	cd sqlite-src-3070900/sqlite-src-3070900 && ./configure --prefix=$(SQLITE_HOME) --disable-tcl --enable-shared --enable-threadsafe --enable-cross-thread-connections && make && make install && make clean
json:
	cd lua-cjson-2.1.0 && make && mv cjson.so $(HOME)/lib/lua/ && make clean
dba:
	cd luadbi.0.5 && make sqlite3 && mv *.so $(HOME)/lib/lua/ && make clean
struct:
	cd lua-struct && make && mv struct.{a,so} $(HOME)/lib/lua/ && make clean
libzip:
	cd lua-zip && make && make clean
luazip:
	cd lua-zip && make
lfs:
	cd luafilesystem && make && make install
luaxml:
	make -C lua-xml