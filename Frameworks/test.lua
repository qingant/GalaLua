module(...,package.seeall)
--[[
local slice = require("http.utils")
local pprint = require("pprint")

pprint.pprint(slice)
table = {1,2,4,5,56}
print(slice(table,2,4))
]]

--TODO parse pPattern replace %. as %s\ 
--@param : pString : str what to split
--@param : pPattern : character is splited by
--@return : table  like python split
function split(pString, pPattern)
   local Table = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pPattern
   local last_end = 1
   --local s, e, cap = pString:find(fpat, 1)
   local s, e, cap = string.find(pString, fpat, 1)
   print(s,e,cap)

   while s do
      if s ~= 1 or cap ~= "" then
        table.insert(Table,cap)
      end
      last_end = e+1
      s, e, cap = string.find(pString, fpat, last_end)
      print(s,e,cap)
   end
   if last_end <= #pString then
      cap = string.sub(pString, last_end)
      table.insert(Table, cap)
   end
   return Table
end

function get_date() 
    local WEEK = {"Sun.","Mon.", "Tues.", "Wed.", "Thur.","Fri.", "Sta."}
    local MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}
    date = os.date("!*t")
    local week = WEEK[date["wday"]]
    local day = date["day"]
    local month = MONTH[date["month"]]
    local year = date["year"]
    local time = date["hour"]..":"..date["min"]..":"..date["sec"].." ".."GMT"
    date = week.." "..day.." "..month.." "..year.." "..time
    return date
end 

table = {1,2,3,4,5}
print(get_date())
--table1 = slice(table,1,3)
--print(table1)
--table2 = slice(table)
--print(table2)

function strip(pString)
    local s1,e1 = string.find(pString, "%s+", 1)
    print(s1,e1)
    if not e1 then
       e1 = 0
       s2,e2 = string.find(pString,"%s+", e1+1)
    end
    print(s2,e2)
    if s1 ~= 1 and s1 then
       return string.sub(pString,1,s1-1)
    end
    if not s2 then
       return string.sub(pString,e1+1)
    end
    return string.sub(pString,e1+1,s2-1)
end

function test_strip()
    a = "abc"
    print(a)
    print(strip(a))
    a = " abc "
    print(a)
    print(strip(a))
    a = "   abc   "
    print(a)
    print(strip(a))
    a = "adga  "
    print(a)
    print(strip(a))
    a = "   adga"
    print(a)
    print(strip(a))
end

test_strip()

function format_cache(cache)
    objs = split(cache,":")
    for i,k in pairs(objs) do
        print(i,k)
        --obj = split(k,"=")
        --cache[strip(obj[1])] = strip(obj[2])
    end
    return cache
end


