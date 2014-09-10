--[[
	Author : Liu Wenxue
	Date   : 2014/08/12

	== Introduction ==
    cpu and mem statistics

	== Usage ==
    glr test_load.lua
    glr test_load.lua

    more see function test_cpu and test_men 

    == comment ==
    cpu_statistics_of_cores and cpu_statistics_of_cores2 aren't expected
    maybe fix in the future because cpu_statistics_of_total is enough
]]

module(..., package.seeall)

function sleep(n)
    local clock = os.clock
    local t0 = clock()
    while clock() - t0 < n do end
end

--[[
jiff = {
         ["cpu0"] = {["user"] = user, ...}
         ["cpu1"] = {["user"] = user, ...}
         ...
]]--
function cpu_info_of_cores()
    local cmd = 'cat /proc/stat'
    local fd = assert(io.popen(cmd), string.format("run %s cpu info error ",cmd))
    local line = fd:read("*line")

    local jiff = {}

    while line do
        local core_id,user,nice,system,idle,iowait,irq,softirq = string.match(line,
        "^cpu([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)")
        if core_id then
            core_id,user,nice,system,idle,iowait,irq,softirq =
                tonumber(core_id),tonumber(user),tonumber(nice),tonumber(system),
                tonumber(idle),tonumber(iowait),tonumber(irq),tonumber(softirq)
            --print(string.format("core:%s user:%s nice:%s system:%s idle:%s iowait:%s irq:%s softirq:%s",
            --                core_id,user,nice,system,idle,iowait,irq,softirq))
            jiff[core_id] = jiff[core_id] or {}

            jiff[core_id] = {
                ["user"] = user, ["nice"] = nice, ["system"] = system,
                ["idle"]=idle, ["iowait"] = iowait ,["irq"]=irq, ["softirq"] = softirq}
        end
        line = fd:read("*line")
    end
    fd:close()
    return jiff
end


-- cpu_usage = { 
--      {
--          {["cpu0"] = {["free"] = idle,["total"] = total},
--          {["cpu1"] ={["free"] = idle,["total"] = total}, ...}
--          ...
--      }
--      {
--          {["cpu0"] = {["free"] = idle1,["total"] = total1},
--          {["cpu1"] ={["free"] = idle1,["total"] = total1}, ...}
--          ...
--      }
--}}

function cpu_usage_of_cores(times,time_gap)
    local times = times or 5
    local time_gap = time_gap or 1
    local jiff = {}
    local cpu_usage = {}

    for cnt = 1,times do
        jiff[cnt] = cpu_info_of_cores()
        os.execute("sleep " .. time_gap)
    end

    for cnt,info in pairs(jiff) do
        cpu_usage[cnt] = {}
        for core_id, value in pairs(info) do
            cpu_usage[cnt][core_id] = {}
            cpu_usage[cnt][core_id]["free"] = value["idle"]
            cpu_usage[cnt][core_id]["total"] = value["user"]
                        + value["nice"] + value["system"]
                        + value["idle"] + value["iowait"]
                        + value["irq"] + value["softirq"]
        end
    end
    return cpu_usage
end

--[[

cpu_statistics = {
    {["cpu0"] = 20[%], ["cpu1"] = 30[%],...}
    {["cpu0"] = 40[%], ["cpu1"] = 20[%],...}
    }
]]--
function cpu_statistics_of_cores(times,time_gap)
    local cpu_used_0 = {}
    local cpu_used_1 = {}
    local cpu_total_0 = {}
    local cpu_total_1 = {}
    local test1 = {}
    local test2 = {}
    local cpu_usage = {}
    local cpu_statistic = {}

    cpu_usage = cpu_usage_of_cores()

    for cnt = 2,#cpu_usage do
        cpu_statistic[cnt-1] = {}
        for core_id, info in pairs(cpu_usage[cnt - 1]) do
            cpu_used_0[core_id] = info["free"]
            cpu_total_0[core_id] = info["total"]
        end
        for core_id, info in pairs(cpu_usage[cnt]) do
            cpu_used_1[core_id] = info["free"]
            cpu_total_1[core_id] = info["total"]
        end
        for core_id, info in pairs(cpu_usage[1]) do
            test1[core_id] = cpu_used_1[core_id] - cpu_used_0[core_id]
            test2[core_id] = cpu_total_1[core_id] - cpu_total_0[core_id]
            print("free: ",test1[core_id])--
            print("total: ",test2[core_id])
            print("percent: ",test1[core_id]/test2[core_id])
            cpu_statistic[cnt-1][core_id] = (cpu_used_1[core_id] - cpu_used_0[core_id]) / (cpu_total_1[core_id] - cpu_total_0[core_id]) * 100
            cpu_statistic[cnt-1][core_id] = 100 - cpu_statistic[cnt-1][core_id]
        end
    end

    return cpu_statistic
end

function store_cpu_statistics_of_cores(path,times,time_gap)
    local path = path  or "./cpu_statistic_cores"
    local fd = assert(io.open(path,"w"))
    for i,info in pairs(cpu_statistics_of_cores(times,time_gap)) do
        for core_id,percent in pairs(info) do
            fd:write(string.format("index:%d core_id:%s percent:%.1f %%\n",i,core_id, percent))
        end
    end
    fd:close()
end

--[[
jiff = {
         ["cpu0"] = { {["user"] = user, ...}, {["nice"] = nice, ...}, ...}
         ["cpu1"] = { {["user"] = user, ...}, {["nice"] = nice, ...}, ...}
         ...
]]--
function cpu_info_of_cores2(times,time_gap)
    local times = times or 5
    local time_gap = time_gap or 1
    local cmd = 'cat /proc/stat'
    local jiff = {}

    for cnt = 1, times do
        local fd = assert(io.popen(cmd), string.format("run %s cpu info error ",cmd))
        local line = fd:read("*line")
        while line do
            local core_id,user,nice,system,idle,iowait,irq,softirq = string.match(line,
            "^cpu([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)")
            core_id,user,nice,system,idle,iowait,irq,softirq =
                    tonumber(core_id),tonumber(user),tonumber(nice),tonumber(system),
                    tonumber(idle),tonumber(iowait),tonumber(irq),tonumber(softirq)
            if core_id then
                --print(string.format("core:%s user:%s nice:%s system:%s idle:%s iowait:%s irq:%s softirq:%s",
                --                core_id,user,nice,system,idle,iowait,irq,softirq))
                jiff[core_id] = jiff[core_id] or {}

                jiff[core_id][cnt] = {
                    ["user"] = user, ["nice"] = nice, ["system"] = system,
                    ["idle"]=idle, ["iowait"] = iowait ,["irq"]=irq, ["softirq"] = softirq}
            end
            line = fd:read("*line")
        end
        fd:close()
        os.execute("sleep " .. tonumber(time_gap))
    end

    return jiff
end


function cpu_usage_of_cores2(times,time_gap)
    local jiff = cpu_info_of_cores2(times,time_gap)
    local cpu_usage = {}
    for core_id,info in pairs(jiff) do
        cpu_usage[core_id] = {}
        for cnt,value in pairs(info) do
            cpu_usage[core_id][cnt] = {}
            cpu_usage[core_id][cnt]["free"] = value["idle"]
            cpu_usage[core_id][cnt]["total"] = value["user"]
                        + value["nice"] + value["system"]
                        + value["idle"] + value["iowait"]
                        + value["irq"] + value["softirq"]
        end
    end
    return cpu_usage
end


--[[

cpu_used = {
        ["cpu0"] = {20, 30,40,...}
        ["cpu1"] = {20, 30,40,...}
    }
]]--
function cpu_statistics_of_cores2(times,time_gap)
    local jiff = cpu_info_of_cores2(times,time_gap)
    local cpu_used_0 = {}
    local cpu_used_1 = {}
    local cpu_total_0 = {}
    local cpu_total_1 = {}

    local cpu_statistics = {}

    for core_id,info in pairs(cpu_usage_of_cores2(times, time_gap)) do
        cpu_statistics[core_id] = {}
        for i = 2,#info do
            cpu_used_0 = info[i-1]["free"]
            cpu_used_1 = info[i]["free"]
            cpu_total_0 = info[i-1]["total"]
            cpu_total_1 = info[i]["total"]

            cpu_statistics[core_id][i-1] = 100 - (cpu_used_1 - cpu_used_0) /
                                (cpu_total_1 - cpu_total_0) * 100
        end
    end
    return cpu_statistics
end

function store_cpu_statistics_of_cores2(path,times, time_gap)
    local path = path  or "./cpu_cores2_statistic"
    local fd = assert(io.open(path,"w"))
    for core_id,info in pairs(cpu_statistics_of_cores2(times,time_gap)) do
        for i,percent in pairs(info) do
            fd:write(string.format("core_id:%d index:%s percent:%.1f %%\n",core_id,i,percent))
        end
    end
    fd:close()
end

-- jiff = {["user"] = user, ["nice"] = nice}
function cpu_info_of_total()
    local cmd = 'cat /proc/stat'
    local fd = assert(io.popen(cmd), string.format("run %s cpu info error ",cmd))
    local line = fd:read("*line")
    local user,nice,system,idle,iowait,irq,softirq = string.match(line,
    "^cpu%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)%s+([0-9]+)")
    user,nice,system,idle,iowait,irq,softirq =
            tonumber(user),tonumber(nice),tonumber(system),
            tonumber(idle),tonumber(iowait),tonumber(irq),tonumber(softirq)

    --print(user,nice,system,idle,iowait,irq,softirq)
    local jiff = {}
    jiff = {
        ["user"] = user, ["nice"] = nice, ["system"] = system,
        ["idle"]=idle, ["iowait"] = iowait ,["irq"]=irq, ["softirq"] = softirq }
    return jiff
end

-- cpu_used = {{["free"] = 1233, ["total"] = 124344} {["free"] = 1233, ["total"] = 124344}}
function cpu_usage_of_total(times, time_gap)
    local times = times or 5
    local time_gap = time_gap or 1
    local cpu_used = {}
    local cpu_info = {}
    for i = 1, times do
        cpu_info[i] = cpu_info_of_total()
        os.execute("sleep " .. time_gap)
    end

    for i = 1, times do
        cpu_used[i] = {}
        cpu_used[i]["free"] = cpu_info[i]["idle"]
        cpu_used[i]["total"] = cpu_info[i]["user"]
                        + cpu_info[i]["nice"] + cpu_info[i]["system"]
                        + cpu_info[i]["idle"] + cpu_info[i]["iowait"]
                        + cpu_info[i]["irq"] + cpu_info[i]["softirq"]
    end

    return cpu_used
end

function cpu_statistics_of_total(times, time_gap)
    local cpu_usage  = cpu_usage_of_total(times,time_gap)
    local cpu_used_0 = 0
    local cpu_used_1 = 0
    local cpu_total_0 = 0
    local cpu_total_1 = 0
    local cpu_statistics = {}

    for i=1,#cpu_usage-1 do
        cpu_used_0 = cpu_usage[i]["free"]
        cpu_used_1 = cpu_usage[i+1]["free"]
        cpu_total_0 = cpu_usage[i]["total"]
        cpu_total_1 = cpu_usage[i+1]["total"]

        cpu_statistics[i] =
                (cpu_used_1 - cpu_used_0) * 100 /
                (cpu_total_1 - cpu_total_0)

        cpu_statistics[i] = 100 - cpu_statistics[i]
    end
    return cpu_statistics
end

function store_cpu_statistics_of_total(path, times, time_gap)
    local path = path  or "./cpu_total_statistic"
    local fd = assert(io.open(path,"w"))
    local cpu_statistics = cpu_statistics_of_total(times,time_gap)
    for i,k in pairs(cpu_statistics) do
        fd:write(string.format("index:%d percent:%.1f %%\n",i,k))
    end
    fd:close()
end

function mem_info()
    local cmd = 'cat /proc/meminfo'
    local fd = assert(io.popen(cmd), string.format("run %s cpu info error ",cmd))
    local line = fd:read("*all")
    local mem_total = tonumber(string.match(line,"MemTotal:%s+([0-9]+)"))
    local mem_free = tonumber(string.match(line,"MemFree:%s+([0-9]+)"))
    return mem_free,mem_total
    --print(mem_total)
    --print(mem_free)
end

-- mem_used = {{"free"="200[KB]","total" = "2000KB"}, {"free"="220[KB]","total" = "2000KB"},...}
function mem_usage(times,time_gap)
    local times = times or 5
    local time_gap = time_gap or 1
    local mem_used = {}
    for i = 1, times do
        mem_used[i] = {}
        mem_used[i]["free"],mem_used[i]["total"] = mem_info()
        os.execute("sleep " .. time_gap)
    end
    return mem_used
end

-- mem_usage = {50[%],60[%],....}
function mem_statistics(times,time_gap)
    local mem_used = mem_usage(times, time_gap)
    local mem_usage = {}
    for i,k in pairs(mem_used) do
        mem_usage[i] = 100 - k["free"]/k["total"] * 100
    end
    return mem_usage
end

function store_mem_statistics(path,times,time_gap)
    local path = path  or "./mem_statistic"
    local fd = assert(io.open(path,"w"))
    for i,k in pairs(mem_statistics(times,time_gap)) do
        fd:write(string.format("index:%d percent:%.1f %%\n",i,k))
    end
    fd:close()
end

----------------------- CPU TEST ----------------------------

function test_cpu_info_of_cores()
    print("-------- test cpu_info_of_cores ---------")
    for core_id,value in pairs(cpu_info_of_cores()) do
        local str = "core_id "
        for i,k in pairs(value) do
            str = string.format("%s %d %s %d",str,core_id,i,k)
        end
        print(str)
    end
end

function test_cpu_usage_of_cores(times, time_gap)
    print("-------- test cpu_usage_of_cores ---------")
    for cnt,value in pairs(cpu_usage_of_cores(times,time_gap)) do
        for core_id,usage in pairs(value) do
            print(string.format("index:%d core_id:%d free:%d total:%d", 
                        cnt,core_id,usage["free"],usage["total"]))
        end
    end
end

function test_cpu_statistics_of_cores(times, time_gap)
    print("-------- test cpu_statistics_of_cores ---------")
    for cnt,value in pairs(cpu_statistics_of_cores(times,time_gap)) do
        for core_id,usage in pairs(value) do
            print(string.format("index:%d core_id:%d percent:%.1f %%", 
                        cnt,core_id,usage))
        end
    end

end

function test_store_cpu_statistics_of_cores(times, time_gap)
    print("-------- test store_cpu_statistics_of_cores ---------")
    local path = "./test/cpu_statistics_of_cores_test"
    os.execute("mkdir -p ./test")
    store_cpu_statistics_of_cores(path,times,time_gap)
    print("store cpu statistics at ",path)
end

function test_cpu_info_of_cores2(times,time_gap)
    print("-------- test cpu_info_of_cores2 ---------")
    for core_id,value in pairs(cpu_info_of_cores2(times,time_gap)) do
        local str = "core_id:" .. core_id
        for i,k in pairs(value) do
            str = "index:" .. i
            for j,v in pairs(k) do
                str = string.format("%s %s %d ",str,j,v)
            end
            print(str)
        end
    end
end


function test_cpu_usage_of_cores2(times,time_gap)
    print("-------- test cpu_usage_of_cores2 ---------")
    for core_id,value in pairs(cpu_usage_of_cores2(times,time_gap)) do
        print("--- core_id --- : ", core_id)
        for i,k in pairs(value) do
            print(string.format("index:%d free:%d total:%d",i,k["free"],k["total"]))
        end
    end
end

function test_cpu_statistics_of_cores2(times,time_gap)
    print("-------- test cpu_statistics_of_cores2 --------")
    for core_id,value in pairs(cpu_statistics_of_cores2(times,time_gap)) do
        local str = "core_id:" .. core_id
        for i,k in pairs(value) do
            print(string.format("core_id:%s index:%d percent:%.1f %%",str,i,k))
        end
    end
end

function test_store_cpu_statistics_of_cores2(times, time_gap)
    print("-------- test store_cpu_statistics_of_cores2 --------")
    local path = "./test/cpu_statistics_of_cores2_test"
    os.execute("mkdir -p ./test")
    store_cpu_statistics_of_cores2(path,times,time_gap)
    print("store cpu statistics at %s",path)
end


function test_cpu_info_of_total()
    print("---- test cpu_info_of_total() ------")
    local str = ""
    for i,k in pairs(cpu_info_of_total()) do
        str = string.format("%s %s %d",str, i,k)
    end
    print(str)
end

function test_cpu_usage_of_total(times,time_gap)
    print("---- test cpu_usage_of_total() ------")
    for i,k in pairs(cpu_usage_of_total(times,time_gap)) do
            print(string.format("index:%d free:%s total:%s",i,
            k["free"],k["total"]))
    end
end

function test_cpu_statistics_of_total(times,time_gap)
    print("---- test cpu_statistics_of_total() ------")
    for i,k in pairs(cpu_statistics_of_total(times,time_gap)) do
        print(string.format("index:%d percent:%.1f %%",i,k))
    end
end

function test_store_cpu_statistics_of_total(times,time_gap)
    print("---- test store_cpu_statistics_of_total() ------")
    local path = "./test/cpu_statistics_of_total_test"
    os.execute("mkdir -p ./test")
    store_cpu_statistics_of_total(path,times,time_gap)
    print("store cpu statistics at ",path)
end

function test_cpu()
    --test_cpu_info_of_total()
    --test_cpu_usage_of_total(5,1)
    --test_cpu_statistics_of_total(5,1)
    --test_store_cpu_statistics_of_total(5,1)

    --test_cpu_info_of_cores()
    --test_cpu_usage_of_cores(5,1)
    --test_cpu_statistics_of_cores(5,1)
    --test_store_cpu_statistics_of_cores(5,1)

    --test_cpu_info_of_cores2()
    --test_cpu_usage_of_cores2(5,1)
    --test_cpu_statistics_of_cores2(5,1)
    test_store_cpu_statistics_of_cores2(5,1)
end

------------------ Memory TEST -------------------

function test_mem_info()
    print("-------  test mem_info ------------")
    local free, total = mem_info()
    print(string.format("free:%dKB  Total: %dKB",free,total))
end

function test_mem_usage(times,time_gap)
    print("-------  test mem_usage ------------")
    for i,k in pairs(mem_usage(times,time_gap)) do
        print(string.format("free:%sKB  total:%sKB ",k["free"],k["total"]))
    end
end

function test_mem_statistics(times,time_gap)
    print("-------  test mem_statistics ------------")
    for i,k in pairs(mem_statistics(times,time_gap)) do
        print(string.format("index:%d  percent:%.1f %% ",i,k))
    end
end

function test_store_mem_statistics(times, time_gap)
    print("-------  test store_mem_statistics ------------")
    local path = "./test/mem_test"
    os.execute("mkdir -p test")
    store_mem_statistics(path,times, time_gap)
    print("store mem statistics at ", path)
end

function test_mem()
    --test_mem_info()
    --test_mem_usage(5,1)
    --test_mem_statistics(5,1)
    test_store_mem_statistics(5,1)
end


--test_cpu()
--test_mem()

test_cpu_statistics_of_cores(5,1)
