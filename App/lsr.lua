local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
-- local configure = require "configure"
--print(package.path)
--print(package.cpath)
local RID = "@router"
local MQID = "@mq"
function main()
   -- init
   local _router=router.new("router")
   glr.global(RID, _routerr, "Router4Lua")
   local _amq=amq.new(configure.amq.path)
   glr.global(MQID, _amq, "CGalaxyMQ");
   -- glr.spawn("lsr.lua","worker")
   -- local err, all = glr.all()
   -- print(pprint.pprint(all))
   -- local err, sta = glr.status(glr.id)
   -- print(pprint.pprint(sta))
   -- local err, sta = glr.status(all[1])
   -- print(pprint.pprint(sta))
    local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")
    print("------Status :::::" .. status_server)
    local err,svc=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","fake_svc")
    print("------svc ::::::" .. svc)
    while true do
        local id
        local status, msg = glr.recv()
        local services={status=status_server}
        print(msg)
        --msg={"host":"192.168.56.101","port":1010,"gpid",122,"type":"agent"}
        assert(status,"glr.recv error")
        msg_table=cjson.decode(msg)
        print(pprint.pprint(msg_table))
        if (msg_table["dev_type"]=="agent") then
            err,id=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","agent_worker")
        end
        services["bind_gpid"]=id
        
        node.send(msg_table["host"],msg_table["port"],msg_table["src_gpid"],cjson.encode(services))
    end
end

--a=cjson.decode("{\"a\":\"vvv\"}")
--print(pprint.pprint(a))
--print(cjson.encode(a))

function agent_worker()
   local _router = glr.get_global(RID)
   local _amq = glr.get_global(MQID)
   local err,msg=glr.recv()
   local msg_table=cjson.decode(msg)
    
   function register()
      if msg_table["command"]=="register" then
         local host,port=glr.node_addr()
         _router:register(msg_table["name"],msg_table["field"],host,port,__id__,type?)
      else
         error("register message expected!")
      end

   end
   register()
    
   local ret={status=true}
   node.send(msg_table["host"],msg_table["port"],msg_table["gpid"],cjson.encode(ret))
    
   msg_table["command"]=nil
   msg_table["registered"]=true

   local nq = _amq:NQArray():get(0)
   nq:put(cjson.encode(msg_table))

    while true do
        err,msg=glr.recv()
        local t=cjson.decode(msg)
        print("----------agent worker------")
        print(pprint.pprint(t))
        if t["header"]["from"]=="agent" then 
            nq:put(msg)
        elseif  t["header"]["from"]=="svc" then
            node.send(msg_table["host"],msg_table["port"],msg_table["gpid"],msg)
        end
    end
end

function fake_svc()
    local r = glr.get_global(RID)
    local m = glr.get_global(MQID)
    local q = m:NQArray():get(0)
    local id,msg=q:get(q.MQC_BAT)
    local t=cjson.decode(msg)
    print("-------------svc--------------------")
    print(pprint.pprint(t))
    print("-------------svc--------------------")
    local item=r:find_by_name(t["name"]) 
    print("-------------router record-------------------")
    print(pprint.pprint(item))
    print("-------------router record-------------------")
    local addr=item["addr"]
    print(pprint.pprint(addr))
    to_agent_worker={header={from="svc"},content={aaa="hello,this is svc"}}
    while true do
        glr.send(addr["gpid"],cjson.encode(to_agent_worker))
        err,msg=q:get(q.MQC_BAT)
        print("fake_svc::::::",msg)
    end
end
