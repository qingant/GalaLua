--[[
-- console
--
--]]

module(...,package.seeall)

local os=require "os"
local string=require "string"
local pprint=require "pprint"
local L = require 'linenoise'

function parse_opt(line)
    local opt={}
    for i in line:gmatch("%s*([^%s]+)") do
        opt[#opt+1]=i
    end
    return opt
end


function ConsoleBase(o)

    local console_base={}

    --completions:{command_name=completion_func}
    --cmds:{command_name=command_func}
    local cmds={}
    function cmds.help(cmd)
        io.write("available commands: \n")
        for i in pairs(cmds) do
            io.write("\t",i,"\n")
            --FIXME:better help information
            --io.write("\t",i,":",i.info(),"\n")
        end
    end

    local mt={__index=function (t,key) io.write(key,": Command not found!\n") return cmds.help end}
    setmetatable(cmds,mt)
    local completions={help=""}

    local history,prompt="history.log",">"

    function console_base.set_history(file)
        history=file or history
    end

    function console_base.set_prompt(s)
        prompt=s or prompt
    end

    function console_base:new(o)
        local o=o or {}
        setmetatable(o,self)
        self.__index=self
        return o
    end


    --[[
    -- register command and completion  
    --
    -- @name: register @helper as @name in console
    -- @helper: command function
    -- @completion: completion function
    --]]
    function console_base.register(name,helper,completion)
        cmds[name]=helper
        completions[name]=completion
    end

    --command completion
    local function cmd_completion(compl,s,prefix)
        local ret={}
        for i in pairs(compl) do
            if string.match(i,"^"..s) then
                ret[#ret+1]=prefix..i
            end
        end
        return ret
    end

    --[[
    --@compl: all possible completions
    --@s:the word to complete
    --@prefix: return the completions string with prefix @prefix
    --]]
    local function get_completion(compl,s,prefix)
        local prefix=prefix or ""
        assert(s,"can't be nil")
        local all={}
        local prefix_space,cmd,arg=s:match("^(%s*)([^%s]*)(.*)$")
        if arg=="" then
            all=cmd_completion(compl,cmd,prefix..prefix_space) 
        else 
            local f=compl[cmd]
            if type(f)=="table" then
                return get_completion(f,arg,prefix..prefix_space..cmd)
            elseif type(f)=="function" then
                local arg_prefix_space,_arg=string.match(arg,"^(%s*)(.*)$") --remove the prefix space
                return f(_arg,prefix..prefix_space..cmd..arg_prefix_space)
            end
        end
        return all 
    end

    local function addcompletion(c,s)
        for i,v in pairs(get_completion(completions,s)) do
            L.addcompletion(c,v)
        end
    end

    function console_base.main()
        L.historyload(history) -- load existing history
        L.setcompletion(addcompletion)

        L.clearscreen()
        print '----- Testing lua-linenoise! ------'
        pprint.pprint(L)
        print '----- Welcome! have a nice day! ------\n'
        local line = L.linenoise(prompt)
        while line do
            if #line > 0 then
                L.historyadd(line)
                L.historysave(history) -- save every new line
                --parse cmdline
                local argv=parse_opt(line)
                --            pprint.pprint(argv)
                cmds[argv[1]](argv)

            end
            --L.clearscreen()
            line = L.linenoise(prompt)
        end
    end

    return console_base:new(o)
end

function Console()
    local c=ConsoleBase()
    
    c.register("exit",glr.exit,"")

    local gar=require "pack_gar"
    local configi=require "configi"

    c.register("gar",gar.helper,gar.completion)
    c.register("config",configi.helper,configi.completion)

    return c
end


local function main()
    local c=Console()
    c.main()
end


if ...=="__main__" then
    main()
end
