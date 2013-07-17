--[[
-- supervisor test example
--
--]]

module(...,package.seeall)
local pprint=require "pprint"
local string=require "string"

function main()
    print(("*"):rep(40))
    print("Testing For Supervisor...")
    print(string.format("I am listening on %s:%d",glr.sys.host,glr.sys.port))
    print(("*"):rep(40))
    while true do
        local msg_type,addr,msg=glr.recv()
        if msg_type==glr.CLOSED then
            print("Process Close:",addr.host)
        else
            print(("*"):rep(40))
            pprint.pprint(addr,"get message from:")
            print("MSG:",msg)
            print(("*"):rep(40))
        end
    end
end
