#include "stdafx.h"
#include "KBgTaskDownload.h"

#include "../Platform/Platform.h"
#include "../Core/KSettings.h"
#include "../Core/KSyncCoreEngine.h"

#include "HttpDownloader\KString.h"
#include "HttpDownloader\KWnetDownloader.h"

#include <strsafe.h>

KBgTaskDownload::KBgTaskDownload(CWnd* owner, BgTaskType task, LPCTSTR arg)
  :KBgTaskLoader(owner, task, arg)
{
  mEcmRoot = NULL;
  mpDocTypeInfo = NULL;
}

KBgTaskDownload::~KBgTaskDownload(void)
{
  if (mEcmRoot != NULL)
  {
    delete mEcmRoot;
    mEcmRoot = NULL;
  }
  if (mpDocTypeInfo != NULL)
  {
    delete mpDocTypeInfo;
    mpDocTypeInfo = NULL;
  }
}

void KBgTaskDownload::Do()
{
  if (mTask == BGTASK_FOLDER_DOWNLOAD)
  {
    CString download_err;

    int result = DownloadFolderList(download_err);

    // parse received pakets
    if (IsContinue() && (result == R_SUCCESS) && (mDownloadLength > 0))
      parseFolderNDocument();
  }
  else if (mTask == BGTASK_FILE_DOWNLOAD)
  {
    EcmFileItem* item = new EcmFileItem(NULL, NULL, mArg, NULL, NULL);
    mEcmRoot = item;
  }

  if (!IsContinue())
    return;

  if (mEcmRoot != NULL)
  {
    mTotalFolder = mEcmRoot->GetFolderCount();
    mTotalFile = mEcmRoot->GetFileCount();

    // set item flag as ITEM_NEED_DOWNLOAD for documents
    mEcmRoot->SetItemLoadFlag(0, ITEM_NEED_DOWNLOAD);
  }

  // post download list
  if ((mOwner != NULL) && (mEcmRoot != NULL))
  {
    HANDLE p = mEcmRoot->CreateListData();
    if (p != NULL)
      mOwner->PostMessage(MSG_BGTASK_MESSAGE, BGTASK_DOWNLIST_READY, (LPARAM)p);
  }

  // download files
  downloadFiles();
}

int KBgTaskDownload::downloadFiles()
{
  EcmFileItem* item;

  mDoneFile = 0;
  while (IsContinue() &&
    ((item = (EcmFileItem*)((EcmFolderItem*)mEcmRoot)->GetNextItem(ITEM_NEED_DOWNLOAD)) != NULL))
  {
#ifdef _DEBUG_SLOW
    Sleep(1000);
#endif

    LPTSTR localFileName = AllocPathName(mWorkingPath, item->mName);
    if ((localFileName != NULL) &&
      (downloadFileItem(item, localFileName) == R_SUCCESS))
    {
      mDoneFile++;

      // ExtDocType
      if ((mpDocTypeInfo == NULL) && (item->pMetadata != NULL) &&
        (lstrlen(item->pMetadata->mDocTypeOid) > 0))
      {
        mpDocTypeInfo = new KEcmDocTypeInfo();

        if (!gpCoreEngine->SyncServerGetDocTypeInfo(item->pMetadata->mDocTypeOid, mpDocTypeInfo))
        {
          delete mpDocTypeInfo;
          mpDocTypeInfo = NULL;
        }
      }
 
      // post downloaded list
      if (mOwner != NULL)
      {
#if 1
        //KVoidPtrArray fileList;
        //fileList.push_back(item);
        //HANDLE h = createListData(fileList);

        HANDLE h = item->CreateListData();
#else
        LPTSTR pathName = item->MakePathName();

        int length = lstrlen(mWorkingPath) + 1;
        length += lstrlen(pathName) + 4;

        HANDLE h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, length * sizeof(TCHAR));
        LPTSTR p = (LPTSTR)GlobalLock(h);
        StringCchCopy(p, length, mWorkingPath);
        p += lstrlen(mWorkingPath) + 1;
        length -= lstrlen(mWorkingPath) + 1;

        StringCchCopy(p, length, pathName);
        p += lstrlen(pathName) + 1;
        length -= lstrlen(pathName) + 1;

        // add extra NULL
        if (length > 0)
          *p = '\0';
        delete[] pathName;
        GlobalUnlock(h);
#endif
        mOwner->PostMessage(MSG_BGTASK_MESSAGE, BGTASK_DOWNLIST_DONE, (LPARAM)h);
      }
    }
    else
    {
      mFailCount++;
    }

    if (localFileName != NULL)
      delete[] localFileName;
  }

#ifdef _DEBUG_FAIL_COUNT
  mFailCount++;
#endif

  return mDoneFile;
}

int KBgTaskDownload::downloadFileItem(EcmFileItem* item, LPCTSTR localFileName)
{
  LPTSTR pErrorMessage = NULL;

  int r = gpCoreEngine->SyncServerDownloadFile(item->fileOID, item->storageOID, localFileName, &pErrorMessage);
  item->SetNeedDownload(FALSE);

#ifdef _DEBUG_ERR
  if (mDoneFile == 1)
  {
    r = 100;
    pErrorMessage = AllocString(_T("Fake error"));
  }
#endif

  if (r != R_SUCCESS)
  {
    TCHAR msg[256] = _T("Download failed, name=");
    StringCchCat(msg, 256, localFileName);
    StringCchCat(msg, 256, _T(", fileOID="));
    StringCchCat(msg, 256, item->fileOID);

    if (pErrorMessage != NULL)
    {
      StringCchCat(msg, 256, _T(", error="));
      StringCchCat(msg, 256, pErrorMessage);
    }
    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
  }
  else
  {
    item->pMetadata = new KMetadataInfo();
    if (gpCoreEngine->SyncServerGetDocumentExt(item->docOID, item->pMetadata, &pErrorMessage))
    {
      TCHAR msg[256] = _T("SyncServerGetDocumentExt failed, name=");
      StringCchCat(msg, 256, item->docOID);
      if (pErrorMessage != NULL)
      {
        StringCchCat(msg, 256, _T(", error="));
        StringCchCat(msg, 256, pErrorMessage);
        SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
      }
    }
  }

#ifdef _DEBUG_ERR
  if (mDoneFile == 1)
  {
    r = R_SUCCESS;
  }
#endif

  if (pErrorMessage != NULL)
    delete[] pErrorMessage;
  return r;
}
