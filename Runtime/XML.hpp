// XML.h: interface for the CXML class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XML_H__4744376A_777A_4B32_880C_F2DA9ADE0415__INCLUDED_)
#define AFX_XML_H__4744376A_777A_4B32_880C_F2DA9ADE0415__INCLUDED_

#ifdef _MSC_VER
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif
#include <vector>

#ifndef O_BINARY
#define O_BINARY (0)
#endif
#include "Object.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

      class CXMLBlader : public CByteArray
      {
      private:
         void Blader();
      public:
         CXMLBlader(const char *_text,unsigned int length);
         virtual ~CXMLBlader();
      };


      class CXMLEscape : public CByteArray
      {
      public:
         CXMLEscape(const char *_text,unsigned int length);
         virtual ~CXMLEscape();
      };

      class CXMLUnescape : public CByteArray
      {
      public:
         CXMLUnescape(const char *_text,unsigned int length);
         virtual ~CXMLUnescape();
      };


      class CXMLBuffer : public CNonCopyAble
      {
      private:
         unsigned char *_Buff;
      protected:
         CXMLBuffer();

         void Allocate(unsigned int);
         void Free();
         unsigned char *_Get() const;
      public:
         CXMLBuffer(unsigned int);
         CXMLBuffer(const char *,unsigned int);
         virtual ~CXMLBuffer();

         unsigned char operator[](unsigned int index);
         unsigned int Size() const;
         const unsigned char  *Get() const;

      };

      class CXMLFile : public CNonCopyAble
      {
      private:
         int		_fd;

         CXMLFile();
      public:
         CXMLFile(const char *_filename,bool _create=false);
         virtual ~CXMLFile();

         unsigned int Size();
         long Seek(long offset, int fromwhere);
         int Read(void *buf ,size_t count);
         int Write(const void *buf ,size_t count);
         int Fill(char val,size_t count);
      };

      class CXMLLoder : public CXMLBuffer
      {
      private:
         void Load(const char *);
      public:
         CXMLLoder(const char *);
         virtual ~CXMLLoder();
      };


      class CXMLParserStream : public CNonCopyAble
      {
      private:
         const CXMLBuffer	&_Buffer;
         char				*_Ptr;

      private:
         char				*Stream();
         void				Stream(char *p);
         void				Stream(unsigned int _Step);
         char				*Entry();
         unsigned int		Size();
         bool INRANGE(const char *_tmpptr);

      public:
         CXMLParserStream(const CXMLBuffer &TheBuffer);
         virtual ~CXMLParserStream();
      public:
         bool IsSpaceChar();
         bool IsSpaceChar(char ch);

         bool IsAllowedKeyChar(char ch);
         bool IsAllowedKeyChar();

         bool IsDigitalChar(char ch);
         bool IsDigitalChar();

         bool IsAllowedKeySymbol(char ch);
         bool IsAllowedKeySymbol();

      public:

         unsigned int Offset();
         unsigned int Limit();
         const char *Current();


         char FirstNoSpaceChar();
         bool FirstNoSpaceCharIs(char KeyChar);
         bool MatchKeyword(const char *_Keyword);

         char GetChar(int Index=0);
         char Next();
         char Step(unsigned int _Step);
      };



      class CXMLOffset : public CObject
      {
      private:
         unsigned int _Offset;
         unsigned int _Length;
      public:
         CXMLOffset();
         CXMLOffset(unsigned int TheOffset,unsigned int TheLength);
         CXMLOffset(const CXMLOffset &_ofs);
         virtual ~CXMLOffset();

         unsigned int Offset() const;
         unsigned int Length() const;

         void Distance(const CXMLOffset &_a,const CXMLOffset &_b);
      };


      class CXMLCustomValue : public CObject
      {
      private:
         std::string		_Key;
         std::string		_Val;
      public:

         std::string Key() const;
         std::string Value() const;
         void Key(const std::string &_TheKey) ;
         void Value(const std::string &_TheVal);

         void EscapeValue(const std::string &_TheVal);
         std::string EscapeValue() const;
      public:
         CXMLCustomValue();
         CXMLCustomValue(const CXMLCustomValue &_TheVal);
         CXMLCustomValue(const std::string &_TheKey,const std::string &_TheVal,bool _IsEscape=false);
         CXMLCustomValue(const CXMLBuffer &_BUF,const CXMLOffset &_TheKey,const CXMLOffset &_TheVal,bool _IsEscape=false);
         virtual ~CXMLCustomValue();

      };


      class CXMLValue : public CXMLCustomValue
      {
      public:
         CXMLValue();
         CXMLValue(const CXMLCustomValue &_TheVal);
         CXMLValue(const std::string &_TheKey,const std::string &_TheVal,bool _IsEscape=false);
         CXMLValue(const CXMLBuffer &_BUF,const CXMLOffset &_TheKey,const CXMLOffset &_TheVal,bool _IsEscape=false);
         virtual ~CXMLValue();
      };

      class CXMLCustomValues : public CNonCopyAble
      {
      private:
         std::vector<const CXMLCustomValue *> _Values;
      public:
         CXMLCustomValues();
         virtual ~CXMLCustomValues();

         void Insert(const CXMLCustomValue *);
         void Append(const CXMLCustomValue *);

         unsigned int Count() const;

         bool Exists(std::string _key) const;

         const CXMLCustomValue *operator [](std::string) const;
         const CXMLCustomValue *operator [](unsigned int index) const ;

      };


      class CXMLValues : public CXMLCustomValues
      {
      public:
         CXMLValues();
         virtual ~CXMLValues();

         const CXMLValue *operator [](std::string) const;
         const CXMLValue *operator [](unsigned int index) const ;
      };


      class CXMLElement;
      class CXMLElements : public CXMLCustomValues
      {
      public:
         CXMLElements();
         virtual ~CXMLElements();

         const CXMLElement *operator [](std::string) const;
         const CXMLElement *operator [](unsigned int index) const ;
      };


      class CXMLElement : public CXMLCustomValue
      {
      private:
         CXMLElements	_Elements;
         CXMLValues		_Properties;

      private:  // emphasize the following members are private
         const CXMLElement& operator=( const CXMLElement& );
         CXMLElement(const CXMLElement &);
      public:
         CXMLElement();
         CXMLElement(const std::string &_Thekey);
         CXMLElement(const std::string &_Thekey,const std::string &_Theval,bool _IsEscape=false);
         CXMLElement(const CXMLBuffer &_BUF,const CXMLOffset &_TheKey);
         virtual ~CXMLElement();

         void AddSubElement(const CXMLElement *_Elm);
         void AddProperty(const CXMLValue *_Property);

         const CXMLValues &Properties() const;
         const CXMLElements &SubElements() const;
         CXMLValues &Properties();
         CXMLElements &SubElements();

      };


      class CXMLDeclaration : public CXMLCustomValue
      {
      private:
         CXMLValues		_Items;

         std::string Key() const;
         void Key(const std::string &_TheKey) ;

      public:
         CXMLDeclaration();
         CXMLDeclaration(const std::string &);
         CXMLDeclaration(const CXMLBuffer &,const CXMLOffset &);
         virtual ~CXMLDeclaration();
         const CXMLValues *Items() const;
         void Append(const CXMLValue *_TheVal);

         using CXMLCustomValue::Value;
      };


      class CXMLDocument : public CNonCopyAble
      {
      private:
         CXMLElement			*_Root;
         CXMLDeclaration		*_Decl;
      public:
         CXMLDocument(CXMLDeclaration *_TheDeclaration,CXMLElement *_TheRoot);
         virtual ~CXMLDocument();

         const CXMLDeclaration *Declarations() const;
         const CXMLElement *Root() const;
      };



      class CXMLParser : public CNonCopyAble
      {
      private:
         const CXMLBuffer	&_Buffer;
      private:
         const CXMLDocument *ParseDocument(CXMLParserStream &_Stream);

         bool ParseIgnoreComment(CXMLParserStream &_Stream);
      private:
         bool ParseDeclaration(CXMLParserStream &_Stream,CXMLDeclaration	*_Declaration);
         bool ParseDeclarationGetBegin(CXMLParserStream &_Stream,CXMLOffset &_Begin);
         bool ParseDeclarationGetEnd(CXMLParserStream &_Stream,CXMLOffset &_End);
         bool ParseDeclarationEnumAttrib(CXMLParserStream &_Stream,CXMLDeclaration *_Declaration);
         bool ParseDeclarationGetAttribKeyVal(CXMLParserStream &_Stream,CXMLOffset &_Key,CXMLOffset &_Val,const char *_Keyword);
      private:
         bool ParseRootElement(CXMLParserStream &_Stream,CXMLElement *_Root);
         bool ParseEnumElement(CXMLParserStream &_Stream,CXMLElement *_Element);
         bool ParseEnumAttrib(CXMLParserStream &_Stream,CXMLElement *_Parent);
      public:
         CXMLParser(const CXMLBuffer &Buffer);
         virtual ~CXMLParser();

         const CXMLDocument *Parse();
      };

      class CXMLReader : public CNonCopyAble
      {
      private:
         const CXMLDocument *_Document;
      public:
         CXMLReader(const char *_PathofXML);
         virtual ~CXMLReader();
         const CXMLDocument *Document() const;
      };

      class CXMLWriter : public CNonCopyAble
      {
      private:
         void WriteDocument(CXMLFile &_File,const CXMLDocument *_Document);
         void WriteElement(CXMLFile &_File,const CXMLElement *_Element,unsigned int iTabs);
         void WriteDeclaration(CXMLFile &_File,const CXMLDeclaration *_Declaration);
      public:
         CXMLWriter(const CXMLDocument *_Document,const char *_PathofXML);
         virtual ~CXMLWriter();

      };

   } /// namespace GalaxyRT
} /// namespace Galaxy
	
#endif // !defined(AFX_XML_H__4744376A_777A_4B32_880C_F2DA9ADE0415__INCLUDED_)
