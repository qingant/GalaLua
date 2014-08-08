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

local _split = function (pString, pPattern)
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

local is_legal = function (path)
        --local path = self._path
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

path = {}

function path:new()
    local o = {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function path:init(path)
    self._path = path
    self.ispath = true
    is_legal(self._path)
    return self
end

function path:get_path(path)
    return self._path
end

function path:set_path(path)
    self._path = path
end

function path:norm_path()
    local path = self._path
    local str = {}
    for i,k in pairs(_split(path,"/")) do
        if k == ".." then
            str[i] = k
            local n = i
            while n > 1 do
                if str[n-1] == "" or str[n-1] == ".."  then
                    n = n - 1
                else
                    str[i] = ""
                    str[n - 1] = ""
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
    self._path = string.gsub(static_path,"/+","/")
    return self
end

function path:real_path()
    return self:abs_path()
end

function path:same_file(p)
    local p1 = self:abs_path()
    local p2 = path:new():init(p):abs_path()
    if p1:get_path() == p2:get_path() and p1:exists() then
        return true
    else
        return false
    end
end

function path:abs_path()
    local p = path:new():init(self:get_path())
    local pwd = path:new():init(os.getenv("PWD"))
    p = pwd:join(p):norm_path():get_path()
    return path:new():init(p)
end

---... :one param : table of path object 
---... :multi params : path objects list
---TODO ... == str1, str2 will work as well
function path:common_prefix(...)
    local t = select(1,...)
    if not getmetatable(t) then
        if #t == 0 then
            error("path of path:common_prefix illegal")
        end
    else
        for i = 1,select('#',...) do
            if not getmetatable(select(i,...)) then
                error("path of path:common_prefix illegal")
            end
        end
    end
    local str = ""
    local len = 0
    while true do
        len = len + 1
        str = string.sub(self:get_path(),1,len)
        if type(t) == "table" and #t > 0 then
            for _,k in pairs(t) do
                local p = k:get_path()
                if #p < len then
                    return string.sub(str,1,len-1)
                elseif string.sub(p,1,len) ~= str then
                    return (str == "") and str or string.sub(str,1,len-1)
                end
            end
        else
            for i = 1, select('#',...) do
                local p = select(i,...):get_path()
                if #p < len then
                    return string.sub(str,1,len-1)
                elseif string.sub(p,1,len) ~= str then
                    return (str == "") and str or string.sub(str,1,len-1)
                end
            end
        end
    end
end


function path:dir_name()
    local path = self._path
    path = _split(path,"/")
    return table.concat(path,"/",1,#path - 1)
end

function path:base_name()
    local path = self._path
    path = _split(path,"/")
    return path[1] == nil and path or table.concat(path,"/",#path)
end

function path:exists()
    local path = self._path
    local fd,err,errno = io.open(path)
    if not fd then
        return false
    else
        io.close(fd)
        return true
    end
end

function path:expand_user()
    local p = self._path
    if string.sub(p,1,2) == "~/" then
        p = string.gsub(p,"^~",os.getenv("HOME"))
    end
    return path:new():init(p)
end

function path:expand_vars()
    local p = self._path
    local t = _split(p,"/")
    for i,k in pairs(t) do
        local m = string.match(k,"${([%w_]+)}")
        if m then
            t[i] = string.gsub(k,"${[%w_]+}",os.getenv(m))
        end
    end
    return path:new():init((table.concat(t,"/")))
end

function path:get_size()
    local path = self._path
    path = self:abs_path():get_path()
    local fd,err,errno = io.open(path)
    if fd and string.sub(path,#path) ~= "/" then
        local content = fd:read("*a")
        return #content
    else
        return nil,"open path error"
    end
end

function path:is_abs()
    local path = self._path
    if string.match(path,"^/") and not string.match(path,"/%.%.*/") then
        return true
    else
        return false
    end
end

function path:is_file()
    local path = self._path
    path = self:abs_path():get_path()
    if string.sub(path,#path) ~= "/" and self:exists(path) then
        return true
    else
        return false
    end
end

function path:is_dir(path)
    local path = self._path
    if string.sub(path,#path) == "/" and self:exists(path) then
        return true
    else
        return false
    end
end

---TODO ... == str1, str2 will work as well
function path:join(...)
    local t = select(1,...)
    if not getmetatable(t) then
        if #t == 0 then
            error("path of path:join illegal")
        end
    else
        for i = 1, select('#',...) do
            if not getmetatable(select(i,...)) then
                error("illegal variable of path:join ")
            end
        end
    end
    local p = self._path or ""
    if type(t) == "table" and #t > 0 then
        for _,k in pairs(t) do
            local iter = k:get_path()
            p = string.format("%s/%s",p,iter)
        end
    else
        for i = 1, select('#',...) do
            local iter = select(i,...):get_path()
            p = string.format("%s/%s",p,iter)
        end
    end
    return path:new():init(p)
end

function path:split()
    local path = self._path
    if string.match(path,"/") then
        return  string.match(path, "^([%w-_/.]+/)([%w-_.]*)$")
    else
        return "",string.match(path,"^([%w-_/.]+)$")
    end
end

function path:split_ext()
    local path = self._path
    if string.match(path,"%.[%w]+$") then
        return string.match(path, "^([%w-_/.]+)%.([%w]+)$") 
    else
        return path,""
    end
end

function path:is_prefix_of(prefix)
    local path = self._path
    local path_prefix = "^" .. string.gsub(prefix,"-","%%-") .. "[%w-_/.]+"
    if string.match(path, path_prefix) then
        return true
    else
        return false
    end
end

function path:is_suffix_of(suffix)
    local path = self._path
    local path_suffix = "^[%w-_/.]-" .. string.gsub(suffix,"-","%%-") .. "$"
    if string.match(self._path, path_suffix) then
        return true
    else
        return false
    end
end

function norm_path(pathname)
    return path:new():init(pathname):norm_path():get_path()
end

function real_path(pathname)
    return path:new():init(pathname):real_path():get_path()
end

function same_file(pathname1, pathname2)
    local p1 = path:new():init(pathname1)
    local p2 = path:new():init(pathname2)
    return p1:same_file(p2)
end

function abs_path(pathname)
    return path:new():init(pathname):abs_path():get_path()
end

function common_prefix(...)
    local t = {}
    if type(select(1,...)) == "table" then
        for i,k in pairs(select(1,...)) do
            t[i] = path:new():init(k)
        end
        path:new():init(select(1,...)[1]):common_prefix(t)
    else
        for i = 1, select('#',...) do
            t[i] = path:new():init(select(i,...))
        end
        path:new():init(select(1,...)):common_prefix(t)
    end
end

function dir_name(pathname)
    return path:new():init(pathname):dir_name()
end

function base_name(pathname)
    return path:new():init(pathname):base_name()
end

function exists(pathname)
    return path:new():init(pathname):exists()
end

function expand_user(pathname)
    return path:new():init(pathname):expand_user():get_path()
end

function expand_vars(pathname)
    return path:new():init(pathname):expand_vars():get_path()
end

function get_size(pathname)
    return path:new():init(pathname):expand_vars():get_size()
end

function is_abs(pathname)
    return path:new():init(pathname):is_abs()
end

function is_file(pathname)
    return path:new():init(pathname):is_file()
end

function is_dir(pathname)
    return path:new():init(pathname):is_dir()
end

function join(...)
    local t = {}
    if type(select(1,...)) == "table" then
        for i,k in pairs(select(1,...)) do
            t[i] = path:new():init(k)
        end
        path:new():init(select(1,...)[1]):join(t)
    else
        for i = 1, select('#',...) do
            t[i] = path:new():init(select(i,...))
        end
        path:new():init(select(1,...)):join(t)
    end
end

function split(pathname)
    return path:new():init(pathname):split()
end

function split_ext(pathname)
    return path:new():init(pathname):split_ext()
end


function test_split()
    print("---------test split ----------")

    str = "a/b/c"
    print("str :", str)
    print("split:")
    for i,k in pairs(_split(str,"/")) do print(i,k) end

    str = "a/b/c/"
    print("str :", str)
    print("split:")
    for i,k in pairs(_split(str,"/")) do print(i,k) end

    str = "/a/b/c/"
    print("str :", str)
    print("split:")
    for i,k in pairs(_split(str,"/")) do print(i,k) end

    str = "/a/b/c"
    print("str :", str)
    print("split:")
    for i,k in pairs(_split(str,"/")) do print(i,k) end

end

function test_normpath(p)
    local path = path:new():init(p)
    print("path :", p)
    print("normpath : ", norm_path(p))
    print("normpath : ", path:norm_path():get_path())
    print("")
end

function test_dirname(p)
    local path = path:new():init(p)
    print("path: ",p)
    print("dirname : ",dir_name(p))
    print("dirname : ",path:dir_name())
    print("")
end

function test_basename(p)
    local path = path:new():init(p)
    print("path: ",p)
    print("basename: ",base_name(p))
    print("basename: ",path:base_name())
    print("")
end

function test_abspath(p)
    local path = path:new():init(p)
    print("path: ",p)
    print("abspath: ", abs_path(p))
    print("abspath: ", path:abs_path():get_path())
    print("")
end

function test_commonprefix(p,...)
    local path = path:init(p)
    print("path: ",p)
    for i = 1,select('#',...) do
        local var = select(1,...)
        if type(var) == "table" and #var > 0 then
            for i,k in pairs(var) do
                print(string.format("table[%d]: %s",i,k:get_path()))
            end
        else
            print(string.format("path : %s",select(i,...):get_path()))
        end
    end
    print("commonprefix: ",path:common_prefix(...))
    print("")
end

function test_exists(p)
    local path = path:init(p)
    print("path: ",p)
    print("exists: ",exists(p))
    print("exists: ",path:exists())
    print("")
end

function test_expanduser(p)
    local path = path:init(p)
    print("path: ",p)
    print("expanduser: ",expand_user(p))
    print("expanduser: ",path:expand_user():get_path())
    print("")
end

function test_expandvars(p)
    local path = path:init(p)
    print("path: ",p)
    print("expandvars: ",expand_vars(p))
    print("expandvars: ",path:expand_vars():get_path())
    print("")
end

function test_get_size(p)
    local path = path:init(p)
    print("path : ",p)
    print("get_size : ",get_size(p))
    print("get_size : ",path:get_size())
    print("")
end

function test_isabs(p)
    local path = path:init(p)
    print("path : ",p)
    print("isabs : ",is_abs(p))
    print("isabs : ",path:is_abs())
    print("")
end

function test_isfile(p)
    local path = path:init(p)
    print("path : ",p)
    print("isfile : ",is_file(p))
    print("isfile : ",path:is_file())
    print("")
end

function test_isdir(p)
    local path = path:init(p)
    print("path : ",p)
    print("isdir : ",is_dir(p))
    print("isdir : ",path:is_dir())
    print("")
end

function test_join(p,...)
    local path = path:init(p)
    print("path : ",path._path)
    for i = 1,select('#',...) do
        local var = select(1,...)
        if type(var) == "table" and #var > 0 then
            for i,k in pairs(var) do
                print(string.format("table[%d]: %s",i,k:get_path())) 
            end
        else
            print(string.format("path : %s",select(i,...):get_path()))
        end
    end
    print("join : ",path:join(...):get_path())
    print("")
end

function test_split(p)
    local path = path:init(p)
    print("path : ",p)
    print("split : ",split(p))
    print("split : ",path:split())
    print("")
end

function test_splitext(p)
    local path = path:init(p)
    print("path : ",p)
    print("splitext : ",split_ext(p))
    print("splitext : ",path:split_ext())
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
    print("------ test basename ------")
    local p = ""
    test_basename(p)
    p = "a/b/"
    test_basename(p)
    p = "a/b/c"
    test_basename(p)

    print("------- test dirname ------")
    p = ""
    test_dirname(p)
    p = "a/b/"
    test_dirname(p)
    p = "a/b/c"
    test_dirname(p)
    p = "/a/b/c"
    test_dirname(p)
    p = "/a/b/"
    test_dirname(p)

    print("----- test normpath --------")
    p = "a/b/c/../..///d"
    test_normpath(p)
    p = "a/b/c/../..///d/"
    test_normpath(p)
    p = "/a/b/c/../..///d"
    test_normpath(p)
    p = "a/./b/../../..//c"
    test_normpath(p)
    p = "/a/./b/../../..//c/"
    test_normpath(p)
    p = "/a/./b/../../..//c"
    test_normpath(p)
    p = "a/./b/../../.."
    test_normpath(p)
    p = "a/./b/../../../"
    test_normpath(p)

    print("------ test abspath  --------")
    p = "a/b"
    test_abspath(p)
    p = "a/./b/../"
    test_abspath(p)
    p = "./a/"
    test_abspath(p)
    p = "../../../a/"
    test_abspath(p)

    print("----- test commonprefix -----")
    p = "asdg/asdg"
    local p1 = "asdg/adg"
    local p2 = "asdg/adg"
    local p3 = "asdg/bdg"
    local p4 = "bsdg/bgd"
    common_prefix(p1,p2)
    common_prefix(p1,p2,p3)
    common_prefix(p1,p2,p3,p4)
    p1 = path:new():init(p1)
    p2 = path:new():init(p2)
    p3 = path:new():init(p3)
    p4 = path:new():init(p4)
    t = {p1,p2}
    test_commonprefix(p,t)
    t = {p1,p2,p3}
    test_commonprefix(p,t)
    t = {p1,p2,p3,p4}
    test_commonprefix(p,t)
    test_commonprefix(p,p1,p2,p3)
    test_commonprefix(p,p1,p2,p3,p4)

    print("------- test exists ---------")
    p = "."
    test_exists(p)
    p = "../"
    test_exists(p)
    p = "asdfg"
    test_exists(p)

    print("------- test expanduser ------")
    p = "~/asdg/asdg"
    test_expanduser(p)
    p = "~asdg/asdg"
    test_expanduser(p)
    p = "/~/asdg/asdg"
    test_expanduser(p)
    p = "~/asdg/~/asdg"
    test_expanduser(p)

    print("------- test expandvars ------")
    p = "${HOME}/asdg/asdg"
    test_expandvars(p)
    p = "/asdg/${PWD}/asdg"
    test_expandvars(p)
    p = "/asdg/ad${PWD}asgd/asdg"
    test_expandvars(p)

    print("------- test isabs -----------")
    p = "asdg/asagds"
    test_isabs(p)
    p = "/asdg/asagds"
    test_isabs(p)
    p = "/asdg/a/../sagds"
    test_isabs(p)
    p = "/asdg/a/./sagds"
    test_isabs(p)

    print("-------- test get_size -------")
    p = "."
    test_get_size(p)
    p = "asdg"
    test_get_size(p)
    p = "/etc/hosts"
    test_get_size(p)

    print("---------test isfile ---------")
    p = "."
    test_isfile(p)
    p = "asag/"
    test_isfile(p)
    p = "asag/asdg"
    test_isfile(p)
    p = "/etc/hosts"
    test_isfile(p)

    print("---------test isdir -----------")
    p = "."
    p = "asag/"
    test_isdir(p)
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
    p1 = "b"
    p2 = "c"
    p3 = "d"
    join(p,p1)
    join(p,p1,p2)
    join(p,p1,p2,p3)
    local p1 = path:new():init(p1)
    local p2 = path:new():init(p2)
    local p3 = path:new():init(p3)
    t = {p1,p2}
    test_join(p,t)
    t = {p1,p2,p3}
    test_join(p,t)
    test_join(p,p1,p2)
    test_join(p,p1,p2,p3)

    print("---------test split ------------")
    p = ""
    test_split(p)
    p = "a"
    test_split(p)
    p = "a/"
    test_split(p)
    p = "a/b"
    test_split(p)

    print("---------test splitext ---------")
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

    print("------ test is_prefix_of -------")
    p = ""
    local prefix =""
    test_is_prefix_of(p,prefix)
    p = "a/b/c"
    local prefix ="a"
    test_is_prefix_of(p,prefix)
    p = "a/b/c"
    local prefix ="a/c"
    test_is_prefix_of(p,prefix)
    p = "a/b/c"
    local prefix ="b/c"
    test_is_prefix_of(p,prefix)
    p = "a-b-c"
    local prefix ="b-c"
    test_is_prefix_of(p,prefix)

    print("------ test is_suffix_of -------")
    p = ""
    local suffix = ""
    test_is_suffix_of(p,suffix)
    p = "a/b/c"
    local suffix = "c"
    test_is_suffix_of(p,suffix)
    p = "a/b/c"
    local suffix = "b/c"
    test_is_suffix_of(p,suffix)
    p = "a/b/c"
    local suffix = "ab/c"
    test_is_suffix_of(p,suffix)
    p = "a-b-c"
    local suffix = "ab-c"
    test_is_suffix_of(p,suffix)
end
