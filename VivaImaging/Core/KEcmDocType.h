#pragma once

#include "stdafx.h"
#include "../Core/KMemoryStream.h"
#include <vector>
#include < XmlLite.h>

typedef enum class tagEcmExtDocTypeColumnType
{
  None,
  SingleLine,
  MultiLine,
  Date,
  Number,
  MaxColumnType = Number,
  Binary,
}EcmExtDocTypeColumnType;

#define KEYNAME_LENGTH 21
#define COLUMNNAME_LENGTH 25

typedef struct tagEcmExtDocTypeColumn
{
  EcmExtDocTypeColumnType type;
  TCHAR name[COLUMNNAME_LENGTH];
  TCHAR key[KEYNAME_LENGTH];
  UINT maxValueLength;
  TCHAR* valueString;
}EcmExtDocTypeColumn;

typedef std::vector<EcmExtDocTypeColumn*> KEcmExtDocTypeColumnVector;
typedef std::vector<EcmExtDocTypeColumn*>::iterator KEcmExtDocTypeColumnIterator;
typedef std::vector<EcmExtDocTypeColumn*>::const_iterator KEcmExtDocTypeColumnConstIterator;

/**
* @class KEcmDocTypeInfo
* @brief ECM Ext-DocType item class definition
*/
class KEcmDocTypeInfo
{
public:
  KEcmDocTypeInfo();
  ~KEcmDocTypeInfo();

  TCHAR mTargetFolderOid[MAX_OID];
  TCHAR mDocTypeOid[MAX_OID];
  LPTSTR mpDocTypeName;

  static EcmExtDocTypeColumnType TypeFromString(LPCTSTR typeName);
  static BOOL TypeToString(EcmExtDocTypeColumnType type, LPTSTR str, int buffLength);
  static LPCTSTR GetValueTypeName(EcmExtDocTypeColumnType type);
  static void ClearDocTypeColumn(EcmExtDocTypeColumn* p);

  KEcmDocTypeInfo& operator=(const KEcmDocTypeInfo& ob);

  static EcmExtDocTypeColumn* CreateColumn(EcmExtDocTypeColumnType type, LPCTSTR name, LPCTSTR key, UINT maxLength, LPCTSTR value);

  void Clear();
  void SetDocTypeInfo(LPCTSTR targetFolderOid, LPCTSTR docTypeOid, LPCTSTR docTypeName);

  void Add(EcmExtDocTypeColumnType type, LPCTSTR name, LPCTSTR key, UINT maxLength, LPCTSTR value = NULL);
  void Add(EcmExtDocTypeColumn* p);

  BOOL IsValidKey(LPCTSTR key);
  BOOL SetValue(LPCTSTR key, LPCTSTR value);

  EcmExtDocTypeColumn* Get(LPCTSTR key);
  EcmExtDocTypeColumnType GetColumnType(LPCTSTR key);

#ifdef USE_XML_STORAGE
  LPSTR StoreToString();

  HRESULT ReadAttributes(IXmlReader* pReader);

  HRESULT ReadDocTypeInfo(CComPtr<IXmlReader> pReader, EcmExtDocTypeColumn* col);

  HRESULT ReadDocument(CComPtr<IXmlReader> pReader, int flag);

  HRESULT StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly);
  
  HRESULT StoreDocTypeInfo(CComPtr<IXmlWriter> pWriter, EcmExtDocTypeColumn* p);
#else
  HANDLE ToHandle();
  BOOL FromHandle(HANDLE h);

  DWORD StoreToBuffer(KMemoryStream& mf);
  DWORD StoreDocTypeColumn(KMemoryStream& mf, EcmExtDocTypeColumn* p);

  DWORD LoadFromBuffer(KMemoryStream& mf);
  DWORD LoadDocTypeColumn(KMemoryStream& mf, EcmExtDocTypeColumn* col, int blockSize);
#endif
  KEcmExtDocTypeColumnVector mArray;

  int GetColumnNumber() { return mArray.size(); }

};

