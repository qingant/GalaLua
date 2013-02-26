local node = require "node"

function main()
   
   local rt = node.register("0.0.0.0", 2345, __id__)
   print("Register")
   print(rt)
   for i = 0, 10 do
      -- gpid
      node.send("0.0.0.0", 2345,0, [[{["type"]="agent", ["des_gpid"]=__id__}]])

   end
end