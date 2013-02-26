/* -*- C++ -*- */

/* Time-stamp: <2013-02-22 04:14:11 星期五 by matao> */

/**
 * @file Utils.hpp
 * @author qingant
 */

#ifndef UTILS_H
#define UTILS_H


#include "Sync.hpp"
#include <string>
#include <map>
#include <iostream>
#include <cctype>
#include <cwctype>

namespace Galaxy
{
   namespace GalaxyRT
   {

      INT GetHostByName(const std::string &, struct hostent *phe);
      void RTrim(std::string& s);
      std::string LTrim(const std::string& s);
      std::string RLtrim(const std::string& s);
      //type traits
      template <typename _D>
      struct pointer_traits
      {
         typedef _D ValueType;
         typedef _D* PointerType;

      };
      template <typename _D>
      struct pointer_traits<_D*>
      {
         typedef _D  ValueType;
         typedef _D* PointerType;
      };

      template <typename _K, typename _V>
      class ConcurrentMap
      {
         // this class is a wrapper of map container in std namespace
         // using a read-write lock to sync the operation on the map
      public:
         typedef typename std::map<_K, _V> _MapType;
         typedef typename _MapType::iterator _IteratorType;
         typedef typename _MapType::iterator iterator; // for MapManager
         typedef typename _MapType::const_iterator const_iterator;
         typedef _K _KeyType;
         typedef typename _MapType::value_type _ElementType;
         ConcurrentMap()
			:_RL(_RWLock, CRWLock::RDLOCK),
			 _WL(_RWLock, CRWLock::WRLOCK)
         {

         }
         _IteratorType find(const _KeyType &key)
         {
            CLockGuard _GL(&_RL);
            return _Map.find(key);
         }
         bool has_key(const _KeyType &key)
         {
            CLockGuard _GL(&_RL);
            const_iterator it = _Map.find(key);
            if (it == _Map.end())
            {
               return false;
            }
            return true;
         }


         _V find_ex(const _KeyType &key) const
         {
            CLockGuard _GL(&_RL);
            const_iterator it = _Map.find(key);
            if (it == _Map.end())
            {
               THROW_EXCEPTION_EX("Key Not In");
            }
            return it->second;
         }

         _V& operator[](const _KeyType &key) 
         {
            CLockGuard _GL(&_RL);
            
            return _Map[key];         
         }
         _V pop(const _KeyType &key)
         {
            CLockGuard _GL(&_WL);
            iterator it = _Map.find(key);
            if (it == _Map.end())
            {
               THROW_EXCEPTION_EX("Key Not In");
            }
            _V v = it->second;
            _Map.erase(key);
            return v;
         }
         _IteratorType begin()
         {
            CLockGuard _GL(&_RL);
            return _Map.begin();
         }
         _IteratorType end()
         {
            CLockGuard _GL(&_RL);
            return _Map.end();
         }
         size_t size() const
         {
            CLockGuard _GL(&_RL);
            return size_t(_Map.size());
         }
         size_t erase(const _KeyType &key)
         {
            CLockGuard _GL(&_WL);
            return _Map.erase(key);
         }
         void erase(iterator _b, iterator _e)
         {
            CLockGuard _GL(&_WL);
            _Map.erase(_b, _e);
         }
         void insert(const _ElementType &pair)
         {
            CLockGuard _GL(&_WL);
            _Map.erase(pair.first);
            _Map.insert(pair);
            //printf("From Line[%d] ConcurentMap In Insert:\n", __LINE__);

            //PrettyPrint();
         }
         void PrettyPrint()
         {
            std::cout<<_Map<<std::endl;
         }
         _V test_and_insert(const _K &key)
         {
            typedef typename pointer_traits<_V>::ValueType VT;
            CLockGuard _GL(&_WL);
            const_iterator it = _Map.find(key);
            if (it == _Map.end())
            {
               _Map.insert(std::make_pair(key, new VT()));
               it = _Map.find(key);

            }
            return it->second;

         }

         virtual ~ConcurrentMap()
         {

         }
      private:
         CRWLock  _RWLock;
         CRWLockAdapter _RL;
         CRWLockAdapter _WL;
         _MapType _Map;
      };


      template <typename _Map, typename _I = typename _Map::iterator>
      class MapManager
      {
      public:
         typedef _Map _MapType;
         typedef _I _MapIteratorType;
         typedef _I iterator;
         MapManager()
         {

         };
         _MapType *operator-> ()
         {
            return &connmap;
         }
         void PrettyPrint()
         {
            std::cout<<connmap<<std::endl;
         }
         ~MapManager()
         {
            for (_MapIteratorType it = connmap.begin(); it!=connmap.end(); ++it)
            {
               if (it->second != NULL)
               {
                  delete it->second;
               }
            }
         }
      private:
         _MapType connmap;
         CRWLock  _RWLock;
      };



      UINT MakeConnKey(USHORT device_id, USHORT serial);

      bool TestConnection(INT fd);
      bool BadConnection(INT fd);

   } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /* UTILS_H */
