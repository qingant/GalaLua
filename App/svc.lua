module(..., package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"


local configure=require("Configure")

local config = require("configure")
package.path=config.svc.PATH
package.cpath=config.CPATH
print(config.svc.PATH)
local node = require "node"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"
local logger = require "logger"
local dbaccess = require "dbaccess"
local structs = require "structs"
local ffi = require "ffi"
local collections = require("collections")

local RID = "@router"
local MQID = "@mq"
local MTCFGID = "@conf"

local confdir = os.getenv("HOME") .. "/cfg/shmfile"
local conffile = "/node.conf"
local basepath = "/Configure/_STRUCTURE/MonitorList[00]/Monitor/"
local mode = "w"

function main()
   local _router,_amq
   function init_fake_global_var()
      _amq = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
      glr.global(MQID, _amq, "CGalaxyMQ");
   end
   init_fake_global_var()

   local err,status_server = glr.spawn("status", "main")
   print("Status :::::" .. status_server)

   local conf = configure.new(confdir .. conffile,mode)
   glr.global(MTCFGID,conf)

   local err,dispatcher = glr.spawn("svc","dispatcher")
   print("svc dispatcher :::::" .. dispatcher)

   local msg_table = ffi.new("BIND_MSG")
   while true do
       print("This SVC>>>>>>>")
        local msg_type, addr, msg= glr.recv()
       print("This SVC recv over >>>>>>>")
        pprint.pprint(addr)
        ffi.copy(msg_table, msg, ffi.sizeof(msg_table))
        if msg_table.Catagory==ffi.C.DEV_MONITOR then
            msg_table.Gpid = ffi.C.htonl(status_server)
            msg_table.Head.Action = ffi.C.ACT_ACK
            if not glr.send(addr, structs.pack(msg_table)) then
                pprint.pprint("Send Failure")
            end
        end
   end
end

function dispatcher()
   local _amq = glr.get_global(MQID)
   local nq = _amq:NQArray():get(0)

   local workers = collections.List.new()
   local conf = glr.get_global(MTCFGID)
   local err,threads = pcall(conf.get,conf,basepath .. "SVC/Threads")
   if not err then
       error(threads)
   end
   local thread_total = 0
   for i=1,tonumber(threads) do
      local err, worker = glr.spawn("svc", "worker", __id__)
      collections.List.pushright(workers, worker)
      thread_total = thread_total + 1
   end
   function collect_idle()
      while glr.msg_available() do
         local msg_type, addr, msg = glr.recv()
         if msg_type ~= glr.CLOSED then
            local p = cjson.decode(msg)
            collections.List.pushright(workers, p.gpid)
         end
      end
   end
   local number = 0

   while true do
      local msg = nq:get()
      number = number + 1
      print("Number", number)
      if collections.List.empty(workers) then
         collect_idle()
      end
      while(thread_total == config.svc.THREAD_MAX) and
        collections.List.empty(workers) do
         local msg_type, addr, msg = glr.recv()
         if msg_type ~= glr.CLOSED then
            local p = cjson.decode(msg)
            collections.List.pushright(workers, p.gpid)
         end
      end
      if collections.List.empty(workers) then
         local err, worker = glr.spawn("svc", "worker", __id__)
         collections.List.pushright(workers, worker)
         thread_total = thread_total + 1
      end
      local worker = collections.List.popleft(workers)
      glr.send(worker, msg)
   end
end

function db_conn(config)
   local err,dbtype = pcall(config.get,config,basepath .. "SVC/DBConnection/_SCROLLTYPE")
   if not err then
       error(dbtype)
   end
   local dbconn
   if dbtype == "4" then
      local err,sqlite_path = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/SQLITE/DBFile")
      if not err then
          error(sqlite_path)
      end
      err,dbconn = pcall(dbaccess.connect,"SQLite3",sqlite_path)
      if not err then
          error(dbconn)
      end
   elseif dbtype == "1" then
      local err,dbname = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/ORACLE/Name")
      if not err then
         error(dbname)
      end
      local err,usr = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/ORACLE/User")
      if not err then
         error(usr)
      end
      local err,passwd = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/ORACLE/Passwd")
      if not err then
         error(passwd)
      end
      err,dbconn = pcall(dbaccess.connect,"Oracle",dbname,usr,passwd)
      if not err then
          error(dbconn)
      end
   elseif dbtype == "2" then
      local err,dbname = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/DB2/Name")
      if not err then
         error(dbname)
      end
      local err,usr = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/DB2/User")
      if not err then
         error(usr)
      end
      local err,passwd = pcall(config.get,config,basepath .. "SVC/DBConnection/_UNIONSELECTOR/DB2/Passwd")
      if not err then
         error(passwd)
      end
      err,dbconn = pcall(dbaccess.connect,"DB2",dbname,usr,passwd)
      if not err then
          error(dbconn)
      end
   end
   return dbconn
end

function request_handle(header)
   local app_type = structs.str_pack(header.To.AppType)
   local trade,app
   if header.Head.Action == ffi.C.ACT_REPORT then
      trade = require(app_type .. ".server.report")
      print(app_type .. ".server.report")
      app = trade.Report:new()
   elseif header.Head.Action == ffi.C.ACT_REQUEST then
      trade = require(app_type .. ".server.request")
      print(app_type .. ".server.request")
      app = trade.Request:new()
   elseif header.Head.Action == ffi.C.ACT_SUBSCRIB then
      trade = require(app_type .. ".server.subscrib")
      print(app_type .. ".server.subscrib")
      app = trade.Subscrib:new()
   elseif header.Head.Action == ffi.C.ACT_CONTROL then
      trade = require(app_type .. ".server.control")
      print(app_type .. ".server.control")
      app = trade.Control:new()
   end
   return app
end

function worker(dispatcherid)
--   local _router = glr.get_global(RID)
   local _router=router.router()
   local _amq = glr.get_global(MQID)

   local nq = _amq:NQArray():get(1)

   local conf = glr.get_global(MTCFGID)
   local dbconn = db_conn(conf)

   package.path=config.svc.PATH

   while true do
      local msg_type, addr, msg = glr.recv()
      local request_header = ffi.new("APP_HEADER")
      ffi.copy(request_header, msg, ffi.sizeof(request_header))
      local content = string.sub(msg, ffi.sizeof(request_header)+1)
      local request_body = cjson.decode(content)
      local item = _router:find_by_name(structs.str_pack(request_header.From.Name))
      pprint.pprint( request_header.Head.Action, "Action::")

      local app = request_handle(request_header)
      local logname =string.format("%s.log",structs.str_pack(request_header.To.AppType))
      local _logger = logger.new(logname)
      local __gala__ = {_router=_router,_logger=_logger,_db=dbconn, _header = request_header, _router_info = item}
      app:init(__gala__, request_body)
      app:Run()
      _logger:finalizer()
      glr.send(dispatcherid, cjson.encode({gpid = __id__}))
   end
   dbconn:close()
end


