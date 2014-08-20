--[[
	Author : Liu Wenxue
	Date   : 2014/08/14

	== Introduction ==
    test glr mutil process

    == TODO ==
    logging
]]

module(...,package.seeall)
local pprint = require("pprint").pprint
local httpClient = require("http_client").httpClient
local httpRequest = require("http_client").httpRequest
local cmsgpack = require("cmsgpack")
local packer = cmsgpack.pack
local unpack = cmsgpack.unpack

local _queue = require("collections.init").queue

queue_server = {}

function queue_server:new()
    o = {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function queue_server:init()
    self._customerq = _queue:new()
    self._dequeue_timeout = 300
    self._data_timeout = 30000
    return self
end

function queue_server:get()
    self._customerq:pop_left()
end

function queue_server:put(desc)
    self._customerq:push_right(desc)
end

function queue_server:call(mod_name,entry,...)
    local ret, err = proc.spawn(mod_name, entry,...)
    if err then
        pprint(err,"err")
    end
end

pool = {}

function pool:new()
    o = {}
    setmetatable(o,self)
    self.__index = self
    return o
end


--params = { ["timeout"] = 30, ["min"] = 32, ["max"]=1024 }
function pool:init(sup,params)
    self._sup = sup

    self._count = 0
    self._min = params.min or 10
    self._max = params.max or 1000
    self._step = params.setp or 10

    self._timeout = params.timeout

    self.queue = queue_server:new():init()

    for i = 1, self._min do
        self:_create_process(self._sup)
    end

    return self
end


function pool:get_process()
    --local p = self.queue:get()
    print("call get_process")
    --local p = self.queue._customerq:pop_left()
    local p = self.queue:get()
    print("queue object",p)
    if not p  and self._count < self._max then
        print("p is nil")
        self:_create_process(self._sup)

        --TODO step
        --for i  = 1, self._step do
        --    self:_create_process(self._sup)
        --end
        --p = self.queue:get()
        p = self.queue._customerq:pop_left()
    end
    return p
end

function pool:_create_process(sup)
    print("call create_process")
    self.queue:put(sup)
    self._count = self._count + 1
    print("count",self._count)
    -- TODO: error handling and logging
end

function pool:call(mod_name,entry,...)
    local ret, err = self._sup.spawn(mod_name, entry,...)
    if err then
        pprint(err,"err")
    end
end

function pool:capacity()
    return self._max
end
