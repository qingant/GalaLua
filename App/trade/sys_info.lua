module(..., package.seeall)

local pprint = require "pprint"
require "router"
require "node"
local cjson = require "cjson"
local dbaccess = require "dbaccess"
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
    if us < 0.5 and false then
        self.request.content = nil
    end
end

function Report:Broadcast(msg)
    if msg then
        local displays = self.__gala__._router:find_by_field("display")
        self.request.header.from.type = "svc"
        self.request.header.id = -1024
        pprint.pprint(displays)
        local _logger = self.__gala__._logger
        _logger:debug(cjson.encode(displays))
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
       local connection = self.__gala__._db
       pprint.pprint(self.__gala__)
       local cpu_info = request.content.cpu
       local prepare = 'insert into cpu_info(us,sy,ni,id,wa,hi,si,st) values(?,?,?,?,?,?,?,?)'
       local strsql = string.format("%s,%s,%s,%s,%s,%s,%s,%s",cpu_info.us,cpu_info.sy,cpu_info.ni,cpu_info.id,cpu_info.wa,cpu_info.hi,cpu_info.si,cpu_info.st)
       connection:execute("create table  if not exists cpu_info (us double, sy double, ni double, id double, wa double, hi double, si double, st double)"):close()
       -- connection:execute(strsql):close()
       _logger:debug("cup information insert to database")
       local cur = connection:execute(prepare,cpu_info.us,cpu_info.sy,cpu_info.ni,cpu_info.id,cpu_info.wa,cpu_info.hi,cpu_info.si,cpu_info.st)
       cur:close()         
       _logger:debug("insert successfully")
       _logger:debug("broadcast  begin")
       self:Broadcast(request) 
       _logger:debug("broadcast complete")
       connection:commit()
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

   self.request.content = {ok = "From SVC!"}
   function request_agent()
      local source = io.open(configure.LUA_AGENT_APP_DIR .. self.request.header.from.app_type .. "_request.lua"):read("*a")
      local host, port = glr.node_addr()
      local request = {
         header = {
            from = {
               type = "svc",
               host = host,
               port = port,
               gpid = __id__,
            },
            to = {
               action = "request",
               
            },
         },
         content = {
            code = source
         }
      }
      local name = self.request.header.to.destination
      local addr = self.__gala__._router:find_by_name(name)
      node.register(addr.addr.host, addr.addr.port)
      print(addr.addr.host, addr.addr.port)
      node.send(addr.addr.host, addr.addr.port, addr.addr.gpid,
                cjson.encode(request))
   end
   request_agent()
   local msg_type, gpid, msg = glr.recv()
   print("FromAgent" .. msg)
   
   local t = cjson.decode(msg)
   self.request.header.from.type = "svc"

   self.request.content = t.content

   -- print(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port, self.__gala__.route_info.addr.gpid)
   -- print("Run!")
   -- node.register(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port)
   node.send(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port, self.__gala__.route_info.addr.gpid,
             cjson.encode(self.request))

end