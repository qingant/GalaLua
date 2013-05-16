local lightningmdb_lib=require "lightningmdb"

-- local lightningmdb = _VERSION=="Lua 5.2" and lightningmdb_lib or lightningmdb

local function pt(t)
  for k,v in pairs(t) do
    print(k,v)
  end
end

local function basic_test()
  print("Lightning MDB version:",lightningmdb.version())
  print("Lightning error:",lightningmdb.strerror(0))

  -- print("-- globals --")
  -- pt(lightningmdb)

  -- env
  local e = lightningmdb.env_create()
  print(e)
  os.execute("mkdir  /tmp/temp/foo")
  print(e:open("/tmp/temp/foo",0,420))
  print("fixedmap",lightningmdb.MDB_FIXEDMAP)
  print("read only",lightningmdb.MDB_RDONLY)

  print("-- stats --")
  pt(e:stat())

  print("-- info --")
  pt(e:info())
  print("get_path",e:get_path())


  --txn
  local t = e:txn_begin(nil,0)
  print("txn",t)
  t:commit()
  t = e:txn_begin(nil,0)
  print("txn",t)
  t:reset()
  t:renew()
  --t:abort()
  local db = t:dbi_open(nil,0)
  print("-- txn stat --")
  pt(t:stat(db))

  t:abort()
  e:close()
end

local function grow_db()
  print("--- grow_db ---")
  local num_pages = 5
  local e

  os.execute("mkdir /tmp/temp/bar")

  local function grow()
    e = lightningmdb.env_create()
    num_pages = num_pages * 2
    print(e:set_mapsize(num_pages*4096))
    print(e:open("/tmp/temp/bar",0,420))
  end

  grow()
  local t = e:txn_begin(nil,0)
  local db = t:dbi_open(nil,lightningmdb.MDB_DUPSORT)
  for i=1,600 do
    local rc,err = t:put(db,"hello "..i,"cruel world",lightningmdb.MDB_NODUPDATA)
    if not rc then
      if err:find("MDB_MAP_FULL",1,true) then
        print("making more room at",i)
        t:abort()
        e:close()

        grow()
        t = e:txn_begin(nil,0)
        db = t:dbi_open(nil,lightningmdb.MDB_DUPSORT)
      else
        print(rc,err)
      end
    end
  end
end

basic_test()
grow_db()
