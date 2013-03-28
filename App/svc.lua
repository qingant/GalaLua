module(..., package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local configure=require("Configure")
--package.path=configure.svc.PATH
--package.cpath=configure.CPATH

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
   --node.register("0.0.0.0", 2345)
   local _router,_amq
   function init_fake_global_var()
--      _router=router.new("router")
--      glr.global(RID,_router, "Router4Lua")
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
   for i=1,100 do
      local err, worker = glr.spawn("svc", "worker", __id__)
      collections.List.pushright(workers, worker)
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
   while true do
      local msg = nq:get()

      if collections.List.empty(workers) then
         collect_idle()
      end
      if collections.List.empty(workers) then
         local err, worker = glr.spawn("svc", "worker", __id__)
         collections.List.pushright(workers, worker)
      end
      local worker = collections.List.popleft(workers)
      glr.send(worker, msg)
   end
end

function worker(dispatcherid)
   local _router=router.router()
   local conf = glr.get_global(MTCFGID)
   local err,dbtype = pcall(conf.get,conf,basepath .. "SVC/DBConnection/_SCROLLTYPE")
   if not err then
       error(dbtype)
   end
   local sqlite_conn
   if dbtype == "4" then
      local err,sqlite_path = pcall(conf.get,conf,basepath .. "SVC/DBConnection/_UNIONSELECTOR/SQLITE/DBFile")
      if not err then
          error(sqlite_path)
      end
      err,sqlite_conn = pcall(dbaccess.connect,"SQLite3",sqlite_path)
      if not err then
          error(sqlite_conn)
      end
   end

   while true do
      local msg = glr.recv()
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

         -- print("router:::",pprint.format(route_info))
         pprint.pprint(_router:find_by_field("display"))
         local app_type = structs.str_pack(request_header.To.AppType)
         local trade = require(app_type)
         local app = trade.Report:new()
         local logname =string.format("%s.log",app_type)
         local _logger = logger.new(logname)
         local __gala__ = {_router=_router,_logger=_logger,_db=sqlite_conn, _header = request_header}
         app:init(__gala__, request_body)
         app:Run()
         _logger:finalizer()

      elseif request_header.Head.Action == ffi.C.ACT_REQUEST then
         local route_info = _router:find_by_name(structs.str_pack(request_header.From.Name))
         print("router:::",pprint.format(route_info))
         print(structs.str_pack(request_header.To.AppType))
         local trade = require(structs.str_pack(request_header.To.AppType))
         local app = trade.Request:new()
         local logname =string.format("%s.log",route_info.app_type)
         local logger = logger.new(logname)
         local sqlite_path = configure.svc.db.sqlite
         local __gala__ = {route_info=route_info,logger=logger,db_connection=sqlite_conn, _router=_router,  _header = request_header}
         app:init(__gala__, request_body)
         app:Run()
         logger:finalizer()
      end
      glr.send(dispatcherid, cjson.encode({gpid = __id__}))
   end
   sqlite_conn:close()
end


