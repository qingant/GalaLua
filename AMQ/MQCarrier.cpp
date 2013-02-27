#include "MQCarrier.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;
	
	
/*CMQCarrier*/	
CMQCarrier::CMQCarrier(PBYTE _D,CPBYTE _S,UINT _Length)
{
	if(ISNULL(_D)||ISNULL(_S))
	{
		THROW_MQEXCEPTION("_S or _D is NULL");	
		return;
	}

#define _AMQCOPY4_
	
#if defined(_AMQCOPY4_)

	UINT *_iD = (UINT *)_D; 
	UINT *_iS = (UINT *)_S; 
	
	while(_Length > 3)
	{
		*_iD++ = ~(*_iS++);
		_Length -= sizeof(UINT);
		
		__asm__ __volatile__("nop;"); 
	}
	
	USHORT *_SD = (USHORT *)_iD; 
	USHORT *_SS = (USHORT *)_iS; 
	
	if(_Length > 1)
	{

		*_SD++ = ~(*_SS++);
		_Length -= sizeof(USHORT); 
	}
	
	if(_Length > 0)
	{
		*((PBYTE)_SD) = ~(*((PBYTE)_SS));
	}
	
#else
	

	USHORT *_SD = (USHORT *)_D; 
	USHORT *_SS = (USHORT *)_S; 

	while(_Length > 1)
	{
		*_SD++ = ~(*_SS++);
		_Length -= sizeof(USHORT); 
		__asm__ __volatile__("nop;");
	}
	
	if(_Length > 0)
	{
		*((PBYTE)_SD) = ~(*((PBYTE)_SS));
	}

	
#endif

}

CMQCarrier::~CMQCarrier()
{
	
}

