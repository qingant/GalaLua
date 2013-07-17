# luaiconv - Performs character set conversions in Lua
# (c) 2005-10 Alexandre Erwin Ittner <alexandre@ittner.com.br>
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# If you use this package in a product, an acknowledgment in the product
# documentation would be greatly appreciated (but it is not required).

PLATFORM=$(shell uname)
LDFLAG   = none
ifeq (${PLATFORM},Linux)
	CC          = gcc -ansi -Wall -O2 -fPIC -I../../../LuaJIT-2.0.0/src/
	CFLAGS      = -I$(PREFIX)/include/luajit-2.0 -fPIC -O3 -Wall -fomit-frame-pointer
	LDFLAGS     = -shared
	AR          = ar
	ARFLAGS     = -rcu
	RANLIB      = ranlib
	RANLIBFLAGS =
else ifeq (${PLATFORM},AIX)
	CC          = xlc_r
	CFLAGS      = -q64 -O2 -I../../../lua-5.1.5/src
	LDFLAGS     = -qmkshrobj -L$(HOME)/lib -lluajit -lm
	AR          = ar
	ARFLAGS     = -X64 -rcu
	RANLIB      = ranlib
	RANLIBFLAGS = -X64
endif

#RM = rm

PREFIX = $(HOME)

# Gives a nice speedup, but also spoils debugging on x86. Comment out this
# line when debugging.
OMIT_FRAME_PTR = -fomit-frame-pointer

LUABIN = luajit
LUAPKG = luajit
LIBS   = -L$(PREFIX)/lib -lluajit

## If your system doesn't have pkg-config or if you do not want to get the
## install path from Lua, comment out the previous lines and uncomment and
## change the following ones according to your building environment.

#CFLAGS = -I/usr/local/include/ -fPIC -O3 -Wall $(OMIT_FRAME_PTR)
#LDFLAGS = -shared
#INSTALL_PATH = /usr/local/lib/lua/5.2/


all: iconv.so iconv.a

iconv.o: luaiconv.c
	$(CC) -o $@ -c $(CFLAGS) $<

iconv.so: iconv.o
	$(CC) -o $@ $< $(LDFLAGS) $(LIBS)

iconv.a: iconv.o
	$(AR) $(ARFLAGS) $@ $<
	$(RANLIB) $(RANLIBFLAGS) $@

install: iconv.so iconv.a
	cp iconv.so $(PREFIX)/lib/lua
	cp iconv.a $(HOME)/share/static/lua

clean:
	$(RM) iconv.so iconv.o iconv.a

test: iconv.so test_iconv.lua
	luajit test_iconv.lua

