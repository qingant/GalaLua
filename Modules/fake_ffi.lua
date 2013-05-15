module(...,package.seeall)

local structX = require("structX")

ffi = {}

function ffi:new()
    obj = {};
    setmetatable(obj,self);
    self.__index = self;
    return obj;
end

function ffi.sizeof(tbldes)
    function get_entry(tbl)
        if type(tbl) ~= "table" then
           error("the "..tbl.." is not table")
        end
        if not tbl["type"] then
           error("the tbl not exist type key")
        end
        if tbl["type"] == "string" then
           return tbl["size"]
        elseif tbl["type"] == "int8_t" or tbl["type"] == "uint8_t" then
           return 1;
        elseif tbl["type"] == "int16_t" or tbl["type"] == "uint16_t" then
           return 2
        elseif tbl["type"] == "int32_t" or tbl["type"] == "uint32_t" then
           return 4
        elseif tbl["type"] == "int64_t" or tbl["type"] == "uint64_t" then
           return 8
        else
           error("the "..tbl["type"].." undefined")
        end
    end

    function foreach_table(tbl)
       local num = 0
       if type(tbl) == "table" then
           if not tbl["type"] then
               for k,v in pairs(tbl) do
	           num = num +foreach_table(v)
	       end
           elseif tbl["type"] == "struct" then
               num = num + foreach_table(tbl["items"])
           elseif tbl["type"] == "arraystruct" then
               num = num + (tbl["size"] * foreach_table(tbl["items"]))
           else
               num = num + get_entry(tbl)
           end
           return num
       else
           error("the tbl is not table")
       end
    end
    return foreach_table(tbldes)
end

C = {
    DEV_AGENT = 0,
    DEV_DISPLAY = 1,
    DEV_SVC = 2,
    DEV_LSR = 3,
    DEV_FILTER = 4,
    DEV_MONITOR = 5,
    ACT_REPORT = 6,
    ACT_REQUEST = 7,
    ACT_RESPONSE = 8,
    ACT_BIND = 9,
    ACT_ROUTER_QUERY = 10,
    ACT_ATTACH= 11,
    ACT_ACK = 12,
    ACT_DEPLOY = 13,
    ACT_ROUTER_DEL=14,
    ACT_SUBSCRIB = 15,
    ACT_SUBSCRIB_DEL = 16,
    ACT_SUBSCRIB_QRY = 17,
    ACT_CONTROL = 18,
    ACT_AGENT_STATUS = 19,
    ACT_TASK_ADD= 20,
    ACT_TASK_DEL= 21,
    ACT_TASK_QUERY= 22,
    REG=23,
    GET=24,
    PUT=25,
    DEL=26,
    ALL=27,
    AMQ=28,
    ROUTER=29,
    NODE=30,
}

ffi.C=C

function ffi.C.htonl(hostlong)
    if type(hostlong) == "number" then
        return hostlong
    else
        error("the hostlong is not number")
    end
end

function ffi.C.htons(hostshort)
    if type(hostshort) == "number" then
        return hostshort
    else
        error("the hostshort is not number")
    end
end

function ffi.C.ntohl(netlong)
    if type(netlong) == "number" then
        return netlong
    else
        error("the netlong is not number")
    end
end

function ffi.C.ntohs(netshort)
    if type(netshort) == "number" then
        return netshort
    else
        error("the netshort is not number")
    end
end

function ffi.C.strlen(s)
   if type(s) == "string" then
       return #s
   else
       error("the s is not string")
   end
end


function encode(tbldata,tbldes)
    return structX.packX(tbldata,"big",tbldes)
end

function decode(tbldata,tbldes)
   return structX.unpackX(tbldata,"big",tbldes)
end

function str_pack(str)
    return str
end

