module(...,package.seeall)
local gar=require "gar"
local path=require "path"
local string=require "string"

function pack_aim(manifest_file,_in)
    assert(manifest_file and _in,"invalid argument")
    local _in=_in
    local g=gar.new(manifest_file,gar.sub_root)
    local name=g:file_name()
    if type(_in)=="string" then
        _in[#_in+1]=_in
    end
    for _,in_path in ipairs(_in) do
        g:add2gar(in_path)
    end
    g:close()
    return name
end



-- {{ for command console

local cmd=require "cmd"
local _cmds=cmd.init_cmd("gar")

--FIXME: not support assign output path 
function pack(...)
    local gar=...
    local path={}
    for i=2,select("#",...) do
        path[#path+1]=select(i,...)
    end
    if not (gar and next(path))  then
        return cmd.cmd_error("gar pack <Manifest> <path1> [path2]...")
    end
    
    local name=pack_aim(gar,path)
    io.write(string.format("Done! package:'%s' is in current directory.\n",name))
end


--FIXME:better show format
function show(gar)
    if not gar then
        return cmd.cmd_error("gar show xxx.gar")
    end
    io.write("TODO \n")
end

local cmd_list={}
cmd_list.pack={pack,"pack aim package","gar pack <Manifest> <path1> [path2]..."}
cmd_list.show={show,"show aim package","gar show xxx.gar"}
for name,cmd in pairs(cmd_list) do
    _cmds:add(name,cmd[1],cmd[2],cmd[3])
end


function helper(argv)
    return _cmds:helper(argv)
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

completion={help=""}
completion.pack=dir_completion
completion.show=gar_completion

-- }} for command console

if ...=="__main__" then
    print("=================================")
    
    local zipfd=zip.open("test.gar")
    pprint.pprint(search_module(zipfd,"lsr"))
end


