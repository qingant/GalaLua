#ifndef  __LUACPP_HPP__
#define __LUACPP_HPP__

#include "Lua.h"

#define CALL_CPP_FUNCTION(L,func)   \
try\
{\
    func;\
}\
catch(std::exception &e)\
{\
    return luaL_error(L,"%s",e.what());\
}\
catch(...)\
{\
    return luaL_error(L,"Unknown exception %s  %d",__func__,__LINE__);\
}

#endif   //  __LUACPP_HPP__
