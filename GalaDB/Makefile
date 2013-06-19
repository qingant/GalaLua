all:lmdb lua-lmdb mdb
.PHONY:lmdb lua-lmdb

lmdb:
	make -C lmdb/
lua-lmdb:
	make -C lua-lmdb/
mdb:
	cp -rf mdb.lua $(HOME)/lib/lua/
