--[[
	Author : Liu Wenxue
	Date   : 2014/08/08

	== Introduction ==
    test webframework with http_client

]]

module(...,package.seeall)
local pprint = require("pprint").pprint
local httpClient = require("http_client").httpClient
local httpRequest = require("http_client").httpRequest

function main(timer, times, mod_name, entry,...)
    for i = 1, times do
        timer[i] = {}
        timer[i]["begin"] =  glr.time.now()
        local ret, err = glr.spawn(mod_name, entry,...)
        if err then
            pprint(err,"err")
        end
        timer[i]["end"] = glr.time.now()
        --glr.npt.registered()
    end
end

function http_connect(url)
    local cli = httpClient:new()
    local req = httpRequest:new():init("GET", url)
    local res = cli:get_Response(req,20)
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


if ... == "__main__" then
    local mod_name = "test_http"
    local entry = "http_connect"
    local url = "http://127.0.0.1:8080/statics/index.html"
    local timer =  {}
    local times = 10 

    main(timer, times, mod_name, entry, url)
    write_timer(timer, times)
    tps(times)
    --print(http_connect(url))
end
