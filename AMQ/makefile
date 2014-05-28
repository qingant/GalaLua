G++		= g++
GCC		= gcc
COMMINC		= -I. -IInclude
LNKFLAG 	= -shared -pthread -fPIC -Wl,--sort-common,-znow
#LNKFLAG 	= -shared -pthread -fPIC -Wl,--sort-common,-s,-znow
#CXXFLAG 	= -c -g -Wall -Weffc++ -fPIC -pthread -O2 -O3 -fwrapv -fbounds-check -fstack-check -fstack-protector-all -Wa,-R,--strip-local-absolute $(COMMINC)
CXXFLAG 	= -c -g -Wall -Weffc++ -fPIC -pthread $(COMMINC)
PLIBDIR		= -L../lib -L$(HOME)/lib
SYTMLIB 	= -lc -lm -ldl -lpthread  -lrt

all:libGalaxyMQ.so


.cpp.S:
	$(G++) -S $(CXXFLAG) -o$@ $<

.cpp.o:
	$(G++) $(CXXFLAG) -o $@ $<

libGalaxyMQ.so:GalaxyMQ/MQException.o GalaxyMQ/MQMedia.o GalaxyMQ/MQ_SQInfor.o GalaxyMQ/MQ_SQAllocator.o GalaxyMQ/MQ_SQSync.o GalaxyMQ/MQ_SQNodes.o GalaxyMQ/MQ_SQueue.o GalaxyMQ/GalaxyMQ.o
	rm -f $@
	$(G++) $(LNKFLAG) $(PLIBDIR) -o $@ $?
	mv $@ $(HOME)/lib/
	cp Include/* $(HOME)/include
	rm -f $?

Test5:Test5.o
	rm -f $@
	$(G++) $(PLIBDIR) -lGalaxyMQ -o $@ $?
	mv $@ $(HOME)/bin/
	rm -f $?

MQSend:Send.o
	rm -f $@
	$(G++) $(PLIBDIR) -lGalaxyMQ -o $@ $?
	mv $@ $(HOME)/bin/
	rm -f $?

MQRecv:Recv.o
	rm -f $@
	$(G++) $(PLIBDIR) -lGalaxyMQ -o $@ $?
	mv $@ $(HOME)/bin/
	rm -f $?
clean::
	rm -rf *.o MQRecv MQSend Test5
