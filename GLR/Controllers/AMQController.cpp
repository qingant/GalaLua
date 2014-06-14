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

        pData = (char *)_Block.get();

        assert(_Block.size()>=sizeof(AMQHeader));

        //TODO: avoid this copy!
        std::string msg(pData,_Block.size());
        
        Runtime::GetInstance().GetBus().Send(((AMQHeader *)pData)->_Route._ToGpid, msg);
    }
}

void AMQWorker::Clean(const Galaxy::GalaxyRT::CThread &)
{

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

        memcpy(header->_Host._V2._Host,host,strlen(host));
        header->_Host._V2._Port=m_queue;

        header->_Protocol._Length=(sizeof(AMQHeader)-4)+ len;
        
        memcpy(&buf[sizeof(AMQHeader)], msg, len);

        const Galaxy::AMQ::CSQSuite &nq=m_amq[port];
        
        nq.Put(buf.c_str(),buf.length());
    }
    catch (IGalaxyException &e)
    {
        GALA_DEBUG("%s\n",e.what());
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
    default:
        InvalidType(l);
        break;
    }
}

AMQController::AMQController(const std::string &path,short queueno)
    :m_amq_path(path),
     m_queue(queueno),
     m_amq(path),
     m_worker(m_amq[queueno])
{
    DeviceId(3);
    m_thread = new Galaxy::GalaxyRT::CThread(m_worker, 0x1234);
    m_thread->Start();
}

AMQController::~AMQController()
{

}

IController *Initialize(void *arg)
{
    lua_State *l=(lua_State *)arg;

    const char *path=luaL_checkstring(l,2);
    //XXX: 大小不能超过INT_MAX
    int queue=luaL_checkint(l,3);

    /*
    if (access(path.c_str(), F_OK) != 0)
    {
        Galaxy::AMQ::CGalaxyMQCreator _Creator(path,1024,64,4,8192,16384,64);
    }
   */

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

