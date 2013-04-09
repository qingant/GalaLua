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
	cd zziplib-0.13.49 && ./configure --prefix=$(HOME)/lib && make && make check && make install
	cd luazip-1.2.3 && make
	cp zloader.lua $(HOME)/lib/lua

clean:
	#find -name "*.o" | xargs rm
	#find -name "*.so" | xargs rm
