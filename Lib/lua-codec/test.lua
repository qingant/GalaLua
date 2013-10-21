local code=require "codec"

local pprint=require "pprint"

function printstr(str,prompt)
    print(string.format("%s (%d):'%s'",prompt or "print",#str,str))
end

local r=("你好abc"):rep(4000)
printstr(r,"raw utf8:")
local a=code.decode("utf8",r)
printstr(a,"unicode:")
local c=code.encode("utf8",a)
printstr(c,"utf8:")
