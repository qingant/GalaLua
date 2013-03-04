local configure=require("configure")
package.path=configure.svc.PATH
package.cpath=configure.CPATH

local node = require "node"
local struct = require "struct"
local pprint = require "pprint"
local cjson = require "cjson"
local amq = require "amq"
local router = require "router"

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
           local __gala__ = {route_info=route_info, _router=_router}
           app:init(__gala__, request)
           app:Run()
        elseif request.header.to.action == "request" then
           local route_info = _router:find_by_name(request.header.from.name)
           print("router:::",pprint.format(route_info))
           local trade = require(route_info.app_type)
           local app = trade.Request:new()
           local __gala__ = {route_info=route_info}
           app:init(__gala__, request)
           app:Run()
        end
    
        -- if not msg_table["from"] then 
            
        -- to_agent_worker={
        --     header={
        --         from="svc"
        --     },
        --     content={
        --         name="test_svc",
        --         code="print(\"hello,this is svc\")"
        --     }
        -- }

        -- local addr=item["addr"]
        -- node.send(addr["host"],addr["port"],addr["gpid"],cjson.encode(to_agent_worker))
    end
end

    -- --register message 
    --     function register()
    --     --[[
    --     {
    --         header= {
    --             from = { 
    --                 type = agent,
    --                 host = bla,
    --                 port = bla,
    --                 gpid = bla,
    --             }
    --             to={
    --                 type = svc,
    --                 action = register,
    --                 name = "ddd",
    --             }
    --         },
    --         content={
    --         }
    --     }
    --     --]]
            

    --     end
    --     --report message 
    --     function report()
    --     end
        