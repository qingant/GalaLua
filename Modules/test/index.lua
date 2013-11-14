module(...,package.seeall)

local httpResponse = require("http").responce
local pprint = require("pprint")

function get(request)
    local rsp = httpResponse:new()
    rsp["Content-Type"] = "text/plain"
    rsp:setContent(pprint.format(request))
    return rsp
end
