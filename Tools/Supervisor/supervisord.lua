--[[
--
--]]
module(...,package.seeall)


local os=require "os"
local pprint=require "pprint"
local cjson=require "cjson"
local conf=require "supervisor_conf"

local db_path=require "db_path"
local path=require "path"

--process state
local STATE={
    STOPPED=1,
    RUNNING=2,
    STOPPING=3,
    STARTING=4,
}

local MSG_TYPE={
    APP=3,
}

local logdir=os.getenv("HOME").."/log"
local DefaultGroup="wg1"

--default gar package search for module
local DefaultGar=glr.get_option("gar")

function execute(cmd)
    return os.execute(cmd)
end

function sleep(s)
    local s=s or 1
    os.execute("sleep "..s)
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
    self._conf=conf.watchConf(self._env)
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

--[[
    process
]]
local p={}
function p:new(o)
    local o=o or {}
    setmetatable(o,self)
    self.__index=self
    p.init(o)

    return o
end

function p:init()
    pprint.pprint(self.entry,"ENTRY")
    assert(self.entry or self.entry.group or self.entry.module,
            "not enough element in entry")
end

function process(entry)
    local Process=p:new{entry=entry}
    
    function Process:stop()
        self.last_cmd="stop"

        local state=self:get_state()
        if (state~=STATE.STOPPING) and (state ~=STATE.STOPPED) then
            self.entry.state=STATE.STOPPING
            local addr={host=self.entry.host,port=self.entry.port,gpid=1}

            local ret=glr.send(addr,cjson.encode({Type="NODE",Action="EXEC",Cmd="kill"})) 
            if ret then
                state=self:get_state()
            end
            if (state ~=STATE.STOPPED) then
                -- just kill -9 it
                local stopcmd=self.entry.stopcmd or string.format("kill -9 %d",self.entry.pid)
                execute(stopcmd)
            end
    --        self:wait_till_finished()
        end
    end
   
    --{{
    -- TODO:add timeout?
    --}}
    function Process:wait_till_finished()
        while true do
            local state=self:get_state()
            if (state==STATE.RUNNING) or (state==STATE.STOPPED) then
                break
            end
            sleep(1)
        end
    end

    --[[get_state:
       1.starting-->glr.connect--success-->running
                      |failed
                    starting
       2.stopping-->glr.connect--success-->stopping
                      |failed
                    stopped
       3.nil------->glr.connect--success-->running
                      |failed
                    stopped
       4.stopped--->glr.connect (should never success)
                      |failed
                    stopped
       5.running--->glr.connect--success-->running
                      |(only fail when process exited accidently)
                    stopped
       
       Note:if the origin state is `stopped`, process state should be `stopped` whether 
            glr.connect succeeded or not. Because there may be other server program
            listen on that host and port.

    ]]
    function Process:get_state()
        local host=assert(self.entry.host,"host should not be empty")
        local port=assert(self.entry.port,"port should not be empty")
        
        --original state
        local state=self.entry.state
        --if (state==STATE.RUNNING) or (state==STATE.STOPPED) then
        --    return state
        --end
        if (state==STATE.STOPPED) then
            return state
        end

        local ret=glr.connect(host,port)
        if ret then
            --((not state) or (state=STATE.STARTING) or (state=STATE.RUNNING))
            if (state~=STATE.STOPPING) then
                state=STATE.RUNNING
            end
        else
            --((not state ) or (state==STATE.STOPPING) or (state=STATE.RUNNING))
            if (state~=STATE.STARTING) then
                state=STATE.STOPPED
            end
        end

        --update state
        self.entry.state=state

        return state
    end
    
    function Process:auto_start()
        if self.last_cmd=="stop" then   --we just want to stop process 
            return
        end
        self:start()
        self.last_cmd="auto_start"
    end

    --XXX:maybe run module from gar package
    function Process:get_run_gar_arg()
        local gar=""
        local gar_search={}
        gar_search[#gar_search+1]=self.entry.gar
        gar_search[#gar_search+1]=DefaultGar
        for i,g in ipairs(gar_search) do
            if g ~="" then
                gar="-g "..path.join(db_path.gar_path,g)
                break
            end
        end
        return gar
    end

    function Process:start()
        self.last_cmd="start"
        --can only start those not started
        local state=self:get_state()
        if (state~=STATE.RUNNING) and (state~=STATE.STARTING) then
            self.entry.state=STATE.STARTING

            local gar=self:get_run_gar_arg()
            startcmd=string.format("glr %s -h %s -p %d -i %d -m %s -e main -D",
                                    gar,self.entry.host,self.entry.port,self.entry.index, self.entry.module)
            print(startcmd)
            local ret=execute(startcmd)
            print("execute:",ret)
            if ret==0 then    --XXX:useless?
                print("execute success")
            end
            --wait till it have started
            self:wait_till_finished()
        end
        self:getpid()
    end
    
    function Process:get_info_from_inspector(cmd)
        print("get_info_from_inspector........")
        local addr={host=glr.sys.host,port=glr.sys.port,gpid=__id__}

        local msg={Type="NODE",Action="GET",Cmd=cmd,ToAddr=addr,Nonstop=true}
        pprint.pprint(msg,"MSG_TO")
        if not glr.send({host=self.entry.host,port=self.entry.port,gpid=1},cjson.encode(msg)) then
            print("send to inspector error...")
        end

        print("waiting for NODE RES.....")
        while true do
            local msg_type,addr,msg=glr.recv()
            local msg_table=cjson.decode(msg)
            pprint.pprint(msg_table,"NODE RES")
            if msg_table.Type=="NODE" and 
               msg_table.Action=="RES"  and 
               msg_table.Cmd==cmd then
                return msg_table.Content
            end
        end
    end

    --[[
    --get process pid 
    --]]
    function Process:getpid()
        pprint.pprint(self.entry,"getpid...........")
        local state=self:get_state()
        if state==STATE.RUNNING then
            --XXX:only get pid when there's `nil
            if not self.entry.pid then
                self.entry.pid=self:get_info_from_inspector("pid").pid
            end
        end
        print("get pid:",self.entry.pid)
    end
    
    --FIXME: what should return when the process is stopped
    --TODO: we should collect some information when calling process start
    function Process:status()
        self.last_cmd="status"
        local state=self:get_state()
        if state==STATE.RUNNING then
            self.entry.status=self:get_info_from_inspector("status")
        else
            return self.entry.status
        end
    end

    --XXX:export self.entry, with it can rebuild Exactly the same process object.
    --    in order to send a process_object to other glr node, we must 
    --    use "cjson.encode" function to encode a process object to json string.
    --    cjson.encode(table1):there is no meaning if some of table1's value are functions.
    function Process:export()
        return self.entry
    end

    return Process
end

--[[
-- supervisor
--
]]
local supervisor_base={}
function supervisor_base:new(o)
    local o=o or {}
    setmetatable(o,self)
    self.__index=self
    supervisor_base.init(o)
    return o
end

function supervisor_base:init()
    local _env=conf.create()
    self._conf=configure(_env)
end

function supervisor()
    local Supervisor=supervisor_base:new({processes={}})

     --[[
    --  return the configure find by @id 
    --]]
    function Supervisor:get_config_by_id(id)
        return self._conf:get_config_by_id(id)
    end

    --[[
    -- 
    -- {
    --    [1]={...},
    --    [2]={...},
    --    ...
    -- }
    --]]
    function Supervisor:get_config(group,name)
        return self._conf:get_config(group,name)
    end
   
    -- group:which group
    -- name:must match pattern (module_name..index)
    --      lsr0 ===> lsr:module_name 0:index
    --      lsr  ===> lsr:module_name match all indexs
    -- return: a process table matching group and name,
    --         empty table if nothing found
    function Supervisor:get_processes(group,name)
        local conf_entries=self._conf:get_config(group,name)
        return self:get_processes_by_entries(conf_entries)
    end
    function Supervisor:get_processes(conf_entries)
        return self:get_processes_by_entries(conf_entries)
    end


    -- return processes specified by conf_entries in self.processes, create 
    -- a new process object if not existed. Empty table will be return 
    -- if no valid entry in conf_entries.
    -- self.processes={group1={lsr0000={...},svc0000={...}},group2={}}
    function Supervisor:get_processes_by_entries(conf_entries)
        pprint.pprint(conf_entries,"get_processes_by_entries")
        ret={} 
        for i,e in pairs(conf_entries) do
            local token=string.format("%s%.4d",e.module,e.index)
            if not self.processes[e.group]  then
                self.processes[e.group]={}
            end
            if (not self.processes[e.group][token]) then
                self.processes[e.group][token]=process(e)
            end
            ret[#ret+1]=self.processes[e.group][token]
        end
        pprint.pprint(ret,"get_processes_by_entries")
        return ret
    end

    -- return a new process object specified by conf_entry and save in self.processes.
    -- the return  value is {1=process object}
    function Supervisor:create_process(conf_entry)
        local e=conf_entry
        local token=string.format("%s%.4d",e.module,e.index)
        if not self.processes[e.group]  then
            self.processes[e.group]={}
        end

        self.processes[e.group][token]=process(e)
        return self.processes[e.group][token]
    end

    --[[
    -- make supervisor monitoring process with configure @conf_entry
    -- @conf_entry:
    --]]
    function Supervisor:attach(conf_entry)
        self:create_process(conf_entry)
        local e=conf_entry
        local token=string.format("%s%.4d",e.module,e.index)
        self.processes[e.group][token]:get_state()
        print("update_state")
    end

    return Supervisor
end

function main()

    node=supervisor()
    local cmds={start="",status="",config=""}
    while true do
        local msg_type, addr, msg = glr.recv()

        if msg_type == glr.CLOSED then
            local conf_entry=node:get_config_by_id(addr.host)
            if conf_entry[1] then
                local proc=node:get_processes_by_entries(conf_entry)[1]
                pprint.pprint(proc,"RESTART")
                if proc.last_cmd~="stop" then
                    local err,id=glr.spawn("supervisord","run_cmd",cjson.encode(conf_entry),"start")
                end
            end
        elseif MSG_TYPE.APP==msg_type then
            local msg_table=cjson.decode(msg)
            local cmd=msg_table.cmd
            if cmd=="return" then   --message from run_cmd function
                print("return from spawn ............")
                pprint.pprint(msg_table,"return")
                node:attach(msg_table.content)
            elseif cmd=="stop" then
                local procs=node:get_processes(msg_table.name)
                pprint.pprint(procs,"STOPP")
                for i,e in ipairs(procs) do   --procs may have more than one item
                    e:stop()
                end
                glr.send(addr,msg)
            elseif cmds[cmd] then   --message from client
                print(("*"):rep(20),msg,("*"):rep(20))

                local procs=node:get_processes(msg_table.name)
                for i,e in ipairs(procs) do   --procs may have more than one item
                    local err,id=glr.spawn("supervisord","run_cmd",cjson.encode(e:export()),msg_table.cmd,cjson.encode(addr))
                end
            end
        else
            glr.send(addr,cjson.encode({ret="unkown message",msg=msg}))
        end
    end
end


--[[
--  run command @cmd, then send `return` message to main process.
--  message format:
--  {
--      cmd="return",
--      content={
--                  host=...,
--                  port=...,
--                  group=...,
--                  state=...,
--                  ...
--              }
--  }
--
--]]
function run_cmd(proc,cmd,addr)
    local proc=cjson.decode(proc) 
    local _p=process(proc)
    _p[cmd](_p)

    local msg_table={}
    msg_table.cmd="return"
    msg_table.content=_p:export()

    local msg=cjson.encode(msg_table)
    glr.send({host=glr.sys.host,port=glr.sys.port,gpid=0},msg)

    if addr then
        msg_table.cmd=cmd
        local msg=cjson.encode(msg_table)
        glr.send(cjson.decode(addr),msg)   --to supervisord client
    end
    print("run_cmd over")
end

if ...=="__main__" then
    print("supervisord is running...")
    main()
end
