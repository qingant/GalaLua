CC=cc
CFLAGS=-O -g -Wall -D_DEBUG_ 

LIB_ZIP_OBJS = 
LIB_STRZIP_OBJS = _ozip.o io_str.o util.o memfile.o -lz

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $*.c
	
LIB:libozip.so

BIN:ozip_tool

test:

all:LIB	BIN
#all: libozip.so ozip_tool

libozip.so:$(LIB_STRZIP_OBJS)
	@echo "libozip.so"
	rm -rf $@
	$(CC) -fPIC -shared $(CFLAGS) -o $@ $(LIB_STRZIP_OBJS)

ozip_tool:ozip_tool.o 
	@echo " == ozip_tool == "
	$(CC) $(CFLAGS) -o $@ ozip_tool.o  -lozip -lz -L.
	
clean:
	/bin/rm -f *.o  *.so
	/bin/rm -f ozip_tool