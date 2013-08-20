module(...,package.seeall)

local sigar = require("sigar")
local osdatetime = require("osdatetime")

function processes_info_get()
    local sig = sigar.new()
    local pids = sig:procs()
    local result = {}
    local dvmem = sig:mem()
    local cpus = sig:cpus()
    local cup_total = 0
    for i=1,#cpus do
       local data = cpus[i]:data()
       cpu_total = cup_total + data.total
    end
    for i = 1,#pids do
        local proc = sig:proc(pids[i])
        if proc ~= nil then
            local item = {}
            local cred,msg = proc:cred()
            item["user"] = cred.user
            item["pid"] = pids[i]
            local time,msg = proc:time()
            item["cpu"] = (time.user + time.sys)/cpu_total
            local mem,msg = proc:mem()
            item["mem"] = mem.size / dvmem.total
            item["stime"] = os.date("%Y-%m-%d %H:%M:%S",time.start_time/1000)
            item["time"] = osdatetime.gettimeofday().sec - (time.start_time/1000)
            result[#result + 1] = item
        end
    end
    return result
end

function processes_info_by_name_get(name)
    if name == nil then
        error("name:invalid type")
    end
    local infos = processes_info_get()
    local result = {}
    for _,v in pairs(infos) do
        for _,val in pairs(v) do
            if val == name then
                table.insert(result,v)
                break
            end
        end
    end
    return result
end