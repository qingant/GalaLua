--[[
-- path
--]]
module(...,package.seeall)

local _dir=require "_dir"



dir=_dir.dir
dir_type=_dir.dir_type

--[[
--just return @path2 if @path2 is an absolute path, join two path otherwise.
--@path1:
--@path2: 
--]]
function join(path1,path2)
    assert((path1 and path2),"not valid path")
    if path2:sub(1,1)=="/" then
        return path2
    else
        local sep="/"
        if path1=="" or path1:match(".*/$") then
            sep=""
        end
        return string.format("%s%s%s",path1,sep,path2)
    end
end

--[[
-- split path into basename and dirname
-- @path:
--]]
function split(path)
    assert(path or path~="","invalid path")
    local _basename="/"
    local _dirname="/"
    if path~="/" then
        _dirname,_basename=string.match(path,"^(.*/)([^/]+)$") 
        if ((not _basename) or (not _dirname)) then
            _basename=path
            _dirname="."
        end
    end
    return _dirname,_basename
end

--ls @file
--@filter: support lua string pattern syntax
function ls(file,filter)
    local filter=filter or "^.*$"

    local all={}
    if (not file) or (file=="") then
        file="."
    end
    for i,f in pairs(dir(file)) do
        if (f~=".") and (f~="..") then 
            local _ft=dir_type(path.join(file,f))
            if _ft=="dir" or string.match(f,filter) then
                all[f]=_ft
            end
        end
    end
    return all
end

function replace(path,from,to)
    local sep="/"
    if path:sub(-#from)==from then
        sep=""
    end
    local d=string.gsub(path,from,to..sep,1)
    return pretty(d)
end

function pretty(path)
    assert(type(path)=="string","must pass path")
    return path:gsub("/+","/")
end

if ...=="__main__" then
    function test_split()
        print(split("test.lua"))
        print(split("/test/ac/c.lua"))
        print(split("a/test.lua"))
        print(split("./"))
        print(split("/"))
    end

    function test_join()
        print(join("test","ss"))
        print(join("/test","ss"))
        print(join("//","ss"))
        print(join("/","/ss"))
        print(join(".","."))
    end
    test_join()

    test_split()
end
