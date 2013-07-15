--[[
-- 
-- interactive console for managing AIM processes
--
--
--]]

module(...,package.seeall)

local pprint=require "pprint"
local cjson=require "cjson"
local io=require "io"

local serv_host,serv_port="127.0.0.1",56789

local cmds={}
function cmds.start(name)
    if name then
        local host=serv_host
        local port=serv_port
        glr.connect(host,port)

        glr.send({host=host,port=port,gpid=0},
        cjson.encode({cmd="start",name=name}))
        --XXX:waiting reply ????
    end
end

function cmds.stop(name)
    print("stop",name)
    if name then
        local host=serv_host
        local port=serv_port
        glr.connect(host,port)
        glr.send({host=host,port=port,gpid=0},
        cjson.encode({cmd="stop",name=name}))
    end
end

function cmds.help(arg)
    local help_msg={
            start="start ctr0",
            stop="stop ctr0",
            help="help [cmd]"
         }

    local arg=arg or "all"

    local prompt=help_msg[arg] 
    if not prompt then 
        prompt="available command:\n"
        for i,v in pairs(help_msg) do
            prompt=prompt.."\t"..v.."\n"
        end
    end
    io.write(prompt,"\n")
end

local mt={__index=function (t,key) io.write("monitor ",key,":command not found\n") return cmds.help end }
setmetatable(cmds,mt)

function helper(argv)
    pprint.pprint(argv)
    pprint.pprint(cmds)

    table.remove(argv,1)
    pprint.pprint(argv)
    local cmd=argv[1] 
    table.remove(argv,1)
    if cmd then
        cmds[cmd](unpack(argv))
    else
        cmds.help()
    end
end

function info()
    return "process monitor tool"
end

completion={}
for i,v in pairs(cmds) do
    completion[i]=""
end


