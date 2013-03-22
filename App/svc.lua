local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local configure=require("Configure")
local node = require "node"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
local logger = require "logger"
local dbaccess = require "dbaccess"
local structs = require "structs"
local ffi = require "ffi"

local RID = "@router"
local MQID = "@mq"
local MTRCFG = "@conf"

local confpath = "/Configure/_STRUCTURE/MonitorList[00]/Monitor/"

function main()
   local _router,_amq
   function init_fake_global_var()
      _router=router.new("router")
      glr.global(RID,_router)
      _amq = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
      glr.global(MQID, _amq);
   end
   init_fake_global_var()

   local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")

   local processes = {}
   local err

   local conf = configure.new(os.getenv("HOME") .. "/cfg/shmfile/node.conf","r")
   glr.global(MTRCFG,conf)
   local err,svcmax = pcall(conf.get,conf,confpath .. "Base/SVCLimits")
   if not err then
       error(svcmax)
   end
   for i=0,tonumber(svcmax) do
      err,processes[i]= glr.spawn(os.getenv("HOME") .. "/lib/lua/" .. "svc.lua","worker")
   end

    while true do
        local msg_type,addr, msg = glr.recv()
        local msg_table = ffi.new("BIND_MSG")
        ffi.copy(msg_table, msg,ffi.sizeof(msg_table))
        pprint.pprint(msg_table)
        if msg_table.Catagory == ffi.C.DEV_MONITOR then
            msg_table.Gpid = ffi.C.htonl(status_server)
            msg_table.Head.Action = ffi.C.ACT_ACK
            if not node.send(addr.host,addr.port,addr.gpid, structs.pack(msg_table)) then
               pprint.pprint("Send Failure")
            end
        end
    end
end

function worker()
   local _router = glr.get_global(RID)
   local _amq = glr.get_global(MQID)

   local nq = _amq:NQArray():get(0)

   -- sqlite access connection create

   local conf = glr.get_global(MTRCFG)
   local err,dbtype = pcall(conf.get,conf,confpath .. "SVC/DBConnection/_SCROLLTYPE")
   if not err then
       error(dbtype)
   end
   local sqlite_conn
   if dbtype == "4" then
      local err,sqlite_path = pcall(conf.get,conf,confpath .. "SVC/DBConnection/_UNIONSELECTOR/SQLITE/DBFile")
      if not err then
          error(sqlite_path)
      end
      err,sqlite_conn = pcall(dbaccess.connect,"SQLite3",sqlite_path)
      if not err then
          error(sqlite_conn)
      end
   end

   while true do
      local msg = nq:get()
      print("svc:::",#msg, msg)
      local request_header = ffi.new("APP_HEADER")
      ffi.copy(request_header, msg, ffi.sizeof(request_header))
      local content = string.sub(msg, ffi.sizeof(request_header)+1)
      pprint.pprint(#msg)
      local request_body = cjson.decode(content)


      -- local item=_router:find_by_name(request.from.name)
      -- print("router:::",pprint.format(item))
      pprint.pprint( request_header.Head.Action, "Action::")
      if request_header.Head.Action == ffi.C.ACT_REPORT then
         local err, route_info = pcall(function () return _router:find_by_name(structs.str_pack(request_header.From.Name)) end)
         if  err then
            print("router:::",pprint.format(route_info))
            pprint.pprint(_router:find_by_field("display"))
            local trade = require(route_info.app_type)
            local app = trade.Report:new()
            local logname =string.format("%s.log",route_info.app_type)
            local _logger = logger.new(logname)
            local __gala__ = {route_info=route_info, _router=_router,_logger=_logger,_db=sqlite_conn, _header = request_header}
            app:init(__gala__, request_body)
            app:Run()
            _logger:finalizer()
         else
            _router:delete(structs.str_pack(request_header.From.Name))
         end
      elseif request_header.Head.Action == ffi.C.ACT_REQUEST then
         local route_info = _router:find_by_name(structs.str_pack(request_header.From.Name))
         print("router:::",pprint.format(route_info))
         print(structs.str_pack(request_header.To.AppType))
         local trade = require(structs.str_pack(request_header.To.AppType))
         local app = trade.Request:new()
         local logname =string.format("%s.log",route_info.app_type)
         local logger = logger.new(logname)
         --local sqlite_path = configure.svc.db.sqlite
         -- local sqlite_env = dbaccess.AC_Environment:new()
         -- local sqlite_conn = sqlite_env:AC_Connect("SQLite3",sqlite_path)
         local __gala__ = {route_info=route_info,logger=logger,db_connection=sqlite_conn, _router=_router,  _header = request_header}
         app:init(__gala__, request_body)
         app:Run()
         logger:finalizer()
      end

   end
   -- close db connect
   sqlite_conn:close()
end


