--[[
-- instance tool
--]]
module(...,package.seeall)

local os=require "os"

local string=require "string"
local pprint=require "pprint"
local config=require "config"

local GDK=os.getenv("GDK_HOME") or "/opt/agree/gdk"


local function execute(cmd)
    local ret=0
    print(cmd)
    --local ret=os.execute(cmd)
    local error_msg=string.format("command %s failed",cmd)
    if ret~=0 then
        return nil,error_msg
    end
    return true
end

--install ... to @dest
function install(uid,gid,dest,...)
    local source=table.concat({...}," ")
    print(source)
    local cmd1=string.format("install -o %s -g %s -d %s",uid,gid,dest)
    local cmd2=string.format("install -o %s -g %s %s %s",uid,gid,source,dest)
    
    execute(string.format("%s && %s ",cmd1,cmd2))
end

--[[
--Usernames must start with a lower case letter or an underscore, followed by 
--lower case letters, digits, underscores, or dashes. They can end with 
--a dollar sign. In regular expression terms: "[a-z_][a-z0-9_-]*[$]?". 
--Usernames may only be up to 32 characters long.
]]
local function is_valid_username(name)
    return name:match("^[%l_][%l%d_%-]*[%$]?$")
end

local function useradd(user)
    local cmd=string.format("useradd -m %s -p 123 && chage -d 0 %s",user,user)
    return execute(cmd)
end

local function userdel(user)
    local cmd=string.format("userdel -r %s",user)
    return execute(cmd)
end

--parse /etc/passwd
local function parse_etc_passwd()
    local passwd={}
    local passwd_fmt=string.format("^(.*)%s$",(":(.*)"):rep(6))

    for line in io.lines("/etc/passwd") do
        local name,password,uid,gid,comment,home,shell=line:match(passwd_fmt) 
        passwd[name]={ 
            name=name,
            password=password,
            uid=uid,
            gid=gid,
            comment=comment,
            home=home,
            shell=shell
        }
        passwd[uid]=passwd[name]
    end
    pprint.pprint(passwd)
    return passwd
end

local function get_user(name)
    return parse_etc_passwd()[name] 
end

local function shell_env_file(home)
    assert(home,"not pass home")
    local gdk=string.format("export GDK_HOME=%s",GDK)
    local path=string.format("export PATH=$PATH:$GDK_HOME/bin")
    local ld=string.format("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GDK_HOME/lib")

    local rc=string.format("%s/.bashrc",home)

    local f=io.open(rc,"a")
    f:write(gdk,"\n",path,"\n",ld,"\n")
    f:close()
end


--user must existed
function instance(user,gdk_home)
    local u=assert(get_user(user),"user error")
    
    local Instance={user=u,gdk_home=gdk_home}
    
    --TODO:Can not work
    function Instance:import_configure(from,to)
        if true then
            return 
        end

        local path=to or user.home.."/cfg/config"
        local config_path=from or self.gdk_home.."/share/config.xml"

        os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))
        local db = mdb:new():init(mdb.create_env(path))
        local imp = config._Importer:new{_db = db}
        imp:importFromXML(config_path, "Schema")
--        imp:showResult()
        local conf= config._Config:new():init(path)
        conf:schemaGen()
    end

    function Instance:export_configure()

    end
    function Instance:shell_env()
        shell_env_file(self.user.home)
    end
    return Instance
end
--[[
function instance_manager()
    local manager={instancerc=GDK.."/share/instancerc",instances=nil}
    
    function manager:init(instancerc)
        if instancerc then
            self.instancerc=instancerc
        end
        local ins=assert(io.read(assert(io.open(self.instancerc)),"*a"))
        self.instances=cjson.decode(ins)
    end

    --@id:now is user name
    function manager:get_instance(id)
        return self.instances[id]
    end

    function manager:add_instance(instance)
        local id=instance.user.name
        self.instances[id]=instance
    end

    return manager
end
]]

function create_instance()
    io.write("create instance....\n") 
    local name
    repeat 
        io.write("input user name:") 
        name=io.read()

    until (is_valid_username(name))
    
    if not get_user(name) then
        io.write("user ",name," is not existed,create it? (y/n) [y]") 
        local ensure=io.read()
        if ensure=="" or ensure=="y" then
            useradd(name)
            io.write("user ",name," is created, the password is 123\n") 
        else
            return nil
        end
    end
    io.write("instance installing ....\n") 
    local _instance=instance(name,GDK)
--    _instance:import_configure()
    _instance:shell_env()
    io.write("Done!\n") 
end

function del_instance(user)
   print("del_instance",user,"DONE") 
end


local cmds={}
cmds.create=create_instance
cmds.delete=del_instance

function cmds.help()
    io.write("instance : ")
    io.write("available commands:")
    for i in pairs(cmds) do
        io.write(i," ")
    end
    io.write("\n")
end
local mt={__index=function (t,key) io.write("instance ",key,":command not found\n") return cmds.help end }
    
setmetatable(cmds,mt)

function helper(argv)
    pprint.pprint(argv)
    pprint.pprint(cmds)

    table.remove(argv,1)
    pprint.pprint(argv)
    local cmd=argv[1] 
    table.remove(argv,1)
    if cmd then
        cmds[cmd](unpack(argv))
    else
        cmds.help()
    end
end
--for completion
completion={create={},delete=""}

function info()
    return "instance operation tool"
end

if ...=="__main__" then

end
