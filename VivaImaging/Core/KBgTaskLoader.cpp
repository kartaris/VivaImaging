#include "stdafx.h"
#include "KBgTaskLoader.h"

#include "KConstant.h"
#include "KSettings.h"

#include "ThreadPool.h"
#include <strsafe.h>
#include <Shlwapi.h>

#include "../Platform/Platform.h"
#include "../Core/KSettings.h"
#include "../Document/KImageBase.h"
#include "../Core/KSyncCoreEngine.h"

#include "HttpDownloader\KString.h"
#include "HttpDownloader\KWnetDownloader.h"

KBgTaskLoader::KBgTaskLoader(CWnd* owner, BgTaskType task, LPCTSTR arg)
  : KBgTask(owner, task, arg)
{
  mWorkingPath = NULL;
  mDoneFile = 0;
  mTotalFile = 0;
  mDoneFolder;
  mTotalFolder;

  mResult = 0;
  mDownloadResult = 0;

  mDownloadData = NULL;
  mDownloadLength = 0;
  mEcmRoot = NULL;
  mFailCount = 0;
}

KBgTaskLoader::~KBgTaskLoader(void)
{
  if (mWorkingPath != NULL)
  {
    delete[] mWorkingPath;
    mWorkingPath = NULL;
  }

  if (mDownloadData != NULL)
  {
    // this is GlobalAlloc data
    GlobalFree(mDownloadData);
    mDownloadData = NULL;
  }

  if (mEcmRoot != NULL)
  {
    delete mEcmRoot;
    mEcmRoot = NULL;
  }
  //ClearDownFileInfoArray(mFolderList);
  //ClearDownFileInfoArray(mFileList);
}

void KBgTaskLoader::SetWorkingFolder(LPCTSTR pathName)
{
  mWorkingPath = AllocString(pathName);
}

int KBgTaskLoader::queryRequestFolderNDocument(KDownloader* downloader, LPCTSTR folderOID)
{
  int length = 400;
  char requestContext[400] = { 0 };

  LPSTR folder_oid = MakeUtf8String(folderOID);
  sprintf_s(requestContext, length, "localsync/getModifiedFolderNDocument?folderOID=%s", folder_oid); // with session key
  if (folder_oid != NULL)
    delete[] folder_oid;

  KString context(requestContext);

  LPCTSTR sessionkey_ts = ((KSyncCoreEngine*)gpCoreEngine)->GetUserSessionKey();
  LPCTSTR serverurl_ts = ((KSyncCoreEngine*)gpCoreEngine)->GetRpcServerUrl();

  if (serverurl_ts == NULL)
  {
    SendLogMessage(_T(__FUNCTION__), _T("No RPC address"), SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    return R_FAIL_INTERNAL;
  }

  LPSTR baseUrl = MakeUtf8String(serverurl_ts);
  downloader->SetBaseUrl(baseUrl);
  delete[] baseUrl;

  int rtn = downloader->DownloadRequest(context);

  if (lstrlen(sessionkey_ts) > 0) // session-key value
  {
    char encoded_session_key[128];
    char *session_key = MakeUtf8String(sessionkey_ts);
    downloader->EncodeBase64(encoded_session_key, 128, (BYTE *)session_key, strlen(session_key));
    delete[] session_key;

    downloader->SetSessionKey(encoded_session_key);
  }
  return R_SUCCESS;
}

int KBgTaskLoader::DownloadFolderList( CString& download_err)
{
  int result = R_SUCCESS;
  int retryCount = 3;
  KDownloaderError err = SFS_NO_ERROR;

  while ((retryCount > 0) && IsContinue())
  {
    KDownloader* downloader = new KWnetDownloader();

    queryRequestFolderNDocument(downloader, mArg);

    KStringList out_completedItems;
    int count = 0;

    while (IsContinue() && downloader->ProcessDownload(out_completedItems))
    {
      count++;
    }

    int result = downloader->GetResult();
    if (!IsContinue())
    {
      //mErrorMessage = AllocString(_T("Stop forcely"));
      result = 0;
    }
    else if (result == 200)
    {
      if (downloader->m_ReceiveCount > 0)
      {
        // this is GlobalAlloc data
        mDownloadData = downloader->GetDownloadData(mDownloadLength);
      }
    }
    else
    {
      err = downloader->GetErrorCode();
      LPCSTR msg = downloader->GetErrorMessage();
      if (msg != NULL)
      {
        LPTSTR errmsg = MakeUnicodeString(msg);
        download_err += errmsg;
      }
    }

  #ifdef _DEBUG_BUSY_RETRY
    if ((mJob == REQUEST_UNREADED_MESSAGE) && (mRetry < 5))
      result = 503;
  #endif

    if (result == 200) // (mScanData != NULL)
    {
      result = R_SUCCESS;
    }
    else if (result == 503) // need retry
    {
      TCHAR msg[] = _T("Retry getModifiedFolderNDocument");
      SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      result = R_WARN_SERVER_BUSY;
      downloader->Clear();
    }
    else
    {
      result = FILE_SYNC_SFS_ERROR;
      //mDownloadResult = err;
    }

    delete downloader;
    if (result != R_WARN_SERVER_BUSY)
      break;

    retryCount--;
  }

  if (result == FILE_SYNC_SFS_ERROR)
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Server Request error code=%d, on "), err);
    LPCTSTR serverurl_ts = ((KSyncCoreEngine*)gpCoreEngine)->GetRpcServerUrl();
    StringCchCat(msg, 256, serverurl_ts);
    StringCchCat(msg, 256, _T("localsync/getModifiedFolderNDocument?folderOID="));
    StringCchCat(msg, 256, mArg);
    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);

    if (download_err.GetLength() > 0)
    {
      SendLogMessage(_T(__FUNCTION__), download_err, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    }
  }
  return result;
}

int KBgTaskLoader::parseFolderNDocument()
{
  LPSTR data = (LPSTR)mDownloadData;
  int length = mDownloadLength;

  LPSTR e = strchr(data, SCAN_ITEM_DELEMETER);
  if ((e == NULL) || ((data + 100) < e))
  {
    SendLogMessage(_T(__FUNCTION__), _T("cannot found server time break"), SYNC_MSG_LOG);
    if (data != NULL)
    {
      LPTSTR pStr = MakeUnicodeString(data);
      if (pStr != NULL)
      {
        SendLogMessage(_T(__FUNCTION__), pStr, SYNC_MSG_LOG);
        delete[] pStr;
      }
    }
    return R_FAIL_SCAN_SERVER_FOLDERS;
  }

  KString str;
  str.assign(data, (int)(e - data));
  __int64 scan_time = str.toInt64();
  ++e;

#ifdef _DEBUG_SCAN_SERVER_FOLDERS
  SYSTEMTIME st;
  SystemTimeFromEPOCH_MS(st, scan_time);

  TCHAR buff[100] = _T("Received ServerFileScan utc-time = ");
  if ((scan_time > 0) && IsValidSystemTime(st))
  {
    TCHAR timestr[64];
    SystemUTCTimeToTString(timestr, 64, st);
    StringCchCat(buff, 100, timestr);
  }
  else
  {
    StringCchCat(buff, 100, _T("Invalid time"));
  }
  StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif

  if (scan_time == 0)
    return R_FAIL_SCAN_SERVER_FOLDERS;
  length -= (e - data);
  data = e;

  // parse body data
  int block_len;
  //EcmFolderItem *lastParent = NULL;
  int r = R_SUCCESS;

  while (length > 0)
  {
    e = strchr(data, SCAN_ITEM_DELEMETER);
    if (e != NULL)
    {
      block_len = (int)(e - data);
      *e = '\0';
    }
    else
    {
      block_len = length;
    }

    // create item from data
    EcmBaseItem *item = CreateFromStream(data, block_len, r);

#ifdef _DEBUG_SCAN_POINTER
    TCHAR buff[100];
    StringCchPrintf(buff, 100, _T("data index=%d, total=%d"), m_ScanFolderIndex, m_ScanFolderDataLength);
    StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif

    if (e != NULL)
    {
      length -= block_len + 1;
      data = e + 1;
    }
    else
    {
      length = 0;
    }
  }

  // all data is done
  TCHAR buff[100];

  int folder_count = 0;
  int file_count = 0;
  if (mEcmRoot != NULL)
  {
    folder_count = mEcmRoot->GetFolderCount();
    file_count = mEcmRoot->GetFileCount();
  }
  // int folder_count = mFolderList.size();
  // int file_count = mFileList.size();

  StringCchPrintf(buff, 100, _T("ScanFolderItem found folders=%d, files=%d"),
    folder_count, file_count);
  SendLogMessage(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
  return R_SUCCESS;
}

EcmBaseItem* KBgTaskLoader::CreateFromStream(LPSTR d, long data_len, int& rtn)
{
  KString kstr;
  int str_len;
  char object_type = *d;
  EcmBaseItem* item = NULL;
  LPTSTR oid = NULL;
  int childItemModified = 0;

  LPSTR e = strchr(d, SCAN_VALUE_DELEMETER);
  if (e != NULL)
    str_len = (int)(e - d);
  else
    str_len = strlen(d);

  if (str_len == 11) // unchanged item start with OID
  {
    oid = MakeUnicodeString(d, str_len); // OID

    item = new EcmBaseItem(NULL, oid);

    if (item != NULL)
    {
      if (e != NULL)
      {
        d = e + 1;
        e = strchr(d, SCAN_VALUE_DELEMETER);
        if (e != NULL)
          str_len = (int)(e - d);
        else
          str_len = strlen(d);
        kstr.assign(d, str_len);
        childItemModified = kstr.toInt(NULL);
      }

#ifdef _DEBUG_SCAN_SERVER_FOLDERS
      TCHAR buff[300];
      StringCchPrintf(buff, 300, _T("Unchanged item : OID=%s, name=%s"), oid, item->mLocal.pFilename);
      if (childItemModified)
        StringCchCat(buff, 300, _T(", childItemModified"));
      StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif
    }

    if (oid != NULL)
      delete[] oid;
    return item;
  }

  data_len -= (int)(e - d + 1);

  d = e + 1;
  e = strchr(d, SCAN_VALUE_DELEMETER);
  if (e >= (d + data_len))
    e = NULL;

  if (e != NULL)
    str_len = (int)(e - d);
  else
    str_len = data_len;
  oid = MakeUnicodeString(d, str_len); // OID

  BOOL isRootFolder = FALSE;
  LPTSTR name = NULL;
  LPTSTR parent_oid = NULL;
  EcmBaseItem *parent = NULL;
  LPTSTR creatorOID = NULL;
  LPTSTR fullPathIndex = NULL;

  if (e == NULL)
  {
    // unchanged item
    OutputDebugString(_T("invalid data stream\n"));
    goto end_all;
  }
  else
  {
    d = e + 1;
    e = strchr(d, SCAN_VALUE_DELEMETER);
    if (e == NULL)
      goto end_all;
    name = MakeUnicodeString(d, (int)(e - d)); // name

    d = e + 1;
    e = strchr(d, SCAN_VALUE_DELEMETER);
    if (e == NULL)
      goto end_all;
    kstr.assign(d, (int)(e - d));
    int permissions = kstr.toInt(NULL, 16);

    d = e + 1;
    e = strchr(d, SCAN_VALUE_DELEMETER);
    if (e == NULL)
      goto end_all;
    kstr.assign(d, (int)(e - d));
    __int64 modifiedAt = kstr.toInt64();
    SYSTEMTIME systime;
    SystemTimeFromEPOCH_MS(systime, modifiedAt);

    d = e + 1;
    e = strchr(d, SCAN_VALUE_DELEMETER);
    if (e == NULL)
      goto end_all;
    parent_oid = MakeUnicodeString(d, (int)(e - d)); // parent OID

    parent = findFolderItemByOID(parent_oid);

    if (object_type == 'F')
    {
      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_folder;
      creatorOID = MakeUnicodeString(d, (int)(e - d)); // server folder creator

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);

      if (e != NULL)
      {
        fullPathIndex = MakeUnicodeString(d, (int)(e - d));
        d = e + 1;
        e = strchr(d, SCAN_VALUE_DELEMETER);
        if (e != NULL)
          kstr.assign(d, (int)(e - d));
        else
          kstr.assign(d);
        childItemModified = kstr.toInt(NULL);
      }
      else
      {
        fullPathIndex = MakeUnicodeString(d);
      }

#ifdef _DEBUG_SCAN_SERVER_FOLDERS_ITEM
      TCHAR buff[512];
      StringCchPrintf(buff, 512, _T("Name:%s, OID=%s, permissions=%d, modified=%lld, creator=%s, parent=%s, fullPathIndex=%s, Buffer=%d"),
        name, oid, permissions, modifiedAt, creatorOID, parent_oid, fullPathIndex, m_ScanFolderIndex);
      if (childItemModified)
        StringCchCat(buff, 512, _T(", childItemModified"));
      StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif

      // clear name of root item
      if (lstrcmp(mArg, oid) == 0)
      {
        isRootFolder = TRUE;
        if (name != NULL)
        {
          delete[] name;
          name = NULL;
        }
      }

      // skip 2-or-more-depth item
      if ((parent == NULL) || (parent->mParent == NULL))
      // skip root item, skip 2-or-more-depth item
      //if ((lstrcmp(mArg, oid) != 0) && (parent->mParent == NULL))
      {
        item = findFolderItemByOID(oid);

        if (item == NULL)
        {
          item = new EcmFolderItem(parent, name, oid, fullPathIndex);
          ((EcmFolderItem*)item)->SetFolderInfo(systime, permissions);

          if (parent != NULL)
            ((EcmFolderItem*)parent)->AddChild(item);
          else if (mEcmRoot == NULL)
            mEcmRoot = item;
          else
          {
            TCHAR msg[256] = _T("No parent of folder : ");
            StringCchCat(msg, 256, name);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_DEBUG);
          }
        }
        /*
        if (item != NULL)
        {
          item->UpdateServerNewFileInfo(0, systime, permissions,
            creatorOID, NULL, NULL, folderFullPath, fullPathIndex, name);
        }
        if (folderFullPath != NULL)
          delete[] folderFullPath;
        */
      }

    end_folder:
      if (creatorOID != NULL)
        delete[] creatorOID;
      if (fullPathIndex != NULL)
        delete[] fullPathIndex;
    }
    else if (object_type == 'D')
    {
      LPTSTR lastModifier = NULL;
      LPTSTR file_oid = NULL;
      LPTSTR storage_oid = NULL;

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_doc;
      lastModifier = MakeUnicodeString(d, (int)(e - d)); // server file last modifier

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_doc;
      file_oid = MakeUnicodeString(d, (int)(e - d)); // file OID

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_doc;
      storage_oid = MakeUnicodeString(d, (int)(e - d)); // storage OID

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e != NULL)
        kstr.assign(d, (int)(e - d));
      else
        kstr.assign(d);
      __int64 fileSize = kstr.toInt64();

#ifdef _DEBUG_SCAN_SERVER_FOLDERS_ITEM
      TCHAR buff[512];
      StringCchPrintf(buff, 512, _T("Name:%s, OID=%s, permission=%d, folder=%s, file_oid=%s, storage_oid=%s, fileSize=%lld, Buffer=%d"),
        name, oid, permissions, parent_oid, file_oid, storage_oid, fileSize, m_ScanFolderIndex);
      StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif

      // below on root or first-child
      if ((lstrcmp(mArg, parent_oid) == 0) || (parent != NULL))
      {
        item = findFileItemByOID(oid);

        if ((item == NULL) && HasValidFileExtension(name, TRUE))
        {
          item = new EcmFileItem(parent, name, oid, file_oid, storage_oid);
          ((EcmFileItem*)item)->SetFileInfo(fileSize, systime, permissions);
          if (parent != NULL)
            ((EcmFolderItem*)parent)->AddChild(item);
        }

        /*
        if (item != NULL)
        {
          item->UpdateServerOID(oid, NULL, NULL);

          if ((item->mPhase < PHASE_END_OF_JOB) &&
            item->CheckServerRenamedAs(p->GetBaseFolder(), name))
            item->SetNewParent(p);

          item->SetServerOID(oid);
        }
        else
        {
          LPTSTR pathName = AllocPathName(p->GetBaseFolder(), name);

          if (IsPartialSyncOn(pathName, TRUE))
          {
            item = new KSyncItem(p, p->GetBaseFolder(), name, oid, NULL, NULL);
            item->LoadMetafile(GetStorage(), META_CLEAR_STATE);
            p->AddChild(item);
          }
          else
          {
            TCHAR msg[300];
            StringCchPrintf(msg, 300, _T("Skip on Partial Sync : %s"), pathName);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
          }
          delete[] pathName;
        }

        if (item != NULL)
        {
          LPCTSTR pName = name;
          if (CompareStringICNC(item->mLocal.pFilename, name) == 0)
            pName = NULL;

          LPCTSTR parentPath = p->GetServerRelativePath();

          item->UpdateServerNewFileInfo(fileSize, systime, permissions,
            lastModifier, file_oid, storage_oid, parentPath, NULL, pName);
        }
        */
      }
    end_doc:
      if (lastModifier != NULL)
        delete[] lastModifier;
      if (file_oid != NULL)
        delete[] file_oid;
      if (storage_oid != NULL)
        delete[] storage_oid;
    }
  }

end_all:
  if (oid != NULL)
    delete[] oid;
  if (name != NULL)
    delete[] name;
  if (parent_oid != NULL)
    delete[] parent_oid;

  return item;
}

int KBgTaskLoader::GetLoadNeededFolder(int needed_flag)
{
  return ((EcmFolderItem*)mEcmRoot)->GetLoadNeededFolder(needed_flag);
  /*
  int count = 0;
  KVoidPtrIterator it = mFolderList.begin();
  while (it != mFolderList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (item->IsNeed(needed_flag))
      count++;
  }
  return count;
  */
}

int KBgTaskLoader::GetLoadNeededFile(int needed_flag)
{
  return ((EcmFolderItem*)mEcmRoot)->GetLoadNeededFile(needed_flag);
  /*
  int count = 0;
  KVoidPtrIterator it = mFileList.begin();
  while (it != mFileList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (item->IsNeed(needed_flag))
      count++;
  }
  return count;
  */
}

EcmBaseItem* KBgTaskLoader::findFolderItemByName(LPCTSTR name)
{
  EcmBaseItem* item = NULL;
  if (mEcmRoot != NULL)
  {
    item = mEcmRoot->FindFolderItemByName(name);
  }
  return item;
  /*
  KVoidPtrIterator it = mFolderList.begin();
  while (it != mFolderList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (lstrcmp(item->mName, name) == 0)
      return item;
  }
  return NULL;
  */
}

EcmBaseItem* KBgTaskLoader::findFileItemByName(LPCTSTR parentName, LPCTSTR name)
{
  EcmBaseItem* item = NULL;
  if (mEcmRoot != NULL)
  {
    EcmFolderItem* folder = (EcmFolderItem*)mEcmRoot;
    if ((parentName != NULL) && (lstrlen(parentName) > 0))
      folder = (EcmFolderItem*)folder->FindFolderItemByName(parentName);

    if (folder != NULL)
    {
      item = folder->FindItemByName(name);
    }
  }
  return item;
  /*
  KVoidPtrIterator it = mFileList.begin();
  while (it != mFileList.end())
  {
    EcmFileItem* item = (EcmFileItem*)*(it++);
    LPTSTR par_Name = NULL;
    if (item->mParent != NULL)
      par_Name = item->mParent->mName;

    if ((lstrcmp(item->mName, name) == 0) &&
      (CompareStringNC(parentName, par_Name) == 0))
      return item;
  }
  return NULL;
  */
}

EcmBaseItem* KBgTaskLoader::findFolderItemByOID(LPCTSTR oid)
{
  EcmBaseItem* item = NULL;
  if (mEcmRoot != NULL)
    item = mEcmRoot->FindFolderItemByOID(oid);
  return item;
  /*
  KVoidPtrIterator it = mFolderList.begin();
  while (it != mFolderList.end())
  {
    EcmFolderItem* item = (EcmFolderItem*)*(it++);
    if (lstrcmp(item->folderOID, oid) == 0)
      return item;
  }
  return NULL;
  */
}

EcmBaseItem* KBgTaskLoader::findFileItemByOID(LPCTSTR oid)
{
  EcmBaseItem* item = NULL;
  if (mEcmRoot != NULL)
    item = mEcmRoot->FindFileItemByOID(oid);
  return item;
  /*
  KVoidPtrIterator it = mFileList.begin();
  while (it != mFileList.end())
  {
    EcmFileItem* item = (EcmFileItem*)*(it++);
    if (lstrcmp(item->docOID, oid) == 0)
      return item;
  }
  return NULL;
  */
}

HANDLE KBgTaskLoader::createListData(KVoidPtrArray& fileList)
{
  HANDLE h = NULL;

  if (fileList.size() > 0)
  {
    UINT length = 0;
    LPTSTR pathName = NULL;

    // base folder length
    length += lstrlen(mWorkingPath) + 1;

    // file item name length
    KVoidPtrIterator it = fileList.begin();
    while (it != fileList.end())
    {
      EcmFileItem* item = (EcmFileItem*)*(it++);
      pathName = item->MakeItemInfo();
      length += lstrlen(pathName) + 1;
      delete[] pathName;
    }
    length += 4;

    // allocate buffer
    h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, length * sizeof(TCHAR));
    LPTSTR p = (LPTSTR)GlobalLock(h);

    // add base folder path
    StringCchCopy(p, length, mWorkingPath);
    p += lstrlen(mWorkingPath) + 1;
    length -= lstrlen(mWorkingPath) + 1;

    // add each file items
    it = fileList.begin();
    while (it != fileList.end())
    {
      EcmFileItem* item = (EcmFileItem*)*(it++);
      pathName = item->MakeItemInfo();
      StringCchCopy(p, length, pathName);

      p += lstrlen(p) + 1;
      length -= lstrlen(p) + 1;
      delete[] pathName;
    }
    if (length > 0)
      *p = '\0';

    GlobalUnlock(h);
  }
  return h;
}
