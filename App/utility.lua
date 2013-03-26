module(..., package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]

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


local _router = router.new("router")
local _amq = amq.new(configure.amq.path)
local myhost, myport = glr.sys.host, glr.sys.port

function router_info()
   -- init
   -- _router:delete("10549531@CSharp")
   pprint.pprint(_router:find_by_field("display"), "display")
   pprint.pprint(_router:find_by_field("agent"), "agent")
end

function amq_info()
   local amq_view = _amq:NQArray():get(0):view()
   pprint.pprint({msg_number = amq_view:total(0),
                 }, 
                 "MQ Info")
end

