module(..., package.seeall)

local sigar = require("sigar")
local osdatetime = require("osdatetime")

local io = require("io")
local string = require("string")

local type = type
local tostring = tostring

local getotherinfo = function ()
    local pattern = {
        [1] = "(%d+)%s*",
        [2] = "([%w%_%-%.]+)",
        [3] = "([%d%.%-%%]+)",
        [4] = "([%d%.%-%%]+)",
        [5] = "([^%s]+)",
    }
    local keyword = {
        [1] = "pid",
        [2] = "proc_user",
        [3] = "cpu_usage",
        [4] = "mem_usage",
        [5] = "command"
    }
    local start, sentinel, capture, pid = nil, nil, nil, nil
    local retval = {}
    local command = "ps -e -o 'pid=' -o 'user=' -o 'pcpu=' -o 'pmem=' -o 'args='"
    local handle = io.popen(command, "r")
    for content in handle:lines() do
        start, sentinel, pid = string.find(content, pattern[1], 1)
        if start then
            retval[pid] = {[keyword[1]] = pid}
            for idx = 2, #pattern - 1, 1 do
                start, sentinel, capture = string.find(content, pattern[idx],
                        sentinel + 1)
                if start then
                    retval[pid][keyword[idx]] = capture
                end
            end
            start, sentinel, capture = string.find(content, pattern[5], sentinel + 1)
            if start then
                retval[pid][keyword[5]] = string.sub(content, start,
                        start + 64)
            end
        end
    end
    handle:close()
    return retval
end

local getprocbyname = function (procname)
    local procname = procname
    if type(procname) == "boolean" then
        if procname then
            procname = "true"
        else
            procname = "false"
        end
    else
        procname = tostring(procname)
    end
    local otherinfo = getotherinfo()
    local start, sentinel, capture = nil, nil, nil
    local retval = {}
    local pattern = "^%s*[%w%_%-%.]+%s+(%d+)"
    local command = "ps -ef | grep -E '" .. procname .. "'"
    local handle = io.popen(command, "r")
    handle:read("*l")
    for content in handle:lines() do
        start, sentinel, capture = string.find(content, pattern, 1)
        if otherinfo[capture] then
            start, sentinel = string.find(content,
                    otherinfo[capture]["command"], sentinel + 1, true)
            otherinfo[capture]["command"] = string.sub(content, start)
            retval[#retval + 1] = otherinfo[capture]
        end
    end
    handle:close()
    return retval
end

function processes_info_by_name_get(procname)
    local retval = getprocbyname(procname)
    local sigar = sigar.new()
    local proc = nil
    local time, msg = nil, nil
    for idx, record in ipairs(retval) do
        proc = sigar:proc(record["pid"])
        time, msg = proc:time()
        if time then
            record["start_time"] = os.date("%Y-%m-%d %H:%M:%S",
                    time.start_time / 1000)
            record["run_time"] = osdatetime.gettimeofday().sec
                    - (time.start_time / 1000)
        end
    end
    return retval
end
