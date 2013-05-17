module(...,package.seeall)
local __main__ = ...
local __self__ = package.loaded[__main__]

local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"
local mdb = require "mdb"
require "str_utils"

quote = {}

function connect()
    local path = "./temp/am/quote"
    os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))
    db = mdb.mdb:new(path)
    return db
end

function quote:new(path)
    self._db = connect()
    self._db:with(init)
    setmetatable(o, self)
    self.__index = self
    return o
end

function init(db)
    root = db:get_root("Root")
    root:add_node("User")
    root:add_node("Domain")
    root:add_node("Oper")
    root:add_node("Role")
    pprint.pprint(root:to_xml())
    --user_init(db)
    oper_init(db)
    domain_init(db)
    db:commit()
end

function user_init(db)
    local root = db:get_root("Root")
		local user_root = root:_xpath("User")
    user = user_root:add_node("root")
    user:add_attrib("Type","Root")
    user = user_root:add_node("admin")
    user:add_attrib("Type","Admin")
		db:commit()
end

function domain_init(db)
    local root = db:get_root("Root")
    local oper_root = root:_xpath("Oper")
    local domain_root = root:_xpath("Domain")    
    hb = domain_root:add_node("HeadBank") 
    hb:add_attrib("Department", "总行")
    hb:add_attrib("Level", "1")
    branch = hb:add_node("Branch1")
    branch:add_attrib("Department", "分行1")
    branch:add_attrib("Level", "2")
    subbranch = branch:add_node("SubBranch1")
    subbranch:add_attrib("Department", "支行1")
    subbranch:add_attrib("Level", "3")
    host1 = subbranch:add_node("NormalHost")
    host1:add_attrib("HostName", "NormalHost")
    host1:add_attrib("IPAddress", "192.168.0.110")
    host1:add_attrib("Type", "host")
    device1 = host1:add_node("NormalSys")
    device1:add_attrib("Type", "device")
    device1:add_pair("devtype", "sys")
    subbranch2 = branch:add_node("SubBranch2")
    subbranch2:add_attrib("Department", "支行2")
    subbranch2:add_attrib("Level", "3")
    host2 = subbranch2:add_node("AfaHost")
    host2:add_attrib("HostName", "AfaHost")
    host2:add_attrib("IPAddress", "192.168.0.5")
    host2:add_attrib("Type", "host")
    device2 = host2:add_node("AfaSys")
    device2:add_attrib("Type", "device")
    device2:add_pair("devtype", "sys")
    device3 = host2:add_node("AfaPlat")
    device3:add_attrib("Type", "device")
    device3:add_pair("devtype", "afa")
    pprint.pprint(domain_root:to_xml())
end
  
function oper_init(db)
    local root = db:get_root("Root")
    local oper_root = root:_xpath("Oper")  
    topic = oper_root:add_node("topic") 
    report = topic:add_node("report") 
    report:add_node("subscribe")
    report:add_node("start")
    request = topic:add_node("request") 
    request:add_node("subscribe")
    request:add_node("start")
    control = topic:add_node("control") 
    control:add_node("subscribe")
    control:add_node("start")
    alert = topic:add_node("alert") 
    alert:add_node("1")
    alert:add_node("2")
    manage = oper_root:add_node("manage") 
    manage:add_node("start") 
    manage:add_node("stop")
    task = manage:add_node("task")
    task:add_node("select")
    task:add_node("insert")
    task:add_node("update")
    task:add_node("delete")
    manage:add_node("parameter")
    pprint.pprint(oper_root:to_xml())
end

function adduser(db, username, department, name, address) 
		local root = db:get_root("Root")
		local user_root = root:_xpath("User")
    user = user_root:add_node(username)
    user:add_attrib("Type","Noraml")
    user:add_pair('Department', department)	
    user:add_pair('Name', name)
    user:add_pair('Address', address)   
    user:add_node("Role") 	
		pprint.pprint(root:to_xml())
		db:commit()
end

function alteruser(db, username, pty)
    
end

function selectuser(db, username)
		local root = db:get_root("Root")
		local user_root = root:_xpath("User")
		if username == nil then
		    return user_root:to_xml()
		else
		    user = user_root:get_child(username)
		    return string.format("<User >%s</User>", user:to_xml())
		end
end

function deleteuser(db, username)
    local root = db:get_root("Root")
		local user_root = root:_xpath("User")
		user_root:remove(username)
		db:commit()
end

function addrole(db, rolename, devreg, operreg)
    local root = db:get_root("Root")
    local role_root = root:_xpath("Role")  
    role = role_root:add_node(rolename)
    role:add_pair('DeviceRegluar', devreg)
    role:add_pair('OperRegular', operreg)
    local dev = role:add_node("Device")
    device =  getdevice(db, devreg)
    for k,v in pairs(device) do
        dev:add_ref(v)
    end  
    local oper = role:add_node("Oper")
    operation = getoper(db, operreg)
    for k,v in pairs(operation) do
        oper:add_ref(v)
    end 
    db:commit()
end

function selectrole(db, rolename)
    local root = db:get_root("Root")
    local role_root = root:_xpath("Role") 
    if rolename == nil then
		    return role_root:to_xml()
		else
		    role = role_root:get_child(rolename)
		    return string.format("<Role >%s</Role>", role:to_xml())
		end
end

function deleterole(db, rolename)
    local root = db:get_root("Root")
		local role_root = root:_xpath("Role")
		role_root:remove(rolename)
		db:commit()
end

function alterrole(db, rolename, pty)
    
end

function getoper(db, operg)
    local root = db:get_root("Root")
    local oper_root = root:_xpath("Oper") 
    return oper_root:xpath(operg)
end

function getdevice(db, deviceg)
    local root = db:get_root("Root")
    local domain_root = root:_xpath("Domain") 
    return domain_root:xpath(deviceg)
end

function addaccess(db, username, rolename)
    local root = db:get_root("Root")
		local userrole = root:_xpath("User/" .. username .. "/Role")
		local role = root:_xpath("Role/" .. rolename)
		userrole:add_ref(role) 
		print('after add access', root:_xpath("User"):to_xml())
		db:commit()
end

function deleteaccess(db, username, rolename)  
    local root = db:get_root("Root")
		local userrole = root:_xpath("User/" .. username .. "/Role")
		if rolename ~= nil then
		    userrole:remove(rolename)
		else
		    userrole:remove()
		end
		print('after delete access', userrole:to_xml())
		db:commit()
end

function eventaccess(db, username, agentname, event)
    local root = db:get_root("Root")
    role = root:_xpath("User/" .. username .. "/Role")
    pprint.pprint(role)
    for rolename, rolenode in pairs(role:get_child()) do
        local devicenode = rolenode:get_child("Device")
        if devicefind(db, devicenode, agentname) then
            local opernode = rolenode:get_child("Oper")
            if operfind(db, opernode, event) then
                return true
            end
        end
    end
    return false
end

function devicefind(db, devicenode, agentname)
    devname, devtype = agentname:match("(%w+)::(%w+)") 
    return devicenode:exist(devname)
end

function operfind(db, opernode, event)
    eventlist = string.split(event, "/")
    pprint.pprint(eventlist,'eventlist')
    local idx = 1
    --print(opernode:to_xml())
    while idx <= #eventlist do
        item = eventlist[idx]
        print('item', item)
        print(opernode:to_xml())
        if opernode:exist(item) then
            opernode = opernode:get_child(item)
            idx = idx + 1
        else
            return false
        end
    end
    return true 
end


if ... == __main__ then
   function testinit()
       db = connect()
       db:with(init)
       db:with(adduser, 'mt', '总行', 'Ma.Tao', 'ma.tao@agree.com.cn')
       db:with(adduser, 'wzhq', '支行1', 'Wang.ZhiQiang', 'wang.zq@agree.com.cn')
       users = db:with(selectuser) 
       users = db:with(selectuser, 'mt')
       db:with(addrole, "role1", "HeadBank/*","*")
       db:with(addrole, "role2", "HeadBank/Branch1/SubBranch1/*","topic/*")
       ret = db:with(selectrole, "role1")
       print(ret)
       ret = db:with(selectrole, "role2")
       print(ret)
       db:with(addaccess, 'mt', "role1")
       db:with(addaccess, 'mt', "role2")
       db:with(addaccess, 'wzhq', "role2")
       return db
   end
   
   db = testinit()
   --db:with(deleteaccess, 'mt')
   --db:with(deleteaccess, 'wzhq', "role2")
   --db:with(deleteuser, 'wzhq')
   --users = db:with(selectuser) 
   --db:with(deleterole, 'role1')
   ret = db:with(eventaccess, 'mt', 'NormalHost::sys::3374', 'report/subscribe')
   print(ret)
   ret = db:with(eventaccess, 'mt', 'NormalHost1::sys::3374', 'alert/1')
   print(ret)
end