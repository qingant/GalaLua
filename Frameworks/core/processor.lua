--[[
	Author : Ma Tao
	Date   : 2014/06/23

	== Introduction ==
    GLR dispatcher base

    == 讨论 ==

    处理器作为一种抽象,其要点在于如何驱动,即如何获取一个任务并且合适地
    执行它,而这总是多变的,在一般的RPC系统中,处理器一次处理一个请求,在通
    常的网络应用中,处理器长期地处理一个连接上的多个请求,因此,我们暂时无
    法对其提供更多的抽象.

]]

module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")

local processor = rpc.server:new()

server = processor

function processor:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function processor:init(app_name, pool_name, idx)
    rpc.server.init(self, nil)
    self._app_name = app_name
    self._pool_name = pool_name
    self._pool = rpc.create_client(string.format("%s.%s", app_name, pool_name))
    self._idx = idx
    self._id = {host=glr.sys.host, port=glr.sys.port, gpid=__id__}
    self._logger = require(_PACKAGE .. "logger").logger:new():init(self)
    return self
end
function processor:add_task(params, desc)
    self._logger:debug("add_task")
    if self.on_task_add then
        return self:on_task_add(params, desc)
    end
    -- TODO: error handling
end
function processor:_back_to_pool()
    self._pool:request("put", self._id)
end