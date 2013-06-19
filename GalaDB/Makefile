all:lmdb lua-lmdb mdb
.PHONY:lmdb lua-lmdb

lmdb:
	make -C lmdb/ && make -C lmdb install
lua-lmdb:
	make -C lua-lmdb/
mdb:
	cp -rf mdb.lua $(HOME)/lib/lua/
