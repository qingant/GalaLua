module(..., package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]
local os = require "os"


local cjson = require "cjson" 
local pprint = require "pprint"
local configure = require "configure"
local ffi = require "ffi"
local structs = require "structs"
local string =require  "string"

function main()
    --[[
    --  receive protocol:
    --  {
    --      Type:NODE
    --      Action:GET
    --      Cmd:gpid/status
    --      Gpids:[0,1]/...
    --      ToAddr:{host:,port:,gpid:}
    --  }
    --
    --  response protocol:
    --  {
    --      Type:NODE
    --      Action:RES
    --      ToAddr:         (recv_msg.ToAddr)
    --      Content:{}        (what you request)
    --  }
    --]]

    while true do
        print("STATUS_SERVER:")
        local msg_type, addr, msg = glr.recv()
        msg_table =cjson.decode(msg) 

        if msg_table.Type=="NODE" and msg_table.Action=="GET" then
            local err,gpids=glr.status.processes()
            assert(err,gpids)
            pprint.pprint(gpids,"status::gpids")
            local msg={Type="NODE",Action="RES",ToAddr=msg_table.ToAddr}

            if msg_table.Cmd=="gpid" then
                msg.Content=gpids
                glr.send(addr,cjson.encode(msg))
            elseif msg_table.Cmd=="status" then
                local gpid_list=msg_table.Gpids or gpids
                if #gpid_list==0 then
                    gpid_list=gpids 
                end
                local content={}
                for i,gpid in pairs(gpid_list) do
                    err,content[tostring(gpid)]=glr.status.status(gpid)
                end
                msg.Content=content
                pprint.pprint(msg,"RESPONSETOCNR")
                glr.send(addr,cjson.encode(msg))
            else
                msg.Content={"Not valid Command"}
                glr.send(addr,cjson.encode(msg))
            end
        else
            print("Hey,You should not send messages to me !")
            glr.send(addr,"{'content':'Hey,You should not send messages to me !'}")  
        end
    end
end
