#include <stdio.h>
#include <string>
#include "MTRConfigure.hpp"
#include "Configure.hpp"
#include "MTRxmlConfigure.hpp"


int main(int argc,char **argv)
{
    if(argc < 4 )
    {
        printf("the paramter is least 2!\n");
        exit(-1);
    }
    int ch;
    std::string pltFile;
    std::string SaveFile;
    std::string LoadFile;
    std::string GroupId;
    while(( ch = getopt( argc, argv, "s:S:l:L:g:G:f:F:" ) ) != EOF )
    {
    	switch(ch)
    	{
    		case 's':
    		case 'S':
    		{
    			SaveFile += optarg;
    			break;
    		}
    		case 'l':
    		case 'L':
    		{
    			LoadFile += optarg;
    			break;
    		}
    		case 'g':
    		case 'G':
    		{
    			GroupId += optarg;
    			break;
    		}
    		case 'f':
    		case 'F':
    		{
    			pltFile += optarg;
    			break;
    		}
    		case '?':
    		{
    			printf("Uage :\n");
    			printf("./test -l GalaGroup0000.xml -g 0 -f plf.cfg\n");
    			printf("./test -s GalaGroup0000.xml -g 0 -f plf.cfg\n");
    			exit(-1);
    			break;
    		}
    		default:
    		{
    			printf("Uage :\n");
    			printf("./test -l GalaGroup0000.xml -g 0 -f plf.cfg\n");
    			printf("./test -s GalaGroup0000.xml -g 0 -f plf.cfg\n");
    			exit(-1);
    		}
    	}
    }
    char *szPath = CRT_strrchr((char *)pltFile.c_str(),'/');
    if(!szPath)
    {
        char path[1024];
        memset(path,0x00,sizeof(path));
        snprintf(path,sizeof(path),"%s/cfg/shmfile/%s",getenv("HOME"),pltFile.c_str());
        if(CRT_access(path,F_OK) == -1)
        {
            exit(-1);
        }
        pltFile = path;
    }
    ConfigurePlt *pConf = new ConfigurePlt(pltFile,PROT_READ|PROT_WRITE); //构造平台对象

    /*char LdDir[1024];
    memset(LdDir,0x00,sizeof(LdDir));
    snprintf(LdDir,sizeof(LdDir),"%s/cfg/%s",getenv("HOME"),LoadFile.c_str());*/

    if(NULL == pConf)
    {
        exit(-1);
    }
    if(LoadFile.size() !=0 )
    {
        szPath = CRT_strrchr((char *)LoadFile.c_str(),'/');
        if(!szPath)
        {
            char path[1024];
            memset(path,0x00,sizeof(path));
            snprintf(path,sizeof(path),"%s/cfg/shmfile/%s",getenv("HOME"),LoadFile.c_str());
            if(CRT_access(path,F_OK) == -1)
            {
                exit(-1);
            }
            LoadFile = path;
        }
    	pConf->LoadGRP(LoadFile,atoi(GroupId.c_str()));
    }
    if(SaveFile.size() !=0 )
    {
        szPath = CRT_strrchr((char *)SaveFile.c_str(),'/');
        if(!szPath)
        {
            char path[1024];
            memset(path,0x00,sizeof(path));
            snprintf(path,sizeof(path),"%s/cfg/shmfile/%s",getenv("HOME"),SaveFile.c_str());
            if(CRT_access(path,F_OK) == -1)
            {
                exit(-1);
            }
            SaveFile = path;
        }
    	pConf->SaveGRP(SaveFile,atoi(GroupId.c_str()));
    }

    delete pConf;
    return 0;

}

