--[[
	Author : Ma Tao
	Date   : 2014/04/14

	== Introduction ==
	GLR Service Framework

]]

__VER__ = "1.0"

module(..., package.seeall)

local cjson = require("cjson") 
local pprint = require("pprint")


server = {}
server.stop_error = {}
server.restart_error = {}

function server:new(  )
	local o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function server:init(name, logger, password)
	self._name = name
	self._logger = logger
    self._password = password
	return self
end

function server:main_loop( ... )
	while true do
		local type,addr,msg = glr.recv()
		local call = cjson.decode(msg)
		if call.method and call.params and self[call.method] then
			local response, error_msg = self[call.method](self,
                                                          call.params,
                                                          addr) 

			if response then
				glr.send(addr, cjson.encode{error="null",
                                            id=call.id,
                                            result=response})
			else
				glr.send(addr, cjson.encode{error=error_msg,
                                            id=call.id,
                                            result="null"})
			end
		else
			-- TODO: error logging
            glr.send(addr, cjson.encode{ error = "no method or method not exist",
                                         id = call.id,
                                         result = "null"
                                       })
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

function server:restart(params, addr)
    if params.password and params.password == self._password then
        error(self.restart_error)
    end

end

function server:main(...)
    ::entry::
    local ret, err = pcall(self.main_loop, self)
    if not ret and err == self.restart_error then
        goto entry
    else
        error(err)
    end
        
end



