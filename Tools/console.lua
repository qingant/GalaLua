--[[
-- console
--
--]]

module(...,package.seeall)

local os=require "os"
local string=require "string"
local pprint=require "pprint"
local L = require 'linenoise'
local cmd=require "cmd"

function parse_opt(line)
    local opt={}
    for i in line:gmatch("%s*([^%s]+)") do
        opt[#opt+1]=i
    end
    return opt
end

function writef(fmt,...)
    if type(fmt)=="string" then
        io.write(string.format(fmt,...))
    end
end

function ConsoleBase(o)

    local console_base={}
    
    --completions:{command_name=completion_func}
    --cmds:{command_name=command_func}
    local cmds=cmd.init_cmd()
    local completions={help=""}

    local history,prompt=os.getenv("HOME").."/log/console.log","gdkshell> "
    local header='--------- GDK Console ----------\n\n'
    function console_base.set_history(file)
        history=file or history
    end

    function console_base.set_prompt(s)
        prompt=s or prompt
    end
    
    function console_base.set_header(s)
        header=s or header
    end

    function console_base.show_header()
        L.clearscreen()
        writef(header)
    end

    --[[
    -- register command and completion  
    --
    -- @name: register @helper as @name in console
    -- @helper: command function
    -- @completion: completion function
    --]]
    function console_base.register(name,helper,completion,info,more_info)
        cmds:add(name,helper,info or "",more_info)
        completions[name]=completion
    end

    console_base.register("clear",console_base.show_header,"","clear screen","clear clean")

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

    function console_base.safe_run(func,...)
        local arg={...}
        if type(func)=="function" then
            --local ok,errmsg=xpcall(function () return func(unpack(arg)) end,debug.traceback)
            local ok,errmsg=pcall(func,unpack(arg))
            if not ok then
                cmd.perror(errmsg)
            end
        end
    end

    function console_base.main()
        L.historyload(history) -- load existing history
        L.setcompletion(addcompletion)

        console_base.show_header()        
        local line = L.linenoise(prompt)
        while line do
            if #line > 0 then
                L.historyadd(line)
                L.historysave(history) -- save every new line
                --parse cmdline
                local argv=parse_opt(line)
                local cmd=argv[1]

                if cmd=="exit" then
                    break
                end
                console_base.safe_run(cmds:get_cmd(cmd),argv)

            end
            line = L.linenoise(prompt)
        end
    end

    return console_base
end


function Console()
    local c=ConsoleBase()
    
    local gar=require "pack_gar"
    local configi=require "configi"
--    local sup=require "supervisor"
    local mdb_copy=require "mdb_copy"

--    c.register("supervisor",sup.helper,sup.completion,sup.info)
    c.register("gar",gar.helper,gar.completion,gar.info)
    c.register("config",configi.helper,configi.completion,configi.info)
    c.register("mdb_copy",mdb_copy.helper,mdb_copy.completion,mdb_copy.info)

    return c
end


function main()
    local c=Console()
    c.main()
end


if ...=="__main__" then
    main()
end
