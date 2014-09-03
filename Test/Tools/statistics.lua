--[[
	Author : Liu Wenxue
	Date   : 2014/09/01

	== Introduction ==
    static test_http infomation

    == TODO ==
    logging
]]
module(...,package.seeall)
local tps = require(_PACKAGE .. "tps").tps

function gen_timer_path(timer_path)
    local timer_path = assert(timer_path)
    os.execute("for i in `ls timer/timer_*`; do cat $i >> timer/timer_tmp; done")
    os.execute(string.format("cat timer/timer_tmp | sort -t \" \" -k 6 >> %s",timer_path))
end

function error_statistics(timer_path,err_path)
    local timer_path = assert(timer_path)
    local pfd = assert(io.popen(string.format("cat %s | grep \"processID\" | wc -l ",timer_path)))
    local total_request = tonumber(pfd:read("*l"))
    pfd:close()

    pfd = assert(io.popen(string.format("cat %s | grep \"ret:false\" | wc -l",timer_path)))
    local total_error = tonumber(pfd:read("*l"))
    pfd:close()

    pfd = assert(io.popen(string.format("cat %s | grep -E \"(init|recv)Line timeout\" | wc -l",timer_path)))
    local recvline_timeout = tonumber(pfd:read("*l"))
    pfd:close()

    pfd = assert(io.popen(string.format("cat %s | grep \"recv timeout\" | wc -l",timer_path)))
    local recv_timeout = tonumber(pfd:read("*l"))
    pfd:close()

    pfd = assert(io.popen(string.format("cat %s | grep \"SendWait timeout\" | wc -l",timer_path)))
    local socket_timeout = tonumber(pfd:read("*l"))
    pfd:close()

    err_path = assert(err_path)
    local fd = assert(io.open(err_path,"a+"))
    fd:write(string.format("total request,%d\n", total_request))
    fd:write(string.format("total error,%d\n", total_error))
    fd:write(string.format("glr.recvLine() timeout,%d\n", recvline_timeout))
    fd:write(string.format("glr.recv() timeout,%d\n", recv_timeout))
    fd:write(string.format("socket send timeout,%d\n", socket_timeout))
    fd:close()

end

function complete_statistics(timer_path,complete_path)
    local pwd = os.getenv("PWD")
    local timer_path = assert(timer_path)
    local pd = assert(io.popen(string.format("cat %s | wc -l",timer_path),"r"))
    local lines = tonumber(pd:read("*a"))

    os.execute(string.format("cat %s | sort -t \" \" -k 7 >> timer/timer_complete",timer_path))
    local fd = assert(io.open(timer_path,"r"))
    local line =  fd:read("*l")
    local a,b,begin_secs = string.match(line,".-([0-9]+).-([0-9]+).-([0-9]+)")
    fd:close()
    --print("begin_secs: ",begin_secs)

    fd = assert(io.open(string.format("%s/timer/timer_complete",pwd),"r"))
    line =  fd:read("*l")
    local end_sec = 0
    --a,b,_, end_secs= string.match(line,".-([0-9]+).-([0-9]+).-([0-9]+)")

    local fd_process = assert(io.open(assert(complete_path), "a+"))
    fd_process:write(string.format("total request,%d,,,\n", lines))
    print(string.format("Total request : %d", lines))

    local int,mod = math.modf(lines / 10)
    local times = 1
    for i=1, lines do
        --print(i)
        if i == math.floor(int * times + mod * times) then
            a, b, _, end_secs = string.match(line,"([0-9]+).-([0-9]+).-([0-9]+).-([0-9]+)") 
            --print("end_secs: ", end_secs)
            fd_process:write(string.format("complete,%d%%,takes,%d,secs\n",
                        times*10, end_secs - begin_secs + 1))

            print(string.format("complete %d%% takes %d secs",
                        times*10, end_secs-begin_secs))
            line = fd:read("*l")
            times = times + 1
        else
            line = fd:read("*l")
        end
    end
    fd:close()
    fd_process:close()
end

function tps_statistics(timer_path,tps_path)
    local BUFSIZE = 2^13
    local timer_path = assert(timer_path)
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
    local fd_tps = assert(io.open(path_tps, "a+"))
    --for i, k in pairs(tps) do
    local sum_tps = 0
    local average_tps = 0
    for i = 1,#tps-1 do
        sum_tps = sum_tps + tps[i]
        fd_tps:write(string.format("seconds,%d,tps,%d\n", i, tps[i]))
        print(string.format("seconds %d tps %d\n", i, tps[i]))
    end
    average_tps = sum_tps / (#tps - 1)
    fd_tps:write(string.format("average_tps,%.2f,,\n",average_tps))
    print(string.format("average_tps %.2f\n", average_tps))
    fd_tps:close()
end

function static_http()

    local pwd = os.getenv("PWD")
    os.execute(string.format("mkdir -p %s/statistics",pwd))
    local timer_path = string.format("%s/timer/timer",pwd)
    gen_timer_path(timer_path)

    local err_path = string.format("%s/statistics/error_statistics.csv",pwd)
    error_statistics(timer_path,err_path)

    local tps_path = string.format("%s/statistics/tps_statistics.csv",pwd)
    tps_statistics(timer_path,tps_path)

    local complete_path = string.format("%s/statistics/complete_statistics.csv",pwd)
    complete_statistics(timer_path,complete_path)
    os.execute("mv --backup=t timer/ /tmp/")
end

if ... == "__main__" then
    static_http()
    --complete_processing()
end
