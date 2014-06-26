--[[
	Author : Ma Tao
	Date   : 2014/06/23

	== Introduction ==
    GLR process pool

]]

module(..., package.seeall)

local queue = require(_PACKAGE .. "queue").queue
local rpc = require(_PACKAGE .. "rpc")
local pool = queue:new()

server = pool

function pool:new(app_name, pool_name, dispatcher, worker, min, max)
    local full_name = string.format("%s.%s", app_name, pool_name)
    self:init(full_name)
    self._timeout = timeout
    self._app_name = app_name
    self._pool_name = pool_name
    self._min = min or 32
    self._max = max or 1024
    self._worker = worker
    self._dispacher = dispatcher
    self:_init()
    return self
end
function pool:_init()
    self._count = 0
    local sup_id = string.format("%s.supervisor", app_name)
    self._sup = rpc.create_client(sup_id)
    self._start_argv_template = {
        process_type = "gen",
        process_params = {
            mod_name = self._worker,
            group = self._pool_name,
            parameters = {self._app_name, self._pool_name, self._count},
        },
    }
    -- start dispatcher
    self._sup:call("start_process", {
                       process_type = "gen",
                       process_params = {
                           mod_name = self._dispacher,
                           group = self._pool_name,
                           parameters = {app_name, pool_name},
                       }
    })

    for i = 1, self._min do
        self._count = self._count + 1
        self:_create_process()
    end
end
function pool:get_process(params, desc)
    local p = self:get(params, desc)
    if p == self.no_response and self._count < self._max then
        pool:_create_process()
    end
    return p
end
function pool:on_timeout()
    -- TODO: timeout
end
function pool:_create_process()
    local rt = self._sup:call("start_process", self._start_argv_template)
    -- TODO: error handling and logging
end

