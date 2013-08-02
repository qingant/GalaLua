--[[
--
-- supervisord interface
--
--]]
module(...,package.seeall)

local pprint=require "pprint"
local cjson=require "cjson"
local io=require "io"

function get_supervisord_arg()
    local Config=(require "config").Config
    local db_path=require "db_path"

    local _conf= Config:new():init(db_path.config)
    local host=_conf:get("CTR/Address/IP")
    local port=tonumber(_conf:get("CTR/Address/Port"))

    --local host=_conf:get("SUP/Address/IP")
    --local port=tonumber(_conf:get("SUP/Address/Port"))
    local gar=_conf:get("SUP/GarName")
    return host,port,gar
end


function sleep(n)
    local n=n or 1

    local cmd=string.format("sleep %d",n)
    os.execute(cmd)
end

--register ctr and get supervisord's gpid
function register_and_get_gpid(addr)
    local reg={Type="CTR",Action="REG"}
    if not glr.send(addr,cjson.encode(reg)) then
        return nil
    end
    print("waiting to register to CTR")
    --only recv message from ctr main node
    local msg_type, addr, msg = glr.recvByAddr(addr)

    return cjson.decode(msg)["supervisord"]
end

--return true if supervisord is started
--@sec: delay @sec seconds
function isStarted(sec)
    local serv_host,serv_port=get_supervisord_arg()
    local sec=tonumber(sec) or 0
    local i=0
    local addr={host=serv_host,port=serv_port,gpid=0}
    pprint.pprint(addr,"isStarted")
    while true do
        local ret=glr.connect(serv_host,serv_port)
        if ret then
            local gpid=register_and_get_gpid(addr)
            if gpid then
                addr.gpid=gpid 
                return addr
            end
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

--return an array of what we received, return nil and error message 
--if supervisord is not started.
function recv_from_supervisord() 
    local addr=isStarted()
    if not addr then
        return nil,"supervisord is not running"
    end

    local addr_token=string.format("%s::%s",addr.host,addr.port) 

    local ret={}
    local first_msg=true
    local rest=0
    --XXX:wait for command reply
    while true do
        local msg_type,addr,msg=glr.recv()    --TODO:timeout for failed

        if msg_type==glr.CLOSED then
            if addr_token==addr.host and (not isStarted()) then
                return nil,"supervisord is exited unexpectedly!"
            end
        else
            print("GETMSG:",msg)
            local t=cjson.decode(msg)

            --first message will return the number of rest messages
            if first_msg then   
                rest=t.rest or 0 
                first_msg=false
            else
                ret[#ret+1]=t
            end

            if rest<=0 then
                break
            end
            rest=rest-1
        end
    end
    return ret
end

--send message @content to supervisord
function send_to_supervisord(content)
    local addr=isStarted()
    if not addr then
        return nil,"supervisord is not running"
    end
    
    pprint.pprint(addr,"SENDTOSUPERVISORD")
    if not glr.send(addr,content) then
        return nil,"send to supervisord failed"
    end
    return true
end

local function all_cmds(cmd)
    assert(cmd,"cmd can't be nil")
    function docmd(name)
        if name then
            if name=="*all" then
                name=nil
            end
            local err,msg=send_to_supervisord(cjson.encode({cmd=cmd,name=name}))
            if not err then   --failed sending message to supervisord 
                return nil,msg
            end
            return recv_from_supervisord()
        else
            return  nil,"argument can't be nil"
        end
    end
    return docmd
end

function statusall()
    return all_cmds("status")("*all")
end

function startall()
    return all_cmds("start")("*all")
end

function stopall()
    return all_cmds("stop")("*all")
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

--start supervisord, return true if started, else return nil and error message.
function start_supervisord()
    if isStarted() then
        return true,"supervisord alreadly started"
    else
        print("starting supervisord............")
        local serv_host,serv_port,DefaultGar=get_supervisord_arg()
        local gar=""
        local run=""
        if DefaultGar and DefaultGar~="" then
            gar="--gar="..DefaultGar
            run="-g "..DefaultGar
        end

        --supervisord is now embeded in CTR.
        --local cmd=string.format("glr -m supervisord -e main -h %s -p %d %s -D ",
        local cmd=string.format("glr %s -m ctr -e main -h %s -p %d %s -D ",
                                run,serv_host,serv_port,gar)
        print(cmd)
        ret=os.execute(cmd)
        if isStarted(5) then
            return true 
        else
            return nil,"start_monitor error"
        end
    end
end

function config(name)
    local err,msg=send_to_supervisord(cjson.encode({cmd="config",name=name}))
    if not err then
        return  nil,msg
    end
    --XXX:waiting one reply.
    return recv_from_supervisord()[1]
end


function list()
    local err,msg=send_to_supervisord(cjson.encode({cmd="list"}))
    if not err then
        return  nil,msg
    end
    --XXX:waiting one reply.
    return recv_from_supervisord()[1]
end
