--[[
-- 
-- interactive console for configure
--
--
--]]

local os=require "os"

local string=require "string"
local pprint=require "pprint"
local L = require 'linenoise'
local cjson=require "cjson"
local _Config=require "config"
local _Config=_Config.config()
local mdb=(require "mdb").mdb

local prompt, history = '> ', 'history.txt'

local path = "/tmp/test_config"

local ROOT="Configure"
local ROOT2="Contents"

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
--    Print("strartwith:",s,prefix)
    local re=string.format("^%s",prefix)
    return string.match(s,re)
end

local db=mdb:new():init(mdb.create_env(path))

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

--path must be a valid configure xpath
--can have a suffix / (only one)
local function get_type(path)
    return db:withReadOnly(function (db)
                                local root=db:get_root(ROOT):get_child(ROOT2)
                                local l=root:_xpath(remove_suffix_slash(path))
                                local attr=l:get_attrib()
                                Pprint(attr) 
                                local value_type=attr.type
                                if attr.type=="int" then
                                    return {type=attr.type,min=tonumber(attr.min),max=(tonumber(attr.max) or 0)}
                                elseif attr.type=="string" then
                                    return {type=attr.type,maxLen=tonumber(attr.maxLen)}
                                elseif attr.type=="bool" then
                                    return {type=attr.type}
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
local function get(path,prefix)
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
                                   -- Print("get:",node.key)
                                   -- Print("get:",basename)
                                    if string.startwith(basename,prefix) then
                                        ret[#ret+1]=xpath_join(path,basename)
                                    end
                                end
                                return ret
                            end
                            )
end


--TODO:return completion should be the same as input
function configure_completion(s)
    local argv=parse_opt(s)
    local completion={}
    local path=argv[2] or ""
    
    local dirname,basename=xpath_split(path)
    local first,last
--    if argv[1]=="put" or argv[1]=="get" then
    local s_cmd=""
    local cmds={"put","get"}
    for i,v in pairs(cmds) do
        local re="^%s*"..v.."%s+"
        first,last=s:find(re)
        if first then
            s_cmd=s:sub(1,last)
            completion=get(dirname,basename)
            break
        end
    end
    Print("first:",first,"last:",last)
    local ret={}
    for i,v in pairs(completion) do
        ret[i]=s_cmd..v
    end
    return ret
end

--TODO:better completion
function addcompletion(c,s)
    local completion={
       h={"help"},
       p={"put"},
       g={"get"},
       e={"exit"},
    }
    local all=completion[s] or configure_completion(s)
     
    for i,v in pairs(all) do
 --       Print(v)
        L.addcompletion(c,v)
    end
end

--return true if path is a valid xpath
function xpath_check(path)
    if not path or path=="" then
        return nil 
    end
    return true
end

local operations={}
function operations.put(config,path,value)

    function help()
        io.write("put xpath value\n")
    end
    
    --return its mapping value if argument value is expected,return nil otherwise
    function isvalid_string(attr,value)
        function mapping(i)
            return i
        end
        if type(value)=="string" and #value<=attr.maxLen then
            return mapping(value)
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
    function isvalid_default(attr,value)
        return true
    end

    isvalid_funcs={string=isvalid_string,int=isvalid_int,bool=isvalid_bool}

    function check_and_get_input(attr,value)
        Pprint(attr,"check_and_get_input")
        local isvalid=isvalid_funcs[attr.type] or isvalid_default

        if not isvalid(attr,value) then
            local prompt={}
            if attr.type=="int" then
                prompt[#prompt+1]="value must be integer"
                prompt[#prompt+1]=string.format("valid range is from %s to %s",attr.min,attr.max)
            elseif attr.type=="string" then
                prompt[#prompt+1]="value must be string"
                prompt[#prompt+1]=string.format("must less than %s  characters",attr.maxLen)
            elseif attr.type=="bool" then
                prompt[#prompt+1]="value must be bool"
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
        help()
        return 
    end
    local attr=get_type(path)

    local v=check_and_get_input(attr,value)    
    Print(v)
    config:put(path,v)
end


function operations.get(config,path)

    function help()
        io.write("get xpath \n")
    end

    Print("get",path)
    if not xpath_check(path) then
        help()
        return 
    end
    if string.endwith(path,"/") then
        path=path:sub(1,-2)
    end
    local value=config:get(path)
    io.write(string.format("%s:%s \n",path,value))
end


function operations.help(config,arg)
    local help_msg={
            put="put <xpath> [value]",
            get="get <xpath> ",
            help="help [cmd] ",
            notfound="no such command",
         }
    local arg=arg or "help"
    local prompt=help_msg[arg] or help_msg.notfound
    io.write(prompt,"\n")
end

function operations.exit()
    glr.exit()
end

function main()
    local mt={__index=function () return function () print("Command not found!!!") end  end}
    setmetatable(operations,mt)
    local config = _Config:new():init(path)
--    config:schemaGen()

    L.historyload(history) -- load existing history
    L.setcompletion(addcompletion)

    L.clearscreen()
    Print '----- Testing lua-linenoise! ------'
    Pprint(L)
--    Pprint(glr)
    print '----- Welcome! have a nice day! ------\n'
    local line = L.linenoise(prompt)
    while line do
        if #line > 0 then
            L.historyadd(line)
            L.historysave(history) -- save every new line
            --parse cmdline
            local argv=parse_opt(line)
            Pprint(argv)
            operations[argv[1]](config,argv[2],argv[3])
            
        end
        --L.clearscreen()
        line = L.linenoise(prompt)
    end
end

if ...=="__main__" then
    main()
end
