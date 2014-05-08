/* -*- C++ -*- */


/**
 * @file Process.hpp
 * @author qingant
 */

#ifndef CPROCESS_H
#define CPROCESS_H
#pragma once
#include <cstub.h>
#include <map>
#include <string>
#include <vector>
#include "Object.hpp"
#include "Exception.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      class CProcess:public CNonCopyAble
      {
      public:
         typedef VOID (*sig_t) (int);
         typedef std::map<std::string, std::string> OPTMAP;
         typedef std::pair<size_t, PSTR> ARGPAIR;
         typedef std::vector<ARGPAIR> ARGPAIRS;
      public:
         CProcess(INT argc, CHAR **argv, CHAR **envp, const std::string &optfmt);
         CProcess();
         virtual ~CProcess();

         // INT SignSignals(const std::vector<INT> &ign_signals);
         bool  ExistOption(const std::string&);
         const std::string& GetOption(const std::string&);
         bool SetArgument(INT, const std::string&);
         bool SetArgument(INT, const CHAR*);
         int SetArgument(const CHAR*);
         virtual INT Run();
         INT GetPID();
         INT GetPPID();
         bool SendSignalToSelf(INT sig_t);
         void SetUmask(mode_t mask = 0);
         bool SetRLimit(INT res, const struct rlimit *rlimit);
         struct rlimit GetRLimit(INT res);
         void Initialize(INT argc, CHAR **argv, CHAR **envp, const std::string &optfmt);


      public:
          std::vector<std::string>  Arguments;
      private:
         bool ParseOpt(const std::string&);
      private:
         INT Argc;
         CHAR **Argv;
         CHAR **Envp;
         OPTMAP Options;
         std::vector<std::string> MainArg;
         ARGPAIRS ArgPairs;
         // std::auto_ptr<COptions> _COpts;
      };

      class IApp : public CNonCopyAble
      {
      public:
         IApp(INT argc, CHAR **argv, CHAR **envp, const std::string &optfmt);
          ~IApp();
          virtual void Run() = 0;
      protected:
         std::auto_ptr<CProcess>  _Process;
      }; 
   } /// namespace GalaxyRT
} /// namespace Galaxy
    
#endif /* CPROCESS_H */
