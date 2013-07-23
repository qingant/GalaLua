CC			= clang
COMMINC		= -I.
LNKFLAG		= -shared 
COMPWARN	= -Wall -Wextra 
CMPFLAG		= -O3 -c -g  -fPIC -pthread $(COMPWARN) $(COMMINC) -D_DEBUG_ -D_UNIX95_ -Wno-write-strings  -fno-strict-aliasing  -Wno-missing-field-initializers 
SYTMLIB		= -lc -lz -lm -L.
