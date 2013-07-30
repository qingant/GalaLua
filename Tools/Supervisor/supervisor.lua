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

local interface=require "interface"

local ALL={"svc","lsr","ctr"}

function output()
    local O={}
    local out={}

    function O.add(...)
        local s=""
        for i,v in ipairs{...} do
            s=s..v
        end
        out[#out+1]=s 
    end

    function O.write()
        O.add()
        io.write(table.concat(out,"\n"))
    end

    return O
end

local function show_status(status)
    local out=output()

    local sep1=("*"):rep(40)
    local sep2=("-"):rep(20)
    out.add(sep1)

    local pid=""
    if status.pid then
        pid=string.format("[pid:%s]",status.pid)
    end

    out.add(string.format("%s\t[%s]  %s",status.name,status.state,pid))
    local glr_p=status.nodes or {}
    for gpid,s in pairs(glr_p) do
        out.add(sep2)
        out.add("gpid:"..gpid)
        for k,v in pairs(s) do
            out.add("\t"..k..":"..v)
        end
    end
    out.add(sep1)
    out.write()
end

local function show_config(conf)
    local out=output()
    local token=string.format("%s%.4d",conf.module,conf.index)
    out.add(token)
    out.add(("*"):rep(50))
    out.add(token..":")
    for k,v in pairs(conf) do
        out.add("\t"..k..":"..v)
    end
    out.add(("*"):rep(50))
    out.write()
end

local function help(arg)
    local help_msg={
            start="start xxx: eg start ctr0",
            stop="stop xxx: eg stop ctr0",
            startall="startall: start all processed with valid configure",
            stopall="stopall: stop all process",
            start_monitor="start_monitor: start supervisord",
            status="status xxx: eg status ctr0",
            statusall="statusall: get all processes status",
            help="help [cmd]",
            config="config: list valid configures",
            list="list: list all processes ",
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

--if call function @cmd without right number of 
--argument, print help message and return.
--@name_in_help:
--@cmd:
--@argc: a table of available argument number, 
--       or just a number
function all_cmds(name_in_help,cmd,argc)
    assert(cmd,"cmd can't be nil")
    local argc=argc or 0
    if type(argc)=="number" then
        argc={argc}
    end
    function _cmd(...)
        local argc_pass=select('#',...)
        pprint.pprint(argc,argc_pass)
        for i,v in ipairs(argc) do
            if v==argc_pass then
                return cmd(...)
            end
        end
        return help(name_in_help)
    end
    return _cmd
end

local function status(name)
    local st=interface.status(name)
    if st then
        for i,s in ipairs(st) do
            show_status(s.result)
        end
    end
end


local function stop_supervisord()
    local ret,msg=interface.stop_supervisord() 
    if ret then
        io.write(msg or "supervisor is stopped","\n")
    end
end

local function start_supervisord()
    local ret,msg=interface.start_supervisord()
    if ret then
        io.write(msg or "supervisor is running now","\n")
    else
        io.write(msg,"\n")
    end
end

local function start(name)
    local ret,msg=interface.start(name)
    local out=output()
    if ret then
        for i,v in ipairs(ret) do
            local content=v.result
            out.add(string.format("%s\t[%s]",content.name,content.state))
        end
    else
        out.add(msg)
    end
    out.write()
end

local function stop(name)
    local ret,msg=interface.stop(name)
    local out=output()
    if ret then
        for i,v in ipairs(ret) do
            local content=v.result
            out.add(string.format("%s\t[%s]",content.name,content.state))
        end
    else
        out.add(msg)
    end
    out.write()
end

local cmds={}
cmds.stop_monitor=all_cmds("stop_monitor",stop_supervisord)
cmds.start_monitor=all_cmds("start_monitor",start_supervisord)

cmds.start=all_cmds("start",start,1)
cmds.stop=all_cmds("stop",stop,1)
cmds.status=all_cmds("status",status,1)

local function startall()
    for i=1,#ALL do
        cmds.start(ALL[i])
    end
end

local function stopall()
    for i=#ALL,1,-1 do
        cmds.stop(ALL[i])
    end
end

local function statusall()
    for i=1,#ALL do
        cmds.status(ALL[i])
    end
end

--list config
local function config(name)
    local conf_entries=interface.config(name)

    if #conf_entries==0 then
        io.write("no available config with ",name,"\n")
    else
        for i,c in ipairs(conf_entries) do
            show_config(c)
        end
    end
end


--list all processes that supervisord monitoring
local function list()
    local ret,errmsg=interface.list()
    pprint.pprint(ret,"LIST")
    if ret then
        local p=ret.result or {}
        for i,c in ipairs(p) do
            show_config(c)
        end
    else
        io.write(errmsg,"\n")
    end
end

cmds.stopall=all_cmds("stopall",stopall)
cmds.statusall=all_cmds("statusall",statusall)
cmds.startall=all_cmds("startall",startall)
cmds.config=all_cmds("config",config,{0,1})
cmds.list=all_cmds("list",list)

cmds.help=help

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


