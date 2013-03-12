local os = require "os"
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lib/lua/?.lua"
package.cpath = package.cpath .. ";" .. os.getenv("HOME") .. "/lib/lua/?.so"
local amq = require "amq"
local ffi = require "ffi"
local pprint = require "pprint"
local MQID = "channelv"
function main()
   -- local m = amq.new(os.getenv("HOME") .. "/channels/testAMQ.chl")
   -- glr.global(MQID, m, "CGalaxyMQ");
   -- print("Over")
   -- local q = m:NQArray():get(0)
   -- q:put(q.MQC_RTL,"Just For Test")
   

   ffi.cdef[[
uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);
typedef enum 
{
    DEV_AGENT = 0,
    DEV_DISPLAY = 1,
    DEV_SVC = 2,
    DEV_LSR = 3,
    DEV_FILTER = 4,
} DEV_CATAGORY;



typedef uint32_t MSG_ID_TYPE;
typedef enum
{
    REGISTER = 0,
    REGISTER_OK = 1,
    REGISTER_FAIL = 2,
    APPLICATION = 3,
} GLR_MSG_TYPE;
typedef struct
{
    uint32_t Len;               // Len
    unsigned char Type;          // MSG Type
    uint32_t Gpid;              // Destination
}GLR_HEADER;

struct{
    char   Host[20];              // Host
    uint32_t   Port;              // Port
    uint32_t   Gpid;           
}GLR_ADDR;
typedef struct
{
    MSG_ID_TYPE MsgId;             // MSG ID
    unsigned char Action;       // ACTION_TYPE
}MSG_HEAD;
typedef struct
{
    MSG_HEAD      Head;
    unsigned char Catagory;     // DEV_CATAGORY
    uint32_t      Gpid;
}BIND_MSG;
typedef struct
{
    MSG_HEAD Head;
    char          Name[20];
    char          Field[20];
    char          AppType[20];
}ROUTER_ADD_MSG;

typedef struct
{
    MSG_HEAD Head;

    struct {
        unsigned char Catagory;     // DEV_CATAGORY
        uint32_t      DevId; // Dev Name(Identifier)
    }From;
    struct {
        unsigned char Catagory;     // DEV_CATAGORY
        uint32_t      DevId;
        char          AppType[40];  // Application Type
    }To;
}APP_HEADER;
#pragma pack()
            ]]
            local head = ffi.new("GLR_HEADER", {10, ffi.C.APPLICATION,0 })
            local mt = getmetatable(head)
            
            local s = ffi.string("hello world")
            print(s, type(s))
            print(mt, type(mt))
            pprint.pprint(mt)
   -- configure.flag = 1
   
   -- head.buf = "test";
            print("Hello")
            print(head.Len)
            print(head.Type)
            print(head.Gpid)
            print(ffi.C.ntohl(head.Len))
            print(ffi.C.htonl(ffi.C.ntohl(head.Len)))
            local s = "abcdefg"
            
   -- print(ffi.string(head.buf))
   -- print(head.buf, type(head.buf))
   -- glr.spawn(glr.get_path(), "ttt");
   -- while true do
   --    glr.recv()
   -- end
   -- print "over"
   -- s = "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
   -- s = s..s..s..s..s
   -- for i=0,1000 do
   --    glr.send(id, s..s..s..s..s)
   -- end
--    s = loadstring("local a,b = ...
-- print(type(q))
-- while true do
-- glr.sleep(5)
-- return glr.node_addr()")("test", "ok")
--   print(s)
end


-- function ttt()
--    print("Repl")
--    while true do
--       -- err,msg = glr.recv()
--       -- print(msg)
   
--       local io = require "io"

--       io.stdout:write("\n=> ")
--       io.stdout:flush()
--       -- io.stdin:seek("set",0)
--       local cmd = io.stdin:read("*l")
--       print("return " .. cmd)
--       local status, fun = pcall(function () return loadstring("return "..cmd) end)
--       if status then
--          local rt = fun()
--          io.stdout:write(tostring(rt))
--       else
--          io.stdout:write(tostring(fun))
--       end

      
--       io.flush()
--    end
--    -- local m = glr.get_global(MQID)
--    -- print(type(m))
--    -- print(".." .. type("@" .. MQID))
--    -- local q = m:NQArray():get(0)
--    -- local err,msg = q:get(q.MQC_BAT)
--    -- print(msg)
-- end