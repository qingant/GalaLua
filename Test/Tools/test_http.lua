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

local for_each = require(_PACKAGE .. "parellel").queue
local spawn_cnt = require(_PACKAGE .. "parellel").spawn_cnt
local rpc_call = require(_PACKAGE .. "parellel").rpc_call
local write_timer = require(_PACKAGE .. "tps").write_timer




--params = {["url"] = "http://url",["times"] = times}
function http_conn(params)
    url = params[1]
    times = params[2] or 1

    local timer = {}
    local result = {}
    for cnt = 1,times do
        timer[cnt] = {}
        result[cnt] = {}
        timer[cnt]["begin"] = glr.time.now()

        local cli = httpClient:new()
        local req = httpRequest:new():init("GET", url)
        local res = cli:get_Response(req,20)

        timer[cnt]["end"] = glr.time.now()
        result[cnt]["result"] = res
        result[cnt]["timer"] = timer[cnt]
    end
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
function test_http_parellel(params)
    local parellel_cnt = params["parellel_cnt"] or 2
    local parellel_per = params["parellel_per"] or 2

    local mod_name = params["mod_name"] or "test_http"
    local entry = params["entry"] or "http_conn"
    local args = params["params"] or {"http://127.0.0.1:8080/statics/index.html",1}

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


function test_http_parellel_for_each()
    local mod_name = "test_http"
    local entry = "http_conn"
    local result = for_each("test_http","http_conn",map)
    local timer = {}

    local params_list = {
        {"http://127.0.0.1:8080/statics/index.html",1},
        {"http://127.0.0.1:8080/statics/index.html",1},
        {"http://127.0.0.1:8080/statics/index.html",1},
        {"http://127.0.0.1:8080/statics/index.html",1},
        {"http://127.0.0.1:8080/statics/index.html",1},
    }

    local result = for_each(mod_name,entry,params_list)

    for i,k in pairs(result) do
        timer[i] = result["timer"]
    end
    write_timer(timer)
    tps(#timer)

    return result
end





if ... == "__main__" then



    params = {}
    test_http_parellel(params)
    --local mod_name = "test_http"
    --local entry = "http_connect"
    --local url = "http://127.0.0.1:8080/statics/index.html"
    --local timer =  {}
    --local cnt = 0
    --local params = {
    --    ["timeout"] = 30,
    --    ["min"] = 1,
    --    ["step"] = 2,
    --    ["max"] = 10
    --}

    --local sup = glr

    --local http_pool = pool:new():init(sup,params)


    --p = http_pool:get_process()
    --while p do
    --    cnt = cnt + 1
    --    p.spawn(mod_name,entry,timer,cnt,url)
    --    p = http_pool:get_process()
    --end

    --local times = cnt
    --write_timer(timer, times)
    --tps(times)
end
