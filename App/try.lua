local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"
local amq = require "amq"

local MQID = "channelv"
function main()
   -- local m = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
   -- glr.global(MQID, m, "CGalaxyMQ");
   -- print("Over")
   -- local q = m:NQArray():get(0)
   -- q:put(q.MQC_RTL,"Just For Test")
   print("Repl")
   while true do
      -- err,msg = glr.recv()
      -- print(msg)
   
      local io = require "io"

      io.stdout:write("\n=> ")
      io.stdout:flush()
      -- io.stdin:seek("set",0)
      local cmd = io.stdin:read("*l")
      print("return " .. cmd)
      local status, fun = pcall(function () return loadstring("return "..cmd) end)
      if status then
         local status, rt = pcall(fun)
         io.stdout:write(tostring(rt))
      else
         io.stdout:write(tostring(fun))
      end

      
      io.flush()
   end
   -- glr.spawn(glr.get_path(), "ttt");
   -- while true do
   --    glr.recv()
   -- end
   -- print "over"
   -- s = "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
   -- s = s..s..s..s..s
   -- for i=0,1000 do
   --    glr.send(id, s..s..s..s..s)
   -- end
--    s = loadstring("local a,b = ...
-- print(type(q))
-- while true do
-- glr.sleep(5)
-- return glr.node_addr()")("test", "ok")
--   print(s)
end


function ttt()
   print("Repl")
   while true do
      -- err,msg = glr.recv()
      -- print(msg)
   
      local io = require "io"

      io.stdout:write("\n=> ")
      io.stdout:flush()
      -- io.stdin:seek("set",0)
      local cmd = io.stdin:read("*l")
      print("return " .. cmd)
      local status, fun = pcall(function () return loadstring("return "..cmd) end)
      if status then
         local rt = fun()
         io.stdout:write(tostring(rt))
      else
         io.stdout:write(tostring(fun))
      end

      
      io.flush()
   end
   -- local m = glr.get_global(MQID)
   -- print(type(m))
   -- print(".." .. type("@" .. MQID))
   -- local q = m:NQArray():get(0)
   -- local err,msg = q:get(q.MQC_BAT)
   -- print(msg)
end