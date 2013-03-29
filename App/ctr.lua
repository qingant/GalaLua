--[[

Container

--]] 

module(...,package.seeall)

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

-- local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
local configure = require "configure"
local timer = require "timer"
local ffi = require "ffi"
local structs = require "structs"
local string =require  "string"
--print(package.path)
--print(package.cpath)
local RID = "@router"
local MQID = "@mq"

function main()
    -- init
--    local _router=router.new("router")
--    glr.global(RID, _router)
    local _amq=amq.new(configure.amq.path)
    glr.global(MQID, _amq)
--    glr.set_options("hello there,here is lua lsr.lua,nice to meet you! long enough? May be! Let's make it longger! OK")
--    local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")

    local err,router_server = glr.spawn("ctr", "router_info")
    assert(err,router_server)
    local err,amq_server = glr.spawn("ctr", "amq_info")
    assert(err,amq_server)
    local err,node_server = glr.spawn("ctr", "node_info")
    assert(err,node_server)

    local err,configure_server= glr.spawn("configure_server", "main")
    assert(err,configure_server)
    
--    print("-----links---------------------")
--    local a=glr.get_all_links()
--    pprint.pprint(a)
--    print("------links------------------")

    local services={router=router_server,amq=amq_server,node=node_server,configure=configure_server}
    pprint.pprint(services)
    while true do
        local msg_type, addr, msg = glr.recv()
        print("len:::",string.len(msg))
        local msg_table = ffi.new("MONITOR_HEADER")
        ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
        if msg_table.Action==ffi.C.REG then
            if not glr.send(addr,structs.pack(msg_table)..cjson.encode(services)) then
                pprint.pprint("Send Failure")
            end
        else
            print("Warnning:Not a monitor register message::",msg)
        end
--    print("-----links---------------------")
--    local a=glr.get_all_links()
--    pprint.pprint(a)
--    print("------links------------------")


   end
end

function amq_info()
    local _amq=glr.get_global(MQID)
    local _nqview=_amq:NQArray():get(0):view()


    while true do
        msg_type, addr, msg = glr.recv()
        print("len:"..string.len(msg))
        pprint.pprint(msg)

        msg_table = ffi.new("MONITOR_HEADER")
        local len=ffi.sizeof(msg_table)
        ffi.copy(msg_table, msg,len)

        if msg_table.Type==ffi.C.AMQ then
            if msg_table.Action==ffi.C.GET  then
                local head=_nqview:head()
                local total=_nqview:total()
                local tail=_nqview:tail()
                local get_pending=_nqview:lurkers_in_get()
                local put_pending=_nqview:lurkers_in_put()

                local ret={head=head,total=total,tail=tail,get_pending=get_pending,put_pending=put_pending}
                glr.send(addr,cjson.encode(ret)) 
            end
        else
            print("Hey,You should not send messages to me !")
            glr.send(addr,"{'content':'Hey,You should not send messages to me !'}")  
        end
    end
end



function router_info()
    local msg_type, addr, msg
    --local _router=glr.get_global(RID)

    local _router=router.router()
    while true do
        msg_type, addr, msg = glr.recv()
        print("len:"..string.len(msg))
        pprint.pprint(msg)

        msg_table = ffi.new("MONITOR_HEADER")
        local len=ffi.sizeof(msg_table)
        ffi.copy(msg_table, msg,len)
        if msg_table.Type==ffi.C.ROUTER then
         --   local router_arg= ffi.new("ROUTER_ARG")
         --   ffi.copy(router_arg, msg:sub(len+1),ffi.sizeof(router_arg))
            local arg=cjson.decode(msg:sub(len+1))
            if msg_table.Action==ffi.C.GET  then
                local record={}
                if arg.name then
                    record=_router:find_by_name(arg.name) 
                elseif arg.field then
                    record=_router:find_by_field(arg.field) 
                elseif arg.app_type then
                    record=_router:find_by_app_type(arg.app_type) 
                end

                glr.send(addr,cjson.encode({content=record})) 
            elseif msg_table.Action==ffi.C.DEL then
                if arg.name then
                    print ("delete")
                    _router:delete(arg.name)
                end
                --delete ok
                glr.send(addr,"{'del':'true'}")  

            end
        else
            print("Hey,You should not send messages to me !")
            glr.send(addr,"{'content':'Hey,You should not send messages to me !'}")  
        end
    end
    _router:finalizer()
end

function node_info()
    local status_server={}

    local svc_host="0.0.0.0"
    local svc_port=2348

    local lsr_host="0.0.0.0"
    local lsr_port=2349


    function get_status_server()
        function _get_status_server(host,port)
            local msg = ffi.new("BIND_MSG")
            msg.Head.Action = ffi.C.ACT_BIND
            msg.Catagory = ffi.C.DEV_MONITOR
            local addr={host=host,port=port,gpid=0}
            pprint.pprint(addr)
            if not glr.send(addr,structs.pack(msg)) then
                print("send Error ::::::::::::;:",("#"):rep(12))
                return 
            end
            local gpid
--            while true do
            --    print("I am recving>>>>>>>>>>>>>")
                local msg_type, addr, rsp = glr.recv()
             --   print("recv over<<<<<<<<<<<<<<<<<")
                ffi.copy(msg, rsp, ffi.sizeof(msg))

 --               if addr.host==host and addr.port==port then
                    gpid=ffi.C.ntohl(msg.Gpid)
--                    break
--                end
--            end
            return {gpid=gpid,host=host,port=port}
        end
        status_server.lsr=_get_status_server(lsr_host,lsr_port)
        status_server.svc=_get_status_server(svc_host,svc_port) 
    end
    
    function get_gpids(t)
        local msg_table = ffi.new("MONITOR_HEADER")
        local msg_type, addr, msg 

        msg_table.Type=ffi.C.NODE 
        msg_table.Action=ffi.C.GET
        addr=status_server[t]
        pprint.pprint(addr,t)

        if addr  then
            glr.send(addr,structs.pack(msg_table)..cjson.encode({gpid=true}))
            msg_type, addr, msg = glr.recv()
            return cjson.decode(msg)
        end
    end
    
    function get_status(t,gpid_list)
        local gpid_list=gpid_list or {}

        local msg_table = ffi.new("MONITOR_HEADER")
        local msg_type, addr, msg 
        msg_table.Type=ffi.C.NODE 
        msg_table.Action=ffi.C.GET
        addr=status_server[t]

        local m=structs.pack(msg_table)

        m=m..cjson.encode({status=gpid_list})

        if addr  then
            glr.send(addr,m)
            msg_type, addr, msg = glr.recv()
            return cjson.decode(msg)
        end
    end

--[[
    local gpids=get_gpids("lsr")                    
    pprint.pprint(gpids,"gpids")

    local gpids=get_gpids("svc")                    
    pprint.pprint(gpids,"gpids")

    local status=get_status("lsr")                    
    pprint.pprint(status,"status")

    local status=get_status("svc",{1,2})                    
    pprint.pprint(status,"status")
--]]

    while true do
        local msg_type, addr, msg = glr.recv()
        print("len:"..string.len(msg))
        pprint.pprint(msg)

        msg_table = ffi.new("MONITOR_HEADER")
        local len=ffi.sizeof(msg_table)
        ffi.copy(msg_table, msg,len)

        arg=cjson.decode(msg:sub(len+1))
        pprint.pprint(arg)

        if msg_table.Type==ffi.C.NODE and msg_table.Action==ffi.C.GET then
            glr.connect(lsr_host, lsr_port)
            glr.connect(svc_host, svc_port)

            if not (status_server.lsr and status_server.svc) then
                get_status_server() 
                pprint.pprint(status_server,"status_server")
            end

            if arg["gpid"] then
                local gpids={}
                local l=arg["gpid"]
                if #l==0 then
                    l={"lsr","svc"}
                end

                for i,v in pairs(l) do
                    gpids[v]=get_gpids(v)
                end
                glr.send(addr,cjson.encode(gpids))
            elseif arg["status"] then
                local status={}

                local l=arg["status"]
                if #l==0 then
                    l={"lsr","svc"}
                end

                for i,v in pairs(l) do
                    status[v]=get_status(v,arg[v])
                end
                glr.send(addr,cjson.encode(status))
            end
        else
            print("Hey,You should not send messages to me !")
            glr.send(addr,"{'content':'Hey,You should not send messages to me !'}")  
        end
    end
end

