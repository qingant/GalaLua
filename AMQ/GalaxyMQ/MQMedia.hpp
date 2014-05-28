#ifndef MQMEDIA_HPP_FFD040A8_0C0C_408F_B695_6FEADD614105
#define MQMEDIA_HPP_FFD040A8_0C0C_408F_B695_6FEADD614105

#pragma once

#include <string>
#include "GalaxyObject.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		class CMQMemory : public CGalaxyObject
		{
		private:
			volatile PBYTE	_Entry;
			
			CMQMemory(const CMQMemory &);
		    const CMQMemory & operator=(const CMQMemory &);
		protected:
			explicit CMQMemory(PBYTE _TheEntry,SIZE_T _Size);
		public:
			virtual ~CMQMemory();	
		    PBYTE Entry() const;
		    SIZE_T Size() const;
		};
		
		class CMQFileMemory : public CMQMemory
		{
		private:
			inline PBYTE Attach(const std::string &_FileName,SIZE_T &_Size);
			inline void Detach();
		public:
			explicit CMQFileMemory(const std::string &_FileName,SIZE_T _Size);
			~CMQFileMemory();	
		};
		
		class CSysvSHMemory : public CMQMemory
		{
		private:
		    inline PBYTE Attach(key_t _Key,SIZE_T _Size);
		    inline void Detach();

		    CSysvSHMemory(const CSysvSHMemory &);
		    const CSysvSHMemory & operator=(const CSysvSHMemory &);
		public:
		    explicit CSysvSHMemory(key_t _Key,SIZE_T _Size);
		    ~CSysvSHMemory();
		};
	}
}

#endif /*MQMEDIA_HPP_FFD040A8_0C0C_408F_B695_6FEADD614105*/
