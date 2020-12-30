#include "stdafx.h"

#include "KConstant.h"
#include "KEcmBaseItem.h"
#include "../Platform/Platform.h"

#include <strsafe.h>
#include <Shlwapi.h>

TCHAR EcmBaseItem::efi_pathDelimeter[] = _T("\\");
TCHAR EcmBaseItem::efi_itemDelimeter[] = _T("|");
TCHAR EcmBaseItem::efi_folderItemKey[] = _T("F");
TCHAR EcmBaseItem::efi_fileItemKey[] = _T("D");

EcmBaseItem::EcmBaseItem(EcmBaseItem* parent, LPCTSTR name)
{
  mParent = parent;
  mName = AllocString(name);
  mLoadFlag = 0;
  mPermission = 0;
}

EcmBaseItem::~EcmBaseItem()
{
  if (mName != NULL)
  {
    delete[] mName;
    mName = NULL;
  }
};


void EcmBaseItem::SetNeedUpload(BOOL upload)
{
  if (upload)
    mLoadFlag |= ITEM_NEED_UPLOAD;
  else
    mLoadFlag &= ~ITEM_NEED_UPLOAD;
}

void EcmBaseItem::SetNeedDownload(BOOL download)
{
  if (download)
    mLoadFlag |= ITEM_NEED_DOWNLOAD;
  else
    mLoadFlag &= ~ITEM_NEED_DOWNLOAD;
}

BOOL EcmBaseItem::IsNeed(int needed_flag)
{
  return !!(mLoadFlag & needed_flag);
}

BOOL EcmBaseItem::IsNeedUpload()
{
  return !!(mLoadFlag & ITEM_NEED_UPLOAD);
}

BOOL EcmBaseItem::IsNeedDownload()
{
  return !!(mLoadFlag & ITEM_NEED_DOWNLOAD);
}

HANDLE EcmBaseItem::CreateListData()
{
  HANDLE h = NULL;
  KMemoryStream mf;

  StoreToBuffer(mf);

  if (mf.Size() > 0)
  {
    h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, mf.Size());
    LPTSTR p = (LPTSTR)GlobalLock(h);
    memcpy(p, mf.Data(), mf.Size());
    GlobalUnlock(h);
  }
  return h;
}


