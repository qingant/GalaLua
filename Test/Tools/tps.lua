--[[
	Author : Liu Wenxue
	Date   : 2014/08/08

	== Introduction ==
    statics tps of http_connect

    == TODO ==
    logging
]]
module(...,package.seeall)

function write_timer(ret,path)
    local path = path or os.getenv("PWD") .. "/timer." .. #ret
    os.execute(string.format("mkdir -p %s",string.match(path,".*/")))
    local fd = assert(io.open(path, "w"))

    for i,k in pairs(ret) do
        --print("i",i,"k",k)
        for j,v in pairs(k) do
            local bg = v["timer"]["begin"]
            local ed = v["timer"]["end"]
            local ret = v["result"]
            fd:write(string.format("processID %d index %d begin %d  end %d  gap %d ret %s\n",i,j,bg, ed, ed-bg, ret))
        end
    end
    fd:close()
end

function tps(timer_path, tps_path)
    local BUFSIZE = 2^13
    local path_timer = assert(timer_path)
    local fd_timer = assert(io.open(timer_path, "r"))

    local process = {}
    local index = {}
    local bg = {}
    local ed = {}
    local gap = {}

    local i = 1
    --local lines, rest = fd_timer:read(BUFSIZE, "*line")
    local lines, rest = fd_timer:read("*line")
    while lines do
        process[i], index[i], bg[i], ed[i], gap[i] = string.match(lines, "([0-9]+)%D*([0-9]+)%D*([0-9]+)%D*([0-9]+)")
        --print("lines : " .. lines .. "rest")
        lines = fd_timer:read("*line")
        i = i + 1
    end

    fd_timer:close()


    local tps = {}
    local index = 1
    for i, k  in pairs(bg) do
        if bg[i] == bg[i+1] then
            tps[index] = (tps[index] or 1) + 1
        else
            index = index + 1
            tps[index] = 1
        end
    end

    local path_tps = assert(tps_path)
    local fd_tps = assert(io.open(path_tps, "w"))
    --for i, k in pairs(tps) do
    local sum_tps = 0
    local average_tps = 0
    for i = 1,#tps-1 do
        sum_tps = sum_tps + tps[i]
        fd_tps:write(string.format("seconds %d tps %d\n", i, tps[i]))
    end
    average_tps = sum_tps / (#tps - 1)
    fd_tps:write(string.format("average_tps: %.2f\n",average_tps))
    fd_tps:close()
end

