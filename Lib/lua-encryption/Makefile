#Makefile
#
# Created on: 2013-5-9
#     Author: ocaml
#
#

all:
	gcc -shared -fPIC -g -Wall md5.c -o libmd5.so
	gcc -g -Wall test_md5.c -o test_md5 -lmd5 -L.
	gcc -shared  -fPIC -ansi -fstack-check md5.c lua_md5.c  -o md5.so -lluajit -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib
	cp -rf libmd5.so $(HOME)/lib
	cp -rf md5.so $(HOME)/lib/lua
	cp -rf md5.lua $(HOME)/lib/lua 
clean:
	rm -rf *.so
	rm -rf test_md5
	rm -rf *.o
	
test: