module(..., package.seeall)
local cjson = require("cjson")
local pprint = require("pprint")
local memcached = require("memcached").GlrBuffer
--获取文件大小
local function fSize( file )
    local current = file:seek()	-- 获取当前位置
    local size = file:seek("end")   -- 获取文件大小
    file:seek("set", current)
    return size
end
--获取路径
local function getFilePath(filename)
    return string.match(filename, "(.+)/[^/]*%.%w+$")
end
--获取文件名
local function getFileName(filename)
    return string.match(filename, ".+/([^/]*%.%w+)$")
end
--去除扩展名
local function getFileNameWithoutExtension(filename)
    local idx = filename:match(".+()%.%w+$")
    if(idx) then
        return filename:sub(1, idx-1)
    else
        return filename
    end
end

--获取扩展名
function getFileExtension(filename)
    return filename:match(".+%.(%w+)$")
end
function timedFlusher(timeVal, pid)
    while true do
        print("flusher")
        glr.time.sleep(5)
        glr.send(pid, "!!")     -- flush
    end
end
local timedFlushered = 12
function glrLogServerDispatch(info)
    local info = cjson.decode(info)
    local _, timer = glr.spawn_ex(timedFlushered, "log_server", "timedFlusher", 30, __id__)
    local nameDict = {}
    local pidDict = {}
    while true do
        repeat
            local msgType, addr, msg = glr.recv()
            if msgType == glr.EXIT then
                local key = pidDict[addr.gpid]
                if key ~= nil then
                    nameDict[key] = nil
                    pidDict[addr.gpid] = nil
                end
                break
            elseif msgType ~= glr.APP then
                break
            end
            if msg:sub(1,2) == "!@" then -- open logger
                print(msg)
                -- local len = #msg - 2
                local entry = string.sub(msg, 3, #msg)
                local info = cjson.decode(entry)
                pprint.pprint(info)
                if info.path == nil or info.path == "" then
                    info.path = "stdout"
                    if nameDict["stdout"] == nil then
                        local rt, pid = glr.spawn("log_server", "glrStdLoggerProcessor")
                        nameDict["stdout"] = pid
                    end
                elseif nameDict[info.path] == nil then
                    local rt, pid = glr.spawn("log_server", "glrFileLoggerProcessor",info.path,info.size,info.copys)
                    nameDict[info.path] = pid
                    pidDict[pid] = info.path
                end
                local childaddr = {}
                childaddr.host = glr.sys.host
                childaddr.port = glr.sys.port
                childaddr.gpid = nameDict[info.path]
                glr.send(addr,cjson.encode(childaddr))
            elseif msg:sub(1,2) == "!!" then -- flush
                for pid, name in pairs(pidDict) do
                    print("flush", pid, name)
                    glr.send(pid, "!!")
                end
            elseif msg:sub(1,2) == '!*' then
                for pid,name in pairs(pidDict) do
                    print("reset",pid,name)
                    glr.send(pid,"!#")
                end
                nameDict = {}
                pidDict = {}
            end
        until true
    end
end
function glrStdLoggerProcessor()
    while true do
        local msgType, addr, msg = glr.recv()
        if msgType ~= glr.APP then
            break
        end
        if msg:sub(1,2) == "!!" then
        else
            print(msg)
        end
    end
end

function glrFileLoggerProcessor( targetPath,sizePerFile,Files)
    local filepath = targetPath
    local size = sizePerFile
    local files = Files
    local fileid = 0
    local cache = memcached:new():init(1024)
    local fileHandle = io.open(filepath, "a")
    fileHandle:setvbuf("no")
    while true do
        local filesize = fSize(fileHandle)
        local msgType, addr, msg = glr.recv()
        local msglen = #msg
        if filesize  >= size then
            fileid = fileid % files
            local strpath = string.format("%s_%d.%s",getFileNameWithoutExtension(filepath),
                                          fileid,getFileExtension(filepath))
            fileHandle:close()
            os.rename(filepath,strpath)
            fileHandle = io.open(filepath, "a")
            fileHandle:setvbuf("no")
            fileid = fileid + 1
        end
        if msg:sub(1,2) == "!!" or msg:sub(1,2) == "!#" then -- flush
            print("flush!")
            --fileHandle:flush()
            local cachedata = cache:get()
            for _, v in pairs(cachedata) do
                fileHandle:write(v)
                fileHandle:write("\n")
            end
            if msg:sub(1,2) == "!#" then
                return
            end
            cache:clean()
        else
            --fileHandle:write(msg)
            --fileHandle:write("\n")
            if (cache:size() + msglen) >= cache:cap() then
                local cachedata = cache:get()
                for _, v in pairs(cachedata) do
                    fileHandle:write(v)
                    fileHandle:write("\n")
                end
                cache:clean()
            end
            if(msglen >= cache:cap()) then
                fileHandle:write(msg)
                fileHandle:write("\n")
            else
                cache:put(msg)
            end
        end
    end
end


