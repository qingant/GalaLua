DES = pre stub rt amq cfg lua lobj lib glr mod db #pyo #app sup
all:$(DES)
.PHONY:$(DES) static clean
PLATFORM      =  $(shell uname)
ifeq ($(PLATFORM), Linux)
lua:luajit
MAKE = make
else
lua:lua51
MAKE = gmake
endif

pre:
	mkdir -p $(HOME)/bin
	mkdir -p $(HOME)/lib/lua
	mkdir -p $(HOME)/share/static/lua
stub:
	$(MAKE) -C Stub all
rt:
	$(MAKE) -C Runtime
amq:
	$(MAKE) -C AMQ
lua51:
	$(MAKE) -C lua-5.1.5 && $(MAKE) -C lua-5.1.5 install
luajit:
	$(MAKE) -C LuaJIT-2.0.0 && $(MAKE) -C LuaJIT-2.0.0 install
glr:lua
	$(MAKE) -C GLR
lobj:
	$(MAKE) -C LuaObject
# pyo:
# 	$(MAKE) -C PyObject
lib:
	$(MAKE) -C Lib all
mod:
	$(MAKE) -C Modules
db:
	$(MAKE) -C GalaDB
static: pre
ifeq ($(PLATFORM), Linux)
	$(MAKE) -C LuaJIT-2.0.0 && $(MAKE) -C LuaJIT-2.0.0 static
else
	$(MAKE) -C lua-5.1.5 && $(MAKE) -C lua-5.1.5 install
endif
	$(MAKE) -C Stub static
	$(MAKE) -C Runtime static
	$(MAKE) -C GLR static
	$(MAKE) -C LuaObject static
	$(MAKE) -C Lib static

clean:
	$(MAKE) -C Stub clean
	$(MAKE) -C Runtime clean
	$(MAKE) -C AMQ clean
ifeq ($(PLATFORM), Linux)
	$(MAKE) -C LuaJIT-2.0.0 clean
else
	$(MAKE) -C lua-5.1.5 clean
endif
	$(MAKE) -C GLR clean
	$(MAKE) -C LuaObject clean
	$(MAKE) -C Lib clean
	$(MAKE) -C GalaDB clean
