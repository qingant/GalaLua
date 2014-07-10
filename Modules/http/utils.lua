--[[
--Author : Liu Wenxue
--Date  : 2014-07-08
]]

--gen Http request header Date property
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

--table get between e,b seperate by sep 
--@param table
--@param s : start
--@param e : end
--@param sep : seprate character
--@return : string 
function slice(table,...)
    s, e, sep = ...
    if not s then 
        s = 1
    end
    if not e then
        e = #table + 1
    end

    if not sep then
        sep = ""
    end

    local str = ""
    for i,k in  ipairs(table) do
        if i == s then
            str = str..k
        elseif i > s and i < e then
            str = str..sep..k
        end
    end
    return str
end


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

   while s do
      if s ~= 1 or cap ~= "" then
        table.insert(Table,cap)
      end
      last_end = e+1
      s, e, cap = string.find(pString, fpat, last_end)
   end
   if last_end <= #pString then
      cap = string.sub(pString, last_end)
      table.insert(Table, cap)
   end
   return Table
end


function test_get_date()
    print(get_date())
end

function test_slice()
    table = {1,3,4,5}
    print(slice(table,1,2,"."))
end
