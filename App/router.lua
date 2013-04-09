
module(...,package.seeall)
local cjson = require "cjson"
local configure=require("configure")
package.path=configure.PATH
package.cpath=configure.CPATH

local os=require("os")
local dbaccess=require("dbaccess")
local string=require("string")

local router_path=os.getenv("HOME").."/tmp/router.db"

Router_M={path=router_path,name="router",struct="(name text, field text,host text,port integer,gpid integer,dev_type text,app_type text,lsr_name text,timestamp integer, dev_catagory text, userdata text)"}

function device()
   Devices = Router_M:new{name="device",struct="(dev_type text, desp text)"}
   function Devices:add(dev_type, desp)
      local cmd=string.format("insert into %s values('%s','%s')", self.name, dev_type, desp)
      self._conn:execute(cmd) 
      self._conn:commit()
   end
   function Devices:find(dev_type)
      local cmd=string.format("select * from %s where dev_type='%s'",self.name, dev_type)
      local ret=self._conn:execute(cmd)
      self._conn:commit()
      return ret:fetchall()
   end
   function Devices:delete(dev_type)
      local cmd = string.format("delete from %s where dev_type='%s'", self.name, dev_type)
      local ret = self._conn:execute(cmd)
      self._conn:commit()
   end
   function Devices:all()
      local cmd=string.format("select * from %s ",self.name)
      local ret=self._conn:execute(cmd)
      self._conn:commit()
      return ret:fetchall()
   end


   return Devices
end
function subscription()
    --[==[
            dev text, whom text,topic text, event text,who text
            设备类型,  设备id  ,主题， 事件,  订阅者
    --]==]
    Subscriber=Router_M:new{name="subscription",struct="(dev text, whom text,topic text, event text, who text)"}

--    function Subscriber:find_by_dev(dev)
--        return self:_find("dev",dev)
--    end
--    function Subscriber:find_by_whom(whom)
--        return self:_find("whom",whom)
--    end
--
--    function Subscriber:find_by_topic(topic)
--        return self:_find("topic",topic)
--    end

    function Subscriber:find(v)
        local cmd=string.format("select * from %s where %s",self.name,v)
        local ret=self._conn:execute(cmd)
        return ret:fetchall()
    end

    function Subscriber:delete(v)
        local cmd=string.format("delete from %s where %s",self.name,v)
        self:execute(cmd)
        self:commit()
    end

    function Subscriber:subscribe(dev,whom,theme, event, who)
        local cmd=string.format("insert into %s values('%s','%s','%s','%s', '%s')",self.name,dev,whom,theme,event, who)
        self._conn:execute(cmd) 
        self._conn:commit()
    end
    
    function Subscriber:all()
       local cmd=string.format("select * from %s",self.name)
       local ret=self._conn:execute(cmd)
       self._conn:commit()
       return ret:fetchall()
    end

    return Subscriber
end

function router()
    Router=Router_M:new()

    function Router:register(name,field,host,port,gpid,dev_type,app_type,lsr_name,timestamp,cata, userdata)
        --delete the old one if existed
        local cmd=string.format("delete from %s where name='%s'",self.name,name)
        self._conn:execute(cmd)

        cmd=string.format("insert into %s values('%s','%s','%s',%d,%d,'%s','%s','%s',%d, '%s', '%s')",self.name,name,field,host,port,gpid,dev_type,app_type,lsr_name,timestamp, cata, userdata)

        self._conn:execute(cmd) 
        self._conn:commit()
    end

    function Router:delete(value,bytype)
        bytype=bytype or "name"
        local cmd=string.format("delete from %s  where %s='%s'",self.name,bytype,value)
        self._conn:execute(cmd) 
        self._conn:commit()
    end

    function Router:delete_stale(lsr_name,timestamp)
        local cmd=string.format("delete from %s where lsr_name='%s' and timestamp<>%d ", self.name, lsr_name, timestamp)
        self._conn:execute(cmd) 
        self._conn:commit()
    end

    function Router:find_by_cata(field)
        return self:_find("dev_catagory",field)
    end

    function Router:find_by_app_type(app_type)
        return self:_find("app_type",app_type)
    end

    function Router:find_by_dev_type(dev_type)
        return self:_find("dev_type",dev_type)
    end

    function Router:find_by_name(name)
        return self:_find("name",name)[1]
    end

    return Router
end


function Router_M:new(o)
    local o=o or {}
    setmetatable(o,self)
    self.__index=self
    self.init(o)
    return o
end

function Router_M:init()
    self._conn=dbaccess.connect("SQLite3",self.path)
    self._conn:autocommit(true)

    --query the table table_name exists or not
    function create_table_if_not_exist(name,table_struct)
        local cmd="select name from sqlite_master where type='table' and name='"
        local ret=self._conn:execute(cmd..name.."'")
        if not ret:fetchall()[1] then
            print("create table....")
            self._conn:execute("create table "..name..table_struct)
            self._conn:commit()
        end
    end
    print("init::",self.name,self.struct)
    create_table_if_not_exist(self.name,self.struct)
end


function Router_M:_find(bytype,type_value)
    local ret=self._conn:execute("select * from "..self.name.." where "..bytype.."='"..type_value.."'")
    return ret:fetchall()
end

function Router_M:finalizer()
    print("destructor")
    self._conn:commit()
    self._conn:close()
end
