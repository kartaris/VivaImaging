#include "stdafx.h"
#include "KImageCacheManager.h"
#include "../Core/KSettings.h"

KImageCacheManager::KImageCacheManager(unsigned __int64 maxCacheSize)
  : mMaxCacheSize(maxCacheSize)
{
  MEMORYSTATUSEX ms;
  ms.dwLength = sizeof(MEMORYSTATUSEX);
  if (GlobalMemoryStatusEx(&ms))
  {
    mMaxCacheSize = (LONGLONG)(ms.ullAvailPhys / 5);

#ifdef _DEBUG
    if (mMaxCacheSize > 100 * 1000000)
      mMaxCacheSize = 100 * 1000000;
#endif
  }
}

KImageCacheManager::~KImageCacheManager()
{
  Clear();
}

void KImageCacheManager::Clear()
{
  ImageCacheIter it = mCacheVector.begin();
  while (it != mCacheVector.end())
  {
    ImageCacheDataInfo* s = *it;
    delete s;
    it++;
  }
  mCacheVector.clear();
}

void KImageCacheManager::OnAccessImage(KImageFolder* f, KImagePage* p, LPCTSTR workingPath)
{
  SYSTEMTIME time;
  GetLocalTime(&time);

  // remove not-existing page
  ImageCacheIter it = mCacheVector.begin();
  while (it != mCacheVector.end())
  {
    ImageCacheDataInfo* s = *it;
    if (f->GetIndexOfPage(s->pOwner, 0) < 0) // not existing
    {
      TCHAR msg[] = _T(" : NotExiting cache page deleted");
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      delete s;
      it = mCacheVector.erase(it);
    }
    else
    {
      it++;
    }
  }

  // find existing item
  it = mCacheVector.begin();
  while (it != mCacheVector.end())
  {
    ImageCacheDataInfo* s = *it;
    if (s->pOwner == p)
    {
      memcpy(&s->lastAccess, &time, sizeof(SYSTEMTIME));
      // CheckTotalCacheSize(f, workingPath);
      return;
    }
    it++;
  }

  // add new items
  ImageCacheDataInfo* s = new ImageCacheDataInfo;
  s->pOwner = p;
  memcpy(&s->lastAccess, &time, sizeof(SYSTEMTIME));
  s->imageCacheSize = p->GetImageCacheSize();
  mCacheVector.push_back(s);

  // check total cache size
  CheckTotalCacheSize(f, workingPath);
}

static void StoreWithTimeSort(ImageCacheVector& d, ImageCacheDataInfo* s)
{
  if (d.size() > 0)
  {
    ImageCacheIter it = d.begin();
    while (it != d.end())
    {
      ImageCacheDataInfo* ds = *it;

      if (CompareSystemTime(s->lastAccess, ds->lastAccess) > 0)
      {
        d.insert(it, s);
        return;
      }
      it++;
    }
  }
  d.push_back(s);
}

void KImageCacheManager::CheckTotalCacheSize(KImageFolder* f, LPCTSTR workingPath)
{
  unsigned __int64 totalSize = 0;
  ImageCacheIter it = mCacheVector.begin();
  while (it != mCacheVector.end())
  {
    ImageCacheDataInfo* s = *it;
    totalSize += s->imageCacheSize;
    it++;
  }

#ifdef _DEBUG_IMAGE_CACHE
  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("TotalCacheSize : %d(Mb)"), (int)(totalSize / (1024 * 1024)));
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

  /*
  */

  // sort it as LRU
  ImageCacheVector tempVector;
  it = mCacheVector.begin();
  while (it != mCacheVector.end())
  {
    ImageCacheDataInfo* s = *it++;
    StoreWithTimeSort(tempVector, s);
  }
  mCacheVector.clear();

#ifdef _DEBUG
  TCHAR msg[256];
  TCHAR maxSizeStr[16];
  FormatFileSize(maxSizeStr, 16, mMaxCacheSize);
  int sizef = (int)(totalSize * 100 / mMaxCacheSize);
  StringCchPrintf(msg, 256, _T("Used=%d%%, total=%s"), sizef, maxSizeStr);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

  it = tempVector.begin();
  totalSize = 0;
  while (it != tempVector.end())
  {
    ImageCacheDataInfo* s = *it;

    mCacheVector.push_back(s);
    it = tempVector.erase(it);

    totalSize += s->imageCacheSize;
    if (totalSize > mMaxCacheSize)
      break;
  }

  // clear remained cache items
  it = tempVector.begin();
  totalSize = 0;
  while (it != tempVector.end())
  {
    ImageCacheDataInfo* s = *it++;

#ifdef _DEBUG //_IMAGE_CACHE
    if ((s->pOwner != NULL) && (s->pOwner->mName != NULL))
      StringCchCopy(msg, 256, s->pOwner->mName);
    StringCchCat(msg, 256, _T(" : Need to retired from Cache"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

    s->pOwner->ClearCacheImage(workingPath);
    delete s;
  }
  tempVector.clear();
}
