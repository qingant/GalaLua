#include <iostream>
#include "lua_read_stream.h"


 int read_stream_finalize(lua_State *L);
 int read_indexes_by_zone(lua_State *L);
 int read_indexes(lua_State *L);
 int read_stream_get_info(lua_State *L);
 int read_stream_seek(lua_State*L);
 int read_stream_read_next(lua_State*L);

int read_stream_seek(lua_State *L)
{
	read_stream * rs = NULL;
	uint32_t offset;
	rs = *(read_stream**)lua_topointer(L, 1);
	offset = lua_tointeger(L, 2);
	rs->read_stream_seek(offset);
	return 0;
}

int read_stream_read_next(lua_State* L)
{
	read_stream *rs = NULL;
	void *ptr = NULL;
	uint32_t size = 0;
	//struct  bson_reader br;


	rs = *(read_stream**) lua_topointer(L, 1);
	size = rs->read_next(ptr);

	lua_pushlstring(L,(const char*)ptr,size);
	/*
	if(size > 0)
	{
		br.ptr =(uint8_t *) ptr;
		br.size = size;
		unpack_dict(L,&br,false);

		free(ptr);
		return 1;
	}
	else
	{
		if(ptr != NULL)
		{
			free(ptr);
		}
		
		return 0;
	}
	*/
	if(size > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	

}
int read_stream_initialize(lua_State*L)
{
	const char* str = NULL;
	size_t size = 0;	

	str = lua_tolstring(L, 1, &size);
	if(str == NULL)
	{
		//lua_pushstring(L, "log name couldn't be null.");
		luaL_error(L,"log name couldn't be null.");
	}
	
	*(read_stream**) lua_newuserdata(L, sizeof(read_stream*)) = new read_stream(str);
	
	lua_newtable(L);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, read_stream_finalize);
	lua_settable(L, -3);

	lua_pushstring(L, "read_indexes");
	lua_pushcfunction(L, read_indexes);
	lua_settable(L, -3);

	lua_pushstring(L, "get_info");
	lua_pushcfunction(L, read_stream_get_info);
	lua_settable(L, -3);

	lua_pushstring(L, "read_indexes_by_zone");
	lua_pushcfunction(L, read_indexes_by_zone);
	lua_settable(L, -3);
	
	lua_pushstring(L, "seek");
	lua_pushcfunction(L, read_stream_seek);
	lua_settable(L, -3);

	lua_pushstring(L, "read_next");
	lua_pushcfunction(L, read_stream_read_next);
	lua_settable(L, -3);

	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);	
	lua_settable(L, -3);

	lua_setmetatable(L, -2);

	return 1;
}

int read_stream_finalize(lua_State *L)
{
	//std::cout<<"I come here."<<std::endl;
	delete *(read_stream **) lua_topointer(L,1);
	return 0;
}

int read_indexes_by_zone(lua_State* L)
{
	uint32_t index_count = 0, item_count=0;
	uint32_t start, end;
	uint16_t zone_id = 0;

	struct Index* indexes_list =NULL;
	read_stream *rs =NULL;
	rs = *(read_stream**) lua_topointer(L, 1);

	zone_id = lua_tointeger(L, 2);
	item_count = rs->get_item_num();
	
	rs->get_log_timestamp(start, end);

	index_count = rs->read_indexes_by_zone(indexes_list, zone_id, item_count, start, end);

	lua_newtable(L);

	lua_pushstring(L, "timestamp");
	lua_newtable(L);

	lua_pushnumber(L, start);
	lua_rawseti(L, -2, 1);

	lua_pushnumber(L, end);
	lua_rawseti(L, -2, 2);

	lua_settable(L, -3);

	lua_pushstring(L, "count");
	lua_newtable(L);


	lua_pushstring(L, "index");
	lua_pushnumber(L, index_count);
	lua_settable(L, -3);

	lua_pushstring(L, "item");
	lua_pushnumber(L, item_count);
	lua_settable(L, -3);

	lua_settable(L, -3);

	lua_pushstring(L, "indexes");
	lua_newtable(L);

	for(int i=0; i<(int)index_count; i++)
	{
		lua_newtable(L);

		lua_pushnumber(L,indexes_list[i].i_timestamp);
		lua_rawseti(L, -2, 1);

		lua_pushnumber(L,indexes_list[i].i_location);
		lua_rawseti(L, -2, 2);

		lua_rawseti(L, -2, (i+1));
	}
	lua_settable(L, -3);
	return 1;
}

int read_indexes(lua_State *L)
{
	//std::cout<<"read_all_indexes"<<std::endl;
	uint32_t index_count = 0, item_count=0, size = 0;
	uint32_t start, end;
	uint16_t zone_count = 0;

	struct Index* indexes_list =NULL;
	read_stream *rs =NULL;
	rs = *(read_stream**) lua_topointer(L, 1);

	item_count = rs->get_item_num();
	zone_count = rs->get_index_zone_num();
	size = rs->get_log_size();
	rs->get_log_timestamp(start, end);

	index_count = rs->read_indexes_by_all(indexes_list);

	lua_newtable(L);

	lua_pushstring(L, "timestamp");
	lua_newtable(L);

	lua_pushnumber(L, start);
	lua_rawseti(L, -2, 1);

	lua_pushnumber(L, end);
	lua_rawseti(L, -2, 2);

	lua_settable(L, -3);

	lua_pushstring(L, "count");
	lua_newtable(L);

	lua_pushstring(L, "index_zone");
	lua_pushnumber(L, zone_count);
	lua_settable(L, -3);

	lua_pushstring(L, "index");
	lua_pushnumber(L, index_count);
	lua_settable(L, -3);

	lua_pushstring(L, "item");
	lua_pushnumber(L, item_count);
	lua_settable(L, -3);

	lua_settable(L, -3);

	lua_pushstring(L, "size");
	lua_pushnumber(L, size);
	lua_settable(L, -3);

	lua_pushstring(L, "indexes");
	lua_newtable(L);

	for(int i=0; i<(int)index_count; i++)
	{
		lua_newtable(L);

		lua_pushnumber(L,indexes_list[i].i_timestamp);
		lua_rawseti(L, -2, 1);

		lua_pushnumber(L,indexes_list[i].i_location);
		lua_rawseti(L, -2, 2);

		lua_rawseti(L, -2, (i+1));
	}
	lua_settable(L, -3);
	return 1;
}

int read_stream_get_info(lua_State *L)
{
	uint32_t index_count = 0, item_count=0, size = 0;
	uint32_t start, end;
	uint16_t zone_count = 0;

	
	read_stream *rs =NULL;
	rs = *(read_stream**) lua_topointer(L, 1);

	item_count = rs->get_item_num();
	zone_count = rs->get_index_zone_num();
	size = rs->get_log_size();
	rs->get_log_timestamp(start, end);

	lua_newtable(L);

	lua_pushstring(L, "timestamp");
	lua_newtable(L);

	lua_pushnumber(L, start);
	lua_rawseti(L, -2, 1);

	lua_pushnumber(L, end);
	lua_rawseti(L, -2, 2);

	lua_settable(L, -3);

	lua_pushstring(L, "count");
	lua_newtable(L);

	lua_pushstring(L, "index_zone");
	lua_pushnumber(L, zone_count);
	lua_settable(L, -3);

	lua_pushstring(L, "index");
	lua_pushnumber(L, index_count);
	lua_settable(L, -3);

	lua_pushstring(L, "item");
	lua_pushnumber(L, item_count);
	lua_settable(L, -3);

	lua_settable(L, -3);

	lua_pushstring(L, "size");
	lua_pushnumber(L, size);
	lua_settable(L, -3);
	return 1;
}


