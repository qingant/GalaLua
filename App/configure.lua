--[[
    configure
--]]
module(...,package.seeall)

local os=require("os")
LUA_HOME = os.getenv("HOME") .. "/lib/lua/"
LUA_AGENT_APP_DIR = os.getenv("HOME") .. "/workspace/agent/"
PATH="./?.lua;"..os.getenv("HOME").."/lib/lua/?.lua"
CPATH="./?.so;"..os.getenv("HOME").."/lib/lua/?.so"
local db={DB_Type="SQLite3",Connection={Path=os.getenv("HOME").."/tmp/test.db"}}
svc={SVC_MAX=4, PATH = PATH .. ";" .. os.getenv("HOME") .. "/workspace/trade/?.lua",db=db}

amq={path=os.getenv("HOME") .. "/channels/testAMQ.chl"}
