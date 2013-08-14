/**
 * @file    Object.hpp
 * @author  Ma Tao <qingant@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 *
 * @section DESCRIPTION
 *
 *CObject is the root base class.It now has only virtual destructor.
 */


#ifndef COBJECT_H
#define COBJECT_H

#include "TypeInc.hpp"
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
#pragma once

#include <assert.h>


#define GALA_ERROR(args...)   \
    do                    \
    {                     \
       fprintf(stderr,"[ FILE=%s FUNC=%s LINE=%d ] [", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
       fprintf(stderr,args);                                            \
       fprintf(stderr,"]\n");                                           \
    }while(false)


#ifdef _DEBUG_
#define GALA_DEBUG(args...)												\
   do																	\
   {																	\
      printf("FILE=%s FUNC=%s LINE=%d\n  ", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
      printf(args);                                                     \
      printf("\n");                                                     \
   }while(false)

#else
#define GALA_DEBUG(msg...) (void)0;
#endif

namespace Galaxy
{
   namespace GalaxyRT
   {

      class CObject
      {
      public:
         CObject();
         virtual ~CObject();
      public:
         std::string ClassName();
         bool ClassNameIs(const std::string name);
      };


      class CNonCopyAble:public CObject
      {
      protected:
         CNonCopyAble() {}
         virtual ~CNonCopyAble() {}
      private:
         // emphasize the following members are private
         CNonCopyAble( const CNonCopyAble& );
         const CNonCopyAble& operator=( const CNonCopyAble& );
      };


      class CContainer : public CObject
      {

      public:
         CContainer();
         virtual ~CContainer();
      public:
         void Push(CObject* item);
         bool Pop(CObject* item);
         bool Clear();
         bool Empty();
         bool Exists(CObject* item);
         size_t Count() const;
      private :
         CContainer(const CContainer& other);
         CContainer& operator=(const CContainer& other);
      private:
         std::list<CObject*> _list;
      };



      class CNursery : public CNonCopyAble
      {
      private:
         std::vector<CObject *> _List;

         static void* operator new (size_t);
      public:
         CNursery();
         ~CNursery();

         void Enter(CObject *_obj);
         void Realse();
      };


      class CByteArray : public CObject
      {
      private:
         unsigned char *_Buffer;
         void Size(unsigned int _Size);
      protected:
         CByteArray();

         void Allocate(unsigned int _Size);
         void Length(unsigned int _Length);
         unsigned char *_Get() const;
      public:
         CByteArray(unsigned int _Size);
         CByteArray(const char *_Text);
         CByteArray(const void *_data,unsigned int _Length);
         CByteArray(unsigned int _Maxsize,const char *_format,...);
         virtual ~CByteArray();

         bool Empty() const;
         unsigned int Size() const;
         unsigned int Length() const;
         const char *  Get() const;
      public:
         unsigned char operator[]( unsigned int _Index );
         bool operator==( const CByteArray& _Other);
      };

   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif
