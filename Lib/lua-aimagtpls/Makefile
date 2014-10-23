include ../../mk.rules
OBJ=aimagtpls.o

default: $(OBJ)
	$(CC) $(LDFLG) -o aimagtpls.so  $(OBJ)
	cp -rf  aimagtpls.so $(HOME)/lib/lua/
.PHONY: static

static: aimagtpls.a

aimagtpls.a: $(OBJ)
	$(AR) $(ARFLAGS) aimagtpls.a $(OBJ)
	$(RANLIB) $(RANLIBFLAGS) aimagtpls.a
	mv aimagtpls.a $(HOME)/share/static/lua

clean::
	rm -f $(OBJ)
	rm -f aimagtpls.so aimagtpls.a

