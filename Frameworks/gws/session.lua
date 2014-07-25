--[[
--Author liuwenxue
--Date 2014-07-17
--
]]
module(...,package.seeall)

local get_date = require(_PACKAGE .. "utils").get_date
local diff_date = require(_PACKAGE .. "utils").diff_date
local pprint = require("pprint")

session = {}

function session:new()
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function session:init(session_manager, id)
    self._id = id
    self._mgr = session_manager
    self._storage = self._mgr:get_session_storage(id)
    self._expires = 2592000
    self._expired = nil
    self._path = "/"
    self._domain = "127.0.0.1"
    return self
end

function session:get_id()
    return self._id
end

function session:get(key)
    return self._mgr:get_database():withReadOnly(function (db)
            local v = self._storage:get_child(key)
            if v == nil then
                return nil
            elseif v:is_leaf() then
                return v:get_value()
            else
                return v:to_table()
            end
                                                end)
end

function session:set(key, value)
    self._mgr:get_database():with(function (db)
            self._storage:remove(key) -- if  exist
            if type(value) == "table" then
                self._storage:add_node(key):add_table(value)
            else
                self._storage:add_node(key):set_value(tostring(value))
            end
            db:commit()
                                 end)
end

function session:to_string()
    -- TODO: pretty print
    return self._storage:to_xml()
end

function session:destory()
    self._mgr:remove_session(self._id)
end

local mdb = require("mdb").mdb
session_manager = {}

function session_manager:new()
    local o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function session_manager:init(path)
    self._database =  mdb:new():init(mdb.create_env(path))
    self._serial = 0
    return self
end

function session_manager:generate_session_id()
    local id = string.format("G%dS%dT%d",__id__, self._serial, glr.time.now())
    self._serial = self._serial + 1
    return id
end
function session_manager:get_database()
    return self._database
end
function session_manager:get_session_storage(id)
    return self._database:get_root("session"):get_child(id)
end
function session_manager:remove_session(id)
    self._database:with(function ()
            self._database:get_root("session"):remove(id)
            self._database:commit()
    end)
end
function session_manager:exist(id)
    return self._database:withReadOnly(function (db)
            return self._database:get_root("session"):get_child(id) ~= nil
    end)
end
function session_manager:create_session()
    local id = self:generate_session_id()
    self._database:with(function (db)
            self._database:get_root("session"):add_node(id)
            db:commit()
    end)
    local session = session:new():init(self,id)
    return session
end

function session_manager:get_session_ref(id)
    return session:new():init(self, id)
end