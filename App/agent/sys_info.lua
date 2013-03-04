local host, port , gpid = ...
pprint.print("Come on")
-- local timer = require "timer"
-- local os = require "os"
-- local io = require "io"
-- local pprint = require "pprint"
while true do
   print("Ok")
   local out = io.popen("top -n 1|grep Cpu"):read("*a") 
   print(out)
   report(host, port, gpid, {["cpu_info"]=out})
   timer.sleep(5)
end
   