OS=$(shell uname)

OBJECTS=linenoise.o

ifeq ($(LIBLINENOISE),)
  OBJECTS+=linenoiselib.o
else
  OPT_LIB=$(LIBLINENOISE)
endif

ifeq ($(OS),Darwin)
linenoise.dylib: $(OBJECTS)
	gcc -o $@ -bundle -undefined dynamic_lookup $^ $(OPT_LIB)
else
CFLAGS=-fPIC -I/opt/agree/gdk/include/luajit-2.0
linenoise.so: $(OBJECTS)
	gcc -o $@ -shared $^ $(OPT_LIB)
endif

clean:
	rm -f *.o *.so *.dylib
