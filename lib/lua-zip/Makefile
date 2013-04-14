#all:zip.so
#zip.so:
# 
# 	mv $@ $(HOME)/lib/lua/
# 
#clean:
# 	rm -rf *.o
# 	rm -rf *.so
# 
all:
	gcc -shared  -fPIC -ansi -fstack-check lua_minizip.c  -o minizip.so -lluajit -lzip -Ilibzip-0.11/lib -I../../LuaJIT-2.0.0/src/ -L$(HOME)/lib/  -L$(HOME)/lib/lib 
	cp zloader.lua $(HOME)/lib/lua

clean:
	#find -name "*.o" | xargs rm

