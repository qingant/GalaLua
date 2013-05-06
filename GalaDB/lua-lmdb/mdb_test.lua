local lightningmdb_lib=require "lightningmdb"
local lightningmdb = _VERSION=="Lua 5.2" and lightningmdb_lib or lightningmdb

local function pt(t)
  for k,v in pairs(t) do
    print(k,v)
  end
end

local function ps(e)
  print("--- env stat")
  pt(e:stat())
  print("---")
end

local function cursor_pairs(cursor_,key_,op_)
  return coroutine.wrap(
    function()
      local k = key_,v
      repeat
        k,v = cursor_:get(k,op_ or cursor_.MDB_NEXT)
        if k then
          coroutine.yield(k,v)
        end
      until not k
    end)
end

local function mtest()
  print("--- mtest2")
  local count = math.random(10)+15
  local values = {}
  math.randomseed(os.time())
  for i=1,count do
    values[i] = math.random(1024)
  end

  local e = lightningmdb.env_create()
  e:set_mapsize(10485760)
  os.execute("mkdir ./temp/testdb")
  e:open("./temp/testdb",e.MDB_FIXEDMAP,420)
  local t = e:txn_begin(nil,0)
  local d = t:dbi_open(nil,0)

  print("adding values:",count)
  local j = 0
  for i,v in ipairs(values) do
    local rc = t:put(d,string.format("%03x",v),string.format("%d foo bar",v),
                     lightningmdb.MDB_NOOVERWRITE)
    if not rc then
      j = j + 1
    end
  end

  print(j,"duplicates skipped")
  t:commit()
  ps(e)

  t = e:txn_begin(nil,0)
  c = t:cursor_open(d)
  local k


  for k,v in cursor_pairs(c) do
    print(k,v)
  end

  c:close()
  t:abort()
  math.randomseed(os.time())
  j = 0
  for i=count,1,-math.random(5) do
    j = j + 1
    t = e:txn_begin(nil,0)
    local key = string.format("%03x",values[i])
    if not t:del(d,key,nil) then
      j = j - 1
      t:abort()
    else
      t:commit()
    end
  end

  print("deleted",j,"values")
  ps(e)
  t = e:txn_begin(nil,0)
  c = t:cursor_open(d)
  print("cursor next")
  local key
  for k,v in cursor_pairs(c,nil,c.MDB_NEXT) do
    print(k,v)
    key = k
  end

  print("cursor prev")

  for k,v in cursor_pairs(c,key,c.MDB_PREV) do
    print(k,v)
  end

  c:close()
  e:dbi_close(d)

  t:abort()
  e:close()
end

local function mtest2()
  print("--- mtest2")

  local count = math.random(10)+15
  local values = {}
  math.randomseed(os.time())
  for i=1,count do
    values[i] = math.random(1024)
  end

  local e = lightningmdb.env_create()
  e:set_mapsize(10485760)
  e:set_maxdbs(4)
  os.execute("mkdir ./temp/testdb")
  e:open("./temp/testdb",e.MDB_FIXEDMAP + e.MDB_NOSYNC,420)
  local t = e:txn_begin(nil,0)
  local d = t:dbi_open("id1",t.MDB_CREATE)

  print("adding values:",count)
  local j = 0
  for i,v in ipairs(values) do
    local rc = t:put(d,string.format("%03x",v),string.format("%d foo bar",v),
                     lightningmdb.MDB_NOOVERWRITE)
    if not rc then
      j = j + 1
    end
  end

  print(j,"duplicates skipped")
  t:commit()
  ps(e)

  t = e:txn_begin(nil,0)
  c = t:cursor_open(d)
  local k


  for k,v in cursor_pairs(c) do
    print(k,v)
  end

  c:close()
  t:abort()
  math.randomseed(os.time())
  j = 0
  for i=count,1,-math.random(5) do
    j = j + 1
    t = e:txn_begin(nil,0)
    local key = string.format("%03x",values[i])
    if not t:del(d,key,nil) then
      j = j - 1
      t:abort()
    else
      t:commit()
    end
  end

  print("deleted",j,"values")

  ps(e)
  t = e:txn_begin(nil,0)
  c = t:cursor_open(d)
  print("cursor next")
  local key
  for k,v in cursor_pairs(c,nil,c.MDB_NEXT) do
    print(k,v)
    key = k
  end

  print("cursor prev")

  for k,v in cursor_pairs(c,key,c.MDB_PREV) do
    print(k,v)
  end

  c:close()
  e:dbi_close(d)

  t:abort()
  e:close()
end

local function mtest3()
  print("--- mtest3")

  local count = math.random(10)+15
  local values = {}
  math.randomseed(os.time())
  for i=1,count do
    values[i] = math.random(1024)
  end

  local e = lightningmdb.env_create()
  e:set_mapsize(10485760)
  e:set_maxdbs(4)
  os.execute("mkdir ./temp/testdb")
  e:open("./temp/testdb",e.MDB_FIXEDMAP + e.MDB_NOSYNC,420)

  local t = e:txn_begin(nil,0)
  local d = t:dbi_open("id2",t.MDB_CREATE+t.MDB_DUPSORT)

  print("adding values:",count)
  local j = 0
  for i,v in ipairs(values) do
    if i%5==0 then
      v = values[i-1]
    end
    local rc = t:put(d,string.format("%03x",v),string.format("%d foo bar",v),
                     lightningmdb.MDB_NODUPDATA)
    if not rc then
      j = j + 1
    end
  end
  if j>0 then
    print("duplicate skipped",j)
  end
  t:commit()
  ps(e)

  t = e:txn_begin(nil,0)
  c = t:cursor_open(d)

  for k,v in cursor_pairs(c,nil,c.MDB_NEXT) do
    print(k,v)
  end

end


mtest()
mtest2()
mtest3()