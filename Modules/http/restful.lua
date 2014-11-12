--[[
RESTful json client.
]]
module(...,package.seeall)
local client=require(_PACKAGE.."client")
local pprint=require "pprint".pprint
require "str_utils"
local cjson=require "cjson"

local RESTjson={}
function RESTjson:new(...)
    local o={}
    setmetatable(o,self)
    self.__index=self
    
    self.init(o,...)
    return o
end

function RESTjson:encode(t)
    local _t=type(t)
    if _t=="string" then
        return t
    elseif _t=="table" then
        return cjson.encode(t)
    end
    error("invalid value:"..t or "nil")
end

function RESTjson:decode(t)
    
    if t==nil or t:trim()=="" then
        return {}
    else
        return cjson.decode(t)
    end
end

function RESTjson:init()

    self._headers={
        ["Content-Type"]="application/json",
    }
    self._cli=client.client:new()
end

function RESTjson:request(method,url,kwargs)
    local kwargs=kwargs or {}

    if kwargs.data then
        kwargs.data = self:encode(kwargs.data)
    end
    if kwargs.headers then
        table.update(kwargs.headers,self._headers)
    else
        kwargs.headers=self._headers
    end

    local res=self._cli:request(method,url,kwargs)

    res.content=self:decode(res.content)

    return res
end


for m in pairs(client.support_methods) do
    RESTjson[string.lower(m)]= function (self,...) return self:request(m,...) end
end


function new()
    return RESTjson:new()
end

if ...=="__main__" then

    local cli=new()
    local api_key="231da05edbd3fe6b6304644ff4d434f22784e924"
    local headers={["X-Redmine-API-Key"]=api_key}

    pprint(cli:get("192.168.56.112/projects.json",{headers=headers}))

    local data={
        user= {
            login= "jplang",
            firstname= "Jean-Philippe",
            lastname= "Lang",
            mail= "jp_lang@yahoo.fr",
            password= "secret" 
        }
    }
    local data={
        project={
            name="luaproject",
            identifier="lua-project",
        }
    }
    print("post ........")
    --pprint(cli:post("192.168.56.112/projects.json",{headers=headers,data=data}))
    pprint(cli:delete("192.168.56.112/projects/8.json",{headers=headers}))


    pprint(cli:get("192.168.56.112/projects.json",{headers=headers}))
end
