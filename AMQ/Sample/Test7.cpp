#include <GalaxyMQ.hpp>

using namespace Galaxy::AMQ;

class CRecvBlock : public IBlock
{
private:
    BYTE    _Buffer[8192];
public:
    CRecvBlock() {};
    ~CRecvBlock() {};
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
        
        CRecvBlock _BLK;
        
        UINT iLen,Id;
        UINT i;
        PBYTE pData = _BLK.RAWEntry();

        for(i=0;i<0xFFFFFFFF;i++)
        {
            Id = _NQ0.Get(IMQueue::MQC_BAT,_BLK,iLen);
            
            /*	
            {
                printf("[ %d ]:<%d> RT GET By %d: LEN[%d] TXT[%s] Done!\n",getpid(),i,(Id & 0x0FFFFFFF),iLen,_BLK.RAWEntry());
                fflush(stdout); 	
            }
            */
            //_OUT.Print("[ %d ]:<%d> RT GET By %d: LEN[%d] TXT[%s] Done!\n",getpid(),i,(Id & 0x0FFFFFFF),iLen,_BLK.RAWEntry());
            //_OUT.Flush(); 
            
            if(pData[0] == 0xFC)
            {
                printf("[%d]: Exit!\n",getpid());
                fflush(stdout); 
                break;  
            } 

            if((i%100000)==0)
            {
                printf("[%d]: GET: %d Done!\n",getpid(),i);
                fflush(stdout); 
            }   
        }
        
        printf("[%d]: GET: %d Done!\n",getpid(),i);
        fflush(stdout);
    }
    catch(std::exception &e)
    {
        printf("%s",e.what());
        fflush(stdout);
    }
}

