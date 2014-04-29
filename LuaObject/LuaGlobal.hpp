#include "LuaCPP.hpp"
#include "Utils.hpp"

class Globals
{
public:
    struct UserData
    {
        void *Content;
        std::string Name;
        UserData():Content(NULL), Name(32,0){}
    };
    typedef Galaxy::GalaxyRT::ConcurrentMap<std::string, UserData> VARMAP;
public:
    Globals();
    ~Globals();
    void Put(const std::string &, void**, const std::string &);
    const UserData &Get(const std::string &);
private:
    VARMAP _VarMap;
};

class LuaGlobal
{
public:
    static int RegisterGlobal(lua_State *l);
    static int GetGlobal(lua_State *l);

public:
    static Globals GlobalVars;
};
