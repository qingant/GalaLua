include ../../mk.rules

all: liblmdb.so  

clean:
	rm -rf *so
	rm -rf *.o

test1:	all test 
	mkdir testdb

liblmdb.a:	mdb.o midl.o
	$(AR) $(ARFLAGS) $@ mdb.o midl.o
	#mv $@ $(HOME)/lib/

liblmdb.so:	mdb.o midl.o
	$(GCC) $(LNKFLAG) $(SYTMLIB) -o $@ $?
	mv $@ $(HOME)/lib/

mdb_stat: mdb_stat.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
mdb_copy: mdb_copy.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
	mv $@ $(HOME)/bin/
mtest:mtest.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
mtest2:	mtest2.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
mtest3:	mtest3.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
mtest4:	mtest4.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
mtest5:	mtest5.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?
mtest6:	mtest6.o
	$(GCC) -L$(HOME)/lib -L. -llmdb -o $@ $?

.PHONY: all test clean
