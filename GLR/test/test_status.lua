module(...,package.seeall)
local cjson = require "cjson"
local pprint = require "pprint"
local des_host
local des_port
local des_addr
function test_basic( ... )
    des_host = glr.get_option("des_host")
    des_port = glr.get_option("des_port")
    des_addr = {host = des_host, port = des_port, gpid = 1}
    local req = {Type = "NODE", Action = "GET", Cmd = "status", ToAddr = glr.sys}
    glr.connect(des_host, des_port)
    glr.send(des_addr, cjson.encode(req))
    local _,_,result = glr.recv()
    print("BASIC TEST", result)
end

if ... == "__main__" then
    test_basic()
end
