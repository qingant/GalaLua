DES = pre stub rt amq cfg luajit glr lobj lib pyo #app sup 
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
cfg:
	make -C Config/
luajit:
	make -C LuaJIT-2.0.0/  && make -C LuaJIT-2.0.0/ install 
glr:
	make -C GLR/
lobj:
	make -C LuaObject/
pyo:
	make -C PyObject/
app:
	make -C App/
sup:
	make -C Supervisor/
lib:
	make -C lib/
