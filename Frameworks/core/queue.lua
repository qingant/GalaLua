--[[
	Author : Ma Tao
	Date   : 2014/06/01

	== Introduction ==
	Queue implemented with GLR primitives

]]

module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")
local rpc_client = require(_PACKAGE .. "rpc_client")
local _queue = require("collections.init").queue


local queue_server = rpc.server:new()
server = queue_server
--[[
    这个new函数不同于常见的new函数，是因为在一个lua环境中只会存在一个这样的实例。
]]
function queue_server:new(name)
    self:init(name)
    self._dataq = _queue:new()
    self._customerq = _queue:new()
    self._dequeue_timeout = 300
    self._data_timeout = 30000
    return self
end
function queue_server:get(params, desc)
    if self._dataq:is_empty() then
        self._customerq:push_right(desc)
        return self.no_response
    else
        return self._dataq:pop_left()
    end
end
function queue_server:set_options(params, desc)
    if params.timeout and type(params.timeout) == "number" then
        self._dequeue_timeout = params.timeout
    end
    return {timeout=self._dequeue_timeout}
end
function queue_server:get_options(params, desc)
    return {timeout=self._dequeue_timeout}
end
function queue_server:status(params, desc)
    return {dataq=self._dataq, customerq=self._customerq, timeout=self._dequeue_timeout}
end
function queue_server:put(params, desc)
    if self.put_process then
        self:put_process(params, desc)
    end
    self._logger:info("put by %d", desc.addr.gpid)
    self._logger:info("data size: %d", self._dataq:length())
    if self._customerq:is_empty() then
        self._dataq:push_right(params)
    else
        local customer = self._customerq:pop_left()
        self:_send(customer, params)
    end
    return self.no_response
end
function queue_server:on_timeout()
end

