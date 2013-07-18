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

local serv_host,serv_port="127.0.0.1",56789

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


--return true if supervisord is started
--@sec: delay @sec seconds
function isStarted(sec)
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
                return 
            end

            if name=="all" then
                name=nil
            end

            local DefaultGroup="wg1"

            local _conf=configure(config.create())
            local conf_entries=_conf:get_config(DefaultGroup,name)

            glr.send(addr,cjson.encode({cmd=cmd,name=conf_entries}))
            
            --XXX:waiting replies.
            for i=1,#conf_entries do
                local msg_type,addr,msg=glr.recv()

                print("GETMSG:",msg)
                local msg_table=cjson.decode(msg)

                pprint.pprint(msg_table)
            end
        else
            cmds.help(cmd)
        end
    end
    return docmd
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
function cmds.start_monitir(name)
    
    if name then
        return cmds.help("start_monitir")
    end
    
    if isStarted() then
        io.write("supervisord alreadly started...\n")
    else
        ret=os.execute("glr -m supervisord -e main -h 127.0.0.1 -p 56789 -D")
        if isStarted(5) then
            io.write("supervisord is running now... \n")
        else
            io.write("start_monitor error  \n")
        end
    end
end

cmds.start=all_cmds("start")
cmds.stop=all_cmds("stop")
cmds.status=all_cmds("status")

function cmds.help(arg)
    local help_msg={
            start="start ctr0",
            stop="stop ctr0",
            start_monitor="start_monitor",
            status="status ctr0",
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


