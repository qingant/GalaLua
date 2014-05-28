#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Include/AMQ.hpp"

using namespace Galaxy::AMQ; 

typedef struct
{
	UINT _Size;
	PSTR _Buffer;
}STBLOCK;

PSTR RecvCallBack(UINT _Length,STBLOCK &_Block)
{
	if(_Block._Buffer==NULL)
	{
		_Block._Size = _Length+8;
		_Block._Buffer = new char[_Block._Size];
		
	}
	else
	{
		if(_Block._Size < _Length)
		{
			delete [] _Block._Buffer;
			
			_Block._Size = _Length+8;
			_Block._Buffer = new char[_Block._Size];
		}
	}
	
	return _Block._Buffer ;
}

int main(INT argc,char *argv[],char *envp[])
{
    try
    { 
		if(argc >1)
		{
			UINT _Len  = strlen(argv[1]);
			memset(argv[1],0,_Len);
		}
    	
    	STBLOCK _Block = {0,NULL};
    	memset(&_Block,0,sizeof(_Block));
    	//CHAR szBuffer[1024];
    	CHAR szFile[256];
    	sprintf(szFile,"%s/tmp/TQ.dat",getenv("HOME"));
    	
		CGalaxyMQ	_MQ(szFile);
		
		const CSQSuite &_QSuite = _MQ[0];
		
		for(UINT i=0;;)
		{
			//_QSuite.Get(szBuffer,sizeof(szBuffer));
			
			_QSuite.GetCB((RTN_RECVCALLBACK)RecvCallBack,&_Block);

			printf("%s\n",_Block._Buffer);
			fflush(stdout);
			
			i++;
			
			if(argc >1)
			{
				sprintf(argv[1],"[%u]",i);
			}
		}
    	
 
    }
    catch(IGalaxyException &e)
    {
       	printf("%s\n",e.what());
    }
}

