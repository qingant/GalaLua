
local structX = require("structX")

local data = {
   base={
      alias="alias",svc=8
   },
   lsr={
      conn={
         [3]={host="127.0.0.1",port=80},
         [5]={host="192.168.0.1",port=81}
      }
   },
   ctr={
     [2]={alias="ctr000",pid=1234},
     [4]={alias="ctr001",pid=1235}
   }
}

local endian = "little"
local des = {
   {name="base",type="struct",items=
      {
         {name="alias",type="string",size=32},
         {name="svc",type="int32_t"}
      }
   },
   {name="lsr",type="struct",size=2 ,items=
      {
         {name="conn",type="arraystruct",size=8 ,items=
            {
               {name="host",type="string",size=20},
               {name="port",type="int16_t"}
            }
         }
      }
   },
   {name="ctr",type="arraystruct",size=10,items=
      {
         {name="alias",type="string",size=12},
         {name="pid",type="int32_t"}
      }
   }
}



local str = structX.packX(data,"big",des)
--print(str,#str)
local rt = structX.unpackX(str,"big",des)

function show(rt)
   for k,v in pairs(rt) do
      if type(v) == "table"  then
         --print("k:v",k,v)
         show(v)
      else
         print(k,v)
      end
   end
end

show(rt)

print("------",rt.base.svc,rt.lsr.conn[3].host,rt.lsr.conn[3].port,rt.lsr.conn[5].host,rt.lsr.conn[5].port,"---------")

print(rt.ctr[2].alias,rt.ctr[2].pid)
