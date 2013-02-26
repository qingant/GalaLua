// Options.cpp: implementation of the COptions class.
//
//////////////////////////////////////////////////////////////////////

#include "Options.hpp"
#include "Exception.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
      COption::COption(unsigned int key, char* val)
         :_key(key),_val(NULL)
      {
         if(val!=NULL)
         {
			int len = strlen(val);
			if(len>0)
			{
               _val = new char[len+2];
               if(_val==NULL)
               {
                  THROW_EXCEPTION_EX("new buffer error!");
                  return;
               }
               strcpy(_val,val);
			}
         }
      }

      COption::~COption()
      {
         if(_val!=NULL)
         {
			delete [] _val;
			_val = NULL;
         }
      }

      unsigned int COption::Key() const
      {
         return (_key & 0x0000FFFF);
      }

      bool COption::HasVal() const
      {
         return (((_key & 0xFFFF0000)!=0) && (_val!=NULL));
      }

      const char *COption::Val() const
      {
         return _val;
      }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

      class _OPTFMT
      {
      private:
         std::vector<unsigned int> _Indications;

         bool isallowchar(int ch)
         {
            if((ch >= 'a') && (ch <= 'z'))
            {
               return true;
            }
            else if((ch >= 'A') && (ch <= 'Z'))
            {
               return true;
            }
            else if(ch == '?')
            {
               return true;
            }

            return false;
         }

         bool isspacechar(int ch)
         {
            return ((ch == '\r') || (ch == '\n')|| (ch == '\t')|| (ch == ' '));
         }

         void analyse(const char *_opt)
         {
            char *_var;
            unsigned int val = 0;
            for(_var =(char *)_opt;(_var[0]!=0);_var++)
            {
               if(!isspacechar(_var[0]))
               {
                  if(!isallowchar(_var[0]))
                  {
                     THROW_EXCEPTION_EX("invalid fmt!");
                     return;
                  }

                  val = (((int)_var[0]) & 0x0000FFFF);

                  if(_var[1]==':')
                  {
                     val = (val | 0x00010000);
                     _var++;
                  }

                  _Indications.push_back(val);
               }

            }
         }
      public:
         _OPTFMT(const char *_opt)
			:_Indications()
         {
            if(_opt==NULL)
            {
               THROW_EXCEPTION_EX("invalid opt!");
               return;
            }
            else if(_opt[0]==0)
            {
               THROW_EXCEPTION_EX("invalid opt!");
               return;
            }

            analyse(_opt);
         }

         virtual ~_OPTFMT()
         {

         }

         unsigned int operator [] (char key)
         {
            for(unsigned int i=0;i<_Indications.size();i++)
            {
               if((_Indications[i] & 0x0000FFFF) == (unsigned int)key)
               {
                  return (_Indications[i]);
               }
            }

            return (0);
         }

         /*
           unsigned int operator [] (unsigned int Index)
           {
           if(Index < _Indications.size())
           {
           return (_Indications[Index]);
           }
           else
           {
           return (0);
           }
           }
         */

         unsigned int Count()
         {
            return _Indications.size();
         }
      };



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

      COptions::COptions(int argc, char* argv[],const char *_opt)
      {

         Analyse(argc,argv,_opt);
      }

      COptions::~COptions()
      {
         for(unsigned int i=0;i<_Options.size();i++)
         {
			if(_Options[i]!=NULL)
			{
               delete _Options[i];
               _Options[i] = NULL;
			}
         }
      }

      void COptions::Analyse(int argc, char* argv[],const char *_opt)
      {
         _OPTFMT		_fmt(_opt);


         for(int i=1;i<argc;i++)
         {
			char *v=argv[i];
			if(v[0]=='-')
			{
               unsigned int _opt = _fmt[v[1]];
               if(_opt!=0)
               {

                  if((_opt & 0xFFFF0000)!=0)
                  {
                     char *val=NULL;
                     if(v[2]!=0)
                     {
                        val=&v[2];
                     }
                     else
                     {
                        i++;
                        val = argv[i];
                        if((val[0]=='-')||(val[0]=='/'))
                        {
                           val = NULL;
                           THROW_EXCEPTION_EX("option format incorrect!");
                           return;
                        }
                     }

                     COption *option = new COption(_opt,val);
                     if(option==NULL)
                     {
                        THROW_EXCEPTION_EX("new the COption error!");
                        return;
                     }
                     _Options.push_back(option);
                  }
                  else
                  {
                     if((i+1)<argc)
                     {
                        char *val = argv[i+1];
                        if(val[0]!='-')
                        {
                           val = NULL;
                           THROW_EXCEPTION_EX("option format incorrect!");
                           return;
                        }
                     }


                     COption *option = new COption(_opt,NULL);
                     if(option==NULL)
                     {
                        THROW_EXCEPTION_EX("new the COption error!");
                        return;
                     }
                     _Options.push_back(option);
                  }
               }
               else
               {
                  char	msg[32];
                  sprintf(msg,"unknow option [%c]",v[1]);
                  THROW_EXCEPTION_EX(msg);
                  return;
               }
			}
			else
			{
               // char	msg[32];
               // sprintf(msg,"unknow option order [%c]",v[0]);
               // THROW_EXCEPTION_EX(msg);
               _MainArgs.push_back(v);
			}
         }

      }

      const COption *COptions::operator [](char _key)
      {
         for(unsigned int i=0;i< _Options.size();i++)
         {
			if(_Options[i]!=NULL)
			{
               if(_Options[i]->Key()==(unsigned int)_key)
               {
                  return (_Options[i]);
               }
			}
         }

         return (NULL);
      }

      const COption *COptions::operator [](unsigned int index)
      {
         if(index < _Options.size())
         {
			return _Options[index];
         }
         else
         {
			return (NULL);
         }
      }

      unsigned int COptions::Count()
      {
         return _Options.size();
      }



      CArgArray::CArgArray()
         :_Args()
      {

      }

      CArgArray::~CArgArray()
      {
         unsigned int i;
         for(i=0;i<_Args.size();i++)
         {
			if(_Args[i]!=NULL)
			{
               delete [] _Args[i];
               _Args[i] = NULL;
			}
         }
      }

      void CArgArray::Pushback(const char *_str)
      {
         int _len = strlen(_str);
         char *_argstr = new char[_len+2];
         strcpy(_argstr,_str);
         _Args.push_back(_argstr);
      }


      const char *CArgArray::operator [](unsigned int index) const
      {
         if(index < _Args.size())
         {
			return _Args[index];
         }
         else
         {
			THROW_EXCEPTION_EX("Index Out of bound!");
			return NULL;
         }
      }

      unsigned int CArgArray::Count() const
      {
         return _Args.size();
      }

   } /// namespace GalaxyRT
} /// namespace Galaxy
