module(...,package.seeall)

local zip=require "minizip"
local pprint=require "pprint"
local path=require "path"
local string=require "string"
local cjson=require "cjson"
local xml=require "xml"

function startwith(str,prefix)
    assert(prefix,"can't be nil")

    local b=str:sub(1,#prefix)
    if b==prefix then
        return true
    end
    return false

end

function endwith(str,suffix)
    assert(suffix,"can't be nil")
    local e=str:sub(-#suffix)
    if e==suffix then
        return true
    end
    return false
end


local manifest={}
function manifest:new(manifest_file)
    local o={}
    self.__index=self
    setmetatable(o,self)
    self.init(o,manifest_file)
    return o
end

--[[
  @Version: version
  @Name: gar package name 
  @Author:
  @Summary:gar package summary
  @Resource: resource directory, Only when @Catagory is "Executable"
  @Compatibility: not use 
  @Date: the time when packed this gar package
  @Catagory: "Resource" or "Executable"
  @Path: lua module search dir in this gar package
  @Root:root dir in this gar package
]]
function manifest:init(manifest_file)
    local _t=type(manifest_file)
    if _t=="string" then
        if startwith(manifest_file,"<?xml") then
            self.manifest=manifest_file
        else
            self.manifest=io.open(manifest_file):read("*a")
        end
    elseif _t=="table" then
        self.manifest=self:build(manifest_file)
    else
        error("invalid manifest")
    end

    self.manifest_t=self:totable()
end

function manifest:build(t)
    local xml=[[
<?xml version="1.0" encoding="uft8"?>
<Manifest>
       <Version>%s</Version>
       <Name>%s</Name>
       <Author>%s</Author>
       <Summary>gar package</Summary>
       <Resource>%s</Resource>
       <Compatibility>%s</Compatibility>
       <Date>%s</Date> 
       <Catagory>%s</Catagory>
       <Path>%s</Path>
</Manifest>
]]

    local ver=t.Version
    local name=t.Name 
    local author=t.Author or "agree"
    local res=t.Resource or "Resource"
    local catagory=t.Catagory or "Executable"
    local compatibility=t.Compatibility or "0.0.0"
    local path=t.Path or "?.lua"
    local date=os.date('%F %H:%M')

    return string.format(xml,ver,name,author,res,compatibility,date,catagory,path)
end

function manifest:tostr()
    return self.manifest
end
function manifest:totable()
    local doc=xml.cxml_reader(self.manifest,#self.manifest)
    local root=doc:document()
    local ret={}
    for i,elm in pairs(root:sub_elements()) do 
        ret[elm:key()]=elm:value()
    end
    return ret
end

function manifest:gar_name()
    local name=assert(self.manifest_t.Name,"invalid Manifest file")
    local ver=assert(self.manifest_t.Version,"invalid Manifest file")
    return string.format("%s-%s.gar",name,ver)
end

function manifest:gar_path()
    return self.manifest_t.Path
end
function manifest:resource_path()
    return self.manifest_t.Resource
end

function manifest:root()
    return self.manifest_t.Root
end



local Zip={}

function Zip:init(zippkg,mode)
    local m=zip.CHECKCONS
    if mode=="w" then
        m=zip.CREATE
    end

    self.fd=assert(zip.open(zippkg,m),"open error")
end

function Zip:new(zippkg,mode)
    local o={}
    self.__index=self
    setmetatable(o,self)
    self.init(o,zippkg,mode)
    return o
end

--[[
@str:string add to package
@zipfile: 
@replace: whether to replace if @zipfile alreadly existed

add string @str to @self.fd as @zipfile,
]]
function Zip:add_string(zipfile,str,not_replace)
    if not self:is_valid(zipfile) then
        return  false
    end
    local id=self.fd:name_locate(zipfile)
    if id then 
        if not_replace then
            error(string.format("file:%s existed!",zipfile))
        else
            self.fd:replace(id,"string",str)
        end
    else
        self.fd:add(zipfile,"string",str)
    end
    return true
end

function Zip:is_valid(zipfile)
    if (not zipfile) or zipfile==""  then
        return false
    end
    return true
end

function Zip:add_file(zipfile,file,not_replace)
    if not self:is_valid(zipfile) then
        return  false
    end
    local id=self.fd:name_locate(zipfile)  
    if id then
        if not_replace then
            error(string.format("file:%s existed!",zipfile))
        else
            self.fd:replace(id,"file",file)
        end
    else
        self.fd:add(zipfile,"file",file,0,-1)
    end
    return true
end

function Zip:add_dir(zipfile,file)
    if not self:is_valid(zipfile) then
        return false
    end
    if not endwith(zipfile,"/") then
        zipfile=zipfile.."/"
    end

    print("add_dir:",file,zipfile)
    local id=self.fd:name_locate(zipfile)
    if not id then
        self.fd:add_dir(zipfile)
    end
    return true
end
--[[
--@file: if @file is directory,it must with "/" suffix,
--       because directory always have "/" in zip package.
--]]
function Zip:is_exist(file)
    return self.fd:name_locate(file)  
end

function Zip:close()
    if self.fd then
        self.fd:close()
    end
    self.fd=nil
end

function Zip:read_file(file)
    if type(file)=="string" then
        local f=self.fd:open(file)
        if f then
            return f:read(self.fd:stat(file).size)
        end
    end
end


--read @file from opened gar package 
function Zip:read_file2(file)
    local ret={}
    if not file then
        local max_id=self.fd:get_num_files()
        for i=1,max_id do
            local name=self.fd:get_name(i)
            local f=self.fd:open(name)
            local size=self.fd:stat(name).size
            if size>0 then
                ret[name]=f:read(size)
            end
        end
    else
        if type(file)=="string" then
            file={file}
        end
        for i,v in pairs(file) do 
            ret[v]=self:read_file(v)
        end
    end

    return ret
end

local gar_reader={}
function gar_reader:new(gar_path)
    local o={}
    self.__index=self
    setmetatable(o,self)
    self.init(o,gar_path)
    return o
end

function gar_reader:init(gar_path)
    self.Manifest="Manifest"

    self._gar=Zip:new(gar_path)
    local m=self._gar:read_file(self.Manifest)
    self._manifest=manifest:new(m)
end

function gar_reader:read_file(file)
    return self._gar:read_file(file)
end

function gar_reader:read_resource(file)
    if startwith(file,"/") then
        error("file path must not be asbolute path")
    end
    local p=string.format("%s/%s/%s",self._manifest:root(),self._manifest:resource_path(),file)
    print(p)
    return self._gar:read_file(p)
end





local gar={}

--[[
@ignore_func:function ignore_func(self,file)
@fname_func:function fname_func(self,file)
]]
function gar:new(manifest_file,fname_func)
    local o={}
    self.__index=self
    setmetatable(o,self)
    self.init(o,manifest_file,fname_func)
    return o
end


function gar:init(manifest_file,fname_func)
    if type(fname_func)=="function" then
        self.name_in_gar=fname_func
    end

    self.Manifest="Manifest"

    self._manifest=manifest:new(manifest_file)

    self._gar=Zip:new(self._manifest:gar_name(),"w")
    self:add_manifest()
end

function gar:close()
    self._gar:close()
end

function gar:name_in_gar(path,file_type)
    return path
end
function gar:ignore_me(f)
    if (f==".") or (f=="..") then
        return true
    end
end

function gar:add_manifest()
    self._gar:add_string(self.Manifest,self._manifest:tostr())
end


function gar:add2gar(file,not_recurse)
    self.in_path=file
    function add(file,not_recurse)
        
        local ft=path.dir_type(file)
        if (ft=="reg") then
            local zipfile=self:name_in_gar(file)
            if not self._gar:is_valid(zipfile)  then
                return 
            end

            self._gar:add_file(zipfile,file)
        elseif (ft=="dir") then
            local zipfile=self:name_in_gar(file,"d")
            if not self._gar:is_valid(zipfile)  then
                return 
            end

            self._gar:add_dir(zipfile,file)

            --Zip sub-directory recursively 
            if not not_recurse then
                for i,f in pairs(path.dir(file)) do
                    if not self:ignore_me(f) then
                        add(path.join(file,f))
                    end
                end
            end
        end
    end
    add(file,not_recurse)
end

function new(manifest_file,fname_func)
    local t=type(manifest_file)
    assert(t=="string" or t=="table","argument error")
    if t=="string" and string.sub(manifest_file,-4)==".gar" then
        return  gar_reader:new(manifest_file)
    end
    return gar:new(manifest_file,fname_func)
end

function remove_prefix_slash(str)
    local str=str or ""
    return  str:match("/*(.*)") 
end

function sub_root(self,in_path,file_type)
    print("sub_root:",in_path)
    local root=self._manifest:root()
    local as_path
    if root and root~="" then
        local dir,base=path.split(in_path)
        if in_path==self.in_path then
            if file_type~="d" then 
                as_path=path.join(root,base)
            else
                as_path=root
            end
        else
            if startwith(base,".") then
                return 
            end

            as_path=path.replace(in_path,self.in_path,root)
        end
    end

    if (not as_path) or (as_path=="") then
        as_path=remove_prefix_slash(in_path)
    end
    print("as_path:",as_path)
    return as_path
end

