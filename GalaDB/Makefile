all:lmdb lua-lmdb mdb
.PHONY:lmdb lua-lmdb clean

PLATFORM      =  $(shell uname)
ifeq ($(PLATFORM), Linux)
MAKE = make
else
MAKE = gmake
endif

lmdb:
	$(MAKE) -C lmdb
lua-lmdb:
	$(MAKE) -C lua-lmdb
mdb:
	cp -rf mdb.lua $(HOME)/lib/lua

clean:
	$(MAKE) -C lmdb clean
	$(MAKE) -C lua-lmdb clean
