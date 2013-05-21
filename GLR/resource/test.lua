--*-- coding: utf-8 --*--
-- gettime.lua --

local retval = os.date("*t")
retval = os.date("%F %T", os.time(retval))
io.write(string.format("gettime.lua: %s\n", retval))

