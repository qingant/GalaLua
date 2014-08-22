
module(..., package.seeall)

local pprint = require("pprint").pprint
local cmsgpack = require("cmsgpack")
local packer = cmsgpack.pack
local unpack = cmsgpack.unpack
local write_timer2 = require("tps").write_timer2

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

function for_each2(mod_name,entry,params_list)
    local result = {}

    local cnt = #params_list
    local timeout = 3000
    local process = {}
    process = spawn_cnt(cnt,"parellel","main_loop2",timeout)

    local params = {}
    params["mod_name"] = mod_name
    params["entry"] = entry
    for i,addr in pairs(process) do
        params["id"] = 1
        params["params"] = params_list[i]
        rpc_send(addr, params)
    end
    --[[
    for i,addr in pairs(process) do
        rpc_recv()
    end
    ]]--
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

--params = {
--      ["id"] = id,
--      ["mod_name"] = mod_name,
--      ["entry"] = entry,
--      ["params"] = params}
--
--- mod_name[entry](params)
--- return ret,err_msg
function rpc_send(addr,params)
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

end

function rpc_recv()
    local timeout = timeout or 3000
    local mtype,desc,rec_msg = glr.recv(timeout)

    if desc and rec_msg then
        rec_msg = unpack(rec_msg)
        if desc.addr and desc.addr.gpid == id and desc.attr.corrid == 2 then
            --pprint(rec_msg,"---- recev ----")
            result["id"] = rec_msg["id"]
            result["process"] = rec_msg["process"]
            result["error"] = rec_msg["error"]
            result["result"] = rec_msg["result"]
        end
    else
        print(string.format("receive %s:%s:%d error : %s",addr.host,addr.port,addr.gpid, err_msg))
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
            pprint("--- main_loop receive ----",rec_msg["id"])
            local mod_name = require(rec_msg["module"]["mod_name"])
            local entry = rec_msg["module"]["entry"]
            if mod_name and mod_name[entry] then
                --print("mod_name[entry]:",mod_name[entry])
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
                print("--- main_loop send ----",send_msg["id"])
                glr.send(desc.addr,packer(send_msg),attr)
            else
                local err_msg = "mod_name or entry doesn't exist"
                local send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["error"]=err_msg,
                        ["result"]=nil}
                local attr = {corrid = 2, msgid = 1}
                print("--- main_loop send ----",send_msg["id"])
                glr.send(desc.addr,packer(send_msg),attr)
            end
        else
            goto beg
        end
    end
end

function main_loop2(timeout)
    while true do
        ::beg::
        local timeout = timeout or 3000
        local mtype,desc,rec_msg = glr.recv(timeout)
        if rec_msg and desc and desc.attr and desc.attr.corrid == 1 then
            rec_msg = unpack(rec_msg)
            pprint("--- main_loop receive ----",rec_msg["id"])
            local mod_name = require(rec_msg["module"]["mod_name"])
            local entry = rec_msg["module"]["entry"]
            if mod_name and mod_name[entry] then
                --print("mod_name[entry]:",mod_name[entry])
                local func = mod_name[entry]
                local ret,err_msg = func(rec_msg["module"]["params"])
                --pprint(ret,"ret")
                local len = #ret
                local gpid = rec_msg["process"]["id"]
                local  pwd = os.getenv("PWD")
                local path = string.format("%s/timer/%d_%d",pwd, gpid,len)
                print("path", path)
                write_timer2(gpid, ret, path)
                local send_msg = {}
                if ret then
                    send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["result"]=ret,
                        ["error"] ="nil"
                    }
                else
                    send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["result"]="nil",
                        ["error"]=err_msg
                    }
                end
                local attr = {corrid = 2, msgid = 1}
                print("--- main_loop send ----",send_msg["id"])
                --pprint(" result ", ret)
                glr.send(desc.addr,packer(send_msg),attr)
            else
                local err_msg = "mod_name or entry doesn't exist"
                local send_msg = {
                        ["id"] = rec_msg["id"],
                        ["process"] = rec_msg["process"],
                        ["error"]=err_msg,
                        ["result"]=nil}
                local attr = {corrid = 2, msgid = 1}
                --print("--- main_loop send ----",send_msg["id"])
                glr.send(desc.addr,packer(send_msg),attr)
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
    for i = 1, 200 do
        params_list[i] = {"http://127.0.0.1:8080/static/index.html",1}
    end

    local result = for_each("test_http","http_conn",params_list)

    pprint(result,"result")
end

function test_for_each2()
    local timeout = 3000
    local timer = {}
    local params_list = {}
    for i = 1, 2 do
        params_list[i] = {"http://127.0.0.1:8080/static/index.html",5}
    end

    for_each2("test_http","http_conn",params_list)

end

if ... == "__main__" then
    --local ret, err_msg = glr.spawn("parellel","main_loop",timeout)
    --if not ret then
    --    error("spawn error")
    --end

    test_for_each2()
    --glr.exit()
end
