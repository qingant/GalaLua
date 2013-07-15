
local ARCHIVE_SUFFIX_NAME = ".a"
local LUA_PCK_SUFFIX_NAME = ".lua"
local MAKEFILE_STATIC_PATH = "/opt/agree/gdk/share/Makefile.static"

local function basename(pathname)
    local retval = string.reverse(pathname)
    retval = string.find(retval, "/")
    return string.sub(pathname, -retval + 1)
end

local function suffix(pathname)
    local retval = basename(pathname)
    local idx = string.find(retval, "%.")
    if idx then
        return string.sub(retval, idx)
    else
        return nil
    end
end

local function analysis(pathname)
    local retval = {["scripts"] = {}, ["archives"] = {}}
    local handle = io.open(pathname, "r")
    for var in handle:lines() do
        var = string.gsub(var, "^%s*(.-)%s*$", "%1")
        if string.len(var) > 0 then
            local suffixname = suffix(var)
            if suffixname == ARCHIVE_SUFFIX_NAME then
                retval["archives"][#retval["archives"] + 1] = var
            elseif suffixname == LUA_PCK_SUFFIX_NAME then
                retval["scripts"][#retval["scripts"] + 1] = var
            elseif not suffixname then
                retval["scripts"][#retval["scripts"] + 1] = var
            end
        end
    end
    return retval
end

local function makeagent(args)
    local scripts = ""
    local archives = ""
    if args[1] then
        local retval = analysis(args[1])
        scripts = table.concat(retval["scripts"], " ")
        scripts = "USER_LUA_SCRIPT=" .. "'" .. scripts .. "'"
    
        archives = table.concat(retval["archives"], " ")
        archives = "USER_AR_LIBS=" .. "'" .. archives .. "'"    
    end
    retval = {"make -f", MAKEFILE_STATIC_PATH, scripts, archives}
    local make = table.concat(retval, " ")
    local clean = "make -f " .. MAKEFILE_STATIC_PATH .. " clean"
    if os.execute(clean) == 0 and os.execute(make) == 0 and args[2] == "install" then
        retval = {"make -f", MAKEFILE_STATIC_PATH, args[2]}
        os.execute(table.concat(retval, " "))
    end
end

makeagent(arg)

