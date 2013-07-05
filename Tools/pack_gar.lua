module(...,package.seeall)

local zip=require "minizip"
local pprint=require "pprint"
local dir=require "_dir"
local string=require "string"
local cjson=require "cjson"

function string.startwith(str,prefix)
    assert(prefix,"can't be nil")
    local pat=string.format("^%s.*",prefix)
    if str:match(pat) then
        return true
    end
end

function string.endwith(str,suffix)
    assert(suffix,"can't be nil")
    local pat=string.format(".*%s$",suffix)
    if str:match(pat) then
        return true
    end
end

local path={}
--[[
--just return @path2 if @path2 is an absolute path, join two path otherwise.
--@path1:
--@path2: 
--]]
function path.join(path1,path2)
    assert((path1 and path2),"not valid path")
    if path2:sub(1,1)=="/" then
        return path2
    else
        return string.format("%s/%s",path1,path2)
    end
end

--[[
-- split path into basename and dirname
-- @path:
--]]
function path.split(path)
    assert(path or path~="","invalid path")
    local _basename="/"
    local _dirname="/"
    if path~="/" then
        _dirname,_basename=string.match(path,"(.*)/([^/]+)") 
        if ((not _basename) or (not _dirname)) then
            _basename=path
            _dirname="."
        end
    end
--    print("dir",_dirname)
--    print("base",_basename)
    return _dirname,_basename
end

local main_lua=[[
module(...,package.seeall)

local cjson=require "cjson"
local zip=require "minizip"

local GAR=glr.get_option("g")
print(GAR)

function read_manifest(zipfd)
    local manifest="MANIFEST"
    local f=zipfd:open(manifest)
    local size=zipfd:stat(manifest).size
    if size>0 then
        return cjson.decode(f:read(size))
    else
        return {}
    end
end

-- gar loader
function loader(modulename)
    local gar=GAR
    local zipfd=zip.open(gar)
    local file=read_manifest(zipfd)[modulename]
    print("loader:",modulename,file)
    if file then
        local fstream=zipfd:open(file)
        local str=fstream:read(zipfd:stat(file).size)
        fstream:close()
        return assert(loadstring(str))
    end
    zipfd:close()
    return "error"
end

table.insert(package.loaders,2,loader)

function main()
    local module_name=glr.get_option("m")
    local entry=glr.get_option("e")
    
    print("main:",module_name,entry)
    local m=require(module_name)
    
    m[entry]()
end

if ...=="__main__" then
    main()
end

]]

--add main.lua to zip
function add_main(zipfd,file)
    if file then
        add_file(zipfd,file,"main.lua")
    else
        add_string(zipfd,main_lua,"main.lua")
    end
end

--read @file from @gar
function read_file(gar,file)
    local zipfd=zip.open(gar)
    local ret={}
    if not file then
        local max_id=zipfd:get_num_files()
        for i=1,max_id do
            local name=zipfd:get_name(i)
            local f=zipfd:open(name)
            local size=zipfd:stat(name).size
            if size>0 then
                ret[name]=f:read(size)
            end
        end
    else
        if type(file)=="string" then
            file={file}
        end
        for i,v in pairs(file) do 
            if zipfd:name_locate(v) then
                local f=zipfd:open(v)
                ret[v]=f:read(zipfd:stat(v).size)
            end
        end
    end

    return ret
end

--[[
-- generate manifest and add to zip package @zipfd.
--
-- @zipfd: an opened zip package
-- @struct:
--]]
function add_manifest(zipfd)
    function get_modules()
        local module_list={}
        local max_id=zipfd:get_num_files()
        if max_id then
            for i=1,max_id do
                local file=zipfd:get_name(i)
                local module_name=file:match("/*([^/]+)%.lua$") 
                print("get_modules:",file,module_name)
                if module_name then
                    module_list[module_name]=file   -- sub ".lua"
                end
            end
        end
        return module_list
    end
    local module_list=get_modules()

    --TODO:add more information into mainfest
    local manifest=cjson.encode(module_list)
    add_string(zipfd,manifest,"MANIFEST")
end

--add string @str to @zipfd as @zipfile,
--replace it if @zipfile existed.
function add_string(zipfd,str,zipfile,not_replace)
    print("add_string:",str,zipfile)
    local id=zipfd:name_locate(zipfile)  
    if ((not not_replace) and id) then
        zipfd:replace(id,"string",str)
    else
        zipfd:add(zipfile,"string",str)
    end
end

function add_file(zipfd,file,zipfile,not_replace)
    local id=zipfd:name_locate(zipfile)  
    print("add_file:",file,zipfile)
    if ((not not_replace )and id) then
        zipfd:replace(id,"file",file)
    else
        zipfd:add(zipfile,"file",file,0,-1)
    end
end

function add_dir(zipfd,file,zipfile,not_replace)
    if not zipfile:match(".*/$") then
        zipfile=zipfile.."/"
    end
    print("add_dir:",file,zipfile)
    local id=zipfd:name_locate(zipfile)
    if ((not not_replace) and id ) then
        zipfd:replace(id,"string","")
    else
        zipfd:add_dir(zipfile)
    end
end

--[[
--@file: if @file is directory,it must with "/" suffix,
--       because directory always have "/" in zip package.
--]]
function is_exist(zipfd,file)
    return zipfd:name_locate(file)  
end

--add @file to @zipfd as @zipfile
--@not_recurse: run add2zip recursively if false
function add2zip(zipfd,file,zipfile,not_recurse)
    local zipfile=zipfile or file
    local ft=dir.dir_type(file)
    if (ft=="reg") then
        add_file(zipfd,file,zipfile)
    elseif (ft=="dir") then
        add_dir(zipfd,file,zipfile)
        
        --Zip sub-directory recursively 
        if not not_recurse then
            for i,f in pairs(dir.dir(file)) do
                if f~="." and f~=".." then
                    add2zip(zipfd,path.join(file,f),path.join(zipfile,f))
                end
            end
        end
    end
end

--[==[
-- @_out:the output zip file
-- @_in:the file want to zip. support multi paths in a table like {in_path=zip_path,...},
--      zip in_path as zip_path, use in_path if zip_path is empty string.
]==]
function create_zip(_out,_in)
    assert(_out and _in,"invalid argument")
    function remove_prefix_slash(str)
        local str=str or ""
        return  str:match("/*(.*)") 
    end
    local zipfd=zip.open(_out,zip.CREATE)

    if type(_in)=="string" then
        _in={[_in]=""}
    end

    for in_path,as_path in pairs(_in) do
        if as_path=="" then
            as_path=remove_prefix_slash(in_path)
        end
        add2zip(zipfd,in_path,as_path)
    end
    add_manifest(zipfd)
    add_main(zipfd)

    zipfd:close()
end


if ...=="__main__" then
    create_zip("test.gar",{["/home/dev/git/aim"]="aim"})
    print("=================================")
--    pprint.pprint(read_file("test1.gar","MANIFEST"))
end


