module(..., package.seeall)

local pprint = require "pprint"
require "router"
local ffi = require "ffi"
local structs = require "structs"
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
   local us = self.request.cpu.us
   if us < 1 and false  then
      self.request = nil
   end
end

function Report:Broadcast(msg)
   if msg then
      local displays = self.__gala__._router:find_by_cata("display")
      pprint.pprint(displays)
      -- self.request.header.from.type = "svc"
      -- self.request.header.id = -1024
      -- pprint.pprint(displays)
      local _sub = router.subscription()
      local _router = router.router()
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

function Report:Run()
   pprint.pprint(self.request)
   local _logger = self.__gala__._logger
   --_logger:debug("trade  begin")
   -- _logger:debug(cjson.encode(self.request))
   self:Filter()
   local request = self.request
   if request then
      local connection = self.__gala__._db
      pprint.pprint(self.__gala__)
      if  false then 
         local cpu_info = request.cpu
         local prepare = 'insert into cpu_info(us,sy,ni,id,wa,hi,si,st) values(?,?,?,?,?,?,?,?)'
         local strsql = string.format("%s,%s,%s,%s,%s,%s,%s,%s",cpu_info.us,cpu_info.sy,cpu_info.ni,cpu_info.id,cpu_info.wa,cpu_info.hi,cpu_info.si,cpu_info.st)
         -- connection:execute("create table  if not exists cpu_info (us double, sy double, ni double, id double, wa double, hi double, si double, st double)"):close()
         -- connection:execute(strsql):close()
         _logger:debug("cup information insert to database")
         --local cur = connection:execute(prepare,cpu_info.us,cpu_info.sy,cpu_info.ni,cpu_info.id,cpu_info.wa,cpu_info.hi,cpu_info.si,cpu_info.st)
         --cur:close()         
         _logger:debug("insert successfully")
         _logger:debug("broadcast  begin")
         _logger:debug("broadcast complete")

      else
      end
      self:Broadcast(request) 

      --connection:commit()
   end
end



Request = Gala:new()
function Request:init(__gala__, request)
   self.__gala__ = __gala__
   self.request = request
end


function Request:Run()
   pprint.pprint(self.request)
   local header = self.__gala__._header
   header.From.Catagory = ffi.C.DEV_SVC
   function request_agent()
      -- local source = io.open(configure.LUA_AGENT_APP_DIR .. structs.str_pack(header.To.AppType) .. "_request.lua"):read("*a")
      local host, port = glr.sys.host, glr.sys.port

      header.Head.Action = ffi.C.ACT_REQUEST
      header.To.Catagory = ffi.C.DEV_AGENT
      header.To.Addr.Host = host
      header.To.Addr.Port = ffi.C.htonl(port)
      header.To.Addr.Gpid = ffi.C.htonl(__id__)
      local name = structs.str_pack(header.To.Name)
      local addr = self.__gala__._router:find_by_name(name)
      pprint.pprint(name, "name")
      pprint.pprint(addr, "addr")

      glr.connect(addr.host, addr.port)
      -- print(addr.addr.host, addr.addr.port, addr.addr.gpid)
      glr.send(addr,
                structs.pack(header) .. cjson.encode({code = ""}))
   end
   request_agent()
   local msg_type, gpid, msg = glr.recv()
   print("FromAgent" .. msg)
   
   -- local t = cjson.decode(msg)
   -- self.request.header.from.type = "svc"
   -- local result = ffi.new("APP_HEADER")
   -- ffi.copy(result, msg, ffi.sizeof(result))

   header.From.Catagory = ffi.C.DEV_SVC
   local content = string.sub(msg, ffi.sizeof(header)+1)
   -- print(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port, self.__gala__.route_info.addr.gpid)
   -- print("Run!")
   -- print(self.__gala__.route_info.addr.host, self.__gala__.route_info.addr.port, self.__gala__.route_info.addr.gpid)
   glr.connect(self.__gala__.route_info.host, self.__gala__.route_info.port)
   glr.send(self.__gala__.route_info,
             structs.pack(header) .. content)

end