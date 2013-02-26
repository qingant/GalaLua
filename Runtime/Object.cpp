/*
  <one line to give the program's name and a brief idea of what it does.>
  Copyright (C) 2011  qingant <qingant@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vector>
#include <typeinfo>
#include "Object.hpp"
#include "Exception.hpp"
#include "cstub.h"
#include <stdarg.h>

namespace Galaxy
{
   namespace GalaxyRT
   {

/*CObject*/
      CObject::CObject()
      {

      }


      CObject::~CObject()
      {
      }

      std::string CObject::ClassName()
      {
         return std::string(typeid(this).name());
      }

      bool CObject::ClassNameIs(const std::string name)
      {
         return (std::string(typeid(this).name()) == name);
      }



/*CContainer*/
      CContainer::CContainer()
      {

      }

      CContainer::~CContainer()
      {
         for(std::list<CObject*>::iterator it = _list.begin();it!=_list.end();++it)
         {
			delete (*it);
         }
      }

      void CContainer::Push(CObject* item)
      {
         _list.push_back(item);
         _list.unique();
      }

      bool CContainer::Pop(CObject* item)
      {
         _list.remove(item);
         return true;
      }

      bool CContainer::Clear()
      {
         _list.clear();
         return true;
      }

      bool CContainer::Empty()
      {
         return _list.empty();
      }

      bool CContainer::Exists(CObject* item)
      {
         for(std::list<CObject*>::iterator it = _list.begin();it!=_list.end();++it)
         {
			if((*it)==item)
			{
               return true;
			}
         }

         return false;
      }

      size_t CContainer::Count() const
      {
         return _list.size();
      }




// CNursery


      CNursery::CNursery()
         :_List()
      {

      }

      CNursery::~CNursery()
      {

         unsigned int i;
         for(i=0;i<_List.size();i++)
         {
			if(_List[i]!=NULL)
			{
               delete _List[i];
               _List[i] = NULL;
			}
         }

         _List.clear();
      }

      void CNursery::Enter(CObject *_obj)
      {
         if(_obj!=NULL)
         {
			_List.push_back(_obj);
         }
      }

      void CNursery::Realse()
      {
         _List.clear();
      }




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

      CByteArray::CByteArray()
         :_Buffer(NULL)
      {

      }

      CByteArray::CByteArray(unsigned int _Size)
         :_Buffer(NULL)
      {
         Allocate(_Size);
      }

      CByteArray::CByteArray(const char *_Text)
         :_Buffer(NULL)
      {
         char *p;
         if(_Text==NULL)
         {
			THROW_EXCEPTION_EX("_Text is null");
			return;
         }

         for(p=(char *)_Text;(p!=NULL) && (*p!=0);p++);

         unsigned int _Length = (unsigned int)(p - (char *)_Text);
         Allocate(_Length+1);
         memcpy(_Get(),_Text,_Length);
         Length(_Length);
      }

      CByteArray::CByteArray(const void *_data,unsigned int _Length)
         :_Buffer(NULL)
      {
         if(_data==NULL)
         {
			THROW_EXCEPTION_EX("_data is null");
			return;
         }

         Allocate(_Length+1);
         memcpy(_Get(),_data,_Length);
         Length(_Length);
      }

      CByteArray::CByteArray(unsigned int _Maxsize,const char *_format,...)
         :_Buffer(NULL)
      {
         unsigned int i;
         va_list argp;

         if(_format==NULL)
         {
			THROW_EXCEPTION_EX("_format is null");
			return;
         }
         else if(_Maxsize==0)
         {
			THROW_EXCEPTION_EX("_Maxsize = 0");
			return;
         }

         Allocate(_Maxsize+1);


         va_start(argp, _format);
         CRT_vsnprintf((char *)_Get(),_Maxsize,(char *)_format,argp);
         va_end(argp);

         for(i=0;(i<_Maxsize) && (_Get()[i]!=0);i++);

         Length(i);
      }

      CByteArray::~CByteArray()
      {
         if(_Buffer!=NULL)
         {
			delete [] _Buffer;
			_Buffer = NULL;
         }
      }

      void CByteArray::Allocate(unsigned int _Size)
      {
         if(_Size==0)
         {
			THROW_EXCEPTION_EX("Length = 0");
			return;
         }

         unsigned int _AllSize = _Size+sizeof(unsigned int)+sizeof(unsigned int)+1;
         _Buffer = new unsigned char[_AllSize];
         if(_Buffer==NULL)
         {
			THROW_EXCEPTION_EX("new buffer error");
			return;
         }

         Size(_Size);
         Length(0);
         memset(_Get(),0,Size()+1);
      }

      void CByteArray::Size(unsigned int _Size)
      {
         memcpy(_Buffer,&_Size,sizeof(unsigned int));
      }

      void CByteArray::Length(unsigned int _Length)
      {
         memcpy(&_Buffer[sizeof(unsigned int)],&_Length,sizeof(unsigned int));
      }


      unsigned char *CByteArray::_Get() const
      {
         if(_Buffer==NULL)
         {
			THROW_EXCEPTION_EX("invalid buffer");
         }
         return &_Buffer[(sizeof(unsigned int)*2)];
      }

      unsigned int CByteArray::Size() const
      {
         return *((unsigned int *)_Buffer);
      }

      unsigned int CByteArray::Length() const
      {
         return *((unsigned int *) (&_Buffer[sizeof(unsigned int)]));
      }

      const char *CByteArray::Get() const
      {
         return (char *)_Get();
      }

      bool CByteArray::Empty() const
      {
         return ((_Buffer==NULL)||(Length()==0));
      }

      unsigned char CByteArray::operator[]( unsigned int _Index )
      {
         if(_Index < Size())
         {
			return _Get()[_Index];
         }
         else
         {
			return (0);
         }
      }

      bool CByteArray::operator==( const CByteArray& _Other)
      {
         if(Length()!=_Other.Length())
         {
			return false;
         }
         else if(memcmp(Get(),_Other.Get(),Length())!=0)
         {
			return false;
         }

         return true;
      }

   } /// namespace GalaxyRT
} /// namespace Galaxy
