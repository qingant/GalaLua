--[[
	Author : Ma Tao
	Date   : 2014/04/16

	== Introduction ==
	GLR Service Client

]]

_VER = "1.0"

module(..., package.seeall)

local cjson = require("cjson")
local cmsgpack = require("cmsgpack")
local pprint = require("pprint").pprint
local RPC_CORRID = require(_PACKAGE .. "const").RPC_CORRID

client = {}

function client:new(...)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    self._packer = cmsgpack.pack
    self._unpacker = cmsgpack.unpack
    return o
end

function client:init(server_addr)
    -- TODO: name resolution
    self._server_addr = server_addr
    self._id = 1
    return self
end

function client:call(method, params)
    self:request(method, params)
    -- TODO: matching recv
    local mtype, desc, msg = glr.recv_by_addr(self._server_addr)
    -- pprint(desc, "call")
    return self._unpacker(msg)
end

function client:request(method, params)
    self._id = self._id + 1
    glr.send(self._server_addr, self._packer{ method = method,
                                              params = params,
                                              id = self._id
                                            },
             {corrid = RPC_CORRID})
end