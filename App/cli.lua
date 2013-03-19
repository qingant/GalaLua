
local os = require "os"
local io = require "io"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
--local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
-- local io = require "io"
local timer = require "timer"
local ffi = require "ffi"
local structs = require "structs"
function main()
   local host = "0.0.0.0"
   local port = 2345
   local rt = node.register(host, port, __id__)


   -- print("Register")
   -- print(rt)
   
   -- entry
   local msg = ffi.new("BIND_MSG")
   msg.Head.Action = ffi.C.ACT_BIND
   msg.Catagory = ffi.C.DEV_AGENT
   node.send(host, port, 0, ffi.string(msg, ffi.sizeof(msg)))
   local msg_type, addr, rsp = glr.recv()
   assert(msg_type,rsp)
   -- local response = cjson.decode(msg)
   ffi.copy(msg, rsp, ffi.sizeof(msg))
   local bind_gpid = ffi.C.ntohl(msg.Gpid)
   -- local bind_gpid = msg.Gpid
   print(host,port,bind_gpid) 
   -- print(pprint.pprint(response))
   local reg_msg = ffi.new("ROUTER_ADD_MSG")
   reg_msg.Head.Action = ffi.C.ACT_ROUTER_ADD
   reg_msg.Name = string.format("myhost::sys::%d", bind_gpid)
   reg_msg.Field = "bank::east"
   reg_msg.AppType =  "sys_info"
   node.send(host, port, bind_gpid, ffi.string(reg_msg, ffi.sizeof(reg_msg)))
   msg_type,addr, rsp = glr.recv()
   ffi.copy(reg_msg, rsp, ffi.sizeof(reg_msg))
   
   -- response = cjson.decode(msg)
   -- local status = response["status"]
   if reg_msg.Head.Action ~= ffi.C.ACT_ACK then
      error("")
   end
   print("Register Succ")
   local app_head = ffi.new("APP_HEADER")
   while true do
      msg_type, _, msg = glr.recv()
      ffi.copy(app_head, msg, ffi.sizeof(app_head))
      local content = string.sub(msg, ffi.sizeof(app_head)+1)
      pprint.pprint(content)
      content = cjson.decode(content)
      if msg_type == glr.CLOSED then
         break
      end
      -- local request = cjson.decode(msg)
      if app_head.Head.Action == ffi.C.ACT_DEPLOY then
         local msg = {}
         msg["code"] = content.content.code
         msg["host"] = addr.host
         msg["port"] = addr.port
         msg["gpid"] =  bind_gpid
         local err,id = glr.spawn(os.getenv("HOME") .. "/lib/lua/cli.lua", "worker")
         glr.send(id, cjson.encode(msg))
      elseif app_head.Head.Action == ffi.C.ACT_REQUEST then
         local msg = {}
         msg["code"] = content.code
         msg["host"] = host
         msg["port"] = port
         msg["gpid"] =  bind_gpid
         msg["des_host"] = structs.str_pack(app_head.To.Addr.Host)
         msg["des_port"] = ffi.C.ntohl(app_head.To.Addr.Port)
         msg["des_gpid"] = ffi.C.ntohl(app_head.To.Addr.Gpid)
         local err,id = glr.spawn(glr.get_path(), "request_handle")
         glr.send(id, cjson.encode(msg))
      end
   end
      -- print(pprint.pprint(response))
      -- node.send(host, port, bind_gpid, cjson.encode({header={from="agent"},content={aaa="Hello world"}}))
end

local header = ffi.new("APP_HEADER")
function report (host, port, gpid, data)
   header.Head.MsgId = -1
   header.Head.Action = ffi.C.ACT_REPORT
   header.From.Catagory = ffi.C.DEV_AGENT
   header.From.Name = string.format("myhost::sys::%d", gpid)
   header.To.Catagory = ffi.C.DEV_SVC
   header.To.AppType = "sys_info"
   return node.send(host, port , gpid, structs.pack(header) .. cjson.encode(data))
end
function response(host, port, gpid, des_host, des_port, des_gpid, data)

   header.Head.MsgId = -1
   header.Head.Action = ffi.C.ACT_RESPONSE

   header.From.Catagory = ffi.C.DEV_AGENT
   header.From.Name = string.format("myhost::sys::%d", gpid)
   header.To.Catagory = ffi.C.DEV_SVC
   header.To.Addr.Host = des_host
   header.To.Addr.Port = ffi.C.htonl(des_port)
   header.To.Addr.Gpid = ffi.C.htonl(des_gpid)
   print( des_host, des_port, des_gpid)
   return node.send(host, port , gpid, structs.pack(header) .. cjson.encode(data))
end

function worker()
   print("Before")
   local msg_type, addr, msg = glr.recv()
   print("Recved!")
   print(msg)
   local request = cjson.decode(msg)
   -- print(msg)
   -- local fun = loadstring(request["code"])
   -- pprint.print(fun)
   -- fun(request["host"], request["port"], request["gpid"], request.des_host, request.des_port, request.des_gpid)
   while true do

      local fun = loadstring(request["code"])
      local data = fun(request.host, request.port, request.gpid)
      -- print(data)
      -- report(request.host, request.port, request.gpid, {data})
      timer.sleep(5)
   end
end

function request_handle()
   local msg_type, gpid, msg = glr.recv()
   local request = cjson.decode(msg)
   local fun = loadstring(request["code"])
   print(request.host, request.port, request.gpid,request.des_host, request.des_port, request.des_gpid)
   local data = fun(request.host, request.port, request.gpid,request.des_host, request.des_port, request.des_gpid)
   -- responce(request.host, request.port, request.gpid,request.des_host, request.des_port, request.des_gpid, data)
end