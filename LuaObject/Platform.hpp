#include "LuaCPP.hpp"

namespace GLR
{
    class GLRInfo
    {
    public:
        static void RegisterInfo(lua_State *l);
    private:
        static const char *platform;
        static const char *version;
        static const char *compiler;  
    };
}


