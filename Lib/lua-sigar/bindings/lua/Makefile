all:sigar.so
include ../../../../mk.rules
.PHONY:all static 

COMMINC += -I../../include/

OBJ = sigar.o sigar-cpu.o sigar-disk.o sigar-fs.o sigar-mem.o sigar-netif.o \
      sigar-proc.o sigar-swap.o sigar-sysinfo.o sigar-who.o sigar-version.o
sigar.so:$(OBJ)
	$(GCC) $(LNKFLAG) $(SYTMLIB) -lsigar -L$(HOME)/lib -o $@ $?
	mv $@ $(HOME)/lib/lua
	cp *.lua $(HOME)/lib/lua
static:sigar.a
sigar.a:$(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static/lua
	cp *.lua $(HOME)/lib/lua
