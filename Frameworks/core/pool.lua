--[[
	Author : Ma Tao
	Date   : 2014/06/23

	== Introduction ==
    GLR process pool

]]

module(..., package.seeall)

local queue = require(_PACKAGE .. "queue").server
local rpc = require(_PACKAGE .. "rpc")
local pool = queue:new()
local logger = require(_PACKAGE .. "logger").logger
server = pool

function pool:new(app_name, pool_name, dispatcher, worker, params)
    local full_name = string.format("%s.%s", app_name, pool_name)
    glr.npt.register(full_name)
    self._logger = logger:new():init(self)
    self._logger:info("%s init", full_name)
    self._logger:info("with: %s %s", dispatcher, worker)
    self._timeout = timeout
    self._app_name = app_name   -- XXX: this is the app-instance name
    self._pool_name = pool_name
    self._min = params.min or 32
    self._max = params.max or 1024
    self._worker = worker
    self._dispatcher = dispatcher
    self._params = params
    self:_init()
    return self
end
function pool:_init()
    self._count = 0
    -- glr.time.sleep(2)
    local sup_id = string.format("%s.supervisor", self._app_name)
    self._logger:info("supervisor: %s", sup_id)
    if not glr.npt.registered(sup_id)[sup_id] then
        print("Not registered!")
    end
    self._sup = rpc.create_client(sup_id)
    self._start_argv_template = {
        process_type = "gen",
        process_params = {
            mod_name = self._worker,
            group = self._pool_name,
            parameters = {self._app_name, self._pool_name, self._count, self._params},
        },
    }


    for i = 1, self._min do
        self._logger:info("start processor:%d of %d", i, self._min)
        self._count = self._count + 1
        self:_create_process()
    end

    -- start dispatcher
    local rt = self._sup:call("start_process", {
                                  process_type = "gen",
                                  process_params = {
                                      mod_name = self._dispatcher,
                                      group = self._pool_name,
                                      parameters = {self._app_name, self._pool_name, self._params},
                                  }
    })
    self._logger:info("dispatcher started:%s", self._logger.format(rt, "rt"))
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
    self._logger:info("start processor:%d %s", self._count, self._logger.format(rt))
    -- TODO: error handling and logging
end

