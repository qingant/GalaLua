local host, port , gpid = ...
pprint.print("Come on")
-- local timer = require "timer"
-- local os = require "os"
-- local io = require "io"
-- local pprint = require "pprint"
while true do
   -- print("Ok")
   local out = io.popen("top -b -n 1|grep Cpu"):read("*a") 
   print(out)
   local words = string.gmatch(out, "(%d*%.%d*)%%(%a*)")
   local data = {}
   for k,v in words do
      data[v] = k
   end
   pprint.pprint(data)
   report(host, port, gpid, {["cpu"]=data})
   timer.sleep(5)
end
   