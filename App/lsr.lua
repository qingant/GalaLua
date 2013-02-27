local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"

print(package.path)
print(package.cpath)
function main()
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
      local status, msg = glr.recv()
      print("MSG")
      print(msg)
      if msg then
         print(pprint.pprint(msg))
      end
      print(msg)
   end
end




