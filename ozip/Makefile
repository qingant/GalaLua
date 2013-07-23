
include common.mk

.c.o:
	$(CC) $(CMPFLAG) -o $@ $<

UNZ_OBJS	= miniunz.o unzip.o ioapi.o -lz
ZIP_OBJS	= minizip.o zip.o   ioapi.o -lz
LIB_OBJS	= ozip.o ioapi.o iostr.o unzip.o zip.o memfile.o -lz
OZIP_OBJS	= ozip_lua.o ozip.o ioapi.o iostr.o unzip.o zip.o memfile.o -llua5.1 -lz

all: libozip.so _ozip.so test1
	
libozip.so: $(LIB_OBJS)
	$(CC) $(LNKFLAG) -o $@ $(LIB_OBJS) $(SYTMLIB) 
	
_ozip.so: $(OZIP_OBJS)
	$(CC) $(LNKFLAG) -o $@ $(OZIP_OBJS) $(SYTMLIB) 
	
test1:sample/strzip_test.o
	$(CC) -o $@ sample/strzip_test.o -lozip -L.
	
	
install:


.PHONY:clean

clean:
	/bin/rm -f *.o *~ minizip miniunz libozip.so _ozip.so test1 *.zip sample/*.o
