
local os = require "os"
local io = require "io"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
-- local io = require "io"
local timer = require "timer"
function main()
   local host = "0.0.0.0"
   local port = 2345
   local rt = node.register(host, port, __id__)


   -- print("Register")
   -- print(rt)
   
   -- entry
   node.send(host, port, 0, cjson.encode({
                                            ["host"] = host,
                                            ["port"] = 2346, -- youself port
                                            ["src_gpid"] = __id__,
                                            ["dev_type"] = "agent",
                                         }))
   local err, msg = glr.recv()
   assert(err,msg)
   local response = cjson.decode(msg)
   local bind_gpid = response["bind_gpid"]
   print(host,port,bind_gpid) 
   print(pprint.pprint(response))
   node.send(host, port, bind_gpid, cjson.encode({
                                                    ["host"] = host,
                                                    ["port"] = 2346,
                                                    ["gpid"] = __id__,
                                                    ["command"] = "register",
                                                    ["name"] = "myhost::sys::0",
                                                    ["field"] = "bank::east",
                                                    ["app_type"] = "sys_info",
                                                 }))
   err,msg = glr.recv()
   response = cjson.decode(msg)
   local status = response["status"]
   if not status then
      error("")
   end
   print("Register Succ")
   while true do
      err, msg = glr.recv()
      local request = cjson.decode(msg)
      if request.header.to.action == "excute" then
         local msg = {}
         msg["code"] = request.content.code
         msg["host"] = host
         msg["port"] = port
         msg["gpid"] =  bind_gpid
         local err,id = glr.spawn(os.getenv("HOME") .. "/lib/lua/cli.lua", "worker")
         glr.send(id, cjson.encode(msg))
      elseif request.header.to.action == "request" then
         local msg = {}
         msg["code"] = request.content.code
         msg["host"] = host
         msg["port"] = port
         msg["gpid"] =  bind_gpid
         msg["des_host"] = request.header.request.from.host
         msg["des_port"] = request.header.request.from.port
         msg["des_gpid"] = request.header.request.from.gpid
         local err,id = glr.spawn(glr.get_path(), "worker")
         glr.send(id, cjson.encode(msg))
      end
   end
      -- print(pprint.pprint(response))
      -- node.send(host, port, bind_gpid, cjson.encode({header={from="agent"},content={aaa="Hello world"}}))
end


function report (host, port, gpid, data)
   local response = {
      ["header"] = {
         ["from"] = {
            ["type"] = "agent",
            ["name"] =  "myhost::sys::0",
         },
         ["to"] = {
            ["type"] = "svc",
            ["action"] = "report",
         }
      }
   }
   response.content = data
   return node.send(host, port , gpid, cjson.encode(response))
end
function response(host, port, gpid, des_host, des_port, des_gpid, data)
   local response = {
      ["header"] = {
         ["from"] = {
            ["type"] = "agent",
            ["name"] =  "myhost::sys::0",
         },
         ["to"] = {
            ["type"] = "svc",
            ["action"] = "response",
 ["host"] = des_host,
            ["port"] = des_port,
            ["gpid"] = des_gpid,
         }
      }
   }
   response.content = data
   return node.send(host, port , gpid, cjson.encode(response))
end

function worker()
   print("Before")
   local err,msg = glr.recv()
   print("Recved!")
   print(msg)
   local request = cjson.decode(msg)
   -- print(msg)
   -- local fun = loadstring(request["code"])
   -- pprint.print(fun)
   -- fun(request["host"], request["port"], request["gpid"], request.des_host, request.des_port, request.des_gpid)
   while true do
      local data = {"100 200 300 400"}
      print(data)
      report(request.host, request.port, request.gpid, data)
      timer.sleep(5)
   end
end

function request_handle()
   local err,msg = glr.recv()
   local request = cjson.decode(msg)
   local data = {"100 200 300 400"}
   responce(request.host, request.port, request.gpid, data)
end