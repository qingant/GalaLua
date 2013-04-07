#all:zip.so
#zip.so:
# 	gcc -shared  -fPIC -ansi -fstack-check lua_zip.c  -llua -o zip.so -I/usr/local/include/ -I/usr/local/lib/libzip/include/ -lzzip -lzip -L /usr/local/lib/
# 	mv $@ $(HOME)/lib/lua/
# 
#clean:
# 	rm -rf *.o
# 	rm -rf *.so
# 
all:
	cp zloader.lua $(HOME)/lib/lua