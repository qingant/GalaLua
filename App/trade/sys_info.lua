module(..., package.seeall)

local pprint = require "pprint"
require "router"
require "node"
local cjson = require "cjson"
-- local loggger = require "logger"
-- print(logger)
print(cjson)
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
local logger
function Report:init(__gala__, request)
   self.__gala__ = __gala__
   self.request = request
   local _logger = self.__gala__._logger
   _logger:debug("app init")
end

function Report:Filter()    
    local us = self.request.content.cpu.us
    if us < 0.5 then
        self.request.content = nil
    end
end

function Report:Broadcast(msg)
    if msg then
        local displays = self.__gala__._router:find_by_field("display")
        self.request.header.from.type = "svc"
        pprint.pprint(displays)
        local _logger = self.__gala__._logger
        _logger:debug(cjson.encod(displays))
        for i=1,#displays do
      
            local item =displays[i]
            print(item.addr.host, item.addr.port, __id__)
            node.register(item.addr.host, item.addr.port)
            print(item.addr.host, item.addr.port , item.addr.gpid)
            node.send(item.addr.host, item.addr.port , item.addr.gpid, cjson.encode(msg))
       end
    end
end

function Report:Run()
   pprint.pprint(self.request)
   local _logger = self.__gala__._logger
   _logger:debug("trade  begin")
   _logger:debug(cjson.encode(self.request))
   self:Filter()
   local request = self.request
   if request.content then
       local connection = self.__gala__.db_connection
       local cpu_info = request.content.cpu
       local prepare = 'insert into cpu_info(us,sy,ni,id,wa,hi,si,st) values(?,?,?,?,?,?,?,?)'
       local strsql = string.format("%s,%s,%s,%s,%s,%s,%s,%s",cpu_info.us,cpu_info.sy,cpu_info.ni,cpu_info.id,cpu_info.wa,cpu_info.hi,cpu_info.si,cpu_info.st)
       _logger:debug("cup information insert to database")
       local cur = connection:excute(prepare,strsql)
       cur:close()         
       _logger:debug("insert successfully")
       _logger:debug("broadcast  begin")
       self:Broadcast(request) 
       _logger:debug("broadcast complete")
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