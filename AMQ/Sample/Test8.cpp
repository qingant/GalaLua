#include <GalaxyMQ.hpp>

using namespace Galaxy::AMQ; 

class CSendBlock : public IBlock
{
private:
    BYTE    _Buffer[8192];
public:
    CSendBlock()
    {
        memset((PSTR)RAWEntry(),'Y',RAWSize());
        RAWEntry()[0] = '[';
        RAWEntry()[RAWSize()-1] = 0; 
        RAWEntry()[RAWSize()-2] = ']';  
        //strcpy((PSTR)RAWEntry(),"我在测试MQ数据！"); 
    };
    ~CSendBlock()
    {
    
    };
    PBYTE RAWEntry() const 
    {
        return (PBYTE)_Buffer;    
    };
    
    UINT RAWSize() const
    {
        return sizeof(_Buffer);    
        //return 17; 
    };
};

class CSendBlock1 : public IBlock
{
private:
    BYTE    _Buffer[2];
public:
    CSendBlock1()
    {
        RAWEntry()[0] = 0xFC;
        RAWEntry()[1] = 0; 
    };
    ~CSendBlock1()
    {
    
    };
    PBYTE RAWEntry() const 
    {
        return (PBYTE)_Buffer;    
    };
    
    UINT RAWSize() const
    {
        return sizeof(_Buffer);    
    };
};

int main(// INT argc,char *argv[],char *envp[]
    )
{
    try
    { 
        CHAR    szPath[8194];
        snprintf(szPath,sizeof(szPath),"%s/mem/Q.DAT",getenv("HOME"));
        CGalaxyMQ _MQ(szPath);
        //CGalaxyMQ _MQ(0x5253b);
        const IMQueue &_NQ0 = _MQ.DQArray()[0];
      
        CSendBlock 	_BLK;
        
        UINT iLen,Id;
        UINT i;
  
  
        for(i =0;i<10000000;i++)
        {
            Id = _NQ0.Put(IMQueue::MQC_RTL,_BLK,iLen);

            if((i%100000)==0)
            {
                printf("[%d]: PUT: %d Done!\n",getpid(),i);
                fflush(stdout);
            } 
            /*
            else
            {
                printf("[%d]: PUT: %d Done!\n",getpid(),i);
                fflush(stdout);
            } 
            */
        }
        
        CSendBlock1 _BLKEXIT;
        Id = _NQ0.Put(IMQueue::MQC_RTL,_BLKEXIT,iLen);
        
        printf("[%d]: PUT: %d Done!\n",getpid(),i);
        fflush(stdout);  
    }
    catch(std::exception &e)
    {
        printf("%s",e.what());
        fflush(stdout);
    }
}

