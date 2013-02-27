#include <GalaxyMQ.hpp>

using namespace Galaxy::AMQ; 

int main(INT argc,char *argv[],char *envp[])
{
    try
    { 
        CHAR    szPath[8194];
        
        //printf("pthread_cond_t = %ld\n",sizeof(pthread_cond_t));
        //printf("pthread_mutex_t = %ld\n",sizeof(pthread_mutex_t));
        
        snprintf(szPath,sizeof(szPath),"%s/mem/Q.DAT",getenv("HOME"));
        
        CGalaxyMQCreator _Creator0(std::string(szPath),std::string("内部测试"),8192,8192,1024,10,0,2012,12,1);
        CGalaxyMQCreator _Creator1(0x5253b,std::string("内部测试"),8192,8192,1024,10,0,2012,12,1);
 
    }
    catch(std::exception &e)
    {
       	printf("%s",e.what());
    }
}

