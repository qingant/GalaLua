#include "stdafx.h"
#include "lua.hpp"
#include <vector>
#include <string>
#include <exception>
#ifdef _AIX
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#include "Runtime/Queue.hpp"
#include "Runtime/Utils.hpp"
#include "GLR.hpp"
#pragma once

namespace GLR
{

    typedef std::string LN_MSG_TYPE;
    typedef int32_t     LN_ID_TYPE;
    typedef Galaxy::GalaxyRT::_TypeDef_FIFO<std::string>::CFIFOQueue LN_CHL_TYPE;

    class Schedule;
    class Globals
    {

    public:
        struct UserData
        {
            void *Content;
            std::string Name;
            UserData():Content(NULL), Name(32,0){}
        };
        typedef Galaxy::GalaxyRT::ConcurrentMap<std::string, UserData> VARMAP;
    public:
        Globals();
        ~Globals();
        void Put(const std::string &, void**, const std::string &);
        const UserData &Get(const std::string &);
    private:
        VARMAP _VarMap;
    };
    class Process
    {
    public:
        friend class Bus;
        //friend class IController;
        struct ProcessStatus
        {
            enum STATE
            {
                CREATED,      // Created
                STOPED,       // Stopped
                RUNNING,      // Running ByteCode
                YIELD,        // Not Running, Waiting for Scheduled
                SUSPEND,      //
                RECV_WAIT,    // On glr.recv call while no msg available
                RECV_RETURN,
                INT_WAIT,     // On Interrupt, wait for wake up
                INT_RESP,
                GLR_CALL,     // trap in glr system call
                KILLED,
            };
            STATE _State;
            STATE State() const { return _State; }
            void State(STATE val) { _State = val; }
            time_t _Start;
            time_t _LastOp;
            int    _Tick;
            int    _NArg;
            bool   _Killed;
            ProcessStatus():_NArg(0)
            {
                memset(this, 0, sizeof(*this));
                _Killed = false;
            }

        };

        enum INTRRUPT_TYPE
        {
            EXIT,
            TIMER,
            SOCKET,
        };
        static const int SpyerId = 1;
    private:
        Process(int id = 0);
        void InitNode(void);
    public:
        static void Status();
    private:
        //TODO: use concurrent map instead
        static std::vector<Process*> NodeMap;

        static Galaxy::GalaxyRT::CRWLock  ProcessMapLock;
        static int32_t NodeId;
        static int32_t NodeCount;     //现在活动中的进程数
    public:
        static Globals GlobalVars;
    public:
        // Built In Ops
        static int Spawn(lua_State *l);
        static int SendMsgToNode(lua_State *l);
        static int Recieve(lua_State *l);
        static int Debug(lua_State *l);
        static void Preempt(lua_State *l, lua_Debug *ar);
        static int Interrupt(lua_State *l);
        static int Status(lua_State *l);
        static int AllProcesses(lua_State *l);
        static int RegisterGlobal(lua_State *l);
        static int GetGlobal(lua_State *l);
        static int GetNodeAddr(lua_State *l);
        static int GetFilePath(lua_State *l);
        static int Kill(lua_State *l);
        static int SetOptions(lua_State *l);
        static int GetOption(lua_State *l);
        static int MessageAvailable(lua_State *l);
        static int GLRStamp(lua_State *l);
        static int Exit(lua_State *l);
    public:
        static LN_ID_TYPE CreateNode();
        static LN_ID_TYPE CreateNode(int);
        static Process &GetNodeById(LN_ID_TYPE);
        static bool GetNodeExceptionHandle(LN_ID_TYPE);
        static void Destory(LN_ID_TYPE);
        static void SendMsgToNode(LN_ID_TYPE,
            const std::string &,
            MSG_HEAD::MSG_TYPE type = MSG_HEAD::APP);
        static void CreateSpyer();

    public:
        void DoString(const std::string&);
        void Entry(const std::string &module, const std::string &entry, ...);
        void EntryGar(const std::string &Gar,const std::string &module, const std::string &entry, ... );
        void LoadFile(const std::string &path);
        void LoadString(const std::string &path);
        void PushFun(const std::string &fname);
        void SendMsg(const LN_MSG_TYPE &);
        LN_MSG_TYPE RecvMsg();
        void Resume();
        void Interrupt(int);
        void Start(Schedule&);
        int Yield();
        ProcessStatus::STATE State() const;
        void Terminal();

        virtual ~Process(void);
    private:
        void SendExitMsg();
    public:
        void StackDump();
        int32_t Id() const { return _Id; }
    public:
        lua_State * Stack() const { return _Stack; }
    private:
        lua_State *_Stack;

        LN_CHL_TYPE _Channel;
        Galaxy::GalaxyRT::CPthreadMutex              _Lock;
        Galaxy::GalaxyRT::CPthreadMutex              _IntLock;   //Interrupt Lock
        //std::condition_variable _Cond;
        int32_t                 _Id;
        int32_t                 _ParentId;                       //_ParentId 父进程Id
        void Id(int32_t val) { _Id = val; }
        std::string   _Path;
    public:
        ProcessStatus _Status;
    };
};
