DES = stub rt glr lobj app sup lib
all:$(DES)
.PHONY:$(DES)
stub:
	make -C Stub/
rt:
	make -C Runtime/
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
