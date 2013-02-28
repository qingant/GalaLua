
local os = require "os"
local io = require "io"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
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
   print(pprint.pprint(response))
   node.send(host, port, bind_gpid, cjson.encode({
                                                    ["host"] = host,
                                                    ["port"] = 2346,
                                                    ["gpid"] = __id__,
                                                    ["command"] = "register",
                                                    ["name"] = "myhost::sys::0",
                                                    ["field"] = "bank::east",
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
      local response = cjson.decode(msg)
      print(pprint.pprint(response))
   end

end