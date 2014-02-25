module(...,package.seeall)
local TAP=require "tap"

local TAP=require "harness"
local string=require "string"

local Asserts={}

function Asserts.ok1(exp)
    return (exp)
end

function Asserts.is2(exp,val)
    if exp==val then
        return true
    end
    return false
end

function Asserts.fail()
    return false
end

function Asserts.pass()
    return true
end

function Asserts.isnt2(exp,val)
    return not Asserts.is2(exp,val)
end

function Asserts.isnt_2(var,type_name)
    if not Asserts.is_2(var,type_name) then
        return true
    else
        return false,string.format('expected type is not "%s".',type_name)
    end
end

function Asserts.is_2(var,type_name)
    local var_type=type(var)
    local expect_type=string.lower(type_name)
    if var_type==expect_type then
        return true
    end
    return false,string.format('expected type is "%s", but get "%s".',expect_type,var_type)
end

function Asserts.is_deeply2(actual,expected)
    local actual_t=type(actual)
    local expected_t=type(expected)

    if actual_t~=expected_t then
        return false
    end

    if actual_t=="table" then
        for k1,v1 in pairs(actual) do
            local v=expected[k1]
            if v==nil or not Asserts.is_deeply2(v1,v) then
                return false
            end
        end
        for k2 in pairs(expected) do
            local v=actual[k2]
            if v==nil then
                return false
            else
                --It's compared when travelling `actual` and must be EQUAL if `v` is not nil.
            end
        end
        return true
    else
        return Asserts.is2(actual,expected)
    end
end

local Types={"nil","string","userdata","number","function","table","boolean","thread"}

for i,n in pairs(Types) do
    Asserts["is_"..n.."1"] = function (var) 
        return Asserts.is_2(var,n)
    end
end

for i,n in pairs(Types) do
    Asserts["isnt_"..n.."1"] = function (var)
        return Asserts.isnt_2(var,n)
    end
end

local function get_func_and_argc(key)
    local argc2name={[0]="","1","2","3"}
    local f=nil
    local argc=1
    for i=0,3 do
        local suffix=argc2name[i] or ""
        f=Asserts[key..suffix]
        if f~=nil then
            argc=i
            break
        end
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
            local name=args[argc+1] or ""
            local ok=false

            local ret,msg=f(...)
            if ret then
                ok=true
            end

            local result={status=ok,name=name}
            result["line"]=self._test_info.currentline + debug.getinfo(2,"l").currentline
            result.msg=msg
            results[#results+1]=result
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
    context["_test_info"]=debug.getinfo(2)
    local info={file=context._test_info.source:sub(2)}
    
    load_and_run(s,context)
    
    local t=TAP.new{info=info,result=results}
    t:output()
end

