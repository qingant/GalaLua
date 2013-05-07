all:libGalaxyQueue.so 
test:Test5 Test8 Test7 Test9
include ../mk.rules


OBJS = GalaxyMQ.o    MQBlock.o    MQDQ.o         MQMutex.o  MQPooler.o   MQBuilder.o  MQException.o  MQNodes.o  MQSuite.o MQAnchor.o    MQCond.o     MQMedia.o      MQNQ.o     MQView.o MQLicense.o MQCarrier.o
libGalaxyQueue.so:$(OBJS)
	@echo $(PLATFORM)
	$(G++) $(LNKFLAG) $? $(SYTMLIB) $(STUBLIB) -o $@
	mv $@ $(HOME)/lib/

Test5:Sample/Test5.o
	rm -f $@
	$(G++) $? -lGalaxyQueue $(STUBLIB) $(SYTMLIB)  -o $@
	mv $@ $(HOME)/bin/


Test7:Sample/Test7.o
	rm -f $@
	$(G++) $? -lGalaxyQueue $(STUBLIB) $(SYTMLIB)  -o $@
	mv $@ $(HOME)/bin/
	rm -f $?

Test8:Sample/Test8.o
	rm -f $@
	$(G++) $? -lGalaxyQueue $(STUBLIB) $(SYTMLIB)  -o $@
	mv $@ $(HOME)/bin/
	rm -f $?

Test9:Sample/Test9.o
	rm -f $@
	$(G++) $? -lGalaxyQueue	$(STUBLIB) $(SYTMLIB)  -o $@
	mv $@ $(HOME)/bin/
	rm -f $?

test_block:Sample/test_block.o
	rm -f $@
	$(G++) $? -lGalaxyQueue $(STUBLIB) $(SYTMLIB)  -o $@
	mv $@ $(HOME)/bin/
	rm -f $?
