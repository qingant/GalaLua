#include "luaxml.hpp"

int LuaXmlNode::Lua_finalizer(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    delete *node;
    *node=NULL;
    return 0;
}

int LuaXmlNode::node_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    lua_pushstring(L,(*node)->name());

    return 1;
}

int LuaXmlNode::node_value(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    lua_pushstring(L,(*node)->value());

    return 1;
}

int LuaXmlNode::first_attribute(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_attribute **attr = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *attr = new pugi::xml_attribute((*node)->first_attribute().internal_object());
        luaL_getmetatable(L, LuaXmlAttribute::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::last_attribute(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_attribute **attr = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *attr = new pugi::xml_attribute((*node)->last_attribute().internal_object());
        luaL_getmetatable(L, LuaXmlAttribute::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::first_child(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->first_child().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::last_child(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->last_child().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::node_next(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->next_sibling().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::node_prev(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->previous_sibling().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::parent(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->parent().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::root(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->root().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::child_by_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->child(name).internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::attribute_by_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);
    const pugi::xml_attribute **attr = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *attr = new pugi::xml_attribute((*node)->attribute(name).internal_object());
        luaL_getmetatable(L, LuaXmlAttribute::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::next_by_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->next_sibling(name).internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::prev_by_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *sztemp = new pugi::xml_node((*node)->previous_sibling(name).internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::child_value(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    lua_pushstring(L,(*node)->child_value());

    return 1;
}

int LuaXmlNode::child_value_by_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);
    lua_pushstring(L,(*node)->child_value(name));

    return 1;
}

int LuaXmlNode::node_set_name(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    lua_pushboolean (L,(*node)->set_name(name) ? 1 : 0);

    return 1;
}

int LuaXmlNode::node_set_value(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    lua_pushboolean (L,(*node)->set_value(name) ? 1 : 0);

    return 1;
}

int LuaXmlNode::append_attribute(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    const pugi::xml_attribute **attr = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *attr = new pugi::xml_attribute((*node)->append_attribute(name).internal_object());
        luaL_getmetatable(L, LuaXmlAttribute::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::prepend_attribute(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    const pugi::xml_attribute **attr = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *attr = new pugi::xml_attribute((*node)->prepend_attribute(name).internal_object());
        luaL_getmetatable(L, LuaXmlAttribute::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlNode::append_child(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *type = luaL_checkstring(L,2);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));

    try
    {

        if(!strcasecmp(type,"document"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_document).internal_object());
        }
        else if(!strcasecmp(type,"element"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_element).internal_object());
        }
        else if(!strcasecmp(type,"pcdata"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_pcdata).internal_object());
        }
        else if(!strcasecmp(type,"cdata"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_cdata).internal_object());
        }
        else if(!strcasecmp(type,"comment"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_comment).internal_object());
        }
        else if(!strcasecmp(type,"pi"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_pi).internal_object());
        }
        else if(!strcasecmp(type,"declaration"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_declaration).internal_object());
        }
        else if(!strcasecmp(type,"doctype"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_doctype).internal_object());
        }
        else if(!strcasecmp(type,"null"))
        {
            *sztemp = new pugi::xml_node((*node)->append_child(pugi::node_null).internal_object());
        }

        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }

    return 1;
}

int LuaXmlNode::prepend_child(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *type = luaL_checkstring(L,2);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));

    try
    {

        if(!strcasecmp(type,"document"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_document).internal_object());
        }
        else if(!strcasecmp(type,"element"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_element).internal_object());
        }
        else if(!strcasecmp(type,"pcdata"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_pcdata).internal_object());
        }
        else if(!strcasecmp(type,"cdata"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_cdata).internal_object());
        }
        else if(!strcasecmp(type,"comment"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_comment).internal_object());
        }
        else if(!strcasecmp(type,"pi"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_pi).internal_object());
        }
        else if(!strcasecmp(type,"declaration"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_declaration).internal_object());
        }
        else if(!strcasecmp(type,"doctype"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_doctype).internal_object());
        }
        else if(!strcasecmp(type,"null"))
        {
            *sztemp = new pugi::xml_node((*node)->prepend_child(pugi::node_null).internal_object());
        }

        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }

    return 1;
}

int LuaXmlNode::insert_child_after(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *type = luaL_checkstring(L,2);
    pugi::xml_node **child = (pugi::xml_node **)luaL_checkudata(L,3,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));

    try
    {

        if(!strcasecmp(type,"document"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_document,**child).internal_object());
        }
        else if(!strcasecmp(type,"element"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_element,**child).internal_object());
        }
        else if(!strcasecmp(type,"pcdata"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_pcdata,**child).internal_object());
        }
        else if(!strcasecmp(type,"cdata"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_cdata,**child).internal_object());
        }
        else if(!strcasecmp(type,"comment"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_comment,**child).internal_object());
        }
        else if(!strcasecmp(type,"pi"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_pi,**child).internal_object());
        }
        else if(!strcasecmp(type,"declaration"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_declaration,**child).internal_object());
        }
        else if(!strcasecmp(type,"doctype"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_doctype,**child).internal_object());
        }
        else if(!strcasecmp(type,"null"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_after(pugi::node_null,**child).internal_object());
        }

        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }

    return 1;
}

int LuaXmlNode::insert_child_before(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *type = luaL_checkstring(L,2);
    pugi::xml_node **child = (pugi::xml_node **)luaL_checkudata(L,3,METATABLE);
    const pugi::xml_node **sztemp = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));

    try
    {

        if(!strcasecmp(type,"document"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_document,**child).internal_object());
        }
        else if(!strcasecmp(type,"element"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_element,**child).internal_object());
        }
        else if(!strcasecmp(type,"pcdata"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_pcdata,**child).internal_object());
        }
        else if(!strcasecmp(type,"cdata"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_cdata,**child).internal_object());
        }
        else if(!strcasecmp(type,"comment"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_comment,**child).internal_object());
        }
        else if(!strcasecmp(type,"pi"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_pi,**child).internal_object());
        }
        else if(!strcasecmp(type,"declaration"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_declaration,**child).internal_object());
        }
        else if(!strcasecmp(type,"doctype"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_doctype,**child).internal_object());
        }
        else if(!strcasecmp(type,"null"))
        {
            *sztemp = new pugi::xml_node((*node)->insert_child_before(pugi::node_null,**child).internal_object());
        }

        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }

    return 1;
}

int LuaXmlNode::remove_attribute(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    lua_pushboolean(L,(*node)->remove_attribute(name) ? 1 : 0);

    return 1;
}

int LuaXmlNode::remove_child(lua_State *L)
{
    pugi::xml_node **node=(pugi::xml_node **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    lua_pushboolean(L,(*node)->remove_child(name) ? 1 : 0);

    return 1;
}

int LuaXmlNode::Lua_register(lua_State *L)
{
    struct luaL_Reg m[]={
        {"__gc",Lua_finalizer},
        {"finalizer",Lua_finalizer},
        {"name",node_name},
        {"value",node_value},
        {"first_attribute",first_attribute},
        {"last_attribute",last_attribute},
        {"first_child",first_child},
        {"last_child",last_child},
        {"next",node_next},
        {"prev",node_prev},
        {"parent",parent},
        {"root",root},
        {"child_by_name",child_by_name},
        {"attribute_by_name",attribute_by_name},
        {"next_by_name",next_by_name},
        {"prev_by_name",prev_by_name},
        {"child_value",child_value},
        {"child_value_by_name",child_value_by_name},
        {"set_name",node_set_name},
        {"set_value",node_set_value},
        {"append_attribute",append_attribute},
        {"prepend_attribute",prepend_attribute},
        {"append_child",append_child},
        {"prepend_child",prepend_child},
        {"insert_child_after",insert_child_after},
        {"insert_child_before",insert_child_before},
        {"remove_child",remove_child},
        {"remove_attribute",remove_attribute},
        {NULL,NULL}
    };

    luaL_newmetatable(L,METATABLE);

    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    return 1;
}

const char *LuaXmlNode::METATABLE = "XmlNode" ;

int LuaXmlAttribute::Lua_finalizer(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,-1,METATABLE);
    delete *attr;
    *attr=NULL;
    return 0;
}

int LuaXmlAttribute::attribute_name(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,-1,METATABLE);
    lua_pushstring(L,(*attr)->name());

    return 1;
}

int LuaXmlAttribute::attribute_set_name(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,1,METATABLE);
    const char *name = luaL_checkstring(L,2);

    lua_pushboolean(L,(*attr)->set_name(name) ? 1 : 0 );

    return 1;
}

int LuaXmlAttribute::attribute_value(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,-1,METATABLE);
    lua_pushstring(L,(*attr)->value());

    return 1;
}

int LuaXmlAttribute::attribute_set_value(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,1,METATABLE);

    switch(lua_type(L,2))
    {
        case LUA_TNUMBER:
            lua_pushboolean(L,(*attr)->set_value(luaL_checknumber(L,2)) ? 1 : 0 );
            break;
        case LUA_TBOOLEAN:
            lua_pushboolean(L,(*attr)->set_value(lua_toboolean(L,2)? true : false) ? 1 : 0 );
            break;
        case LUA_TSTRING:
            lua_pushboolean(L,(*attr)->set_value(luaL_checkstring(L,2)) ? 1 : 0 );
            break;
        default:
            return luaL_error(L,"invalud value");
    }

    return 1;
}

int LuaXmlAttribute::attribute_next(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,1,METATABLE);
    const pugi::xml_attribute **sztemp = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *sztemp = new pugi::xml_attribute((*attr)->next_attribute().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }

    return 1;
}

int LuaXmlAttribute::attribute_prev(lua_State *L)
{
    pugi::xml_attribute **attr=(pugi::xml_attribute **)luaL_checkudata(L,1,METATABLE);
    const pugi::xml_attribute **sztemp = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *sztemp = new pugi::xml_attribute((*attr)->previous_attribute().internal_object());
        luaL_getmetatable(L, METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }

    return 1;
}

int LuaXmlAttribute::Lua_register(lua_State *L)
{
    struct luaL_Reg m[]={
        {"__gc",Lua_finalizer},
        {"finalizer",Lua_finalizer},
        {"name",attribute_name},
        {"set_name",attribute_set_name},
        {"value",attribute_value},
        {"set_value",attribute_set_value},
        {"next",attribute_next},
        {"prev",attribute_prev},
        {NULL,NULL}
    };

    luaL_newmetatable(L,METATABLE);

    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    return 1;
}

const char *LuaXmlAttribute::METATABLE = "XmlAttribute" ;


int LuaXmlPathNode::Lua_finalizer(lua_State *L)
{
    pugi::xpath_node **nodexpath=(pugi::xpath_node **)luaL_checkudata(L,-1,METATABLE);
    delete *nodexpath;
    *nodexpath=NULL;
    return 0;
}

int LuaXmlPathNode::node(lua_State *L)
{
    pugi::xpath_node **nodexpath=(pugi::xpath_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_node **node = (const pugi::xml_node **)lua_newuserdata(L,sizeof(pugi::xml_node *));
    try
    {
        *node = new pugi::xml_node((*nodexpath)->node().internal_object());
        luaL_getmetatable(L, LuaXmlNode::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlPathNode::attribute(lua_State *L)
{
    pugi::xpath_node **nodexpath=(pugi::xpath_node **)luaL_checkudata(L,-1,METATABLE);
    const pugi::xml_attribute **attr = (const pugi::xml_attribute **)lua_newuserdata(L,sizeof(pugi::xml_attribute *));
    try
    {
        *attr = new pugi::xml_attribute((*nodexpath)->attribute().internal_object());
        luaL_getmetatable(L, LuaXmlAttribute::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlPathNode::Lua_register(lua_State *L)
{
    struct luaL_Reg m[]={
        {"__gc",Lua_finalizer},
        {"finalizer",Lua_finalizer},
        {"node",node},
        {"attribute",attribute},
        {NULL,NULL}
    };

    luaL_newmetatable(L,METATABLE);

    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    return 1;
}


const char *LuaXmlPathNode::METATABLE = "XpathNode" ;


int LuaXmlPathNodeSet::Lua_finalizer(lua_State *L)
{
    pugi::xpath_node_set **nodeset=(pugi::xpath_node_set **)luaL_checkudata(L,-1,METATABLE);
    delete *nodeset;
    *nodeset=NULL;
    return 0;
}

int LuaXmlPathNodeSet::get(lua_State *L)
{
    pugi::xpath_node_set *node_set = *(pugi::xpath_node_set **)luaL_checkudata(L,1,METATABLE);
    int index = luaL_checkint(L,2);
    if(index < 0 || (size_t)index >= node_set->size())
    {
        return luaL_error(L,"invalid index");
    }
    try
    {
        const pugi::xpath_node **xpathnode = (const pugi::xpath_node **)lua_newuserdata(L,sizeof(pugi::xpath_node *));
        *xpathnode = (*node_set)[index].attribute() ? new pugi::xpath_node((*node_set)[index].attribute(),(*node_set)[index].node()) : new pugi::xpath_node((*node_set)[index].node());

        luaL_getmetatable(L, LuaXmlPathNode::METATABLE);
        lua_setmetatable(L,-2);
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
    return 1;
}

int LuaXmlPathNodeSet::size(lua_State *L)
{
    pugi::xpath_node_set *node_set = *(pugi::xpath_node_set **)luaL_checkudata(L,1,METATABLE);
    lua_pushnumber(L,node_set->size());
    return 1;
}

int LuaXmlPathNodeSet::sort(lua_State *L)
{
    pugi::xpath_node_set *node_set = *(pugi::xpath_node_set **)luaL_checkudata(L,1,METATABLE);
    node_set->sort();
    return 0;
}

int LuaXmlPathNodeSet::empty(lua_State *L)
{
    pugi::xpath_node_set *node_set = *(pugi::xpath_node_set **)luaL_checkudata(L,1,METATABLE);
    lua_pushboolean(L,node_set->empty() ? 1 : 0);
    return 1;
}

int LuaXmlPathNodeSet::Lua_register(lua_State *L)
{
    struct luaL_Reg m[]={
        {"__gc",Lua_finalizer},
        {"finalizer",Lua_finalizer},
        {"xpath_node_by_index",get},
        {"size",size},
        {"sort",sort},
        {"empty",empty},
        {NULL,NULL}
    };

    luaL_newmetatable(L,METATABLE);

    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    return 1;
}


const char *LuaXmlPathNodeSet::METATABLE = "XpathNodeSet" ;


int LuaXmlDoc::lua_init(lua_State *L)
{
    const pugi::xml_document **doc = (const pugi::xml_document **)lua_newuserdata(L,sizeof(pugi::xml_document *));
    *doc = new pugi::xml_document();
    luaL_getmetatable(L, METATABLE);
    lua_setmetatable(L,-2);
    return 1;
}

int LuaXmlDoc::Lua_finalizer(lua_State *L)
{
    pugi::xml_document **doc=(pugi::xml_document **)luaL_checkudata(L,-1,METATABLE);
    delete *doc;
    *doc=NULL;
    return 0;
}

int LuaXmlDoc::load_file(lua_State *L)
{
    pugi::xml_document *doc = *(pugi::xml_document **)luaL_checkudata(L,1,METATABLE);
    const char *path=luaL_checkstring(L,2);
    try
    {
        pugi::xml_parse_result res = doc->load_file(path);
        if(res.status != pugi::status_ok)
        {
            return luaL_error(L,res.description());
        }
        return 0;
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
}

int LuaXmlDoc::load_buffer(lua_State *L)
{
    pugi::xml_document *doc = *(pugi::xml_document **)luaL_checkudata(L,1,METATABLE);
    const char *path=luaL_checkstring(L,2);
    try
    {
        pugi::xml_parse_result res = doc->load_file(path);
        if(res.status != pugi::status_ok)
        {
            return luaL_error(L,res.description());
        }
        return 0;
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
}

int LuaXmlDoc::save(lua_State *L)
{
    pugi::xml_document *doc = *(pugi::xml_document **)luaL_checkudata(L,1,METATABLE);
    const char *path=luaL_checkstring(L,2);
    try
    {
        if(!doc->save_file(path))
        {
            return luaL_error(L,"save file failure");
        }
        return 0;
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
}

int LuaXmlDoc::xpath(lua_State *L)
{
    pugi::xml_document *doc = *(pugi::xml_document **)luaL_checkudata(L,1,METATABLE);
    const char *query=luaL_checkstring(L,2);
    try
    {
        pugi::xpath_node_set nodes = doc->select_nodes(query);
        const pugi::xpath_node_set **node_set = (const pugi::xpath_node_set **)lua_newuserdata(L,sizeof(const pugi::xpath_node_set *));
        *node_set = new pugi::xpath_node_set(nodes);

        luaL_getmetatable(L, LuaXmlPathNodeSet::METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }
    catch(std::exception &e)
    {
        return luaL_error(L, e.what());     //luaL_error use long jump so it will never return , using return here just idiom
    }
    catch(...)
    {
        return luaL_error(L,"unknown Exception %s  %d",__func__,__LINE__);

    }
}



const char *LuaXmlDoc::METATABLE = "Document" ;

extern "C" int luaopen_minixml(lua_State *L)
{
    LuaXmlPathNodeSet::Lua_register(L);
    LuaXmlPathNode::Lua_register(L);
    LuaXmlNode::Lua_register(L);
    LuaXmlAttribute::Lua_register(L);

    struct luaL_Reg f[]={
            {"new",LuaXmlDoc::lua_init},
            {NULL,NULL}
        };
        struct luaL_Reg m[]={
            {"__gc",LuaXmlDoc::Lua_finalizer},
            {"finalizer",LuaXmlDoc::Lua_finalizer},
            {"load_file",LuaXmlDoc::load_file},
            {"load_buffer",LuaXmlDoc::load_buffer},
            {"save",LuaXmlDoc::save},
            {"xpath",LuaXmlDoc::xpath},
            {NULL,NULL}
        };

        luaL_newmetatable(L,LuaXmlDoc::METATABLE);
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");

        //setfield_string(L,LuaEnvironment::METATABLE,"__my_name");

        luaL_register(L,"minixml",f);

        return 1;
}
