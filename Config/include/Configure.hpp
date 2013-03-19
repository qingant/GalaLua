
#ifndef GALA_CONFIGURE_WRKGRP_PLT_H_P_P
#define GALA_CONFIGURE_WRKGRP_PLT_H_P_P
#include <stdio.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include "Runtime/IPC.hpp"
#include "Runtime/File.hpp"
#include "MTRConfigure.hpp"
#include "MTRxmlConfigure.hpp"


#define USABLE_DISABLED 0x0
#define USABLE_ENABLED 0xAB


class Configure:public Galaxy::GalaxyRT::CNonCopyAble
{
public:
    Configure(const std::string &path,int prot = PROT_READ);
    ~Configure();
protected:
    bool    _Validication;
    PVOID   GetSHMEntry();
    bool    OpenFileAndMapping(const std::string &,int);
    bool    IsExistPath(const std::string &Path);
    std::string  _Path;
    std::auto_ptr<Galaxy::GalaxyRT::CFileMapping>  _SHMObject;
};

class GalaConfigureGRP:public Configure
{
public:
    GalaConfigureGRP(const std::string &,int prot = PROT_READ);
    ~GalaConfigureGRP();
    CMonitorConfigure *GetObject();
protected:
    void   Initialize();
private:
    //int _Size;
    std::auto_ptr<CMonitorConfigure>    _ConfObject;
};

class ConfigurePlt:public Configure
{
public:
    ConfigurePlt(const std::string &path,int prot = PROT_READ);
    ~ConfigurePlt();
    void Put(const std::string &,char *);
    std::string Get(const std::string &);
    std::pair<std::string,std::string> Get_ex(const std::string &);
    PGalaGrpCfg GetGrpCfg(int);
    int GetWRKGRPStatus(int);
    void Load(const std::string &);
    void Save(const std::string &);
    void LoadGRP(const std::string &,int);
    void SaveGRP(const std::string &,int);
    PBYTE GetLowerLeavel(int);
protected:
	std::map<xmlDocPtr,xmlNodePtr> NodeEntry();
	bool isMatch(int id ,xmlNodePtr node);
    void   Initialize();
    std::string WRKGRPReady(const std::string &path);
    PMonitorConfigureSet GetWorkGrpSet();
private:
    //int _Size;
    std::string _PltFile;
    int _Prot;
    int GroupId;
    std::auto_ptr<GalaConfigureGRP> _GRP;

};

#endif

