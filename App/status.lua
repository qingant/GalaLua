local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"

local cjson = require "cjson" 
local node = require "node"
local pprint = require "pprint"
local configure = require "configure"
local timer = require "timer"
local ffi = require "ffi"
local structs = require "structs"
local string =require  "string"


--function main()
--   while true do
--      local err,msg = glr.recv()
--      if err then
--         local request = cjson.decode(msg)
--         print(request)
--         local response
--         host = request.host
--         port = request.port
--         src = request.src
--         command = request.command
--         if command == "all" then
--            err,response = glr.all()
--         elseif command == "status" then
--            err,response = glr.status(request.pid)
--         end
--         local err = node.send(host,port, src, cjson.encode(response))
--         if err then
--         else
--            error("send error")
--         end
--      end
--   end
--end

function main()
    while true do
        local msg_type, addr, msg = glr.recv()

        msg_table = ffi.new("MONITOR_HEADER")
        local len=ffi.sizeof(msg_table)
        ffi.copy(msg_table, msg,len)
        arg=cjson.decode(msg:sub(len+1))
        if msg_table.Type==ffi.C.NODE and msg_table.Action==ffi.C.GET then
            local err,gpids=glr.all()
            assert(err,gpids)
            pprint.pprint(gpids,"status::gpids")
            if arg["gpid"] then
                node.send(addr.host,addr.port,addr.gpid,cjson.encode(gpids))
            elseif arg["status"] then
                local l=arg["status"]
                pprint.pprint (l)
                if #l==0 then
                    l=gpids 
                end
                local s={}
                for i,v in pairs(l) do
                    err,s[tostring(v)]=glr.status(v)
                end
                node.send(addr.host,addr.port,addr.gpid,cjson.encode(s))
            end
        else
            print("Hey,You should not send messages to me !")
            node.send(addr.host,addr.port,addr.gpid,"{'content':'Hey,You should not send messages to me !'}")  
        end
    end
end
