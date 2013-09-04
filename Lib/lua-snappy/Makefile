include ./mk.rules
all:snappy.so
.PHONY:all static clean
OBJTAG = lua-snappy.o

snappy.so:$(OBJTAG)
	$(G++) $(LNKFLAG) $(SYTMLIB) $(PLIBDIR)/libsnappy.so -o $@ $?
	mv $@ $(HOME)/lib/
static:snappy.a
snappy.a:$(OBJTAG)
	$(AR) $(ARFLAGS) $@ $(OBJTAG)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static
clean:
	rm -rf *.o *.a *.so
