--[[
--Author : Liu Wenxue
--Date  : 2014-07-08
]]

module(...,package.seeall)
pprint = require("pprint")
--gen Http request header Date property
--timezone of GMT,default current timezone is china
function get_date(delay,timezone) 
    if not timezone  then
        timezone = 8
    end
    if not delay then
        delay = 0
    end
    local WEEK = {"Sun.","Mon.", "Tues.", "Wed.", "Thur.","Fri.", "Sta."}
    local MONTH = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}
    local NUM = {"01","02","03","04","05","06","07","08","09","10","11","12","13","14","15",
                "16","17","18","19","20","21","22","23","24","25","26","27","28","29","30",
                "31","32","33","34","35","36","37","38","39","40","41","42","43","44","45",
                "46","47","48","49","50","51","52","53","54","55","56","57","58","59",[0]="00"}
    --timezone of GMT
    local date = os.date("*t",os.time() - 3600*timezone + delay)
    local week = WEEK[date["wday"]]
    local day = NUM[date["day"]]
    local month = MONTH[date["month"]]
    local year = date["year"]
    local time = string.format("%s:%s:%s",NUM[date["hour"]],NUM[date["min"]],NUM[date["sec"]])
    --local time = date["hour"]..":"..date["min"]..":"..date["sec"].." ".."GMT"
    --date = week.." "..day.." "..month.." "..year.." "..time
    local dt = string.format("%s %s %s %d %s %s",week,day,month,year,time,"GMT")
    return dt
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
-- see test_strip()
function strip(pString)
    local s1,e1 = string.find(pString,"^%s+")
    if not e1 then
        e1 = 0
    end
    local s2,e2 = string.find(pString,"%s+$")
    if not s2 then
        s2 = #pString + 1
    end
    return string.sub(pString,e1+1,s2-1)
end
--------------------------------------
---seperate date to items
--for example, 
--
--Mon. Jul 21 09:59:44 2014 
--
--date_item["wday"] = 1
--date_item["month"] = 7
--date_item["day"] = 21
--date_item["hour"] = 09
--date_item["min"] = 59
--date_item["sec"] = 44
--date_item["year"] 2014
--------------------------------------
function parse_date(date)
    local WEEK = {["Sun,"] = 1,["Mon,"] = 2, ["Tues,"] = 3, ["Wed,"] = 4,["Thur,"] = 5,["Fri,"] = 6, ["Sta,"] = 7,
                 ["Sun"] = 1,["Mon"] = 2, ["Tues"] = 3, ["Wed"] = 4, ["Thur"] = 5,["Fri"] = 6, ["Sta"] = 7}
    local MONTH = {["Jan"] = 1, ["Feb"] = 2, ["Mar"] = 3, ["Apr"] = 4, ["May"] = 5, 
                    ["Jun"] = 6, ["Jul"] = 7, ["Aug"] = 8, ["Sept"] = 9, ["Oct"] = 10, ["Nov"] = 11, ["Dec"] = 12}
    local date_item = {}
    date = tostring(date)
    date_table = split(date,"%s+")
    date_item["wday"] = WEEK[date_table[1]]
    date_item["day"] = date_table[2]
    date_item["month"] = MONTH[date_table[3]]
    date_item["year"] = date_table[4]
    time = split(date_table[5],":")
    date_item["hour"] = time[1]
    date_item["min"] = time[2]
    date_item["sec"] = time[3]
    return date_item
end

-------------------------------
-- date1 and date2 are same as os.data()'s return
-- if date1 old date2 return -1
-- if date1 equal date2 return 0
-- if date1 new date2 return 1
-------------------------------

function diff_date(date1, date2)
    if date1 == date2 then
        return 0
    end
    date1 = parse_date(date1)
    date2 = parse_date(date2)
    if date1["year"] < date2["year"] then
        return -1
    elseif date1["year"] > date2["year"] then
        return 1
    else
        if date1["month"] < date2["month"] then
            return -1
        elseif date1["month"] > date2["month"] then
            return 1
        else
            if date1["day"] < date2["day"] then
                return -1
            elseif date1["day"] > date2["day"] then
                return 1
            else
                if date1["hour"] < date2["hour"] then
                    return -1
                elseif date1["hour"] > date2["hour"] then
                    return 1
                else
                    if date1["min"] < date2["min"] then
                        return -1
                    elseif date1["min"] > date2["min"] then
                        return 1
                    else
                        if date1["sec"] < date2["sec"] then
                            return -1
                        elseif date1["sec"] > date2["sec"] then
                            return 1
                        else
                            return 0
                        end
                    end
                end
            end
        end
    end
end

function test_get_date()
    print(get_date())
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

function test_parse_date()
    str = os.date()
    print("date : ", str)
    local t = parse_date(str)
    for i,k in pairs(t) do
        print(i,k)
    end

    str1 = "Mon   Jul  21   10:42:03   2014"
    print("date : ", str1)
    local t1 = parse_date(str1)
    for i,k in pairs(t1) do
        print(i,k)
    end

    str2 = "Mon.   Jul  21   10:42:03   2014"
    print("date : ", str2)
    local t2 = parse_date(str2)
    for i,k in pairs(t2) do
        print(i,k)
    end
end

function test_diff_date()
    str1 = "Mon   Jul  22   10:42:03   2015"
    str2 = "Mon.  Jul  22   10:42:03  2014"
    str3 = "Mon.  Jul  22   10:42:05   2014"
    str4 = "Mon.   Jul  22   10:45:05   2014"
    str5 = "Mon.   Jul  22  19:45:05   2014"
    str6 = "Mon.   Jul  21   19:45:05   2014"
    str7 = "Mon.   Jun  21  19:45:05  2014"
    str8 = "Wed.   Jun  21  19:45:05   2014"
    function print_diff(str1, str2)
        print ("str1", str1)
        print ("str2", str2)
        local d = diff_date(str1, str2)
        if d == 1 then
            print("str1 newer then str2\n")
        elseif d == -1 then
            print("str1 older then str2\n")
        else
            print("str1 equal to str2\n")
        end
    end

    print_diff(str1,str2)
    print_diff(str2,str3)
    print_diff(str3,str4)
    print_diff(str4,str5)
    print_diff(str5,str6)
    print_diff(str6,str7)
    print_diff(str7,str8)

end

