--[[
--
--]]
module(...,package.seeall)


local os=require "os"
local pprint=require "pprint"
local cjson=require "cjson"
local conf=require "supervisor_conf"

--process state
STATE={
    STOPPED=1,
    RUNNING=2,
    STOPPING=3,
    STARTING=4,
}

MSG_TYPE={
    APP=3,
}

local logdir=os.getenv("HOME").."/log"
local DefaultGroup="wg1"

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
    
    --FIXME:can not work right now
    function Process:stop()
        local state=self:get_state()
        if (state~=STATE.STOPPING) and (state ~=STATE.STOPPED) then
            self.entry.state=STATE.STOPPING
            --TODO:how to get child pid(send self_pid when it start??)
            local stopcmd=self.entry.stopcmd or string.format("kill -9 %d",self.entry.status.pid)
            execute(stopcmd)
            self:wait_till_finished()
        end
    end
    
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

    function Process:start()
        --can only start those not started
        local state=self:get_state()
        if (state~=STATE.RUNNING) and (state~=STATE.STARTING) then
            self.entry.state=STATE.STARTING
            startcmd=string.format("glr -h %s -p %d -i %d -m %s -e main -D",
                                    self.entry.host,self.entry.port,self.entry.index, self.entry.module)
            print(startcmd)
            local ret=execute(startcmd)
            print("execute:",ret)
            if ret==0 then    --XXX:useless?
                print("execute success")
            end
            --wait till it have started
            self:wait_till_finished()
        end
--        self:get_info()
    end

    --XXX:should call glr.recv after this function to get the information
    function Process:get_info()
        local state=self:get_state()
        if (state==STATE.RUNNING) then
            glr.get_info(self.entry.host,self.entry.port)
        end
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
    function get_config_by_id(id)
        return self._conf:get_config_by_id(id)[1]
    end

    --[[
    -- 
    -- {
    --    [1]={...},
    --    [2]={...},
    --    ...
    -- }
    --]]
    function get_config(group,name)
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
        return self.process[e.group][token]
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

    --TODO: do not work right now
    function Supervisor:save_status(addr,status)
        for i,e in pairs(self.processes) do
            for _,process in pairs(e) do
                pprint.pprint(process,"aaaaaa")
                if process:get_state()==STATE.RUNNING then
                    if process.entry.host==addr.host and process.entry.port==addr.port then
                         process.status=status
                         return true
                    end
                end
            end
        end
    end

    return Supervisor
end

function main()
    node=supervisor()
    local cmds={start="",stop="",status="",config=""}
    while true do
        local msg_type, addr, msg = glr.recv()

        if msg_type == glr.CLOSED then
            local conf_entry=node:get_config_by_id(addr.host)
            pprint.pprint(conf_entry,"RESTART")
            if conf_entry then
                local err,id=glr.spawn("watch","run_cmd",cjson.encode(conf_entry),"start")
            end
        elseif MSG_TYPE.APP==msg_type then
            local msg_table=cjson.decode(msg)
            local cmd=msg_table.cmd
            if cmd=="return" then   --message from run_cmd function
                pprint.pprint(msg_table)
                node:attach(msg_table.content)
            elseif cmds[cmd] then   --message from client
                print(("*"):rep(20),msg,("*"):rep(20))

                local conf_entry=node:get_config(msg_table.group or DefaultGroup,msg_table.name)
                for i,e in ipairs(conf_entry) do   --conf_entry may have more than one item
                    local err,id=glr.spawn("watch","run_cmd",cjson.encode(e),msg_table.cmd)
                end

            end
        else
            --TODO:reply something for success or error ????
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
function run_cmd(conf,cmd)
    local conf_entry=cjson.decode(conf) 
    local _p=process(conf_entry)
    _p[cmd](_p)

    local msg_table={}
    msg_table.cmd="return"
    msg_table.content=conf_entry

    glr.send({host=glr.sys.host,port=glr.sys.port,gpid=0},cjson.encode(msg_table))
    print("run_cmd over")
end

if ...=="__main__" then
    main()
end
