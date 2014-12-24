#include "AMQController.hpp"

using namespace GLR;

static PSTR RecvCallBack(UINT _Length,BlockBuffer *_Block)
{
    _Block->resize(_Length);

    return _Block->get();
}

AMQWorker::AMQWorker(const Galaxy::AMQ::CSQSuite &nq)
    :m_nq(nq)
{

}

AMQWorker::~AMQWorker()
{

}

void *AMQWorker::Run(const Galaxy::GalaxyRT::CThread &t)
{
    (void)t;

    BlockBuffer _Block;

    char *pData =NULL;
    while (true)
    {
        m_nq.GetCB((Galaxy::AMQ::RTN_RECVCALLBACK)RecvCallBack,&_Block);

        assert(_Block.size()>=sizeof(AMQHeader));

        Runtime::GetInstance().GetBus().Send(((AMQHeader *)_Block.get())->_Route._ToGpid, _Block.get_string());
    }
}

void AMQWorker::Clean(const Galaxy::GalaxyRT::CThread &)
{

}

void AMQController::Count(lua_State *l)
{
    int port=luaL_checkinteger(l,3);
    const Galaxy::AMQ::CSQSuite &nq=m_amq[port];
    UINT count = nq.Count();

    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    Runtime::GetInstance().GetBus().Return(pid, 1,LUA_TNUMBER, count);
}

void AMQController::Get(lua_State *l)
{
    int port=luaL_checkinteger(l,3);

    BlockBuffer _Block;
    const Galaxy::AMQ::CSQSuite &nq=m_amq[port];

    nq.GetCB((Galaxy::AMQ::RTN_RECVCALLBACK)RecvCallBack,&_Block);

    assert(_Block.size()>=sizeof(AMQHeader));

    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);

    Runtime::GetInstance().GetBus().Return(pid, 3,(GLRPROTOCOL *)(_Block.get()));
}

void AMQController::Put(lua_State *l)
{
    //XXX:必须先处理可选参数，避免栈被修改。
    int corrid=0;
    int msgid=0;
    if (lua_gettop(l) == 5 && lua_istable(l, -1))
    {
        lua_getfield(l, 5, "msgid");
        if (!lua_isnil(l, -1))
        {
            msgid = luaL_checkinteger(l, -1);
        }
        lua_pop(l,1);

        lua_getfield(l, 5, "corrid");
        corrid = luaL_checkinteger(l, -1);
        lua_pop(l,1);
    }

    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);

    lua_getfield(l, 3, "port");
    int port = luaL_checkinteger(l, -1);

    lua_getfield(l, 3, "gpid");
    int des_pid = luaL_checkinteger(l, -1);

    const char *host = "AMQ";



    try
    {
        size_t len=0;
        const char *msg=luaL_checklstring(l,4, &len);
        
        std::string buf(sizeof(AMQHeader)+len,0);

        AMQHeader *header=(AMQHeader *)&buf[0];
        header->_Protocol._Type=AMQHeader::APP;
        header->_Route._FromGpid=pid;
        header->_Route._ToGpid=des_pid;
        header->_Msg._CorrId = corrid;
        header->_Msg._MsgId = msgid;
        CRT_time((time_t*)&header->_Protocol._Stamp);

        memcpy(header->_Host._V2._Host,host,strlen(host));
        header->_Host._V2._Port=m_queue;

        header->_Protocol._Length=(sizeof(AMQHeader)-4)+ len;
        
        memcpy(&buf[sizeof(AMQHeader)], msg, len);

        const Galaxy::AMQ::CSQSuite &nq=m_amq[port];
        
        nq.Put(buf.c_str(),buf.length());

        lua_newtable(l);
        lua_pushnumber(l,corrid);
        lua_setfield(l,-2,"corrid");

        lua_pushnumber(l,msgid);
        lua_setfield(l,-2,"msgid");

        Runtime::GetInstance().GetBus().Return2(pid, 1);
    }
    catch (IGalaxyException &e)
    {
        const char *errmsg=e.what();
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg,strlen(errmsg));
        return;
    }

}

void AMQController::SetQueue(lua_State *l)
{
    int queue=luaL_checkinteger(l,3);
    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    try
    {
        StartWorker(queue);
    }
    catch (IGalaxyException &e)
    {
        const char *errmsg=e.what();
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg,strlen(errmsg));
        return;
    }
    catch (const Galaxy::GalaxyRT::CException &e)
    {
        const char *errmsg=e.what();
        Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, errmsg,strlen(errmsg));
        return;
    }
    Runtime::GetInstance().GetBus().Return(pid, 1, LUA_TBOOLEAN, 1);
}

void AMQController::InvalidType(lua_State *l)
{ 
    (void) l;

    lua_getglobal(l,"__id__");
    int pid = luaL_checkinteger(l,-1);
    const char *err="not supported";
    Runtime::GetInstance().GetBus().Return(pid, 2, LUA_TNIL, LUA_TSTRING, err, strlen(err));
}

void AMQController::Request( lua_State *l)
{
    int type = luaL_checkinteger(l, 2);
    //Process &nd = Runtime::GetInstance().GetProcess(pid);
    switch ((AMQ_CMD)type)
    {
    case AMQ_PUT:
        Put(l);
        break;
    case AMQ_GET:
        Get(l);
        break;
    case AMQ_SET_QUEUE:
        SetQueue(l);
        break;
    case AMQ_COUNT:
        Count(l);
        break;
    default:
        InvalidType(l);
        break;
    }
}

void AMQController::StartWorker(int queue)
{
    
    if (ValidQueue(m_queue))
    {
        //TODO:支持改变私有队列号
        THROW_EXCEPTION_EX("private queue had set");
    }
    if (!ValidQueue(queue))
    {
        THROW_EXCEPTION_EX("invalid private queue");
    }

    m_queue=queue;
    m_worker=new AMQWorker(m_amq[m_queue]);
    m_thread = new Galaxy::GalaxyRT::CThread(*m_worker, 0x1234);
    m_thread->Start();
}

AMQController::AMQController(const std::string &path,int queueno)
    :m_amq_path(path),
    m_queue(INVALID_QUEUE),
    m_amq(path),
    m_worker(NULL)
{
    if (ValidQueue(queueno))
    {
        StartWorker(queueno);
    }
    DeviceId(3);
    
}

AMQController::~AMQController()
{
    //TODO:应该退出线程，释放内存
}

IController *Initialize(void *arg)
{
    lua_State *l=(lua_State *)arg;

    const char *path=luaL_checkstring(l,2);
    //XXX: 大小不能超过INT_MAX
    int queue=luaL_checkint(l,3);

    if (access(path, F_OK) != 0)
    {
        int queues=luaL_optint(l,4,20);
        int max_depth=luaL_optint(l,5,64);
        int catch_count=luaL_optint(l,6,4);
        int nodes=luaL_optint(l,7,8192);
        int pages=luaL_optint(l,8,16384);
        int page_size=luaL_optint(l,9,4096);
        Galaxy::AMQ::CGalaxyMQCreator _Creator(path,queues,max_depth,catch_count,nodes,pages,page_size);
    }

    AMQController *amq=NULL;
    try
    {
        amq=new AMQController(path,queue);
    }
    catch (IGalaxyException &e)
    {
        //TODO: 应该与GLR用同一异常类
        THROW_EXCEPTION_EX(e.what());
    }

    return amq;
}

void Finalize(void *arg)
{
    (void) arg;
}

