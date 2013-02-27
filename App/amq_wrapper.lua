--[[
    amq wrapper
--]]
module(...,package.seeall)
package.cpath="/home/gala/src/GalaLua/LuaObject/?.so;;"
local amq=require("amq")

function put(msg)
    --must create testAMQ.chl before
    local m=amq.new("/home/gala/channels/testAMQ.chl")
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
