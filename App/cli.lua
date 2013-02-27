local node = require "node"
local cjson = require "cjson"

function main()
   
   local rt = node.register("0.0.0.0", 2345, __id__)
   print("Register")
   print(rt)
   for i = 0, 10 do
      -- gpid
      node.send("0.0.0.0", 2345, 1, cjson.encode({["host"]="0.0.0.0",["port"]=2346, ["src"]=__id__, ["command"]="all"}))
      local err, msg = glr.recv()
      print(msg)

   end
end