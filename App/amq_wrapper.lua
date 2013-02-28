--[[
    amq wrapper
--]]
module(...,package.seeall)
package.cpath="/home/gala/src/GalaLua/LuaObject/?.so;;"
local amq=require("amq")

local MQID = "channelv"
function new()
   local m = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
   glr.global(MQID, m, "CGalaxyMQ");
end


function ()
   print(type(m))
   print(".." .. type("@" .. MQID))
   local q = m:NQArray():get(0)
   local err,msg = q:get(q.MQC_BAT)
   print(msg)
end

function put(msg)
    --must create testAMQ.chl before
    local m = glr.get_global(MQID)
    local nqa=m:NQArray()

    local nq=nqa:get(0)

    nq:put(nq.MQC_RTL,msg)
    m:finalizer()
end

function get()
    --must create testAMQ.chl before
    local m=amq.new("/home/gala/channels/testAMQ.chl")
    local nqa=m:NQArray()

    local nq=nqa:get(0)

    local id,msg,len=nq:get(nq.MQC_BAT)

    m:finalizer()
    return msg
end
