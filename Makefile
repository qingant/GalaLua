WHICH_LUA=lua
PLATFORM = $(shell uname)
ifeq ($(PLATFORM), Linux)
	MAKE = make
	WHICH_LUA=luajit
else
	MAKE = gmake
endif

DES = pre header $(WHICH_LUA) stub rt amq cfg lobj lib glr mod db conf tool 

all:$(DES)

pre:
	mkdir -p $(HOME)/bin
	mkdir -p $(HOME)/lib/lua
	mkdir -p $(HOME)/share/static/lua
	mkdir -p $(HOME)/include
header:
	cp -rf include/* $(HOME)/include
stub:
	$(MAKE) -C Stub all
rt:stub
	$(MAKE) -C Runtime
amq:
	$(MAKE) -C AMQ
lua:
	$(MAKE) -C lua-5.2.3 && $(MAKE) -C lua-5.2.3 install
luajit:
	$(MAKE) -C LuaJIT-2.0.0 && $(MAKE) -C LuaJIT-2.0.0 install
glr:$(WHICH_LUA) rt mod
	$(MAKE) -C GLR
lobj:
	$(MAKE) -C LuaObject
lib:
	$(MAKE) -C Lib all
mod:
	$(MAKE) -C Modules
tool:
	$(MAKE) -C Tools
conf:
	$(MAKE) -C Config
db:
	$(MAKE) -C GalaDB
static: pre
ifeq ($(WHICH_LUA), luajit)
	$(MAKE) -C LuaJIT-2.0.0 && $(MAKE) -C LuaJIT-2.0.0 static
else
	$(MAKE) -C lua-5.2.3 && $(MAKE) -C lua-5.2.3 install
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

ifeq ($(WHICH_LUA),luajit)
	$(MAKE) -C LuaJIT-2.0.0 clean
else
	$(MAKE) -C lua-5.2.3 clean
endif
	$(MAKE) -C GLR clean
	$(MAKE) -C LuaObject clean
	$(MAKE) -C Lib clean
	$(MAKE) -C GalaDB clean

.PHONY:$(DES) static clean
