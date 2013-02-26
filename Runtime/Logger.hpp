/**
 * @file Logger.hpp
 * @author luozhihui@arzan.cn
 *
 * platform log----> CLowlevelLoggerWriter(no buffer, It is thread safety)
 * application log---->CBufferLoggerWriter(have buffer, It is not thread safety)
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "Sync.hpp"
#include "Buffer.hpp"

#pragma once

namespace Galaxy
{
    namespace GalaxyRT
    {
	
//#define CRT_gethostname gethostname
#define CRT_strftime    strftime

        enum LOGLEVEL{
            DEBUG=0,
            INFO=1,
            WARN=2,
            ERROR=3,
            FATAL=4,
        };

        class CLoggerBaseFomatter:public CNonCopyAble
        {
        public:
            CLoggerBaseFomatter();
            virtual ~CLoggerBaseFomatter();
            virtual std::string Formatter(CPVOID ptr, size_t nbytes, LOGLEVEL level) const;
            std::string GetTimeStamp() const;
        };

        class CLoggerBaseWriter:public CNonCopyAble
        {
        public:
            virtual void WriteLogger(LOGLEVEL, CPVOID, size_t) const = 0;
            virtual void WriteReset(const std::string &logname,  size_t loglimit, size_t buflen)=0;
            virtual void LoggerRename(const std::string &logname) = 0;
            virtual void LoggerResetSwitcherToTrade(PSTR switcher) = 0;
            virtual void LoggerResetSwitcherToPlatform(PSTR switcher) = 0;
            virtual void ResetBufferDepth(size_t buflen)=0;
            virtual ~CLoggerBaseWriter(){};
        protected:
            void CreateLogPath(std::string filepath) const
            {
                for(std::string::iterator it=filepath.begin(); it!=filepath.end(); ++it)
                {
                    if(*it == '\\')
                    {
                        *it = '/';
                    }
                }
                std::string tmp;
                std::size_t found;
                found=filepath.find_first_of('/');
                while (found!=std::string::npos)
                {
                    tmp = filepath.substr(0,found);
                    if(!tmp.empty())
                    {
                        if(access(tmp.c_str(), F_OK) != 0)
                        {
                            if(-1 == mkdir(tmp.c_str(), 0755))
                            {
                                throw;
                            }
                        }
                    }
                    found=filepath.find_first_of('/', found+1);
                }
            }
        };

//the Platform log.Thread safety, but never reset
        class CLowlevelLoggerWriter:public CLoggerBaseWriter
        {
        public:
            CLowlevelLoggerWriter(const std::string &logname, PSTR switcher, size_t loglimit = 1024*1024*1024);
            virtual ~CLowlevelLoggerWriter();
            void WriteLogger(LOGLEVEL level, CPVOID ptr, size_t nbytes) const;
            bool LoggerSwitch(LOGLEVEL level) const;
            void LoggerRename(const std::string &logname);
            bool OutOfDate() const;

        private:
            void Close();
            void WriteReset(const std::string &logname,  size_t loglimit, size_t buflen)
            {std::cout << logname << loglimit << buflen << std::endl;};
            void LoggerResetSwitcherToPlatform(PSTR switcher)
            {std::cout << switcher << std::endl;};
            void LoggerResetSwitcherToTrade(PSTR switcher)
            {std::cout << switcher << std::endl;};
            void ResetBufferDepth(size_t buflen)
            {std::cout << buflen << std::endl;};
        private:
            mutable int             m_logger;
            mutable std::string     m_logname;
            PSTR                    m_switcher;
            size_t                  m_limit;
        };



//the application log. It is not thread safety,because application it is Single-thread.
        class CBufferLoggerWriter:public CLoggerBaseWriter
        {
        public:
            CBufferLoggerWriter(const std::string &logname, PSTR switcher, size_t loglimit = 1024*1024*1024, size_t buflen=256);
            void WriteReset(const std::string &logname,  size_t loglimit, size_t buflen) ;
            void WriteLogger(LOGLEVEL level, CPVOID ptr, size_t nbytes) const;
            bool LoggerSwitch(LOGLEVEL level) const;
            void LoggerRename(const std::string &logname);
            void LoggerResetSwitcherToPlatform(PSTR switcher);
            void LoggerResetSwitcherToTrade(PSTR switcher);
            void Flush() const;
            void ResetBufferDepth(size_t buflen);
            virtual ~CBufferLoggerWriter();
        private:
            void Close();
            void WriteLogger(CPVOID ptr, size_t nbytes) const;
            void Open() const;
        private:
            mutable INT                     m_logger;
            std::string                     m_logname;
            PSTR                            m_switcher;
            BYTE                            m_tradeSwitcher[5];
            size_t                          m_limit;
            mutable std::auto_ptr<CBuffer>  m_pCBuffer;
//      CBuffer                 m_CBuffer;
            CPthreadMutex                   m_mutex;
        };

        class CLogger: public CObject
        {
        public:
            CLogger(CLoggerBaseFomatter*, CLoggerBaseWriter*);

        public:
            enum   LOG_REDIRECT {LOG_STDOUT, LOG_STDERR, LOG_LOGWRITTER };
            static const CLogger &GetLogger(CLoggerBaseFomatter* formatter = NULL, CLoggerBaseWriter *writer = NULL);
            void Logger(LOGLEVEL level, CPVOID ptr, size_t nbytes) const;
            void Logger(LOGLEVEL level, CPVOID ptr) const;
            void Redirect(/*LOG_REDIRECT type*/);
            void FormatLogger(LOGLEVEL level, const std::string& format, size_t nbytes, ...) const;
            void _FormatLogger(LOGLEVEL level, CPSTR format, size_t nbytes, va_list) const;
            //dump the transaction file
            void LoggerDump(CPVOID ptr, size_t nbytes, LOGLEVEL level) const;
            //reset the writer,because maybe the next transaction
            void ResetWriter(const std::string &logname, size_t loglimit=1024*1024, size_t buflen=256) const;
            void LoggerResetSwitcherToPlatform(PSTR switcher);
            void LoggerResetSwitcherToTrade(PSTR switcher);
            void LoggerError(CPVOID ptr, size_t nbytes) const;
            void LoggerError(CPVOID ptr, ...) const;
            void LoggerDebug(CPVOID ptr, size_t nbytes) const;
            void LoggerDebug(CPVOID ptr, ...) const;

            void LoggerWarn(CPVOID ptr, size_t nbytes) const;
            void LoggerWarn(CPVOID ptr,  ...) const;

            void LoggerFatal(CPVOID ptr, size_t nbytes) const;
            void LoggerFatal(CPVOID ptr, ...) const;

            void LoggerInfo(CPVOID ptr, size_t nbytes) const;
            void LoggerInfo(CPVOID ptr,  ...) const;

            void LoggerRename(const std::string &logname);
            void ResetBufferDepth(size_t buflen);
            virtual ~CLogger();
        private:
            std::string GetDumpString(CPVOID ptr, size_t nbytes) const;
            std::string INTL_MakeDump(CPSTR Data, UINT Len) const;
            std::string INTL_GenHexDump(PSTR Src, UINT Len) const;
            PBYTE INTL_B2HEX(BYTE pHex[2], BYTE bVal) const;

        private:
            mutable std::auto_ptr<CLoggerBaseFomatter>          m_formatter;
            mutable std::auto_ptr<CLoggerBaseWriter>            m_writer;
            size_t                                              NBYTES; // 
        };

        typedef CLogger CDefaultLogger;

    } /// namespace GalaxyRT
} /// namespace Galaxy

#endif /* LOGGER_H */
