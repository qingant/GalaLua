
module(..., package.seeall)

local pprint = require("pprint").pprint
local cmsgpack = require("cmsgpack")
local packer = cmsgpack.pack
local unpack = cmsgpack.unpack

function for_each1(mod_name,entry,map)
    local result = {}
    for i,params in pairs(map) do
        local ret, err_msg = glr.spawn(mod_name,entry,params)
        if not ret then
            error(string.format("spawn %s.%s error: %s",mod_name,entry,err_msg))
        end
        result[i] = ret
    end
    return result
end

function for_each(mod_name,entry,params_list)
    local result = {}

    local cnt = #params_list
    local timeout = 3000
    local process = {}
    process = spawn_cnt(cnt,"parellel","main_loop",timeout)

    local params = {}
    params["mod_name"] = mod_name
    params["entry"] = entry
    for i,addr in pairs(process) do
        params["id"] = 1
        params["params"] = params_list[i]
        result[i] = rpc_call(addr, params)
        --pprint(result[i],"-- result --")
    end
    return result
end

function spawn_cnt(cnt,mod_name,entry,...)
    local process = {}
    for i = 1, cnt do
        local ret, err_msg = glr.spawn(mod_name,entry,...)
        if not ret then
            error(string.format("spawn %s.%s error: %s",mod_name,entry,err_msg))
        end
        process[i] = ret
    end
    return process
end

--params = {
--      ["id"] = id, 
--      ["mod_name"] = mod_name, 
--      ["entry"] = entry,
--      ["params"] = params}
--
--- mod_name[entry](params) 
--- return ret,err_msg
function rpc_call(addr,params)
    local msgid = params["id"] or 1
    local mod_name = params["mod_name"]
    local entry = params["entry"]
    local params = params["params"]

    local result = {}
    local timer = {}
    local addr = addr
    local id = addr.gpid
    local status = glr.status.status(addr.gpid)
    send_msg = {
            ["id"] = msgid,
            ["process"] = {
                ["id"]=id,
                ["status"] = status
                },
            ["module"] =  {
                ["mod_name"] = mod_name,
                ["entry"]=entry,
                ["params"] = params
                }
    }
    attr = { ["corrid"] = 1,["msgid"] = msgid }
    --pprint(addr,"send")
    local ret, err_msg = glr.send(addr, packer(send_msg),attr)
    if not ret then
        error(string.format("send to %s:%s:%d error : %s",addr.host,addr.port,addr.gpid, err_msg))
    end

    timer["begin"] = ret["stamp"]
    local timeout = timeout or 3000
    local mtype,desc,rec_msg = glr.recv(timeout)
    timer["end"] = glr.time.now()

    if desc and rec_msg then
        rec_msg = unpack(rec_msg)
        if desc.addr and desc.addr.gpid == id and desc.attr.corrid == 2 then
            --pprint(rec_msg,"---- recev ----")
            result["id"] = rec_msg["id"]
            result["process"] = rec_msg["process"]
            result["error"] = rec_msg["error"]
            result["result"] = rec_msg["result"]
            result["timer"] = {}
            result["timer"]["begin"] = timer["begin"]
            result["timer"]["end"] = timer["end"]
        end
    else
        error(string.format("receive %s:%s:%d error : %s",addr.host,addr.port,addr.gpid, err_msg))
    end
    --pprint(result,"--- result ---")
    return result
end


function main_loop(timeout)
    while true do
        ::beg::
        local timeout = timeout or 3000
        local mtype,desc,rec_msg = glr.recv(timeout)
        if rec_msg and desc and desc.attr and desc.attr.corrid == 1 then
            rec_msg = unpack(rec_msg)
            --pprint("--- main_loop receive ----",rec_msg["id"])
            local mod_name = require(rec_msg["module"]["mod_name"])
            local entry = rec_msg["module"]["entry"]
            if mod_name and mod_name[entry] then
                local func = mod_name[entry]
                local ret,err_msg = func(rec_msg["module"]["params"])
                local send_msg = {}
                if ret then
                    send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["error"] = nil,
                        ["result"]=ret
                    }
                else
                    send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["error"]=err_msg,
                        ["result"]=nil}
                end
                local attr = {corrid = 2, msgid = 1}
                --print("--- main_loop send ----",send_msg["id"])
                glr.send(desc.addr,packer(send_msg),attr)
            else
                local err_msg = "mod_name or entry doesn't exist"
                local send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["error"]=err_msg,
                        ["result"]=nil}
                local attr = {corrid = 2, msgid = 1}
                --print("--- main_loop send ----",send_msg)
                glr.send(desc.addr,packer(send_msg),attr)
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
    --local ret, err_msg = glr.spawn("parellel","main_loop",timeout)
    --if not ret then
    --    error("spawn error")
    --end

    local timeout = 3000
    local timer = {}
    local map = {
        {"http://127.0.0.1:8080/statics/index.html",1},
        --{"http://127.0.0.1:8080/statics/index.html",1},
        --{"http://127.0.0.1:8080/statics/index.html",1},
        --{"http://127.0.0.1:8080/statics/index.html",1},
        {"http://127.0.0.1:8080/statics/index.html",1},
    }


    local result = for_each("test_http","http_conn",map)
    pprint(result,"result")
end
