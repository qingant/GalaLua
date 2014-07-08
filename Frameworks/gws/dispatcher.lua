--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    GalaLua Web Server dispatcher
]]

module(..., package.seeall)

local base = require("core.dispatcher").server
local pprint = require("pprint")
local dispatcher = base:new()
local rpc = require("core.rpc")
server = dispatcher

function dispatcher:new(app_name, pool_name, params)
    base.init(self, app_name, pool_name)
    self._port = params.port or 80
    self._host = params.host or "0.0.0.0"
    self._worker = rpc.create_client("no client")
    return self
end

function dispatcher:on_init()
    if self._fd == nil then
        self._fd = assert(glr.net.tcp_server(self._host, self._port))
    end
    self._logger:info("HTTP server(%s:%d) listen", self._host, self._port)
    glr.net.accept_poll(self._fd)
end

function dispatcher:on_message(mtype, desc, msg)
    if mtype == glr.IOCP then
        local fd = self._unpacker(msg)
        local worker = self._pool:call("get").result
        self._logger:debug("get worker:%s", pprint.format(worker))
        self._worker._server_addr = worker
        self._worker:call("add_task", {fd = fd})
    else
        -- TODO: loggging
    end
    glr.net.accept_poll(self._fd)
end
