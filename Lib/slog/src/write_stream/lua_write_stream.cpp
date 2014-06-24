
#include <iostream>
#include <string.h>
#include "lua_write_stream.h"

int write_stream_finalize(lua_State *L);
int write_binary(lua_State *L);
int write_stream_get_info(lua_State *L);
int set_index_zone_size(lua_State *L);
int write_stream_backup(lua_State *L);
int write_stream_write_vector(lua_State* L);

int write_stream_initialize(lua_State*L)
{
	const char* str = NULL;
	size_t size = 0;	

	str = lua_tolstring(L, 1, &size);
	if(str == NULL)
	{
		//lua_pushstring(L, "log name couldn't be null.");
		luaL_error(L,"log name couldn't be null.");
	}
	
	
	*(write_stream**) lua_newuserdata(L, sizeof(write_stream*)) = new write_stream(str);
	
	lua_newtable(L);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, write_stream_finalize);
	lua_settable(L, -3);

	lua_pushstring(L, "write");
	lua_pushcfunction(L, write_binary);
	lua_settable(L, -3);

	lua_pushstring(L, "write_array");
	lua_pushcfunction(L, write_stream_write_vector);
	lua_settable(L, -3);

	lua_pushstring(L, "backup");
	lua_pushcfunction(L, write_stream_backup);
	lua_settable(L, -3);
	
	lua_pushstring(L, "get_info");
	lua_pushcfunction(L, write_stream_get_info);
	lua_settable(L, -3);

	lua_pushstring(L, "set_index_zone_size");
	lua_pushcfunction(L, set_index_zone_size);
	lua_settable(L, -3);

	
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);	
	lua_settable(L, -3);

	lua_setmetatable(L, -2);	
	return 1;
}

int write_stream_backup(lua_State *L)
{
	write_stream * ws = NULL;
	const char* toName;
	ws = *(write_stream**) lua_topointer(L,1);
	toName = lua_tostring(L,2);
	ws->backup(toName);
	return 0;
}
int write_stream_finalize(lua_State *L)
{
	//std::cout<<"I come here."<<std::endl;
	delete *(write_stream **) lua_topointer(L,1);
	return 0;
}

int write_binary(lua_State *L)
{
	//std::cout<<"write_binary"<<std::endl;
	size_t s = 0;
	uint8_t* ptr;
	write_stream *ws =NULL;
	ws = *(write_stream**) lua_topointer(L, 1);
	//std::cout<<ws<<std::endl;
	//struct bson b;
	//bson_create(&b);
	//luaL_checks(L, 2, LUA_TSTRING);

	if(lua_type(L, 2) == LUA_TSTRING)
	{
		ptr =(uint8_t*) lua_tolstring(L,2, &s);
		s = ws->write_binary(ptr, s);
	}
	
	//lua_pushvalue(L,2);
	//pack_dict(L, &b, false);
	
	//bson_destroy(&b);
	//lua_pop(L,1);
	lua_pushnumber(L, s);
	return 1;
}

int write_stream_write_vector(lua_State* L)
{
	size_t s = 0;
	uint8_t* ptr;
	write_stream *ws =NULL;
	ws = *(write_stream**) lua_topointer(L, 1);
	//std::cout<<ws<<std::endl;
	//struct bson b;
	//bson_create(&b);
	//luaL_checks(L, 2, LUA_TSTRING);

	if(lua_type(L, 2) == LUA_TTABLE)
	{
		lua_pushnil(L);
		while(lua_next(L, 2))
		{
			//-1 value
			// -2 key
			if(lua_type(L,-1) == LUA_TSTRING)
			{
				ptr =(uint8_t*) lua_tolstring(L,-1, &s);
				std::cout<<ptr<<std::endl;
				ws->write_binary(ptr,s);
			}
			lua_pop(L, 1);
		}
	}
	
	//lua_pushvalue(L,2);
	//pack_dict(L, &b, false);
	
	//bson_destroy(&b);
	//lua_pop(L,1);
	lua_pushnumber(L, s);
	return 1;
}
int write_stream_get_info(lua_State *L)
{
	write_stream *ws = NULL;
	ws = *(write_stream**) lua_topointer(L, 1);
	uint16_t zn= ws->get_izone_num();
	uint32_t start = ws->get_start_timestamp();
	uint32_t end = ws->get_end_timestamp();
	uint32_t size = ws->get_log_size();
	uint32_t item_num = ws->get_item_num();
	uint32_t index_num = ws->get_index_num();
	lua_newtable(L);

	


	lua_pushstring(L,"timestamp");
	lua_newtable(L);

	lua_pushstring(L, "start");
	lua_pushinteger(L, start);
	lua_settable(L, -3);

	lua_pushstring(L, "end");
	lua_pushinteger(L, end);
	lua_settable(L, -3);

	lua_settable(L, -3);

	lua_pushstring(L, "size");
	lua_pushinteger(L, size);
	lua_settable(L, -3);

	lua_pushstring(L, "count");
	lua_newtable(L);

	lua_pushstring(L,"index_zone");
	lua_pushinteger(L, zn);
	lua_settable(L, -3);
	lua_pushstring(L, "item");
	lua_pushinteger(L, item_num);
	lua_settable(L, -3);

	lua_pushstring(L, "index");
	lua_pushinteger(L, index_num);
	lua_settable(L, -3);

	lua_settable(L, -3);
	return 1;


}




int set_index_zone_size(lua_State *L)
{
	uint16_t s = 0;
	write_stream *ws =NULL;
	ws = *(write_stream**) lua_topointer(L, 1);

	s = lua_tointeger(L, 2);
	ws->set_index_zone_size(s);
	return 0;
}


