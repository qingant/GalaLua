#include "zip.h"

class Gar
{
    public:
        Gar(const std::string &garfile,int flag=0)
        {
            int err=-1;
            _gar=zip_open(garfile.c_str(),flag,&err);
            if (_gar==NULL)
            {
                throw "not a valid gar";
            }
        }

        ~Gar()
        {
            zip_close(_gar);
        }

        std::string getGarMain()
        {
            return Read("main.lua");
        }

        std::string Read(const char *file)
        {
            struct zip_stat st;
            struct zip_file *zf=NULL;

            if (zip_stat(_gar,file,0,&st)<0)
            {
                throw "zip_stat error";
            }

            zf=zip_fopen(_gar,file,0);
            if (zf==NULL)
            {
                throw "zip_fopen error";
            }

            std::string buf(st.size,0); 

            if (zip_fread(zf,&buf[0],st.size)!=st.size)
            {
                throw "zip_fread error";
            }

            return buf;
        }

    private:
        struct zip *_gar;

};

