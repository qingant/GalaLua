--[[
Athor: Ma Tao
	
	Simple NXD(Native XML Database) for Gala

]]

module(...,package.seeall)

local lmdb = require "lightningmdb"
local pprint = require "pprint"

--[[
	Node:
	a:key`value  attribute key -> value
	c:key        children  key -> Node
	v:value      list item  [value ...]
]]

mdb = {}
local element = {}
local path_sep = "/"
local indent_one = "    "
function mdb:new(path)
	local e = lightningmdb.env_create()
	e:open(path,0,420)
	local t = e:txn_begin(nil,0)
	local db = t:dbi_open(nil,lightningmdb.MDB_DUPSORT)
	local o = {env = e, txn = t, dbi = db}
	setmetatable(o, self)
	self.__index = self
	return o
end



function mdb:get_root(name)
	return element:new(self, path_sep .. name)
end

function mdb:clear()
	
end
function mdb:commit()
	self.txn:commit()
	self.txn = self.env:txn_begin(nil, 0)
end

function mdb:abort()
	self.txn:abort()
	self.txn = self.env:txn_begin(nil, 0)
end

function element:new(_db, key)
	-- local key = "/" .. name
	local o = {_db = _db, key = key, is_root = true}
	setmetatable(o, self)
	self.__index = self
	return o
end
function element:xpath(path)
	-- if not self.is_root then
	-- 	error("Not A Root Element")
	-- end
	local _path = string.format("%s%s%s", self.key,path_sep, path)
	assert(self._db.txn:get(self._db.dbi, _path), _path)
	return element:new(self._db, _path)
end

function element:_get_dup(op)
	local cur = self._db.txn:cursor_open(self._db.dbi)
	local flag = lightningmdb.MDB_SET
	local k,v = cur:get(self.key, flag)
	flag = lightningmdb.MDB_NEXT_DUP
	local dict = {}
	repeat
		op(dict, k, v)
		k,v = cur:get(self.key, flag)
	until k == nil
	cur:close()
	return dict
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

function element:get_child(name)
	if name ~= nil then
		local key = string.format("%s%s%s", self.key, path_sep,  name)
		local v = assert(self._db.txn:get(self._db.dbi, key))
		local els = self:get_child(nil)
		return els[name]
	else
		return self:_get_dup(function (dict, k, v)
								 if k and v  and k == self.key and string.sub(v, 1, 2) == "c:" then
									 local name = string.sub(v, 3)
									 local key = string.format("%s%s%s", self.key, path_sep, name)
									 dict[name] = element:new(self._db, key)
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
	self._db.txn:put(self._db.dbi, self.key, ch,lightningmdb.MDB_NODUPDATA)

	local key = string.format("%s%s%s", self.key, path_sep,  k)
	print("Child",self.key, key)
	
	return element:new(self._db, key)
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


function element:to_xml(str)
	-- not that pretty err but it do works well ...
	if str == nil then
		str = ""
	end
	local tmp = split(self.key, "/")
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
	local db = mdb:new(path)
	local e = db:get_root("Domain")
	function test()
		os.execute(string.format("rm -rf %s && mkdir -p %s", path, path))

		print("Root", e)
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
		db:commit()

		
	end


	function test1()
		e:show()
	end
	function test_xpath()
		local branch = e:xpath("Bank/Branch")
		print("Show Branch")
		branch:show()

		print("Show Host")

		local host = branch:xpath("Host1")
		host:show()
		
	end
	function test_value( )
		-- local db = mdb:new(path)
		-- local e = db:get_root("Domain")
		print("TestValue")
		local branch = e:xpath("Bank/Branch2/Head")
		local values = branch:get_value()
		pprint.pprint(values)
		pprint.pprint(branch:get_value())


	end
	function test_xml(el)
		pprint.pprint(el:to_xml())
	end

	test()
	test1()
	test_xpath()
	test_value()
	test_xml(e)

end
