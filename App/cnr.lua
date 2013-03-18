--[[

Container

--]]

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
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
    local _router=router.new("router")
    glr.global(RID, _router)
--    local _amq=amq.new(configure.amq.path)
--    glr.global(MQID, _amq)
--    glr.set_options("hello there,here is lua lsr.lua,nice to meet you! long enough? May be! Let's make it longger! OK")
--    local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")
    local err,router_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/cnr.lua", "router_info")
--    local services={status=status_server,router=router_server}
    local services={router=router_server}
    while true do
        local msg_type, addr, msg = glr.recv()
        print("len:::",string.len(msg))
        local msg_table = ffi.new("MONITOR_HEADER")
        ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
        if msg_table.Action==ffi.C.REG then
            if not node.send(addr.host,addr.port,addr.gpid,structs.pack(msg_table)..cjson.encode(services)) then
                pprint.pprint("Send Failure")
            end
        else
            print("Warnning:Not a monitor register message::",msg)
        end
   end
end

function router_info()
    local msg_type, addr, msg
    local _router=glr.get_global(RID)
    while true do
        msg_type, addr, msg = glr.recv()
        print("len:"..string.len(msg))
        pprint.pprint(msg)
--        node.send(addr.host,addr.port,addr.gpid,"This is fuck you ") 

        msg_table = ffi.new("MONITOR_HEADER")
        local len=ffi.sizeof(msg_table)
        ffi.copy(msg_table, msg,len)
        print("I am here 65")
        if msg_table.Type==ffi.C.ROUTER then
            local router_arg= ffi.new("ROUTER_ARG")
            ffi.copy(router_arg, msg:sub(len+1),ffi.sizeof(router_arg))
            local name=ffi.string(router_arg.Name,ffi.sizeof(router_arg.Name))
            local field=ffi.string(router_arg.Field,ffi.sizeof(router_arg.Field))
            local app_type=ffi.string(router_arg.AppType,ffi.sizeof(router_arg.AppType))
            if msg_table.Action==ffi.C.GET  then
                local ret,i
                print(name,field,app_type)
                local record=_router:find(name,field,app_type) 
                ret={content=record}
                node.send(addr.host,addr.port,addr.gpid,cjson.encode(ret)) 
            elseif msg_table.Action==ffi.C.DEL then
                if name~=('\000'):rep(ffi.sizeof(router_arg.Name)) then
                    print ("delete")
                    _router:delete(name)
                end
                --delete ok
                node.send(addr.host,addr.port,addr.gpid,"{'del':'true'}")  
            end
        else
            print("Hey,You should not send messages to me !")
            node.send(addr.host,addr.port,addr.gpid,"{'content':'Hey,You should not send messages to me !'}")  
        end
    end
end

