DES = pre stub rt amq cfg lua lobj lib glr mod db #pyo #app sup 
all:$(DES)
.PHONY:$(DES)
PLATFORM      =  $(shell uname)
ifeq ($(PLATFORM), Linux)
lua:luajit
else
lua:lua51
endif

pre:
	mkdir -p $(HOME)/bin
	mkdir -p $(HOME)/lib/lua
	mkdir -p $(HOME)/share/static/lua
stub:
	make -C Stub/ all
rt:
	make -C Runtime/
amq:
	make -C AMQ/
lua51:
	make -C lua-5.1.5/  && make -C lua-5.1.5/ install
luajit:
	make -C LuaJIT-2.0.0/ && make -C LuaJIT-2.0.0/ install
glr:luajit
	make -C GLR/
lobj:
	make -C LuaObject/
# pyo:
# 	make -C PyObject/
lib:
	make -C Lib/
mod:
	make -C Modules/
db:
	make -C GalaDB
