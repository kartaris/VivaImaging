#include "stdafx.h"
#include "KMetadataInfo.h"

KMetadataInfo::KMetadataInfo()
{
  ExtDocTypeOID[0] = '\0';
}

KMetadataInfo::~KMetadataInfo()
{
  Clear();
}

void KMetadataInfo::Clear()
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    MetaKeyNameValue* d = *(it++);
    ClearMetaKeyNameValue(d);
  }
  mVector.clear();
}

void KMetadataInfo::Add(MetaKeyNameValue* d)
{
  mVector.push_back(d);
}

BOOL KMetadataInfo::Set(LPCTSTR key, LPCTSTR str, BOOL addNewIfNotExist)
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    MetaKeyNameValue* p = *(it++);
    if (CompareStringNC(p->key, key) == 0)
    {
      if (p->value != NULL)
        delete[] p->value;
      p->value = AllocString(str);
      return TRUE;
    }
  }

  if (addNewIfNotExist)
  {
    MetaKeyNameValue* n = new MetaKeyNameValue;
    n->key = AllocString(key);
    n->name = NULL;
    n->value = AllocString(str);
    Add(n);
    return TRUE;
  }
  return FALSE;
}

MetaKeyNameValue* KMetadataInfo::Find(LPCTSTR key)
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue)*it;

    if (CompareStringNC(p->key, key) == 0)
    {
      return p;
    }
    ++it;
  }
  return NULL;
}

BOOL KMetadataInfo::Remove(LPCTSTR key)
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue)*it;

    if (CompareStringNC(p->key, key) == 0)
    {
      ClearMetaKeyNameValue(p);
      it = mMetaDatas.erase(it);
      return TRUE;
    }
    ++it;
  }
  return FALSE;
}

void KMetadataInfo::ClearMetaKeyNameValue(MetaKeyNameValue* p)
{
  if (p->key != NULL)
    delete[] p->key;
  if (p->name != NULL)
    delete[] p->name;
  if (p->value != NULL)
    delete[] p->value;
  delete p;
}

