#include <vector>
#include <string.h>
#include "Runtime/File.hpp"
#include "Runtime/Sync.hpp"

    typedef struct
    {
        char Magic[4];
        pthread_mutex_t Mutex;
        int Len;
    }Header,*pHeader;

    typedef struct 
    {
        char Usable;
        char Name[24];
        char Field[24];
        struct
        {
            char Host[32];
            int Port;
            int Gpid;
        }Addr;
        char DeviceType[24];
        char AppType[24];
    }Item;

    const int MAX_ITEMS=1000;
    const int MAX_MEM=MAX_ITEMS*sizeof(Item)+sizeof(Header);

    class Router:public Galaxy::GalaxyRT::CNonCopyAble
    {
    public: 
        Router(const std::string &path);
        ~Router();
        void Register(const std::string &name,const std::string &field,const std::string &host,int port,int gpid,const std::string &dev,const std::string &app);
        void Delete(const std::string &name);
        Item FindByName(const std::string &);
        std::vector<Item> FindByField(const std::string &);
    private:
        bool Initialized(void);
        Item *_FindByName(const std::string &);
        void Init(void);
        Item *GetItem(int index);
    private:
        std::auto_ptr<Galaxy::GalaxyRT::CFileMapping>     _FileMapp;
        std::auto_ptr<Galaxy::GalaxyRT::CRawMutex> _pRawMutex;
    };
