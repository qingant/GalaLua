module("mdb_copy",package.seeall)
local mdb=(require "mdb").mdb
local pprint=require "pprint"
local path=require "path"
local cmd=require "cmd"

local function _mdb_copy(src,dst)
    local e=mdb._create_env(src)
    local ok,emsg=xpcall(function () return e:copy(dst) end,debug.traceback)
    e:close()
    if not ok then
        cmd.cmd_error(emsg or "mdb_copy error")
    end
end

local function existed(f)
    local fp=io.open(f)
    if fp~=nil then
        fp:close()
        return true
    end
    return false
end

local function fuser(p)
    local f=io.popen(string.format("fuser %s 2>/dev/null",p))
    local s=f:read("*all")
    return s
end

local function available(p)
    local data_file=string.format("%s/data.mdb",p)
    if not existed(data_file) then
        return cmd.cmd_error("invalid mdb directory")
    end
    local s=fuser(data_file)
    if s=="" then
        return cmd.yes_or_no("Ensure there's no process using this mdb")
    else
        cmd.writef("There are processes using this mdb: %s\n",s)
        return cmd.cmd_error("some processes are using this mdb")
    end
end

local function mdb_copy(src,bak)
    local src=path.pretty2(src)

    if not available(src) then
        return cmd.cmd_abort()
    end

    local bak=bak or string.format("%s-%s",src,os.date("%Y%m%d%H%M"))
    local ok,d=pcall(path.ls,bak)
    if ok and next(d) then
        cmd.writef("Backup directory is existed and not empty:%s\n",bak)
        if cmd.yes_or_no("Files in that directory will *LOST*") then
            return 
        end
    end
    os.execute(string.format("rm -rf %s && mv %s %s && mkdir -p %s ",bak,src,bak,src))
    _mdb_copy(bak,src)
    cmd.writef("\nBackup path:%s\nDONE\n",bak)
end


local helpmsg="mdb_copy  <src_dir>  [bak_dir]"
local function help()
    io.write(helpmsg,"\n")
end

--command
function helper(argv)
    table.remove(argv,1)
    local src,dst=unpack(argv)
    if src==nil then
        help("mdb_copy")
        return 
    end
    mdb_copy(src,dst)
end

--command completion
local _completion=require "completion"

local function dir_completion(s,prefix)
    local pre,arg=string.match(s,"(.*%s+)(.*)$")
    if not arg then
        arg=s
        pre=""
    end
    return _completion.path_completion(arg,prefix..pre)
end

completion=dir_completion

-- }} for command console
