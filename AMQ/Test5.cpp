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
    	
		CGalaxyMQCreator	_Creator(szFile,1024,64,4,8192,16384,64);
    	
 
    }
    catch(IGalaxyException &e)
    {
       	printf("%s\n",e.what());
    }
}

