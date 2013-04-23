/* -*- C++ -*- */
// Time-stamp: <2012-04-17 09:50:58 星期二 by OCaml>
/**
* @file    Process.cpp
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

#include "Process.hpp"
#include "File.hpp"
#include "IPC.hpp"

namespace Galaxy
{
    namespace GalaxyRT
    {

        CProcess::CProcess(INT argc, CHAR **argv, CHAR **envp, const std::string &optfmt)
            :Argc(argc),
            Argv(argv),
            Envp(envp)
        {
            this->Initialize(argc, argv, envp, optfmt);

            // InitConfig();

        }


        // CProcess::CProcess(INT argc, CHAR **argv):Argc(argc), Argv(argv)
        // {
        //     ParseOpt();
        //     InitConfig();
        // };
        CProcess::CProcess()
            :Argc(0),
            Argv(NULL),
            Envp(NULL)
        {
        }

        void CProcess::Initialize(INT argc, CHAR **argv, CHAR **envp, const std::string &optfmt)
        {

            this->Argc = argc;
            this->Argv = argv;
            this->Envp = envp;
            for (INT i = 0; i != argc; ++i)
            {
                ArgPairs.push_back(std::make_pair((size_t)CRT_strlen(argv[i])+1, argv[i]));
                Arguments.push_back(argv[i]);
            }

            // _COpts.reset(new COptions(argc, argv, optfmt.c_str()));
            // for (UINT i = 0; i != _COpts->Count(); ++i)
            // {
            //     char key[2] = {0};
            //     key[0] = _COpts->operator[](i)->Key();
            //     if (_COpts->operator[](i)->HasVal())
            //     {

            //         Options.insert(std::make_pair(key, _COpts->operator[](i)->Val()));
            //     }
            //     else
            //     {
            //         Options.insert(std::make_pair(key, ""));
            //     }
            // }
            // this->MainArg = _COpts->_MainArgs;
            ParseOpt(optfmt);

            //clear for ps
            for(int i=1; i<Argc; i++)
            {
                CRT_memset(Argv[i], 0x00, ArgPairs[i].first);
            }
        }

        CProcess::~CProcess()
        {
            // printf("%s\n", __PRETTY_FUNCTION__);

        };

        INT CProcess::Run()
        {
            return 0;
        }

        // INT CProcess::AddSignalHandle(INT sig, CProcess::sig_t func)
        // {
        //     CRT_signal(sig, func);
        //     return 0;
        // };

        // INT CProcess::SignSignals(const std::vector<INT> &ign_signals)
        // {
        //     for (size_t i = 0; i != ign_signals.size(); ++i)
        //     {
        //         if (CRT_signal(ign_signals[i],SIG_IGN) == SIG_ERR)
        //         {
        //             THROW_GENERALERROR(CException, "SIG_ERR");
        //         }
        //     }


        //     return 0;
        // }
        bool CProcess::ExistOption(const std::string &name)
        {
            CProcess::OPTMAP::iterator it = Options.find(name);
            if (it == Options.end())
            {
                return false;
            }
            return true;
        }
        const std::string& CProcess::GetOption(const std::string &name)
        {

            return Options[name];

        };


        bool CProcess::ParseOpt(const std::string &)
        {
            std::string tmp(1,0);
            int argc = 1;
            char **argv = Argv;
            for(;argc < Argc;)
            {
                const char *item = argv[argc];
                if ((strlen(item) == 2) && (item[0] == '-') && isalpha(item[1]))
                {
                    tmp[0] = item[1];
                    
                    const char *next = argv[argc+1];
                    if (next[0] != '-')
                    {
                        argc++;
                        Options.insert(std::make_pair(tmp, std::string(next)));
                    }
                    else
                    {

                        Options.insert(std::make_pair(tmp, std::string("")));
                    }

                }
                argc++;
            }
            return true;
        }
        // bool CProcess::ParseOpt(const std::string &optfmt)
        // {
        //     INT argc = Argc;
        //     //CHAR **argv = Argv;
        //     INT state = 0;              // state 0: no leading flag, 1: leading flag
        //     CHAR *leadingArg;
        //     while(argc != 0)
        //     {
        //         CHAR *szArg = Argv[Argc-argc];
        //         if (szArg[0] != '-' && state == 0)
        //         {
        //             MainArg.push_back(std::string(szArg));
        //         }
        //         else if (szArg[0] != '-' && state == 1)
        //         {
        //             std::pair<std::string, std::string> pairArg(leadingArg, szArg);
        //             std::cout<<leadingArg<<std::endl<<szArg<<std::endl;

        //             Options.insert(pairArg);
        //             state = 0;
        //         }
        //         else if (szArg[0] == '-' && state == 0)
        //         {
        //             // printf("leading   %s\n", szArg);

        //             if (CRT_strlen(szArg) != 2)
        //             {
        //                 CHAR flag[2];
        //                 flag[0] = szArg[1];
        //                 flag[1] = 0;
        //                 std::pair<std::string, std::string> pairArg(flag, szArg+2);
        //                 Options.insert(pairArg);
        //             }
        //             else
        //             {
        //                 state = 1;
        //                 leadingArg = szArg+1;
        //                 if (argc == 1)
        //                 {
        //                     std::pair<std::string, std::string> pairArg(leadingArg, "");
        //                     Options.insert(pairArg);
        //                 }
        //             }
        //         }
        //         else
        //         {
        //             std::pair<std::string, std::string> pairArg(leadingArg, "");
        //             Options.insert(pairArg);
        //             state = 0;
        //             continue;
        //         }
        //         --argc;
        //     }
        //     std::string flag(2,0);
        //     for (OPTMAP::iterator it = Options.begin(); it != Options.end(); ++it)
        //     {
        //         size_t iter = optfmt.find(it->first);
        //         // printf("iter : %d\n", iter);
        //         if ((int)iter == -1)
        //         {
        //             std::string tmp(30, 0);
        //             CRT_snprintf(&tmp[0], tmp.size(), "arg [ %s ] not regconized", it->first.c_str());
        //             tmp.resize(CRT_strlen(&tmp[0]));
        //             THROW_GENERALERROR(CException, tmp);
        //         }
        //         else if (optfmt[iter + 1] == ':')
        //         {
        //             if (it->second.empty())
        //             {
        //                 std::string tmp(30, 0);
        //                 CRT_snprintf(&tmp[0], tmp.size(), "[ %s ] must followed by an argument", it->first.c_str());
        //                 tmp.resize(CRT_strlen(&tmp[0]));
        //                 THROW_GENERALERROR(CException, tmp);
        //             }
        //         }
        //     }

        //     return true;
        // };

        bool CProcess::SetArgument(INT index, const std::string& info)
        {

            return SetArgument(index, info.c_str());
        }

        int CProcess::SetArgument(const CHAR* info)
        {
            size_t lenLimit=0;
            size_t len = CRT_strlen(info);
            size_t length=0;
            for(size_t i=0;i<(size_t)Argc && len>length;++i)
            {
                lenLimit = ArgPairs[i].first;
                CRT_memset(Argv[i], 0x00, lenLimit);
                CRT_snprintf(Argv[i], lenLimit, "%s", info+length);
                length +=lenLimit-1;
            }
            return length;
        }

        bool CProcess::SetArgument(INT index, const CHAR* info)
        {
            if (index >= Argc)
            {
                return false;
            }
            size_t lenLimit = ArgPairs[index].first;
            size_t len = CRT_strlen(info);
            len = len>lenLimit?lenLimit:len;
            CRT_memset(Argv[index], 0x00, lenLimit);
            CRT_snprintf(Argv[index], lenLimit, "%s", info);

            return true;
        }

        INT CProcess::GetPID()
        {
            return CRT_getpid();
        }
        INT CProcess::GetPPID()
        {
            return CRT_getppid();
        }
        bool CProcess::SendSignalToSelf(INT sig_t)
        {
            if (CRT_kill(GetPID(), sig_t) == 0)
            {
                return true;
            }
            return false;
        }

        void CProcess::SetUmask(mode_t mask)
        {
            CRT_umask(mask);
        }

        bool CProcess::SetRLimit(INT res, const struct rlimit *rlimit)
        {
            if (CRT_setrlimit(res, rlimit) == 0)
            {
                return true;
            }
            return false;
        }
        struct rlimit CProcess::GetRLimit(INT res)
        {
            struct rlimit rl = {0, 0};
            if (CRT_getrlimit(res, &rl) != 0)
            {
                THROW_SYSERROR();
            }
            return rl;
        }


        // void CProcess::InitConfig()
        // {
        // CFile *pFile = new CFile("/home/afa/test.mmp", O_CREAT|O_RDWR, S_IRWXU);
        // pFile->Ftruncate(1024*1024);
        // CSHM shm(std::auto_ptr<CIOHandle>(pFile), 1024*1024);
        // }

    } /// namespace GalaxyRT
} /// namespace Galaxy
