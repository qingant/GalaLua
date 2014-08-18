--[[
	Author : Liu Wenxue
	Date   : 2014/08/08

	== Introduction ==
    test webframework with http_client

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

function http_connect(timer,cnt,url)
    local cli = httpClient:new()
    timer[cnt] = {}
    timer[cnt]["begin"] =  glr.time.now()
    for i = 1,1 do
        local req = httpRequest:new():init("GET", url)
        local res = cli:get_Response(req,20)
    end
    timer[cnt]["end"] = glr.time.now()
    --return res
    --glr.exit()
end

--params = {["timer"] = {}, ["cnt"] = 1, ["url"] = "http://url"}
function http_conn(params)
    --timer = params["timer"]
    --cnt = params["cnt"]
    --url = params["url"]
    timer = params[1]
    cnt = params[2]
    url = params[3]
    local cli = httpClient:new()
    timer[cnt] = {}
    timer[cnt]["begin"] =  glr.time.now()
    for i = 1,1 do
        local req = httpRequest:new():init("GET", url)
        local res = cli:get_Response(req,20)
    end
    timer[cnt]["end"] = glr.time.now()
    --return res
    --glr.exit()
end

function write_timer(timer, times)
    local path = os.getenv("PWD") .. "/timer." .. times
    local fd = assert(io.open(path, "wa"))

    for i,k in pairs(timer) do
        local bg = timer[i]["begin"]
        local ed = timer[i]["end"]
        fd:write(string.format("timer %d begin %d  end %d  gap %d\n",i,bg, ed, ed-bg))
    end

    fd:close()
end

function tps(times)
    local BUFSIZE = 2^13
    local path_timer = os.getenv("PWD") .. "/timer." .. times
    local fd_timer = assert(io.open(path_timer, "r"))

    local _times = {}
    local bg = {}
    local ed = {}
    local gap = {}

    local i = 1
    --local lines, rest = fd_timer:read(BUFSIZE, "*line")
    local lines, rest = fd_timer:read("*line")
    while lines do
        _times[i], bg[i], ed[i], gap[i] = string.match(lines, "([0-9]+)%D*([0-9]+)%D*([0-9]+)")
        --print("lines : " .. lines .. "rest")
        lines = fd_timer:read("*line")
        i = i + 1
    end

    fd_timer:close()


    local tps = {}
    local index = 1
    for i, k  in pairs(bg) do
        if bg[i] == bg[i+1] then
            tps[index] = (tps[index] or 0) + 1
        else
            index = index + 1
            tps[index] = 0
        end
    end

    local path_tps = os.getenv("PWD") .. "/tps." .. times
    local fd_tps = assert(io.open(path_tps, "wa"))
    for i, k in pairs(tps) do
        fd_tps:write(string.format("index %d tps %d\n", i, tps[i]))
    end
    fd_tps:close()
end

function main_loop(timeout)
    while true do
        ::beg::
        local timeout = timeout or 3000
        local mtype,desc,msg = glr.recv(timeout)
        if desc and desc.attr and  desc.attr.corrid == 1 then
            msg = unpack(msg)
            local mod_name = require(msg["module"])
            local entry = msg["entry"]
            if mod_name and mod_name[entry] then
                local func = mod_name[entry]
                local ret,err_msg = func(msg["params"])
                if ret then
                    local msg = {["error"]=nil,["result"]=ret}
                else
                    local msg = {["error"]=err_msg,["result"]=nil}
                end
                glr.send(desc.addr,packer(msg),{corrid = 2})
            else
                local err_msg = "mod_name or entry doesn't exist"
                local msg = {["error"]=err_msg,["result"]=nil}
                glr.send(desc.addr,packer(msg),{corrid = 2})
            end
        else
            --local err_msg = "invaild corrid"
            --local msg = {["error"]=err_msg,["result"]=nil}
            --glr.send(desc.addr,packer(msg),{corrid = "dispatch"})
            goto beg
        end
    end
end

if ... == "__main__" then
    local mod_name = "test_http"
    local entry = "http_connect"
    local url = "http://127.0.0.1:8080/statics/index.html"
    local timer =  {}
    local cnt = 0
    local params = {
        ["timeout"] = 30,
        ["min"] = 1,
        ["step"] = 2,
        ["max"] = 10
    }

    local sup = glr

    local http_pool = pool:new():init(sup,params)


    p = http_pool:get_process()
    while p do
        cnt = cnt + 1
        p.spawn(mod_name,entry,timer,cnt,url)
        p = http_pool:get_process()
    end

    local times = cnt
    --write_timer(timer, times)
    --tps(times)
end
