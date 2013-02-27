#include <GalaxyMQ.hpp>

using namespace Galaxy::AMQ; 

class CSendBlock : public IBlock
{
private:
    BYTE    _Buffer[2];
public:
    CSendBlock()
    {
        RAWEntry()[0] = 0xFC;
        RAWEntry()[1] = 0; 
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
    };
};


		typedef struct
		{
			BYTE	_Provider[24];
			BYTE	_Consumer[24]; 
			BYTE	_Licensed[16];
			UINT	_MaxPages;
			UINT	_MaxPageSize; 
			UINT	_MaxNQs;
			UINT	_MaxDQs; 
			time_t	_DateLimit;
			UINT	_CheckSum; 
		}MQLINENSE,*PMQLINENSE;

int main(// INT argc,char *argv[],char *envp[]
    )
{
    try
    { 
        printf("%lu\n",sizeof(MQLINENSE));
        
        CHAR    szPath[8194];
        snprintf(szPath,sizeof(szPath),"%s/mem/Q.DAT",getenv("HOME"));
        CGalaxyMQ _MQ(szPath);
        _MQ.InstanceOfSuite().BKD(0xFF424B44,0);
        //CGalaxyMQ _MQ(0x5253b);
        /*
        _MQ.DQArray()[0].Reset();

        CSendBlock _BLK;
        
        UINT iLen,Id;

        Id = _MQ.DQArray()[0].Put(IMQueue::MQC_RTL,_BLK,iLen); 
        */
    }
    catch(std::exception &e)
    {
        printf("%s",e.what());
        fflush(stdout);
    }
}

