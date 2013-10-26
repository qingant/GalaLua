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
    local xml_str
    local xml_table
    if _t=="string" then
        if startwith(manifest_file,"<?xml") then
            xml_str=manifest_file
        else
            xml_str=io.open(manifest_file):read("*a")
        end
        xml_table=self:xmltotable(xml_str)
    elseif _t=="table" then
        xml_table=manifest_file
    else
        error("invalid manifest")
    end

    self:build(xml_table)
end


function manifest:build(t)
    self.m_t={}
    self.m_t.Version=t.Version
    self.m_t.Name=t.Name
    self.m_t.Author=t.Author or "agree"
    self.m_t.Resource=t.Resource or "Resource"
    self.m_t.Catagory=t.Catagory or "Executable"
    self.m_t.Compatibility=t.Compatibility or "0.0.0"
    self.m_t.Path=t.Path or "?.lua"
    self.m_t.Root=t.Root or ""

    assert(self.m_t.Name,"invalid Manifest file")
    assert(self.m_t.Version,"invalid Manifest file")
end

function manifest:output()
    if self.m_str==nil then
        self.m_str=self:_output()
    end
    return self.m_str
end

function manifest:_output()
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
       <Root>%s</Root>
</Manifest>
]]
    local ver=self.m_t.Version
    local name=self.m_t.Name
    local author=self.m_t.Author
    local res=self.m_t.Resource
    local compatibility=self.m_t.Compatibility
    local date=os.date('%F %H:%M')
    local catagory=self.m_t.Catagory
    local path=self.m_t.Path
    local root=self.m_t.Root

    return string.format(xml,ver,name,author,res,compatibility,date,catagory,path,root)
end

function manifest:xmltotable(xml_str)
    local doc=xml.cxml_reader(xml_str,#xml_str)
    local root=doc:document()
    local ret={}
    for i,elm in pairs(root:sub_elements()) do 
        ret[elm:key()]=elm:value()
    end
    return ret
end

function manifest:gar_name()
    return string.format("%s-%s.gar",self.m_t.Name,self.m_t.Version)
end

function manifest:gar_path()
    return self.m_t.Path
end

function manifest:resource_path()
    local root=self.m_t.Root
    if root==nil or root=="" then
        root=""
    end
    local res=string.format("%s%s",root,self.m_t.Resource)
    return res
end

function manifest:root()
    return self.m_t.Root
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
    local p=string.format("%s/%s",self._manifest:resource_path(),file)
    return self:read_file(p)
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
    self._gar:add_string(self.Manifest,self._manifest:output())
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

