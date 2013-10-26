all: libcodec.so codec.so
include ../mk.rules
OBJS=codec_cn.o codec_utf8.o
libcodec.so:$(OBJS)
	@echo $(PLATFORM)
	$(G++) $? $(LNKFLAG) -o $@
	mv $@ $(HOME)/lib/

codec.so:LuaCodec.o
	$(G++) $? $(LNKFLAG) $(SYTMLIB) -lcodec -o $@
	mv $@ $(HOME)/lib/lua/

clean:
	rm -rf *.o *.so

static: codec.a

codec.a: LuaCodec.o $(OBJS)
	$(AR) $(ARFLAGS) $@ LuaCodec.o $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $@
	mv $@ $(HOME)/share/static/lua
