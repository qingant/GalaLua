--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    GalaLua Web Server

]]

module(..., package.seeall)
local modular = require("core.modular")

app = modular.inherit()
function app:on_load(mgr)
    mgr._logger:info("App:%s load!", _NAME)
end

function app:on_started(mgr)
    mgr._logger:info("App:%s started!", _NAME)
end

function app:set_addr(host, port)
    self._host = host
    self._port = port
end
function app:set_urls(urls)
    self._urls = urls
end

app:add_component{
    catagory = "pool",
    pool_name = "http",
    dispatcher = _PACKAGE .. "dispatcher",
    worker = _PACKAGE .. "processor",
    params = {
        port = app._port or 8080,
        min = 1,
        max = 4096,
        urls = app._urls
    }
}

