#include "LuaCPP.hpp"
#include "Runtime/Logger.hpp"

using namespace Galaxy::GalaxyRT;

struct ILogger
{
public:
    ILogger(const char *name,PSTR switcher,int loglimit,int buflen)
    {
          CLoggerBaseFomatter *formatter = new CLoggerBaseFomatter();
          writer = new CBufferLoggerWriter(std::string(name), switcher,loglimit, buflen);

          logger= new CLogger(formatter, writer);     //formatter,writer will be deleted when delete *pp
    }

    ~ILogger()
    {
        delete logger;   //formatter,writer will be deleted when delete logger
    }

    CBufferLoggerWriter *writer;
    CLogger *logger;
};

class ILogger4Lua
{
public:
    static int init(lua_State *L)
    {
        const char *file=lua_tostring(L,1);
        if (file==NULL)
        {
            return luaL_error(L,"argument 1: string expected!");
        }
        //FIXME::switcher argument should be passed from lua
        PSTR switcher=NULL;

        int loglimit=luaL_optint(L,2,1024*1024*1024);
        int buflen=luaL_optint(L,3,256);

        ILogger **pp=(ILogger **)lua_newuserdata(L,sizeof(ILogger *));
        
        CALL_CPP_FUNCTION(L,
            *pp= new ILogger(file,switcher,loglimit, buflen);
        );

        luaL_getmetatable(L,METATABLE);
        lua_setmetatable(L,-2);

        return 1;
    }

    static int finalizer(lua_State *L)
    {
        ILogger **pp=(ILogger**)luaL_checkudata(L,1,METATABLE);

        delete *pp;
        
        *pp=NULL;

        return 0;
    }
    
    static int LoggerDump(lua_State *L)
    {
        ILogger *p=CheckILogger(L,1);
        const char *pstr=luaL_checkstring(L,2);
        if (pstr==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }
        int nbytes=luaL_checkint(L,3); 
        CALL_CPP_FUNCTION(L,p->logger->LoggerDump(pstr,nbytes,ERROR));    

        return 0;
    }
    
    static int Logger(lua_State *L)
    {
        ILogger *p=CheckILogger(L,1);
        int level=luaL_checkint(L,2); 

        const char *pstr=luaL_checkstring(L,3);
        if (pstr==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }
        CALL_CPP_FUNCTION(L,p->logger->Logger((LOGLEVEL)level,pstr));    

        return 0;
    }
    static int LoggerDebug(lua_State *L)
    {
        ILogger *p=CheckILogger(L,1);
        //int level=luaL_checkint(L,2); 

        const char *pstr=luaL_checkstring(L,2);
        if (pstr==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }
        CALL_CPP_FUNCTION(L,p->logger->Logger((LOGLEVEL)DEBUG,pstr));    

        return 0;
    }
    static int LoggerWarn(lua_State *L)
    {
        ILogger *p=CheckILogger(L,1);
        //int level=luaL_checkint(L,2); 

        const char *pstr=luaL_checkstring(L,2);
        if (pstr==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }
        CALL_CPP_FUNCTION(L,p->logger->Logger((LOGLEVEL)WARN,pstr));    

        return 0;
    }
    static int LoggerFatal(lua_State *L)
    {
        ILogger *p=CheckILogger(L,1);
        //int level=luaL_checkint(L,2); 

        const char *pstr=luaL_checkstring(L,2);
        if (pstr==NULL)
        {
            return luaL_error(L,"string expected! %s  %d",__func__,__LINE__);
        }
        CALL_CPP_FUNCTION(L,p->logger->Logger((LOGLEVEL)FATAL,pstr));    

        return 0;
    }
    
    static int Flush(lua_State *L)
    {
        ILogger *p=CheckILogger(L,1);
        CALL_CPP_FUNCTION(L,p->writer->Flush());

        return 0;
    }

    static int LoggerRename(lua_State *L)//const std::string &logname);
    {
        ILogger *p=CheckILogger(L,1);
        const char *name=luaL_checkstring(L,2);
        if (name==NULL)
        {
            return luaL_error(L,"string expected!  %s  %d",__func__,__LINE__);
        }
        CALL_CPP_FUNCTION(L,std::string logname(name); p->logger->LoggerRename(logname));
        return 0;
    }
    //void LoggerResetSwitcherToPlatform(PSTR switcher);
    //void LoggerResetSwitcherToTrade(PSTR switcher);
public:
    static const char *METATABLE;

private:
    static ILogger *CheckILogger(lua_State *L,int n)
    {
        ILogger **pp=(ILogger**)luaL_checkudata(L,n,METATABLE);

        return *pp;
    }
};
const char *ILogger4Lua::METATABLE="ILogger";

extern "C" int luaopen_logger(lua_State *L)
{
    struct luaL_Reg f[]={
        {"new",ILogger4Lua::init},
        {NULL,NULL}
    }; struct luaL_Reg m[] ={
       {"finalizer",ILogger4Lua::finalizer},
       {"__gc",ILogger4Lua::finalizer},
       { "logDump",ILogger4Lua::LoggerDump},
       { "log",ILogger4Lua::Logger},
       { "debug",ILogger4Lua::LoggerDebug},
       { "warn",ILogger4Lua::LoggerWarn},
       { "fatal",ILogger4Lua::LoggerFatal},

       { "flush",ILogger4Lua::Flush},
       { "logger_rename",ILogger4Lua::LoggerRename},
//       { "LoggerResetSwitcherToPlatform",ILogger4Lua::LoggerResetSwitcherToPlatform},
 //      { "LoggerResetSwitcherToTrade",ILogger4Lua::LoggerResetSwitcherToTrade},
       { NULL,NULL}
    };
    
    luaL_newmetatable(L,ILogger4Lua::METATABLE);

    luaL_register(L,NULL,m);

    lua_pushvalue(L,-1);
    lua_setfield(L,-2,"__index");

    setfield_string(L,ILogger4Lua::METATABLE,"__my_name");

    luaL_register(L,"Logger",f);
    setfield_int(L,DEBUG,"DEBUG");
    setfield_int(L,INFO,"INFO");
    setfield_int(L,WARN,"WARN");
    setfield_int(L,ERROR,"ERROR");
    setfield_int(L,FATAL,"FATAL");
    
    return 1;
}
