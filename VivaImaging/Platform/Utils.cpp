#include "stdafx.h"
#include "Utils.h"

void ClearVoidPtrArray(KVoidPtrArray& ar)
{
  KVoidPtrIterator it = ar.begin();
  while (it != ar.end())
  {
    void *page = *(it++);
    delete page;
  }
  ar.clear();
}

void ClearTStrArray(KVoidPtrArray& ar)
{
  KVoidPtrIterator it = ar.begin();
  while (it != ar.end())
  {
    LPTSTR page = (LPTSTR)*(it++);
    delete[] page;
  }
  ar.clear();
}

void AddVoidPtrArray(KVoidPtrArray& dst, KVoidPtrArray& src)
{
  KVoidPtrIterator it = src.begin();
  while (it != src.end())
  {
    void *page = *it;
    dst.push_back(page);
    it++;
  }
  src.clear();
}

BOOL DoesListContainValue(KIntVector& intVector, int value)
{
  KintIterator it = intVector.begin();
  while (it != intVector.end())
  {
    if (*it == value)
      return TRUE;
    it++;
  }
  return FALSE;
}

