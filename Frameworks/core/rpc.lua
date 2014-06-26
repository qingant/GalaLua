--[[
	Author : Ma Tao
	Date   : 2014/04/14

	== Introduction ==
	GLR Service Framework

    == TODO ==
    - replace json with message-pack or some more effecent protocol conditionally
    - working with general logging interface
]]

_VER = "1.0"

module(..., package.seeall)

local cjson = require("cjson")
local cmsgpack = require("cmsgpack")
local pprint = require("pprint")
local RPC_CORRID = require(_PACKAGE .. "const").RPC_CORRID
local client = require(_PACKAGE .. "rpc_client").client

server = {}
server.stop_error = {}
server.restart_error = {}
server.no_response = {}
server.ok = {}
function server:new(  )
	local o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function server:init(name, logger, password)
    if name then
        glr.npt.register(name)
    end
	self._name = name
    self._stamp = glr.time.now()
	self._logger = logger
    self._log_level = 0
    self._password = password
    self._timeout = nil
    self._packer = cmsgpack.pack
    self._unpacker = cmsgpack.unpack
    self._tick = 0
	return self
end
function server:_send(desc, result)
    return glr.send(desc.addr, self._packer({error=nil,
                                             id=nil,
                                             result=result}),
                    {corrid=desc.attr.msgid})
end
function server:_main_loop( ... )
    local packer = self._packer
    local unpacker = self._unpacker
    local response_attr = {corrid=0}
	while true do
        ::beg::
		local mtype,desc,msg = glr.recv(self._timeout)
        self._tick = self._tick + 1
        if mtype == nil then
            if self.on_timeout then
                self:on_timeout()
            end
            goto beg
        elseif  desc.attr.corrid ~= RPC_CORRID then
            if self.on_message then
                self:on_message(mtype, desc, msg)
            end
            goto beg
        end
        local addr = desc.addr
        response_attr.corrid = desc.attr.msgid
        -- local response_attr = {corrid=desc.attr.msgid}
		local call = unpacker(msg)
        -- TODO: method must **NOT** begin with "_"
		if call.method  and self[call.method] then
			local response, error_msg = self[call.method](self,
                                                          call.params,
                                                          desc)

			if response and response ~= self.no_response then
                if response == self.ok then
                    response = nil
                end
				glr.send(addr, packer{error=nil,
                                            id=call.id,
                                            result=response}, response_attr)
            elseif  response == self.no_response then
            else
				glr.send(addr, packer{error=error_msg,
                                            id=call.id,
                                            result=nil}, response_attr)
			end
		else
			-- TODO: error logging
            glr.send(addr, packer{ error = "no method or method not exist",
                                         id = call.id,
                                         result = nil
                                       }, response_attr)
		end
	end
end

function server:stop(params, addr)
    if params.password and params.password == self._password then
        self.stop_error.message = string.format("Server Stoped by %s",
                                                pprint.format(addr))
        error(self.stop_error)
    end
end
local function _collect_methods(t, mt)
    for k,v in pairs(t) do
        if type(v) == "function" and k:sub(1,1) ~= '_' and k:sub(1,2) ~= "on" and k ~= "new" and k ~= "init" then
            mt[#mt + 1] = k
        end
    end
end
function server:methods(params, addr)
    if self._methods then
        return self._methods
    end
    self._methods = {}
    local t = self
    while t do
        _collect_methods(t, self._methods)
        t = getmetatable(t)
    end
    return self._methods
end
function server:restart(params, addr)
    if params.password and params.password == self._password then
        error(self.restart_error)
    end
end

function server:set_log_level(params, desc)
    self._log_level = params.log_level
    return self.ok
end
function server:sys_status(params, desc)
    -- TODO: serve richer info
    return { name = self._name,
             stamp = self._stamp,
             call_times = self._tick}
end

function server:memory_usage(params, desc)
    local kb =  collectgarbage("count")
    return {total=kb*1024}
end
function server:_main(...)
    ::entry::
    if self.on_init then
        self:on_init()
    end
    local ret, err = pcall(self._main_loop, self)
    if not ret and err == self.restart_error then
        goto entry
    elseif not ret and err == self.stop_error then
        error(err)
    else
        print(err)
        -- TODO: logging
        goto entry
    end
end

function _server(mod_name, cls_name, args)
    local o = require(mod_name)[cls_name]:new(unpack(args))
    o:_main()
end
--[[
    {
    mod_name:string
    cls_name:string?
    server_name:string?
    parameters:table
    bind_gpid:number?
    }
]]
local default_server_cls_name = "server"
function create_server(params)
    print("create_server")
    local rt, errmsg
    params.parameters = params.parameters or {}
    if params.bind_gpid then
        rt, errmsg = glr.spawn_ex(params.bind_gpid,
                                        _NAME,
                                        "_server",
                                        params.mod_name,
                                        params.cls_name or default_server_cls_name,
                                        params.parameters)
    else
        print("glr.spawn")
        rt, errmsg = glr.spawn(_NAME,
                               "_server",
                               params.mod_name,
                               params.cls_name or default_server_cls_name,
                               params.parameters)
        print("errmsg",errmsg)
    end
    assert(rt, errmsg)
    local cli = client:new():init(params.server_name or rt)
    return cli
end
function create_client(addr)
    local cli = client:new():init(addr)
    return cli
end
