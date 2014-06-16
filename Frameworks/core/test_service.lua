--[[
	Author : Ma Tao
	Date   : 2014/04/16

	== Introduction ==
    Test server
]]
module(..., package.seeall)

local rpc_server = require("rpc").server

function test_server(name)
    local server = rpc_server:new():init(name)
    function server:test(params, desc)
        return {stamp = glr.time.now(), params = params, source = desc}
    end
    server:main()
end