--[[
-- 
-- config interface for gdk console 
--]]

module(...,package.seeall)

local os=require "os"

local string=require "string"
local pprint=require "pprint"
local L = require 'linenoise'
local cjson=require "cjson"
local _Config=require "config"
local _Config=_Config.Config
local mdb=(require "mdb").mdb

local prompt, history = '> ', 'history.txt'

local db_path=require "db_path"
local mdb_path =db_path.config

local ROOT="Configure"
local ROOT2="Contents"

local SCHEMA_ROOT="Schema"
local SCHEMA_ROOT2="_TYPEDEFINE"

DEBUG=nil
local function Print(...)
    local flag=flag or true
    if DEBUG and flag then
       print(...) 
    end
end
local function Pprint(s,name,flag)
    local flag=flag or true
    if DEBUG and flag then
       pprint.pprint(s,name) 
    end
end

function parse_opt(line)
    local opt={}
    for i in line:gmatch("%s*([^%s]+)") do
        opt[#opt+1]=i
    end
    return opt
end

--return the x-th char in s
--if s is nil or "", then return ""
--if x is larger than string s's length then return ""
function get_char(s,x)
    local x=x or 1
    local s=s or ""
    return s:sub(x,x)
end
--if s end with suffix then return suffix, else nil
function string.endwith(s,suffix)
    local re=string.format("%s$",suffix)
    return string.match(s,re)
end

--if s start with prefix then return prefix, else nil
function string.startwith(s,prefix)
    return string.match(s,string.format("^%s",prefix)) 
end



--split xpath with '/' (only zero or one '/' is available) 
--if path is nil then return emtpy table
function xpath_split2(path)
    local ret={}
    path=path or ""
    for i,v in  string.gmatch(path,"/?([^/]+)/?") do
        ret[#ret+1]=i
    end
    return ret
end

--if path is nil or "",return ("","")
function xpath_split(path)
--    local last_char=get_char(path,-1)
    local basename=""
    local dirname=""
    if (not path) or path=="" then 
        return dirname,basename
    end

    local p=xpath_split2(path)

    local len=#p
    
    --not endwith '/' and not empty
--    if last_char~="/" and last_char~="" then
    if not string.endwith(path,"/")   then
        len=len-1
        basename=p[#p]
    end
    local sep="/"
    for i=1,len do
        if i==len then
            sep=""
        end
        dirname=dirname..p[i]..sep
    end
    return dirname,basename
end

--XXX:remove only one slash
local function remove_suffix_slash(path)
    assert(path,"can't be nil")
    if path:endwith("/") then
        path=path:sub(1,-2)
    end
    return path
end

function get_enum_type(db,enum_type)
   local ret={}
   local root=db:get_root(SCHEMA_ROOT):get_child(SCHEMA_ROOT2)
   local types=root:xpath(string.format("%s/*",enum_type))
   local _,base
   for i,v in pairs(types) do
       _,base=xpath_split(v.key)
       ret[#ret+1]=base
   end
   return ret
end
--path must be a valid configure xpath
--can have a suffix / (only one)
local function get_type(path)
    local db=mdb:new():init(mdb.create_env(mdb_path))
    return db:withReadOnly(function (db)
                                local root=db:get_root(ROOT):get_child(ROOT2)
                                local l=root:_xpath(remove_suffix_slash(path))
                                local attr=l:get_attrib()
                                Pprint(attr) 
                                if attr.type=="int" then
                                    return {type=attr.type,min=tonumber(attr.min),max=(tonumber(attr.max) or 0)}
                                elseif attr.type=="string" or attr.type=="ip" or attr.type=="path" then
                                    return {type=attr.type,maxLen=tonumber(attr.maxLen)}
                                elseif attr.type=="bool" then
                                    return {type=attr.type}
                                elseif attr.type=="bool" then
                                    return {type=attr.type}
                                elseif attr.type then
                                    local subtype=get_enum_type(db,attr.type)
                                    return {type="define",realtype=attr.type,subtype=subtype}
                                else
                                    return {}
                                end
                           end
                          )
end

--join two xpath like python's os.path.join
function xpath_join(path1,path2)
    path1=path1 or ""
    assert(path2,"path2 cannot be nil") 
    local sep="/"
--    if get_char(path2,1)=="/" then
    if path2:startwith("/") then
        return path2
    end

--    if get_char(path1,-1)=="/" or path1==""  then
    if path1=="" or path1:endwith("/") then
        sep=""
    end

    return path1..sep..path2
end

-- TODO:better completion ?
-- return all children keys start with prefix 
-- path: must without suffix '/'
local function xpath_completion(path,prefix)
    local db=mdb:new():init(mdb.create_env(mdb_path))
    return db:withReadOnly(function (db)
                                --Print("get:",path,">>",prefix)
                                local ret={}
                                local root=db:get_root(ROOT):get_child(ROOT2)
                                
                                local xpath=path
                                if xpath=="" then
                                 --   Print("empty")
                                    xpath=nil
                                end
                                --XXX:xpath can not have suffix '/'
                                local l=root:_xpath(xpath)
--                                Pprint(l,"GET") 
                                local children=l:get_child()
--                                Pprint(children,"children")
                                for i,node in pairs(children) do
                                    local dirname,basename=xpath_split(node.key)
                                    if string.startwith(basename,prefix) then
                                        ret[#ret+1]=xpath_join(path,basename)
                                    end
                                end
                                return ret
                            end
                            )
end


--[[
-- @s:string which not start with `space` character
--]]
function configure_completion(s,prefix)
    local path=s or "" 
    local dirname,basename=xpath_split(path)

    local ret={}
    for i,v in pairs(xpath_completion(dirname,basename)) do
        ret[i]=prefix..v
    end
    return ret
end

--command completion
local _completion=require "completion"
function xml_file_completion(s,prefix)
    return _completion.path_completion(s,prefix,".*%.xml$")
end

completion={}
completion.put=configure_completion
completion.get=configure_completion
completion.import=xml_file_completion
completion.export=xml_file_completion
completion.help=nil

--return true if path is a valid xpath
function xpath_check(path)
    if not path or path=="" then
        return nil 
    end
    return true
end



local cmds={}
function cmds.put(config,path,value)
    
    --return its mapping value if argument value is expected,return nil otherwise
    function isvalid_string(attr,value)
        function mapping(i)
            return i
        end
        if type(value)=="string" and #value<=attr.maxLen then
            return mapping(value)
        end
    end

    function isvalid_path(attr,value)
        function mapping(i)
            return i
        end
        if type(value)=="string" and #value<=attr.maxLen then
            return mapping(value)
        end

    end
    
    function isvalid_ip(attr,value)
        function mapping(i)
            return i
        end
        if type(value)=="string" and #value<=attr.maxLen then
            local a,b,c,d=string.match(value,"^(%d?%d?%d)%.(%d?%d?%d)%.(%d?%d?%d)%.(%d?%d?%d)$") 
            if a then
                if tonumber(a)<255 and tonumber(b)<255 and 
                    tonumber(c)<255 and tonumber(d)<255 then
                    return mapping(value)
                end 
            end 

        end

    end

    --return its mapping value if argument value is expected,return nil otherwise
    function isvalid_int(attr,value)
        function mapping(i)
            return i
        end
        local i=tonumber(value)
         
        if i and i>=attr.min  and (attr.max==0 or i<=attr.max) then
            return mapping(i)
        end
    end
    
    --if the argument value is expected,then return its mapping value 
    --return nil if not expected.
    function isvalid_bool(attr,value)
        function mapping(v)
            local m={}
            m["true"]=1
            m["false"]=0
            return m[v]
        end

        if value=="true" or value=="false" then 
            return mapping(value)
        end
    end

    function isvalid_define(attr,value)
        for i,v in pairs(attr.subtype) do
            if v==value then
                return true
            end
        end
    end

    function isvalid_default(attr,value)
        return true
    end

    isvalid_funcs={string=isvalid_string,int=isvalid_int,
                   bool=isvalid_bool,define=isvalid_define,
                   ip=isvalid_ip,path=isvalid_path}

    function check_and_get_input(attr,value)
        Pprint(attr,"check_and_get_input")
        local isvalid=isvalid_funcs[attr.type] or isvalid_default

        if not isvalid(attr,value) then
            local prompt={}
            local _type=attr.type
            if _type=="int" then
                prompt[#prompt+1]="value must be integer"
                prompt[#prompt+1]=string.format("valid range is from %s to %s",attr.min,attr.max)
            elseif _type=="string" or _type=="ip" or _type=="path" then
                prompt[#prompt+1]="value must be ".._type
                prompt[#prompt+1]=string.format("must less than %s  characters",attr.maxLen)
            elseif _type=="bool" then
                prompt[#prompt+1]="value must be bool"
            elseif _type=="define"  then   --self define type such as enum type
                local s=""
                for i,v in pairs(attr.subtype) do
                    s=s.." "..v
                end
                prompt[#prompt+1]="value must be one of "..s
            else
                prompt[#prompt+1]="unknown type,all input treated as string"
            end

            for i,v in pairs(prompt) do
                io.write(v,"\n")
            end

            repeat 
                io.write("please input value:")
                value=io.read()
            until isvalid(attr,value) 
        end

        return value
    end

    Print("put",path,value)
    if not xpath_check(path) then
        cmds.help("put")
        return 
    end
    local attr=get_type(path)
    Pprint(attr,"346")
    local v=check_and_get_input(attr,value)    
    config:put(path,v)
end


function cmds.get(config,path)

    Print("get",path)
    if not xpath_check(path) then
        cmds.help("get")
        return 
    end
    if string.endwith(path,"/") then
        path=path:sub(1,-2)
    end
    local value=config:get(path)
    io.write(string.format("%s:%s \n",path,value))
end


function cmds.help(arg)
    local help_msg={
            put="put <xpath> [value]",
            get="get <xpath> ",
            help="help [cmd] ",
            export="export xxx.xml",
            import="import xxx.xml",
         }
    local arg=arg or "all"
    local prompt=help_msg[arg] or table.concat(help_msg,"\n")
    io.write(prompt,"\n")
end

function cmds.import(config,xml_path)
    if xml_path then
        config:import(xml_path)
    else
        cmds.help("import")
    end
end


function cmds.export(config,xml_path)
    local xml_path=xml_path or string.format("config-%s.xml",os.date("%Y%m%d"))
    io.write("export to ",xml_path,"\n")
    config:export(xml_path)
end


local mt={__index=function (t,key) io.write("config ",key,":command not found\n") return cmds.help end }
setmetatable(cmds,mt)

function helper(argv)
    pprint.pprint(argv)
    pprint.pprint(cmds)

    table.remove(argv,1)
    pprint.pprint(argv)
    local cmd=argv[1] 
    table.remove(argv,1)
    if cmd then
        local config=_Config:new():init(mdb_path)
        cmds[cmd](config,unpack(argv))
        config:close()
    else
        cmds.help()
    end
end

function info()
    return "instance operation tool"
end

if ...=="__main__" then
    main()
end
