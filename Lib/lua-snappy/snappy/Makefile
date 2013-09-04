include ./mk.rules
all:libsnappy.so
.PHONY:all static clean
OBJTAG = snappy.o snappy-c.o snappy-sinksource.o snappy-stubs-internal.o

libsnappy.so:$(OBJTAG)
	$(G++) $(LNKFLAG) $(SYTMLIB) -o $@ $?
	mv $@ $(HOME)/lib/
static:libsnappy.a
libsnappy.a:$(OBJTAG)
	$(AR) $(ARFLAGS) $@ $(OBJTAG)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static
clean:
	rm -rf *.o *.a *.so
