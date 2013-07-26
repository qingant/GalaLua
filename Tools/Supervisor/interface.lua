--[[
--
-- supervisord interface
--
--]]
module(...,package.seeall)

local pprint=require "pprint"
local cjson=require "cjson"
local io=require "io"

local supervisor_conf=require "supervisor_conf"


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
function configure(env)
    local Configure={}

    function Configure:init()
        self._conf=supervisor_conf.watchConf(supervisor_conf.create())
    end

    Configure:init()
    
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
    local n=n or 1

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

local function all_cmds(cmd)
    assert(cmd,"cmd can't be nil")
    function docmd(name)
        if name then
            local addr=isStarted()
            if not addr then
                return nil,"supervisord is not running"
            end

            if name=="*all" then
                name=nil
            end

            local _conf=configure()
            local conf_entries=_conf:get_config(supervisor_conf.defaultGroup,name)

            if not next(conf_entries) then
                return nil,"no valid config with "..name
            end

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
            return  nil,"argument can't be nil"
        end
    end
    return docmd
end

function statusall()
    all_cmds("status")("*all")
end

function startall()
    all_cmds("start")("*all")
end

function stopall()
    all_cmds("stop")("*all")
end

start=all_cmds("start")
stop=all_cmds("stop")
status=all_cmds("status")

--stop supervisord
function stop_supervisord()
    local addr =isStarted()
    if addr then
        addr.gpid=1
        glr.send(addr,cjson.encode({Type="NODE",Action="EXEC",Cmd="kill"}))
        return true
    else
        return true,"supervisord is alreadly stopped"
    end
end

--start supervisord
--return true if started, else return nil and error message
function start_supervisord()
    if isStarted() then
        return true,"supervisord alreadly started"
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
            return true 
        else
            return nil,"start_monitor error"
        end
    end
end

function config(name)
    local _conf=configure()
    return _conf:get_config(supervisor_conf.defaultGroup,name)
end

function list()
    local addr=isStarted()
    if not addr then
        return  nil,"supervisord is not running"
    end

    glr.send(addr,cjson.encode({cmd="list"}))

    --XXX:waiting replies.
    local msg_type,addr,msg=glr.recv()    --TODO:timeout for failed

    return cjson.decode(msg)
end
