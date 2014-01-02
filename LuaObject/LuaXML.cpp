#include "XML.hpp"
#include "LuaCPP.hpp"

using namespace Galaxy::GalaxyRT;

class CXMLElement4Lua
{
public:
    /*
     * arg1:key (optional)
     * arg2:value (optional)
     */
    static int init(lua_State *L)
    {
        const char *key=luaL_optstring(L,1,NULL);
        const char *val=luaL_optstring(L,2,NULL);

        CXMLElement **p=(CXMLElement  **)lua_newuserdata(L,sizeof(CXMLElement *));

        if(key!=NULL)
        {
            if(val!=NULL)
            {
                CALL_CPP_FUNCTION(L,*p=new CXMLElement(std::string(key),std::string(val)));
            }
            else
            {
                CALL_CPP_FUNCTION(L,*p=new CXMLElement(std::string(key)));
            }
        }
        else
        {
            CALL_CPP_FUNCTION(L,*p=new CXMLElement());
        }

        luaL_getmetatable(L,_tname);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        CXMLElement **pp=(CXMLElement **)luaL_checkudata(L,1,_tname);

        delete *pp;

        *pp=NULL;

        return 0;
    }

    static int Key(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);

        std::string key;
        CALL_CPP_FUNCTION(L,key=p->Key());

        lua_pushlstring(L,&key[0],key.size());

        return 1;
    }

    static int Value(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);

        std::string val;
        CALL_CPP_FUNCTION(L,val=p->Value());

        lua_pushlstring(L,&val[0],val.size());

        return 1;
    }
    /*
     * arg1:lua XMLElement object
     * return:None
     */
    static int AddSubElement(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);

        const CXMLElement *_Elm=CheckCXMLElement(L,2);
        CALL_CPP_FUNCTION(L,p->AddSubElement(_Elm));

        return 0;
    }
    /*
     * arg1:key
     * arg2:value
     * return:None
     */
    static int AddProperty(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);
        const char *k=luaL_checkstring(L,2);
        const char *v=luaL_checkstring(L,3);

        if((k==NULL) || (v==NULL))
        {
            return luaL_error(L,"string expected!  %s  %d",__func__,__LINE__);
        }
        std::string key(k);
        std::string value(v);
        const CXMLValue *_Elm=new CXMLValue(key,value);
        CALL_CPP_FUNCTION(L,p->AddProperty(_Elm));

        return 0;
    }

    /*
     * arg:None
     * return:lua table(empty table if no property)
     * */
    static int Properties(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);

        try
        {
            const CXMLValues &_vals=p->Properties();
            unsigned int count=0;
            count=_vals.Count();
            lua_newtable(L);
            const CXMLValue *_val;
            std::string k,v;
            for (unsigned int i=0;i<count;++i)
            {
                _val=_vals[i];
                k=_val->Key();
                v=_val->Value();
                setfield_string(L,v.c_str(),k.c_str());
            }
            return 1;
        }
        catch(std::exception &e)
        {
            return luaL_error(L,"%s",e.what());
        }
        catch(...)
        {
            return luaL_error(L,"Unknown exception %s  %d",__func__,__LINE__);
        }
    }

    /*
     * arg:None
     * return:lua array(table) containing XMLElement object,empty table if no Subelement
     * */
    static int SubElements(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);

        try
        {
            const  CXMLElements &_elms=p->SubElements();
            unsigned int count=0;
            count=_elms.Count();
            
            lua_newtable(L);
            for (unsigned int i=0;i<count;++i)
            {
                const CXMLElement **sub_element=(const CXMLElement  **)lua_newuserdata(L,sizeof(CXMLElement *));
                *sub_element=_elms[i];

                luaL_getmetatable(L,_tname);
                lua_setmetatable(L,-2);

                lua_rawseti(L,-2,i+1);
            }
            return 1;
        }
        catch(std::exception &e)
        {
            return luaL_error(L,"%s",e.what());
        }
        catch(...)
        {
            return luaL_error(L,"Unknown exception %s  %d",__func__,__LINE__);
        }
    }

    /*
     * return true if it's leaf element.
     */
    static int isLeaf(lua_State *L)
    {
        CXMLElement *p=CheckCXMLElement(L,1);

        try
        {
            const  CXMLElements &_elms=p->SubElements();
            lua_pushboolean(L,!_elms.Count());
            return 1;
        }
        catch(std::exception &e)
        {
            return luaL_error(L,"%s",e.what());
        }
        catch(...)
        {
            return luaL_error(L,"Unknown exception %s  %d",__func__,__LINE__);
        }
    }

private:
    static CXMLElement *CheckCXMLElement(lua_State *L,int n)
    {
        CXMLElement **pp=(CXMLElement **)luaL_checkudata(L,n,_tname);

        return *pp;
    }

public:
    static const char *_tname;
public:
    static void CXMLElement_register(lua_State *L)
    {
        int ret=0;
//        struct luaL_Reg f[]={
//            {"cxml_element",CXMLElement4Lua::init},
//            {NULL,NULL}
//        };
        struct luaL_Reg m[]={
            {"__gc",CXMLElement4Lua::finalizer},
            {"finalizer",CXMLElement4Lua::finalizer},
            {"add_sub_element",CXMLElement4Lua::AddSubElement},
            {"add_property",CXMLElement4Lua::AddProperty},
            {"sub_elements",CXMLElement4Lua::SubElements},
            {"properties",CXMLElement4Lua::Properties},
            {"key",CXMLElement4Lua::Key},
            {"value",CXMLElement4Lua::Value},
            {"isleaf",CXMLElement4Lua::isLeaf},
            {NULL,NULL}
        };

        ret=luaL_newmetatable(L,CXMLElement4Lua::_tname);
        if (ret==0)
        {
            luaL_error(L,"%s  existed!  %s  %d",CXMLElement4Lua::_tname,__func__,__LINE__);
        }
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
};
const char *CXMLElement4Lua::_tname="CXMLElement";

class CXMLReader4Lua
{
public:
    /*
     * arg1:xml string 
     * */
    static int init(lua_State *L)
    {
        const char *path=luaL_checkstring(L,1);
        int len=luaL_checkint(L,2);
        if(path==NULL)
        {
            return luaL_error(L,"string expected!  %s  %d",__func__,__LINE__);
        }

        CXMLReader **p=(CXMLReader  **)lua_newuserdata(L,sizeof(CXMLReader *));

        CALL_CPP_FUNCTION(L,*p=new CXMLReader(path,len));

        luaL_getmetatable(L,_tname);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        CXMLReader **pp=(CXMLReader **)luaL_checkudata(L,1,_tname);

        delete *pp;

        *pp=NULL;

        return 0;
    }

    /*
     * arg:None
     * return:xml root element (lua object)
     * */
    static int Document(lua_State *L)
    {
        CXMLReader *p=CheckCXMLReader(L,1);

        const CXMLElement *root_element;
        const CXMLDocument *doc;
        CALL_CPP_FUNCTION(L,doc=p->Document();root_element=doc->Root());

        const CXMLElement **root=(const CXMLElement  **)lua_newuserdata(L,sizeof(CXMLElement *));
        *root=root_element; 

        luaL_getmetatable(L,CXMLElement4Lua::_tname);

        //XXX:clear __gc, because CXMLDocument will `delete` the CXMLElement it created 
        lua_pushnil(L);
        lua_setfield(L,-2,"__gc");

        lua_setmetatable(L,-2);

        return 1;
    }
private:
    static CXMLReader *CheckCXMLReader(lua_State *L,int n)
    {
        CXMLReader **pp=(CXMLReader **)luaL_checkudata(L,n,_tname);

        return *pp;
    }

public:
    static const char *_tname;

public:
    static void CXMLReader_register(lua_State *L)
    {
        int ret=0;
//        struct luaL_Reg f[]={
//            {"cxml_reader",CXMLReader4Lua::init},
//            {NULL,NULL}
//        };
        struct luaL_Reg m[]={
            {"__gc",CXMLReader4Lua::finalizer},
            {"finalizer",CXMLReader4Lua::finalizer},
            {"document",CXMLReader4Lua::Document},
            {NULL,NULL}
        };

        ret=luaL_newmetatable(L,CXMLReader4Lua::_tname);
        if (ret==0)
        {
            luaL_error(L,"%s  existed!  %s  %d",CXMLReader4Lua::_tname,__func__,__LINE__);
        }
        luaL_register(L,NULL,m);

        lua_pushvalue(L,-1);
        lua_setfield(L,-2,"__index");
    }
};

const char *CXMLReader4Lua::_tname="CXMLReader";



class CXMLWriter4Lua
{
public:
    /*
     * arg1:root element (must be created calling cxml_element)
     * arg2:path of xml file (if nil then return the xml string)
     * arg3:xml encoding (optional,default:UTF-8)
     * arg4:xml version  (optional,default:1.0)
     */
    static int init(lua_State *L)
    {
        CXMLElement **root_element=(CXMLElement **)luaL_checkudata(L,1,CXMLElement4Lua::_tname);

        const char *path=luaL_optstring(L,2,NULL);
        const char *encode=luaL_optstring(L,3,"UTF-8");
        const char *version=luaL_optstring(L,4,"1.0");

//        CXMLWriter **p=(CXMLWriter  **)lua_newuserdata(L,sizeof(CXMLWriter *));

        CXMLDeclaration *_dec=new CXMLDeclaration ;
        
        std::string XMLStr;
        try
        {
            CXMLValue *enc=new CXMLValue(std::string("encoding"),std::string(encode));
            CXMLValue *ver=new CXMLValue(std::string("version"),std::string(version));
            _dec->Append(enc);
            _dec->Append(ver);
            CXMLDocument doc(_dec,*root_element);
            if (path==NULL)
            {
                //*p=new CXMLWriter(&doc,XMLStr);
                CXMLWriter  w(&doc,XMLStr);
                lua_pushlstring(L,&XMLStr[0],XMLStr.size());
                return 1;
            }
            else
            {
                //*p=new CXMLWriter(&doc,path);
                CXMLWriter w(&doc,path);
                return 0;
            }
        }
        catch(std::exception &e)
        {
            return luaL_error(L,"%s",e.what());
        }
        catch(...)
        {
            return luaL_error(L,"Unknown exception %s  %d",__func__,__LINE__);
        }
    }

    static int finalizer(lua_State *L)
    {
        CXMLWriter **pp=(CXMLWriter **)luaL_checkudata(L,1,_tname);

        delete *pp;

        *pp=NULL;

        return 0;
    }

private:
    static CXMLWriter *CheckCXMLWriter(lua_State *L,int n)
    {
        CXMLWriter **pp=(CXMLWriter **)luaL_checkudata(L,n,_tname);

        return *pp;
    }

public:
    static const char *_tname;
public:
    // XXX:CXMLWriter is only a function now
    static void CXMLWriter_register(lua_State *L)
    {
        (void)L;
//        int ret=0;
//        struct luaL_Reg f[]={
//            {"cxml_writer",CXMLWriter4Lua::init},
//            {NULL,NULL}
//        };
//        struct luaL_Reg m[]={
//            {"__gc",CXMLWriter4Lua::finalizer},
//            {"finalizer",CXMLWriter4Lua::finalizer},
//            {NULL,NULL}
//        };

//        ret=luaL_newmetatable(L,CXMLWriter4Lua::_tname);
//        if (ret==0)
//        {
//            luaL_error(L,"%s  existed!  %s  %d",CXMLWriter4Lua::_tname,__func__,__LINE__);
//        }
//        luaL_register(L,NULL,m);
//
//        lua_pushvalue(L,-1);
//        lua_setfield(L,-2,"__index");
    }
};
const char *CXMLWriter4Lua::_tname="CXMLWriter";

extern "C"
int luaopen_xml(lua_State *L)
{
    CXMLElement4Lua::CXMLElement_register(L);
    CXMLReader4Lua::CXMLReader_register(L);
    CXMLWriter4Lua::CXMLWriter_register(L);
//    int ret=0;

    struct luaL_Reg f[]={
        {"cxml_writer",CXMLWriter4Lua::init},
        {"cxml_reader",CXMLReader4Lua::init},
        {"cxml_element",CXMLElement4Lua::init},
        {NULL,NULL}
    };

//    ret=luaL_newmetatable(L,CXMLReader4Lua::_tname);
//    if (ret==0)
//    {
//        return luaL_error(L,"%s  existed!  %s  %d",CXMLReader4Lua::_tname,__func__,__LINE__);
//    }
//    luaL_register(L,NULL,m);

//    lua_pushvalue(L,-1);
//    lua_setfield(L,-2,"__index");


    luaL_register(L,"CXML",f);
//    setfield_string(L,"CXML","__my_name");

    return 1;
}
