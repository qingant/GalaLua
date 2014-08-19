--[[
	Author : Ma Tao
	Date   : 2014/06/23

	== Introduction ==
    GLR dispatcher base

]]

module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")
local logger = require(_PACKAGE .. "logger").logger
local dispatcher = rpc.server:new()

server = dispatcher

function dispatcher:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function dispatcher:init(app_name, pool_name)
    local full_name = string.format("%s.%s.dispatcher", app_name, pool_name)
    rpc.server.init(self, full_name)
    self._app_name = app_name
    self._pool_name = pool_name
    self._id = {host=glr.sys.host, port=glr.sys.port, gpid=__id__}
    self._logger = logger:new():init(self)
    self._pool = rpc.create_client(string.format("%s.%s", app_name, pool_name))
    return self
end

function dispatcher:_back_to_pool()
    self._pool:request("put_dispatcher", self._id)
end
function dispatcher:on_init()
    error("not implemented")
end

function dispatcher:on_message(...)
    error("not implemented")
end