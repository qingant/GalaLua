
local structX = require("structX")

local data = {base={alias="alias",svc=8},lsr={conn={{host="127.0.0.0",port=80},{host="192.168.0.1",port=81}}}}
local endian = "little"
local des = {{name="base",type="struct",items={{name="alias",type="string"},{name="svc",type="int32_t"}}},{name="lsr",type="struct",items={name="conn",type="arraystruct",items={{{name="host",type="string"},{name="port",type="int16_t"}},{{name="host",type="string"},{name="port",type="int16_t"}}}}}}

local fmt, str = structX.packX(data,"big",des)

local rt = structX.unpackX(fmt,str,des)

function show(rt)
   for k,v in pairs(rt) do
      if type(v) == "table"  then
         print("k:v",k,v)
         show(v)
      else
         print(k,v)
      end
   end
end

show(rt)

print("------",rt.base.svc,rt.lsr.conn[1].host,"---------")

