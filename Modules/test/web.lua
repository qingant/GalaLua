module(...,package.seeall)
local yaws = require("yaws.server")
local socket = require("glr.socket")
local httpResponse = require("http").responce
local pprint = require("pprint")
function main()
    local tcpServer = socket.socket:new():init(glr.net.tcp_server("0.0.0.0", 8080))
    pprint.pprint(tcpServer, "tcpServer")
    local server = yaws.server:new():init(tcpServer)
    server:setUriMap({
                         ["/"] = "index"
                     })
    server:mainLoop()
end



if ... == "__main__" then
    main()
end
