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

--[[
process state

STOPPED : process has been stopped due to a stop request or has never been started.
STARTING: process is starting due to a start request.
RUNNING : process is running.
BACKOFF : process entered the STARTING state but subsequently 
          exited too quickly to move to the RUNNING state.
STOPPING: process is stopping due to a stop request.
EXITED  : process exited from the RUNNING state (expectedly or unexpectedly).
FATAL   : process could not be started successfully.
UNKNOWN : process is in an unknown state (supervisord programming error).

When an autorestarting process is in the BACKOFF state, it will be automatically 
restarted by supervisord. It will switch between STARTING and BACKOFF states 
until it becomes evident that it cannot be started because the number of 
`startretries` has exceeded the maximum, at which point it will transition to 
the FATAL state. Each start retry will take progressively more time.
]]
local STATE={
    STOPPED=1,
    RUNNING=2,
    STOPPING=3,
    STARTING=4,
    FATAL=5,
    BACKOFF=6,
    EXITED=7,
    UNKNOWN=8,
}

local STATE_NAME={
    "STOPPED",
    "RUNNING",
    "STOPPING",
    "STARTING",
    "FATAL",
    "BACKOFF",
    "EXITED",
    "UNKNOWN",
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
    -- return: a table with all found configure entries,
    --        empty table if not found. Default is just 
    --        valid config entry.
    function Configure:get_config(group,name)
        assert(group,"group must be passed!")
        local module_name,index
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
        
        return filter(conf_entries)
    end

    --[[
    -- set default value if the value is invalid of every valid configure, and
    -- return it. This function can ensure all usable key is not nil.
    --]]
    function filter(conf_entries)
        local ret={}
        for i,v in pairs(conf_entries) do
            if v.valid then
                v.index=tonumber(v.index)or -1

                v.group=v.group or DefaultGroup
                v.module=v.module or ""

                v.host=v.host or ""
                v.port=tonumber(v.port) or -1 
                ret[#ret+1]=v
            end
        end
        return ret
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

    self.entry=self.entry or {}
    self.entry.host=tostring(self.entry.host)
    self.entry.port=tonumber(self.entry.port) or -1 
end

--[[
-- @entry:must has key below
--        {
--          host="",
--          port="",
--          index="",
--          module="",
--        }
--]]
function process(entry,max)
    local Process=p:new{entry=entry}
    local MAX_TIMES=max or 3
    local pid
    local startcmd

    --maybe run module from gar package
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

    function Process:get_startcmd()
        local startcmd=string.format("glr %s -h %s -p %d -i %d -m %s -e main -D",
                                     self:get_run_gar_arg(),self.entry.host,self.entry.port,
                                     self.entry.index, self.entry.module)
        return startcmd
    end

    startcmd=Process:get_startcmd()
    --[[
    --  stop process.
    --
    --  Two methods to stop a process:
    --     1.send "kill" command to it
    --     2.kill -9 directly.
    --
    --  first use method 1, if failed then take method 2
    --]]
    function Process:stop()
        local state=self:get_state()

        --FATA means error occurs, it's not started, 
        --so update_state as STOPPED directly.
        if (state==STATE.FATAL) or (state==STATE.STOPPED) then
            self:update_state(STATE.STOPPED)
        elseif (state~=STATE.STOPPING) then  --FIXME: whatif state==STATE.EXITED???
            self:update_state(STATE.STOPPING)
            local addr={host=self.entry.host,port=self.entry.port,gpid=1}

            local ret=glr.send(addr,cjson.encode({Type="NODE",Action="EXEC",Cmd="kill"})) 
            if ret then  --send "kill" command success
                --FIXME: some time delay before get_state?
                sleep(1)
                state=self:get_state()
            end
            if (state~=STATE.STOPPED) then
                -- just kill -9 it
                pprint.pprint(self.entry,"stop")
                local stopcmd=string.format("kill -9 %d",pid)
                execute(stopcmd)
            end
            self:update_state(STATE.STOPPED)
        end
    end
    
    --[[
      get_state: get the process's current state.

       1.STARTING-->glr.connect--success-->RUNNING
                      |failed
              (retry < max_retries)--no-->FATAL
                      |yes
                    BACKOFF
       2.STOPPING-->glr.connect--success-->STOPPING
                      |failed
                    STOPPED
       3.nil------->glr.connect--success-->RUNNING
                      |failed
                    STOPPED
       4.STOPPED--->glr.connect--(should never success)-->UNKNOWN
                      |failed
                    STOPPED
       5.RUNNING--->glr.connect--success-->RUNNING
                      |(only fail when process exited accidently)
                    EXITED
       6.FATAL--->glr.connect--(should never success)-->UNKNOWN
                      |failed
                    FATAL
    ]]
    function Process:get_state()
        --original state
        local state=self.entry.state
        --if (state==STATE.RUNNING) or (state==STATE.STOPPED) then
        --    return state
        --end
        if (state==STATE.STOPPED) or (state==STATE.FATAL) or 
            (state==STATE.BACKOFF) then

           return state
        else 
            local ret=glr.connect(self.entry.host,self.entry.port)
            if ret then
                if (not state) or (state==STATE.STARTING) or (state==STATE.RUNNING) then
                    state=STATE.RUNNING
                end
            else
                if (not state) or (state==STATE.STOPPING) then
                    state=STATE.STOPPED
                elseif (state==STATE.RUNNING) then
                    state=STATE.EXITED
                end
            end
        end
        return state
    end
    
    --update state
    function Process:update_state(state)
        local state=state or self:get_state()
        self.entry.state=state
    end
    
    --[[
        restart process when it exited accidentally
    ]]
    function Process:restart_if_exited()
        local state=self:get_state()
        if state==STATE.EXITED then
            self:start()
        end
    end


    --[[
    --@timeout: default is 10 seconds
    
      wait for starting process, if success then update state as RUNNING and return.
      Otherwise when timeout then update the process state as BACKOFF.

      After calling this function, process state will be one of `RUNNING,BACKOFF`. 
    --]]
    function Process:wait_till_started(timeout)
        local timeout=timeout or 10 
        while true do
            --origin state is STARTING so the result of get_state 
            --will be either RUNNING or BACKOFF
            local state=self:get_state()
            if state==STATE.RUNNING then 
                self:update_state(STATE.RUNNING)
                return 
            else
                if 0<timeout then
                    sleep(1)
                else
                    break
                end
            end
            timeout=timeout-1
        end
        self:update_state(STATE.BACKOFF)
    end


    --[[
        Do actual work to start process, after calling this function,
        process state will be one of `RUNNING,BACKOFF`.
    ]]
    function Process:_start()
        --can only start those not started
        local state=self:get_state()
        if (state~=STATE.RUNNING) and (state~=STATE.STARTING) then
            self:update_state(STATE.STARTING)

            print(startcmd)
            local ret=execute(startcmd)
            print("execute:",ret)
--            if ret==0 then    --XXX:useless, because "glr" always return 0 when run with "-D".
--                print("execute success")
--            end

            --wait till it started or 3 seconds
            self:wait_till_started(3)
        end
    end
    
    --[[
    -- start process, if failed then retry MAX_TIMES to start it. 
    -- After calling this function, process state will be one of `RUNNING,FATAL`. 
    --]]
    function Process:start()
        local start_retry=0
        repeat
            self:_start()

            --if `_start success then origin state is RUNNING, otherwise BACKOFF, 
            --get_state will only return either BACKOFF or RUNNING here.
            local state=self:get_state()
            if state==STATE.RUNNING then
                self:getpid()
                return self:update_state(STATE.RUNNING)
            elseif state==STATE.BACKOFF then
                start_retry=start_retry+1
            else
                --XXX:should never get here
                return self:update_state(STATE.UNKNOWN)
            end
        until start_retry>MAX_TIMES

        return self:update_state(STATE.FATAL)
    end


    --[[
    -- get information from glr process's "spyer server(gpid: 1)"
    --
    --]]
    function Process:get_info_from_inspector(cmd)
        print("get_info_from_inspector........")
        local addr={host=glr.sys.host,port=glr.sys.port,gpid=__id__}

        local msg={Type="NODE",Action="GET",Cmd=cmd,ToAddr=addr,Nonstop=true}
        pprint.pprint(msg,"MSG_TO")
        if not glr.send({host=self.entry.host,port=self.entry.port,gpid=1},cjson.encode(msg)) then
            --must not into here
            print("send to inspector error...")
            self:update_state(STATE.UNKNOWN)
            return {}
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
            pid=self:get_info_from_inspector("pid").pid
            return pid
        end
        print("get pid:",pid)
    end
    
    --FIXME: what should return when the process is stopped
    function Process:status()
        local ret={}
        local state=self:get_state()
        if state==STATE.RUNNING then
            ret.pid=self:getpid()
            ret.status=self:get_info_from_inspector("status")

            --XXX:update state?? 
            --self:update_state(STATE.RUNNING)
        end
        ret.state=state
        return ret
    end

    --XXX:export self.entry, with it can rebuild Exactly the same process object.
    --    in order to send a process_object to other glr node, we must 
    --    use "cjson.encode" function to encode a process object to json string.
    --    cjson.encode(table1):there is no meaning if some of table1's value are functions.
    function Process:export()
        return self.entry
    end
    
    --TODO:better information for show
--    function Process:export_for_show()
--        local result={}
--        result.state=STATE_NAME[self:get_state()]
--        result.pid=self:getpid()
--        result.name=string.format("%s%.4d",self.entry.module,self.entry.index)
--        result.group=self.entry.group
--
--        return result
--    end

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

--[[
-- sorts of functions to maniplate process object.  
--]]
function ProcessTable()
    local self={}
    local self_id={}

    local _process={}
    function _process.get_by_id(id)
        return self_id[id]
    end

    function _process.get(group,token)
        if self[group] then
            return self[group][token]
        end
    end
    
    --save it if not existed!
    function _process.save_new(conf)
        local token=string.format("%s%.4d",conf.module,conf.index)
        local _p=_process.get(conf.group,token)

        if not _p then
            _p=_process.save(conf)
        end
        return _p
    end
    
    --save process, if existed then replace it.
    function _process.save(conf)
        self[conf.group]=self[conf.group] or {}
        local token=string.format("%s%.4d",conf.module,(tonumber(conf.index) or 0))
        local _p=process(conf)
        self[conf.group][token]=_p
        local id=string.format("%s::%s",conf.host,(tonumber(conf.port) or -1))
        self_id[id]=_p
        return _p
    end
    
    --get all process 
    function _process.getall()
        local ret={}
        for k,v in pairs(self_id) do
            ret[#ret+1]=v:export()
        end
        return ret 
    end

    return _process
end

function supervisor()
    local Supervisor=supervisor_base:new()

    local _process=ProcessTable() 


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

    --[[ 
    -- group:which group
    -- name:must match pattern (module_name..index)
    --      lsr0 ===> lsr:module_name 0:index
    --      lsr  ===> lsr:module_name match all indexs
    -- return: a process table matching group and name,
    --         empty table if nothing found
    --
    ]]
    function Supervisor:get_processes_from_config(group,name)
        local group=group or DefaultGroup
        return self:get_processes_by_entries(self._conf:get_config(group,name))
    end
    
    --[[
    -- return the process index by @id, nil otherwise. 
    --]]
    function Supervisor:get_processes_by_id(id)
        if id then
            return _process.get_by_id(id)
        end
    end

    -- return processes specified by @procs in `_process, create 
    -- a new process object if not existed. Empty table will be return 
    -- if no valid entry in @procs.
    function Supervisor:get_processes_by_entries(procs)
        local ret={} 
        for i,e in pairs(procs) do
            ret[#ret+1]=_process.save_new(e)
        end
        return ret
    end
    
    --[[
    -- return the founded process table, empty table if not found or error
    -- arguments. 
    --]]
    function Supervisor:get_processes(procs,group)
        local _t=type(procs)
        local ret={}
        if _t=="table" then
            ret=self:get_processes_by_entries(procs)
        elseif (_t=="string" or _t=="nil") then
            ret=self:get_processes_from_config(group,procs)
        else
            print("error argument to get_processes:",procs)
        end
        return ret
    end

    -- return a new process object specified by @proc and save it.
    function Supervisor:create_process(proc)
        return _process.save(proc)
    end

    --[[
    -- make supervisor monitoring process @proc
    -- @proc: return by process:export()
    --]]
    function Supervisor:attach(proc)
        local _p=self:create_process(proc)
        _p:update_state()
    end

    function Supervisor:get_all_process()
        return _process.getall()
    end

    return Supervisor
end



function cmds(sup)
    local sup=assert(sup,"must pass valid supervisor")

    local Cmds={}
    
    function unknown(msg_table,toaddr)
        rest_message_number(msg_table.cmd,1,toaddr)
        local msg=cjson.encode(msg_table)
        print("unknown command:",msg_table.cmd)
        glr.send(toaddr,cjson.encode({ret="unkown cmd",msg=msg}))
    end

    local mt={__index=function () return unkown end}
    setmetatable(Cmds,mt)
    
    
    --reply the number of rest messages when calling cmd name @cmd_name
    function rest_message_number(cmd_name,num,toaddr)
        local ret={cmd=cmd_name}
        ret.rest=num     -- number of rest messages
        glr.send(toaddr,cjson.encode(ret))
    end

    function cmd_skel(func,cmd_name)
        local func=func
        local cmd_name=cmd_name

        function skel(msg_table,toaddr)
            local procs=sup:get_processes(msg_table.name)
            rest_message_number(cmd_name,#procs,toaddr)
            func(procs,toaddr)
        end

        return skel
    end

    function stop(procs,toaddr)
        local ret={cmd="stop"}
        for i,_p in ipairs(procs) do   --procs may have more than one item
            _p:stop()
            local _proc=_p:export()
            ret.result={}
            ret.result.state=STATE_NAME[_proc.state]
            ret.result.name=string.format("%s%.4d",_proc.module,_proc.index)
            ret.result.group=_proc.group

            glr.send(toaddr,cjson.encode(ret))
        end
    end
    Cmds.stop=cmd_skel(stop,"stop")

    function start(procs,toaddr)
        for i,e in ipairs(procs) do   --procs may have more than one item
            local err,id=glr.spawn("supervisord","run_ctrl_cmd",cjson.encode(e:export()),
                                   "start",cjson.encode(toaddr))
        end
    end
    Cmds.start=cmd_skel(start,"start")

    function status(procs,toaddr)
        for i,e in ipairs(procs) do   --procs may have more than one item
            local err,id=glr.spawn("supervisord","run_info_cmd",cjson.encode(e:export()),
                                   "status",cjson.encode(toaddr))
        end

    end
    Cmds.status=cmd_skel(status,"status")

    function Cmds.list(msg_table,toaddr)
        rest_message_number("list",1,toaddr)
        msg_table.result=sup:get_all_process()
        glr.send(toaddr,cjson.encode(msg_table))
    end

    --update process 
    function Cmds.update(msg_table)
        sup:attach(msg_table.content)
    end

    return Cmds
end

--[[
--  run control command @cmd, then send "update" message to 
--  main process and send success or failed message to the client @addr.
--
--  "update" message format:
--  {
--      cmd="update",
--      content={
--                  host=...,
--                  port=...,
--                  group=...,
--                  state=...,
--                  ...
--              }
--  }
--
--  client message format:
--  {
--      cmd="",
--      result={
--         state=statename,
--         group=group,
--         name=process name,
--      }
--  }
--
--]]
function run_ctrl_cmd(proc,cmd,addr)
    local proc=cjson.decode(proc) 
    local _p=process(proc)

    _p[cmd](_p)

    local _proc=_p:export()
    local msg_table={}

    msg_table.cmd="update"
    msg_table.content=_proc

    local msg=cjson.encode(msg_table)
    glr.send({host=glr.sys.host,port=glr.sys.port,gpid=0},msg)

    if addr then
        local ret={}
        ret.cmd=cmd
        ret.result={}

        ret.result.state=STATE_NAME[_proc.state]
        ret.result.name=string.format("%s%.4d",_proc.module,_proc.index)
        ret.result.group=_proc.group

        local msg=cjson.encode(ret)
        glr.send(cjson.decode(addr),msg)   --to supervisord client
    end
    print("run_ctrl_cmd over")
end

--[[
--  run get information command @cmd, then send what @cmd function return 
--  to the client @addr.
--
--  --  client message format:
--  {
--      cmd="",
--      result={
--          state="",  STATE_NAME
--          name="",
--          group="",
--          nodes={...}, glr nodes
--          pid=,
--      }
--  }
--
]]
function run_info_cmd(proc,cmd,addr)
    local addr=assert(addr,"must pass addr")
    local proc=cjson.decode(proc) 
    local _p=process(proc)

    local ret=_p[cmd](_p)
    local msg_table={cmd=cmd}
    local _proc=_p:export()

    msg_table.result={}

    msg_table.result.state=STATE_NAME[ret.state]
    msg_table.result.pid=ret.pid
    msg_table.result.name=string.format("%s%.4d",_proc.module,_proc.index)
    msg_table.result.group=_proc.group
    msg_table.result.nodes=ret.status

    local msg=cjson.encode(msg_table)
    glr.send(cjson.decode(addr),msg)   --to supervisord client

    print("run_info_cmd over")
end

function main()
    local node=supervisor()
    local command=cmds(node)
    while true do
        local msg_type, addr, msg = glr.recv()

        if msg_type == glr.CLOSED then
            local proc=node:get_processes_by_id(addr.host)
            if proc then
                pprint.pprint(proc,"RESTART")
                local err,id=glr.spawn("supervisord","run_ctrl_cmd",
                                       cjson.encode(proc:export()),"restart_if_exited")
            end
        elseif MSG_TYPE.APP==msg_type then
            local msg_table=cjson.decode(msg)
            command[msg_table.cmd](msg_table,addr)
        else
            glr.send(addr,cjson.encode({ret="unkown message",msg=msg}))
        end
    end
end


if ...=="__main__" then
    print("supervisord is running...")
    main()
end
