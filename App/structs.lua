module(..., package.seeall)

local ffi = require "ffi"




ffi.cdef[[
uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);

size_t strlen(const char *s);
#pragma pack(1)

typedef enum 
{
    DEV_AGENT = 0,
    DEV_DISPLAY = 1,
    DEV_SVC = 2,
    DEV_LSR = 3,
    DEV_FILTER = 4,
    DEV_MONITOR = 5,
} DEV_CATAGORY;

typedef enum 
{
    ACT_REPORT = 0,
    ACT_REQUEST = 1,
    ACT_RESPONSE = 3,
    ACT_BIND = 4,
    ACT_ROUTER_QUERY = 5,
    ACT_ROUTER_ADD = 6,
    ACT_ACK = 7,
	ACT_DEPLOY = 8,
    ACT_ROUTER_DEL=9,
    ACT_SUBSCRIB = 10,
    ACT_SUBSCRIB_DEL = 11,
    ACT_SUBSCRIB_QRY = 12,
}ACTION_TYPE;
typedef uint32_t MSG_ID_TYPE;

typedef struct
{
    MSG_ID_TYPE MsgId;             // MSG ID
    unsigned char Action;       // ACTION_TYPE
}MSG_HEAD;
typedef struct
{
    char Host[20];
    uint32_t Port;
    uint32_t Gpid;
}GLR_ADDR;
typedef struct
{
    MSG_HEAD      Head;
    unsigned char Catagory;     // DEV_CATAGORY
    uint32_t      Gpid;        // Gpid Binded Used to response
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
        char          Name[20];
    }From;
    struct {
        unsigned char Catagory;     // DEV_CATAGORY
        GLR_ADDR      Addr;
        char          AppType[20];  // Application Type
        char          Name[20];
    }To;
}APP_HEADER;

typedef enum
{
    REG=0,
    GET=1,
    PUT=2,
    DEL=3,
}MONITOR_ACTION;

typedef enum
{
    ALL=0,
    AMQ=1,
    ROUTER=2,
    NODE=3,
}MONITOR_TYPE;

typedef struct
{
    MSG_ID_TYPE     MsgId;             // MSG ID
    unsigned char Action;  //MONITOR_ACTION  
    unsigned char Type;     //MONITOR_TYPE    
}MONITOR_HEADER;

#pragma pack()
]]


function pack(cdata)
   return ffi.string(cdata, ffi.sizeof(cdata))
end

function str_pack(cdata)
   return ffi.string(cdata, ffi.C.strlen(cdata))
end
