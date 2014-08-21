--[[
	Author : Liu Wenxue
	Date   : 2014/08/08

	== Introduction ==
    statics tps of http_connect

    == TODO ==
    logging
]]
module(...,package.seeall)

function write_timer(path,timer)
    print(" ---- ",path,timer)
    local path = path or os.getenv("PWD") .. "/timer." .. #timer
    local fd = assert(io.open(path, "w"))

    for i,k in pairs(timer) do
        --print("i",i,"k",k)
        for j,v in pairs(k) do
            local bg = v["begin"]
            local ed = v["end"]
            fd:write(string.format("processID %d index %d begin %d  end %d  gap %d\n",i,j,bg, ed, ed-bg))
        end
    end
    fd:close()
end

function tps(timer_path, tps_path)
    local BUFSIZE = 2^13
    local path_timer = assert(timer_path)
    local fd_timer = assert(io.open(path_timer, "r"))

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
            tps[index] = (tps[index] or 0) + 1
        else
            index = index + 1
            tps[index] = 0
        end
    end

    local path_tps = assert(tps_path)
    local fd_tps = assert(io.open(path_tps, "w"))
    for i, k in pairs(tps) do
        fd_tps:write(string.format("index %d tps %d\n", i, tps[i]))
    end
    fd_tps:close()
end

function test_timer()
    local timer1 = {
        {
            ["begin"] = 1,
            ["end"] = 2
        },
        {
            ["begin"] = 2,
            ["end"] = 3
        }
    }

    local timer2 = {
        [1] = {
            [1] = {
                ["begin"] = 1,
                ["end"] = 2
            },
            [2] = {
                ["begin"] = 1,
                ["end"] = 2
            },
        },
        [2] = {
            [1] = {
                ["begin"] = 2,
                ["end"] = 3
            }
        }
    }
    local timer_path = os.getenv("PWD") .. "/timer"
    local tps_path = os.getenv("PWD") .. "/tps"
    write_timer(timer_path, timer2)
    tps(timer_path,tps_path)
end

if ... == "__main__" then
    test_timer()
end
