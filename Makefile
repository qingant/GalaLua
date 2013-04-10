DES = stub rt amq cfg glr lobj app sup lib
all:$(DES)
.PHONY:$(DES)

stub:
	make -C Stub/
rt:
	make -C Runtime/
amq:
	make -C AMQ/
cfg:
	make -C Config/
glr:
	make -C GLR/
lobj:
	make -C LuaObject/
app:
	make -C App/
sup:
	make -C Supervisor/
lib:
	make -C lib/
