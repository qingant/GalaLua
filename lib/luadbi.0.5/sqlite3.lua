local database = require("DBI")

local conn = database.Connect('SQLite3',"/home/gala/test.db")
conn:autocommit(true)
--conn:execute("CREATE TABLE IF NOT EXISTS  request_pool (trade_no INT UNIQUE, stamp INT, buffer blob);")
--local cur = conn:execute('insert into request_pool(trade_no,stamp,buffer) values(?,?,?)',1,234,"wdqwdqdq")
local cur = conn:execute("select * from request_pool where trade_no=?",1)
local rc = cur:fetchall()
for k ,v in ipairs(rc) do
   print(k,v)
   for key ,value in pairs(v) do
       print(key,value)
   end
end

