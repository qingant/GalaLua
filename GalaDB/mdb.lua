--[[
Athor: Ma Tao

    Simple NXD(Native XML Database) for Gala

]]

module(...,package.seeall)

local lmdb = require "lightningmdb"
local pprint = require "pprint"
assert(require("str_utils"), "Cannot Import str_utils")
assert(require("tab_utils"),"Cannot Import tab_utils")
local logging = require("logging")

local osxpcall = xpcall
local function _xpcall(action,err,...)
   local args = unpack{...}
   return osxpcall(function() return action(args) end,err)
end

xpcall = _xpcall

--[[
    Node:
    a:key`value  attribute key -> value
    c:key        children  path/key -> Node
    v:value      list item  [value ...]
    l:key        vector type node
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
local num_pages = 1024
local vector_index_key = "__VectorIndexGenerator"
local vector_item_tag = "__VectorItemTag"
local to_table_attrib_key = "@attr"

function mdb.create_env(path)
    local path_id="@"..path
    local e,err_msg=glr.get_global(path_id)
    if not e then
        e=mdb._create_env(path)
        glr.global(path_id,e)
    end
    return e
end

function mdb._create_env(path)

    local e=lightningmdb.env_create()
    e:set_mapsize(num_pages*4096)
    assert(e:open(path,0,420))
    return e
end

function mdb.reader_free(path,e)
    if type(path)=="string" then
        local e=e or mdb.create_env(path)
        e:reader_check()
    end
end
function mdb.lock_clear(path,e)
    if type(path)=="string" then
        local e=e or mdb.create_env(path)
        local t = e:txn_begin(nil,0)
        t:abort()
    end
end

function mdb:new()
--    local e = lightningmdb.env_create()
--    e:set_mapsize(num_pages*4096)
--    e:open(path,0,420)
    -- local t = e:txn_begin(nil,0)
    -- local db = t:dbi_open(nil,lightningmdb.MDB_DUPSORT)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function mdb:init(e)
    assert(e,"must pass env")
    self.env = e
    return self
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
    local flag = lightningmdb.MDB_NEXT_DUP
    repeat
        local k,v = cur:get(self.key, flag)
        print("IMPL",k, "->",v)
    until k == nil
    cur:close()
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
    --print(...)
    local err, value = xpcall(action, debug.traceback, self, ...)
    if self.txn ~= nil then
        self:abort()
    end
    if not err then
        error(value)
    end
    return value
end
function mdb:with(action, ...)
    if self.txn == nil then
        self:beginWRTrans()
    end
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
                           real_key = self.real_key ~= nil and string.format("%s%s%s", self.real_key,path_sep, path) or nil,
                           _root = self._root}
    else
        while true do
            local _relative_path = string.match(_path, "/[^/]*/(.*)")
            local parent, name = xpath_split(_relative_path)

            if _relative_path == "" then
                error(string.format("`%s` -> `%s` not found!", self.key, path))
            end
            local p = self._root:_xpath(parent)
            if p then
                return p:get_child(name)
            else
                return nil
                -- return self._root:_xpath(parent):get_child(name)
            end
        end
    end

end
--element.xpath = element._xpath

function element:_xpath_attr_selector(elist, attr_key, attr_value)

    local rt = {}
    if attr_value then
        for k,v in pairs(elist) do

            if v:get_attrib()[attr_key] == attr_value then
                rt[#rt+1] = v
            end
        end
    else
        for k,v in pairs(elist) do

            if v:get_attrib()[attr_key] ~= nil then
                rt[#rt+1] = v
            end
        end
    end
    return rt
end

function element:_xpath_any_selector(  )
    return self:get_child()
end
function element:_xpath_all_selector(  )
    local all = {}
    self:walk(function ( e )
              if e.key ~= self.key then

                  all[#all + 1] = e
              end
    end)
    return all
end
function element:_xpath_collect( all, tokens, idx )
    if #tokens < idx then
        return all
    end

    local result = {}
    for k,v in pairs(all) do
        if v then
            --pprint.pprint(v)

            local r = v:_xpath_selector(tokens, idx)
            if r then
                for i,v in pairs(r) do
                  result[#result + 1] = v
                end
            end
        end
    end
    return result
end
function element:_xpath_selector( tokens, idx )
    assert(self, "`self` is nil")
    local tok = tokens[idx]

    local sel
    if string.sub(tok, 1, 2) == "**" then
        -- assert(idx==#tokens, "** selector can only used at last")
        local all = self:_xpath_all_selector()
        if string.sub(tok,3,3) == "@" then
            local k,v = unpack(string.split(string.sub(tok,4),"="))

            sel =  self:_xpath_attr_selector(all,k,v)

        else
            sel = all
        end
    elseif string.sub(tok,1,1) == "*" then
        local all = self:get_child()
        if string.sub(tok,2,2) == "@" then
            sel = all
            for i,v in ipairs(string.split(string.sub(tok,3), "&")) do
                local k,v = string.split(string.sub(v,3),"=")
                sel = self:_xpath_attr_selector(sel,k,v)
            end
        else

            sel = all
        end
    else
        -- optimize for plain _xpath selector
        local _xp = tokens[idx]
        while idx < #tokens  do
            local tok =  tokens[idx+1]
            if string.sub(tok,1,1) ~= "*" then
                _xp = _xp .. "/" .. tok
                idx = idx+1
            else
                break
            end
        end
        sel = {self:_xpath(_xp)}

    end

    if idx < #tokens then
        return self:_xpath_collect(sel, tokens, idx+1)
    else
        return sel
    end
end
function element:xpath( path )
    -- is root /path/to/*@xxx=yy
    local o = self:_xpath_selector(string.split(path, "/"),1)
    return o
end


function table_slice (values,i1,i2)
    local res = {}
    local n = #values
    -- default values for range
    i1 = i1 or 1
    i2 = i2 or n
    if i2 < 0 then
        i2 = n + i2 + 1
    elseif i2 > n then
        i2 = n
    end
    if i1 < 1 or i1 > n then
        return {}
    end
    local k = 1
    for i = i1,i2 do
        res[k] = values[i]
        k = k + 1
    end
    return res
end
-- TODO: use matched selector instead
function element:xpath_match( path )

    local tokens = string.split(path, "/")
    local self_tokens = string.split(self.real_key or self.key, "/")

    local cursor = 1
    for i,v in ipairs(tokens) do

        if cursor == #self_tokens then
            return false
        end
        cursor = cursor + 1
        if v:sub(1,2) == "**" then
            cursor = #self_tokens
            if v:sub(3,3) == "@" then
                for i1,v1 in ipairs(string.split(v:sub(4, #v), "&")) do
                    local k2, v2 = unpack(string.split(v1, "="))

                    if self:get_attrib(k2) then
                        if v2 then
                            if self:get_attrib(k2) ~= v2 then
                                return false
                            end
                        end
                    else
                        return false
                    end
                end
            end
            return true
        elseif v:sub(1,1) == "*" then
            if v:sub(2,2) == "@" then
                local tags = table_slice(self_tokens, 2, cursor)
                local c_xpath = table.concat(tags, "/") -- current xpath
                -- print("Current Path:", c_xpath)         -- 
                local element = self._root:_xpath(c_xpath)
                for i1,v1 in ipairs(string.split(v:sub(3, #v), "&")) do
                    local k2, v2 = unpack(string.split(v1, "="))
                    if element:get_attrib(k2) then
                        if v2 then
                            if element:get_attrib(k2) ~= v2 then
                                return false
                            end
                        end
                    else
                        return false
                    end
                end
            end
            return true
        else
            if v ~= self_tokens[cursor] then
                return false
            end
        end
    end
    return true
end

function element:xpath_set( xpath, value )

    local o = self:xpath(xpath, 1)
    local num = 0
    for i,v in ipairs(o) do
        if o:is_leaf() then
            num = num +1
            o:add_value(key, value)
        end
    end
    return num
end
function element:_get_dup(op)
    local cur = self._db.txn:cursor_open(self._db.dbi)

    local flag = lightningmdb.MDB_SET_KEY
    -- assert(self._db.txn:get(self._db.dbi,self.key), self.key)
    assert(type(self.key) == "string", "Key Cannot be of type other than string")
    local k,v = cur:get(self.key, flag)


    local dict = {}
    if k == nil or v == nil then
        cur:close()
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
        local this_key
        if self.e_type == "ref" then
            this_key = string.format("s:%s`%s" , this, self.key)
        else
            this_key = string.format("c:%s", this)
        end

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
    if not self.is_root then
        self:_remove_parent_refer()
    end
end


function element:remove(_xpath)
    self:_xpath(_xpath):_remove()
end
function element:remove_value(v)
    if v then
        assert(self.e_type == "regular", "Cannot Remove Value from ref element")
        local key = string.format("v:%s", v)
        self._db.txn:del(self._db.dbi, self.key, key)
    else -- remove all
        for i,v in ipairs(self:get_value()) do
            self:remove_value(v)
        end
    end
end


function element:remove_attrib( k )
    local v = self:get_attrib(k)
    if v then
        self._db.txn:del(self._db.dbi, self.key, string.format("a:%s`%s", k, v))
    end
end
function element:get_attrib(k)
    if k then
        return self:get_attrib()[k]
    end

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
        -- pprint.pprint(els)
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
                                         real_key = string.format("%s%s%s", self.real_key or self.key, path_sep, name)
                                     else
                                         e_type = "regular" -- regular
                                     end
                                     --print("Get", name)
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

    self._db.txn:put(self._db.dbi, self.key, attr,lightningmdb.MDB_NODUPDATA)
end

function element:set_attrib( k, v )
    self:remove_attrib(k)
    self:add_attrib(k, v)
end
function element:add_value(v)
    local value = string.format("v:%s", v)
    self._db.txn:put(self._db.dbi, self.key, value,lightningmdb.MDB_NODUPDATA)
end
-- function element:set_value(v)
--    self:remove_value(v)
--    self:add_value(v)
-- end
function element:add_pair(k, v)
    self:add_node(k):add_value(v)
end
function element:_raw_get( k )
    return self._db.txn:get(self._db.dbi, k)
end
function element:add_vector_node(k, itag)

    local o = self:add_node(k)
    o:add_attrib(vector_index_key, "1")
    if itag then
        o:add_attrib(vector_item_tag, itag)
    end
    o._is_vector = true
--    o:_raw_put("t:node")
    return o
end

function element:is_vector( ... )

    if self._is_vector == nil then
        if self:get_attrib()[vector_index_key] ~= nil then
            self._is_vector = true
        else
            self._is_vector = false
        end
    end
    return self._is_vector
end
function element:add_vector_item()
    assert(self:is_vector(), "Element is not Vector Node")
    local serial = self:get_attrib()[vector_index_key]
    self:set_attrib(vector_index_key, tostring(tonumber(serial) + 1))
    return self:add_node(serial)
end
function element:add_node(k)
    assert(k, "Key Cannot be nil")
    -- assert(not self.is_leaf())
    local ch = string.format("c:%s", k)
    --pprint.pprint(self._db)

    --if self:get_child(k) == nil then
    self._db.txn:put(self._db.dbi, self.key, ch,lightningmdb.MDB_NODUPDATA)
    --end

    local key = string.format("%s%s%s", self.key, path_sep,  k)


    o = element:new{_db = self._db, key = key, e_type = "regular", _root = self._root}
--    o:_raw_put("t:node")
    return o
end
function element:_raw_put(v)
    --print(debug.traceback())
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

function element:create_xpath(path)
  local pathlist = path:split("/")
  local cnode = self
  for _, item in pairs(pathlist) do
      node = cnode:_xpath(item)
      if node == nil then
          cnode = cnode:add_node(item)
      else
          cnode = node
      end
  end
  return cnode
end

function element:add_table(t)
    if table.isArray(t) then
        assert(self:is_vector(), "Cannot Add List to NON-Vector Node")
        for i,v in ipairs(t) do
            local item = self:add_vector_item()
            if type(v) ~= "table" then
                item:add_value(tostring(v))
            else
                item:add_table(v)
            end
        end
    else
        for k,v in pairs(t) do
            assert(not self:is_vector(), "Cannot Add K-V to Vector Node")
            local item = self:add_node(k)
            if type(v) ~= "table" then
                item:add_value(tostring(v))
            else
                item:add_table(v)
            end
        end
    end
    return self
end

function element:to_table(attr_flag)
    -- if attr_flag true return table include attribute
    local result = {}
	  -- assert((args == nil) or (type(args) == "table"), "args must be table or nil")
    if attr_flag == nil then
        attr_flag = true
    end

    if self:is_vector() then

        for k,v in pairs(self:get_child()) do
            result[#result + 1] = v:to_table(attr_flag)
        end
		-- result[to_table_attrib_key] = {}
		-- local attrib = result[to_table_attrib_key]
		-- for k,v in pairs(self:get_attrib()) do
		-- 	attrib[k] = v
		-- end
    elseif self:is_leaf() then
        local vs = self:get_value()
        if #vs == 1 then
            result = vs[1]
        else
            result = vs
        end
    else
        for k,v in pairs(self:get_child()) do
            result[k] = v:to_table(attr_flag)
        end
        if attr_flag then
        		result[to_table_attrib_key] = {}
        		local attrib = result[to_table_attrib_key]
        		for k,v in pairs(self:get_attrib()) do
        			attrib[k] = v
        		end
        end
    end
    return result
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
function element:tag(  )
    if self._tag == nil then
        local ts = string.split(self.real_key or self.key, "/")
        self._tag = ts[#ts]
    end
    return self._tag
end

function merge_to_xml( els ) -- static method

    local path = {}
    for i,v in pairs(els) do
        print(i,v)
        local key = v.real_key or v.key

        local cache = path
        local tokens = string.split(key, "/")
        for i,t in ipairs(tokens) do
            if i == #tokens then
                cache[t] = v
            else
                cache[t] = path[t] or {}
                cache = cache[t]
            end
        end
    end


    function _to_xml( t, str )
        local str = str or ""

        for k,v in pairs(t) do
            if v.key then
                str = str .. v:to_xml()
            else
                str = str .. string.format("<%s>", k)
                str = _to_xml(v, str)
                str = str .. string.format("</%s>", k)
            end
        end

        return str
    end
    return _to_xml(path)
end

function element:to_xml(str)
    -- not that pretty  but it does work well ...
    if str == nil then
        str = ""
    end
    --local tmp = split(self.real_key or self.key, "/")
    local root = self:tag()
    local values = self:get_value()
    local attrs = self:get_attrib()
    if #values == 0 then
        str = str .. string.format("<%s", root)
        for k,v in pairs(attrs) do
            if string.sub(k, 1, 2) ~= "__" then  -- hiden attribute
                str = str .. string.format(" %s=\"%s\"", k, v)
            end
        end
        if self._count then
          str = str .. string.format(" %s=\"%s\"", "__count", self._count)
        end
        str = str .. ">"
        local childs = self:get_child()
        if self:is_vector() then
            local item_key = self:get_attrib()[vector_item_tag]
            for k,v in pairs(childs) do
                v._tag = item_key
                v._count = k
                str = v:to_xml(str)
            end
        else

            for k,v in pairs(childs) do
                str = v:to_xml(str)
            end
        end
        str = str .. string.format("</%s>", root)

    else

        for _,v in pairs(values) do
            str = str .. string.format("<%s>%s</%s>", root, v, root)
        end

    end
    return str
end

function element:exist( nodename )
    local node = self:_xpath(nodename)
    -- pprint.pprint(node, 'node')
    if node ~= nil then
        return true
    else
        return false
    end
end

-- tests
if ... == "__main__" then
    local path = "./temp/bar"
    os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))
    local root1 = "Domain"
    local test={}
    local db = mdb:new():init(mdb.create_env(path))


    -- for i=1,5000000 do
    --     test[#test+1]= mdb:new():init(mdb.create_env(path))
    -- end
    -- local ID="AAA"
    -- local e=mdb.create_env(path)
    -- glr.global(ID,e)

    -- local ee=glr.get_global(ID)
    -- pprint.pprint(getmetatable(ee))


    print("DONE")

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
        print(e.key)
        assert(e.real_key == "/Define/Branch2/Head", e.real_key)
        e:show()
    end
    function test_more_more_xpath( db )
        print("Test More More Xpath With Sym")
        local another = "Define"
        local r = db:get_root(another)
        pprint.pprint(r:xpath("Branch2/*"))
        pprint.pprint(r:xpath("**"))
        print("Test More More Xpath orig")
        local domain_root = db:get_root(root1)
        print("Domain Root All")
        pprint.pprint(domain_root:xpath("**"))
        print("/Domain/Bank/**")
        pprint.pprint(domain_root:xpath("Bank/**"))
        print("/Domain/Bank/**@IP")
        pprint.pprint(domain_root:xpath("Bank/**@IP"))
        print("/Domain/Bank/**@Serial=1")
        pprint.pprint(domain_root:xpath("Bank/**@Serial=1"))

        print("/Domain/Bank/*/**@IP")

        pprint.pprint(domain_root:xpath("Bank/*/**@IP"))



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
    function test_remove_root_child( db )
        local domain_root = db:get_root(root1)
        domain_root:add_node("Bank1"):add_node("Test"):add_value("ok")
        print(domain_root:to_xml())
        domain_root:_xpath("Bank"):remove()

        print(domain_root:to_xml())
    end
    function test_exist( db )
        local e = db:get_root(root1)
        local branch = e:_xpath("Bank/Branch")
        print(e:exist("Host1"))
        print(e:exist("Host2"))
    end
    function test_vector( db )
        local e = db:get_root(root1)
        local v = e:add_vector_node("TestVector", "Item")
        local i1 = v:add_vector_item()
        local i2 = v:add_vector_item()
        i1:add_value("good")
        i2:add_value("bad")
        print(e:to_xml())

    end
    function test_table( db, t )
        local e = db:get_root(root1)
        local tt = e:add_node("TestTable")

        tt:add_table(t)
        pprint.pprint(t, "Table")
        pprint.pprint(tt:to_table(), "Reversed")

		pprint.pprint(db:get_root("Domain"):to_table(), "Domain")


    end
    function test_merge( db )
        local domain_root = db:get_root(root1)
        print("merge_to_xml")
        print(merge_to_xml(domain_root:xpath("Bank/**")))
    end

    function test_xquery_xpath(db)
        local e = db:get_root(root1)
        local ref_node = e:add_node("sys")
        local node1 = ref_node:add_node("mem")
        node11 = node1:add_node("event")
        node11:add_pair("report","111")
        node11:add_pair("request","222")
        local node2 = e:add_node("cpu")
        node21 = node2:add_node("event")
        node21:add_pair("report","333")
        node21:add_pair("request","444")

        ref_node = e:add_node("afa")
        node3 = ref_node:add_node("oper")
        node11 = node3:add_node("event")
        node11:add_pair("report","111")
        node11:add_pair("request","222")
        node4 = e:add_node("status")
        node21 = node4:add_node("event")
        node21:add_pair("report","333")
        node21:add_pair("request","444")

        local head = e:add_node("headbank")
        local host = head:add_node("host1")
        host:_add_ref("mem", node1)
        host:_add_ref("cpu", node2)
        local host = head:add_node("host2")
        host:_add_ref("oper", node3)
        host:_add_ref("status", node4)
        nodes = e:xpath("**/report")
        pprint.pprint(e:xpath("**"), "All Nodes")
        pprint.pprint(nodes,'Nodes')
        db:commit()
    end

    function test_create_xpath(db)
        local domain_root = db:get_root(root1)
        local node = domain_root:create_xpath("test/test1")
        node:add_table({["test"] = "test"})
        pprint.pprint(node:to_table(), "node")
        db:commit()
    end
    function test_xpath_match( db )
        local domain_root =  db:get_root(root1)
        local node = domain_root:create_xpath("Test/Test1/Ok")
        node:add_attrib("abc","def")
        local node1 = node:add_node("NoOk")
        local node2 = node:add_node("WangBaDan")

        local ref = node2:add_ref(node1)
        assert(node:xpath_match("Test/Test1/*"), "Easy One Star Match Failed")
        print("Example 1 Succ")
        assert(node:xpath_match("**"), "Easy Two Star Match Failed")
        print("Example 2 Succ")
        assert(node:xpath_match("**@abc"), "Easy Two Star with attr Match Failed")
        print("Example 3 Succ")
        assert(node:xpath_match("**@abc=def"), "Easy Two Star with attr equals Match Failed")
        print("Example 4 Succ")
        assert(node:xpath_match("**@abc=def1") == false, "Easy Two Star with attr equals Match Failed")
        print("Example 5 Succ")
        assert(node1:xpath_match("Test/Test1/*@abc/NoOk"))
        assert(node1:xpath_match("Test/Test1/*@abc=def/NoOk"))
        assert(node1:xpath_match("Test/Test1/*@abc=def1/NoOk")==false)
        assert(node1:xpath_match("Test/Test1/*@abc=def1")==false)
        print("Example OneStarWithCondition Succ")

        assert(ref:xpath_match("Test/Test1/Ok/WangBaDan/NoOk"))
        assert(ref:xpath_match("Test/Test1/*@abc/WangBaDan/NoOk"))
        assert(ref:xpath_match("Test/Test1/*@abc=def/WangBaDan/NoOk"))
        assert(ref:xpath_match("Test/Test1/*@abcd/WangBaDan/NoOk") == false)
        assert(ref:xpath_match("Test/Test1/*@abc=def1/WangBaDan/NoOk") == false)

        ref:add_attrib("aa", "bb")
        assert(ref:xpath_match("**@aa"))
        assert(ref:xpath_match("**@aa=bb"))
        assert(ref:xpath_match("**@aaaa") == false)
        assert(ref:xpath_match("**@aa=bbb") == false)
        assert(ref:xpath_match("**@aaaa=bb") == false)
        print("Example Ref Succ")

    end

    local limit = 1
    for i=1,limit do
        db:with(test)

        -- db:with(test1)
        -- db:with(test_xquery_xpath)
        -- db:withReadOnly(test_xpath)
        -- db:withReadOnly(test_value)
        -- db:with(test_xml)
        -- db:with(test_symlink)
        -- db:with(test_more_sym)
        -- db:with(test_more_xpath)
        -- db:withReadOnly(test_more_more_xpath)
        -- db:withReadOnly(test_walk)
        -- db:with(test5)
        -- db:with(test_remove_root_child)
        -- collectgarbage("collect")

        -- db:withReadOnly(test_exist)
        -- db:with(test_vector)
        -- db:with(test_table, {abc="ok",efg={def="laf",test="dddd"}})
        -- db:withReadOnly(test_merge)
        -- db:withReadOnly(test_xquery_xpath)
        -- db:with(test_create_xpath)
        db:with(test_xpath_match)
    end

end
