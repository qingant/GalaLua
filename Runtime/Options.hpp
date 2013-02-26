// Options.h: interface for the COptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONS_H__0F1E3677_D94F_4DD2_A57E_A71F6F9B1B97__INCLUDED_)
#define AFX_OPTIONS_H__0F1E3677_D94F_4DD2_A57E_A71F6F9B1B97__INCLUDED_
// Demo Code


// unsigned int i;
// 	COptions	_opt(argc,argv,"b:B: a:A:?h");

// 	if(_opt.Options().Count() > 0)
// 	{
// 		for(i=0;i<_opt.Options().Count();i++)
// 		{

// 			char Key = (char) _opt.Options()[i]->Key();
// 			const COption *Option = _opt.Options()[Key];
// 			if(Option->HasVal())
// 			{
// 				Print.Print("-%c = %s\n",Option->Key(),Option->Val());
// 			}
// 			else
// 			{
// 				Print.Print("-%c is Indication\n",Option->Key());
// 			}

// 		}

// 	}

// 	if(_opt.Args().Count() > 0)
// 	{
// 		for(i=0;i<_opt.Args().Count();i++)
// 		{
// 			Print.Print("Args%d = %s\n",i,_opt.Args()[i].c_str());
// 		}

// 	}




#include "Object.hpp"
#include <vector>
#include <string>

namespace Galaxy
{
   namespace GalaxyRT
   {

      class COption : public CNonCopyAble
      {
      private:
         unsigned int _key;
         char *_val;
      public:
         COption(unsigned key, char* val);
         virtual ~COption();
         unsigned int Key() const;
         bool HasVal() const;
         const char *Val() const;
      };

      class CArgArray :  public CNonCopyAble
      {
      private:
         std::vector<char*> _Args;
      public:
         CArgArray();
         virtual ~CArgArray();
         void Pushback(const char *);
         const char *operator [](unsigned int index) const;
         unsigned int Count() const;
      };

      class COptions : public CNonCopyAble
      {
      private:
         std::vector<COption*> _Options;
         void Analyse(int argc, char* argv[],const char *_opt);
      public:
         std::vector<std::string> _MainArgs;
      public:
         COptions(int argc, char* argv[],const char *_opt);
         virtual ~COptions();
         const COption *operator [](char _key);
         const COption *operator [](unsigned int index);
         unsigned int Count();

      };

   } /// namespace GalaxyRT
} /// namespace Galaxy
   
#endif // !defined(AFX_OPTIONS_H__0F1E3677_D94F_4DD2_A57E_A71F6F9B1B97__INCLUDED_)
