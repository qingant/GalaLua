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
local packer = cmsgpack.pack
local unpack = cmsgpack.unpack


function for_each(mod_name,entry,params_list)
    local result = {}

    local cnt = #params_list
    local timeout = 3000
    local process = {}
    for i = 1, cnt do
        local ret, err_msg = glr.spawn("for_each","main_loop",timeout)
        if not ret then
            error(string.format("spawn %s.%s error: %s",mod_name,entry,err_msg))
        end
        process[i] = ret
    end

    local params = {}
    params["mod_name"] = mod_name
    params["entry"] = entry
    for i,addr in pairs(process) do
        params["params"] = params_list[i]
        local attr = { ["corrid"] = 1,["msgid"] = i}
        local ret, err_msg = glr.send(addr, packer(params),attr)
        if not ret then
            result[i] = {
                ["mtype"] = nil,
                ["desc"] = nil,
                ["result"]= string.format("send to %s:%s:%d error : %s",addr.host,addr.port,addr.gpid, err_msg)
            }
        end
    end

    local num = 0;
    while num < cnt do
        local timeout = timeout or 30000
        local mtype,desc,rec_msg = glr.recv(timeout)
        if desc and rec_msg then
            rec_msg = unpack(rec_msg)
            if desc.addr and desc.attr.corrid == 2 then
                result[desc.attr.msgid] = {
                                        ["mtype"] = mtype,
                                        ["rec_msg"] = rec_msg,
                                        ["desc"] = desc
                                    }
            end
            print("num : ",num)
            print("cnt : ",cnt)
            num = num + 1
        else
            print("num : ",num)
            print("cnt : ",cnt)
            print("result length",#result)
            print(string.format("receive error"))
        end
    end

    print("result length",#result)
    return result
end

function main_loop(timeout)
    while true do
        ::beg::
        local timeout = timeout or 30000
        local mtype,desc,rec_msg = glr.recv(timeout)
        if rec_msg and desc and desc.attr and desc.attr.corrid == 1 then
            rec_msg = unpack(rec_msg)
            local mod_name = require(rec_msg["mod_name"])
            local entry = rec_msg["entry"]
            if mod_name and mod_name[entry] then
                --print("mod_name[entry]:",mod_name[entry])
                local func = mod_name[entry]
                local ret,err_msg = func(rec_msg["params"])
                --print(ret,"result")
                local send_msg = {
                    ["result"]=ret,
                    ["error"] = err_msg,
                }
                local attr = { corrid = 2, msgid = desc.attr.msgid }
                local ret, err_msg = glr.send(desc.addr,packer(send_msg),attr)
                if not ret then
                    pprint(desc.addr, "main loop send error:")
                end
                break
            else
                local err_msg = "mod_name or entry doesn't exist"
                local send_msg = {
                        ["error"]=err_msg,
                        ["result"]=nil}
                local attr = { corrid = 2, msgid = desc.attr.msgid }
                local ret, err_msg = glr.send(desc.addr,packer(send_msg),attr)
                if not ret then
                    pprint(desc.addr, "main loop send error:")
                end
                break
            end
        else
            goto beg
        end
    end
end

function test_for_each()
    local timeout = 3000
    local timer = {}
    local params_list = {}
    for i = 1, 10 do
        params_list[i] = {"http://127.0.0.1:8080/static/html/index.html",10}
    end

    local result = for_each("test_http","http_conn_by_type",params_list)

    --pprint(result,"result")
end


if ... == "__main__" then
    test_for_each()
end
