include ./mk.rules
all:libzz.so
.PHONY:all static clean
OBJTAG = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o \
      inflate.o infback.o inftrees.o inffast.o trees.o uncompr.o zutil.o

libzz.so:$(OBJTAG)
	$(GCC) $(LNKFLAG) $(SYTMLIB) -o $@ $?
	mv $@ $(HOME)/lib/
static:libzz.a
libzz.a:$(OBJTAG)
	$(AR) $(ARFLAGS) $@ $(OBJTAG)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static
clean:
	rm -rf *.o *.a *.so
