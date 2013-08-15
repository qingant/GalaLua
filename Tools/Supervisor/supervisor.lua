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

--local ALL={"svc","lsr","ctr"}
local ALL={"svc","lsr"}


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

    function O.format(sep)
        return table.concat(out,sep or "\n")
    end

    function O.write()
        O.add()
        io.write(table.concat(out,"\n"))
    end

    return O
end

--command output
function cmd_output()
    local out={}

    function out.show(...)
        local o=output()
        o.add(...)
        o.write()
    end

    function out.result(prompt,state)
        local s=type(state)
        if (s=="boolean" or s=="nil") then
            if state then
                out.success(prompt)
            else
                out.fail(prompt)
            end
        elseif(s=="string" or s=="number") then
            out.show(string.format("%s   \t [%s]",prompt,state))
        end
    end

    function out.fail(prompt,sep)
        local sep=sep or (" "):rep(10)
        out.show(string.format("%s%s[FAIL]",prompt,sep))
    end

    function out.success(prompt)
        local sep=sep or (" "):rep(10)
        out.show(string.format("%s%s[OK]",prompt,sep))
    end

    function out.error(prompt)
        out.show("Error: ",prompt)
    end
    function out.warn(prompt)
        out.show("Warn: ",prompt)
    end

    return out
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
            help="help [cmd]: show this help.",
            start="start <process>: start process, eg start lsr0.",
            stop="stop <process>: stop process, eg stop lsr0.",
            startall="startall: start all processes.",
            stopall="stopall: stop all processes.",
            start_monitor="start_monitor: start supervisord.",
            stop_monitor="stop_monitor: stop supervisord.",
            status="status <process>: get process's status, eg status lsr0.",
            statusall="statusall: get all processes's status.",
            config="config: list valid configures.",
            list="list: list all processes.",
         }

    local arg=arg or "all"
    local out=output()
    local prompt=help_msg[arg] 
    if not prompt then 
        out.add("Usage: supervisor <CMD> [process]")
        out.add("Available commands are:")
        local space=(" "):rep(2)
        for i,v in pairs(help_msg) do
            out.add(string.format("%s%-15s%s",space,i,v))
        end
    else
        out.add("Usage: ",prompt)
    end
    out.write()
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
        for i,v in ipairs(argc) do
            if v==argc_pass then
                return cmd(...)
            end
        end
        return help(name_in_help)
    end
    return _cmd
end

local function error_out(name,ret,msg)
    local out=output()
    local success_out={}
    if ret then
        if #ret==0 then
            out.add(string.format("Error: No such process: \"%s\"",name))
        else
            success_out=ret
        end
    else
        if msg then
            out.add("Error: ",msg)
        end
    end
    out.write()
    return success_out
end

local function status(name)
    local st=error_out(name,interface.status(name))
    for i,s in ipairs(st) do
        show_status(s.result)
    end
end

local function start(name)
    local out=cmd_output()
    out.show(string.format("starting %s......",name))
    local ret=error_out(name,interface.start(name))
    for i,v in ipairs(ret) do
        local content=v.result
        out.result(content.name,content.state)
    end
end

local function stop(name)
    local out=cmd_output()
    out.show(string.format("stopping %s......",name))
    local ret=error_out(name,interface.stop(name))
    for i,v in ipairs(ret) do
        local content=v.result
        out.result(content.name,content.state)
    end
end


local function stop_supervisord()
    local out=cmd_output()
    out.show("stopping supervisord........")
    local ret,msg=interface.stop_supervisord() 
    out.result("stop supervisord",ret)
end


local cmds={}
cmds.stop_monitor=all_cmds("stop_monitor",stop_supervisord)
cmds.start=all_cmds("start",start,1)
cmds.stop=all_cmds("stop",stop,1)
cmds.status=all_cmds("status",status,1)

function start_monitor()
    local out=cmd_output()
    out.show("starting supervisord........")

    out.show("importing configure........")
    local conf=require "supervisor_conf"
    local sup_conf=conf.watchConf(conf.create())
    sup_conf:import()

    out.success("importing configure")

    local err,msg=interface.start_supervisord()
    out.result("start supervisord",err)
end

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
    local out=cmd_output()
    local ret,errmsg=interface.config(name)
    if ret then
        ret=ret[1]
        if not (ret and ret.result and next(ret.result)) then 
            return out.warn(string.format("No valid configures"))
        end
        for i,c in ipairs(ret.result) do
            show_config(c)
        end
    else
        out.error(errmsg)
    end
end

local function show_supervisord(result)
    local out=output()
    out.add(("+"):rep(30))
    local statistics=result.status
    for k,v in pairs(statistics) do
        out.add(string.format("%-10s%s",k,v))
    end
    out.add(("+"):rep(30))
    for i,v in ipairs(result.process) do
        local token=string.format("%s%.4d",v.module,tonumber(v.index) or -1)
        out.add(string.format("%-10s[%s]",token,v.state))
    end
    out.write()

end

--list all processes that supervisord monitoring
local function list()
    local out=cmd_output()
    local ret,errmsg=interface.list()
    if ret then
        show_supervisord(ret[1].result)
    else
        out.error(errmsg)
    end
end

cmds.start_monitor=all_cmds("start_monitor",start_monitor)
cmds.stopall=all_cmds("stopall",stopall)
cmds.statusall=all_cmds("statusall",statusall)
cmds.startall=all_cmds("startall",startall)
cmds.config=all_cmds("config",config,{0,1})
cmds.list=all_cmds("list",list)

cmds.help=help

local mt={__index=function (t,key) io.write("supervisor ",key,":command not found\n") return cmds.help end }
setmetatable(cmds,mt)

function helper(argv)
--    pprint.pprint(argv)
--    pprint.pprint(cmds)

    table.remove(argv,1)
    --pprint.pprint(argv)
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


