-- yet another web server
-- author:Ma Tao
module(...,package.seeall)


server = {}

function server:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function server:init(server, host)
    self._server = server
    self._host = host           -- for virtual server support/currently useless
    return self
end


function server:setUriMap(map)
    self._map = map
end

function server:_mainLoop()
    while true do
        pprint.pprint(self, "Server")
        local conn = self._server:accept()
        glr.spawn("yaws.server", "worker", conn)
    end
end
function server:mainLoop()
    assert(xpcall(server._mainLoop, debug.traceback, self))
end
local http = require("http")
function worker(server, fd)
    local protocol = http.http:new():init(fd)
    while true do
        local request = protocol:getRequest()
        for k,v in pairs(server._map) do
            if request.uri:match(k) then
                callHttpHandle(v, {request = request, protocol = protocol})
                break
            end
        end
        if request["Connection"] == "close" then
            protocol._socket:close()
            return
        end
    end
end

function callHttpHandle(name, context)
    local module = require(name)
    local request = context.request
    local method
    if request.method == "GET" then
        method = "get"
    elseif request.method == "POST" then
        method = "post"
    end
    local err, rsp = xpcall(module["method"], debug.traceback, context)
    if not err then
        rsp = http.response:new()
        rsp.statusCode = "500"
        rsp.status = "Internel Error"
        rsp["Content-Type"] = "text/plain"
        rsp:setContent(rsp)
    end
    context.protocol:sendResponse(rsp)
end
