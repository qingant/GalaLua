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
   local r=router.new("router")
   glr.global(RID, r, "Router4Lua")
   
   local m = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
   glr.global(MQID, m, "CGalaxyMQ");
   -- glr.spawn("lsr.lua","worker")
   -- local err, all = glr.all()
   -- print(pprint.pprint(all))
   -- local err, sta = glr.status(glr.id)
   -- print(pprint.pprint(sta))
   -- local err, sta = glr.status(all[1])
   -- print(pprint.pprint(sta))
    local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")
    print("Status " .. status_server)
    while true do
        local id
        local status, msg = glr.recv()
        local services={status=status_server}
        print("MSG")
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
   local r = glr.get_global(RID)
   local mq = glr.get_global(MQID)
   local err,msg=glr.recv()
   local msg_table=cjson.decode(msg)
    
   function register()
      if msg_table["command"]=="register" then
         r:register(msg_table["name"],msg_table["field"],"0.0.0.0",1000,__id__)
      else
         error("register message expected!")
      end

   end
   register()
    
   local ret={status=true}
   node.send(msg_table["host"],msg_table["port"],msg_table["gpid"],cjson.encode(ret))
    
    -- msg_table["command"]=nil
    -- msg_table["registered"]=true
    -- amq.put(cjson.encode(msg_table))
    -- local id 
    -- err,id=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","svc2agent")
    -- msg_table["registered"]=nil
    -- glr.send("127.0.0.1",1000,id,cjson.encode(msg_table))
    
    --agent send to svc
    while true do
        err,msg=glr.recv()
        -- amq.put(msg)
    end
end

-- function svc2agent()
--     local err,msg
--     err,msg=glr.recv()
--     local msg_table=cjson.decode(msg)
--     local msg=amq.get()
--     glr.send(msg_table["host"],msg_table["port"],msg_table["gpid"],msg)
-- end


