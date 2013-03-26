local host, port , gpid, des_host, des_port, des_gpid = ...
pprint.print("Come on")
local cli = require("cli")
-- local timer = require "timer"
-- local os = require "os"
-- local io = require "io"
-- local pprint = require "pprint"
   -- print("Ok")
local cmd = "top -b -n 2 -d0.1|grep Cpu|tail -n 1"
local out = io.popen(cmd):read("*a") 
print(out)
local words = string.gmatch(out, "(%d*%.%d*)%%(%a*)")
local data = {}
for k,v in words do
   data[v] = tonumber(k)
end
pprint.pprint(data)
cli.response(host, port, gpid, des_host, des_port, des_gpid, {["cpu"]=data})

   