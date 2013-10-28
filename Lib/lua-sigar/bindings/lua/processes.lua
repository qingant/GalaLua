module(...,package.seeall)

local sigar = require("sigar")
local osdatetime = require("osdatetime")

local function trim(s)
    if type(s) == "string" then
        return s:match'^%s*(%S.*)'
    end
end

local keys = {
    "proc_user",
    "pid",
    "cpu_usage",
    "mem_usage",
    "start_time",
    "run_time",
    "command"
}

local function commondline(cmd)
    local filehandle = io.popen(cmd)
    filehandle:read("*l")
    local result = filehandle:read("*a")
    filehandle:close()
    return result
end

local function split(str,offset)
    if type(str) ~= "string" then
        error("invalid str ,must string type")
    end
    local items = {}
    local index = 1
    local splits = {}
    for k,_ in string.gmatch(str,"%S+") do
        splits[#splits+1] = k
    end
    local cmd = ""
    for i=1,#splits do
        if i <= 4 then
            items[keys[i]] = splits[i]
        else
            cmd = cmd .. " " .. splits[i]
        end
    end
    items["command"] = cmd
    return items
end

local function readline(str)
    if type(str) ~= "string" then
        error("the readline para must be string type")
    end
    local states = {}
    local ii = 0
    local index = 1
    while true do
        ii = string.find(str,"\n",ii+1)
        if ii == nil then break end
        local value = string.sub(str,index,ii)
        if string.find(value,"grep") == nil then
            states[#states + 1] = split(value,#value)
        end
        index = ii+1
    end
    return states
end

local getpidlist = function (res)
    local retval = {}
    local capture = nil
    local start, sentinel = nil, 0
    repeat
        start, sentinel, capture = string.find(res, 
                "^%s*[%w%_]+%s+(%d+)%s+.-[\r\n]+", sentinel + 1)
        retval[ #retval + 1 ] = capture
    until not start
    return retval
end

function processes_info_by_name_get(name)
    local cmd = "ps -ef "
    if name == nil or name == "" then
    elseif type(name)=="string" then
        cmd = cmd .." | grep -E '"..name.."'"
    elseif type(name)== "number" then
        cmd = cmd .." | grep -E '"..tostring(name).."'"
    elseif type(name) == "boolean" then
        if name == true then
            cmd = cmd.." |grep -E 'true'"
        else
            cmd = cmd.." |grep -E 'false'"
        end
    else
        error("invalid type")
    end
    local res = commondline(cmd)
    if res == nil or res == "" then
        return {}
    end
    -- 获取满足条件的进程的pid
    local pidlist = getpidlist(res)
    pidlist = table.concat(pidlist, ",")
    cmd = "ps -o \"user,pid,pcpu,pmem,args\" -p \"" .. pidlist .. "\""
    res = commondline(cmd)

    local procs = readline(res)
    local sig = sigar.new()
    for k,v in pairs(procs) do
        local pid = v['pid']
        local proc = sig:proc(pid)
        local time,msg = proc:time()
        if time ~= nil then
            v['start_time'] = os.date("%Y-%m-%d %H:%M:%S",time.start_time/1000)
            v['run_time'] = osdatetime.gettimeofday().sec - (time.start_time/1000)
        end
    end
    return procs
end
