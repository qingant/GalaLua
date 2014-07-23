--[[
--Author liuwenxue
--Date 2014-07-17
--
]]
module(...,package.seeall)
local get_date = require("http.utils").get_date
local diff_date = require("http.utils").diff_date
local pprint = require("pprint")

mdb = require("mdb").mdb
session = {}
session.Fields = {"id", "value","expires","path","domain"}

function session:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function session:init()
    --in case of c10k duplicate
    self.id = math.random(os.time()) + math.random(1000000)
    --step if you want to support transaction
    self.value = 1
    --60*60*24*30  one month
    self.expires = 2592000 
    self.path = "/"
    self.domain = "127.0.0.1"
    --if you want write your store,
    --self.store must support new,init,remove,save,exist
    --and raise erro if set, remove key which isn't exist . 
    --take class lmdb as example
    self.store = lmdb:new():init()
    return self
end
function session:get(key)
    for _,k in pairs(self.Fields) do
        if key == k then
            return self[key]
        end
    end
    return nil
end

function session:set(key,value)
    for _,k in pairs(self.Fields) do
        if k == key then
            self[key] = value
            break
        end
    end
end

--设计原则：如果不存在，是抛出异常，还是什么也不做
function session:remove(key)
    if self.store:exist(key) then
        self.store:remove(key)
    else
        error("remove key isn't exist")
    end
end
    
function session:destory()
    self.store:remove()
end

function session:save(key, value)
    --print("key",key)
    --print("value",value)
    --pprint.pprint(self,"self")
    self.store:save(key,value)
end

function session:exist(key)
    return self.store:exist(key)
end

--ses : a session object
function session:to_string()
    return string.format("id=%s, value=%s, expires=%s, domain=%s, path=%s",
    self.id,self.value,get_date(self.expires), self.domain,self.path)
end

--function session:parse(cookie)
--    local ses = split(cookie,",")
--    local session = {}
--    for _,k in pairs(ses) do
--        local tmp = split(k,"=")
--        session[strip(tmp[1])] = strip(tmp[2])
--    end
--    return {name = session["SessionID"],  value = session["Step"], expires = session["Expire"], 
--    path = session["Path"], domain = session["Domain"]}
--end

-- session["Expire"] same as os.date()'t return
function session:is_expired(expires)
    if not expires then
        error("function is_expire argument expires is nil")
    end
    str = get_date()
    --print("str",str)
    --print("expire",expires)
    local t = diff_date(str,expires)
    if t >= 0 then
       print("has expired")
       return true
    else
       print("hasn't expired")
       return false
    end
end
lmdb = {}

function lmdb:new(o)
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function lmdb:init()
    local path = "/tmp/lmdb/session"
    os.execute(string.format("mkdir -p %s",path))
    self.store = mdb:new():init(mdb.create_env(path))
    self.root = "Session"
    return self
end
function lmdb:exist(key)
    function _exist(db,elem) 
        e = db:get_root(self.root)
        return e:exist(elem) 
    end
    return self.store:with(_exist,key)
end

function lmdb:save(key,value)
    function _save(db, key, value)
        local e = db:get_root(self.root)
        local id = e:add_node(key)
        id:set_value(value)
        db:commit()
    end
    self.store:with(_save, key, value)
end
function lmdb:remove(key)
    function _remove(db, elem)
        local e = db:get_root(self.root)
        e:remove(elem)
    end
    self.store:with(_remove, key)
end
