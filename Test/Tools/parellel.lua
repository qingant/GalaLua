
module(..., package.seeall)

local pprint = require("pprint").pprint
local cmsgpack = require("cmsgpack")
local packer = cmsgpack.pack
local unpack = cmsgpack.unpack

function for_each1(mod_name,entry,map)
    local timeout = 3000
    local result = {}
    for proc,params in pairs(map) do
        local ret, err_msg = glr.spawn("parellel","http_conn",timeout)
        if not ret then
            error("spawn error")
        end
    end
end

function for_each(mod_name,entry,map)
    local timeout = 3000
    local result = {}
    for proc,params in pairs(map) do
        local ret, err_msg = glr.spawn("test_http","main_loop",timeout)
        if not ret then
            error("spawn error")
        end

        local addr = ret
        msg = {["module"] = mod_name,["entry"]=entry,["params"] = params}
        glr.send(addr, packer(msg),{corrid = 1})
        local timeout = timeout or 3000
        local mtype,desc,msg = glr.recv(timeout)
        msg = unpack(msg)
        if desc.attr.corrid == 2 then
            result[proc] = {}
            result[proc]["error"] = msg["error"]
            result[proc]["result"] = msg["result"]
        end
    end
end

function main_loop(timeout)
    while true do
        ::beg::
        local timeout = timeout or 3000
        local mtype,desc,msg = glr.recv(timeout)
        if desc and desc.attr.corrid == 1 then
            msg = unpack(msg)
            local mod_name = require(msg["module"])
            local entry = msg["entry"]
            if mod_name and mod_name[entry] then
                local func = mod_name.entry
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

function test()
    local mod_name = require("test_http")
    local entry = "http_conn"
    local func = mod_name[entry]
    --pprint(func({},1,"http://127.0.0.1:8080/statics/index.html"))
    --pprint(func(map[1]))
end

if ... == "__main__" then
    --local ret, err_msg = glr.spawn("parellel","main_loop",timeout)
    --if not ret then
    --    error("spawn error")
    --end

    local timeout = 3000
    local map = {
        {{},1,"http://127.0.0.1:8080/statics/index.html"},
        {{},2,"http://127.0.0.1:8080/statics/index.html"},
        {{},3,"http://127.0.0.1:8080/statics/index.html"},
        {{},4,"http://127.0.0.1:8080/statics/index.html"},
        {{},5,"http://127.0.0.1:8080/statics/index.html"},
    }


    for_each("test_http","http_conn",map)
end
