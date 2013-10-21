extern "C" {

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

}

#include <string>
#include <iostream>
#include "codec.hpp"
#include "codec_cn.hpp"
#include "codec_utf8.hpp"
#include <cstdio>
#include <string.h>

const unsigned int SIZE=1024;

class Buffer
{
public:
    Buffer(size_t len)
    {
        _ptr=new char [len];
    }
    ~Buffer()
    {
        delete [] _ptr;
    }
    char *get()
    {
        return _ptr;
    }

private:
    char *_ptr;
};


class Codec4Lua
{
public:
    static int Encode(lua_State *l);
    static int Decode(lua_State *l);
};

int Codec4Lua::Encode(lua_State *l)
{
    Codec *codec=NULL;

    const char *code=luaL_checkstring(l,1);

    if (strcmp(code,"utf8")==0){
        codec=Utf8Codec::GetCodec();
    }
    else if ((strcmp(code,"gb2312")==0)){
        codec = Gb2312Codec::GetCodec();
    }
    else{
        return luaL_error(l,"not support!");
    }
    
    size_t len=0;
    const char *str=luaL_checklstring(l,2,&len);
    // unicode:4
    size_t u_len=len/4;
    //正常utf8应该4字节就够了
    int out_len=u_len*4;
    if (out_len>SIZE) {
        Buffer buffer(out_len);
        int real_len=codec->encode((unicode_str)str,u_len,buffer.get(),out_len);

        lua_pushlstring(l,buffer.get(),real_len);
    }
    else{
        char buf[SIZE]={0};
        int real_len=codec->encode((unicode_str)str,u_len,buf,SIZE);

        lua_pushlstring(l,buf,real_len);
    }

    return 1;

}


int Codec4Lua::Decode(lua_State *l){

    Codec *codec=NULL;

    const char *code=luaL_checkstring(l,1);
    size_t len=0;
    const char *str=luaL_checklstring(l,2,&len);
    size_t u_len=len*4; 

    if ((strcmp(code,"utf8")==0)){
        codec=Utf8Codec::GetCodec();
    }
    else if ((strcmp(code,"gb2312")==0)){
        codec = Gb2312Codec::GetCodec();
    }
    else{
        return luaL_error(l,"not support!");
    }

    if (u_len>SIZE){
        //unicode 4字节
        Buffer buffer(u_len);
        int real_len=codec->decode(str,len,(unicode_str)buffer.get(),u_len/4);
        lua_pushlstring(l,buffer.get(),real_len*4);
    }
    else{
        char buf[SIZE]={0};
        int real_len=codec->decode(str,len,(unicode_str)buf,SIZE/4);
        lua_pushlstring(l,buf,real_len*4);

    }
    return 1;
}


extern "C"
int luaopen_codec(lua_State *L)
{
    struct luaL_Reg f[]={
        {"encode",Codec4Lua::Encode},
        {"decode",Codec4Lua::Decode},
        {NULL,NULL}
    };

    luaL_register(L,"Codec",f);

    return 1;
}

