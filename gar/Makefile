
all:walk.so
include ../mk.rules
walk.so:walk.o
	@echo $(PLATFORM)
	$(GCC) $? $(LNKFLAG)  $(SYTMLIB) -lluajit -o $@

clean:
	rm -rf *.o 
	rm -rf walk.so

