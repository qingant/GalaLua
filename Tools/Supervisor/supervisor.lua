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

local _interface=require "interface"
local cmd_template=require "cmd"

function pairByKey(t,f)
    local a={}
    for k in pairs(t) do a[#a+1]=k end
    table.sort(a,f)
    local i=0
    return function ()
        i=i+1
        return a[i],t[a[i]]
    end
end

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

    local status=status.result
    local pid=""
    if status.pid then
        pid=string.format("[pid:%s]",status.pid)
    end

    out.add(string.format("%s\t[%s]  %s",status.name,status.state,pid))
    local glr_p=status.nodes or {}
    for gpid,s in pairByKey(glr_p) do
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

local help={
    start="start process, eg start lsr0.",
    stop="stop process, eg stop lsr0.",
    startall="start all processes.",
    stopall="stop all processes.",
    start_monitor="start supervisord.",
    stop_monitor="stop supervisord.",
    status="get process's status, eg status lsr0.",
    statusall="get all processes's status.",
    config="list valid configures.",
    list="list all processes.",
}

local help_more={
    start="'start <process>', start process, eg 'start lsr0'.",
    stop="'stop <process>', stop process, eg 'stop lsr0'.",
    startall="start all processes.",
    stopall="stop all processes.",
    start_monitor="start supervisord.",
    stop_monitor="stop supervisord.",
    status="'status <process>', get process's status, eg 'status lsr0'.",
    statusall="get all processes's status.",
    config="list valid configures.",
    list="list all processes.",
}

--if call function @cmd without right number of 
--argument, print help message and return.
--@name:
--@cmd:
--@argc: a table of available argument number, 
--       or just a number
function all_cmds(name,cmd,argc)
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
        return cmd_template.cmd_error("Not enough argument!\nUsage:%s",help_more[name])
    end
    return _cmd
end


local function status(name)
    local interface=_interface.new()
    local st=interface:status(name)
    local ret={}
    local out=cmd_output()
    for i,s in pairs(st) do
        ret[s.name]=s
    end
    for k,s in pairByKey(ret) do
        if s.status==0 then
            show_status(s)
        else
            out.error(s.result)
        end

    end
end

local function start_stop_show(ret,out)
    local sort_ret={}
    for i,v in ipairs(ret) do
        local content=v.result
        sort_ret[v.name]=v
    end
    for name,v in pairByKey(sort_ret) do
        if v.status==0 then
            out.result(name,v.result.state)
        else
            out.error(v.result)
        end
    end

end

local function start(name)
    local out=cmd_output()
    out.show(string.format("starting %s......",name))
    local interface=_interface.new()
    local sort_ret={}
    local ret=interface:start(name)
    start_stop_show(ret,out)
end


local function stop(name)
    local out=cmd_output()
    out.show(string.format("stopping %s......",name))
    local interface=_interface.new()
    local sort_ret={}
    local ret=interface:stop(name)
    start_stop_show(ret,out)
end


local function stop_monitor()
    local out=cmd_output()
    out.show("stopping supervisord........")
    local interface=_interface.new()
    local ret,msg=interface:stop_supervisord() 
    out.result("stop supervisord",ret)
end



function start_monitor()
    local out=cmd_output()
    out.show("starting supervisord........")
    local interface=_interface.new()
    local err,msg=interface:start_supervisord()
    out.result("start supervisord",err)
end


--list config
local function config(name)
    local out=cmd_output()
    local interface=_interface.new()
    local ret,errmsg=interface:config(name)
    if ret then
        ret=ret[1]
        if not (ret and ret.result and next(ret.result)) then 
            return out.warn(string.format("No valid configures"))
        end
        if ret.status==0 then
            for i,c in ipairs(ret.result) do
                show_config(c)
            end
        else
            out.error(ret.result)
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
    local interface=_interface.new()
    local ret,errmsg=interface:list()
    if ret then
        show_supervisord(ret[1].result)
    else
        out.error(errmsg)
    end
end

local cmds=cmd_template.init_cmd("supervisor")

function add_command(name,func,argc)
    cmds:add(name,all_cmds(name,func,argc),help[name],help_more[name])
end

add_command("stop_monitor",stop_monitor)
add_command("start_monitor",start_monitor)
add_command("start",start,1)
add_command("stop",stop,1)
add_command("status",status,1)
add_command("config",config,{0,1})
add_command("list",list)

function helper(argv)
    return cmds:helper(argv)
end

function info()
    return "process monitor tool"
end

completion={}
completion.start_monitor=""
completion.stop_monitor=""
completion.start=""
completion.startall=""
completion.stop=""
completion.stopall=""
completion.config=""
completion.list=""
completion.statusall=""
completion.status=""
completion.help=""

