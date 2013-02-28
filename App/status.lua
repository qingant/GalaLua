local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local cjson = require "cjson" 
local node = require "node"

function main()
   while true do
      local err,msg = glr.recv()
      if err then
         local request = cjson.decode(msg)
         print(request)
         local response
         host = request.host
         port = request.port
         src = request.src
         command = request.command
         if command == "all" then
            err,response = glr.all()
         elseif command == "status" then
            err,response = glr.status(request.pid)
         end
         local err = node.send(host,port, src, cjson.encode(response))
         if err then
         else
            error("send error")
         end
      end
   end
end