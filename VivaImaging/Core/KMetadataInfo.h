#pragma once

#include "stdafx.h"
#include "KConstant.h"
#include "KEcmDocType.h"
#include "../Platform/Utils.h"

typedef enum class tagMetaDataCategory
{
  None,
  Exif,
  Gps,
  Ifd,
  ExtDocType,
  MaxCategory,
}MetaDataCategory;

typedef struct tagMetaKeyNameValue
{
  LPTSTR key;
  LPTSTR name; // display name
  LPTSTR value;
  int len;
  WORD vtype;
  MetaDataCategory category;
  EcmExtDocTypeColumnType type;
} MetaKeyNameValue, * PMetaKeyNameValue;


typedef std::vector<MetaKeyNameValue*> KMetadataVector;
typedef std::vector<MetaKeyNameValue*>::iterator KMetadataVectorIterator;

class KMetadataInfo
{
public:
  KMetadataInfo();
  ~KMetadataInfo();

  TCHAR mDocTypeOid[MAX_OID]; // ExtDocTypeOID[MAX_OID];
  KMetadataVector mVector;

  static void InitMetaKeyNameValue(MetaKeyNameValue* p);
  static void ClearMetaKeyNameValue(MetaKeyNameValue* p);
  static PMetaKeyNameValue CopyMetaKeyNameValue(MetaKeyNameValue* p);

  // static LPCTSTR GetValueTypeName();
  static BOOL IsDocTypeMetadata(MetaKeyNameValue* p);
  static void dumpKeyNameValue(MetaKeyNameValue* p);

  void Clear(BOOL clearEditOnly);

  void SetDocTypeInfo(LPCTSTR docTypeOid, LPCTSTR docTypeName);

  void Add(MetaKeyNameValue* d);
  BOOL Set(LPCTSTR key, LPCTSTR value, LPCTSTR name = NULL, 
    EcmExtDocTypeColumnType type = EcmExtDocTypeColumnType::None,
    MetaDataCategory category = MetaDataCategory::None, BOOL addNewIfNotExist = FALSE);

  BOOL Set(LPCTSTR key, LPBYTE buff, int len, WORD valueType, LPCTSTR name = NULL,
    EcmExtDocTypeColumnType type = EcmExtDocTypeColumnType::None,
    MetaDataCategory category = MetaDataCategory::None, BOOL addNewIfNotExist = FALSE);

  MetaKeyNameValue* Find(LPCTSTR key);
  BOOL Remove(LPCTSTR key);

#ifdef USE_XML_STORAGE
  LPSTR StoreToString();

  HRESULT StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly);
  HRESULT StoreMetaKeyNameValue(CComPtr<IXmlWriter> pWriter, MetaKeyNameValue* p);
#endif

  DWORD loadMetaKeyNameValueFromBuffer(KMemoryStream& mf, int blockSize, MetaKeyNameValue* p);
  DWORD LoadFromBuffer(KMemoryStream& mf);

  DWORD storeMetaKeyNameValue(KMemoryStream& mf, MetaKeyNameValue* p);
  DWORD StoreToBuffer(KMemoryStream& mf);

private:
  void addItem(MetaKeyNameValue* d);

};

extern void ClearKeyNameValueArray(KVoidPtrArray& list);
