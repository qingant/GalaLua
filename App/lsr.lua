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
   local dev_map = {}
   while true do
      local id
      local msg_type, addr, msg = glr.recv()
      local services={status=status_server}
      print(msg)
      if msg_type == glr.CLOSED then
         pprint.pprint(dev_map)
         pprint.pprint(addr)
         glr.kill(dev_map[addr.host])
         dev_map[addr.host] = nil
         local _,all = glr.all()
         pprint.pprint(all, "GProcesses")
         pprint.pprint(dev_map, "map")
      else
         
         --msg={"host":"192.168.56.101","port":1010,"gpid",122,"type":"agent"}
         assert(msg_type,"glr.recv error")
         -- msg_table=cjson.decode(msg)
         local msg_table = ffi.new("BIND_MSG")
         ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
         
         pprint.pprint("DEV_CATA", msg_table.Catagory)
         if (msg_table.Catagory == ffi.C.DEV_AGENT) then
            -- for i=0,100 do
            err,id=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","agent_worker")
            dev_map[string.format("%s::%d", addr.host, addr.port)] = id

            -- end
         elseif msg_table.Catagory == ffi.C.DEV_MONITOR then
            id = status_server
         elseif msg_table.Catagory == ffi.C.DEV_DISPLAY then
            err,id=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","display_worker")
            dev_map[string.format("%s::%d", addr.host, addr.port)] = id
         end
         -- services["bind_gpid"]=
         msg_table.Gpid = ffi.C.htonl(id)
         pprint.pprint(addr)
         pprint.pprint(addr.host,addr.port,addr.gpid)
         if not node.send(addr.host,addr.port,addr.gpid, structs.pack(msg_table)) then
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
   local msg_type, addr, msg=glr.recv()
   local msg_table = ffi.new("ROUTER_ADD_MSG")
   ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
   
   function register()
      if msg_table.Head.Action==ffi.C.ACT_ROUTER_ADD then
         pprint.pprint("Rigester")
         local host,port=glr.node_addr()
         _router:register(structs.str_pack(msg_table.Name),
                          "display",
                          host,
                          port,
                          __id__,
                          "display",
                          structs.str_pack(msg_table.AppType),
                          "This is userdata")
      else
         error("register message expected!")
      end

   end
   register()
   --print(pprint.pprint(_router:find_by_field("display")))
   local ret={status=true}
   msg_table.Head.Action = ffi.C.ACT_ACK
   node.send(addr.host, addr.port, addr.gpid ,structs.pack(msg_table))

   local nq = _amq:NQArray():get(0)
   --   nq:put()
   local app_msg = ffi.new("APP_HEADER")
   while true do
      local msg_type, _ ,msg = glr.recv()
      print("2display", #msg, msg)
      -- msg = nil
      if msg  then
         ffi.copy(app_msg, msg, ffi.sizeof(app_msg))
      end
      -- local t=cjson.decode(msg)

      -- t.header.timestamp = timer.time() -- add time stamp
      --pprint.pprint("------Display Recved")
      --pprint.pprint(t)
      print(app_msg.From.Catagory, "Send")
      if  app_msg.From.Catagory == ffi.C.DEV_SVC then
         print("Send 2 Display")
         node.send(addr.host, addr.port, addr.gpid, msg)
      elseif app_msg.From.Catagory == ffi.C.DEV_DISPLAY then
         if app_msg.Head.Action == ffi.C.ACT_REQUEST then
            nq:put(msg)
         elseif app_msg.Head.Action == ffi.C.ACT_ROUTER_QUERY then
            -- print("::::router::",pprint.pprint(t)) 
            local content={items = _router:find_by_field("agent")}
            pprint.pprint(content, "Return router info")
            app_msg.Head.Action = ffi.C.ACT_RESPONSE
            
            node.send(addr.host, addr.port, addr.gpid, structs.pack(app_msg) .. cjson.encode(content))
         end
      end
   end
end 

function agent_worker()
   local _router = glr.get_global(RID)
   local _amq = glr.get_global(MQID)
   local msg_type, addr, msg=glr.recv()
   -- local msg_table=cjson.decode(msg)
   
   local msg_table = ffi.new("ROUTER_ADD_MSG")
   ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
   function register()
      if msg_table.Head.Action == ffi.C.ACT_ROUTER_ADD then
         local host,port=glr.node_addr()
         _router:register(ffi.string(msg_table.Name, ffi.C.strlen(msg_table.Name)),
                          "agent",
                          host,
                          port,
                          __id__,
                          "agent",
                          ffi.string(msg_table.AppType, ffi.C.strlen(msg_table.AppType)), 
                          "This is userdata")
      else
         error("register message expected!")
      end

   end
   register()
   
   msg_table.Head.Action = ffi.C.ACT_ACK
   node.send(addr.host,addr.port,addr.gpid, ffi.string(msg_table, ffi.sizeof(msg_table)))
    
   -- pprint.print("Read source file")
   local source = io.open(configure.LUA_AGENT_APP_DIR .. structs.str_pack(msg_table.AppType) .. ".lua"):read("*a")
   -- pprint.print(source)
   local send_msg = ffi.new("APP_HEADER")
   send_msg.Head.Action = ffi.C.ACT_DEPLOY
   send_msg.From.Catagory = ffi.C.DEV_LSR
   -- send_msg.From.DevId = 0
   
   local content =  cjson.encode({
                                    ["content"] = {
                                       ["code"] = source,
                                    }
                                 })
   pprint.pprint("SendMsg\n")
   node.send(addr.host,
             addr.port,
             addr.gpid, 
             structs.pack(send_msg) .. content
            )
   -- tell svc that an agent have registered
   local nq = _amq:NQArray():get(0)
   -- nq:put(cjson.encode(msg_table))

   local app_msg = ffi.new("APP_HEADER")
   while true do
      local msg_type, _ ,msg = glr.recv()
      -- local t=cjson.decode(msg)
      ffi.copy(app_msg, msg, ffi.sizeof(app_msg))
      print(msg, app_msg.From.Catagory)
            
      -- t.header.timestamp = timer.time() -- add time stamp
      if  app_msg.From.Catagory == ffi.C.DEV_SVC then
         node.send(addr.host, addr.port, addr.gpid, msg)
      elseif app_msg.From.Catagory == ffi.C.DEV_AGENT then
         if app_msg.Head.Action == ffi.C.ACT_REPORT then
            print("SendMsg", #msg, msg)
            nq:put(msg)
         elseif app_msg.Head.Action == ffi.C.ACT_RESPONSE then
            local to_addr = app_msg.To.Addr
            print(structs.str_pack(to_addr.Host), ffi.C.ntohl(to_addr.Port), ffi.C.ntohl(to_addr.Gpid))
            node.send(structs.str_pack(to_addr.Host), ffi.C.ntohl(to_addr.Port), ffi.C.ntohl(to_addr.Gpid), msg)
         end
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
