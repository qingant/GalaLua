
local node = require "node"
function main()
   while true do
      local msg = glr.recv()
      print(msg)
   end
end