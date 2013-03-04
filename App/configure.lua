--[[
    configure
--]]
module(...,package.seeall)

local os=require("os")
LUA_HOME = os.getenv("HOME") .. "/lib/lua/"
LUA_AGENT_APP_DIR = os.getenv("HOME") .. "/workspace/agent/"
PATH="./?.lua;"..os.getenv("HOME").."/lib/lua/?.lua"
CPATH="./?.so;"..os.getenv("HOME").."/lib/lua/?.so"

svc={SVC_MAX=1, PATH = PATH .. ";" .. os.getenv("HOME") .. "/workspace/trade/?.lua"}
amq={path=os.getenv("HOME") .. "/channels/testAMQ.chl"}
