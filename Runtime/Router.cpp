#include "Router.hpp"

using namespace Galaxy::GalaxyRT;
Router::Router(const std::string &path)
{
    if (path.size()==0)
    {
        THROW_EXCEPTION(1,"the paramter is NULL");
    }

    char szTemp[1024]={0};
    CRT_snprintf(szTemp,sizeof(szTemp),"%s/tmp",CRT_getenv("HOME"));
    if(CRT_access(szTemp,F_OK) != 0)
    {   
        if(CRT_mkdir(szTemp,0755) != 0)
        {   
            THROW_EXCEPTION(1,"the dir of "+std::string(szTemp)+" not exist,but try to create failure");
        }   
    }
    char szKey[1024]={0};
    CRT_snprintf(szKey,sizeof(szKey),"%s/%s",szTemp,path.c_str());

    CFile _File((CPSTR)szKey,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    _File.Ftruncate(MAX_MEM);

    _FileMapp.reset(new Galaxy::GalaxyRT::CFileMapping(&_File, MAX_MEM, PROT_READ|PROT_WRITE));
    if(_FileMapp.get() == NULL)
    {
        THROW_SYSERROR();
    }

    if (!Initialized()) 
    {
        Init();
    }
    pHeader _pRouter =  (pHeader)_FileMapp->Get();   
    _pRawMutex.reset(new CRawMutex(&_pRouter->Mutex));
    printf("%s   %d :router inited\n",__func__,__LINE__);
}

Item *Router::GetItem(int index=0)
{
    if (index<MAX_ITEMS);
    {
        Item *ItemArray=(Item *) ((char *)_FileMapp->Get() + sizeof(Header));
        return ItemArray+index;
    }

    return NULL;
}

void Router::Init(void)
{
    pHeader _pRouter =  (pHeader)_FileMapp->Get();

    _pRouter->Magic[0] = 'A';
    _pRouter->Magic[1] = 'B';
    _pRouter->Magic[2] = 'C';
    _pRouter->Magic[3] = 'D';
    
    pthread_mutexattr_t mattr;
    if (!(pthread_mutexattr_init(&mattr) == 0 && pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED) == 0))
    {
        pthread_mutexattr_destroy(&mattr);
        THROW_EXCEPTION_EX("Pthread Mutex Attr Init Failure");
    }
    if (pthread_mutex_init(&_pRouter->Mutex,&mattr)!= 0)
    {
        THROW_EXCEPTION_EX("Pthread Mutex Init Failure");
    }   

    pthread_mutexattr_destroy(&mattr);

    
    _pRouter->Len=MAX_MEM-sizeof(Header);
}

bool Router::Initialized(void)
{
    pHeader _pRouter =  (pHeader)_FileMapp->Get();

    if(_pRouter->Magic[0] != 'A' || _pRouter->Magic[1] != 'B'
        || _pRouter->Magic[2] != 'C' || _pRouter->Magic[3] != 'D')
    {
        return false;
    }

    return true;
}

Router::~Router()
{

}
void Router::Register(const std::string &name,const std::string &field,const std::string &host,
                    int port,int gpid,const std::string &devtype,const std::string &apptype,const std::string &userdata)
{
    CLockGuard _Gl(_pRawMutex.get());

    Item *pItem = _FindByName(name);
    int i=0;
    if (pItem==NULL)
    {
        for (;i<MAX_ITEMS;++i)
        {
            pItem=GetItem(i);
            if(pItem==NULL)
            {
                THROW_EXCEPTION(1,"You should not be here");
            }   
            
            if (!pItem->Usable)
            {
                break;
            }
        }
    }   
    if (i==MAX_ITEMS)
    {
        THROW_EXCEPTION(1,"items is full");
    }

//Ensure the terminating Zero
#define safe_strncpy(dest,src,n)  \
    do\
    {\
        strncpy(dest,src,n);\
        dest[(n)-1]=0;\
    }while(0);

    std::cout <<pItem<<std::endl;

    // FIXME:: ensure the terminating \0 ?
    safe_strncpy(pItem->Name,name.c_str(),sizeof(pItem->Name));

    safe_strncpy(pItem->Field,field.c_str(),sizeof(pItem->Field));
    safe_strncpy(pItem->Addr.Host,host.c_str(),sizeof(pItem->Addr.Host));
    safe_strncpy(pItem->DeviceType,devtype.c_str(),sizeof(pItem->DeviceType));
    safe_strncpy(pItem->AppType,apptype.c_str(),sizeof(pItem->AppType));

    safe_strncpy(pItem->UserData,userdata.c_str(),sizeof(pItem->UserData));

    pItem->Addr.Port=port;
    pItem->Addr.Gpid=gpid;

    printf("%s  %d  %s\n",__func__,__LINE__,name.c_str()); 
    pItem->Usable=1;
}

void Router::Delete(const std::string &name)
{
    //pHeader _pRouter =  (pHeader)_FileMapp->Get();   
    //CRawMutex _M(&_pRouter->Mutex);
    //CLockGuard _Gl(&_M);
    CLockGuard _Gl(_pRawMutex.get());

    Item *pItem=NULL;
    for (int i=0;i<MAX_ITEMS;++i)
    {
        pItem=GetItem(i);
        if(pItem==NULL)
        {
            THROW_EXCEPTION(1,"You should not be here");
        }   
               
        if (pItem->Usable)
        {
            printf("%s   %d  %s\n",__func__,__LINE__,pItem->Name);
            if (strncmp(pItem->Name,name.c_str(),20)==0)
            {
                pItem->Usable=0;    //enough?
            }
        }       
    }
}

Item *Router::_FindByName(const std::string &name)
{
    Item *pItem=NULL;
    int i=0;
    for (;i<MAX_ITEMS;++i)
    {
        pItem=GetItem(i);
        if(pItem==NULL)
        {
            THROW_EXCEPTION(1,"You should not be here");
        }   
               
        if (pItem->Usable)
        {
            //just compare "len=(sizeof(pItem->Name)-1)" chars,because the pItem->Name[len] is always '\0'
            if (strncmp(pItem->Name,name.c_str(),sizeof(pItem->Name)-1)==0)
            {
                break;
            }
        }
    }
    return (i<MAX_ITEMS) ? pItem:NULL;
}

Item Router::FindByName(const std::string &name)
{
   // pHeader _pRouter =  (pHeader)_FileMapp->Get();   
   // CRawMutex _M(&_pRouter->Mutex);
   // CLockGuard _Gl(&_M);
    CLockGuard _Gl(_pRawMutex.get());

    Item *pItem=NULL;

    pItem=_FindByName(name);
    if (pItem==NULL)
    {
        //Not found
        THROW_EXCEPTION(1,"Not found");    
    }
    return *pItem;
}

std::vector<Item> Router::FindByField(const std::string &field)
{
//    pHeader _pRouter =  (pHeader)_FileMapp->Get();   
//    CRawMutex _M(&_pRouter->Mutex);
//    CLockGuard _Gl(&_M);
    CLockGuard _Gl(_pRawMutex.get());
    std::vector<Item> ret;

    Item *pItem=NULL;
    for (int i=0;i<MAX_ITEMS;++i)
    {
        pItem=GetItem(i);
        if(pItem==NULL)
        {
            THROW_EXCEPTION(1,"You should not be here");
        }   
               
        if (pItem->Usable)
        {
            if (strncmp(pItem->Field,field.c_str(),sizeof(pItem->Field))==0)
            {
                ret.push_back(*pItem);
            }
        }       
    }
    return ret;
}

std::vector<Item> Router::FindByDevType(const std::string &dev_type)
{
    CLockGuard _Gl(_pRawMutex.get());
    std::vector<Item> ret;

    Item *pItem=NULL;
    for (int i=0;i<MAX_ITEMS;++i)
    {
        pItem=GetItem(i);
        if(pItem==NULL)
        {
            THROW_EXCEPTION(1,"You should not be here");
        }   
               
        if (pItem->Usable)
        {
            if (strncmp(pItem->DeviceType,dev_type.c_str(),sizeof(pItem->DeviceType))==0)
            {
                ret.push_back(*pItem);
            }
        }       
    }
    return ret;
}

std::vector<Item> Router::FindByAppType(const std::string &field)
{
    CLockGuard _Gl(_pRawMutex.get());
    std::vector<Item> ret;

    Item *pItem=NULL;
    for (int i=0;i<MAX_ITEMS;++i)
    {
        pItem=GetItem(i);
        if(pItem==NULL)
        {
            THROW_EXCEPTION(1,"You should not be here");
        }   
               
        if (pItem->Usable)
        {
            if (strncmp(pItem->AppType,field.c_str(),sizeof(pItem->AppType))==0)
            {
                ret.push_back(*pItem);
            }
        }       
    }
    return ret;
}

std::vector<Item> Router::Find(const std::string &name,const std::string &field,const std::string &app_type,const std::string &dev_type)
{
    std::vector<Item> ret;
    Item *pItem=NULL;
    if (strlen(name.c_str())!=0) 
    {
        ret.push_back(FindByName(name));
        return ret;
    }

    if (strlen(field.c_str())!=0) 
    {
        return FindByField(field);
    }

    if (strlen(app_type.c_str())!=0) 
    {
        return FindByAppType(app_type);
    }

    if (strlen(dev_type.c_str())!=0) 
    {
        return FindByAppType(dev_type);
    }

    //return all usable items
    CLockGuard _Gl(_pRawMutex.get());

    for (int i=0;i<MAX_ITEMS;++i)
    {
        pItem=GetItem(i);
        if(pItem==NULL)
        {
            THROW_EXCEPTION(1,"You should not be here");
        }   
               
        if (pItem->Usable)
        {
            ret.push_back(*pItem);
        }       
    }
    return ret;
}
