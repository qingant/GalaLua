
#include "MQException.hpp"
using namespace Galaxy::AMQ;
/*Exception*/
CMQException::CMQException() throw()
   :_What()
{  

}

CMQException::CMQException(const std::string& __excp__) throw()
   :_What(__excp__)
{
    
}

CMQException::CMQException(const std::string &_file_,const std::string &_func_,
                       INT _line_,const std::string& _msg_) throw()
   :_What()
{
    
   //CHAR    szBuffer[3584];
   CHAR    szBuffer[4096];

   if(_msg_.length() > 0)
   {
      CRT_snprintf(szBuffer,sizeof(szBuffer),"Exception:\n"
                   "    FILE  (%s);\n"
                   "    FUNC  (%s);\n"
                   "    LINE  (%d);\n"
                   "    MESG  (%s);\n"
                   "    TIME  (%s);\n"
                   , \
                   _file_.c_str()
                   , \
                   _func_.c_str()
                   , \
                   _line_
                   , \
                   _msg_.c_str()
                   , \
                   Now().c_str()
         );
                         
   }
   else
   {
      if(errno!=0)
      {
         CHAR    szStrError[128];
         CRT_strerror_r(errno,szStrError,128);
         CRT_snprintf(szBuffer,sizeof(szBuffer),"Exception:\n"
                      "    FILE  (%s);\n"
                      "    FUNC  (%s);\n"
                      "    LINE  (%d);\n"
                      "    ERNO  (%d);\n"
                      "    SYER  (%s);\n" 
                      "    TIME  (%s);\n" 
                      , \
                      _file_.c_str()
                      , \
                      _func_.c_str()
                      , \
                      _line_
                      , \
                      errno
                      , \
                      szStrError
                      , \
                      Now().c_str()
            );
      }
      else
      {
         CRT_snprintf(szBuffer,sizeof(szBuffer),"Exception:\n"
                      "    FILE  (%s);\n"
                      "    FUNC  (%s);\n"
                      "    LINE  (%d);\n"
                      "    TIME  (%s);\n" 
                      , \
                      _file_.c_str()
                      , \
                      _func_.c_str()
                      , \
                      _line_
                      , \
                      Now().c_str()
            ); 
            
      } 
   } 
    
   What(szBuffer);

   errno = 0; 
}

CMQException::~CMQException() throw()
{

}

void CMQException::What(const std::string &_what_) throw()
{
   _What = _what_;    
}


std::string CMQException::Now() throw()
{
   CHAR            szStamp[64];
   struct	tm 		tme;
   time_t 			now;

   CRT_time(&now);
   CRT_localtime_r(&now, &tme);

   CRT_snprintf((PSTR) szStamp,sizeof(szStamp), "%04d/%02d/%02d %02d:%02d:%02d", \
                tme.tm_year+1900,tme.tm_mon+1,tme.tm_mday,tme.tm_hour,tme.tm_min, \
                tme.tm_sec );

   return szStamp;
}

CPSTR CMQException::what() const throw()
{
   return  (_What.c_str());
}

