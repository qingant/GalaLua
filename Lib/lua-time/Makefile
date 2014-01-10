include ../../mk.rules

OBJ=osdatetime.o

default: $(OBJ)
	$(CC)  $(LDFLAGS) -o osdatetime.so  $(OBJ)
	cp -rf  osdatetime.so $(HOME)/lib/lua/
.PHONY: static

static: osdatetime.a

osdatetime.a: $(OBJ)
	$(AR) $(ARFLAGS) osdatetime.a $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) osdatetime.a
	cp osdatetime.a $(HOME)/share/static/lua

clean:
	rm -f $(OBJ)
	rm -f osdatetime.so osdatetime.a

