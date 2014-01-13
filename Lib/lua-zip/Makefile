OBJ=lua_minizip.o


all:zip minizip

include ../../mk.rules

LDFLG   = $(LNKFLAG)
COMMINC += -I./libzip -I./zlib 
LIBZIP_ZLIB =

ifeq ($(PLATFORM),Linux)
	CFLAGS     += -ansi -std=gnu99  $(COMMINC)
endif

zip:
	$(MAKE) -C zlib all
	$(MAKE) -Clibzip all


minizip:lua_minizip.o
	$(CC) $(LDFLG) $<  -o minizip.so -lzip $(PLIBDIR)
	cp -rf minizip.so $(HOME)/lib/lua

static: libzip.a minizip.a

minizip.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@
	cp -rf minizip.a $(HOME)/share/static/lua
	rm -f minizip.a

libzip.a:
	$(MAKE) -C zlib static
	$(MAKE) -C libzip static

clean:
	-rm -f *.o *.a *.so
	-rm -rf libzip-0.11.1/autom4te.cache
	@if [ -f libzip-0.11.1/Makefile ]; then \
		$(MAKE) -C libzip-0.11.1 clean; \
		$(MAKE) -C libzip-0.11.1 distclean; \
	fi

