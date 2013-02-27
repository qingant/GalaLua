
#include "MQBlock.hpp"
#include "MQException.hpp"

using namespace Galaxy::AMQ;
	
/*CMQRefBlock*/
CMQRefBlock::CMQRefBlock(PBYTE _TheBuffer,UINT _TheSize)
	:_Buffer(_TheBuffer),_Size(_TheSize)
{
	
}

CMQRefBlock::~CMQRefBlock()
{
	
}

PBYTE CMQRefBlock::RAWEntry() const 
{
	if(ISNULL(_Buffer))
	{
		THROW_MQEXCEPTION("C-API:Invalid Block Ref.!");		
	}
	
	return (PBYTE)_Buffer;    
}

UINT CMQRefBlock::RAWSize() const
{
	return _Size;    
}

			
/*CStrBlock*/
CStrBlock::CStrBlock( std::string &str)
   :_Block( str )
{
   
}

CStrBlock::~CStrBlock(  )
{
   
}

CStrBlock::CStrBlock( const CStrBlock & rhs)
    :IBlock(), _Block( rhs._Block )
{

}
PBYTE CStrBlock::RAWEntry() const
{
   return ( PBYTE )&_Block[ 0 ];
}


UINT CStrBlock::RAWSize() const
{
   return _Block.size(  );
}
