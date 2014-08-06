--[[
--
-- supervisord interface
--
--]]
module(...,package.seeall)

local pprint=require "pprint"
local cjson=require "cjson"
local io=require "io"
local conf=require "supervisor_conf"
local osext=require "osext"

function get_supervisord_arg()
    local sup_conf=conf.watchConf(conf.create())
    local ok,entry=pcall(sup_conf.find_supervisord,sup_conf)
    sup_conf:close()
    assert(ok,entry)
    assert(entry,"no supervisord configure")
    return entry
end

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



local Interface={}
function Interface:new()
    local o={}
    self.__index=self
    setmetatable(o,self)
    self.init(o)
    return o
end

function Interface:init()
    self.conf=get_supervisord_arg()
    self.host=self.conf.host
    self.port=tonumber(self.conf.port)
    self.addr_token=string.format("%s::%s",self.host,self.port) 
    --用pid来确保消息是本进程的。
    self.attr={corrid=osext.getpid()}
end


function Interface:register_and_get_gpid()
    --[[
    @msg:{cmd=register,name="supervisord",result={supervisord=0}} 
    ]]
    function parse_gpid(msg)
        local mt=cjson.decode(msg)
        return assert(tonumber(mt.result.supervisord),"fatal error")
    end

    --recv message from ctr main node and the CLOSED message
    function _cond(msg)
        local msg_type=msg[1]
        local addr=msg[2].addr
        if msg_type==glr.CLOSED then
            return true
        end
        if (addr.host == self.host and
            addr.port == self.port and
            addr.gpid == 0) then
            return true
        end
    end

    local reg={cmd="register",name="supervisord"}
    local addr={host=self.host,port=self.port,gpid=0}
    assert(glr.send(addr,cjson.encode(reg)))
    local msg_type, addr, msg = glr.recv_by_condition(_cond,3)
    if msg_type==nil or msg_type==glr.CLOSED then
        error("supervisord is not running")
    end
    
    self.gpid=parse_gpid(msg)
    self.addr={host=self.host,port=self.port,gpid=self.gpid}
end

--return true if supervisord is started
--@sec: delay @sec seconds.
function Interface:isStarted(sec)
    local sec=tonumber(sec) or 0
    local i=0
    repeat
        local ret,err=glr.connect(self.host,self.port)
        if ret then
            local ok,err=pcall(self.register_and_get_gpid,self)
            if ok then
                return true
            end
        end
        if (sec>i) then
            glr.time.sleep(1)
        end
        i=i+1
    until (i>=sec)
    return nil,"supervisord is not running"
end

local function expected_msg(attr1,attr2)
    return  attr1.msgid==attr2.msgid and attr1.corrid==attr2.corrid
end

--[[
return an array of what we received, throw error if supervisord is not started.
TODO: 增加超时
]]
function Interface:recv_from_supervisord(attr)
    while true do
        local msg_type,maddr,msg=glr.recv()
        if msg_type==nil then
            error("timeout")
        end
        local addr=maddr.addr
        local _attr=maddr.attr
        if msg_type==glr.CLOSED then
            assert(self.addr_token~=addr.host, "supervisord is exited unexpectedly!")
        elseif msg_type==glr.APP then
            print(msg)
            pprint.pprint(attr)
            pprint.pprint(_attr)
            if expected_msg(attr,_attr) then
                return cjson.decode(msg)
            end
        end
    end
end



--send message @content to supervisord
function Interface:send_to_supervisord(content)
    return assert(glr.send(self.addr,content,self.attr))
end

--stop supervisord
function Interface:stop_supervisord()
    if self:isStarted()then
        local addr={host=self.host,port=self.port,gpid=1}

        glr.send(addr,cjson.encode({Type="NODE",Action="EXEC",Cmd="kill"}))

        --XXX: we must recv the CLOSED msg here. Otherwise it will mess the rest
        --command.
        while true do
            local msg_type,addr,msg=glr.recv_by_type(glr.CLOSED)
            addr=addr.addr
            if msg_type==glr.CLOSED and self.addr_token==addr.host then
                break
            end
        end
    end
    return true
end

--start supervisord, return true if started, throw error otherwise.
function Interface:start_supervisord()
    if self:isStarted() then
        return true,"supervisord alreadly started"
    else
        local g=self.conf.gar
        if g and g~="" then
            g=string.format("-g %s",g)
        end
        local cmd=string.format("glr %s -m %s -e main -h %s -p %d %s -D ",
                                g,self.conf.module,self.host,self.port,self.conf.arg or "")
        os.execute(cmd)
        if self:isStarted(3) then
            return true 
        end
        error(string.format("start_monitor error:%s",cmd))
    end
end

--[[
@cmd: command name. start,stop,list,config...
@name: command argument.
]]
function Interface:send_cmd(cmd,name)
    assert(cmd,"command is nil")
    assert(self:isStarted())
    local attr=self:send_to_supervisord(cjson.encode({cmd=cmd,name=name}))
    --XXX:waiting one reply.
    return self:recv_from_supervisord(attr)
end


function Interface:all_cmds(cmd)
    assert(cmd,"cmd can't be nil")
    function docmd(name)
        if name then
            if name=="*all" then
                name=nil
            end
            local ret=self:send_cmd(cmd,name)
            if ret.status==0 then
                return ret.result
            else
                return nil,ret.result
            end

        else
            error("argument can't be nil")
        end
    end
    return docmd
end

function Interface:statusall()
    return self:all_cmds("status")("*all")
end

function Interface:startall()
    return self:all_cmds("start")("*all")
end

function Interface:stopall()
    return self:all_cmds("stop")("*all")
end

function Interface:start(name)
    return self:all_cmds("start")(name)
end

function Interface:stop(name)
    return self:all_cmds("stop")(name)
end
function Interface:status(name)
    return self:all_cmds("status")(name)
end

function Interface:config(name)
    return self:all_cmds("config")(name or "*all")
end

function Interface:list()
    return self:all_cmds("list")("*all")
end

function new()
    return Interface:new()
end
