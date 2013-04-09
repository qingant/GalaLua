#!/usr/bin/env python

#struct MSG_HEAD
#{   
#    enum MSG_TYPE
#    {
#        REGISTER = 0,
#        REGISTER_OK = 1,
#        REGISTER_FAIL = 2,
#        APP = 3,        //Normal Application Message
#        KILL = 4,       //Message To Kill this GLR Process
#        CLOSED = 5,     //Resource bind to this GLR Process closed
#    };
#    uint32_t Len;  //Message Length not including this head structure
#    unsigned char  Type; //Message Type
#    int32_t GPid; // dest_GPid 
#typedef struct{
#    char   Host[20];              // Host
#    uint32_t   Port;              // Port
#    uint32_t   Gpid;    
#}GLR_ADDR;
#struct GLR_BUS_HEAD 
#{   
#    MSG_HEAD Head;
#    GLR_ADDR Source; 
#};  

MSG_TYPE={
        "REGISTER" : 0,
        "REGISTER_OK" : 1,
        "REGISTER_FAIL" : 2,
        "APP" : 3,
        "KILL": 4, 
        "CLOSED" : 5, 
}


MONITOR_ACTION={
    "REG":0,      #ask for services list
    "GET":1,
    "PUT":2,
    "DEL":3,
}

MONITOR_TYPE={
    "ALL":0,
    "AMQ":1,
    "ROUTER":2,
    "NODE":3,
}

#typedef struct
#{
#    char Name[20];
#    char Field[20];
#    char AppType[20];
#}ROUTER_ARG;

#typedef struct
#{
#    MSG_ID_TYPE     MsgId;             // MSG ID
#    unsigned char Action;//MONITOR_ACTION  
#    unsigned char Type;   //MONITOR_TYPE    
#}MONITOR_HEADER;

