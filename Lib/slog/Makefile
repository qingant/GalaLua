all: slog.so
include ../../mk.rules
CC = g++

INC = /usr/local/include/lua
LIB = /usr/local/lib


SLOG_DIR = src
READ_STREAM_DIR = src/read_stream
WRITE_STREAM_DIR = src/write_stream

CXXFLAGS = -DTOLUA_API='extern "C"' -Wall -shared -fPIC -I$(INC) -L$(LIB)  -lm -ldl   -llua 



SLOG_SRC = $(SLOG_DIR)/lua_slog.cpp
SLOG_OBJ = $(SLOG_DIR)/lua_slog.o

WRITE_STREAM_SRC = $(WRITE_STREAM_DIR)/write_stream.cpp
WRITE_STREAM_HF = $(WRITE_STREAM_DIR)/write_stream.h 
WRITE_STREAM_OBJ = $(WRITE_STREAM_DIR)/write_stream.o

LUA_WRITE_STREAM_SRC = $(WRITE_STREAM_DIR)/lua_write_stream.cpp
LUA_WRITE_STREAM_HF = $(WRITE_STREAM_DIR)/lua_write_stream.h 
LUA_WRITE_STREAM_OBJ = $(WRITE_STREAM_DIR)/lua_write_stream.o

READ_STREAM_SRC = $(READ_STREAM_DIR)/read_stream.cpp
READ_STREAM_HF = $(READ_STREAM_DIR)/read_stream.h
READ_STREAM_OBJ = $(READ_STREAM_DIR)/read_stream.o

LUA_READ_STREAM_SRC = $(READ_STREAM_DIR)/lua_read_stream.cpp
LUA_READ_STREAM_HF = $(READ_STREAM_DIR)/lua_read_stream.h
LUA_READ_STREAM_OBJ = $(READ_STREAM_DIR)/lua_read_stream.o


OBJ =  $(SLOG_OBJ)  \
	   $(WRITE_STREAM_OBJ) $(LUA_WRITE_STREAM_OBJ) \
	   $(READ_STREAM_OBJ) $(LUA_READ_STREAM_OBJ) \

.PHONEY: all linux clean

slog.so:$(OBJ)
	@echo "[CC]    slog.so"
	$(CC) $(CXXFLAGS) -o $@ $?

slog.o:$(SLOG_SRC)
	@echo "[CC]    slog.o"
	$(CC) $(CXXFLAGS) -c -o $(SLOG_DIR)/$@ $(SLOG_SRC)

write_stream.o:$(WRITE_STREAM_SRC) $(WRITE_STREAM_HF) $(BSON_HF)
	@echo "[CC]    write_stream.o"
	$(CC) $(CXXFLAGS) -c -o $(WRITE_STREAM_DIR)/$@ $?

lua_write_stream.o:$(LUA_WRITE_STREAM_SRC) $(LUA_WRITE_STREAM_HF) $(WRITE_STREAM_HF) $(BSON_HF)
	@echo "[CC]    lua_write_stream.o"
	$(CC) $(CXXFLAGS) -c -o $(WRITE_STREAM_DIR)/$@ $?

read_stream.o:$(READ_STREAM_SRC) $(READ_STREAM_HF)
	@echo "[CC]    read_stream.o"
	$(CC) $(CXXFLAGS) -c -o $(READ_STREAM_DIR)/$@ $?

lua_read_stream.o:$(LUA_READ_STREAM_SRC) $(LUA_READ_STREAM_HF)
	@echo "[CC]    lua_read_stream.o"
	$(CC) $(CXXFLAGS) -c -o $(READ_STREAM_DIR)/$@ $?

clean:
	find . -name "*.o" -exec rm -f '{}' \;
	find . -name "*.so" -exec rm -f '{}' \;