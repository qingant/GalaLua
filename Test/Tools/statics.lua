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

function complete_processing()
    local pwd = os.getenv("PWD")
    local pd = assert(io.popen("cat timer/timer | wc -l","r"))
    local lines = tonumber(pd:read("*a"))
    print(string.format("Total request : %d", lines))

    os.execute("cat timer/timer | sort -t \" \" -k 7 >> timer/timer_complete ")
    local fd = assert(io.open(string.format("%s/timer/timer",pwd),"r"))
    local line =  fd:read("*l")
    local a,b,begin_secs = string.match(line,".-([0-9]+).-([0-9]+).-([0-9]+)")
    fd:close()
    --print("begin_secs: ",begin_secs)

    fd = assert(io.open(string.format("%s/timer/timer_complete",pwd),"r"))
    line =  fd:read("*l")
    local end_sec = 0
    --a,b,_, end_secs= string.match(line,".-([0-9]+).-([0-9]+).-([0-9]+)")

    local int,mod = math.modf(lines / 10)
    local times = 1
    for i=1, lines do
        --print(i)
        if i == math.floor(int * times + mod * times) then
            a, b, _, end_secs = string.match(line,"([0-9]+).-([0-9]+).-([0-9]+).-([0-9]+)") 
            --print("end_secs: ", end_secs)
            print(string.format("complete %d%% takes %d secs",
                        times*10, end_secs-begin_secs))
            line = fd:read("*l")
            times = times + 1
        else
            line = fd:read("*l")
        end
    end
end

function static_http()
    os.execute("/bin/sh test_shell")
    local pwd = os.getenv("PWD")
    local timer_path = string.format("%s/timer/timer",pwd)
    local tps_path = string.format("%s/tps/tps",pwd)
    tps(timer_path, tps_path)
    os.execute("cat tps/tps")
    complete_processing()
    os.execute("mv --backup=t tps/ timer/ /tmp/")
end

if ... == "__main__" then
    static_http()
    --complete_processing()
end
