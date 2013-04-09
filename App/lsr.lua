module(..., package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]

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
--print(package.path)
--print(package.cpath)
--local RID = "@router"
local MQID = "@mq"
-- local glr = require "glr"


function main()
   -- init
   pprint.pprint(glr)
--   local _router=router.new("router")
--   glr.global(RID, _router)
   local _router=router.router()
   local _amq = amq.new(configure.amq.path)
   _router:delete_stale("lsr0", glr.glr_stamp())
   print("global")
   glr.global(MQID, _amq)
   print("Init")
   print(glr.sys.host)
   local err,status_server = glr.spawn("status", "main")
   print("------Status :::::" .. status_server)
   -- local err,svc=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","fake_svc")
   -- print("------svc ::::::" .. svc)
   local myhost, myport = glr.sys.host, glr.sys.port
   local dev_map = {}
   while true do
      local id
      local msg_type, addr, msg = glr.recv()
      local services={status=status_server}
      print(msg)
      if msg_type == glr.CLOSED then
         pprint.pprint(dev_map)
         pprint.pprint(addr)
         function delete_junk_items()
            local items = _router:find_by_cata("agent")
            for _,it in pairs(items) do
             
               if it.host == myhost and it.port == myport and it.gpid == dev_map[addr.host] then
                  _router:delete(it.name)
               end
            end
         end
         
         if dev_map[addr.host] then
            glr.kill(dev_map[addr.host])
            delete_junk_items()
         end
         dev_map[addr.host] = nil
         
         local _,all = glr.status.processes()
         pprint.pprint(all, "GProcesses")
         pprint.pprint(dev_map, "map")
      else
         
         --msg={"host":"192.168.56.101","port":1010,"gpid",122,"type":"agent"}
         assert(msg_type,"glr.recv error")
         -- msg_table=cjson.decode(msg)
         local msg_table = ffi.new("BIND_MSG")
         ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
         -- msg_table.Catagory = msg_table.Catagory
         pprint.pprint(msg_table.Catagory, "DEV_CATA")
         if (msg_table.Catagory == ffi.C.DEV_AGENT) then
            -- for i=0,100 do
            err,id = glr.spawn("lsr","agent_worker")
            dev_map[string.format("%s::%d", addr.host, addr.port)] = id
            -- end
         elseif msg_table.Catagory == ffi.C.DEV_MONITOR then
            id = status_server
         elseif msg_table.Catagory == ffi.C.DEV_DISPLAY then
            err,id=glr.spawn("lsr","display_worker")
            dev_map[string.format("%s::%d", addr.host, addr.port)] = id
         end
         -- services["bind_gpid"]=
         print(err,id)
         msg_table.Gpid = ffi.C.htonl(id)
         msg_table.Head.Action = ffi.C.ACT_ACK
         pprint.pprint(addr)
         print(addr.host,addr.port,addr.gpid)
         if not glr.send(addr, structs.pack(msg_table)) then
            pprint.pprint("Send Failure")
         end
      end
   end
   -- _router:finalizer()
end

--a=cjson.decode("{\"a\":\"vvv\"}")
--print(pprint.pprint(a))
--print(cjson.encode(a))
function display_worker()
   print("Display")
   --local _router = glr.get_global(RID)
   local _router=router.router()
   local _subscrib = router.subscription()
   local _dev_map = router.device()
   local _amq = glr.get_global(MQID)
   local msg_type, addr, msg=glr.recv()
   local msg_table = ffi.new("ROUTER_ADD_MSG")
   ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
   function register()
      if msg_table.Head.Action==ffi.C.ACT_ROUTER_ADD then
         local host,port=glr.sys.host, glr.sys.port
         _router:register(structs.str_pack(msg_table.Name),
                          "",
                          host,
                          port,
                          __id__,
                          "display",
                          structs.str_pack(msg_table.AppType),
                          "lsr0",
                          glr.glr_stamp(),
                          "display",
                          "This is userdata")
         print("------------------------- Reigester",structs.str_pack(msg_table.Name),
               "display",
               host,
               port,
               __id__)
            
      else
         error("register message expected!")
      end

   end
   register()
   --print(pprint.pprint(_router:find_by_field("display")))
   local ret={status=true}
   msg_table.Head.Action = ffi.C.ACT_ACK
   glr.send(addr, structs.pack(msg_table))
   
   local nq = _amq:NQArray():get(0)
   local app_msg = ffi.new("APP_HEADER")
   local subscribes = {}

   while true do
      local msg_type, msg_addr ,msg = glr.recv()
      if msg  then
         ffi.copy(app_msg, msg, ffi.sizeof(app_msg))
      end
      
      if  app_msg.From.Catagory == ffi.C.DEV_SVC then
         
         local gpid = subscribes[structs.str_pack(app_msg.To.AppType)] or 0
         local des_addr = {host = addr.host, port = addr.port, gpid = gpid}
         pprint.pprint(structs.str_pack(app_msg.To.AppType), "Topic")
         pprint.pprint(subscribes, "Sub")

         pprint.pprint(des_addr, "Send2Display")
         glr.send(des_addr, msg)
      elseif app_msg.From.Catagory == ffi.C.DEV_DISPLAY then
         if app_msg.Head.Action == ffi.C.ACT_REQUEST then
            nq:put(msg)
         elseif app_msg.Head.Action == ffi.C.ACT_ROUTER_QUERY then
            local content={items = _router:find_by_cata("agent")}
            for _,item in pairs(content.items) do
               local desp = cjson.decode(_dev_map:find(item.dev_type)[1].desp)
               item.desp = desp
            end
            pprint.pprint(content, "Router")
            app_msg.Head.Action = ffi.C.ACT_RESPONSE
            
            glr.send(addr, structs.pack(app_msg) .. cjson.encode(content))
         elseif app_msg.Head.Action == ffi.C.ACT_SUBSCRIB then
            local content = cjson.decode(string.sub(msg, ffi.sizeof(app_msg) + 1))
            _subscrib:subscribe(content.dev_type, content.dev_id, content.topic, content.event_type, content.self)
            subscribes[string.format("%s.%s", content.dev_type, content.topic)] = msg_addr.gpid
            pprint.pprint(subscribes)
            app_msg.Head.Action = ffi.C.ACT_RESPONSE
            glr.send(addr, structs.pack(app_msg) .. cjson.encode({}))
         elseif app_msg.Head.Action == ffi.C.ACT_SUBSCRIB_DEL then
            local content = cjson.decode(string.sub(msg, ffi.sizeof(app_msg) + 1))
            _subscrib:delete(content.condition)
            app_msg.Head.Action = ffi.C.ACT_RESPONSE
            glr.send(addr, structs.pack(app_msg))
         elseif app_msg.Head.Action == ffi.C.ACT_SUBSCRIB_QRY then
            local cond = string.format("who=%s", structs.str_pack(app_msg.From.Name))
            local subs = _subscrib:find(cond)
            app_msg.Head.Action = ffi.C.ACT_RESPONSE
            glr.send(addr, structs.pack(app_msg) .. cjson.encode(subs))
         end
      end
   end
   -- _router:finalizer()
end 

function agent_worker()
   --local _router = glr.get_global(RID)
   local _router=router.router()
   local _amq = glr.get_global(MQID)
   local msg_type, addr, msg=glr.recv()
   -- local msg_table=cjson.decode(msg)
   
   local msg_table = ffi.new("ROUTER_ADD_MSG")
   ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
   function register()
      if msg_table.Head.Action == ffi.C.ACT_ROUTER_ADD then
         local host = glr.sys.host
         local port = glr.sys.port
         _router:register(structs.str_pack(msg_table.Name),
                          "agent",
                          host,
                          port,
                          __id__,
                          structs.str_pack(msg_table.AppType),
                          "",
                          "lsr0",
                          glr.glr_stamp(),
                          "agent",
                          "This is userdata")
      else
         error("register message expected!")
      end

   end
   register()
   
   msg_table.Head.Action = ffi.C.ACT_ACK
   glr.send(addr, structs.pack(msg_table))
    
   -- pprint.print("Read source file")
   local handle = io.open(configure.LUA_AGENT_APP_DIR .. structs.str_pack(msg_table.AppType) .. ".lua")
   print(structs.str_pack(msg_table.AppType))
   local source
   if handle then
      source = handle:read("*a")
   else
      source = ""
   end
   
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
   glr.send(addr,
             structs.pack(send_msg) .. content)
   -- tell svc that an agent have registered
   local nq = _amq:NQArray():get(0)
   -- nq:put(cjson.encode(msg_table))

   local app_msg = ffi.new("APP_HEADER")
   while true do
      print("Event Loop")
      local msg_type, _ ,msg = glr.recv()
      -- local t=cjson.decode(msg)
      ffi.copy(app_msg, msg, ffi.sizeof(app_msg))
      print(msg, app_msg.From.Catagory)
            
      -- t.header.timestamp = timer.time() -- add time stamp
      if  app_msg.From.Catagory == ffi.C.DEV_SVC and app_msg.To.Catagory == ffi.C.DEV_AGENT then
         glr.send(addr, msg)
      elseif app_msg.From.Catagory == ffi.C.DEV_AGENT then
         if app_msg.Head.Action == ffi.C.ACT_REPORT then
            print("SendMsg", #msg, msg)
            nq:put(msg)
         elseif app_msg.Head.Action == ffi.C.ACT_RESPONSE then
            local to_addr = app_msg.To.Addr
            print(structs.str_pack(to_addr.Host), ffi.C.ntohl(to_addr.Port), ffi.C.ntohl(to_addr.Gpid))
            glr.send({host = structs.str_pack(to_addr.Host), 
                      port = ffi.C.ntohl(to_addr.Port), 
                      gpid = ffi.C.ntohl(to_addr.Gpid)}, 
                     msg)
         end
      end
    end
   -- _router:finalizer()
end

