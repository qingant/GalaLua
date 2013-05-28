
include config.mk

VERSION = 0.1.1
ETREE = etree-$(VERSION)

.PHONY: install
install: doc
	install -d $(libdir) $(docdir)/etree
	install src/etree.lua $(libdir)

.PHONY: clean
clean:
	rm -rf *~ *.tar.gz
	rm -rf build
	rm -rf doc/*~
	rm -rf src/*~
	make -C doc clean

