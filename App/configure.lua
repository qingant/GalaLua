--[[
    configure
--]]
module(...,package.seeall)

local os=require("os")

PATH="./?.lua;"..os.getenv("HOME").."/lib/lua/?.lua"
CPATH="./?.so;"..os.getenv("HOME").."/lib/lua/?.so"
svc={SVC_MAX=100}
amq={path=os.getenv("HOME") .. "/channels/testAMQ.chl"}
