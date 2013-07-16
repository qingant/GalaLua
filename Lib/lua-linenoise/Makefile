
OS=$(shell uname)
include ../mk.rules

OBJECTS=linenoise.o
BUILDOBJ = none
ifeq ($(LIBLINENOISE),)
  OBJECTS+=linenoiselib.o
else
  OPT_LIB=$(LIBLINENOISE)
endif
ifeq ($(OS),Darwin)
BUILDOBJ = linenoise.dylib
else
BUILDOBJ = linenoise.so
endif
all:$(BUILDOBJ)
ifeq ($(OS),Darwin)
linenoise.dylib: $(OBJECTS)
	gcc -o $@ -bundle -undefined dynamic_lookup $^ $(OPT_LIB)
else
linenoise.so: $(OBJECTS)
	$(GCC) -o $@ $(LNKFLAG) $(SYTMLIB) $^ $(OPT_LIB) $(LIBDIR)
endif

