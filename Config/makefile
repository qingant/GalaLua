all:createDir libMTRCofigure.so cfgctl
include ../mk.rules
OBJ = MTRConfigure.o MTRxmlConfigure.o Configure.o
.PHONY:cfgctl
createDir:
	mkdir -p $(HOME)/lib
	mkdir -p $(HOME)/cfg/shmfile
	mkdir -p $(HOME)/bin
libMTRCofigure.so:$(OBJ)
	$(G++) $(LNKFLAG) $? $(PLIBDIR) $(SYTMLIB) -lGalaxyRT -lxml2 -o $@
	mv $@ $(HOME)/lib/


cfgctl:sample/test.o
	$(G++) $? $(PLIBDIR) $(SYTMLIB)	-lMTRCofigure -o $@
	cp MonitorDef.xml $(HOME)/cfg/
	rm -rf $(HOME)/cfg/shmfile/*
	mv $@ $(HOME)/bin/
	cp sample/MonitorDefault.xml	$(HOME)/cfg/
	cd $(HOME)/bin/	&& ./cfgctl	-l $(HOME)/cfg/MonitorDefault.xml -g	0 -f $(HOME)/cfg/shmfile/node.conf
