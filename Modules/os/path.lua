--[[
    Author : Liu Wenxue
	Date   : 2014/08/04

	== Introduction ==
    path similar as python os.path

    == common ==
    the symbolic links don't support

    == todo ===
    file privilege control
]]

module(...,package.seeall)
--TODO parse pPattern replace %. as %s\ 
--@param : pString : str what to split
--@param : pPattern : character is splited by
--@return : table  like python split

function split(pString, pPattern)
    local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
    local fpat = "(.-)" .. pPattern
    local last_end = 1
    local s, e, cap = string.find(pString, fpat, 1)

    while s do
        table.insert(Table,cap)
        last_end = e+1
        s, e, cap = string.find(pString, fpat, last_end)
    end
    if last_end <= #pString then
        cap = string.sub(pString, last_end)
        table.insert(Table, cap)
    end
    if string.sub(pString,#pString) == pPattern then
        table.insert(Table,"")
    end
    return Table
end

path = {}

function path:new()
    local o = {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function path:init(path)
    self._path = path
    return self
end

function path:islegal(path)
    local path = path or self._path
    if path then
        if string.match(path,"^([%w-_/.]+)$")  or string.match(path,"^([%w-_/.]+)(%.[%w]+)$")then
            return true
        else
            return false
        end
    else
        error("path is illegal")
    end
end

function path:normpath(path)
    local path = path or self._path
    self:islegal(path)
    local str = {}
    for i,k in pairs(split(path,"/")) do
        if k == ".." then
            str[i] = ""
            while i > 1 do
                if str[i-1] == "" then
                    i = i - 1
                else
                    str[i - 1] = ""
                    break
                end
            end
        elseif k == "." then
            str[i] = ""
        else
            str[i] = k
        end
    end
    --for i,k in pairs(str) do
    --    print(i,k)
    --end
    local static_path = table.concat(str,"/")
    return string.gsub(static_path,"/+","/")
end

function path:abspath()
    error("hasn't implement")
end

--path : table of string
function path:commonprefix(table)
    if not table or type(table) ~= "table" then
        error("path of path:commonprefix illegal")
    end
    local str = ""
    local len = 0
    while true do
        len = len + 1
        str = string.sub(self._path,1,len)
        for i, k in pairs(table) do
            if #table[i] < len then
                return string.sub(str,1,len-1)
            elseif string.sub(table[i],1,len) ~= str then
                return (str == "") and str or string.sub(str,1,len-1)
            end
        end
    end
end

function path:dirname(path)
    local path = path or self._path
    self:islegal(path)
    dir_name = split(path,"/")
    return table.concat(dir_name,"/",1,#dir_name - 1)
end

function path:basename(path)
    local path = path or self._path
    self:islegal(path)
    dir_name = split(path,"/")
    return dir_name[1] == nil and path or table.concat(dir_name,"/",#dir_name)
end

function path:exists(path)
    local path = path or self._path
    self:islegal(path)
    local fd,err,errno = io.open(path)
    if not fd then
        return false
    else
        io.close(fd)
        return true
    end
end

function path:get_size(path)
    local path = path or self._path
    self:islegal(path)
    local fd,err,errno = io.open(path)
    if fd then
        local content = fd:read("*a")
        return #content
    else
        return nil
    end
end

function path:isabs(path)
    local path = path or self._path
    self:islegal(path)
    if string.match(path,"^/") and not string.match(path,"/%.%.*/") then
        return true
    else
        return false
    end
end

function path:isfile(path)
    local path = path or self._path
    self:islegal(path)
    if string.sub(path,#path) ~= "/" and self:exists(path) then
        return true
    else
        return false
    end
end

function path:isdir(path)
    local path = path or self._path
    self:islegal(path)
    if string.sub(path,#path) == "/" and self:exists(path) then
        return true
    else
        return false
    end
end

function path:join(...)
    local path = self._path
    self:islegal(path)
    local p = ""
    for i = 1, select('#',...) do
        local iter = select(i,...)
        if type(iter) == "table" then
            p = string.format("%s/%s",p,table.concat(iter,"/"))
        elseif type(iter) == "string" then
            p = string.format("%s/%s",p,select(i,...))
        else
            error("illegal variable in path:join")
        end
    end
    return p
end

function path:split(path)
    local path = path or self._path
    self:islegal(path)
    if string.match(path,"/") then
        return  string.match(path, "^([%w-_/.]+/)([%w-_.]*)$") 
    else
        return "",string.match(path,"^([%w-_/.]+)$")
    end
end

function path:splitext(path)
    local path = path or self._path
    self:islegal(path)
    if string.match(path,"%.[%w]+$") then
        return string.match(path, "^([%w-_/.]+)%.([%w]+)$") 
    else
        return path,""
    end
end

function path:is_prefix_of(prefix)
    local path = self._path
    self:islegal(path)
    local path_prefix = "^" .. prefix.."[%w-_/.]+"
    if string.match(path, path_prefix) then
        return true
    else
        return false
    end
end

function path:is_suffix_of(suffix)
    local path = self._path
    local path_suffix = "^[%w-_/.]-" .. suffix .. "$"
    if string.match(self._path, path_suffix) then
        return true
    else
        return false
    end
end

function test_split()
    print("---------test split ----------")

    str = "a/b/c"
    print("str :", str)
    print("split:")
    for i,k in pairs(split(str,"/")) do print(i,k) end

    str = "a/b/c/"
    print("str :", str)
    print("split:")
    for i,k in pairs(split(str,"/")) do print(i,k) end

    str = "/a/b/c/"
    print("str :", str)
    print("split:")
    for i,k in pairs(split(str,"/")) do print(i,k) end

    str = "/a/b/c"
    print("str :", str)
    print("split:")
    for i,k in pairs(split(str,"/")) do print(i,k) end

end

function test_normpath(p)
    local path = path:init(p)
    print("path :", p)
    print("normpath : ", path:normpath(p))
    print("normpath : ", path:normpath())
    print("")
end

function test_dirname(p)
    local path = path:init(p)
    print("path: ",p)
    print("dirname : ",path:dirname(p))
    print("dirname : ",path:dirname())
    print("")
end

function test_basename(p)
    local path = path:init(p)
    print("path: ",p)
    print("basename: ",path:basename(p))
    print("basename: ",path:basename())
    print("")
end

function test_commonprefix(p,t)
    local path = path:init(p)
    print("path: ",p)
    for i,k in pairs(t) do print(string.format("table[%d]: %s",i,k)) end
    print("commonprefix: ",path:commonprefix(t))
    print("")
end

function test_exists(p)
    local path = path:init(p)
    print("path: ",p)
    print("exists: ",path:exists(p))
    print("exists: ",path:exists())
    print("")
end

function test_get_size(p)
    local path = path:init(p)
    print("path : ",p)
    print("get_size : ",path:get_size(p))
    print("get_size : ",path:get_size())
    print("")
end

function test_isabs(p)
    local path = path:init(p)
    print("path : ",p)
    print("isabs : ",path:isabs(p))
    print("isabs : ",path:isabs())
    print("")
end

function test_isfile(p)
    local path = path:init(p)
    print("path : ",p)
    print("isfile : ",path:isfile(p))
    print("isfile : ",path:isfile())
    print("")
end

function test_isdir(p)
    local path = path:init(p)
    print("path : ",p)
    print("isdir : ",path:isdir(p))
    print("isdir : ",path:isdir())
    print("")
end

function test_join(p,t)
    local path = path:init(p)
    print("path : ",p)
    for i,k in pairs(t) do print(string.format("table[%d]: %s",i,k)) end
    print("join : ",path:join(p,t,t,t))
    print("join : ",path:join(t,t,p))
    print("")
end

function test_split(p)
    local path = path:init(p)
    print("path : ",p)
    print("split : ",path:split(p))
    print("split : ",path:split())
    print("")
end

function test_splitext(p)
    local path = path:init(p)
    print("path : ",p)
    print("splitext : ",path:splitext(p))
    print("splitext : ",path:splitext())
    print("")
end

function test_is_prefix_of(p,prefix)
    local path = path:init(p)
    print("path : ",p)
    print("prefix : ", prefix)
    print("is_prefix_of: ",path:is_prefix_of(prefix))
    print("")
end

function test_is_suffix_of(p,suffix)
    local path = path:init(p)
    print("path : ",p)
    print("suffix : ", suffix)
    print("is_suffix_of: ",path:is_suffix_of(suffix))
    print("")
end

function test_path()
    print("-----test basename ------")
    local p = ""
    test_basename(p)
    p = "asdg/asdg/"
    test_basename(p)
    p = "asdg/asdg/asdg"
    test_basename(p)

    print("-----test dirname ------")
    p = ""
    test_dirname(p)
    p = "asdg/asdg/"
    test_dirname(p)
    p = "asdg/asdg/asdg"
    test_dirname(p)
    p = "/asdg/asdg/asdg"
    test_dirname(p)
    p = "/asdg/asdg/"
    test_dirname(p)

    print("---------test normpath----------")
    p = "asdg/asdg/asdg/../..///asdg"
    test_normpath(p)
    p = "asdg/asdg/asdg/../..///asdg/"
    test_normpath(p)
    p = "/asdg/asdg/asdg/../..///asdg"
    test_normpath(p)
    p = "asdg/./asdg/../../..//asdg"
    test_normpath(p)
    p = "/asdg/./asdg/../../..//asdg/"
    test_normpath(p)
    p = "/asdg/./asdg/../../..//asdg"
    test_normpath(p)
    p = "asdg/./asdg/../../.."
    test_normpath(p)
    p = "asdg/./asdg/../../../"
    test_normpath(p)

    print("---------test commonprefix------")
    p = "asdg/asdg"
    local t = {"asdg", "asdg"}
    test_commonprefix(p,t)
    local t = {"asdg/adg", "asdg/a"}
    test_commonprefix(p,t)
    local t = {"asdg/asdg", "asdg/asg"}
    test_commonprefix(p,t)
    local t = {"basdg", "asdg"}
    test_commonprefix(p,t)

    print("---------test exists ------------")
    p = "."
    test_exists(p)
    p = "../"
    test_exists(p)
    p = "asdfg"
    test_exists(p)

    print("---------test isabs  -----------")
    p = "asdg/asagds"
    test_isabs(p)
    p = "/asdg/asagds"
    test_isabs(p)
    p = "/asdg/a/../sagds"
    test_isabs(p)
    p = "/asdg/a/./sagds"
    test_isabs(p)

    print("---------test get_size ---------")
    p = "asdg"
    test_get_size(p)
    p = "/etc/hosts"
    test_get_size(p)

    print("---------test isfile -----------")
    p = "asag/"
    test_isfile(p)
    p = "asag/asdg"
    test_isfile(p)
    p = "/etc/hosts"
    test_isfile(p)

    print("---------test isdir ------------")
    p = "asag/"
    test_isdir(p)
    p = "asag/asdg"
    test_isdir(p)
    p = "/etc/hosts"
    test_isdir(p)
    p = "/etc/"
    test_isdir(p)

    print("---------test join -------------")
    p = "a"
    t = {"b","c"}
    test_join(p,t)

    print("---------test split -------------")
    p = ""
    test_split(p)
    p = "asdg"
    test_split(p)
    p = "asdg/"
    test_split(p)
    p = "asdg/asdg"
    test_split(p)

    print("---------test splitext -------------")
    p = ""
    test_splitext(p)
    p = "asdg/asdg"
    test_splitext(p)
    p = "asdg/asdg.js"
    test_splitext(p)
    p = "asdg/as.dg.js"
    test_splitext(p)
    p = "as.dg/as.dg.js"
    test_splitext(p)

    print("---------test is_prefix_of -------------")
    p = ""
    local prefix =""
    test_is_prefix_of(p,prefix)
    p = "asdg/asdg/asdg"
    local prefix ="asdg"
    test_is_prefix_of(p,prefix)
    p = "asdg/asdg/asdg"
    local prefix ="asdg/asdg"
    test_is_prefix_of(p,prefix)
    p = "asdg/asdg/asdg"
    local prefix ="sdg/asdg"
    test_is_prefix_of(p,prefix)

    print("---------test is_suffix_of -------------")
    p = ""
    local suffix = ""
    test_is_suffix_of(p,suffix)
    p = "asdg/asdg/asdg"
    local suffix = "asdg"
    test_is_suffix_of(p,suffix)
    p = "asdg/asdg/asdg"
    local suffix = "asdg/asdg"
    test_is_suffix_of(p,suffix)
    p = "asdg/asdg/asdg"
    local suffix = "adg"
    test_is_suffix_of(p,suffix)
end
