module(...,package.seeall)

local function trim(s)
    return s:find'^%s*$' and '' or s:match'^%s*(.*%S)'
end

local function commondline(cmd)
    local filehandle = io.popen(cmd)
    local result = filehandle:read("*a")
    filehandle:close()
    return result
end

local function readline(str)
    if type(str) ~= "string" then
        error("the readline para must be string type")
    end
    local states = {}
    local ii = 0
    local index = 1
    while true do
        ii = string.find(str,"\n",ii+1)
        if ii == nil then break end
        local value = string.sub(str,index,ii)
        local key = string.match(value,"%s([^%s]+)%s*$")
        if key == nil then
            error("invalid key from parsering str")
        end
        if states[key] == nil then
            states[key] = {}
        end
        table.insert(states[key],value)
        index = ii+1
    end
    return states
end

--local function split(str)
--    if type(str) ~= "string" then
--       error("the param must be string type")
--    end
--    local res  = {}
--    for token in string.gmatch(str, "[^%s]+") do
--        res [token] = token
--    end
--    return res
--end

local function uname()
    return commondline("uname")
end

local function isListenPortLinux(port)
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

local function getListenStatesLinux(port)
    if isListenPortLinux(port) == false then
        return {}
    end
    local cmd = "netstat -tnl |grep -E '*:"..tostring(port).." '"
    local result = commondline(cmd)
    return readline(result)
end

local function getPortStatesLinux(port)
    if isListenPortLinux(port) == false then
        return {}
    end
    local cmd = "netstat -na |grep -E '*:"..tostring(port).." '"
    local result = commondline(cmd)
    return readline(result)
end

local function getConnectedStatesLinux(port)
    local lsrstates = getListenStatesLinux(port)
    local states = getPortStatesLinux(port)
    local connstates = {}
    for k, _ in pairs(lsrstates) do
        for key, val in pairs(states) do
            if k ~= key then
                connstates[key] =  val
            end
        end
    end
    return connstates
end

local function isListenPortAix(port)
    if type(port) ~= "number" then
        error("invalid type,the port must be number type")
    end
    local cmd = "netstat -Aan  -f inet |grep -E '\\."..tostring(port).." '"
    local result = commondline(cmd)
    result = readline(result)
    if result["LISTEN"] == nil then
        return false
    end
    return true
end

local function getListenStatesAix(port)
    if isListenPortAix(port) == false then
        return {}
    end
    local cmd = "netstat -Aan  -f inet |grep -E '\\."..tostring(port).." '"
    local result = commondline(cmd)
    result = readline(result)
    local res = {}
    for k,v in pairs(result) do
        if k == "LISTEN" then
            res[k] = v
            break
        end
    end
    return res
end

local function getConnectedStatesAix(port)
    if type(port) ~= "number" then
        error("invalid type,the port must be number type")
    end
    local cmd = "netstat -Aan  -f inet |grep -E '\\."..tostring(port).." '"
    local result = commondline(cmd)
    result = readline(result)
    local res = {}
    for k,v in pairs(result) do
        if k ~= "LISTEN" then
            res[k] = v
        end
    end
    return res
end

function isListenPort(port)
    local platform = trim(uname())
    if platform == "Linux" then
        return isListenPortLinux(port)
    elseif platform == "AIX" then
        return isListenPortAix(port)
    else
        error("no support platform")
    end
end

function getListenStates(port)
    local platform = trim(uname())
    if platform == "Linux" then
        return getListenStatesLinux(port)
    elseif platform == "AIX" then
        return getListenStatesAix(port)
    else
        error("no support platform")
    end
end

function getConnectedStates(port)
    local platform = trim(uname())
    if platform == "Linux" then
        return getConnectedStatesLinux(port)
    elseif platform == "AIX" then
        return getConnectedStatesAix(port)
    else
        error("no support platform")
    end
end


