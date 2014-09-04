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
local for_each2 = require(_PACKAGE .. "parellel").for_each2
local spawn_cnt = require(_PACKAGE .. "parellel").spawn_cnt
local rpc_call = require(_PACKAGE .. "parellel").rpc_call
local write_timer = require(_PACKAGE .. "tps").write_timer
local write_timer1 = require(_PACKAGE .. "tps").write_timer1
local tps = require(_PACKAGE .. "tps").tps


--params = {["url"] = "http://url",["times"] = times,["type"] = type}
-- times : default 1
-- type :
--      "only_connect" socket connect only
--      "only_send" : socket connect and send only
--      "normal" : socket connect, send and recv
function http_conn_by_type(params)
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
        --print("----------- before http_conn_by_type ---- ",cnt)
        local res,err_msg = cli:conn_by_type(req,e_type)
        --pprint(res,"res")
        --print("----------- after  http_conn_by_type ---- ",cnt)
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

    http_conn_by_type(url,times)
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

--[[
result = {
    [1] = {["id"],["process"],["result"],["error"]}
    [2] = {}
    ...
    [parellel_total] = {}
]]--
function http_parellel_for_each(params)
    local parellel_total = params["parellel_total"]
    local req_of_per_parellel = params["req_of_per_parellel"]
    local url = params["url"]
    local only_send_ratio = params["only_send_ratio"] -- only_send 差错率
    local only_connect_ratio = params["only_connect_ratio"] -- only_connect 差错率
    local normal_ratio = 1 - only_send_ratio - only_connect_ratio

    local timeout = 3000
    local timer = {}
    local params_list = {}
    local normal_count = parellel_total * normal_ratio
    local only_connect_count = parellel_total * only_connect_ratio
    local only_send_count = parellel_total - normal_count - only_connect_count 

    for i = 1,parellel_total do --并发数
        if i > normal_count then
            if i <= normal_count + only_connect_count  then
            	print("---------------it is going to  status :  only_connect-------------",i)
            	params_list[i] = {url,req_of_per_parellel, "only_connect"}--请求数/并发
            else
            	print("---------------it is going to  status :  only_send -------------",i)
            	params_list[i] = {url,req_of_per_parellel,"only_send"}--请求数/并发
            end
        else -- Normal 并发数
            print("---------------it is going to  status : normal------------",i)
            params_list[i] = {url,req_of_per_parellel}
        end
    end
    glr.time.sleep(4)
    result = for_each("test_http","http_conn_by_type",params_list)
    -- pprint(result,"--result--")

    write_timer1(path,result)
end

--[[
params = {
    ["parellel_total"] = num_of_times,
    ["req_of_per_parellel"] = num_of_times,
    ["url"] = url,
    ["only_send_ratio"] = float_with_rate_of_send,
    ["only_connect_ratio"] = float_with_rate_of_conn,

]]--
function http_parellel_for_each2(params)
    local parellel_total = params["parellel_total"]
    local req_of_per_parellel = params["req_of_per_parellel"]
    local url = params["url"]
    local only_send_ratio = params["only_send_ratio"] -- only_send 差错率
    local only_connect_ratio = params["only_connect_ratio"] -- only_connect 差错率
    local normal_ratio = 1 - only_send_ratio - only_connect_ratio

    os.execute("mkdir -p ./timer")
    os.execute("mkdir -p ./tps")

    local timeout = 3000
    local timer = {}
    local params_list = {}
    local normal_count = parellel_total * normal_ratio
    local only_connect_count = parellel_total * only_connect_ratio
    local only_send_count = parellel_total - normal_count - only_connect_count 

    for i = 1,parellel_total do --并发数
        if i > normal_count then
            if i <= normal_count + only_connect_count  then
            	--print("---------------it is going to  status :  only_connect-------------",i)
            	params_list[i] = {url,req_of_per_parellel, "only_connect"}--请求数/并发
            else
            	--print("---------------it is going to  status :  only_send -------------",i)
            	params_list[i] = {url,req_of_per_parellel,"only_send"}--请求数/并发
            end
        else -- Normal 并发数
            --print("---------------it is going to  status : normal------------",i)
            params_list[i] = {url,req_of_per_parellel}
        end
    end
    --glr.time.sleep(4)
    for_each2("test_http","http_conn_by_type",params_list)

    repeat
        running_process = glr.status.processes()
        --pprint(running_process,"running_process")
        --print("len running_process :",table.maxn(running_process))
        glr.time.sleep(1)
    until table.maxn(running_process) == 0
    print("---------- completed -----------")
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

function test_for_each()
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

function test_http_parellel_for_each2()
    params = {
        ["parellel_total"] = 10,
        ["req_of_per_parellel"] = 10,
        ["url"] = "http://127.0.0.1:8080/static/html/index.html",
        ["only_send_ratio"] = 0,
        ["only_connect_ratio"] = 0,
    }
    http_parellel_for_each2(params)
end

function test_http_parellel_for_each()
    params = {
        ["parellel_total"] = 10,
        ["req_of_per_parellel"] = 50,
        ["url"] = "http://127.0.0.1:8080/static/html/index.html",
        ["only_send_ratio"] = 0,
        ["only_connect_ratio"] = 0,
    }
    http_parellel_for_each(params)
end


if ... == "__main__" then

    -- test_http_parellel()
    test_http_parellel_for_each2()
    -- test_http_parellel_for_each()
    -- timer_handle()
    -- glr.exit()
end
