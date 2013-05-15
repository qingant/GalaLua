--[[
Athor: Ma Tao
    
    Simple NXD(Native XML Database) for Gala

]]

module(...,package.seeall)

local lmdb = require "lightningmdb"
local pprint = require "pprint"
require "str_utils"
require "tab_utils"
local logging = require("logging")
--[[
    Node:
    a:key`value  attribute key -> value
    c:key        children  path/key -> Node
    v:value      list item  [value ...]
    s:name`link  symbolink to link
    r:ref        node who refered to this node
]]
local function split(pString, pPattern)
   local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pPattern
   local last_end = 1
   local s, e, cap = pString:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
     table.insert(Table,cap)
      end
      last_end = e+1
      s, e, cap = pString:find(fpat, last_end)
   end
   if last_end <= #pString then
      cap = pString:sub(last_end)
      table.insert(Table, cap)
   end
   return Table
end

local function xpath_split(path)
    if string.contains(path, "/") then
        return string.match(path, "(.*)/([^/]*)")
    else
        return "",path
    end
end

mdb = {}
local element = {}
local path_sep = "/"
local indent_one = "    "
local num_pages = 1024*1024
function mdb:new(path)
    local e = lightningmdb.env_create()
    e:set_mapsize(num_pages*4096)
    e:open(path,0,420)
    -- local t = e:txn_begin(nil,0)
    -- local db = t:dbi_open(nil,lightningmdb.MDB_DUPSORT)
    local o = {env = e, txn = nil, dbi = nil}
    setmetatable(o, self)
    self.__index = self
    return o
end

function mdb:close( ... )
    self.env:close()
end

function mdb:get_root(name)
    local o = element:new{_db = self, 
                       key = path_sep .. name,
                       is_root = true,
                       e_type = "regular"
    }
    o._root = o
    return o
end

function mdb:clear()
    -- TODO:
end
function mdb:_implement_storage()
    -- show all key-value pairs in this document
    -- TODO:
    local cur = self.txn:cursor_open(self.dbi)
    local flag = lightningmdb.MDB_NEXT
    repeat
        local k,v = cur:get(self.key, flag)
        print("IMPL",k, "->",v)
    until k == nil
end
function mdb:commit()
    if  self.txn ~= nil then
        self.txn:commit()
        self.txn = nil
        self.dbi = nil
    else
        error("Not In Transaction")
    end
    -- self.txn = self.env:txn_begin(nil, 0)
end

function mdb:abort()
    if  self.txn ~= nil then
        self.txn:abort()
        self.txn = nil
        self.dbi = nil
    else
        error("Not In Transaction")
    end
    -- self.txn = self.env:txn_begin(nil, 0)
end
function mdb:beginTrans()
    assert(self.txn == nil, "Alredy in Transaction")
    self.txn = self.env:txn_begin(nil, lightningmdb.MDB_RDONLY)
    self.dbi = self.txn:dbi_open(nil,lightningmdb.MDB_DUPSORT)
end
function mdb:beginWRTrans()
    assert(self.txn == nil, "Alredy in Transaction")
    self.txn = self.env:txn_begin(nil, 0)
    self.dbi = self.txn:dbi_open(nil,lightningmdb.MDB_DUPSORT)
end
mdb.beginRDTrans = mdb.beginTrans

function mdb:_with(action, ...)
    local err, value = pcall(action, self, ...)
    if self.txn ~= nil then
        self:abort()
    end
    if not err then
        error(value)
    end
    return value
end
function mdb:with(action, ...)
    self:beginWRTrans()
    return self:_with(action, ...)
end
function mdb:withReadOnly(action, ...)
    self:beginTrans()
    return self:_with(action, ...)
end
function element:new(o)
    -- local key = "/" .. name
    local o =  o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end
-- function element:init(_db, key, e_type)
--  table.update(self, {_db = _db,
--                      key = key,
--                      e_type = e_type,
--                      })
    
-- end
function element:get_root()
    return self._root
end
function element:is_leaf()
    if self._is_leaf == nil  then
        self._is_leaf = (#self:get_value() ~= 0)
    end
    return self._is_leaf
end
function element:walk(op, ...)
    op(self,...)
    for k,v in pairs(self:get_child()) do
        print(k,v)
        v:walk(op, ...)
    end
end

-- w3c style xpath support, not that complete
--function element:xpath( path )

--end

function element:_xpath(path)
    -- if not self.is_root then
    --  error("Not A Root Element")
    -- end
    if path == nil or path == "" then
        return self
    end
    local _path = string.format("%s%s%s", self.key,path_sep, path)
    local e_type
    -- logging.log(_path)
    if self._db.txn:get(self._db.dbi, _path) then
        return element:new{_db = self._db,
                           key = _path,
                           e_type = self.e_type,
                           _root = self._root}
    else
        while true do
            local _relative_path = string.match(_path, "/[^/]*/(.*)")
            local parent, name = xpath_split(_relative_path)
            if _relative_path == "" then
                error(string.format("`%s` -> `%s` not found!", self.key, path))
            end

            return self._root:_xpath(parent):get_child(name)
        end
    end

end
element.xpath = element._xpath

function element:_xpath_attr_selector(elist, attr_key, attr_value)

    local rt = {}
    if attr_value then
        for k,v in pairs(elist) do
            if v[attr_key] == attr_value then
                rt[#rt+1] = v
            end
        end
    else
        for k,v in pairs(elist) do
            if v[attr_key] ~= nil then
                rt[#rt+1] = v
            end
        end
    end
end
function element:_xpath_any_selector(  )
    return self:get_child()
end
function element:_xpath_all_selector(  )

end
function element:xpath( path )
    -- is root /path/to/*@xxx=yy
end
function element:_get_dup(op)
    local cur = self._db.txn:cursor_open(self._db.dbi)

    local flag = lightningmdb.MDB_SET_KEY
    -- assert(self._db.txn:get(self._db.dbi,self.key), self.key)
    local k,v = cur:get(self.key, flag)

    
    local dict = {}
    if k == nil or v == nil then
        return dict
    end
    flag = lightningmdb.MDB_NEXT_DUP
    repeat
    
        op(dict, k, v)
    
        k,v = cur:get(self.key, flag)
    until k == nil
    cur:close()
    return dict
end
function element:get_parent()
    if self.is_root then
        error("This is Root ")
    end
    if self.real_key then
        --print("Real", self.real_key)
        local parent, this = xpath_split(self.real_key)
        if self._db.txn:get(self._db.dbi, parent) then
            -- print(xpath_split(self.key))
            -- print("eeeeeeeeee")
            print("Return")
            return element:new{_db = self._db, 
                               key = parent,
                               e_type = "regular",
                               _root = self._root}
        else
            -- TODO: 判断新的real_key和key是否有交集，如果没有，则应该是该引用节点已经被删除
            return element:new{_db = self._db, 
                               key = xpath_split(self.key),
                               e_type = "ref",
                               real_key = parent,
                               _root = self._root}
        end

    else
        local parent, _ = xpath_split(self.key)
        return element:new{_db = self._db,
                           key = parent,
                           e_type = "regular",
                           _root = self._root}
    end
end
function element:_remove_parent_refer()
    local parent = self:get_parent()

    if parent.e_type == "regular" then
        local _, this = xpath_split(self.key)
        local map = {ref="s:", regular="c:"}
        local this_key = string.format("%s%s`%s", map[self.e_type], this, self.key)
        print(parent.key, this_key)
        parent._db.txn:del(parent._db.dbi, parent.key, this_key)
    else
        error("symbolink readonly")
    end
end
function element:_remove()
    if self.e_type == "regular" then
        -- remove all childs
        for k,v in pairs(self:get_child()) do
            v:_remove()
        end

        -- remove refers
        for i, v in ipairs(self:get_refers()) do
            v:_remove()
        end
        -- remove all 
        self._db.txn:del(self._db.dbi, self.key, nil)
    else
        self._db.txn:del(self._db.dbi, self.key, string.format("r:%s", self.real_key))
    end
    if string.contains(self.key, "/") then
        self:_remove_parent_refer()
    end
end


function element:remove(_xpath)
    self:_xpath(_xpath):_remove()
end
function element:get_attrib()

    return self:_get_dup(function (dict, k, v)
                             if k and v  and k == self.key and string.sub(v, 1, 2) == "a:" then
                                 local ct = string.sub(v, 3)
                                 local _, ii = string.find(ct, "`")
                                 if not ii then
                                     dict[ct] = ""
                                 else
                                     dict[string.sub(ct, 1, ii-1)] = string.sub(ct, ii+1)
                                 end
                             end
                         end)

end
function element:get_refers()
    -- get elements who index this element
    return self:_get_dup(function (dict, k, v)
                             if k and v  and k == self.key and string.sub(v, 1, 2) == "r:" then
                                 local ct = string.sub(v, 3)
                                 print("Ref Key", ct)
                                 dict[#dict+1] = element:new{_db = self._db,
                                                             key = self.key,
                                                             real_key = ct,
                                                             e_type = "ref",
                                                             _root = self._root}
                             end
                         end)
end


function element:get_child(name)
    if name ~= nil then
        local key = string.format("%s%s%s", self.key, path_sep,  name)
        -- local v = assert(self._db.txn:get(self._db.dbi, key), key)
        local els = self:get_child(nil)
        return els[name]
    else
        return self:_get_dup(function (dict, k, v)
                                 local meta = string.sub(v, 1, 2)
                                 assert(#k < 1000,"Key is too long,there may be something wrong")
                                 if k and v  and k == self.key and (meta == "c:" or meta == "s:") then -- `s:` means 'symbolink to'
                                     local name, key, real_key
                                     if meta == "s:" then
                                         name, key = unpack(string.split(string.sub(v,3), "`"))
                                     else
                                         name = string.sub(v, 3)
                                         key = string.format("%s%s%s", self.key, path_sep, name)
                                     end
                                     local e_type
                                     if self.e_type == "ref" or meta == "s:" then
                                         e_type = "ref"    -- ref
                                         real_key = string.format("%s%s%s", self.key, path_sep, name)
                                     else
                                         e_type = "regular" -- regular 
                                     end
                                     dict[name] = element:new{_db = self._db,
                                                              key = key,
                                                              e_type = e_type,
                                                              real_key = real_key,
                                                              _root = self._root}
                                                            
                                 end
                             end)
    end
end

function element:get_value()
    return self:_get_dup(function (dict, k, v)
                             if k and v  and k == self.key and string.sub(v, 1, 2) == "v:" then
                                 local value = string.sub(v, 3)
                                 dict[#dict+1] = value
                             end 
                         end)
end

function element:add_attrib(k, v)
    local attr = string.format("a:%s`%s", k, v)
    print("Attr",self.key, attr)

    self._db.txn:put(self._db.dbi, self.key, attr,lightningmdb.MDB_NODUPDATA)
end

function element:add_value(v)
    local value = string.format("v:%s", v)
    self._db.txn:put(self._db.dbi, self.key, value,lightningmdb.MDB_NODUPDATA)
end

function element:add_pair(k, v)
    self:add_node(k):add_value(v)
end

function element:add_node(k)

    local ch = string.format("c:%s", k)
    --pprint.pprint(self._db)
    print("AddNode",self.key, ch)
    --if self:get_child(k) == nil then
        self._db.txn:put(self._db.dbi, self.key, ch,lightningmdb.MDB_NODUPDATA)
    --end

    local key = string.format("%s%s%s", self.key, path_sep,  k)
    print("Child",self.key, key)

    o = element:new{_db = self._db, key = key, e_type = "regular", _root = self._root}
--    o:_raw_put("t:node")
    return o
end
function element:_raw_put(v)
    print(debug.traceback())
    self._db.txn:put(self._db.dbi, self.key, v,lightningmdb.MDB_NODUPDATA)
end
function element:_add_ref(k, link)
    local key = string.format("%s%s%s", self.key, path_sep,  k)
    link:_raw_put(string.format("r:%s", key))
    self:_raw_put(string.format("s:%s`%s", k, link.key))
    return element:new{_db = self._db,
                       key = link.key,
                       real_key = key,
                       e_type = "ref",
                       _root = self._root}
end

function element:add_ref( link )
    local _, name = xpath_split(link.key)
    return self:_add_ref(name, link)
end

function element:show(indent)
    if indent == nil then
        indent = ""
    end
    print(indent .. "Node:", self.key)
    print(indent .. "    " .. "Attrib:")
    for k,v in pairs(self:get_attrib()) do
        print(indent .. "        " .. k .. ":" .. v)
    end
    local values = self:get_value()
    if #values ~= 0 then
        print(indent .. "    " .. "Values:")
        for k,v in pairs(values) do
            print(indent .. "        " .. v)
        end
        return
    end
    --print(self:get_child())
    for _, v in pairs(self:get_child()) do
        v:show(indent .. "    ")
    end
end




function element:to_xml(str)
    -- not that pretty  but it does work well ...
    if str == nil then
        str = ""
    end
    local tmp = split(self.real_key or self.key, "/")
    local root = tmp[#tmp]
    local values = self:get_value()
    local attrs = self:get_attrib()
    if #values == 0 then
        str = str .. string.format("<%s ", root)
        for k,v in pairs(attrs) do
            str = str .. string.format("%s=\"%s\" ", k, v)
        end
        str = str .. ">"


        local childs = self:get_child()
        for k,v in pairs(childs) do
            str = v:to_xml(str)
        end
        str = str .. string.format("</%s>", root)
        
    else

        for _,v in pairs(values) do
            str = str .. string.format("<%s>%s</%s>", root, v, root)
        end

    end
    return str
end

-- tests
if ... == "__main__" then
    local path = "./temp/bar"
    os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))
    local root1 = "Domain"
    local db = mdb:new(path)
    function test(db)
        
        local e = db:get_root(root1)
        pprint.pprint(e, "Root")
        local e1 = e:add_node("Bank")
        local e2 = e1:add_node("Branch")
        e2:add_attrib("Serial", "1")
        local host = e2:add_node("Host1")
        host:add_attrib("IP", "192.168.0.1")
        local e3 = e1:add_node("Branch2")
        e3:add_attrib("Serial", "2")
        e3:add_attrib("Alias", "fenhang1")
        e3:add_pair("Head", "Mr.Wang")
        e3:add_pair("Head", "Mrs.Wang")
        print("Not Commit")
        pprint.pprint(db, "ddd")
        db:commit()
        print("Commit!")
        pprint.pprint(db.env:stat(db.dbi))
        pprint.pprint(db.env:info(db.dbi))
        -- db:close()
        -- db = mdb:new(path)
        -- e = db:get_root("Domain")
     end


    function test1(db)
        db:get_root(root1):show()
    end
    function test_xpath(db)
        local e = db:get_root(root1)
        local branch = e:_xpath("Bank/Branch")
        print("Show Branch", branch)
        branch:show()

        print("Show Host")

        local host = branch:_xpath("Host1")
        host:show()
        
    end
    function test_value(db)
        local e = db:get_root(root1)
        -- local db = mdb:new(path)
        -- local e = db:get_root("Domain")
        print("TestValue")
        local branch = e:_xpath("Bank/Branch2/Head")
        local values = branch:get_value()
        pprint.pprint(values)
        pprint.pprint(branch:get_value())


    end
    function test_xml(db)
        
        local el = db:get_root(root1)

        pprint.pprint(el:to_xml())
        el:remove("Bank/Branch2")
        pprint.pprint(el:to_xml())
    end
    local another = "Define"
    function test_symlink(db)
        local r = db:get_root(another)
        local domain_root = db:get_root(root1)
        local part = domain_root:_xpath("Bank/Branch2")
        print("Node from Domain:")
        part:show()
        print("Add to Define:")
        r:add_ref(part)
        print("Now Define:")
        r:show()
        print("symlink xml!")
        pprint.pprint(r:to_xml())
        
        db:commit()
    end
    function test_more_sym(db)
        local another = "Define"
        local r = db:get_root(another)
        local domain_root = db:get_root(root1)
        local part = domain_root:_xpath("Bank/Branch2")
        print("delete ref:")
        r:remove("Branch2")
        print("Symbolink to:")
        r:show()
        print("Original:")
        domain_root:show()
        print ""
        print("Delete Orig:")
        part:remove()
        print("Orig:")
        domain_root:show()
        print("Symlink to:")
        r:show()
    end
    function test_more_xpath(db)
        local another = "Define"
        local r = db:get_root(another)
        local e = r:_xpath("Branch2/Head")
        print "More on xpath:"
        e:show()
    end
    function test_walk( db )
        local r = db:get_root(root1)
        local iter = 
        function (e)
            if e:is_leaf() then
                print(e:is_leaf())
                pprint.pprint(e:get_value(), e.key)
            end
        end
        r:walk(iter)
    end
    function test5( db )
        local r = db:get_root("device")
        r:add_node("dev_type12"):add_node("3desp213")
        r:add_node("dev_type1122"):add_node("3desp213")
        db:_implement_storage()
        r:show()
        db:commit()
    end
    db:with(test)

    db:with(test1)
    db:withReadOnly(test_xpath)
    db:withReadOnly(test_value)
    db:with(test_xml)
    db:with(test_symlink)
    db:with(test_more_sym)
    db:with(test_more_xpath)
    db:withReadOnly(test_walk)
    db:with(test5)
end
