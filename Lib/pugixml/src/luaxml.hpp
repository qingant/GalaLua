#ifndef  LUAXML_HPP_INCLUDED
#define  LUAXML_HPP_INCLUDED

#pragma once

#include "pugixml.hpp"
#include <lua.hpp>

class LuaXmlNode
{
public:
    static int Lua_finalizer(lua_State *L);
    static int node_name(lua_State *L);
    static int node_value(lua_State *L);
    static int first_attribute(lua_State *L);
    static int last_attribute(lua_State *L);
    static int first_child(lua_State *L);
    static int last_child(lua_State *L);
    static int node_next(lua_State *L);
    static int node_prev(lua_State *L);
    static int parent(lua_State *L);
    static int root(lua_State *L);
    static int child_by_name(lua_State *L);
    static int attribute_by_name(lua_State *L);
    static int next_by_name(lua_State *L);
    static int prev_by_name(lua_State *L);
    static int child_value(lua_State *L);
    static int child_value_by_name(lua_State *L);
    static int node_set_name(lua_State *L);
    static int node_set_value(lua_State *L);
    static int append_attribute(lua_State *L);
    static int prepend_attribute(lua_State *L);
    static int append_child(lua_State *L);
    static int prepend_child(lua_State *L);
    static int insert_child_after(lua_State *L);
    static int insert_child_before(lua_State *L);
    static int remove_attribute(lua_State *L);
    static int remove_child(lua_State *L);
    static int Lua_register(lua_State *L);
public:
    static const char *METATABLE;
};

class LuaXmlAttribute
{
public:
    static int Lua_finalizer(lua_State *L);
    static int attribute_name(lua_State *L);
    static int attribute_set_name(lua_State *L);
    static int attribute_value(lua_State *L);
    static int attribute_set_value(lua_State *L);
    static int attribute_next(lua_State *L);
    static int attribute_prev(lua_State *L);
    static int Lua_register(lua_State *L);
public:
    static const char *METATABLE;
};

class LuaXmlPathNode
{
public:
    static int Lua_finalizer(lua_State *L);
    static int node(lua_State *L);
    static int attribute(lua_State *L);
    static int Lua_register(lua_State *L);
public:
    static const char *METATABLE ;
};

class LuaXmlPathNodeSet
{
public:
    static int Lua_finalizer(lua_State *L);
    static int get(lua_State *L);
    static int size(lua_State *L);
    static int sort(lua_State *L);
    static int empty(lua_State *L);
    static int Lua_register(lua_State *L);
public:
    static const char *METATABLE;
};

class LuaXmlDoc
{
public:
    static int lua_init(lua_State *L);
    static int Lua_finalizer(lua_State *L);
    static int load_file(lua_State *L);
    static int load_buffer(lua_State *L);
    static int save(lua_State *L);
    static int xpath(lua_State *L);
public:
    static const char *METATABLE;
};

#endif
