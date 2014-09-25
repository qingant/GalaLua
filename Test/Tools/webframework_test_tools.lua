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
local httpClient = require("http_client").httpClient local httpRequest = require("http_client").httpRequest local cmsgpack = require("cmsgpack")
local packer = cmsgpack.pack
local unpack = cmsgpack.unpack

local for_each = require(_PACKAGE .. "for_each").for_each
local statistics_http_response = require(_PACKAGE .. "statistics").statistics_http_response
local writer_timer = require(_PACKAGE .. "tps").write_timer3


--params = {["url"] = "http://url",["times"] = times,["type"] = type}
-- times : default 1
-- type :
--      "only_connect" socket connect only
--      "only_send" : socket connect and send only
--      "normal" : socket connect, send and recv
function http_keepalive_conn_by_type(params)
    url = params[1]
    times = params[2] or 1
    e_type = params[3]
    local cli = httpClient:new()
    local req = httpRequest:new():init("GET", url)
    cli:init(req)

    local timer = {}
    local result = {}
    for cnt = 1,times do
        timer[cnt] = {}
        result[cnt] = {}
        timer[cnt]["begin"] = os.time()

        --print("----------- before http_conn_by_type1 ---- ",cnt)
        local res, err_msg = cli:keepalive_conn_by_type(req, e_type)
        --print("----------- after  http_conn_by_type1 ---- ",cnt)
        timer[cnt]["end"] = os.time()
        result[cnt]["result"] = string.format("ret:%s err:%s",res,err_msg)
        result[cnt]["timer"] = timer[cnt]
    end
    cli:close()
    return result
end

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

--[[
result = {
    [1] = {["id"],["process"],["result"],["error"]}
    [2] = {}
    ...
    [parellel_total] = {}
]]--
function http_parellel_for_each(params)
    local request_type = params["request_type"]
    local parellel_total = params["parellel_total"]
    local req_of_per_parellel = params["req_of_per_parellel"]
    local url = params["url"]
    local only_send_ratio = params["only_send_ratio"] -- only_send 差错率
    local only_connect_ratio = params["only_connect_ratio"] -- only_connect 差错率
    local normal_ratio = 1 - only_send_ratio - only_connect_ratio

    local normal_count = parellel_total * normal_ratio
    local only_connect_count = parellel_total * only_connect_ratio
    local only_send_count = parellel_total - normal_count - only_connect_count 

    local timeout = 3000
    local params_list = {}

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
    local result = {}
    if request_type == "keepalive" then
        print("keepalive request")
        result = for_each("test_http","http_keepalive_conn_by_type",params_list)
    else
        result = for_each("test_http","http_conn_by_type",params_list)
    end
    -- pprint(result,"--result--")
    --repeat
    --    running_process = glr.status.processes()
    --    --pprint(running_process,"running_process")
    --    --print("len running_process :",table.maxn(running_process))
    --    glr.time.sleep(1)
    --until table.maxn(running_process) == 0
    print("---------- completed -----------")
    return result

    --write_timer1(path,result)
end



function webframewrok_test_by_http_parellel_for_each(params)
    local result = http_parellel_for_each(params)
    pprint(result,"result")

    local pwd = os.getenv("PWD")

    writer_timer(result,pwd .. "/timer/timer_tmp")

    local parellel_total = params["parellel_total"]
    local req_of_per_parellel = params["req_of_per_parellel"]
    local only_send_ratio = params["only_send_ratio"]
    local only_connect_ratio = params["only_connect_ratio"]
    local err_path = string.format("%s/statistics/error_statistics_%d_%d_%d_%d.csv",pwd,
                        parellel_total,
                        req_of_per_parellel,
                        only_send_ratio * 100,
                        only_connect_ratio * 100
                        )
    local tps_path = string.format("%s/statistics/tps_statistics_%d_%d_%d_%d.csv",pwd,
                        parellel_total,
                        req_of_per_parellel,
                        only_send_ratio * 100,
                        only_connect_ratio * 100
                        )
    local complete_path = string.format("%s/statistics/complete_statistics_%d_%d_%d_%d.csv",pwd,
                        parellel_total,
                        req_of_per_parellel,
                        only_send_ratio * 100,
                        only_connect_ratio * 100
                        )
    print(err_path)
    print(tps_path)
    print(complete_path)
    statistics_http_response(err_path, tps_path, complete_path)
end


-----------------------  test ----------------------

function test_http_keepalive_conn_by_type()
    params = {}
    params[1] =  "127.0.0.1:8080/index"
    params[2] = 5
    params[3] = "only_send"
    pprint(http_keepalive_conn_by_type(params),"keepalive result")
    params[3] = "only_connect"
    pprint(http_keepalive_conn_by_type(params),"keepalive result")
    params[3] = "normal"
    pprint(http_keepalive_conn_by_type(params),"keepalive result")
end

function test_http_conn_by_type()
    params = {}
    params[1] =  "127.0.0.1:8080/index"
    params[2] = 5
    params[3] = "only_send"
    pprint(http_conn_by_type(params),"keepalive result")
    params[3] = "only_connect"
    pprint(http_conn_by_type(params),"keepalive result")
    params[3] = "normal"
    pprint(http_conn_by_type(params),"keepalive result")
end

function test_http_parellel_for_each()
    local params = {
        ["parellel_total"] = 10,
        ["req_of_per_parellel"] = 10,
        ["url"] = "http://127.0.0.1:8080/static/html/index.html",
        ["only_send_ratio"] = 0,
        ["only_connect_ratio"] = 0,
    }
    local result = http_parellel_for_each(params)
    pprint(result,"result")

    local params = {
        ["request_type"] = "keepalive",
        ["parellel_total"] = 10,
        ["req_of_per_parellel"] = 10,
        ["url"] = "http://127.0.0.1:8080/static/html/index.html",
        ["only_send_ratio"] = 0,
        ["only_connect_ratio"] = 0,
    }
    local result = http_parellel_for_each(params)
    pprint(result,"result")
end

if ... == "__main__" then

    local params = {}
    local parellel_cnt = assert(glr.get_option("c"),"-c must be given")
    local total_request  = assert(glr.get_option("n"),"-n must be given")
    local req_of_per_parellel = total_request / parellel_cnt
    params["parellel_total"] = parellel_cnt
    params["req_of_per_parellel"] = req_of_per_parellel
    params["url"] = assert(glr.get_option("u"),"-u must be given\n")
    params["only_send_ratio"] = glr.get_option("i") or 0
    params["only_connect_ratio"] = glr.get_option("j") or 0
    params["request_type"] = glr.get_option("t") or "keepalive"
    webframewrok_test_by_http_parellel_for_each(params)
    --test_http_keepalive_conn_by_type()
    --test_http_conn_by_type()
end
