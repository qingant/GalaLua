--[[
	Author : Liu Wenxue
	Date   : 2014/08/08

	== Introduction ==
    statics tps of http_connect

    == TODO ==
    logging
]]
module(..., package.seeall)

local pprint = require("pprint").pprint
local cmsgpack = require("cmsgpack")


function for_each(mod_name,entry,params_list)

    local cnt = #params_list
    local process = {}
    for i = 1, cnt do
        local ret, err_msg = glr.spawn("parellel","caller",i, __id__, mod_name, entry, params_list[i])
        if not ret then
            error(string.format("spawn %s.%s error: %s",mod_name,entry,err_msg))
        end
        process[i] = ret
    end

    local result = {}
    for i = 1, cnt do
        print("cnt", cnt)
        local timeout = 300000
        local mtype,desc,rec_msg = glr.recv_by_condition( 
                function (msg)
                    local desc = msg[2]
                    return ( desc and desc.attr.corrid == 2 )
                end, timeout)
        assert(mtype, "Recv Timeout")
        result[desc.attr.msgid] = cmsgpack.unpack(rec_msg)
    end

    while true do
        local t = glr.recv_by_type(glr.EXIT, 1000)
        if not t then
            break
        end
    end

    print("result length",#result)
    return result
end

function caller(idx, control_id, mod_name, entry, ...)
    local func = require(mod_name)[entry]
    assert(func and type(func) == "function", "mod_name doesn't exist OR entry empty")
    local ret = func(...)
    local ret, err_msg = glr.send(control_id, cmsgpack.pack(ret), { corrid = 2, msgid = idx})
    assert(ret, "main loop send error:")
end

function test_for_each()
    local timer = {}
    local params_list = {}
    for i = 1, 2 do
        params_list[i] = {"http://127.0.0.1:8080/static/html/index.html",1}
    end

    local result = for_each("test_http","http_conn_by_type",params_list)

    --pprint(result,"result")
end


if ... == "__main__" then
    test_for_each()
end
