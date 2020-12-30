#pragma once

#include "stdafx.h"
#include "../Core/KConstant.h"
#include "../Core/KEcmDocType.h"
// #include "KImageBase.h"

typedef struct tagMetaKeyNameValue
{
  LPTSTR key;
  LPTSTR name; // display name
  LPTSTR value;
} MetaKeyNameValue, * PMetaKeyNameValue;


typedef std::vector<MetaKeyNameValue*> KMetadataVector;
typedef std::vector<MetaKeyNameValue*>::iterator KMetadataVectorIterator;

class KMetadataInfo
{
  KMetadataInfo();
  ~KMetadataInfo();

  TCHAR ExtDocTypeOID[MAX_OID];
  KMetadataVector mVector;

  static void ClearMetaKeyNameValue(MetaKeyNameValue* p);

  void Clear();
  void Add(MetaKeyNameValue* d);
  BOOL Set(LPCTSTR key, LPCTSTR str, BOOL addNewIfNotExist);
  MetaKeyNameValue* Find(LPCTSTR key);
  BOOL Remove(LPCTSTR key);


};


// extern void ClearKeyNameValueArray(KVoidPtrArray& list);
