--[[
	Author : Ma Tao
	Date   : 2014/04/16

	== Introduction ==
	GLR Service Client

]]

__VER__ = "1.0"

module(..., package.seeall)

local cjson = require("cjson")
local pprint = require("pprint")

client = {}

function client:new(...)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function client:init(server_addr)
    -- TODO: name resolution
    self._server_addr = server_addr
    self._id = 1
    return self
end

function client:call(method, params)
    self._id = self._id + 1
    glr.send(self._server_addr, cjson.encode{ method = method,
                                              params = params,
                                              id = self._id
                                            })
    -- TODO: matching recv
    local type, addr, msg = glr.recv_by_addr(self._server_addr)
    return cjson.decode(msg)
end