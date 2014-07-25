--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    GalaLua Web Server processor
]]

module(..., package.seeall)



local base = require("core.processor").server
local http = require(_PACKAGE .. "http").http
local response = require(_PACKAGE .. "response").response
local context = require(_PACKAGE .. "context").context
local session_manager = require(_PACKAGE .. "session").session_manager
local pprint = require("pprint")

local processor = base:new()

server = processor

function processor:new(app_name, pool_name, idx, params)
    base.init(self, app_name, pool_name, idx)
    self.urls = params.urls
    self._session_storage_path = params.session_storage_path
    self._session_mgr = session_manager:new():init(self._session_storage_path)
    return self
end
function processor:on_init(...)
    self:_back_to_pool()
    self._logger:debug("back to pool")
    self._handle_cache = {}
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
                    local request = self._protocol:get_request()
                    -- self._logger:debug(pprint.format(request, "request"))
                    local rsp = self:_request_dispatch(request)
                    --pprint.pprint(rsp,"rsp")
                    self._protocol:send_response(rsp)
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

processor.response_404 = response:new():init():set_status_code(404) -- TODO: set status code and error msg
function processor:_request_dispatch(request)
    if self.urls then
        for uri,func in pairs(self.urls) do
            local m = string.match(request.uri, uri)
            if m ~= nil then
                local response = self:_call_hander(func, request)
                return response
            end
        end
    end
    return self.response_404
end

function processor:_get_handle(h)
    if self._handle_cache[h] == nil then
        local mod_name, cls_name = string.match(h, "(.*)%.([^%.]*)")
        local handle_module = require(mod_name)
        local handle_cls = handle_module[cls_name]
        self._handle_cache[h] = handle_cls:new()
    end
    return self._handle_cache[h]
end

function processor:_call_hander(h, request)

    local handle = self:_get_handle(h)
    local context = context:new():init(request,self._session_mgr)

    local response
    if request.method == "GET" then
        response = handle:get(context, params)
    elseif request.method == "POST" then
        response = handle:post(context, params)
    end

    if response then
        context:get_response():set_content(response)
    end
    return context:get_response()
end

