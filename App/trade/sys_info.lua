module(..., package.seeall)

local pprint = require "pprint"
require "router"
require "node"
local cjson = require "cjson"
-- class gala -- 
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

-- class Report
Report = Gala:new()

function Report:init(__gala__, request)
   self.__gala__ = __gala__
   self.request = request
end

function Report:Run()
   pprint.pprint(self.request)
   local displays = self.__gala__._router:find_by_field("display")
   self.request.header.from.type = "svc"
   pprint.pprint(displays)
   for i=1,#displays do
      
      local item =displays[i]
      print(item.addr.host, item.addr.port, __id__)
      node.register(item.addr.host, item.addr.port)
      print(item.addr.host, item.addr.port , item.addr.gpid)
      node.send(item.addr.host, item.addr.port , item.addr.gpid, cjson.encode(self.request))
   end
end



Request = Gala:new()
function Request:init(__gala__, request)
   self.__gala__ = __gala__
   self.request = request
end


function Request:Run()
   pprint.pprint(self.request)
   self.request.header.from.type = "svc"
   self.request.content.ok = "From SVC!"
   print(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port, self.__gala__.route_info.addr.gpid)
   print("Run!")
   node.register(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port)
   node.send(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port, self.__gala__.route_info.addr.gpid,
             cjson.encode(self.request))
   -- local displays = self.__gala__._router:find_by_field("display")
   -- self.request.header.from.type = "svc"
   -- pprint.pprint(displays)
   -- for i=1,#displays do
      
   --    local item =displays[i]
   --    print(item.addr.host, item.addr.port, __id__)
   --    node.register(item.addr.host, item.addr.port)
   --    print(item.addr.host, item.addr.port , item.addr.gpid)
   --    node.send(item.addr.host, item.addr.port , item.addr.gpid, cjson.encode(self.request))
   -- end
end