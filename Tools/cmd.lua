--[[
    command template
    @cmd_error: throw an error at command runtime
    @perror: show the error message @cmd_error throw 
]]
module(...,package.seeall)

local io=require "io"
function writef(fmt,...)
    if type(fmt)=="string" then
        io.write(string.format(fmt,...))
    end
end

function prompt_and_read(prompt,default)
    local default=default or ""
    local default_p=""
    if default~="" then
        default_p=string.format("[%s]",default)
    end
    writef("%s: %s",prompt,default_p)
    local input=io.read()
    if input=="" then
        return default
    end
    return input
end

function yes_or_no(prompt,default)
    local default=default or "n"
    assert(default=="y" or default=="n","must be 'y' or 'n'.")
    local input=""
    while input=="" do
        input=prompt_and_read(string.format("%s. Continue? (y|n)",prompt),default)
    end

    if input~="y" then
        return false
    end
    return true
end

local cmd={}
function cmd:new(name)
    local o={}
    self.__index=self
    setmetatable(o,self)
    cmd.init(o,name)
    return o
end

function cmd:init(name)
    self.self_name=name
    self._cmd={}
    function cmd_not_found(t,key)
        return function () writef("%s: command not found\n",key) end
    end
    function cmd_not_found2(t,key)
        return function () 
                    writef("%s: '%s' is not a %s command. See '%s help'.\n",
                           self.self_name,key,self.self_name,self.self_name)
               end
    end
    local mt={__index=cmd_not_found}
    if type(self.self_name)=="string" then
        mt={__index=cmd_not_found2}
    end
    setmetatable(self._cmd,mt)
end
local SHORT_INFO=2
local LONG_INFO=3
function cmd:get_info(cmd,level)
    local level=level or SHORT_INFO
    if cmd=="help" then
        return "display helpful information about commands."
    end
    local c=rawget(self._cmd,cmd)
    if type(c)=="table" then
        local info=c[level]
        local info_t=type(info)
        if info_t=="function" then
            return info(cmd)
        elseif info_t=="string" then
            return info
        else
            return ""
        end
    end
    return nil
end

function cmd:get_cmd(cmd)
    if cmd=="help" then
        return function (...) return self:help(...) end 
    end
    local c=self._cmd[cmd]
    local f=type(c)
    if f=="table" then
        return c[1]
    elseif f=="function" then
        return c
    end
end
function cmd:help(cmd)
    local cmd=cmd
    if type(cmd)=="table" then
        cmd=cmd[2]
    end
    if cmd~=nil then
        local info=self:get_info(cmd,LONG_INFO)
        if info==nil then
            return writef("help : No such command '%s' \n",cmd)
        end
        writef(" %s: %s\n",cmd,info)
    else
        writef("The most commonly used commands are:\n")
        for i in pairs(self._cmd) do
            --FIXME:better help information
            writef(" %-15s  %s\n",i,self:get_info(i))
        end
    end
end

function cmd:add(name,func,info,more_info)
    self._cmd[name]={func,info,more_info}
end

function cmd:helper(argv)
    table.remove(argv,1)
    local cmd=argv[1] 
    table.remove(argv,1)
    if cmd~=nil  then
        local func=self:get_cmd(cmd)
        if func~=nil then
            return func(unpack(argv))
        end
    end
    return self:help()
end
function init_cmd(name)
    return cmd:new(name)
end


local THROW_TYPE={
    ABORT="@abort@",
    ERROR="@error@",
}

local function cmd_throw(t,str,...)
    assert(type(str)=="string","must be string") 
    local str2=string.format(str,...)
    local s=string.format("%s%s",t,str2)
    error(s)
end

function cmd_error(str,...)
    cmd_throw(THROW_TYPE.ERROR,str,...)
end

function cmd_abort(str,...)
    cmd_throw(THROW_TYPE.ABORT,str or "cancelled by user",...)
end

function perror(str)
    assert(type(str)=="string","must be string") 

    for t,v in pairs(THROW_TYPE) do
        local err=string.match(str,string.format("%s(.*)",v))
        if err~=nil then
            return writef("%s: %s\n",t,err)
        end
    end
    --Just show raw message if throw exception without using cmd_throw.
    return writef("Error: %s\n",str)
end

if ...=="__main__" then

    print(prompt_and_read("input your name","funccc"))
    print(yes_or_no("the process will be killed"))
end
