
module(...,package.seeall)

local pprint = require "pprint"
local router = require "router"
local ffi = require "ffi"
local cjson = require "cjson"
local structs = require "structs"

Gala = {}
Gala.__doc__ = {

}

-- init function --
function Gala:new(o)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   return o
end

function Gala:Broadcast(msg)
   if msg then
      local displays = self.__gala__._router:find_by_cata("display")
      pprint.pprint(displays)
      -- self.request.header.from.type = "svc"
      -- self.request.header.id = -1024
      -- pprint.pprint(displays)
      local _sub = router.subscription()
      local _router = self.__gala__._router
      local _logger = self.__gala__._logger
      local _id = self.__gala__._router_info.name
      -- _logger:debug(cjson.encode(displays))
      print(_id)

      local items = _sub:find(string.format("whom='%s'", _id))
      pprint.pprint(items, "Subscription")
      local mem = {}
      for _, item in pairs(items) do
         local addr
         if mem[item.who] then
            addr = nil
         else
            mem[item.who] = true
            addr = _router:find_by_name(item.who)
         end
         pprint.pprint(addr, "Addr")
         if addr then
            glr.connect(addr.host, addr.port)
            local header = self.__gala__._header
            header.From.Catagory = ffi.C.DEV_SVC
            header.Head.Action = ffi.C.ACT_REPORT
            pprint.pprint(addr, "BroadCast")
            glr.send(addr, structs.pack(header) .. cjson.encode(msg))
         end
      end
      -- for index,item in pairs(displays) do
      --    print("BroadCast")
      --    -- local item =displays[i]
      --    -- print(item.addr.host, item.addr.port, __id__)
      --    glr.connect(item.host, item.port)
      --    print(item.host, item.port , item.gpid)
      --    local header = self.__gala__._header
      --    header.From.Catagory = ffi.C.DEV_SVC
      --    header.Head.Action = ffi.C.ACT_REPORT
      --    -- print(item.addr.host, item.addr.port , item.addr.gpid, cjson.encode(msg))
      --    glr.send(item, structs.pack(header) .. cjson.encode(msg))
      -- end
   end
end
