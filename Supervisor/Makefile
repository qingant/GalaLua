all:supervisor cmd 
include ../mk.rules
COMPATH = $(HOME)/lib/gala/

pre:
	mkdir -p $(COMPATH)
supervisor:pre
	cp -rf supervisor $(COMPATH)
	cp -rf meld3/meld3 $(COMPATH)
#	sh setuptools-0.6c11-py2.7.egg
	ln -sf ../lib/gala/supervisor/supervisord.py $(HOME)/bin/AIMGuard
cmd:pre
	cp -rf Tools/*  $(COMPATH)/
	cp -rf urwid/urwid      $(COMPATH)/
	ln -sf ../lib/gala/aimcmd.py $(HOME)/bin/aimcmd
