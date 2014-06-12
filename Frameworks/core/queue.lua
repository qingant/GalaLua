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

function queue_server(name)
    local queue_server = rpc.server:new():init(name)
    queue_server._dataq = _queue:new()
    queue_server._customerq = _queue:new()
    queue_server._dequeue_timeout = 300
    queue_server._data_timeout = 30000
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
        if self._customerq:is_empty() then
            self._dataq:push_right(params)
        else
            local customer = self._customerq:pop_left()
            self:_send(customer, params)
        end
        return {status="ok"}
    end
    function queue_server:on_timeout()
    end
    queue_server:main()
end



function create_queue(name)
    if not glr.npt.registered(name) then
        glr.spawn(_NAME, "queue_server", name)
    end
    print("name", name)
    local client = rpc_client.client:new():init(name)
    return client
end