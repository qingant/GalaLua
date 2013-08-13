module(..., package.seeall)
local cjson = require("cjson")
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


-- function glrLoggerServer()
--     while true do
--         local msgType, addr, msg = glr.recv()
--         local msgTable = cjson.decode(msg)
--         if msgType == glr.APP then
--             if msgType.Type == "open" then
--             elseif msgType.Type == "close"
--             end
--         end
--     end
-- end
function glrLoggerProcessor( targetPath,id,sizePerFile,Files)
    local filepath = targetPath
    local size = sizePerFile
    local files = Files
    local fileid = 0
    local tagid = id or 0
    local cache = memcached:new():init(1024)
    local fileHandle = io.open(filepath, "a")
    fileHandle:setvbuf("no")
    while true do
        local filesize = fSize(fileHandle)
        local msgType, addr, msg = glr.recv()
        local msglen = #msg
        if filesize  >= size then
            fileid = fileid % files
            local strpath = string.format("%s_%d_%d.%s",getFileNameWithoutExtension(filepath),
	            tagid,fileid,getFileExtension(filepath))
            fileHandle:close()
            os.rename(filepath,strpath)
            fileHandle = io.open(filepath, "a")
            fileHandle:setvbuf("no")
            fileid = fileid + 1
        end
        if msg:sub(1,2) == "!!" or msg:sub(1,2) == "!#" then -- flush
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

GlrLoggerAppender = {}

function GlrLoggerAppender:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function GlrLoggerAppender:init( path,id,size,copys )
    local rt, pid = glr.spawn("log_server", "glrLoggerProcessor", path,id,size,copys)
    self._appender = pid
    self.isclosed = false
    return self
end
function GlrLoggerAppender:reinit(pid,flag)
    self._appender = pid
    self.isclosed = flag
    return self
end
function GlrLoggerAppender:print( msg )
    if self.isclosed == false then
        glr.send(self._appender, msg)
    end
end
function GlrLoggerAppender:flush(  )
    if self.isclosed == false then
        glr.send(self._appender, "!!")
    end
end
function GlrLoggerAppender:close()
    if self.isclosed == false then
        glr.send(self._appender, "!#")
	self.isclosed = true
    end
end
