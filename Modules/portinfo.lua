module(...,package.seeall)

local function commondline(cmd)
    local filehandle = io.popen(cmd)
    local result = filehandle:read("*a")
    filehandle:close()
    return result
end

function isListenPort(port)
    if type(port) ~= "number" then
        error("invalid type,the port must be number type")
    end
    local cmd = "netstat -tnl |grep -E '*:"..tostring(port).." '"
    local result = commondline(cmd)
    if result == "" or 0 == #result then
        return false
    else
        return true
    end
end

function getPortStates(port)
    local states = {}
    if isListenPort(port) == false then
        return states
    end
    local cmd = "netstat -na |grep -E '*:"..tostring(port).." '"
    local result = commondline(cmd)
    local ii = 0
    local index = 1
    while true do
        ii = string.find(result,"\n",ii+1)
        if ii == nil then break end
        states[#states+1] = string.sub(result,index,ii)
        index = ii+1
    end
    return states
end
