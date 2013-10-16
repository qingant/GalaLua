module(...,package.seeall)
local gar=require "gar"
local path=require "path"
local string=require "string"

function pack_aim(manifest_file,_in)
    assert(manifest_file and _in,"invalid argument")
    local _in=_in
    local g=gar.new(manifest_file,gar.sub_root)
    if type(_in)=="string" then
        _in[#_in+1]=_in
    end
    for _,in_path in ipairs(_in) do
        g:add2gar(in_path)
    end
    g:close()
end



-- {{ for command console

local cmds={}

--FIXME: not support assign output path 
function cmds.pack(...)
    local gar=...
    local path={}
    for i=2,select("#",...) do
        path[#path+1]=select(i,...)
    end
    if not (gar and next(path))  then
        io.write("gar pack manifest_file  path1 [path2]...\n")
        return 
    end
    
    pack_aim(gar,path)
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


