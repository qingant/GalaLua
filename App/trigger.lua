module(..., package.seeall)

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
local collections = require("collections")
--print(package.path)
--print(package.cpath)
local RID = "@router"
local MQID = "@mq"

function main(...)
   local err, status = glr.spawn("status", "main")

   while true do
      local msg_type, addr, msg = glr.recv()
      if msg_type == glr.CLOSED then
      else
         local msg_struct = ffi.new("BIND_MSG")
         if msg_struct.Catagory == ffi.C.DEV_MONITOR then
            msg_struct.Gpid = ffi.C.htonl(status)
            msg_struct.Head.Action = ffi.C.ACT_ACK
            glr.send(addr, structs.pack(msg_struct))
         end
      end
   end
end


function dispatcher()
   local _amq = amq.new(configure.amq.path)
   glr.global(MQID, _amq)   
   local nq = _amq:NQArray():get(0)

   local workers = collections.List.new()
   for i = 1, 100 do
      local err, worker = glr.spawn("trigger", "worker", __id__)
      collections.List.pushright(workers, worker)
   end
   function collect_idle()
      while glr.msg_available() then
         local msg_type, addr, msg = glr.recv()
         if msg_type != glr.CLOSED then
            local p = cjson.decode(msg)
            collections.List.pushright(workers, p.gpid)
         end
   end
   while true do
      local msg = nq:get()
   
      if collections.List.empty(workers) then
         collect_idle()
      end
      if collections.List.empty(workers) then
         local err, worker = glr.spawn("trigger", "worker", __id__)
         collections.List.pushright(workers, worker)
      end
      local worker = collections.List.popleft(workers)
      glr.send(worker, msg)
   end
end

function worker(dispacher)
   while true do
      local _amq = glr.get_global()
      local _nq = _amq:NQArray():get(1)
      local msg_type, addr, msg = glr.recv()
      local request_header = ffi.new("APP_HEADER")
      ffi.copy(request_header, msg, ffi.sizeof(request_header)) 
      _nq:put(msg)
      glr.send(dispacher, cjson.encode({gpid = __id__}))
   end
end