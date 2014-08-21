--[[
  模块名：log_server
  模块功能：采集告警写入指定文件，并发送告警
  模块接口：1.new(name)  初始化
       2.finalize() 析构
       3.on_timeout() 定时将缓冲中的内容写入文件
       4.log(params)  写日志( parames={level= (int),msg= (str)} )
]]
module(..., package.seeall)

local rpc = require(_PACKAGE .. "rpc")
local cjson = require("cjson")
local pprint = require("pprint")
local bit = require("bit")
local c_flag = require("c_flag")
local cio = require("native.io")
local logger = require(_PACKAGE .. "logger").logger
print("_PACKAGE", _PACKAGE)
server = rpc.server:new()

function server:new(name)
    self:init(name)
    self._file_path = "../../log/65535.log"
    self._timeout = 10000
    self._logger = logger:new():init(self,self._file_path)
    return self
end

function server:__gc()
    self:finalize()
end

function server:finalize()
    self._logger:finalize()
end

function server:_flush()
    self._logger:flush()
end

function server:on_timeout()
    self:flush()
end

function server:log(params)
    local level = params.level
    local msg = params.msg
    print(params.msg)
    if nil == msg then
        print("msg is null")
    end
    if nil == self._file_path then
        print("path is null")
    end
    self:_warn(params.level,params.msg)
    self._logger:_output(params.msg) --这里只负责往buf里写，buf写满了会自动将内容写入log中
end

function server:_warn(level,msg)
end
