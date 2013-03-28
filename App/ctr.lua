local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local node = require "node"
-- local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
local configure = require "configure"
local timer = require "timer"
local ffi = require "ffi"
local structs = require "structs"
--print(package.path)
--print(package.cpath)
local RID = "@router"
local MQID = "@mq"




function main()
   -- init
   local _router=router.new("router")
   glr.global(RID, _router)
   local _amq=amq.new(configure.amq.path)
   glr.global(MQID, _amq)
   local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")
   print("------Status :::::" .. status_server)
   -- local err,svc=glr.spawn(os.getenv("HOME") .. "/lib/lua/lsr.lua","fake_svc")
   -- print("------svc ::::::" .. svc)
 
end

function router_monitor ()
   while true do
      local msg_type,addr,msg = glr.recv()
   end
end 