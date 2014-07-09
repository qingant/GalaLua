--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    GalaLua Web Server processor
]]

module(..., package.seeall)

local base = require("core.processor").server
local http = require("http").http
local response = require("http").response
local pprint = require("pprint")
local processor = base:new()

server = processor

function processor:new(app_name, pool_name, idx, params)
    base.init(self, app_name, pool_name, idx)
    self.urls = params.urls
    return self
end

function processor:on_init(...)
    self:_back_to_pool()
    self._logger:debug("back to pool")
end

function processor:on_task_add(params, desc)
    self._logger:debug("task_add")
    self._fd = params.fd
    self._protocol = http:new():init(self._fd)
    glr.net.poll(self._fd)
end

function processor:on_message(mtype, desc, msg)
    if mtype == glr.IOCP then
        print("IOCP...", self._fd)
        local r, err
        while true do
            r, err = pcall(function ()
                    local request = self._protocol:getRequest()
                    -- self._logger:debug(pprint.format(request, "request"))
                    local rsp = self:_request_dispatch(request)
                    self._protocol:sendResponse(rsp)
                                 end, debug.traceback)
            if not r then
                break
            end
        end
        print("ERROR", err)
        self:_back_to_pool()
        -- glr.net.close(self._fd)
        -- self:_back_to_pool()
    end
end

function processor:_request_dispatch(request)
    -- TODO: url matching
    pprint.pprint(self.urls, "urls")
    if self.urls then
        local p, h
        for i,p in ipairs(self.urls) do
            p, h = unpack(p)
            local m = {string.match(request.uri, p)}
            if m then
                local rsp = self:_call_hander(h, request)
                return rsp
            end
        end
    end
    local rsp = response:new()
    rsp.statusCode = 404
    rsp.status = "Service Not Found"
    return rsp
end

function processor:_call_hander(h, request)
    print(h, request)
    -- TODO: load handler and cache
    local rsp = response:new()
    rsp["Content-Type"] = "text/plain"
    rsp:setContent("Hello World")
    return rsp
end