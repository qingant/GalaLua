#ifndef MQMEDIA_HPP_FFD040A8_0C0C_408F_B695_6FEADD614105
#define MQMEDIA_HPP_FFD040A8_0C0C_408F_B695_6FEADD614105

#pragma once

#include "MQI.hpp"

namespace Galaxy
{
	namespace AMQ
	{
		class CBinaryFile : public CMQLoneAncestor
		{
		private:
			INT     _FD;
		
			inline INT Open(const std::string &_FileName,INT _Flag,mode_t _Perm);
			inline void Close();
		public:
			explicit CBinaryFile(const std::string &_FileName,INT _Flag,mode_t _Perm);
			~CBinaryFile();
		
		
			INT Handle() const;
			SIZE_T Size() const;
			void Truncate(SIZE_T _Length) const;
		};
		
		
		class CSHM : public CMQLoneAncestor
		{
		private:
			INT 	_Handle;
		    PBYTE   _RAWEntry;
		    
		    inline INT Create(key_t _Key,UINT _Size, INT _oflag);
		    inline PBYTE Attach();
		    inline void Detach();
		    inline void Size(UINT _Size); 

		    CSHM(const CSHM &);
		    const CSHM & operator=(const CSHM &);
		public:
		    explicit CSHM(key_t _Key,UINT _Size, INT _oflag);
		    ~CSHM();

		    PBYTE Entry() const;
		    UINT Size() const;
		};
	}
}

#endif /*MQMEDIA_HPP_FFD040A8_0C0C_408F_B695_6FEADD614105*/
