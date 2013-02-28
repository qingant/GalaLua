local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"
local amq = require "amq"

local MQID = "channelv"
function main()
   local m = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
   glr.global(MQID, m, "CGalaxyMQ");
   print("Over")
   local q = m:NQArray():get(0)
   q:put(q.MQC_RTL,"Just For Test")
   glr.spawn(os.getenv("HOME") .. "/lib/lua/try.lua", "ttt");
end


function ttt()
   local m = glr.get_global(MQID)
   print(type(m))
   print(".." .. type("@" .. MQID))
   local q = m:NQArray():get(0)
   local err,msg = q:get(q.MQC_BAT)
   print(msg)
end