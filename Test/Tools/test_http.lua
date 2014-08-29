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

local for_each = require(_PACKAGE .. "parellel").for_each
local spawn_cnt = require(_PACKAGE .. "parellel").spawn_cnt
local rpc_call = require(_PACKAGE .. "parellel").rpc_call
local write_timer = require(_PACKAGE .. "tps").write_timer
local tps = require(_PACKAGE .. "tps").tps


--params = {["url"] = "http://url",["times"] = times}
function http_conn_interrupt(params)
    url = params[1]
    times = params[2] or 1
    e_type = params[3] 

    local timer = {}
    local result = {}
    for cnt = 1,times do
        timer[cnt] = {}
        result[cnt] = {}
        timer[cnt]["begin"] = os.time()

        local cli = httpClient:new()
        local req = httpRequest:new():init("GET", url)
        print("-----------before  conn_by_type ---- ",cnt)
        local res,err_msg = cli:conn_by_type(req,e_type)
        pprint(res,"res")
        print("-----------after  conn_by_type ---- ",cnt)
        timer[cnt]["end"] = os.time()
        result[cnt]["result"] = string.format("ret:%s err:%s",res,err_msg)
        result[cnt]["timer"] = timer[cnt]
    end
    return result
end

--params = {["url"] = "http://url",["times"] = times}
function http_conn(params)
    url = params[1]
    times = params[2]

    local timer = {}
    local result = {}
    for cnt = 1,times do
        timer[cnt] = {}
        result[cnt] = {}
        --timer[cnt]["begin"] = glr.time.now()
        timer[cnt]["begin"] = os.time()

        local cli = httpClient:new()
        local req = httpRequest:new():init("GET", url)
        local res,err_msg = cli:get2(req)

        --timer[cnt]["end"] = glr.time.now()
        timer[cnt]["end"] = os.time()
        result[cnt]["result"] = string.format("res:%s  err_msg:%s",res,err_msg)
        result[cnt]["timer"] = timer[cnt]
    end
    pprint(result,"--- result ---")
    return result
end

-- params = {
--          ["parellel_cnt"] = parellel_cnt,
--          ["parellel_per"] = parellel_per,
--          ["mod_name"]= mod_name
--          ["entry"]= entry
--          ["params"]= params
--         }
-- for example
-- params = {
--    ["parellel_cnt"] = 1,
--    ["parellel_per"] = 1,
--    ["mod_name"] = "test_http",
--    ["params"] = {"http://127.0.0.1:8080/statics/index.html",1}
--}
function http_parellel(params)
    local parellel_cnt = assert(params["parellel_cnt"])
    local parellel_per = assert(params["parellel_per"]) 

    local mod_name = assert(params["mod_name"])
    local entry = assert(params["entry"])
    local args = assert(params["params"])

    local process = spawn_cnt(parellel_cnt, "parellel", "main_loop", args)
    --pprint(process,"process")

    local opts = {}
    local result = {}
    opts["mod_name"] = mod_name
    opts["entry"] = entry
    opts["params"] = args
    for i,addr in pairs(process) do
        result[i] = {}
        for j = 1,parellel_per do
            opts["id"] = j
            --pprint(opts,"--- opts ----")
            result[i][j] = rpc_call(addr,opts)
        end
    end
    --pprint(result,"---- result -----")

    local timer = {}
    for i,k in pairs(result) do
        timer[i] = {}
        for j,v in pairs(k) do
            timer[i][j] = v["timer"]
        end
    end

    pprint(timer,"timer")
    local path = string.format("./timer_%d_%d", parellel_cnt, parellel_per)
    write_timer(path, timer)

    return result
end

-----------------------  test http ----------------------

function test_http_parellel()
     params = {
        ["parellel_cnt"] = 4,
        ["parellel_per"] = 2,
        ["mod_name"] = "test_http",
        ["entry"] = "http_conn",
        ["params"] = {"http://127.0.0.1:8080/statics/index.html",1}
    }
    http_parellel(params)
end

function test_http_parellel_for_each()
    local mod_name = "test_http"
    local entry = "http_conn"
    local timer = {}

    local params_list = {
        {"http://127.0.0.1:8080/statics/index.html",1},
        {"http://127.0.0.1:8080/statics/index.html",1}
    }

    local result = for_each(mod_name,entry,params_list)
    pprint(result,"result")

    --for i,k in pairs(result) do
    --    timer[i] = result["timer"]
    --end
    --write_timer(timer)
    --tps(#timer)

    return result
end

function test_http_pool()
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
    write_timer(timer, times)
    tps(times)
end




if ... == "__main__" then

    test_http_parellel()
    --test_http_parellel_for_each()
end
