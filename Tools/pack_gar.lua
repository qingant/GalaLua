module(...,package.seeall)

local zip=require "minizip"
local pprint=require "pprint"
local path=require "path"
local string=require "string"
local cjson=require "cjson"
local xml=require "xml"

local Manifest="MANIFEST"
local ModuleCache="module.cache"

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

--read module search cache file
function read_cache(zipfd)
    local f=zipfd:open(ModuleCache)
    local size=zipfd:stat(ModuleCache).size
    if size>0 then
        return cjson.decode(f:read(size))
    else
        return {}
    end
end

--read @file from opened gar package @zipfd
function read_file(zipfd,file)
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
            local f=zipfd:open(v)
            if f then
                ret[v]=f:read(zipfd:stat(v).size)
            end
        end
    end

    return ret
end

--[[
<?xml version="1.0" encoding="uft8"?>
<Manifest>
       <Version>1.0</Version>
       <Name>sys</Name>   <!-- gar package name -->
       <Author>wzhq</Author>
       <Summary>gar package</Summary>
       <Revision/>
       <Compatibility>2.0</Compatibility>
       <Date>2013/6/20 16:45:03</Date>  
       <Catagory>Resource</Catagory>  <!-- Excutable/Resource -->
       <Path>?.lua;aim/Service/?.lua</Path>  <!--lua module search dir in this gar package-->
       <Root>aim</Root>    <!--root dir in this gar package-->
</Manifest>
]]
function add_manifest(zipfd,manifest_file)
--    local root=xml.cxml_element("Manifest")
--    root:add_sub_element(xml.cxml_element("Version","1.0"))
--    root:add_sub_element(xml.cxml_element("Name","aim"))
--    root:add_sub_element(xml.cxml_element("Author","aaa"))
--    root:add_sub_element(xml.cxml_element("Summary","gar package"))
--    root:add_sub_element(xml.cxml_element("Date",os.date('%F %H:%M')))
--    root:add_sub_element(xml.cxml_element("Catagory","Excutable"))
--    root:add_sub_element(xml.cxml_element("Path","?.lua"))

--    local manifest=xml.cxml_writer(root)

    add_file(zipfd,manifest_file,"Manifest")
end

--[[
--check manifest and get gar file name 
--@Manifest: file with xml structure
--]]
function check_and_get(Manifest)
    local manifest=io.open(Manifest):read("*a")
    local doc=xml.cxml_reader(manifest,#manifest)
    local root=doc:document()
    local ret={}
    for i,elm in pairs(root:sub_elements()) do 
        ret[elm:key()]=elm:value()
    end
    return ret
end


--FIXME: edit manifest,add Path,Name...
function edit_manifest(zipfd)
    local f=zipfd:open(Manifest)
    if f then
        local str=f:read(zipfd:stat(Manifest).size)
        local doc=xml.cxml_reader(str,#str)
    else
    end
 
end



--[[
-- generate module cache file and add to zip package @zipfd.
--
-- @zipfd: an opened zip package
-- @struct:
--]]
function add_cache(zipfd)
    function get_modules()
        local module_list={}
        local max_id=zipfd:get_num_files()
        if max_id then
            for i=1,max_id do
                local file=zipfd:get_name(i)
                local module_name=file:match("/*([^/]+)%.lua$") 
--                print("get_modules:",file,module_name)
                if module_name then
                    module_list[module_name]=file   -- sub ".lua"
                end
            end
        end
        return module_list
    end
    local module_list=get_modules()

    local cache=cjson.encode(module_list)
    add_string(zipfd,cache,ModuleCache)
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
    if (not zipfile) or (zipfile=="") then
        zipfile=file
    end
    local ft=path.dir_type(file)
    if (ft=="reg") then
        add_file(zipfd,file,zipfile)
    elseif (ft=="dir") then
        add_dir(zipfd,file,zipfile)
        
        --Zip sub-directory recursively 
        if not not_recurse then
            for i,f in pairs(path.dir(file)) do
                if f~="." and f~=".." then
                    add2zip(zipfd,path.join(file,f),path.join(zipfile,f))
                end
            end
        end
    end
end

--[==[
-- @_out:Manifest file
-- @_in:the file want to zip. support multi paths in a table like {in_path=zip_path,...},
--      zip in_path as zip_path, use in_path if zip_path is empty string.
]==]
function create_zip(manifest_file,_in)
    assert(manifest_file and _in,"invalid argument")
    function remove_prefix_slash(str)
        local str=str or ""
        return  str:match("/*(.*)") 
    end
    local m=check_and_get(manifest_file)
    local _out=assert(m.Name,"invalid Manifest file")
    local zipfd=zip.open(_out..".gar",zip.CREATE)
    
    if type(_in)=="string" then
        _in={[_in]=""}
    end
    

    for in_path,as_path in pairs(_in) do
        if as_path=="" then
            if path.dir_type(in_path)=="reg" then
                local dirn,basen=path.split(in_path)
                as_path=path.join(m.Root,basen)
            else
                as_path=m.Root 
            end

            if (not as_path) or (as_path=="") then
                as_path=remove_prefix_slash(in_path)
            end
        end
        add2zip(zipfd,in_path,as_path)
    end
    add_manifest(zipfd,manifest_file)
--    add_cache(zipfd)

    zipfd:close()
end


-- {{ for command console

local cmds={}

--FIXME: not support assign output path 
function cmds.pack(...)
    local gar=...
    local path={}
    for i=2,select("#",...) do
        path[select(i,...)]=""
    end
    if not (gar and next(path))  then
        io.write("gar pack manifest_file  path1 [path2]...\n")
        return 
    end
    
    print(gar)
    pprint.pprint(path)

    create_zip(gar,path)
end

function cmds.edit(gar)
    if not gar then
        io.write("gar edit xxx.gar\n")
        return 
    end
    io.write("TODO\n")
end

--FIXME:better show format
function cmds.show(gar)
    if not gar then
        io.write("gar show xxx.gar\n")
        return 
    end
    io.write("TODO \n")
--    local zipfd=zip.open(gar)
end

function cmds.help()
    io.write("gar :\n")
    io.write("available commands:\n")
    for i in pairs(cmds) do
        io.write("\t",i,"\n")
    end
end

local mt={__index=function (t,key) io.write(key,":command not founded!\n") return cmds.help  end }
setmetatable(cmds,mt)

function helper(argv)
    pprint.pprint(argv)

    table.remove(argv,1)
    local cmd=argv[1] 
    table.remove(argv,1)

    cmds[cmd](unpack(argv))
end

function info()
    return "gar operation tool"
end

--command completion
local _completion=require "completion"
function gar_completion(s,prefix)
    return _completion.path_completion(s,prefix,".*%.gar$")
end

function dir_completion(s,prefix)
    local pre,arg=string.match(s,"(.*%s+)(.*)$")
    if not arg then
        arg=s
        pre=""
    end
    return _completion.path_completion(arg,prefix..pre)
end

completion={}
completion.pack=dir_completion
completion.show=gar_completion

-- }} for command console

if ...=="__main__" then
    print("=================================")
    
    local zipfd=zip.open("test.gar")
    pprint.pprint(search_module(zipfd,"lsr"))
end


