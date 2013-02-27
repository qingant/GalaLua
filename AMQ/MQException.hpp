#ifndef MQEXCEPTION_HPP_FD997534_5931_47FD_8B83_F05ECD0A58C1
#define MQEXCEPTION_HPP_FD997534_5931_47FD_8B83_F05ECD0A58C1

#pragma once

#include "MQI.hpp"

namespace Galaxy
{
	namespace AMQ
	{
	
		#if defined(__xlC__)
		#define __PRETTY_FUNCTION__		__FUNCTION__
		#endif
		
		#if !defined(__PRETTY_FUNCTION__)
		#define __PRETTY_FUNCTION__		__FUNCTION__
		#endif
		
		
		class CMQException : public std::exception,public CMQAncestor
		{
		private:
			std::string _What;
		protected: 
			explicit CMQException() throw();
			explicit CMQException(const std::string& __excp__) throw(); 
			
			void What(const std::string &_what_) throw(); 
			std::string Now() throw();
		public:
			explicit CMQException(const std::string &_file_,const std::string &_func_, INT _line_,const std::string& _msg_="") throw();
			virtual ~CMQException() throw();
			CPSTR what() const throw();
		};
		
		#define THROW_MQEXCEPTION(__excp__) (throw CMQException(__FILE__,__PRETTY_FUNCTION__,__LINE__,__excp__))
		
	} // AMQ namespace
} // Galaxy namespace

#endif /*MQEXCEPTION_HPP_FD997534_5931_47FD_8B83_F05ECD0A58C1*/
