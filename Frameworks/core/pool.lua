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
    local log_path = string.format("%s/%s.%s.pool.log", params.log_path, app_name, pool_name)
    print("Pool Logging:", log_path)
    self._logger = logger:new():init(self, log_path)
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
    self._processes = {}
    self._dispatch = {}
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

function pool:put_dispatcher(params, desc)
    if params and params.gpid then
        self._dispatch.gpid = params.gpid
        self._dispatch.status = "started"
        self._dispatch.addr = params
    end
end
function pool:_get_process(params, desc)
    local p = self:get(params, desc)
    while true do
        if p.gpid and glr.status.status(p.gpid) then
            if self._processes[p.gpid] then
                return p
            else 
                self._count = self._count - 1
                self._processes[p.gpid]=nil
            end
        end
        p = self:get(params, desc)
    end
end

function pool:get_process(params, desc)
    self._logger:info("worker remain: %s", self._logger.format(self._dataq))
    p = self:_get_process(params, desc)
    if p == nil and self._count < self._max then
        pool:_create_process()
    end
    local gpid = p.gpid
    self._processes[gpid].status = "used"
    return p
end
function pool:on_timeout()
    self._logger:debug(self._logger.format(glr.status.processes()))
    self._logger:debug(self._logger.format(self._sup:call("query")))
    self._logger:flush()
end
function pool:_create_process()
    local rt = self._sup:call("start_process", self._start_argv_template)
    if rt and rt.result and rt.result.process and rt.result.process.gpid then
        local gpid = rt.result.process.gpid
        self._count = self._count + 1
        self._processes[gpid] = {}
        self._processes[gpid].status = "idle"
        self._processes[gpid].gpid = gpid
        self._processes[gpid].addr = rt.result.process
        self._logger:info("start processor:%d %s", self._count, self._logger.format(rt))
        
        return rt.result.process
    end
    -- TODO: error handling and logging
end

function pool:put_process(params, desc)
    local gpid = params.gpid
    if self._processes[gpid] then
        self._processes[gpid].status = "idle"
    else 
        self._processes[gpid] = {}
        self._processes[gpid].status = "idle"
        self._processes[gpid].gpid = gpid
        self._processes[gpid].addr = desc.addr
    end
end

function pool:on_timeout()
    self._logger:debug(self._logger.format(glr.status.processes()))
    self._logger:debug(self._logger.format(self._sup:call("query")))
    self._logger:flush()
end

--[[
pool 提供的管理接口

Author: Zhouxiaopeng
Date  : 2014/8/14
Desc  : 提供外部访问、管理进程池的API
]]

--停止进程
--入参：{gpid1, gpid2, gpid3,...}
--返回值：此次停止的进程id列表，{gpid1, gpid2,...}
function pool:stop_processes(params)
    local stop_params = {}
    stop_params.process_type="gen"
    stop_params.process_params = self._start_argv_template.process_params
    local stop_processes = {}
    local gpid
    if type(params) == "number" then
        gpid = params
        local rt = self._processes[gpid]
        if rt and rt.addr then
            stop_params.process = rt.addr
            self._sup:call("stop_process",stop_params)
            self._processes[gpid] = nil
            self._logger:info("processor(%d) has stopped", gpid)
            stop_processes[#stop_processes+1] = gpid
        else
            self._logger:info("stopping processor(%d) not exist", gpid) 
        end
    elseif type(params) == "table" then
        for _, gpid in pairs(params) do
            local rt = self._processes[gpid]
            if rt and rt.addr then
                stop_params.process = rt.addr
                self._sup:call("stop_process", stop_params)
                self._logger:info("processor(%d) has stopped", gpid)
                self._processes[gpid] = nil
                stop_processes[#stop_processes+1] = gpid
            else
                self._logger:info("stopping processor(%d) not exist", gpid) 
            end
        end
    end
    self._count = self._count - #stop_processes
    return stop_processes
end

--批量地创建进程
--入参:创建的进程个数
--
function pool:start_processes(nums_of_processes_starting)
    local started_processes = {}
    local nums_of_processes = 0
    if type(nums_of_processes_starting) == "number" then
        if nums_of_processes_starting > 0 then
            nums_of_processes = self._count + nums_of_processes_starting
            if nums_of_processes > self._max then
                nums_of_processes = self._max
            end
        end
    local started_process_addr = nil
        for i = self._count+1, nums_of_processes do
            started_process_addr = self:_create_process()
            started_processes[#started_processes+1] = started_process_addr.gpid
        end        
    end
    return started_processes
end

--查询params列表指定的进程状态
--返回值：{gpid1="used" or "idle", gpid2="used" or "idle" }
function  pool:get_status_of_processes(params)
    local gpid_list = {}
    local process = nil
    if type(params) == "table" then
        for _,gpid in pairs(params) do
            process = self._processes[gpid]
            if process then
                gpid_list[gpid] = self._processes[gpid].status
            else
                gpid_list[gpid] = "unkonw process"
            end
        end
    end

    return gpid_list
end


--查询进程池状态
--返回值：{进程池最大支持的进程数，目前进程池中的进程数}
function pool:get_status_of_pool()
    local max = 0
    local idle = 0
    local used = 0
    local min = 0
    max = self._max
    min = self._min

    for gpid, p in pairs(self._processes) do
        if p.status == "used" then
            used = used + 1
        elseif p.status == "idle" then
            idle = idle + 1
        end
    end

    return {max_count=max, min_count=min, used_count=used, idle_count=idle}
end

--查询dispatcher的状态
--返回值："started" or "stopped"
function pool:get_status_of_dispatcher()
    return {status = self._dispatch.status,
            gpid = self._dispatch.gpid, 
            addr = self._dispatch.addr}
end

--查询指定类型进程的id
--入参："all"（全部）, "used"（正在使用）, "idle"（空闲）中的一个，
--返回值：{id1，id2, id3,...}
function pool:get_processes_list(params)
    local gpid_list = {}
    for gpid, p in pairs(self._processes) do
        if params == "all" then
            gpid_list[#gpid_list + 1] = gpid
        elseif p.status == params then
            gpid_list[#gpid_list + 1] = gpid
        end
    end
    return gpid_list
end


function pool:start_dispatcher()
    if self._dispatch.status == "started" then
        return self._dispatch
    end

    local rt = self._sup:call("start_process", {
                                  process_type = "gen",
                                  process_params = {
                                      mod_name = self._dispatcher,
                                      group = self._pool_name,
                                      parameters = {self._app_name, self._pool_name, self._params},
                                  }
    })
    if rt and rt.result and rt.result.process and rt.result.process.gpid then
        local dispatcher_gpid = rt.result.process.gpid
        self._dispatch.gpid = dispatcher_gpid
        self._dispatch.status = "started"
        self._dispatch.addr = rt.result.process
    end
    return self._dispatch

end 
--停止dispatcher
function pool:stop_dispatcher()

    if self._dispatch.status == "stopped" then
        return self._dispatch
    end

    local stop_params = {}
    local gpid = nil
    stop_params.process_type = "gen"
    stop_params.process_params = {
                                      mod_name = self._dispatcher,
                                      group = self._pool_name,
                                      parameters = {self._app_name, self._pool_name, self._params},
                                  }
    if self._dispatch and self._dispatch.addr then
        gpid = self._dispatch.addr.gpid
        stop_params.process = self._dispatch.addr                               
        local rt = self._sup:call("stop_process", stop_params)
        --pprint.pprint(rt, "pool.rt")
        self._dispatch.status = "stopped"
        self._dispatch.addr = nil
        self._dispatch.gpid = nil
        self._logger:info(" dispatcher(%d) stopped", gpid)
    end 
    return self._dispatch
end


--重启dispatcher
function pool:restart_dispatcher()
    self:stop_dispatcher()
    self:start_dispatcher()
end