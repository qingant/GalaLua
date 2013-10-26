module("mdb_copy",package.seeall)
local lightningmdb_lib=require "lightningmdb"
local lightningmdb = _VERSION=="Lua 5.2" and lightningmdb_lib or lightningmdb
local pprint=require "pprint"
local path=require "path"

local function _mdb_copy(src,dst)
    local e=lightningmdb.env_create()
    local num_pages = 1024
    e:set_mapsize(num_pages*4096)
    assert(e:open(src,0,420))
    e:copy(dst)
    e:close()
end

--TODO:ensure all process open mdb must be stopped
local function available(p)
end

local function mdb_copy(src,bak)
    local src=path.pretty2(src)

    if not available(src) then
        return 
    end

    local bak=bak or string.format("%s-%s",src,os.date("%Y%m%d%H%M"))
    local ok,d=pcall(path.ls,bak)
    if ok and next(d) then
        io.write("Backup directory existed and not empty:",bak,"\n")
        io.write("Files in that directory will *LOST* if continue (y|n)[n]")
        local input=io.read()
        if input~="y" then
            io.write("Aborted\n")
            return 
        end
    end
    os.execute(string.format("rm -rf %s && mv %s %s && mkdir -p %s ",bak,src,bak,src))
    _mdb_copy(bak,src)
    io.write("\nBackup path:",bak,"\n")
    io.write("DONE\n")
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
