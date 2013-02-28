local configure=require("configure")
package.path=configure.PATH
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
       err,processes[i]= glr.spawn("svc.lua","worker")
   end

   while true do
      msg = _amq:get()
      ---do something here
      print(msg)
   end
end

function worker()
    local _router = glr.get_global(RID)
    local _amq = glr.get_global(MQID)
    
    local nq = _amq:NQArray():get(0)

    while true do

        local msg=nq:get()
        print("svc:::",msg)
        local msg_table=cjson.decode(msg)   

        local item=_router:find_by_name(msg_table["name"]) 
        print("router:::",pprint.pprint(item))
        
        --register message 
        function register()
        --[[
        {
            header= {
                from = { 
                    type = agent,
                    host = bla,
                    port = bla,
                    gpid = bla,
                }
                to={
                    type = svc,
                    action = register,
                    name = "ddd",
                }
            },
            content={
            }
        }
        --]]
            

        end
        --report message 
        function report()
        end
        
        if not msg_table["from"] then 
            
        to_agent_worker={
            header={
                from="svc"
            },
            content={
                name="test_svc",
                code="print(\"hello,this is svc\")"
            }
        }

        local addr=item["addr"]
        node.send(addr["host"],addr["port"],addr["gpid"],cjson.encode(to_agent_worker))
    end
end

