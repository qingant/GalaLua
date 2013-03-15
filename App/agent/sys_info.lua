local host, port , gpid = ...
pprint.print("Come on")
-- local timer = require "timer"
-- local os = require "os"
-- local io = require "io"
-- local pprint = require "pprint"
while true do
   -- print("Ok")
   local cmd = "top -b -n 2 -d0.1|grep Cpu|tail -n 1"

   local out = io.popen(cmd):read("*a") 
   -- print(out)
   local words = string.gmatch(out, "(%d*%.%d*)%%(%a*)")
   local data = {}
   for k,v in words do
      data[v] = tonumber(k)
   end
   -- pprint.pprint(data)
   for i=0,100 do
      data.ID = i
      report(host, port, gpid, {["cpu"]=data})
   end
   timer.sleep(1)
end
   