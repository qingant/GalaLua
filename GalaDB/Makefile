all:lmdb lua-lmdb mdb
.PHONY:lmdb lua-lmdb clean

lmdb:
	make -C lmdb && make -C lmdb install
lua-lmdb:
	make -C lua-lmdb && cp -rf lua-lmdb/lightningmdb.so $(HOME)/lib/lua
mdb:
	cp -rf mdb.lua $(HOME)/lib/lua

clean:
	make -C lmdb clean
	make -C lua-lmdb clean
