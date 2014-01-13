all:snappy.so
include ../../mk.rules
.PHONY:all static clean
OBJTAG = lua-snappy.o

snappy.so:$(OBJTAG)
	$(G++) $(LNKFLAG) $(SYTMLIB) -lsnappy -o $@ $?
	mv $@ $(HOME)/lib/lua

static:snappy.a
snappy.a:$(OBJTAG)
	$(AR) $(ARFLAGS) $@ $(OBJTAG)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static
clean:
	rm -rf *.o *.a *.so
	make -C snappy/ clean
