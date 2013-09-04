module(...,package.seeall)

local sigar = require "sigar"

local function trim(s)
    return s:find'^%s*$' and '' or s:match'^%s*(.*%S)'
end

local function commondline(cmd)
    local filehandle = io.popen(cmd)
    local result = filehandle:read("*a")
    filehandle:close()
    return result
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
        states[#states + 1] = value
        index = ii+1
    end
    return states
end

local function detail_split(str)
    if type(str) ~= "string" then
        error("invalid str ,must string type")
    end
    local items = {}
    for k , v in string.gmatch(str,"(%w+)=(%w+)") do
        items[k] = v
    end
    return items
end

local function split(tbl)
    if type(tbl) ~= "table" then
        error("invalid tbl,must table type")
    end
    local procs = {}
    for k,v in pairs(tbl) do
        local value = detail_split(v)
        table.insert(procs,vale)
    end
    return procs
end

function getprocs(name)
    local cmd = ""
    local awk = "awk '{printf 'User=%s,Pid=%s,CPU=%s,MEM=%s\n',$1,$2,$3,$4}'"
    if type(name) == "string" then
        cmd = "ps aux | grep "..name.." | "..awk
    elseif type(name) == "number" then
        cmd = "ps aux | grep "..tostring(name).." | "..awk
    elseif name == nil then
        cmd = "ps aux | "..awk
    else
        error("invalid name")
    end
    local result = commondline(cmd)
    result = readline(result)
    result = split(result)
    local sig = sigar.new()
    for k ,v in pairs(result) do
        local pid = v["Pid"]
        local proc = sig:procs(tonumber(pid))
        local stime = proc:time().start_time
        v["STime"] = stime
    end
    return result
end
