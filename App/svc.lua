local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local configure=require("configure")
package.path=configure.svc.PATH
package.cpath=configure.CPATH

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
local logger = require "logger"
local dbaccess = require "dbaccess"

local RID = "@router"
local MQID = "@mq"


function main()
   --node.register("0.0.0.0", 2345)
   local _router,_amq
   function init_fake_global_var()
      _router=router.new("router")
      glr.global(RID,_router, "Router4Lua")
      _amq = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
      glr.global(MQID, _amq, "CGalaxyMQ");
   end
   init_fake_global_var()

   local err,status_server = glr.spawn(os.getenv("HOME") .. "/lib/lua/status.lua", "main")
   print("Status :::::" .. status_server)

   local processes = {}
   local err

   for i=0,configure.svc.SVC_MAX do
      err,processes[i]= glr.spawn(configure.LUA_HOME .. "svc.lua","worker")
   end

   while true do
      msg = glr.recv()
      ---do something here
      print(msg)
   end
end

function worker()
   local _router = glr.get_global(RID)
   local _amq = glr.get_global(MQID)
   
   local nq = _amq:NQArray():get(0)
   
   -- sqlite access connection create
   
   local dbtype = configure.svc.db.DB_Type
   local sqlite_conn
   if dbtype == "SQLite3" then
      local sqlite_path = configure.svc.db.Connection.Path
      sqlite_conn = dbaccess.connect(dbtype,sqlite_path)
      --local sqlite_env = dbaccess.AC_Environment:new()
      --local sqlite_conn = sqlite_env:AC_Connect("SQLite3",sqlite_path)
   end
   while true do
      local msg = nq:get()
      print("svc:::",msg)
      local request = cjson.decode(msg)   

      -- local item=_router:find_by_name(request.from.name) 
      -- print("router:::",pprint.format(item))
      
      if request.header.to.action == "report" then
         local route_info = _router:find_by_name(request.header.from.name)
         print("router:::",pprint.format(route_info))
         pprint.pprint(_router:find_by_field("display"))
         local trade = require(route_info.app_type)
         local app = trade.Report:new()
         local logname =string.format("%s.log",route_info.app_type)
         local _logger = logger.new(logname)	   
         local __gala__ = {route_info=route_info, _router=_router,_logger=_logger,_db=sqlite_conn}
         app:init(__gala__, request)
         app:Run()
         _logger:finalizer()
      elseif request.header.to.action == "request" then
         local route_info = _router:find_by_name(request.header.from.name)
         print("router:::",pprint.format(route_info))
         local trade = require(route_info.app_type)
         local app = trade.Request:new()
         local logname =string.format("%s.log",route_info.app_type)
         local logger = logger.new(logname)
         local sqlite_path = configure.svc.db.sqlite
         -- local sqlite_env = dbaccess.AC_Environment:new()
         -- local sqlite_conn = sqlite_env:AC_Connect("SQLite3",sqlite_path)
         local __gala__ = {route_info=route_info,logger=logger,db_connection=sqlite_conn, _router=_router}
         app:init(__gala__, request)
         app:Run()
         logger:finalizer()
      end
      
   end
   -- close db connect
   sqlite_conn:close()
end


