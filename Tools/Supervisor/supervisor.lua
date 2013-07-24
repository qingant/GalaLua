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

local config=require "supervisor_conf"


function get_supervisord_arg()
    local Config=(require "config").Config
    local db_path=require "db_path"

    local _conf= Config:new():init(db_path.config)
    local host=_conf:get("SUP/Address/IP")
    local port=_conf:get("SUP/Address/Port")
    local gar=_conf:get("SUP/GarName")
    return host,port,gar
end


--[[
    configure
]]
local configure_base={}
function configure_base:new(o)
    local o=o or {}
    setmetatable(o,self)
    self.__index=self
    configure_base.init(o)

    return o
end

function configure_base:init()
    self._conf=config.watchConf(self._env)
end
function configure(env)
    local Configure=configure_base:new({_env=env})
    
    --id: host::port
    function Configure:get_config_by_id(id,_type)
        local _type=_type or "valid"
        local conf_entries=self._conf:find_by_id(id)
        return filter(conf_entries,_type=="valid")
    end

    -- group:which group
    -- name:must match pattern (module_name..index)
    --      lsr0 ===> lsr:module_name 0:index
    --      lsr  ===> lsr:module_name match all indexs
    --      if not passed, then return the whole @group
    -- _type: default value is "valid". 
    -- return: a table with all found configure entries,
    --        empty table if not found. Default is just 
    --        valid config entry.
    function Configure:get_config(group,name,_type)
        assert(group,"group must be passed!")
        local module_name,index
        local _type=_type or "valid"
        if name then
            local i=name:find("%d")
            if i then
                index=assert(tonumber(name:sub(i)),"not a valid name")
                
                module_name=name:sub(1,i-1)
            else
                module_name=name
            end
            print(module_name,index)
        end

        local conf_entries=self._conf:find(group,module_name,index)
        
        return filter(conf_entries,_type=="valid")
    end

    --[[
    -- @valid:just return those valid
    --]]
    function filter(conf_entries,valid)
        if valid then
            local ret={}
            for i,v in pairs(conf_entries) do
                if v.valid then
                    ret[#ret+1]=v
                end
            end
            return ret
        else
            return conf_entries
        end
    end
    
    return Configure
end

function sleep(n)
    assert(n,"must pass seconds")
    local cmd=string.format("sleep %d",n)
    os.execute(cmd)
end

--return true if supervisord is started
--@sec: delay @sec seconds
function isStarted(sec)
    local serv_host,serv_port,DefaultGar=get_supervisord_arg()
    local sec=sec or 0
    local i=0
    local addr={host=serv_host,port=serv_port,gpid=0}
    while true do
        local ret=glr.connect(serv_host,serv_port)
        if ret then
            return addr
        else
            if i<sec then
                sleep(1)
            else
                break
            end
        end
        i=i+1
    end
end


local cmds={}
function all_cmds(cmd)
    function docmd(name)
        if name  then
            local addr=isStarted()
            if not addr then
                io.write("supervisord is not running\n")
                io.write("run 'supervisor start_monitor' to start it\n")
                return 
            end

            local DefaultGroup="wg1"

            local _conf=configure(config.create())
            local conf_entries=_conf:get_config(DefaultGroup,name)

            glr.send(addr,cjson.encode({cmd=cmd,name=conf_entries}))
            
            pprint.pprint(conf_entries,"SEND")
            local ret={}
            --XXX:waiting replies.
            for i=1,#conf_entries do
                local msg_type,addr,msg=glr.recv()    --TODO:timeout for failed

                print("GETMSG:",msg)
                pprint.pprint(addr,msg_type)
                ret[#ret+1]=cjson.decode(msg)
            end
            return ret
        else
            cmds.help(cmd)
        end
    end
    return docmd
end

function startall()
    all_cmds("start")("svc")
    all_cmds("start")("lsr")
    all_cmds("start")("ctr")
end

function stopall()
    all_cmds("stop")("ctr")
    all_cmds("stop")("lsr")
    all_cmds("stop")("svc")
end

--stop supervisord
function cmds.stop_monitor()
    local addr =isStarted()
    if addr then
        addr.gpid=1
        glr.send(addr,cjson.encode({Type="NODE",Action="EXEC",Cmd="kill"}))
    else
        io.write("supervisord is alreadly stopped\n")
    end
end
--start supervisord, not argument should be passed.
function cmds.start_monitor(name)
    
    if name then
        return cmds.help("start_monitor")
    end
    
    if isStarted() then
        io.write("supervisord alreadly started...\n")
    else
        local serv_host,serv_port,DefaultGar=get_supervisord_arg()
        local gar=""
        if DefaultGar and DefaultGar~="" then
            gar="--gar="..DefaultGar
        end
        local cmd=string.format("glr -m supervisord -e main -h %s -p %d %s -D ",
                                serv_host,serv_port,gar)
        ret=os.execute(cmd)
        if isStarted(5) then
            io.write("supervisord is running now... \n")
        else
            io.write("start_monitor error  \n")
        end
    end
end

function show_status(status)
    local out={}
    function add(s)
        out[#out+1]=s or ""
    end
    local content=status.content
    local self=status.self
    local token=string.format("%s%.4d",self.module,tonumber(self.index))
    local sep1=("*"):rep(40)
    local sep2=("-"):rep(20)
    add(sep1)
    add(string.format("%s  state:%s",token,content.state))
    add(string.format("host:%s   port:%s",self.host,self.port))
    local glr_p=content.status or {}
    for gpid,s in pairs(glr_p) do
        add(sep2)
        add("gpid:"..gpid)
        for k,v in pairs(s) do
            add("\t"..k..":"..v)
        end
    end
    add(sep1)
    add()
    io.write(table.concat(out,"\n"))
end

function cmds.status(name)
    local st=all_cmds("status")(name)
    for i,s in ipairs(st) do
        show_status(s)
    end
end

cmds.start=all_cmds("start")
cmds.stop=all_cmds("stop")

cmds.startall=startall
cmds.stopall=stopall


function list_config()
    local DefaultGroup="wg1"
    local _conf=configure(config.create())
    local conf_entries=_conf:get_config(DefaultGroup)
    for i,c in ipairs(conf_entries) do
        output(c)
    end
end

function output(conf)
    local token=string.format("%s%.4d",conf.module,conf.index)
    io.write(("*"):rep(50),"\n")
    io.write(token,":\n")
    for k,v in pairs(conf) do
        io.write("\t",k,":",v,"\n")
    end
    io.write(("*"):rep(50),"\n")
end

function cmds.list(arg)
    if not arg then
        local addr=isStarted()
        if not addr then
            return 
        end

        glr.send(addr,cjson.encode({cmd="list"}))

        --XXX:waiting replies.
        local msg_type,addr,msg=glr.recv()    --TODO:timeout for failed

        print("GETMSG:",msg)
        pprint.pprint(addr,msg_type)
        local msg_table=cjson.decode(msg)

        pprint.pprint(msg_table)
    else
        cmds.help(cmd)
    end
end

cmds.config=list_config

function cmds.help(arg)
    local help_msg={
            start="start ctr0",
            stop="stop ctr0",
            startall="startall",
            stopall="stopall ",
            start_monitor="start_monitor",
            status="status ctr0",
            help="help [cmd]",
            config="list valid configures",
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


