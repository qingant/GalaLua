/* -*- C++ -*- */

/* Time-stamp: <2012-11-05 15:17:59 星期一 by matao> */

/**
 * @file    Logger.cpp
 * @author  Ma Tao <qingant@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 *
 * @section DESCRIPTION
 *
 *
 */


#include "Logger.hpp"
#include "Utils.hpp"
#include "Sync.hpp"
#include <stdexcept>

namespace Galaxy
{
    namespace GalaxyRT
    {

#define CRT_rename rename

        CLoggerBaseFomatter::CLoggerBaseFomatter()
        {
        }

        CLoggerBaseFomatter::~CLoggerBaseFomatter()
        {
        }

        std::string CLoggerBaseFomatter::GetTimeStamp() const
        {
            std::string tmp(64,0);
            PSTR szStamp = &tmp[0];
            struct  tm      tme;
            time_t          now;
            CRT_time(&now);
            CRT_localtime_r(&now, &tme);
            CRT_snprintf((PSTR) szStamp,tmp.size(), "[%04d/%02d/%02d %02d:%02d:%02d] - [", \
                         tme.tm_year+1900,tme.tm_mon+1,tme.tm_mday,tme.tm_hour,tme.tm_min, \
                         tme.tm_sec );
            size_t timeMsgLen = CRT_strlen(szStamp);
            tmp.resize(timeMsgLen);
            return tmp;
        }


        std::string CLoggerBaseFomatter::Formatter(CPVOID ptr, size_t nbytes, LOGLEVEL level) const
        {
            // CHAR szStamp[64] = {0};
            // struct   tm      tme;
            // time_t           now;
            // CRT_time(&now);
            // CRT_localtime_r(&now, &tme);

            static CPSTR type[] = {"D][", "I][", "W][", "E][", "F]["};

            std::string timeinfo = GetTimeStamp();
            size_t timeMsgLen = timeinfo.size();
            //printf("timeMsgLen: %d\n", timeMsgLen);
            size_t msgLen = timeMsgLen + nbytes + 5;
            std::string tmp(msgLen, 0);
            PSTR szTmp = &tmp[0];
            CRT_memcpy(szTmp, &timeinfo[0], timeMsgLen);
            CRT_memcpy(szTmp+timeMsgLen, type[(size_t)level], 3);
            CRT_memcpy(szTmp+timeMsgLen+3, ptr, nbytes);
            CRT_memcpy(szTmp+timeMsgLen+3+nbytes, "]\n", 2);
            return tmp;
        }

        CLowlevelLoggerWriter::CLowlevelLoggerWriter(const std::string &logname, PSTR switcher, size_t limit)
            :m_logger(-1),
             m_logname(logname),
             m_switcher(switcher),
             m_limit(limit)
        {
                this->CreateLogPath(m_logname);
                if ((m_logger = CRT_open(m_logname.c_str(), O_CREAT|O_RDWR|O_APPEND, 0664)) == -1)
                {
                    THROW_SYSERROR();
                }
                else
                {
                    //you can create buffer
                    struct stat sta;
                    if (CRT_fstat(m_logger, &sta) == -1)
                    {
                        THROW_SYSERROR();
                    }
                    if ((size_t)sta.st_size > m_limit)
                    {
                        if (CRT_ftruncate(m_logger, 0) == -1)
                        {
                            THROW_SYSERROR();
                        }
                    }
                }
        }

//
//void CLowlevelLoggerWriter::WriteReset(/*const std::string &logname,  size_t loglimit, size_t
//buflen*/)
//{
//    std::cerr << "platform never reset" << std::endl;
//}

        void CLowlevelLoggerWriter::WriteLogger(LOGLEVEL level, CPVOID ptr, size_t nbytes) const
        {
            if (!LoggerSwitch(level))
            {
                return;
            }
            // if (m_logger == -1)
            // {
            
            // }
            if (CRT_write(m_logger, ptr, nbytes)!= (INT)nbytes)
            {
                THROW_GENERALERROR(CException, "Write Call In Low Level Logger Failed");
            }
            //GALA_DEBUG("Logger : [%d]", m_logger);
            // CRT_close(m_logger);
            // m_logger = -1;
        }

        bool CLowlevelLoggerWriter::LoggerSwitch(LOGLEVEL level) const
        {
            if (m_switcher == NULL)
            {

                return true;
            }
            else
            {
                //printf("m_switch [%d]\n", m_switcher[0]);
                
                return (m_switcher[0] <= level);
            }
        }

        void CLowlevelLoggerWriter::LoggerRename(const std::string &logname)
        {
            if(-1 == CRT_rename(m_logname.c_str(), logname.c_str()))
            {
                THROW_SYSERROR();
            }
            m_logname = logname;
        }
//
//void CLowlevelLoggerWriter::LoggerResetSwitcherToPlatform(/*PSTR switcher*/)
//{
//    //should not call
//}

//void CLowlevelLoggerWriter::LoggerResetSwitcherToTrade(/*PSTR switcher*/)
//{
//    //should not be call
//}

//check the switcher before call
        bool CLowlevelLoggerWriter::OutOfDate() const
        {
            time_t  timep;
            struct tm m;
            std::string buf(50,0);

            CRT_time(&timep);
            CRT_localtime_r(&timep, &m);
            CRT_strftime(&buf[0], 50, "%Y%m%d", &m);

            if(m_logname.size() < 10)
            {
                m_logname.insert(m_logname.size()-4, buf);
            }
            else
            {
                std::string subStr(m_logname.substr(m_logname.size()-10, 6));
                if(0 == buf.compare(subStr))
                {
                    return true;
                }
                m_logname.replace(m_logname.size()-12, 8, buf);
            }

            return true;
        }


        void CLowlevelLoggerWriter::Close()
        {
            if (m_logger == -1)
            {
                return;
            }
            else
            {
                if (CRT_close(m_logger) == -1)
                {
                    THROW_SYSERROR();
                }
            }
        }


        CLowlevelLoggerWriter::~CLowlevelLoggerWriter()
        {
            try
            {
                Close();
            }
            catch(...)
            {
                printf("Low Level Logger Close Exception\n");
            }
        }

        CLogger::CLogger(CLoggerBaseFomatter *formatter, CLoggerBaseWriter *writer)
            :m_formatter(formatter),
             m_writer(writer)
        {
            NBYTES = 256;
        }



        const CLogger &CLogger::GetLogger(CLoggerBaseFomatter* formatter, CLoggerBaseWriter *writer)
        {
            static CLogger _logger(formatter, writer);
            return _logger;

        }



        void CLogger::Logger(LOGLEVEL level, CPVOID ptr, size_t nbytes) const
        {
            if (ISNULL(ptr) && nbytes != 0)
            {
                THROW_GENERALERROR(CException, "WRONG ARGV");
            }
            std::string msg = m_formatter->Formatter(ptr, nbytes, level);
            m_writer->WriteLogger(level, (CPSTR)&msg[0], msg.size());
        }


        void CLogger::Logger(LOGLEVEL level, CPVOID ptr) const
        {
            Logger(level, ptr, CRT_strlen((CPSTR)ptr));
        }

        void CLogger::_FormatLogger(LOGLEVEL level, CPSTR format, size_t nbytes, va_list ap) const
        {
            std::string tmpBuf(nbytes, 0);
            PSTR tmp = &tmpBuf[0];
            CRT_vsnprintf(tmp, nbytes, format, ap);
            size_t msgLen = CRT_strlen(tmp);
            Logger(level, tmp, msgLen);
        }

        void CLogger::FormatLogger(LOGLEVEL level, const std::string& format, size_t nbytes, ...) const
        {
            // std::string tmpBuf(nbytes, 0);
            // PSTR tmp = &tmpBuf[0];
            // va_list argp;
            // va_start(argp, nbytes);
            // CRT_vsnprintf(tmp, nbytes, format.c_str(), argp);
            // va_end(argp);
            // size_t msgLen = CRT_strlen(tmp);
            // Logger(level, tmp, msgLen);
            va_list argp;
            va_start(argp, nbytes);
            _FormatLogger(level, format.c_str(), nbytes, argp);
            va_end(argp);
        }

        void CLogger::LoggerError(CPVOID ptr,  ...) const
        {

            va_list argp;
            va_start(argp, ptr);
            _FormatLogger(ERROR, (CPSTR)ptr, NBYTES, argp);
            va_end(argp);
        }
        void CLogger::LoggerWarn(CPVOID ptr,  ...) const
        {
            va_list argp;
            va_start(argp, ptr);
            _FormatLogger(WARN, (CPSTR)ptr, NBYTES, argp);
            va_end(argp);
        }

        void CLogger::LoggerInfo(CPVOID ptr, ...) const
        {
            va_list argp;
            va_start(argp, ptr);
            _FormatLogger(INFO, (CPSTR)ptr, NBYTES, argp);
            va_end(argp);
        }

        void CLogger::LoggerFatal(CPVOID ptr,  ...) const
        {
            va_list argp;
            va_start(argp, ptr);
            _FormatLogger(FATAL, (CPSTR)ptr, NBYTES, argp);
            va_end(argp);
        }

        void CLogger::LoggerDebug(CPVOID ptr,  ...) const
        {
            va_list argp;
            va_start(argp, ptr);
            _FormatLogger(DEBUG, (CPSTR)ptr, NBYTES, argp);
            va_end(argp);
        }

        CLogger::~CLogger()
        {
        }

        void CLogger::Redirect(/*LOG_REDIRECT type*/)
        {
            std::cerr << "Redirect" << std::endl;
        }

/*
 * writer dump log
 */
        void CLogger::LoggerDump(CPVOID ptr, size_t nbytes, LOGLEVEL level) const
        {
            std::string msg = GetDumpString(ptr, nbytes);
            Logger(level, msg.c_str(), msg.size());
        }

/*
 * create dump log string
 */
        std::string CLogger::GetDumpString(CPVOID ptr, size_t nbytes) const
        {
            return INTL_MakeDump((const char *)ptr, nbytes);
        }

        std::string CLogger::INTL_MakeDump(CPSTR Data, UINT Len) const
        {
            if(ISNULL(Data))
            {
                return NULL;
            }

            return INTL_GenHexDump((PSTR)Data,Len);
        }



        std::string CLogger::INTL_GenHexDump(PSTR Src, UINT Len) const
        {
            BYTE    szHex[128];
            BYTE    szDBL[4];
            BYTE    szIDX[16];
            PBYTE   pOfs;
            PBYTE   pDbl;
            PBYTE   pSep;
            PBYTE   pDsp;
            PBYTE   pSrc;
            INT     iLen;
            UINT    cLen;
            UINT    i,j;

            pOfs    = szHex;
            pDbl    = &szHex[11];
            pSep    = &szHex[59];
            pDsp    = &szHex[61];

            if(ISNULL(Src)||(Len == 0))
            {
                return std::string();
            }

            std::string dumpStr;
            dumpStr.append("\n--------------------------------- [ DUMP ] ----------------------------------\n");
            for(i=0,iLen = (INT)Len;iLen > 0;(iLen -= 16),i++)
            {
                pSrc = (PBYTE)&Src[i*16];
                cLen = (iLen < 16)? iLen : 16 ;

                memset(szHex,' ',77);
                szHex[77]=0;

                snprintf((PSTR)szIDX,11,"%08xh:",(i * 16));
                memcpy(pOfs,szIDX,10);

                for(j=0;j<cLen;j++)
                {
                    INTL_B2HEX(szDBL,(BYTE)pSrc[j]);
                    memcpy(&pDbl[j*3],szDBL,2);

                    pDsp[j] = pSrc[j];
                    if((pDsp[j]=='\r')||(pDsp[j]=='\n')||(pDsp[j]=='\t'))
                    {
                        pDsp[j]=' ';
                    }
                    else if(pDsp[j]==0)
                    {
                        pDsp[j]='.';
                    }
                }

                memcpy(pSep,"; ",2);

                std::string tmpStr((const char *)szHex);
                tmpStr.append("\n");
                dumpStr += tmpStr;
            }
            dumpStr.erase(dumpStr.end()-1, dumpStr.end());
            dumpStr.append("\n-----------------------------------------------------------------------------");

            return dumpStr;
        }



        PBYTE CLogger::INTL_B2HEX(BYTE pHex[2], BYTE bVal) const
        {

            BYTE        HI_BYTE;
            BYTE        LO_BYTE;


            HI_BYTE = (BYTE)(bVal >> 4 ) & (BYTE)0x0F;
            LO_BYTE = (BYTE)(bVal & (BYTE)0x0F);

            if(HI_BYTE > 0x09)
            {
                pHex[0]=HI_BYTE + 0x37;
            }
            else
            {
                pHex[0]=HI_BYTE + 0x30;
            }

            if(LO_BYTE > 0x09)
            {
                pHex[1]=LO_BYTE + 0x37;
            }
            else
            {
                pHex[1]=LO_BYTE + 0x30;
            }

            return (pHex);
        }



        void CLogger::ResetWriter(const std::string &logname,  size_t loglimit, size_t buflen) const
        {
            m_writer->WriteReset(logname, loglimit, buflen);
        }

        void CLogger::LoggerResetSwitcherToPlatform(PSTR switcher)
        {
            m_writer->LoggerResetSwitcherToPlatform(switcher);
        }

        void CLogger::LoggerResetSwitcherToTrade(PSTR switcher)
        {
            m_writer->LoggerResetSwitcherToTrade(switcher);
        }



        void CLogger::LoggerError(CPVOID ptr, size_t nbytes)const
        {
            Logger(ERROR, ptr, nbytes);
        }



        void CLogger::LoggerDebug(CPVOID ptr, size_t nbytes)const
        {
            Logger(DEBUG, ptr, nbytes);
        }



        void CLogger::LoggerWarn(CPVOID ptr, size_t nbytes)const
        {
            Logger(WARN, ptr, nbytes);
        }



        void CLogger::LoggerFatal(CPVOID ptr, size_t nbytes)const
        {
            Logger(FATAL, ptr, nbytes);
        }



        void CLogger::LoggerInfo(CPVOID ptr, size_t nbytes)const
        {
            Logger(INFO, ptr, nbytes);
        }



        void CLogger::LoggerRename(const std::string &logname)
        {
            m_writer->LoggerRename(logname);
        }


        void CLogger::ResetBufferDepth(size_t buflen)
        {
            m_writer->ResetBufferDepth(buflen);
        }

        CBufferLoggerWriter::CBufferLoggerWriter(const std::string & logname, PSTR switcher, size_t loglimit, size_t buflen)
            :m_logger(-1),
             m_logname(logname),
             m_switcher(switcher),
             m_limit(loglimit),
             m_pCBuffer(new CBuffer(buflen))
        {
            // std::cerr << "CLowLevelLoggerWriter" << std::endl;
            CRT_memset(m_tradeSwitcher, 0x00, sizeof(m_tradeSwitcher));
        }


        void CBufferLoggerWriter::WriteReset(const std::string &logname,size_t loglimit, size_t buflen)
        {
            //reset the logger in the trade
            Close();
            m_logname = logname;
            m_limit = loglimit;
            this->m_pCBuffer.reset(new CBuffer(buflen));
            Open();
        }


        void CBufferLoggerWriter::WriteLogger(LOGLEVEL level, CPVOID ptr, size_t nbytes) const
        {
            if (!LoggerSwitch(level))
            {
                return;
            }
            //CBuffer not
            if (!m_pCBuffer->Writeable(nbytes))
            {
                //flush
                Flush();

                if (!m_pCBuffer->Writeable(nbytes))
                {
                    WriteLogger(ptr, nbytes);
                }
                else
                {
                    m_pCBuffer->Write(ptr, nbytes);
                }
            }
            else
            {
                m_pCBuffer->Write(ptr, nbytes);
            }
        }

        void CBufferLoggerWriter::Open() const
        {
            this->CreateLogPath(m_logname);

            if ((m_logger = CRT_open(m_logname.c_str(), O_CREAT|O_RDWR|O_APPEND, 0664)) == -1)
            {
                THROW_SYSERROR();
            }
            else
            {
                struct stat sta;
                if (CRT_fstat(m_logger, &sta) == -1)
                {
                    THROW_SYSERROR();
                }
                if ((size_t)sta.st_size > m_limit)
                {
                    if (CRT_ftruncate(m_logger, 0) == -1)
                    {
                        THROW_SYSERROR();
                    }
                }
            }
        }


        void CBufferLoggerWriter::WriteLogger(CPVOID ptr, size_t nbytes) const
        {
            if (m_logger == -1)
            {
                Open();
            }

            if (CRT_write(m_logger, ptr, nbytes)!= (INT)nbytes)
            {
                THROW_GENERALERROR(CException, "Write Call In Low Level Logger Failed");
            }
        }



        bool CBufferLoggerWriter::LoggerSwitch(LOGLEVEL level) const
        {
            if (m_switcher == NULL)
            {
                return true;
            }
            else
            {
                //printf("m_witcher [%d]", m_switcher[0]);
                return (m_switcher[0] <= level);
            }
        }

        void CBufferLoggerWriter::LoggerRename(const std::string &logname)
        {
            this->CreateLogPath(logname);
            if(-1 == CRT_rename(m_logname.c_str(), logname.c_str()))
            {
                THROW_SYSERROR();
            }
            m_logname = logname;
        }

        void CBufferLoggerWriter::LoggerResetSwitcherToPlatform(PSTR switcher)
        {
            this->m_switcher = switcher;
        }

        void CBufferLoggerWriter::LoggerResetSwitcherToTrade(PSTR switcher)
        {
            CRT_memcpy(m_tradeSwitcher, switcher, 5);
            this->m_switcher = (PSTR)&m_tradeSwitcher[0];
        }

        void CBufferLoggerWriter::Flush() const
        {
            std::string tmp(m_pCBuffer->Read());
            WriteLogger((CPSTR)&tmp[0], tmp.size());
        }



        void CBufferLoggerWriter::ResetBufferDepth(size_t depth)
        {
            std::string context(this->m_pCBuffer->ResetBufferDepth(depth));
            WriteLogger((CPSTR)&context[0], context.size());
        }



        CBufferLoggerWriter::~CBufferLoggerWriter()
        {
            Close();
            //GALA_DEBUG("~CBufferLoggerWriter");
        }



        void CBufferLoggerWriter::Close()
        {
            Flush();
            CRT_close(m_logger);
        }
    } /// namespace GalaxyRT
} /// namespace Galaxy
