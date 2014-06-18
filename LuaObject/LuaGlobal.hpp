#include "LuaCPP.hpp"
#include "Utils.hpp"
#include "Sync.hpp"

class Globals
{
public:
    struct UserData
    {
        void *Content;
        //TODO:当Count溢出时对象将不会正确析构
        mutable unsigned int Count;
        std::string Name;
        UserData():Content(NULL),Count(1), Name(32,0){}
    };
    typedef Galaxy::GalaxyRT::ConcurrentMap<std::string, UserData> VARMAP;
public:
    Globals();
    ~Globals();
    void Put(const std::string &, void**, const std::string &);
    const UserData &Get(const std::string &);
    void Erase(const std::string &);
private:
    VARMAP _VarMap;
};

class LuaGlobal
{
public:
    static int RegisterGlobal(lua_State *l);
    static int GetGlobal(lua_State *l);

    static int Finalize(lua_State *l);
    static int __Index(lua_State *l);
    inline static void ModifyState(lua_State *l,const char *id);
public:
    static Globals GlobalVars;
    static const char *const FINALIZER;
    static Galaxy::GalaxyRT::CPthreadMutex GlobalMutex;
};
