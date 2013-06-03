DES = pre stub rt amq cfg luajit lobj lib glr mod #pyo #app sup 
all:$(DES)
.PHONY:$(DES)

pre:
	mkdir -p $(HOME)/{lib/lua,bin}
stub:
	make -C Stub/
rt:
	make -C Runtime/
amq:
	make -C AMQ/
luajit:
	make -C lua-5.1.5/  && make -C lua-5.1.5/ install
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
