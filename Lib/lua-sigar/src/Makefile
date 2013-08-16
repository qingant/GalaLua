include ./mk.rules
all:libsigar.so
.PHONY:all static clean
OBJTAG =
OBJ = sigar.o sigar_cache.o sigar_fileinfo.o sigar_format.o  sigar_ptql.o \
      sigar_signal.o sigar_util.o sigar_getline.o


OBJ_AIX = $(OBJ) aix_sigar.o
OBJ_HPUX = $(OBJ) hpux_sigar.o
OBJ_LINUX = $(OBJ) linux_sigar.o

PLATFORM      =  $(shell uname)
ifeq ($(PLATFORM),Linux)
OBJTAG = $(OBJ_LINUX)
endif
ifeq ($(PLATFORM),AIX)
OBJTAG = $(OBJ_AIX)
endif
ifeq ($(PLATFORM),unixware)
OBJTAG = $(OBJ_HPUX)
endif

libsigar.so:$(OBJTAG)
	$(GCC) $(LNKFLAG) $(SYTMLIB) -o $@ $?
#	mv $@ $(HOME)/lib
static:libsigar.a
libsigar.a:$(OBJTAG)
	$(AR) $(ARFLAGS) $@ $(OBJTAG)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static
clean:
	rm -rf *.o
