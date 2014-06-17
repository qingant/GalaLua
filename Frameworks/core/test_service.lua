--[[
	Author : Ma Tao
	Date   : 2014/04/16

	== Introduction ==
    Test server
]]
module(..., package.seeall)

local rpc_server = require("rpc").server

server = rpc_server:new()
function server:new(name)
    self:init(name)
    return self
end
function server:test(params, desc)
    return {stamp = glr.time.now(), params = params, source = desc}
end

