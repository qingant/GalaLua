--[[
-- console
--
--]]

local os=require "os"
local string=require "string"
local pprint=require "pprint"
local L = require 'linenoise'
local instance=require "instance"
local gar=require "pack_gar"
--local configi=require "configi"

local history,prompt="history.log",">"


function parse_opt(line)
    local opt={}
    for i in line:gmatch("%s*([^%s]+)") do
        opt[#opt+1]=i
    end
    return opt
end


local cmds={}
function cmds.help(cmd)
    io.write("available commands: \n")
    for i in pairs(cmds) do
        io.write("\t",i,"\n")
        --FIXME:better help information
--        io.write("\t",i,":",i.info(),"\n")
    end
end

cmds.instance=instance.helper
cmds.gar=gar.helper
--cmds.config=configi.helper
cmds.exit=glr.exit

local mt={__index=function (t,key) io.write(key,": Command not found!\n") return cmds.help end}
    
setmetatable(cmds,mt)

local completions={help="",exit=""}
--completions.config=configi.completion
completions.instance=instance.completion
completions.gar=gar.completion

--command completion
local function cmd_completion(completions,s,prefix)
    local ret={}
    for i in pairs(completions) do
        if string.match(i,"^"..s) then
            ret[#ret+1]=prefix..i
        end
    end
    return ret
end

function get_completion(completions,s,prefix)
    local prefix=prefix or ""
    assert(s,"can't be nil")
    local all={}
    local prefix_space,cmd,arg=s:match("^(%s*)([^%s]*)(.*)$")
    if arg=="" then
        all=cmd_completion(completions,cmd,prefix..prefix_space) 
    else 
        local f=completions[cmd]
        if type(f)=="table" then
            return get_completion(f,arg,prefix..prefix_space..cmd)
        elseif type(f)=="function" then
            local arg_prefix_space,_arg=string.match(arg,"^(%s*)(.*)$") --remove the prefix space
            return f(_arg,prefix..prefix_space..cmd..arg_prefix_space)
        end
    end
    return all 
end

function addcompletion(c,s)
    for i,v in pairs(get_completion(completions,s)) do
        L.addcompletion(c,v)
    end
end

function main()

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


if ...=="__main__" then
    main()
end
