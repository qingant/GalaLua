DES = pre stub rt amq cfg lua lobj lib glr mod db #pyo #app sup 
all:$(DES)
.PHONY:$(DES) static clean
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
	make -C Stub all
rt:
	make -C Runtime
amq:
	make -C AMQ
lua51:
	make -C lua-5.1.5 && make -C lua-5.1.5 install
luajit:
	make -C LuaJIT-2.0.0 && make -C LuaJIT-2.0.0 install
glr:luajit
	make -C GLR
lobj:
	make -C LuaObject
# pyo:
# 	make -C PyObject
lib:
	make -C Lib all
mod:
	make -C Modules
db:
	make -C GalaDB

static: pre
	make -C LuaJIT-2.0.0 && make -C LuaJIT-2.0.0 static
	make -C Stub static
	make -C Runtime static
	make -C GLR static
	make -C Lib static
	make -C LuaObject static

clean:
	make -C Stub clean
	make -C Runtime clean
	make -C AMQ clean
	make -C lua-5.1.5 clean
	make -C LuaJIT-2.0.0 clean
	make -C GLR clean
	make -C LuaObject clean
	make -C Lib clean	
	make -C GalaDB clean
