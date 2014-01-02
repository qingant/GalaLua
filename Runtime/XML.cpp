// XMLReader.cpp: implementation of the CXMLReader class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "XML.hpp"
#include "Exception.hpp"
#include <ctype.h>

namespace Galaxy
{
   namespace GalaxyRT
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLBlader::CXMLBlader(const char *_text,unsigned int length)
 :CByteArray(_text,length)
	{
		Blader();
	}

	CXMLBlader::~CXMLBlader()
	{

	}

	void CXMLBlader::Blader()
	{
		unsigned char *pszSrc;
		unsigned char *pszDes;
		unsigned int    i;
		unsigned int    MaxLen;

		pszSrc = _Get();
		MaxLen = Size();

		if(MaxLen==0)
		{
			while((*pszSrc)!=0)
			{
				++pszSrc;
			}
		}
		else
		{
			for(i=0;(i<MaxLen) && ((*pszSrc)!=0);i++)
			{
				++pszSrc;
			}
		}

		--pszSrc;
		while(pszSrc >= _Get())
		{
			if ((*pszSrc==' ')||(*pszSrc=='\t')||(*pszSrc=='\n')||(*pszSrc==0x0d)||(*pszSrc==0x0a))
			{
				--pszSrc;
				continue;
			}
			else
			{
				pszSrc[1]=0;
				break;
			}
		}

		pszSrc = _Get();
		pszDes = _Get();

		while ( *pszSrc )
		{
			if((*pszSrc==' ')||(*pszSrc=='\t')||(*pszSrc=='\n')||(*pszSrc==0x0d)||(*pszSrc==0x0a))
			{
				++pszSrc;
			}
			else
			{
				break;
			}
		}

		for(i=0;(((*pszDes++)=(*pszSrc++))!=0);i++);
		Length(i);
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLEscape::CXMLEscape(const char *_text,unsigned int length)
 :CByteArray()
	{
		unsigned int i;
		unsigned int _Count;

		if(_text==NULL)
		{
			THROW_EXCEPTION_EX("_Text is null");
			return;
		}

		_Count = 0;
		for(i=0;i<length;i++)
		{
			switch(_text[i])
			{
			case('&'):
			case('>'):
			case('<'):
			case('\''):
			case('\"'):
			case('#'):
			{
				_Count++;
				break;
			}
			}
		}

		Allocate(length + (_Count * 6));

		char *p = (char *)_Get();
		for(i=0;i<length;i++,p++)
		{
			switch(_text[i])
			{
			case('&'):
			{
				memcpy(p,"&amp;",5);
				p+=4;
				break;
			}
			case('>'):
			{
				memcpy(p,"&gt;",4);
				p+=3;
				break;
			}
			case('<'):
			{
				memcpy(p,"&lt;",4);
				p+=3;
				break;
			}
			/*
			  case(' '):
			  {
			  memcpy(p,"&nbsp;",6);
			  p+=5;
			  break;
			  }
			*/
			case('\''):
			{
				memcpy(p,"&apos;",6);
				p+=5;
				break;
			}
			case('\"'):
			{
				memcpy(p,"&quot;",6);
				p+=5;
				break;
			}
			case('#'):
			{
				memcpy(p,"&#35;",5);
				p+=4;
				break;
			}
			default:
			{
				*p = _text[i];
				break;
			}
			}
		}

		Length((unsigned int)(p - (char *)_Get()));

	}

	CXMLEscape::~CXMLEscape()
	{

	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLUnescape::CXMLUnescape(const char *_text,unsigned int length)
 :CByteArray(length)
	{
		if(_text==NULL)
		{
			THROW_EXCEPTION_EX("_Text is null");
			return;
		}

		unsigned int i;
		char *p = (char *)_Get();
		for(i=0;i<length;i++,p++)
		{
			if(_text[i]=='&')
			{
				if(memcmp(&_text[i],"&amp;",5)==0)
				{
					*p = '&';
					i+=4;
				}
				else if(memcmp(&_text[i],"&gt;",4)==0)
				{
					*p = '>';
					i+=3;
				}
				else if(memcmp(&_text[i],"&lt;",4)==0)
				{
					*p = '<';
					i+=3;
				}
				else if(memcmp(&_text[i],"&nbsp;",6)==0)
				{
					*p = ' ';
					i+=5;
				}
				else if(memcmp(&_text[i],"&apos;",6)==0)
				{
					*p = '\'';
					i+=5;
				}
				else if(memcmp(&_text[i],"&quot;",6)==0)
				{
					*p = '\"';
					i+=5;
				}
				else if(memcmp(&_text[i],"&#35;",5)==0)
				{
					*p = '#';
					i+=4;
				}
				else
				{
					THROW_EXCEPTION_EX("Escape error!");
					return;
				}

			}
			else
			{
				*p=_text[i];
			}
		}

		Length((unsigned int)(p - (char *)_Get()));
	}

	CXMLUnescape::~CXMLUnescape()
	{

	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	CXMLBuffer::CXMLBuffer()
		:_Buff(NULL)
	{


	}

	CXMLBuffer::CXMLBuffer(unsigned int _PreAllocate)
		:_Buff(NULL)
	{
		Allocate(_PreAllocate);
	}

	CXMLBuffer::CXMLBuffer(const char *_Data,unsigned int _Length)
	{
		if(_Data == NULL)
		{
			THROW_EXCEPTION_EX("Data Is NULL");
			return;
		}

		Allocate(_Length);

		memcpy(_Get(),_Data,_Length);

	}


	CXMLBuffer::~CXMLBuffer()
	{
		Free();
	}

	void CXMLBuffer::Allocate(unsigned int _Size)
	{
		if(_Size == 0)
		{
			THROW_EXCEPTION_EX("can't new a buffer space with length(0)!");
			return;
		}

		unsigned int _Length = _Size+2+sizeof(unsigned int);

		_Buff = new unsigned char[_Length];
		if(_Buff == NULL)
		{
			THROW_EXCEPTION_EX("new a buffer space error!");
			return;
		}

		memset(_Buff,0,_Length);
		memcpy(_Buff,&_Size,sizeof(unsigned int));
	}

	void CXMLBuffer::Free()
	{
		if(_Buff!=NULL)
		{
			delete [] _Buff;
			_Buff = NULL;
		}
	}

	unsigned int CXMLBuffer::Size() const
	{
		if(_Buff == NULL)
		{
			THROW_EXCEPTION_EX("invalid buffer space!");
			return 0;
		}

		return *((unsigned int *)_Buff);
	}

	unsigned char *CXMLBuffer::_Get() const
	{
		if(_Buff == NULL)
		{
			THROW_EXCEPTION_EX("invalid buffer space!");
			return 0;
		}

		return &_Buff[sizeof(unsigned int)];
	}

	const unsigned char  *CXMLBuffer::Get() const
	{
		return _Get();
	}

	unsigned char CXMLBuffer::operator[](unsigned int index)
	{
		if(index >= Size())
		{
			THROW_EXCEPTION_EX("Index out of bound!");
			return 0;
		}

		return Get()[index];
	}

//CXMLString 
    CXMLString::CXMLString(std::string &str)
        :_XMLStr(str)
    {
    }

    CXMLString::~CXMLString()
    {
    }

    unsigned int CXMLString::Size()
    {
        return _XMLStr.size();
    }
    
    long CXMLString::Seek(long offset, int fromwhere)
    {
        //XXX:get rid of warning
        offset=offset,fromwhere=fromwhere;
        return -1;
    }

    int CXMLString::Read(void *buf ,size_t count)
    {
        //XXX:get rid of warning
        buf=buf,count=count;
        return -1;
    }
    int CXMLString::Write(const void *buf ,size_t count)
    {
        _XMLStr.append((const char *)buf,count);
        return count;
    }
    int CXMLString::Fill(char val,size_t count)
    {
        _XMLStr.append(count,val);
        return count;
    }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLFile::CXMLFile(const char *_filename,bool _create)
 :_fd(-1)
	{
		if(_filename==NULL)
		{
			THROW_EXCEPTION_EX("File path is null!");
			return;
		}

		if(_create)
		{
			_fd = open(_filename,O_WRONLY|O_TRUNC|O_BINARY,0666);
			if(_fd==-1)
			{
				_fd = open(_filename,O_CREAT|O_WRONLY|O_TRUNC|O_BINARY,0666);
			}
		}
		else
		{
			_fd = open(_filename,O_RDONLY|O_BINARY,0666);
		}

		if(_fd==-1)
		{
			THROW_EXCEPTION_EX("Open Path of File fail!");
			return;
		}
	}

	CXMLFile::~CXMLFile()
	{
		if(_fd!=-1)
		{
			close(_fd);
			_fd = -1;
		}
	}


	unsigned int CXMLFile::Size()
	{
		struct stat _FileStat;

		memset(&_FileStat,0,sizeof(_FileStat));
		if(fstat(_fd,&_FileStat)==-1)
		{
			THROW_EXCEPTION_EX("call fstat fail!");
			return 0;
		}

		return (_FileStat.st_size);
	}

	long CXMLFile::Seek(long offset, int fromwhere)
	{
		long _rc;
		if((_rc=lseek(_fd,offset,fromwhere))== (-1L))
		{
			THROW_EXCEPTION_EX("call lseek fail!");
			return (-1L);
		}

		return _rc;
	}

	int CXMLFile::Read(void *buf ,size_t count)
	{
		int _rc;
		if((_rc=read(_fd,buf,count))==-1)
		{
			THROW_EXCEPTION_EX("call read fail!");
			return (-1);
		}

		return _rc;
	}


	int CXMLFile::Write(const void *buf ,size_t count)
	{
		int	_rc;
		if((_rc=write(_fd,buf,count))==-1)
		{
			THROW_EXCEPTION_EX("call write fail!");
			return (-1);
		}
		return _rc;
	}

	int CXMLFile::Fill(char val,size_t count)
	{
		for(size_t i=0;i<count;i++)
		{
			if(Write(&val,1)!=1)
			{
				THROW_EXCEPTION_EX("call Write fail!");
				return (-1);
			}
		}

		return count;
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	CXMLLoder::CXMLLoder(const char *xmlStr,int len)
	{
		LoadFromStr(xmlStr,len);
	}

	CXMLLoder::CXMLLoder(const char *_PathOfFile)
	{
		Load(_PathOfFile);
	}

	CXMLLoder::~CXMLLoder()
	{

	}

	void CXMLLoder::LoadFromStr(const char *xmlStr,int len)
	{
		if(xmlStr == NULL)
		{
			THROW_EXCEPTION_EX("xmlStr NULL!");
			return;
		}
        if (len<=0)
        {
			THROW_EXCEPTION_EX("len should be greater than 0!");
			return;
        }

		unsigned int iLen = len;

		Allocate(iLen);

        CRT_memcpy(_Get(),xmlStr,Size());
	}

	void CXMLLoder::Load(const char *_PathOfFile)
	{
		if(_PathOfFile == NULL)
		{
			THROW_EXCEPTION_EX("_PathOfFile NULL!");
			return;
		}

		CXMLFile	_File(_PathOfFile);
		unsigned int iLen = _File.Size();

		Allocate(iLen);

		_File.Seek(0,SEEK_SET);
		iLen = _File.Read(_Get(),Size());

		if(iLen != Size())
		{
			THROW_EXCEPTION_EX("read file length incorrect!");
			return;
		}
	}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	CXMLParserStream::CXMLParserStream(const CXMLBuffer &TheBuffer)
 :_Buffer(TheBuffer),_Ptr(NULL)
	{
		_Ptr = (char *)_Buffer.Get();
		if(_Ptr==NULL)
		{
			THROW_EXCEPTION_EX("Invalid Buffer!");
			return;
		}
	}

	CXMLParserStream::~CXMLParserStream()
	{
		_Ptr = NULL;
	}

	char *CXMLParserStream::Stream()
	{
		return (this->_Ptr);
	}

	void CXMLParserStream::Stream(char *p)
	{
		if(INRANGE(p))
		{
			this->_Ptr = p;
		}
	}

	void CXMLParserStream::Stream(unsigned int _Step)
	{
		if(INRANGE(this->_Ptr+_Step))
		{
			this->_Ptr += _Step;
		}
	}

	char *CXMLParserStream::Entry()
	{
		return (char *)_Buffer.Get();
	}

	unsigned int CXMLParserStream::Size()
	{
		return _Buffer.Size();
	}


	bool CXMLParserStream::INRANGE(const char *_tmpptr)
	{
		if((_tmpptr) > (char *)(Entry()+Size()))
		{
			return false;
		}
		else if(_tmpptr < (char *)Entry())
		{
			return false;
		}

		return true;
	}

	bool CXMLParserStream::IsSpaceChar(char ch)
	{
		return ((ch == 0x0d)||(ch==0x0a)||(ch==0x09)||(ch==0x20));
	}

	bool CXMLParserStream::IsSpaceChar()
	{
		const char *p = Current();
		if(p==NULL)
		{
			THROW_EXCEPTION_EX("Invalid pointer!");
			return false;
		}

		return IsSpaceChar(*p);
	}
    /*
     * Tag Name must start with letters or '_'
     */
	bool CXMLParserStream::IsAllowedKeyStartChar(char ch)
	{
//		return (((ch >='a') && (ch <='z')) || ((ch >='A') && (ch <='Z'))|| (ch=='_'));
		return ((isalpha(ch)) || (ch =='_') );
	}

	bool CXMLParserStream::IsAllowedKeyStartChar()
	{
		const char *p = Current();
		if(p==NULL)
		{
			THROW_EXCEPTION_EX("Invalid pointer!");
			return false;
		}

		return IsAllowedKeyStartChar(*p);
	}
    
	bool CXMLParserStream::IsAllowedKeyChar(char ch)
	{
//		return (((ch >='a') && (ch <='z')) || ((ch >='A') && (ch <='Z')));
		return ( (IsAllowedKeyStartChar(ch)) || isdigit(ch) || (ch =='-') || (ch =='.') );
	}

	bool CXMLParserStream::IsAllowedKeyChar()
	{
		const char *p = Current();
		if(p==NULL)
		{
			THROW_EXCEPTION_EX("Invalid pointer!");
			return false;
		}

		return IsAllowedKeyChar(*p);
	}
#if 0
	bool CXMLParserStream::IsAllowedKeySymbol(char ch)
	{
		return ((ch =='_') || (ch =='-') || (ch == '.'));
	}

	bool CXMLParserStream::IsAllowedKeySymbol()
	{
		const char *p = Current();
		if(p==NULL)
		{
			THROW_EXCEPTION_EX("Invalid pointer!");
			return false;
		}

		return IsAllowedKeySymbol(*p);
	}

	bool CXMLParserStream::IsDigitalChar(char ch)
	{
		return ((ch >='0') && (ch <='9'));
	}

	bool CXMLParserStream::IsDigitalChar()
	{
		const char *p = Current();
		if(p==NULL)
		{
			THROW_EXCEPTION_EX("Invalid pointer!");
			return false;
		}

		return IsDigitalChar(*p);
	}
#endif //0

	unsigned int CXMLParserStream::Offset()
	{
		const char *p = Current();
		if(p==NULL)
		{
			THROW_EXCEPTION_EX("Invalid pointer!");
			return 0;
		}

		return (unsigned int)(p - (const char *)Entry());

	}

	unsigned int CXMLParserStream::Limit()
	{
		return Size();
	}


	const char *CXMLParserStream::Current()
	{
		if(Stream() > (char *)(Entry()+Size()))
		{
			return NULL;
		}
		else if(Stream() < (char *)Entry())
		{
			return NULL;
		}

		return Stream();
	}

	char CXMLParserStream::GetChar(int Index)
	{
		char	*_VarPtr = Stream();
		return _VarPtr[Index];
	}

	char CXMLParserStream::Next()
	{
		return Step(1);
	}

	char CXMLParserStream::Step(unsigned int _Step)
	{
		if((Stream()+_Step) > (char *)(Entry()+Size()))
		{
			return 0;
		}
		else if(Stream() < (char *)Entry())
		{
			return 0;
		}

		Stream(_Step);

		return (*Stream());
	}

	char CXMLParserStream::FirstNoSpaceChar()
	{
		for(;(Stream()!=NULL) && (*Stream()!=0);Stream(1))
		{
			if(!IsSpaceChar())
			{
				return *Stream();
			}
		}

		return 0;
	}

	bool CXMLParserStream::FirstNoSpaceCharIs(char KeyChar)
	{
		return (FirstNoSpaceChar()==KeyChar);
	}


	bool CXMLParserStream::MatchKeyword(const char *_Keyword)
	{
		char *_pos;
		char *_kw;

		for(_kw = (char *)_Keyword,_pos = Stream(); \
			(_kw!=NULL) && (*_kw!=0); \
			_kw++,_pos++)
		{
			if(*_pos!=*_kw)
			{
				return false;
			}
		}

		if(INRANGE(_pos))
		{
			Stream((char *)_pos);
			return true;
		}
		else
		{
			return false;
		}
	}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLOffset::CXMLOffset()
 :_Offset(0),_Length(0)
	{


	}

	CXMLOffset::CXMLOffset(unsigned int TheOffset,unsigned int TheLength)
 :_Offset(TheOffset),_Length(TheLength)
	{

	}

	CXMLOffset::CXMLOffset(const CXMLOffset &_ofs)
 :_Offset(_ofs.Offset()),_Length(_ofs.Length())
	{

	}


	CXMLOffset::~CXMLOffset()
	{

	}

	void CXMLOffset::Distance(const CXMLOffset &_a,const CXMLOffset &_b)
	{
		if((_a.Offset()+_a.Length()) <= _b.Offset())
		{
			_Offset = _a.Offset()+_a.Length();
			_Length = _b.Offset() - _Offset;
		}
		else if((_b.Offset()+_b.Length()) <= _a.Offset())
		{
			_Offset = _b.Offset()+_b.Length();
			_Length = _a.Offset() - _Offset;
		}
		else
		{
			_Offset = 0;
			_Length = 0;
		}
	}

	unsigned int CXMLOffset::Offset() const
	{
		return _Offset;
	}

	unsigned int CXMLOffset::Length() const
	{
		return _Length;
	}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLCustomValue::CXMLCustomValue()
 :_Key(),_Val()
	{

	}

	CXMLCustomValue::CXMLCustomValue(const CXMLCustomValue &_TheVal)
 :_Key(_TheVal.Key()),_Val(_TheVal.Value())
	{

	}

	CXMLCustomValue::CXMLCustomValue(const std::string &_TheKey,const std::string &_TheVal,bool _IsEscape)
 :_Key(),_Val()
	{
		Key(_TheKey);
		if(_IsEscape)
		{
			EscapeValue(_TheVal);
		}
		else
		{
			Value(_TheVal);
		}

	}


	CXMLCustomValue::CXMLCustomValue(const CXMLBuffer &_BUF,const CXMLOffset &_TheKey,const CXMLOffset &_TheVal,bool _IsEscape)
 :_Key(),_Val()
	{
		const char *p = (const char *)_BUF.Get();

		if(_TheKey.Length()==0)
		{
			_Key = "";
		}
		else
		{
			Key(std::string(&p[_TheKey.Offset()],_TheKey.Length()));
		}

		if(_TheVal.Length()==0)
		{
			_Val = "";
		}
		else
		{
			if(_IsEscape)
			{
				EscapeValue(std::string(&p[_TheVal.Offset()],_TheVal.Length()));
			}
			else
			{
				Value(std::string(&p[_TheVal.Offset()],_TheVal.Length()));
			}

		}
	}

	CXMLCustomValue::~CXMLCustomValue()
	{

	}

	std::string CXMLCustomValue::Key() const
	{
		return _Key;
	}

	void CXMLCustomValue::Key(const std::string &_TheKey)
	{
		if(_TheKey.length() > 0)
		{
			CXMLBlader _KeyBuffer(_TheKey.c_str(),_TheKey.length());
			_Key = _KeyBuffer.Get();
		}
		else
		{
			_Key = _TheKey;
		}
	}

	void CXMLCustomValue::Value(const std::string &_TheVal)
	{
		if(_TheVal.length() > 0)
		{
			CXMLBlader _ValueBuffer(_TheVal.c_str(),_TheVal.length());
			_Val = _ValueBuffer.Get();
		}
		else
		{
			_Val = _TheVal;
		}
	}

	std::string CXMLCustomValue::Value() const
	{
		return _Val;
	}

	void CXMLCustomValue::EscapeValue(const std::string &_TheVal)
	{
		if(_TheVal.length() > 0)
		{
			CXMLEscape _Escape(_TheVal.c_str(),_TheVal.length());
			CXMLBlader _ValueBuffer(_Escape.Get(),_Escape.Length());
			_Val = _ValueBuffer.Get();
		}
		else
		{
			_Val = _TheVal;
		}
	}

	std::string CXMLCustomValue::EscapeValue() const
	{
		if(_Val.length() > 0)
		{
			CXMLUnescape _Unescape(_Val.c_str(),_Val.length());
			return _Unescape.Get();
		}
		else
		{
			return _Val;
		}
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLValue::CXMLValue()
 :CXMLCustomValue()
	{

	}

	CXMLValue::CXMLValue(const CXMLCustomValue &_TheVal)
 :CXMLCustomValue(_TheVal)
	{

	}

	CXMLValue::CXMLValue(const std::string &_TheKey,const std::string &_TheVal,bool _IsEscape)
 :CXMLCustomValue(_TheKey,_TheVal,_IsEscape)
	{

	}


	CXMLValue::CXMLValue(const CXMLBuffer &_BUF,const CXMLOffset &_TheKey,const CXMLOffset &_TheVal,bool _IsEscape)
 :CXMLCustomValue(_BUF,_TheKey,_TheVal,_IsEscape)
	{

	}

	CXMLValue::~CXMLValue()
	{

	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLCustomValues::CXMLCustomValues()
 :_Values()
	{

	}

	CXMLCustomValues::~CXMLCustomValues()
	{
		unsigned int i;
		for(i=0;i<_Values.size();i++)
		{
			if(_Values[i]!=NULL)
			{
				delete _Values[i];
				_Values[i] = NULL;
			}
		}
	}


	void CXMLCustomValues::Insert(const CXMLCustomValue *_Val)
	{
		if(_Val==NULL)
		{
			THROW_EXCEPTION_EX("_Val Is NULL!");
			return;
		}

		_Values.insert(_Values.begin(),_Val);
	}

	void CXMLCustomValues::Append(const CXMLCustomValue *_Val)
	{
		if(_Val==NULL)
		{
			THROW_EXCEPTION_EX("_Val Is NULL!");
			return;
		}

		_Values.push_back(_Val);
	}

	unsigned int CXMLCustomValues::Count() const
	{
		return _Values.size();
	}

	const CXMLCustomValue *CXMLCustomValues::operator [](std::string _Key) const
	{
		for(size_t i=0;i<_Values.size();i++)
		{
			if(_Values[i]!=NULL)
			{
				if(_Values[i]->Key() ==_Key)
				{
					return (_Values[i]);
				}
			}
		}

		CByteArray _ExcpStr(128,"The Key [ \"%s\" ] Not Found!",_Key.c_str());
		THROW_EXCEPTION_EX(_ExcpStr.Get());
		return NULL;
	}

	const CXMLCustomValue *CXMLCustomValues::operator [](unsigned int index) const
	{
		if(index < _Values.size())
		{
			return (_Values[index]);
		}
		else
		{
			CByteArray _ExcpStr(128,"The Index [ %d ] Out of bound [ 0~%d ]!",index,_Values.size()-1);
			THROW_EXCEPTION_EX(_ExcpStr.Get());
			return NULL;
		}
	}

	bool CXMLCustomValues::Exists(std::string _key) const
	{
		for(size_t i=0;i<_Values.size();i++)
		{
			if(_Values[i]!=NULL)
			{
				if(_Values[i]->Key() ==_key)
				{
					return true;
				}
			}
		}

		return false;
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLValues::CXMLValues()
 :CXMLCustomValues()
	{

	}

	CXMLValues::~CXMLValues()
	{

	}

	const CXMLValue *CXMLValues::operator [](std::string _Key) const
	{
		return (const CXMLValue *)CXMLCustomValues::operator [](_Key);
	}

	const CXMLValue *CXMLValues::operator [](unsigned int index) const
	{
		return (const CXMLValue *)CXMLCustomValues::operator [](index);
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLElement::CXMLElement()
 :CXMLCustomValue(),_Elements(),_Properties()
	{


	}

	CXMLElement::CXMLElement(const CXMLBuffer &_BUF,const CXMLOffset &_TheKey)
 :CXMLCustomValue(_BUF,_TheKey,CXMLOffset(0,0),false),_Elements(),_Properties()
	{

	}

	CXMLElement::CXMLElement(const std::string &_Thekey,const std::string &_Theval,bool _IsEscape)
 :CXMLCustomValue(_Thekey,_Theval,_IsEscape),_Elements(),_Properties()
	{

	}

	CXMLElement::CXMLElement(const std::string &_Thekey)
 :CXMLCustomValue(_Thekey,std::string(""),false),_Elements(),_Properties()
	{

	}

	CXMLElement::~CXMLElement()
	{

	}

	const CXMLElements &CXMLElement::SubElements() const
	{
		return _Elements;
	}

	CXMLElements &CXMLElement::SubElements()
	{
		return _Elements;
	}

	const CXMLValues &CXMLElement::Properties() const
	{
		return _Properties;
	}

	CXMLValues &CXMLElement::Properties()
	{
		return _Properties;
	}

	void CXMLElement::AddSubElement(const CXMLElement *_Elm)
	{
		_Elements.Append(_Elm);
	}

	void CXMLElement::AddProperty(const CXMLValue *_Property)
	{
		_Properties.Append(_Property);
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLElements::CXMLElements()
	{

	}

	CXMLElements::~CXMLElements()
	{

	}

	const CXMLElement *CXMLElements::operator [](std::string _key) const
	{
		return (CXMLElement *) CXMLCustomValues::operator [](_key);
	}

	const CXMLElement *CXMLElements::operator [](unsigned int index) const
	{
		return (CXMLElement *) CXMLCustomValues::operator [](index);
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	CXMLDeclaration::CXMLDeclaration()
 :CXMLCustomValue(),_Items()
	{

	}

	CXMLDeclaration::CXMLDeclaration(const std::string &_TheValue)
 :CXMLCustomValue("",_TheValue),_Items()
	{

	}

	CXMLDeclaration::CXMLDeclaration(const CXMLBuffer &_BUF,const CXMLOffset &_TheVal)
 :CXMLCustomValue(_BUF,CXMLOffset(0,0),_TheVal),_Items()
	{

	}

	CXMLDeclaration::~CXMLDeclaration()
	{

	}

	const CXMLValues *CXMLDeclaration::Items() const
	{
		return &_Items;
	}

	void CXMLDeclaration::Append(const CXMLValue *_TheVal)
	{
		_Items.Append(_TheVal);
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLDocument::CXMLDocument(CXMLDeclaration *_TheDeclaration,CXMLElement *_TheRoot)
		:_Root(_TheRoot),_Decl(_TheDeclaration)
	{
		if(_Decl==NULL)
		{
			THROW_EXCEPTION_EX("Declaration is NULL!");
			return;
		}

		if(_Root==NULL)
		{
			THROW_EXCEPTION_EX("Root Element is NULL!");
			return;
		}
	}

	CXMLDocument::~CXMLDocument()
	{
		if(_Root!=NULL)
		{
			delete _Root;
			_Root = NULL;
		}

		if(_Decl!=NULL)
		{
			delete _Decl;
			_Decl = NULL;
		}
	}

	const CXMLDeclaration *CXMLDocument::Declarations() const
	{
		if(_Decl==NULL)
		{
			THROW_EXCEPTION_EX("Declaration is NULL!");
			return NULL;
		}

		return _Decl;
	}

	const CXMLElement *CXMLDocument::Root() const
	{
		if(_Root==NULL)
		{
			THROW_EXCEPTION_EX("Root Element is NULL!");
			return NULL;
		}

		return (_Root);
	}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLParser::CXMLParser(const CXMLBuffer &Buffer)
 :_Buffer(Buffer)
	{

	}

	CXMLParser::~CXMLParser()
	{

	}

	const CXMLDocument *CXMLParser::Parse()
	{
		CXMLParserStream	_Stream(_Buffer);

		return ParseDocument(_Stream);
	}


	const CXMLDocument *CXMLParser::ParseDocument(CXMLParserStream &_Stream)
	{
		CNursery	_Nursery;
		CXMLDeclaration	*_Declaration = new CXMLDeclaration();
		if(_Declaration == NULL)
		{
			THROW_EXCEPTION_EX("New Declaration failure!");
		}

		//很重要
		_Nursery.Enter(_Declaration);

		if(!ParseDeclaration(_Stream,_Declaration))
		{
			THROW_EXCEPTION_EX("Parse The Declaration failure!");
		}

		CXMLElement	*_Root = new CXMLElement();
		if(_Root == NULL)
		{
			THROW_EXCEPTION_EX("New _Root failure!");
		}


		//很重要
		_Nursery.Enter(_Root);

		if(!ParseRootElement(_Stream,_Root))
		{
			THROW_EXCEPTION_EX("Parse The root failure!");
		}

		CXMLDocument *_Document = new CXMLDocument(_Declaration,_Root);
		if(_Document == NULL)
		{
			THROW_EXCEPTION_EX("New _Document failure!");
		}

		//很重要
		_Nursery.Realse();

		return _Document;
	}

	bool CXMLParser::ParseDeclaration(CXMLParserStream &_Stream,CXMLDeclaration	*_Declaration)
	{
		CXMLOffset	_Begin;
		CXMLOffset	_End;


		if(!ParseDeclarationGetBegin(_Stream,_Begin))
		{
			return false;
		}

		if(!ParseDeclarationEnumAttrib(_Stream,_Declaration))
		{
			return false;
		}


		if(!ParseDeclarationGetEnd(_Stream,_End))
		{
			return false;
		}

		CXMLOffset _Dist;
		_Dist.Distance(_Begin,_End);
		CXMLValue _Value(_Buffer,CXMLOffset(0,0),_Dist);
		_Declaration->Value(_Value.Value());

		return true;
	}

	bool CXMLParser::ParseDeclarationGetBegin(CXMLParserStream &_Stream,CXMLOffset &_Begin)
	{
		unsigned int BeginOffset=0;
		unsigned int BeginLen=0;

		if(!_Stream.FirstNoSpaceCharIs('<'))
		{
			THROW_EXCEPTION_EX("Begin with \"<?xml \" only!");
			return false;
		}

		BeginOffset = _Stream.Offset();

		if(!_Stream.MatchKeyword("<?xml"))
		{
			THROW_EXCEPTION_EX("Begin with \"<?xml \" only!");
			return false;
		}

		if(!_Stream.IsSpaceChar())
		{
			THROW_EXCEPTION_EX("Begin with \"<?xml \" only!");
			return false;
		}

		BeginLen = _Stream.Offset() - BeginOffset;

		_Begin = CXMLOffset(BeginOffset,BeginLen);

		return true;
	}

	bool CXMLParser::ParseDeclarationEnumAttrib(CXMLParserStream &_Stream,CXMLDeclaration *_Declaration)
	{
		char  ch;

		if(_Declaration==NULL)
		{
			THROW_EXCEPTION_EX("_Declaration is NULL");
			return false;
		}

		for(ch = _Stream.FirstNoSpaceChar();(ch!=0);ch = _Stream.FirstNoSpaceChar())
		{
			if(ch=='v')
			{
				CXMLOffset _VersionKey;
				CXMLOffset _VersionVal;
				if(!ParseDeclarationGetAttribKeyVal(_Stream,_VersionKey,_VersionVal,"version"))
				{
					THROW_EXCEPTION_EX("Syntax error");
					return false;
				}

				_Declaration->Append(new CXMLValue(_Buffer,_VersionKey,_VersionVal));
			}
			else if(ch == 'e')
			{
				CXMLOffset _EncodingKey;
				CXMLOffset _EncodingVal;
				if(!ParseDeclarationGetAttribKeyVal(_Stream,_EncodingKey,_EncodingVal,"encoding"))
				{
					THROW_EXCEPTION_EX("Syntax error");
					return false;
				}

				_Declaration->Append(new CXMLValue(_Buffer,_EncodingKey,_EncodingVal));
			}
			else if(ch == 's')
			{
				CXMLOffset _StandaloneKey;
				CXMLOffset _StandaloneVal;
				if(!ParseDeclarationGetAttribKeyVal(_Stream,_StandaloneKey,_StandaloneVal,"standalone"))
				{
					THROW_EXCEPTION_EX("Syntax error");
					return false;
				}

				_Declaration->Append(new CXMLValue(_Buffer,_StandaloneKey,_StandaloneVal));
			}
			else
			{
				break;
			}
		}

		if(_Stream.FirstNoSpaceCharIs('?'))
		{
			return true;
		}
		else
		{
			THROW_EXCEPTION_EX("Syntax error");
			return false;
		}
	}

	bool CXMLParser::ParseDeclarationGetAttribKeyVal(CXMLParserStream &_Stream,CXMLOffset &_Key,CXMLOffset &_Val,const char *_Keyword)
	{
		char		 Qoute;
		unsigned int KeyOffset=0;
		unsigned int KeyLen=0;
		unsigned int ValOffset=0;
		unsigned int ValLen=0;

		KeyOffset = _Stream.Offset();

		if(!_Stream.MatchKeyword(_Keyword))
		{
			THROW_EXCEPTION_EX("invalid  keyword!");
			return false;
		}

		KeyLen = _Stream.Offset() - KeyOffset;

		if(!_Stream.FirstNoSpaceCharIs('='))
		{
			THROW_EXCEPTION_EX("Attrib Syntax error,May be \'=\'??");
			return false;
		}

		_Stream.Next();
		char ch = _Stream.FirstNoSpaceChar();
		if((ch != '\"') && (ch!='\''))
		{
			THROW_EXCEPTION_EX("Attrib Syntax error,use qoute \" or \'??");
			return false;
		}
		Qoute = ch;

		ValOffset = _Stream.Offset() +1;

		for(ch = _Stream.Next();(ch!=0) && (ch!=Qoute) && (ch!='?') && (ch!='<') && (ch!='>');ch = _Stream.Next())
		{
			if(_Stream.IsSpaceChar(ch))
			{
				THROW_EXCEPTION_EX("Attrib Syntax error,use qoute \" or \' pair??");
				return false;
			}
		}

		if(ch!=Qoute)
		{
			THROW_EXCEPTION_EX("Attrib Syntax error,use qoute \" or \' pair??");
			return false;
		}

		ValLen = _Stream.Offset() - ValOffset;
		_Stream.Next();


		_Key = CXMLOffset(KeyOffset,KeyLen);
		_Val = CXMLOffset(ValOffset,ValLen);

		return true;
	}

	bool CXMLParser::ParseDeclarationGetEnd(CXMLParserStream &_Stream,CXMLOffset &_End)
	{
		unsigned int EndOffset=0;
		unsigned int EndLen=0;

		if(!_Stream.FirstNoSpaceCharIs('?'))
		{
			THROW_EXCEPTION_EX("Syntax error");
			return false;
		}

		EndOffset = _Stream.Offset();

		if(!_Stream.MatchKeyword("?>"))
		{
			THROW_EXCEPTION_EX("End with \"?>\" only!");
			return false;
		}

		EndLen = _Stream.Offset() - EndOffset;

		_End = CXMLOffset(EndOffset,EndLen);

		return true;
	}

	bool CXMLParser::ParseRootElement(CXMLParserStream &_Stream,CXMLElement *_Root)
	{
		_Stream.FirstNoSpaceChar();
		if((_Stream.GetChar(0)=='<') && (_Stream.GetChar(1)=='!')) //注释
		{
			if(!ParseIgnoreComment(_Stream))
			{
				THROW_EXCEPTION_EX("XML Comment Syntex error!");
				return false;
			}

			_Stream.Next();
			_Stream.FirstNoSpaceChar();
		}

		if(!ParseEnumElement(_Stream,_Root))
		{
			THROW_EXCEPTION_EX("XML Element Syntex error!");
			return false;
		}

		_Stream.Next();

		unsigned int i;
		char ch;
		for(i=_Stream.Offset(),ch = _Stream.FirstNoSpaceChar();(i<_Stream.Limit()) && (ch!=0);ch=_Stream.Step(1))
		{
			if(ch=='<')
			{
				if(_Stream.GetChar(1)=='!')
				{
					if(!ParseIgnoreComment(_Stream))
					{
						THROW_EXCEPTION_EX("XML Comment Syntex error!");
						return false;
					}
				}
				else
				{
					THROW_EXCEPTION_EX("No more element after root element!");
					return false;
				}
			}
		}

		return true;
	}

	bool CXMLParser::ParseEnumElement(CXMLParserStream &_Stream,CXMLElement *_Element)
	{
		unsigned int i;
		char ch ;
		unsigned int _KeyBegin=0;
		unsigned int _KeyLen=0;
		unsigned int _ValBegin=0;
		unsigned int _ValLen=0;

		if(_Element==NULL)
		{
			THROW_EXCEPTION_EX("_Element is NULL");
			return false;
		}

		if(!_Stream.FirstNoSpaceCharIs('<'))
		{
			THROW_EXCEPTION_EX("XML Key Syntex error!");
			return false;
		}

		_Stream.Next();

		_KeyBegin = _Stream.Offset();

		if(!_Stream.IsAllowedKeyStartChar())
		{
			THROW_EXCEPTION_EX("XML Key Syntex error!");
			return false;
		}

		for(i=_Stream.Offset(),ch = _Stream.FirstNoSpaceChar();(i<_Stream.Limit()) && (ch!=0);ch=_Stream.Step(1))
		{
			//ch = _Stream.GetChar(0);
			//if(!((_Stream.IsAllowedKeyChar(ch)) || (_Stream.IsAllowedKeySymbol(ch))|| (_Stream.IsDigitalChar(ch))))
			if(!_Stream.IsAllowedKeyChar(ch)) 
			{
				if(_Stream.IsSpaceChar(ch))
				{
					_KeyLen = _Stream.Offset() - _KeyBegin;

					_Element->Key(CXMLValue(_Buffer,CXMLOffset(_KeyBegin,_KeyLen),CXMLOffset(_KeyBegin,_KeyLen)).Key());
					break;
				}
				else if(ch=='>')
				{
					_KeyLen = _Stream.Offset() - _KeyBegin;

					if(_KeyLen>0)
					{
						_Element->Key(CXMLValue(_Buffer,CXMLOffset(_KeyBegin,_KeyLen),CXMLOffset(_KeyBegin,_KeyLen)).Key());
					}

					if(_Element->Key().length()==0)
					{
						THROW_EXCEPTION_EX("XML Key is Empty!");
						return false;
					}

					break;
				}
				else if(ch=='/')
				{
					if(_Stream.GetChar(1)=='>')
					{
						_KeyLen = _Stream.Offset() - _KeyBegin;

						_Stream.Next();

						if(_KeyLen > 0)
						{
							_Element->Key(CXMLValue(_Buffer,CXMLOffset(_KeyBegin,_KeyLen),CXMLOffset(_KeyBegin,_KeyLen)).Key());

						}

						if(_Element->Key().length()==0)
						{
							THROW_EXCEPTION_EX("XML Key is Empty!");
							return false;
						}

						return true;
					}
					else
					{
						THROW_EXCEPTION_EX("XML Key Syntex error!");
						return false;
					}
				}
			}
		}

		if(_Stream.IsSpaceChar())	//Property
		{

			if(_Stream.FirstNoSpaceChar()!='>')
			{
				if(!ParseEnumAttrib(_Stream,_Element))
				{
					THROW_EXCEPTION_EX("XML Property Syntex error!");
					return false;
				}
			}

		}


		if(_Stream.GetChar(0)=='>') // Value
		{
			_Stream.Next();

			_Stream.FirstNoSpaceChar();
			_ValBegin = _Stream.Offset();


			for(ch = _Stream.FirstNoSpaceChar(),i=_Stream.Offset();(i<_Stream.Limit()) && (ch!=0);ch = _Stream.Step(1))
			{
				if(_Stream.IsSpaceChar())
				{
					ch = _Stream.FirstNoSpaceChar();
				}

				if(ch=='<')
				{
					if(_Stream.GetChar(1)=='/') //
					{

						_ValLen = _Stream.Offset() - _ValBegin;

						_Stream.Step(2);
						_Stream.FirstNoSpaceChar();

						if(_Stream.MatchKeyword(_Element->Key().c_str()) && _Stream.FirstNoSpaceCharIs('>'))
						{
							//_Stream.Next();
							if(_ValLen > 0)
							{
								_Element->Value(CXMLValue(_Buffer,CXMLOffset(_ValBegin,_ValLen),CXMLOffset(_ValBegin,_ValLen)).Value());
							}
							return true;
						}
					}
					else if(_Stream.GetChar(1)=='!') //注释
					{
						if(!ParseIgnoreComment(_Stream))
						{
							THROW_EXCEPTION_EX("XML Comment Syntex error!");
							return false;
						}

					}
					else if(_Stream.IsAllowedKeyStartChar(_Stream.GetChar(1)))
					{
						//Children
						CNursery	_Nursery;
						CXMLElement *_E = new CXMLElement();
						if(_E==NULL)
						{
							THROW_EXCEPTION_EX("new Element error");
							return false;
						}

						_Nursery.Enter(_E);


						if(!ParseEnumElement(_Stream,_E))
						{
							THROW_EXCEPTION_EX("Parse Children Element error!");
							return false;
						}

						_Element->AddSubElement(_E);

						_Nursery.Realse();
					}
				}
			}


			THROW_EXCEPTION_EX("XML Key Syntex error!");
			return false;
		}
		else
		{
			THROW_EXCEPTION_EX("XML Key Syntex error!");
			return false;
		}
	}

	bool CXMLParser::ParseIgnoreComment(CXMLParserStream &_Stream)
	{
		_Stream.FirstNoSpaceChar();
		if((_Stream.GetChar(0)=='<') &&(_Stream.GetChar(1)=='!'))
		{
			if(!_Stream.MatchKeyword("<!--"))
			{
				THROW_EXCEPTION_EX("XML Comment Syntex error!");
				return false;
			}

			for(unsigned int i=_Stream.Offset();(i<_Stream.Limit()) && (_Stream.GetChar(0)!=0);_Stream.Step(1))
			{
				if((_Stream.GetChar(0)=='-') && (_Stream.GetChar(1)=='-') && (_Stream.GetChar(2)=='>'))
				{
					_Stream.Step(2);
					return true;
				}
			}

			THROW_EXCEPTION_EX("XML Comment Syntex error!");
			return false;

		}
		else
		{
			return true;
		}


	}

	bool CXMLParser::ParseEnumAttrib(CXMLParserStream &_Stream,CXMLElement *_Parent)
	{
		unsigned int i;
		char ch;
		unsigned int _KeyBegin=0;
		unsigned int _KeyLen=0;
		unsigned int _ValBegin=0;
		unsigned int _ValLen=0;
		char Qoute = 0;

		if(_Parent==NULL)
		{
			THROW_EXCEPTION_EX("_Parent is NULL");
			return false;
		}

		ch = _Stream.FirstNoSpaceChar();
		if(ch == '>')
		{
			return true;
		}

		if(!_Stream.IsAllowedKeyStartChar(ch))
		{
			THROW_EXCEPTION_EX("Invalid Key char");
			return false;
		}

		_KeyBegin = _Stream.Offset();

		for(i=_Stream.Offset(),ch = _Stream.FirstNoSpaceChar();(i<_Stream.Limit()) && (ch!=0);ch = _Stream.Step(1))
		{
			if(ch=='>')
			{
				break;
			}
			else if((_Stream.IsSpaceChar(ch))||(ch=='='))
			{
				_KeyLen = _Stream.Offset() - _KeyBegin;
				break;
			}
			//else if((!_Stream.IsAllowedKeyChar(ch)) && (!_Stream.IsDigitalChar(ch)))
			else if(!_Stream.IsAllowedKeyChar(ch))
			{
				THROW_EXCEPTION_EX("Invalid Key char");
				return false;
			}
		}

		if(_Stream.FirstNoSpaceChar()!='=')
		{
			THROW_EXCEPTION_EX("Attrib Syntax error,May be \'=\'??");
			return false;
		}

		_Stream.Next();

		ch = _Stream.FirstNoSpaceChar();
		if((ch != '\"') && (ch!='\''))
		{
			THROW_EXCEPTION_EX("Attrib Syntax error,use qoute \" or \'??");
			return false;
		}

		Qoute = ch;
		_Stream.Next();

		_Stream.FirstNoSpaceChar();
		_ValBegin = _Stream.Offset();

		for(i=_Stream.Offset(),ch = _Stream.FirstNoSpaceChar();(i<_Stream.Limit()) && (ch!=0);ch = _Stream.Step(1))
		{
			if(ch=='>')
			{
				break;
			}
			else if(ch==Qoute)
			{
				_ValLen = _Stream.Offset() - _ValBegin;
				break;
			}
		}

		if(_Stream.FirstNoSpaceChar()!=Qoute)
		{
			THROW_EXCEPTION_EX("Attrib Syntax error");
			return false;
		}

		_Parent->AddProperty(new CXMLValue(_Buffer,CXMLOffset(_KeyBegin,_KeyLen),CXMLOffset(_ValBegin,_ValLen)));

		_Stream.Next();

		if(_Stream.FirstNoSpaceChar()!='>')
		{
			return ParseEnumAttrib(_Stream,_Parent);
		}

		return true;
	}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	CXMLReader::CXMLReader(const char *_PathofXML)
     :_Document(NULL)
	{
		CXMLLoder	_XFLE(_PathofXML);
		CXMLParser	_Parser(_XFLE);
		_Document = _Parser.Parse();
		if(_Document==NULL)
		{
			THROW_EXCEPTION_EX("_Document is NULL");
			return;
		}
	}

	CXMLReader::CXMLReader(const char *xmlStr,int len)
     :_Document(NULL)
	{
		CXMLLoder	_XFLE(xmlStr,len);
		CXMLParser	_Parser(_XFLE);
		_Document = _Parser.Parse();
		if(_Document==NULL)
		{
			THROW_EXCEPTION_EX("_Document is NULL");
			return;
		}
	}
	CXMLReader::~CXMLReader()
	{
		if(_Document!=NULL)
		{
			delete _Document;
			_Document = NULL;
		}
	}

	const CXMLDocument *CXMLReader::Document() const
	{
		if(_Document==NULL)
		{
			THROW_EXCEPTION_EX("_Document is NULL");
			return NULL;
		}
		return _Document;
	}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	CXMLWriter::CXMLWriter(const CXMLDocument *_Document,std::string &XMLStr)
	{
		if(_Document==NULL)
		{
			THROW_EXCEPTION_EX("_Document is NULL");
			return;
		}

		CXMLString	_Str(XMLStr);
		WriteDocument(&_Str,_Document);

    }

	CXMLWriter::CXMLWriter(const CXMLDocument *_Document,const char *_PathofXML)
	{
		if(_Document==NULL)
		{
			THROW_EXCEPTION_EX("_Document is NULL");
			return;
		}

		if(_PathofXML==NULL)
		{
			THROW_EXCEPTION_EX("_Document is NULL");
			return;
		}

		CXMLFile	_File(_PathofXML,true);

		WriteDocument(&_File,_Document);
	}

	CXMLWriter::~CXMLWriter()
	{

	}

	//void CXMLWriter::WriteDocument(CXMLFile &_File,const CXMLDocument *_Document)
	void CXMLWriter::WriteDocument(CXMLMedia *_File,const CXMLDocument *_Document)
	{

		WriteDeclaration(_File,_Document->Declarations());
		WriteElement(_File,_Document->Root(),0);

	}

//	void CXMLWriter::WriteDeclaration(CXMLFile &_File,const CXMLDeclaration *_Declaration)
	void CXMLWriter::WriteDeclaration(CXMLMedia *_File,const CXMLDeclaration *_Declaration)
	{
		if(_Declaration==NULL)
		{
			THROW_EXCEPTION_EX("_Declaration is NULL");
			return;
		}

		if(_Declaration->Items()->Count()>0)
		{
			_File->Write("<?xml",5);
			for(size_t i=0;i<_Declaration->Items()->Count();i++)
			{
				const CXMLValue *_Val = (*_Declaration->Items())[i];
				_File->Fill(' ',1);
				_File->Write((char *)_Val->Key().c_str(),_Val->Key().length());
				_File->Fill('=',1);
				_File->Fill('"',1);
				_File->Write((char *)_Val->Value().c_str(),_Val->Value().length());
				_File->Fill('"',1);
			}

			_File->Write("?>\n",3);
		}
	}

	//void CXMLWriter::WriteElement(CXMLFile &_File,const CXMLElement *_Element,unsigned int iTabs)
	void CXMLWriter::WriteElement(CXMLMedia *_File,const CXMLElement *_Element,unsigned int iTabs)
	{
		unsigned int i;
		if(_Element==NULL)
		{
			THROW_EXCEPTION_EX("_Declaration is NULL");
			return;
		}

		_File->Fill('\t',iTabs);
		_File->Write("<",1);
		_File->Write((char *)_Element->Key().c_str(),_Element->Key().length());

		//属性
		if(_Element->Properties().Count()>0)
		{
			for(i=0;i<_Element->Properties().Count();i++)
			{
				const CXMLValue *_Val = _Element->Properties()[i];
				_File->Fill(' ',1);
				_File->Write((char *)_Val->Key().c_str(),_Val->Key().length());
				_File->Fill('=',1);
				_File->Fill('"',1);
				_File->Write((char *)_Val->Value().c_str(),_Val->Value().length());
				_File->Fill('"',1);
			}
		}

		if((_Element->SubElements().Count()==0) && _Element->Value().empty())
		{
			_File->Write(" />\n",4);
			return;
		}
		else
		{
			_File->Write(">",1);
		}

		if(_Element->SubElements().Count()>0)
		{
			_File->Fill('\n',1);

			for(i=0;i<_Element->SubElements().Count();i++)
			{
				const CXMLElement *_E = _Element->SubElements()[i];
				WriteElement(_File,_E,iTabs+1);
			}

			_File->Fill('\t',iTabs);
			_File->Write("</",2);
			_File->Write((char *)_Element->Key().c_str(),_Element->Key().length());
			_File->Write(">\n",2);
		}
		else
		{
			_File->Write((char *)_Element->Value().c_str(),_Element->Value().length());
			_File->Write("</",2);
			_File->Write((char *)_Element->Key().c_str(),_Element->Key().length());
			_File->Write(">\n",2);
		}
	}

   } /// namespace GalaxyRT
} /// namespace Galaxy
