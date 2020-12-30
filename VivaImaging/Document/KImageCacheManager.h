#pragma once

#include "KImageFolder.h"
#include "KImagePage.h"
#include "KImageDrawer.h"
#include <vector>

typedef struct tagImageCacheDataInfo
{
  KImagePage* pOwner;
  SYSTEMTIME lastAccess;
  unsigned __int64 imageCacheSize;
}ImageCacheDataInfo;

typedef std::vector<ImageCacheDataInfo*> ImageCacheVector;
typedef std::vector<ImageCacheDataInfo*>::iterator ImageCacheIter;

#define DEFAULT_LOCAL_CACHE_SIZE 1024 * 1024 * 500

class KImageCacheManager
{
public:
  KImageCacheManager(unsigned __int64 maxCacheSize = DEFAULT_LOCAL_CACHE_SIZE);
  virtual ~KImageCacheManager();

  void Clear();
  void OnAccessImage(KImageFolder* f, KImagePage* p, LPCTSTR workingPath);

  void CheckTotalCacheSize(KImageFolder* f, LPCTSTR workingPath);

  ImageCacheVector mCacheVector;
  unsigned __int64 mMaxCacheSize;
};
