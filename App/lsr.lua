local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
local configure = require "configure"
--print(package.path)
--print(package.cpath)
local RID = "@router"
local MQID = "@mq"
function main()
   -- init
    local _router=router.new("router")
    glr.global(RID, _router)
    local _amq=amq.new(configure.amq.path)
    glr.global(MQID, _amq)
    local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")
    print("------Status :::::" .. status_server)
    -- local err,svc=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","fake_svc")
    -- print("------svc ::::::" .. svc)
    while true do
        local id
        local msg_type, gpid, msg = glr.recv()
        local services={status=status_server}
        print(msg)
        if msg_type == glr.CLOSED then
           
        else
        --msg={"host":"192.168.56.101","port":1010,"gpid",122,"type":"agent"}
        assert(msg_type,"glr.recv error")
        msg_table=cjson.decode(msg)
        pprint.pprint(msg_table)
        if (msg_table["dev_type"]=="agent") then
           -- for i=0,100 do
           err,id=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","agent_worker")
           -- end
        elseif msg_table["dev_type"] == "monitor" then
           id = status_server
        elseif msg_table.dev_type == "display" then
           err,id=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","display_worker")
        end
        services["bind_gpid"]=id
        pprint.pprint(msg_table)
        if not node.send(msg_table["host"],msg_table["port"],msg_table["src_gpid"],cjson.encode(services)) then
           pprint.pprint("Send Failure")
        end
    end
    end
end

--a=cjson.decode("{\"a\":\"vvv\"}")
--print(pprint.pprint(a))
--print(cjson.encode(a))
function display_worker()
   local _router = glr.get_global(RID)
   local _amq = glr.get_global(MQID)
   local err,msg=glr.recv()
   local msg_table=cjson.decode(msg)
    
   function register()
      if msg_table["command"]=="register" then
         pprint.pprint("Rigester")
         local host,port=glr.node_addr()
         _router:register(msg_table["name"],"display",host,port,__id__, "display", msg_table["app_type"],msg_table["host"].."This is userdata")
      else
         error("register message expected!")
      end

   end
   register()
   print(pprint.pprint(_router:find_by_field("display")))
   local ret={status=true}
   node.send(msg_table["host"],msg_table["port"],msg_table["src_gpid"],cjson.encode(ret))

   local nq = _amq:NQArray():get(0)
--   nq:put()

   while true do
      local err,msg=glr.recv()
      local t=cjson.decode(msg)
      pprint.pprint("------Display Recved")
      pprint.pprint(t)
      if t.header.from.type == "svc" then
         node.send(msg_table["host"],msg_table["port"],msg_table["src_gpid"], msg)
      elseif t.header.from.type == "display" then
         if t.header.to.action=="request" then
             nq:put(msg)
         elseif t.header.to.action=="router" then
             print("::::router::",pprint.pprint(t)) 
             t.content=_router[t.header.to.method](_router,t.header.to.args)
             node.send(msg_table["host"],msg_table["port"],msg_table["src_gpid"], cjson.encode(t))
         end
      end
   end
end 

function agent_worker()
   local _router = glr.get_global(RID)
   local _amq = glr.get_global(MQID)
   local msg_type, gpid, msg=glr.recv()
   local msg_table=cjson.decode(msg)
    
   function register()
      if msg_table["command"]=="register" then
         local host,port=glr.node_addr()
         _router:register(msg_table["name"],"agent",host,port,__id__, "agent", msg_table["app_type"],msg_table["host"].."This is userdata")
      else
         error("register message expected!")
      end

   end
   register()
    
   local ret={status=true}
   node.send(msg_table["host"],msg_table["port"],msg_table["gpid"],cjson.encode(ret))
    
   -- pprint.print("Read source file")
   local source = io.open(configure.LUA_AGENT_APP_DIR .. msg_table.app_type .. ".lua"):read("*a")
   -- pprint.print(source)

   local send_msg =  cjson.encode({
                             ["header"] = {
                                ["from"] = {},
                                ["to"] = {
                                   ["action"] = "excute",
                                }
                             },
                             ["content"] = {
                                ["code"] = source,
                             }
                          })
   pprint.pprint("SendMsg\n"..send_msg)
   node.send(msg_table["host"],
             msg_table["port"],
             msg_table["gpid"], 
             send_msg
            )
   -- msg_table["command"]=nil
   -- msg_table["registered"]=true

   local nq = _amq:NQArray():get(0)
   -- nq:put(cjson.encode(msg_table))

    while true do
        msg_type, gpid, msg=glr.recv()
        local t=cjson.decode(msg)
        print("----------agent worker------")
        print(pprint.pprint(t))
        if t["header"]["from"]["type"]=="agent" then 
           if t.header.to.action == "report" then
              nq:put(msg)
           elseif t.header.to.action == "response" then
              node.send(t.header.to.host, t.header.to.port, t.header.to.gpid, msg)
           end
        elseif  t["header"]["from"]["type"]=="svc" then
            node.send(msg_table["host"],msg_table["port"],msg_table["gpid"],msg)
        end
    end
end

-- function fake_svc()
--     local r = glr.get_global(RID)
--     local m = glr.get_global(MQID)
--     local q = m:NQArray():get(0)
--     local id,msg=q:get(q.MQC_BAT)
--     local t=cjson.decode(msg)
--     print("-------------svc--------------------")
--     print(pprint.pprint(t))
--     print("-------------svc--------------------")
--     local item=r:find_by_name(t["name"]) 
--     print("-------------router record-------------------")
--     print(pprint.pprint(item))
--     print("-------------router record-------------------")
--     local addr=item["addr"]
--     print(pprint.pprint(addr))
--     to_agent_worker={header={from="svc"},content={aaa="hello,this is svc"}}
--     while true do
--         glr.send(addr["gpid"],cjson.encode(to_agent_worker))
--         err,msg=q:get(q.MQC_BAT)
--         print("fake_svc::::::",msg)
--     end
-- end
