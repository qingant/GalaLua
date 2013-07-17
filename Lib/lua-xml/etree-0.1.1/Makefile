
include config.mk

VERSION = 0.1.1
ETREE = etree-$(VERSION)

.PHONY: install

OS = $(shell uname -s)
ifeq ($(OS), Linux)
install: doc
	install -d $(libdir) $(docdir)/etree
	install src/etree.lua $(libdir)
endif

ifeq ($(OS), AIX)
install: doc
	mkdir -p $(libdir)
	cp src/etree.lua $(libdir)
endif

.PHONY: clean
clean:
	rm -rf *~ *.tar.gz
	rm -rf build
	rm -rf doc/*~
	rm -rf src/*~
	make -C doc clean

