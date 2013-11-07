module(...,package.seeall)

local function isleap(year)
    return ((year) % 4 == 0 and ((year) % 100 ~= 0 or (year) % 400 == 0))
end

local months = {31,28,31,30,31,30,31,31,30,31,30,31}

local osdatetime = require("osdatetime")
local floor = math.floor
local ceil  = math.ceil

local function zonediff(num)
   return num * 60 * 60
end

local function fix(n)
   n = tonumber(n)
   return n and ((n > 0 and floor or ceil)(n))
end

local function mod(n,d)
   return n - d*floor(n/d)
end

local function dayfromyear(y)
   return 365*y + floor(y/4) - floor(y/100) + floor(y/400)
end

local function makedaynum(y, m, d)
   local mm = mod(mod(m,12) + 10, 12)
   return dayfromyear(y + floor(m/12) - floor(mm/10)) + floor((mm*306 + 5)/10) + d - 307
end

local function __retimedelta__(a)
   local self = a
   local sztemp = 0
   if self.microseconds < 0 then
      sztemp = fix(self.microseconds / 1000000)
      self.seconds = self.seconds + sztemp
      self.microseconds = self.microseconds - (sztemp * 1000000)
   else
      sztemp = fix(self.microseconds / 1000000)
      self.microseconds = self.microseconds % 1000000
      self.seconds = self.seconds + sztemp
   end
   if self.seconds < 0 then
      sztemp = fix(self.seconds / 86400)
      self.days = self.days + sztemp
      self.seconds = self.seconds - (sztemp * 86400)
   else
      sztemp = fix(self.seconds / 86400)
      self.seconds = self.seconds % 86400
      self.days = self.days + sztemp
   end
   return self
end

local function __redatetime__(a,flag)
   local self = a
   local sztemp = 0
   sztemp = self.microseconds / 1000000
   self.microseconds = self.microseconds % 1000000
   self.second = self.second + fix(sztemp)
   if flag == true then
      sztemp = (((makedaynum(self.year,self.month,self.day)+1)*24+self.hour)*60+self.minute)*60
   else
      sztemp = ((makedaynum(self.year,self.month,self.day)*24+self.hour)*60+self.minute)*60
   end
   sztemp = sztemp - makedaynum(1970,1,1)*24*60*60
   self.second = self.second + sztemp - zonediff(8)
   local datestr = os.date("%Y-%m-%d %H:%M:%S",self.second,"%Y-%m-%d %H:%M:%S")
   local times = osdatetime.strptime(datestr,"%Y-%m-%d %H:%M:%S")
   self.year = times.year
   self.month = times.month
   self.day = times.day
   self.hour = times.hour
   self.minute = times.min
   self.second = times.sec
   return self
end

local function add(a,b)
   local tempobj1 = datetime:new()
   local tempobj2 = timedelta:new()
   if(getmetatable(a) ~= getmetatable(tempobj1) and
      getmetatable(a) ~= getmetatable(tempobj2)) then
      error("not support type")
   end
   if(getmetatable(b) ~= getmetatable(tempobj1) and
      getmetatable(b) ~= getmetatable(tempobj2)) then
      error("not support type")
   end
   if getmetatable(a)== getmetatable(b) then
      if getmetatable(a) == getmetatable(tempobj2) then
         for k,v in pairs(a) do
            a[k] = a[k] + b[k]
         end
         return __retimedelta__(a);
      end
      local astr = string.format("%d-%d-%d %d:%d:%d",a.year,a.month,a.day,a.hour,a.minute,a.second)
      local asec = osdatetime.mktime(astr,"%Y-%m-%d %H:%M:%S")
      local bstr = string.format("%d-%d-%d %d:%d:%d",b.year,b.month,b.day,b.hour,b.minute,b.second)
      local bsec = osdatetime.mktime(bstr,"%Y-%m-%d %H:%M:%S")
      local sum =  asec + bsec
      local tm = osdatetime.strptime(os.date("%Y-%m-%d %H:%M:%S",sum),"%Y-%m-%d %H:%M:%S")

      return datetime:new():init(tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec)
   elseif (getmetatable(a) == getmetatable(tempobj1)) then
      local astr = string.format("%d-%d-%d %d:%d:%d",a.year,a.month,a.day,a.hour,a.minute,a.second)
      local asec = osdatetime.mktime(astr,"%Y-%m-%d %H:%M:%S")
      local bsec = b:tosecond()
      local sum =  asec + bsec
      local tm = osdatetime.strptime(os.date("%Y-%m-%d %H:%M:%S",sum),"%Y-%m-%d %H:%M:%S")

      return datetime:new():init(tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec)
   else
      local bstr = string.format("%d-%d-%d %d:%d:%d",b.year,b.month,b.day,b.hour,b.minute,b.second)
      local bsec = osdatetime.mktime(bstr,"%Y-%m-%d %H:%M:%S")
      local asec = a:tosecond()
      local sum =  asec + bsec
      local tm = osdatetime.strptime(os.date("%Y-%m-%d %H:%M:%S",sum),"%Y-%m-%d %H:%M:%S")
      return datetime:new():init(tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec)
   end
end

local function sub(a,b)
   local tempobj1 = datetime:new()
   local tempobj2 = timedelta:new()
   if(getmetatable(a) ~= getmetatable(tempobj1) and
      getmetatable(a) ~= getmetatable(tempobj2)) then
      error("not support type")
   end
   if(getmetatable(b) ~= getmetatable(tempobj1) and
      getmetatable(b) ~= getmetatable(tempobj2)) then
      error("not support type")
   end
   if getmetatable(a)== getmetatable(b) then
      if getmetatable(a) == getmetatable(tempobj2) then
         for k,v in pairs(a) do
            a[k] = a[k] - b[k]
         end
         return __retimedelta__(a);
      end
      local astr = string.format("%d-%d-%d %d:%d:%d",a.year,a.month,a.day,a.hour,a.minute,a.second)
      local asec = osdatetime.mktime(astr,"%Y-%m-%d %H:%M:%S")
      local bstr = string.format("%d-%d-%d %d:%d:%d",b.year,b.month,b.day,b.hour,b.minute,b.second)
      local bsec = osdatetime.mktime(bstr,"%Y-%m-%d %H:%M:%S")
      local sum =  asec - bsec
      local tm = osdatetime.strptime(os.date("%Y-%m-%d %H:%M:%S",sum),"%Y-%m-%d %H:%M:%S")

      return datetime:new():init(tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec)
   elseif (getmetatable(a) == getmetatable(tempobj1)) then
      local astr = string.format("%d-%d-%d %d:%d:%d",a.year,a.month,a.day,a.hour,a.minute,a.second)
      local asec = osdatetime.mktime(astr,"%Y-%m-%d %H:%M:%S")
      local bsec = b:tosecond()
      local sum =  asec - bsec
      local tm = osdatetime.strptime(os.date("%Y-%m-%d %H:%M:%S",sum),"%Y-%m-%d %H:%M:%S")

      return datetime:new():init(tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec)
   else
      local bstr = string.format("%d-%d-%d %d:%d:%d",b.year,b.month,b.day,b.hour,b.minute,b.second)
      local bsec = osdatetime.mktime(bstr,"%Y-%m-%d %H:%M:%S")
      local asec = a:tosecond()
      local sum =  asec - bsec
      local tm = osdatetime.strptime(os.date("%Y-%m-%d %H:%M:%S",sum),"%Y-%m-%d %H:%M:%S")
      return datetime:new():init(tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec)
   end
end

datetime = {}
function datetime:new()
   local o = {}
   setmetatable(o,self)
   self.__index = self;
   self.__add = add
   self.__sub = sub
   return o;
end

function datetime:init(year,month,day,hour,minute,second,microseconds)

   if year and year < 1970 then
       error("year invalid")
   end
   self.year = year or 1970
   if month and (month < 1 or month > 12) then
       error("moth invalid")
   end
   self.month = month or 1
   if day and (day < 1 or day > 31) then
       error("day invalid")
   end
   self.day = day or 1
   local szmonth = months[self.month]
   if self.month == 2 and isleap(self.year) then
       szmonth = szmonth + 1
   end
   if self.day > szmonth then
       error("day overflow")
   end
   if hour and (hour < 0 or hour > 24) then
       error("hour invalid")
   end
   self.hour = hour or 0
   if minute and (minute < 0 or minute > 60) then
       error("minute invalid")
   end
   self.minute = minute or 0
   if second and (second < 0 or second > 60) then
       error("second invalid")
   end
   self.second = second or 0
   if microseconds and (microseconds < 0 or microseconds >1000000) then
       error("microseconds invalid")
   end
   self.microseconds = microseconds or 0

   return self
end

function datetime:strtime()
   return string.format("%d-%d-%d %d:%d:%d %d",self.year,self.month,
      self.day,self.hour,self.minute,self.second,self.microseconds)
end

function datetime:strftime(format)
   return os.date(format,os.time())
end

function datetime:strptime(date_string, format)
   local  tbltime = osdatetime.strptime(date_string,format)
   self.year = tbltime.year
   self.month = tbltime.month
   self.day = tbltime.day
   self.hour = tbltime.hour
   self.minute = tbltime.min
   self.second = tbltime.sec
   self.microseconds = 0
   return self
end

function datetime:today()
   local now = osdatetime.gettimeofday()
   local obj = self:strptime(os.date("%Y-%m-%d %H:%M:%S",tonumber(now.sec)),"%Y-%m-%d %H:%M:%S")
   obj.microseconds = tonumber(now.msec)
   return obj
end

function datetime:now()
   return self:today()
end

function datetime:utcnow()
   local now = osdatetime.gettimeofday()
   local obj = self:strptime(os.date("!%Y-%m-%d %H:%M:%S",tonumber(now.sec)),"%Y-%m-%d %H:%M:%S")
   obj.microseconds = tonumber(now.msec)
   return obj
end

function datetime:fromtimestamp(timestamp)
   local sec = fix(timestamp)
   local msec = (timestamp - sec)*1000000
   msec = fix(msec)
   local obj = self:strptime(os.date("%Y-%m-%d %H:%M:%S",sec),"%Y-%m-%d %H:%M:%S")
   obj.microseconds = msec
   return obj
end

function datetime:utcfromtimestamp(timestamp)
   local sec = fix(timestamp)
   local msec = (timestamp - sec)*1000000
   msec = fix(msec)
   local obj = self:strptime(os.date("!%Y-%m-%d %H:%M:%S",sec),"%Y-%m-%d %H:%M:%S")
   obj.microseconds = msec
   return obj
end

timedelta = {}
function timedelta:new()
   local o = {}
   setmetatable(o,self)
   self.__index = self;
   self.__add = add
   self.__sub = sub
   return o
end

function timedelta:init(days,seconds,microseconds,milliseconds,minutes,hours,weeks)
   days = days or 0
   seconds = seconds or 0
   microseconds = microseconds or 0
   milliseconds = milliseconds or 0
   minutes = minutes or 0
   hours = hours or 0
   weeks = weeks or 0
   local calculatemsec = (microseconds +(milliseconds * 1000))
   local sztemp = 0
   if calculatemsec < 0 then
      sztemp = (calculatemsec - 999999) / 1000000
      sztemp = fix(sztemp)
      microseconds = calculatemsec - (sztemp * 1000000)
      seconds = seconds + sztemp
   else
      microseconds = calculatemsec % 1000000
      seconds = seconds + fix((calculatemsec / 1000000))
   end
   if seconds < 0 then
      sztemp = fix((seconds - 59) / 60)
      seconds = seconds - (sztemp * 60)
      minutes = minutes + sztemp
   else
      minutes = minutes + fix(seconds / 60)
      seconds = seconds % 60
   end
   if minutes < 0 then
      sztemp = fix((minutes - 59) / 60)
      minutes = minutes - (sztemp * 60)
      hours = hours + sztemp
   else
      hours = hours + fix(minutes / 60)
      minutes = minutes % 60
   end
   if hours < 0 then
      sztemp = fix((hours - 23) / 24)
      hours = hours - (sztemp * 24)
      days = days + sztemp
   else
      days = days + fix(hours / 24)
      hours = hours % 24
   end
   days = days + weeks * 7
   self.days = days
   self.seconds = seconds + (hours * 60 + minutes)*60
   self.microseconds = microseconds
   return self
end

function timedelta:tostring()
   local second = self.seconds % 60
   local minute = self.seconds / 60
   local hour = minute / 60
   minute = minute % 60
   return string.format("%d days %02d:%02d:%02d %d",self.days,hour,minute,second,self.microseconds)
end

function timedelta:tosecond()
    return (self.days * 24 * 60 * 60 + self.seconds + (self.microseconds + 999999)/1000000)
end

if ... == "__main__" then
    --初始化时指定日期2013-10-21 23:45:38
    local date = datetime:new():init(2013,10,21,23,45,38)
    print(date:strtime())
    --当前时间
    date = datetime:new():init():now()
    print(date:strtime())
    --时间字符串转换
    date = date:strptime("2013-10-21 23:45:38","%Y-%m-%d %H:%M:%S")
    print(date:strtime())
    --时间差：指定时间的前一天
    delta = timedelta:new():init(-1)
    local diff = date + delta
    print(diff:strtime())
    --时间差：指定时间的后一天
    delta = timedelta:new():init(1)
    diff = date + delta
    print(diff:strtime())
    --时间差：指定时间的前一个秒
    delta = timedelta:new():init(_,-1)
    diff = date + delta
    print(diff:strtime())
end
