module(...,package.seeall)
local TAP=require "tap"
local string=require "string"

local Asserts={}

function Asserts.ok(exp,name)
    return (exp)
end

function Asserts.is3(exp,val,name)
    if exp==val then
        return true
    end
    return false
end

function Asserts.is_3(var,type_name,name)
    if type(var)==string.lower(type_name) then
        return true
    end
    return false
end

local Types={"nil","string","userdata","number","function","table","boolean","thread"}

for i,n in pairs(Types) do
    Asserts["is_"..n] = function (var,name) 
        return Asserts.is_3(var,n,name) 
    end
end

local function get_func_and_argc(key)
    local argc=2
    local f=Asserts[key]
    if f==nil then
        f=Asserts[key.."3"]
        if f==nil then
            return nil,nil
        end
        argc=3
    end

    return f,argc
end

function unittest(s)
    local context={}
    local results={}

    function get_assert_function(self,key)
        local f,argc=get_func_and_argc(key) 
        if f==nil then
            return _G[key]
        end

        return function (...)
            local args={...}
            local name=args[argc] or ""
            local ok=false
            if f(...) then
                ok=true
            end
            results[#results+1]={ok,name}
        end
    end

    setmetatable(context,{__index=get_assert_function})

    function load_and_run(s,context)
        assert(type(s)=="string")
        local f 
        if _VERSION=="Lua 5.2"  then
            f=assert(load(s,"unittest code","t",context))
        else
            f=assert(loadstring(s))
            setfenv(f,context)
        end
        f()
    end

    load_and_run(s,context)

    local t=TAP.new(results)
    t:output()
end
