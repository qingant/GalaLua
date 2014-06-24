
#include <lua.hpp>
#include "read_stream/lua_read_stream.h"
#include "write_stream/lua_write_stream.h"

extern "C"
int luaopen_slog(lua_State* L)
{
	lua_newtable(L);

	lua_pushstring(L, "create_writer");
	lua_pushcfunction(L, write_stream_initialize);
	lua_settable(L, -3);

	lua_pushstring(L, "create_reader");
	lua_pushcfunction(L, read_stream_initialize);
	lua_settable(L, -3);

	return 1;
}