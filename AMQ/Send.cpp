#include <stdio.h>
#include <stdlib.h>

#include "Include/AMQ.hpp"

using namespace Galaxy::AMQ; 

int main(INT argc,char *argv[],char *envp[])
{
    try
    { 
    	CHAR szFile[256];
    	sprintf(szFile,"%s/tmp/TQ.dat",getenv("HOME"));
    	
		CGalaxyMQ	_MQ(szFile);
		
		const CSQSuite &_QSuite = _MQ[0];
		
		for(UINT i=0;i<1000000;i++)
		{
			_QSuite.Put("TestTheMQSend-11111111111111111111111111111111111111111111111111111111@@@@@!",77);
    	}
 
    }
    catch(IGalaxyException &e)
    {
       	printf("%s\n",e.what());
    }
}

