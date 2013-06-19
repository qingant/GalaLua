#!/usr/bin/env glr
module(..., package.seeall)

local os = require "os"
local L = require 'linenoise'
local pprint = require "pprint"
GRepl = {
    enum_EXPR = 0;
    enum_STMT = 1;
    enum_CMD = 2;
}
function GRepl:new( ... )
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function GRepl:onExpr( expr )
    local src = string.format("setfenv(1, ...);_  = %s; _%d = _;return _;",  expr, self.iter)
    return pcall(loadstring(src), self._env)
end
function GRepl:onSingleAssignStatement( var, expr )
    local src = string.format("setfenv(1, ...);_ =  %s; _%d = _;%s = _;return _;",  expr, self.iter, var)
    return pcall(loadstring(src), self._env)
end
function GRepl:onCommand( cmd )
    os.execute(cmd)
    return nil
end
function GRepl:parseLine( line )
    local singleAssignPattern = "([^;=%s]*)%s*=%s*([^;=]*);?"
    local commandPattern = "!(.*)"
    local v, e = string.match(line, singleAssignPattern)
    if v and e then
        return self:onSingleAssignStatement(v, e)
    elseif string.match(line, commandPattern)  then
        return self:onCommand(string.match(line, commandPattern)[1])
    else
        return self:onExpr(line)
    end


end
function GRepl:initEnv( ... )
    self._env = {}
    self._env.require = require
    self._env.env = self._env
    self._env._G = _G
    self._env.os = os
    self._env.pprint = pprint
end
function GRepl:main()
    self:initEnv()

    -- L.clearscreen()
    local host, port = glr.sys.host,glr.sys.port
    print(string.format("GLR Repl at (%s::%d)\n\n", host, port))
    local history = string.format('%s/history.txt', os.getenv("HOME"))
    L.historyload(history) -- load existing history
    L.setcompletion(function(c,s)
                    if s == 'h' then
                        L.addcompletion(c,'help')
                        L.addcompletion(c,'halt')
                    elseif s == 'r' then
                        L.addcompletion(c, "require")
                  end
                  end)
   
    self.iter = 1
    local prompt = string.format("In [%d]: ", self.iter)
    local line = L.linenoise(prompt)
    while line do

        if #line > 0 then
            print("----> ",line)
            L.historyadd(line)
            L.historysave(history) -- save every new line
            local setfenv =  setfenv
            local _G = _G
            local self = self
            setfenv(1, self._env)
            local status,rt = self:parseLine(line)
            setfenv(1, _G)
            -- print("------->", status, rt)
            if status then
                print(string.format("Out [%d]: %s\n", self.iter, pprint.format(rt)))
                --pprint.pprint(self._env)
            else
                print(tostring(rt))
                print("\n")
            end
            self.iter = self.iter + 1
            -- setfenv(1, _G)
        else
            print ""
        end
    
        prompt = string.format("In [%d]: ", self.iter)
        line = L.linenoise(prompt)
    end



   -- while true do
   --    -- err,msg = glr.recv()
   --    -- print(msg)
   --    io.stdout:write("\n=> ")
   --    io.stdout:flush()
   --    -- io.stdin:seek("set",0)
   --    local cmd = io.stdin:read("*l")
   --    print("-> " .. cmd)
   --    local status, fun = pcall(function () return loadstring(cmd) end)
   --    if status then
   --       local status, rt = pcall(fun)
   --       io.stdout:write(tostring(rt).."     ::(" .. type(rt) .. ")")
   --    else
   --       io.stdout:write(tostring(fun))
   --    end
   --    io.flush()
   -- end

end
if ... == "__main__" then

    GRepl:new():main()
end
