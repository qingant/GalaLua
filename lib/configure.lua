--[[
    configure
--]]
module(...,package.seeall)

local os=require("os")

PATH="./?.lua;"..os.getenv("HOME").."/lib/lua/?.lua"
CPATH="./?.so;"..os.getenv("HOME").."/lib/lua/?.so"
