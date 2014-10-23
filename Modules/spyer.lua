module(..., package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]
local os = require "os"
local osext = require "osext"
local cjson = require "cjson" 
local pprint = require "pprint"
local string =require  "string"

function main()
    --[[
    --  receive protocol:
    --  1) for getting some informations
    --  {
    --      Type:NODE
    --      Action:GET
    --      Cmd:gpid/status/pid
    --      Gpids:[0,1]/...
    --      ToAddr:{host:,port:,gpid:}
    --      Nonstop:true/false   --directly send to ToAddr or backtrack to sender(ctr)
    --  }
    --
    --  2) for run Cmds
    --  {
    --      Type:NODE
    --      Action:EXEC
    --      Cmd:kill
    --  }
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
        local addr=addr.addr
        msg_table =cjson.decode(msg) 

        if msg_table.Type=="NODE" then
            if msg_table.Action=="EXEC" then
                if msg_table.Cmd=="kill" then
                    glr.exit()
                end
            elseif msg_table.Action=="GET" then
                local gpids=assert(glr.status.processes())
                pprint.pprint(gpids,"status::gpids")
                local msg={Type="NODE",Action="RES",Cmd=msg_table.Cmd}

                -- want to take the direct train?
                if msg_table.Nonstop then 
                    addr=msg_table.ToAddr
                else
                    msg.ToAddr=msg_table.ToAddr
                end

                pprint.pprint(addr,"AAAAAAA")
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
                elseif msg_table.Cmd=="pid" then
                    local content={pid=osext.getpid()}
                    msg.Content=content
                    glr.send(addr,cjson.encode(msg))
                else
                    msg.Content={"Not valid Command"}
                    glr.send(addr,cjson.encode(msg))
                end
            else
                pprint.pprint(msg_table,"Get Unkown Action Message")
                glr.send(addr,cjson.encode({content='Unkown Action',msg=msg}))  
            end
        else
            pprint.pprint(msg_table,"Get Unkown Message")
            glr.send(addr,cjson.encode({content='Unkown Message',msg=msg}))  
        end
    end
end
