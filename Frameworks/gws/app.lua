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
    self.components[1].params.host = host
    self.components[1].params.port = port
end
function app:set_urls(urls)
    self.components[1].params.urls = urls
end
function app:set_session_storage_path(path)
    self.components[1].params.session_storage_path = path
end
function app:set_static_path(path)
    self.components[1].params.static_path = path
end
function app:set_log_path(path)
    os.execute("mkdir -p " .. path)
    self.log_path = path
    self.components[1].params.log_path = path .. "/web/"
    os.execute("mkdir -p " .. self.components[1].params.log_path)
end
function app:set_pool(max, min)
    if max then
        self.components[1].params.max = max
    end
    if min then
        self.components[1].params.min = min
    end
end
app:add_component{
    catagory = "pool",
    pool_name = "http",
    dispatcher = _PACKAGE .. "dispatcher",
    worker = _PACKAGE .. "processor",
    params = {
        port = 8080,
        min = 1,
        max = 4096,
        urls = nil
    }
}

