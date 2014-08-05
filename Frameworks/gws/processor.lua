--[[
    Author : Ma Tao
	Date   : 2014/06/26

	== Introduction ==
    GalaLua Web Server processor
]]

module(..., package.seeall)

local base = require("core.processor").server
local http = require("http.http").http
local response = require("http.response").response
local context = require(_PACKAGE .. "context").context
local session_manager = require(_PACKAGE .. "session").session_manager
local path_cls = require("os.path").path
local pprint = require("pprint")

local processor = base:new()

server = processor

function processor:new(app_name, pool_name, idx, params)
    base.init(self, app_name, pool_name, idx) self.urls = params.urls
    self._session_storage_path = params.session_storage_path
    self._static_path  = params.static_path
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
                    pprint.pprint(request,"---request----")
                    local rsp = self:_request_dispatch(request)
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
function processor:_is_static_request(uri)
    local pattern = "^/statics/.*"
    if string.match(uri, pattern) then
        return true
    else
        return false
    end
end

processor.response_404 = response:new():init():set_status_code(404) -- TODO: set status code and error msg
function processor:_request_dispatch(request)
    if self:_is_static_request(request.uri) then
        return self:_static_handle(request)
    else
        if self.urls then
            for uri,func in pairs(self.urls) do
                local m = string.match(request.uri, uri)
                if m ~= nil then
                    local response = self:_call_hander(func, request)
                    return response
                end
            end
        end
    end
    self.response_404:set_content(self.response_404.content)
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

function processor:_read_file_content(path)
    local fd,err,errno = assert(io.open(path,"rb"))
    local content = fd:read("*a")
    io.close(fd)
    return content
end

function processor:is_file_exist(path)
    local fd,err,errno = io.open(path,"rb")
    if fd then
        io.close(fd)
        return true
    else
        return false
    end
end

function processor:is_file_vaild(path)
    print(path,self._static_path,"----compare-----")
    path_cls = path_cls:new():init(path)
    if path_cls:is_prefix_of(self._static_path) and path_cls:isfile(path) then
        print(path,self._static_path,"----match----")
        local fd,err,errno = io.open(path,"rb")
        if fd then
            io.close(fd)
            return true
        else
            return false
        end
    else
        return false
    end
end

function processor:_static_handle(request)
    local CONTENT_TYPE = {["jpg"] = "image/jpg",
                          ["js"] =  "application/x-javascript",
                          ["png"] = "image/png",
                          ["html"] = "text/html",
                          ["css"] = "text/css",
                          ["txt"] = "text/plain"}
    local uri = request.uri
    local fname =  assert(string.match(uri, "^/statics(/.*)"))
    local full_path = self._static_path .. fname
    path_cls = path_cls:new():init(full_path)
    print("full_path",full_path)
    full_path = path_cls:normpath(full_path)
    print("image path : ", full_path)
    if not self:is_file_vaild(full_path) then
        print(self.response_404.status)
        print(self.response_404.statusCode)
        print(self.response_404.content)
        self.response_404:set_content(self.response_404.content)
        return self.response_404
    end
    local response = response:new():init()
    response:set_content(self:_read_file_content(full_path))
    response:set_content_type(CONTENT_TYPE[string.lower(string.match(uri,"%.(%w+)$"))])
    return response
end

