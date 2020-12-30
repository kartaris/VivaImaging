#include "StdAfx.h"
#include <tchar.h>
#include "KSyncRootFolderItem.h"
#include "KSyncCoreBase.h"
#include "KConstant.h"
#include "KSettings.h"
#ifdef USE_SQLITE_META
#include "KSqliteUtil.h"
#endif

#include "../Platform/Platform.h"

#ifdef USE_SYNC_STORAGE
#include "KSyncRootStorageFile.h"
#include "KSyncRootStorageDB.h"
#include "DirectoryMonitor/KRenameStorage.h"
#endif


#include <Strsafe.h>
#include <Shlwapi.h>

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

KSyncRootFolderItem::KSyncRootFolderItem(LPCTSTR fullpathname)
  : KSyncFolderItem(NULL, fullpathname)
{
  if ((fullpathname != NULL) && (lstrlen(fullpathname) > 0))
  {
    if (IsAbsolutePathName(fullpathname))
    {
      SetRelativePathName(fullpathname);
    }
  }
  InitFolderItem();
}

KSyncRootFolderItem::KSyncRootFolderItem(LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid)
  : KSyncFolderItem(NULL, pathname, filename, server_oid, server_file_oid, server_storage_oid)
{
  InitFolderItem();
}

KSyncRootFolderItem::KSyncRootFolderItem(KSyncRootFolderItem &item)
  :KSyncFolderItem(NULL)
{
  *this = item;
}

KSyncRootFolderItem::~KSyncRootFolderItem()
{
  clearAllSyncingItem();
  clearCancelRenamedItem(FALSE);
#ifdef USE_SYNC_STORAGE
  if (mStorage != NULL)
  {
    delete mStorage;
    mStorage = NULL;
  }
#endif

  if (mSyncOnlyPath != NULL)
  {
    delete[] mSyncOnlyPath;
    mSyncOnlyPath = NULL;
  }
  if (mSyncExtraInfo != NULL)
  {
    delete[] mSyncExtraInfo;
    mSyncExtraInfo = NULL;
  }
#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
  if (m_ScanFolderData != NULL)
  {
    delete[] m_ScanFolderData;
    m_ScanFolderData = NULL;
  }
#endif

#ifdef USE_BACKUP
  if (mDOS_FolderList != NULL)
  {
    delete[] mDOS_FolderList;
    mDOS_FolderList = NULL;
  }
  if (mDOS_FileList != NULL)
  {
    delete[] mDOS_FileList;
    mDOS_FileList = NULL;
  }
#endif

#if defined(USE_SYNC_GUI) && defined(PURE_INSTANT_BACKUP)
  FreeModifiedItemList(mModifiedItemList);
  if (mModifiedKey != NULL)
  {
    delete[] mModifiedKey;
    mModifiedKey = NULL;
  }
#endif
}

KSyncRootFolderItem &KSyncRootFolderItem::operator=(KSyncRootFolderItem &item)
{
  *((KSyncFolderItem *)this) = (KSyncFolderItem &)item;

  mSynchState = item.mSynchState;
  mSyncPaused = item.mSyncPaused;
  mSyncMethod = item.mSyncMethod;

  // sync progress
  mSyncTotalSize = item.mSyncTotalSize;
  mSyncDoneSize = item.mSyncDoneSize;
  mSyncTotalCount = item.mSyncTotalCount;
  mSyncFolderCount = item.mSyncFolderCount;
  mSyncFileCount = item.mSyncFileCount;
  mTargetFolderCount = item.mTargetFolderCount;
  mTargetFileCount = item.mTargetFileCount;

  mSyncItemFileSize = item.mSyncItemFileSize;
  mSyncProcessedSize = item.mSyncProcessedSize;

#ifdef USE_SYNC_STORAGE
  mStorage = NULL;
#endif

  // sync start/end time
  memcpy(&mSyncStartTime, &item.mSyncStartTime, sizeof(mSyncStartTime));
  memcpy(&mSyncDoneTime, &item.mSyncDoneTime, sizeof(mSyncDoneTime));
  memset(&mSyncBeginTime, 0, sizeof(mSyncBeginTime));
  memset(&mLastSyncCompleteTime, 0, sizeof(mLastSyncCompleteTime));

  mDbVersion = item.mDbVersion;
  mSyncOnlyPath = AllocString(item.mSyncOnlyPath);
  mSyncExtraInfo = AllocString(item.mSyncExtraInfo);

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
  mPreviousScanTime = 0;
  mLastScanTime = 0;
  mServerFolderScanResult = SFS_NO_ERROR;

  m_ScanFolderData = NULL;
  m_ScanFolderDataLength = 0;
  m_ScanFolderIndex = 0;
  m_ScanFolderItem = NULL;
  m_ScanFolderFlag = 0;
  m_ScanParseLastParent = NULL;
#endif

  return *this;
}

void KSyncRootFolderItem::InitFolderItem()
{
  int item_size = sizeof(KSyncItem); // 2608
  int file_size = sizeof(KSyncFolderItem); // 2648

  // sync action
  mSynchState = 0; // none, pending, synching, sync-done, sync-paused
  mSyncPaused = 0; // pause/resume
  mSyncMethod = 0;

  // sync progress
  mSyncTotalSize = 0;
  mSyncDoneSize = 0;

  mSyncTotalSizeProgress = 0;
  mSyncDoneSizeProgress = 0;

  mSyncTotalCount = 0;
  mSyncFolderCount = 0;
  mSyncFileCount = 0;
  mTargetFolderCount = 0;
  mTargetFileCount = 0;

  mSyncItemFileSize = 0;
  mSyncProcessedSize = 0;
  mSyncTotalSizeProgress = 0;
  mSyncDoneSizeProgress = 0;

  // sync start/end time
  memset(&mSyncBeginTime, 0, sizeof(mSyncBeginTime));
  memset(&mSyncStartTime, 0, sizeof(mSyncStartTime));
  memset(&mSyncDoneTime, 0, sizeof(mSyncDoneTime));

#ifdef USE_SYNC_STORAGE
  mStorage = NULL;
  moldStorage = NULL;
#endif

  mPhase = 0;
  mDbVersion = 0;
  mSyncOnlyPath = NULL;
  mSyncExtraInfo = NULL;
  memset(&mLastSyncCompleteTime, 0, sizeof(mSyncDoneTime));

#ifdef USE_BACKUP
  mDOS_FolderList = NULL;
  mDOS_FolderListSize = 0;
  mDOS_FileList = NULL;
  mDOS_FileListSize = 0;
  mRestoreFolderOID[0] = '\0';
  mInstantBackupRetry = 0;
#endif

#if defined(USE_SYNC_GUI) && defined(PURE_INSTANT_BACKUP)
  mModifiedKey = NULL;
#endif

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
  mPreviousScanTime = 0;
  mLastScanTime = 0;
  mServerFolderScanResult = SFS_NO_ERROR;

  m_ScanFolderData = NULL;
  m_ScanFolderDataLength = 0;
  m_ScanFolderIndex = 0;
  m_ScanFolderItem = NULL;
  m_ScanFolderFlag = 0;
  m_ScanParseLastParent = NULL;
#endif

  SerializePolicy(FALSE);
}

#ifdef USE_SYNC_STORAGE

KSyncRootStorage* KSyncRootFolderItem::GetStorage(BOOL readonly)
{
  if ((mStorage != NULL) && mStorage->m_readOnly && !readonly)
  {
    delete mStorage;
    mStorage = NULL;
  }

#ifdef USE_SYNC_STORAGE
#ifdef USE_BACKUP
  if (mStorage == NULL)
    mStorage = new KSyncRootStorageDB(mHomeFolder, readonly);
#else
  LPCTSTR baseFolder = GetRootPath();
  if ((mStorage == NULL) && (baseFolder != NULL))
  {
#ifdef USE_SYNC_STORAGE
    if (KSyncRootStorageFile::IsMatchRootFolder(baseFolder, FALSE) > 0)
      mStorage = new KSyncRootStorageFile(baseFolder, readonly);
    else if (KSyncRootStorageDB::IsMatchRootFolder(baseFolder, FALSE) > 0)
      mStorage = new KSyncRootStorageDB(baseFolder, readonly);
#endif
  }
#endif // USE_BACKUP
#endif // USE_SYNC_STORAGE
  return mStorage;
}

void KSyncRootFolderItem::CloseStorage()
{
  if (mStorage != NULL)
  {
    delete mStorage;
    mStorage = NULL;
  }
  if (moldStorage != NULL)
  {
    delete moldStorage;
    moldStorage = NULL;
  }
}

BOOL KSyncRootFolderItem::LoadSyncPolicy(KSyncRootStorage* s)
{
  return s->LoadSyncPolicy(mPolicy);
}

int KSyncRootFolderItem::LoadChildItemCascade(LPCTSTR relativePath, int child_flag)
{
  int len = lstrlen(relativePath) + 1;
  LPTSTR subPath = new TCHAR[len];
  StringCchCopy(subPath, len, relativePath);
  LPTSTR s = subPath;
  LPTSTR x;
  int r = KSyncRootStorage::R_DONE;
  KSyncFolderItem* p = this;

  while (*s != '\0')
  {
    LPTSTR n = StrChr(s, '\\');
    int f;

    if (n != NULL)
    {
      *n = '\0';
      x = n + 1;
      f = 0;
    }
    else
    {
      x = s + lstrlen(s);
      f = child_flag; // 마지막에는 하위 항목 포함으로
    }
    r = mStorage->LoadChildMetaData(this, subPath, f, 0);

    if (r == KSyncRootStorage::R_NO_ITEM)
      break;

    if (n != NULL)
      *n = '\\';

    s = x;
  }

  delete[] subPath;
  return r;
}

int KSyncRootFolderItem::LoadChildItem(LPCTSTR relativePath, int child_flag)
{
  return mStorage->LoadChildMetaData(this, relativePath, child_flag, 0);
}

KSyncItem* KSyncRootFolderItem::ReadyChildItemCascade(KSyncFolderItem* p, LPTSTR pathName)
{
  KSyncItem* ch = NULL;
  LPTSTR s = pathName;

  while (s != NULL)
  {
    LPTSTR e = StrChr(s, '\\');
    if (e != NULL)
    {
      *e = '\0';
    }

    ch = p->FindChildItem(s, FALSE);
    if (ch == NULL)
    {
      mStorage->LoadChildMetaData(this, pathName, 0, 0);
      ch = p->FindChildItem(s, FALSE);
    }

    if (ch != NULL)
      p = (KSyncFolderItem*)ch;
    else
      return NULL;

    if (e != NULL)
    {
      *e = '\\';
      s = e + 1;
    }
    else
    {
      break;
    }
  }
  return ch;
}

KSyncItem* KSyncRootFolderItem::LoadItemFromStorage(LPCTSTR itemOID)
{
  KSyncItem* item = mStorage->LoadItemFromStorage(this, itemOID);
  
  // find & attach with parent
  if ((item != NULL) && (item->mpRelativePath != NULL))
  {
    LPTSTR parentPathName = NULL;
    SplitPathNameAlloc(item->mpRelativePath, &parentPathName, NULL);
    if (parentPathName != NULL)
    {
      KSyncFolderItem* p = (KSyncFolderItem*) FindChildItemCascade(parentPathName, 1);

      // 부모 폴더가 없으면 부모 폴더도 로드한다.
      if (p == NULL)
      {
        p = (KSyncFolderItem*) ReadyChildItemCascade(this, parentPathName);
      }
#ifdef _DEBUG_CODE
      else
      {
        // 코드 확인
        p = (KSyncFolderItem*) ReadyChildItemCascade(this, parentPathName);
      }
#endif

      if (p != NULL)
      {
        p->AddChild(item);
        item->SetParent(p);
      }

      delete[] parentPathName;
    }
  }
  return item;
}

#endif // USE_SYNC_STORAGE

void KSyncRootFolderItem::GetSyncFileProgress(SYNC_PROGRESS_STRUCT &progress)
{
  progress.total_size = mSyncTotalSize;
#ifdef USE_BACKUP
  StringCchCopy(progress.mFilename, KMAX_PATH, mServer.pFilename);
  progress.method = mSyncMethod;
#else
  StringCchCopy(progress.mFilename, KMAX_PATH, GetFilename());
#endif

  progress.total = (mSyncTotalCount < 0) ? 0 : mSyncTotalCount;
  progress.folder_done = mSyncFolderCount;
  progress.file_done = mSyncFileCount;

  float progress_percent = 0;
  if(mPhase <= PHASE_COMPARE_FILES)
  {
    progress.percent = 0;
  }
  else
  {
#ifdef BY_NUMBER_OF_FILES  // by number of files
    if(mSyncTotalCount > 0)
      *percent = 10000 * mSyncFileCount / mSyncTotalCount;
#else
    if((mSyncTotalSizeProgress > 0) && (mSyncTotalCount > 0))
    {
      progress_percent = (float)(mSyncDoneSizeProgress + mSyncProcessedSize) / mSyncTotalSizeProgress;

      /*
      float progress_percent = (float)(mSyncDoneSize + mSyncProcessedSize) / mSyncTotalSizeProgress;
      if (mSyncFileCount < mSyncTotalCount)
        progress_percent *= (float) (mSyncFileCount + 1) / mSyncTotalCount;
      */

      if (progress_percent > 1.0)
        progress_percent = 1.0;
      progress.percent = (int)(10000 * progress_percent);
    }
#endif
    else
      progress.percent = 0;
  }
  progress.phase = mPhase;

  if (progress.percent > 0)
  {
    SYSTEMTIME current;
    ::GetLocalTime(&current);
    GetRemainedTime(mSyncStartTime, current, progress_percent, progress.remained_time);

#ifdef _DEBUG_PROGRESS_REMAIN_TIME
    TCHAR msg[200];
    StringCchPrintf(msg, 200, _T("%lld + %lld / %lld, percent=%d, "), mSyncDoneSizeProgress, mSyncProcessedSize,
      mSyncTotalSizeProgress, progress.percent);

    TCHAR time[60];
    SystemTimeToTString(time, 60, progress.remained_time);
    StringCchCat(msg, 200, time);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    // OutputDebugString(msg);
#endif
  }
}

#if 0
BOOL KSyncRootFolderItem::GetSyncFileRemainedTime(SYSTEMTIME &time)
{
  SYSTEMTIME current;
  ::GetLocalTime(&current);

  float progressed_percent = 1.0;
#ifdef BY_NUMBER_OF_FILES  // by number of files
  if (mSyncTotalCount > 0)
    progressed_percent = (float)mSyncFileCount / mSyncTotalCount;
#else // by byte sizes
  if ((mSyncTotalSizeProgress > 0) && (mSyncTotalCount > 0))
  {
    progressed_percent = (float)(mSyncDoneSizeProgress + mSyncProcessedSize) / mSyncTotalSizeProgress;
    /*
    progressed_percent = (float)(mSyncDoneSize + mSyncProcessedSize) / mSyncTotalSizeProgress;

    if (mSyncFileCount < mSyncTotalCount)
    {
      float count_percent = (float) (mSyncFileCount + 1) / mSyncTotalCount;
      progressed_percent *= count_percent;
    }
    */

    /*
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("total=%lld, progress=%lld\n"), mSyncTotalSize, (mSyncDoneSize + mSyncProcessedSize));
    OutputDebugString(msg);
    */
  }
#endif

  return GetRemainedTime(mSyncStartTime, current, progressed_percent, time);
}
#endif

BOOL KSyncRootFolderItem::GetRemainedTime(SYSTEMTIME& start, SYSTEMTIME& end, float percent, SYSTEMTIME& OUT remain)
{
  SYSTEMTIME ellapsed;
  TimeDifference(ellapsed, start, end);

  long ellapsed_sec = ellapsed.wSecond + (ellapsed.wMinute + (ellapsed.wHour + (ellapsed.wDay + (ellapsed.wMonth + (long)ellapsed.wYear * 12) * 32) * 24) * 60) * 60;
  long remaining = -1;
  if (percent > 0)
    remaining = (long)(ellapsed_sec / percent) - ellapsed_sec;

  memset(&remain, 0, sizeof(remain));
  if(remaining > 0)
  {
    remain.wSecond = remaining % 60;
    remaining = remaining / 60;
    if(remaining > 0)
    {
      remain.wMinute = remaining % 60;
      remaining = remaining / 60;
      if(remaining > 0)
      {
        remain.wHour = remaining % 24;
        remaining = remaining / 24;
        if(remaining > 0)
        {
          remain.wDay = remaining % 32;
          remaining = remaining / 32;
          if(remaining > 0)
          {
            remain.wMonth = remaining % 12;
            remain.wYear = (WORD)(remaining / 12);
          }
        }
      }
    }

    /*
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("start=%d:%d:%d, current=%d:%d:%d, ellapsed=%d:%d:%d, remain=%d:%d:%d\n"), 
      (int)start.wHour, (int)start.wMinute, (int)start.wSecond,
      (int)end.wHour, (int)end.wMinute, (int)end.wSecond,
      (int)ellapsed.wHour, (int)ellapsed.wMinute, (int)ellapsed.wSecond,
      (int)remain.wHour, (int)remain.wMinute, (int)remain.wSecond);
    OutputDebugString(msg);
    */
    return TRUE;
  }
  return FALSE;
}

#ifdef USE_SCAN_BY_HTTP_REQUEST
void KSyncRootFolderItem::OnScanFolderProgress(int folders, int files)
{
    mSyncFolderCount = folders;
    mSyncFileCount = files;
}
#endif

void KSyncRootFolderItem::OnFileSyncProgress(LPCTSTR name, int progress)
{
  if (mSyncItemFileSize > 0)
    mSyncProcessedSize = mSyncItemFileSize * progress / 10000;
}

int KSyncRootFolderItem::MergeSameSyncJob(LPCTSTR pathname, int method, LPCTSTR syncOnlyPath)
{
  if((mSyncMethod & SYNC_METHOD_BACKUP_RESTORE) == (method & SYNC_METHOD_BACKUP_RESTORE))
  {
    // 전체 백업 - 실시간 백업 : 포함되는 것으로
    if (!(mSyncMethod & SYNC_METHOD_INSTANT) && 
      (method & SYNC_METHOD_INSTANT))
    {
      if ((method & SYNC_MERGABLE) &&
        MergeInstantBackupItem(syncOnlyPath))
      return R_FAIL_SYNCING_MERGED;
    }
  }
  return 0;
}

int KSyncRootFolderItem::IsSameSyncJob(LPCTSTR pathname, int method, LPCTSTR syncOnlyPath)
{
  if ( (IsEmptyPath(pathname) && IsEmptyPath(mpRelativePath)) ||
    (StrCmpI(mpRelativePath, pathname) == 0) )
  {
#if defined(USE_BACKUP)
    if ((mSyncMethod & SYNC_METHOD_TYPE) == (method & SYNC_METHOD_TYPE))
    {
#ifdef PURE_INSTANT_BACKUP
      // 실시간 백업에서는 항상 다른 것으로 처리
      if (mSyncMethod & SYNC_METHOD_INSTANT)
      {
        return 0;
      }
#else
      if (CompareRelativePath(mSyncOnlyPath, syncOnlyPath) != 0)
        return 0;
#endif
      return R_FAIL_ALREADY_SYNCING;
    }

    if((mSyncMethod & SYNC_METHOD_BACKUP_RESTORE) == (method & SYNC_METHOD_BACKUP_RESTORE))
{
      // 전체 백업 - 실시간 백업 : 포함되는 것으로
      if (!(mSyncMethod & SYNC_METHOD_INSTANT) && 
        (method & SYNC_METHOD_INSTANT))
  {
        if ((method & SYNC_MERGABLE) &&
          MergeInstantBackupItem(syncOnlyPath))
        return R_FAIL_SYNCING_MERGED;
      }
    }
#else
    if (mSyncMethod & method)
      return R_FAIL_ALREADY_SYNCING;
#endif
  }
  return 0;
}

int KSyncRootFolderItem::IsSameSyncJob(LPCTSTR folder, LPCTSTR name, int method, LPCTSTR syncOnlyPath)
{
  int r = 0;

  if (!IS_EMPTY_STR(folder))
  {
    int len = lstrlen(folder) + lstrlen(name) + 2;
    LPTSTR pathname = new TCHAR[len];
    MakeFullPathName(pathname, len, folder, name);
    r = IsSameSyncJob(pathname, method, syncOnlyPath);
    delete[] pathname;
  }
  else
  {
    r = IsSameSyncJob(NULL, method, syncOnlyPath);
  }
  return r;
}

BOOL KSyncRootFolderItem::MergeInstantBackupItem(LPCTSTR syncOnlyPath)
{
  return FALSE;
}

#if defined(USE_SYNC_GUI) && defined(PURE_INSTANT_BACKUP)

#define _DEBUG_INSTANT_MODIFY_LIST

// 실시간 백업의 대상 항목 리스트
BOOL KSyncRootFolderItem::AddModifiedItemList(KVoidPtrArray& modifiedList)
{
  // MergeModifiedItemList(mModifiedItemList, modifiedList, ONLY_SAME_DRIVE);
  // 드라이브 달라도 같이 처리한다. => 드라이브 
  MergeModifiedItemList(mModifiedItemList, modifiedList, 0);

  if (mModifiedItemList.size() > 0)
  {
    TCHAR msg[300];
    LPTSTR rootPath = NULL;

    KVoidPtrIterator it = mModifiedItemList.begin();
    while (it != mModifiedItemList.end())
    {
      ModifiedItemData* p = (ModifiedItemData*)*it++;

#ifdef _DEBUG_INSTANT_MODIFY_LIST
      StringCchPrintf(msg, 300, _T("ModifiedItem=%s, type=%s"), p->pathName, 
        getModifyEventName((DIR_MODIFY_EVENT)p->modifyType));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
#endif

      // 블필요.
      if (p->modifyType == DIR_MOVED)
        continue;

#ifdef _DEBUG_SYNCONLY_TEMP
      if (rootPath == NULL)
        AllocCopyString(&rootPath, p->pathName);
#else
      AllocMergePath(&rootPath, p->pathName);
#endif

      if (p->modifyType == DIR_MOVED_NEW)
      {
        LPTSTR originName = NULL;
        if (AllocMovedRenamedOriginalName(p->pathName, &originName))
        {
          // 원본 경로가 rootPath에 포함되도록 한다
          AllocMergePath(&rootPath, originName);
          delete originName;
        }
      }
    }

#ifdef _DEBUG_INSTANT_MODIFY_LIST
    StringCchPrintf(msg, 300, _T("SyncOnlyPath : %s"), rootPath);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
#endif

    // 변경된 항목들의 최상위 폴더
    if (rootPath != NULL)
    {
      if ((lstrlen(rootPath) > 0) && (IsFileOrFolder(rootPath) == 0))
      {
        LPTSTR e = StrRChr(rootPath, NULL, '\\');
        if (e != NULL)
          *e = '\0';
      }

      SetSyncOnlyPath(rootPath);
      delete[] rootPath;
    }
    return TRUE;
  }
  return FALSE;
}

void KSyncRootFolderItem::SetModifiedListKey(LPCTSTR modifiedKey)
{
  AllocCopyString(&mModifiedKey, modifiedKey);
}

#endif // USE_SYNC_GUI

#ifdef USE_SYNC_STORAGE

BOOL KSyncRootFolderItem::ReadyMetafilename(KSyncRootStorage *s, BOOL createIfNotExist)
{
  if ((s != NULL) && (s->GetStorageType() == KSyncRootStorage::SRS_FILE))
  {
    if ((mpMetafilename != NULL) && (lstrlen(mpMetafilename) > 0))
      return TRUE;

    if (mpMetafilename != NULL)
      delete mpMetafilename;

    int len = lstrlen(s->mpBaseFolder) + 30;
    mpMetafilename = new TCHAR[len];

    MakeFullPathName(mpMetafilename, len, s->mpBaseFolder, meta_folder_name);
	  MakeFullPathName(mpMetafilename, len, mpMetafilename, object_folder_name);
	  MakeFullPathName(mpMetafilename, len, mpMetafilename, ROOT_METAFILENAME);
  }
  return TRUE;
}
#endif

int KSyncRootFolderItem::GetActualPathName(LPTSTR fullname, int len, BOOL abs)
{
  if (abs)
  {
#ifdef SYNC_ALL_DRIVE
    if (fullname != NULL)
      fullname[0] = '\0';
    return 0;
#else
    if (fullname != NULL)
      StringCchCopy(fullname, len, GetRootPath());
    return (lstrlen(GetRootPath()) + 1);
#endif
  }
  else
  {
    if (fullname != NULL)
      fullname[0] = '\0';
    return 0;
  }
}

LPCTSTR KSyncRootFolderItem::GetServerFullPath()
{
  if ((mServerNew.pFullPath != NULL) && (lstrlen(mServerNew.pFullPath) > 0))
    return mServerNew.pFullPath;
  return mServer.pFullPath;
}

#if 0
void KSyncRootFolderItem::GetSyncResultName(LPTSTR name, int length, int flag)
{
  if (!(flag & AS_LOCAL))
  {
    LPCTSTR pPath = GetServerFullPath();
    if ((pPath != NULL) && (lstrlen(pPath) >= 0))
    {
      // server folder name. remove full path
      LPCTSTR p = StrRChr(pPath, NULL, '>');
      if (p == NULL)
        p = pPath;
      else
        p++;

      if (p != NULL)
      {
        StringCchCopy(name, length, p);
        return;
      }
    }
  }
  KSyncItem::GetSyncResultName(name, length, flag);
}
#endif

void KSyncRootFolderItem::ClearUnfinishedBackupState(int method)
{
#ifdef __OLD_LAST_JOB
  KSyncRootStorage* storage = GetStorage();

  storage->StoreSettingValue(KEYB_SYNC_STATE, "");

  int sync_key = 0;

  if (method & SYNC_METHOD_BACKUP)
  {
    sync_key = (method & SYNC_METHOD_AUTO) ? KEYB_SYNC_FULL_STATE : KEYB_SYNC_INSTANT_STATE;
  }
  else if (method & SYNC_METHOD_RESTORE)
  {
    sync_key = KEYB_SYNC_FULL_STATE;
  }
  if (sync_key > 0)
    storage->StoreSettingValue(sync_key, "");
#endif
}

#ifdef USE_BACKUP

int KSyncRootFolderItem::GetUnfinishedBackupState(OUT LPTSTR* pState)
{
  KSyncRootStorage* storage = GetStorage();
  return storage->LoadSettingValue(KEYB_SYNC_STATE, pState);
}

BOOL KSyncRootFolderItem::StoreUnfinishedBackupState(LPTSTR state)
{
  KSyncRootStorage* storage = GetStorage();
  LPSTR value = MakeUtf8String(state);
  BOOL r = storage->StoreSettingValue(KEYB_SYNC_STATE, value);
  if (value != NULL)
    delete[] value;
  return r;
}
#endif

LPCTSTR getSyncTypeName(int method)
{
  static LPCTSTR sync_name[] = { 
    _T("ECM-Instant-Backup"),
    _T("ECM-Full-Backup"),
    _T("ECM-Restore"),
    _T("ECM-AutoSync"),
    _T("ECM-ManualSync"),
    _T("ECM-Unknown")
  };

  if (method & SYNC_METHOD_BACKUP)
  {
    if (method & SYNC_METHOD_INSTANT)
      return sync_name[0];
    if (method & SYNC_METHOD_AUTO)
      return sync_name[1];
  }
  else if (method & SYNC_METHOD_RESTORE)
  {
    return sync_name[2];
  }
  else
  {
    if (method & SYNC_METHOD_AUTO)
      return sync_name[3];
    else
      return sync_name[4];
  }
  return sync_name[5];
}

#ifdef USE_BACKUP_JOB_LIST
LPTSTR KSyncRootFolderItem::JobToString()
{
  int len = lstrlen(mSyncOnlyPath) + lstrlen(mModifiedKey) + 10;

  // 변경목록 key
  LPTSTR buff = new TCHAR[len];
  LPTSTR d = buff;

  if(mSyncMethod & SYNC_METHOD_BACKUP)
  {
    if (mSyncMethod & SYNC_METHOD_AUTO)
      d[0] = 'F';
    else
      d[0] = 'I';
  }
  else //if(mSyncMethod & SYNC_METHOD_RESTORE)
  {
    d[0] = 'R';
  }
  d++;
  --len;

  *d = DA_VALUE_SEPARATOR;
  ++d;
  --len;

  if ((mSyncOnlyPath != NULL) && (lstrlen(mSyncOnlyPath) > 0))
  {
    StringCchCopy(d, len, mSyncOnlyPath);
    d += lstrlen(mSyncOnlyPath);
    len -= lstrlen(mSyncOnlyPath);
  }

  *d = DA_VALUE_SEPARATOR;
  ++d;
  --len;

  if ((mModifiedKey != NULL) && (lstrlen(mModifiedKey) > 0))
  {
    StringCchCopy(d, len, mModifiedKey);
    d += lstrlen(mModifiedKey);
    len -= lstrlen(mSyncOnlyPath);
  }
  *d = '\0'; 
  return buff;
}

int KSyncRootFolderItem::ParseUnfinishedBackupState(LPTSTR state, LPTSTR *syncOnly, LPTSTR *key)
{
  int r = 0;

  if (state[0] == 'R')
    r = SYNC_METHOD_RESTORE;
  else if (state[0] == 'F')
    r = SYNC_METHOD_BACKUP | SYNC_METHOD_AUTO;
  else if (state[0] == 'I')
    r = SYNC_METHOD_BACKUP | SYNC_METHOD_INSTANT;

  if (r != 0)
  {
    LPTSTR p = StrChr(state, DA_VALUE_SEPARATOR);
    if (p != NULL)
    {
      *p = '\0';
      p++;
      LPTSTR e = StrChr(p, DA_VALUE_SEPARATOR);
      if (e != NULL)
      {
        *e = '\0';
        e++;
      }

      if ((p != NULL) && (syncOnly != NULL) && (lstrlen(p) > 0))
        *syncOnly = AllocString(p);
      if ((e != NULL) && (key != NULL) && (lstrlen(e) > 0))
        *key = AllocString(e);
    }
  }
  return r;
}

#endif

int KSyncRootFolderItem::StartSync(int sync_flag)
{
#ifdef USE_SYNC_ENGINE
  mPhase = PHASE_STARTING;

  mSyncTotalCount = 0;
  mSyncFolderCount = 0;
  mSyncFileCount = 0;
  mSyncTotalSize = 0;
  mSyncDoneSize = 0;
  
  mSyncTotalSizeProgress = 0;
  mSyncDoneSizeProgress = 0;

  ::GetLocalTime(&mSyncStartTime);
  memcpy(&mSyncBeginTime, &mSyncStartTime, sizeof(mSyncStartTime));
  memset(&mSyncDoneTime, 0, sizeof(mSyncDoneTime));

  // change root metafilename
  if (mSyncMethod & SYNC_METHOD_SYNCING)
  {
    ReadyMetafilename(GetStorage(), METAFILE_CREATE_IF_NOT_EXIST);
#ifdef _DEBUG
    GetStorage()->dumpRenamedItems();
#endif
  }

  RefreshLocalFileInfo(mUserOid);

  mStorage->LoadSettingValue(KEYB_META_DB_IMM, mDbVersion);

  if (!(sync_flag & SYNC_METHOD_IMMIGRATION) &&
    (mDbVersion == 0))
    mDbVersion = CURRENT_DB_VERSION;

  if ((mDbVersion != CURRENT_DB_VERSION) &&
    (mSyncOnlyPath != NULL) && (lstrlen(mSyncOnlyPath) > 0))
  {
    // 마이그레이션 이후에는 전체 동기화 먼저.
    return NEED_UI_FOR_FULLSYNC;
  }

  // update policy
  /* move to CoreEngine
  TCHAR *buff = gpCoreEngine->GetServerConfig(SERVER_KEY_EXCLUDE_MASKS);
  mPolicy.UpdateSyncExcludeMasks(buff);
  if (buff != NULL)
    delete[] buff;
  */

  mPolicy.SetSyncFlag(sync_flag);
  mPolicy.SetSyncMethod(mSyncMethod);

  // set sync-start flag
  if (mSyncMethod & SYNC_METHOD_SYNCING)
  {
    TCHAR msg[300];
    mResult |= FILE_NOW_SYNCHRONIZING;
    KSyncRootStorage* s = GetStorage();

    __int64 file_size = s->GetCurrentStorageSize();
    file_size += 16 * 1024;
    if (gpCoreEngine->CheckReadyDiskSpace(GetRootPath(), file_size) != R_SUCCESS) // need 16K minimum
    {
      OnCancelSync(FILE_SYNC_METAFILE_DISK_FULL);
      return NEED_UI_FOR_SYNC_ERROR;
    }

#ifdef USING_SYNC_ONE_TRANSACTION
    mStorage->BeginTransaction();
#endif // USING_SYNC_ONE_TRANSACTION

    if (!StoreMetafile(s, META_MASK_SYNCRESULT/*META_MASK_ROOT_VALID_MASK*/, 0))
    {
      StringCchCopy(msg, 300, _T("Start Sync Folder fail : error code="));
      TCHAR errmsg[30];
      StringCchPrintf(errmsg, 30, _T("%d"), s->GetErrorCode());
      StringCchCat(msg, 300, errmsg);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

      int sync_err;
      if (s->GetErrorCode() == KSyncRootStorage::ERR_DISK_FULL)
        sync_err = FILE_SYNC_METAFILE_DISK_FULL;
      else
        sync_err = FILE_SYNC_CANNOT_MAKE_METAFILE;

      OnCancelSync(sync_err);
#ifdef USING_SYNC_ONE_TRANSACTION
      mStorage->EndTransaction(TRUE);
#endif // USING_SYNC_ONE_TRANSACTION

      return NEED_UI_FOR_SYNC_ERROR;
    }
    KSyncRootStorage* storage = GetStorage();

#ifdef USE_BACKUP_JOB
    // 전체백업, 복원 작업) 시작 상태 저장

    /*
    int method;

    LPCSTR key = NULL;
    if (mSyncMethod & SYNC_METHOD_BACKUP)
    {
      key = ((mSyncMethod & SYNC_METHOD_AUTO) ? KEYS_FULL_BACKUP : KEYS_INSTANT_BACKUP);
      method = (mSyncMethod & SYNC_METHOD_AUTO) ? SYNC_H_START_FULL_BACKUP : SYNC_H_START_INSTANT_BACKUP;
    }
    else if (mSyncMethod & SYNC_METHOD_RESTORE)
    {
      key = KEYS_RESTORE;
      method = SYNC_H_START_RESTORE;
    }

    storage->BeginTransaction();
    storage->StoreSettingValue(KEYB_SYNC_STATE, key);
    storage->StoreSettingValueStr(KEYB_SYNC_ONLY_PATH, mSyncOnlyPath);
    storage->StoreSettingValueStr(KEYB_SYNC_EXTRA_INFO, mSyncExtraInfo);
    storage->EndTransaction(TRUE);
    */

    // 실시간백업-(전체백업/복원) 분리하여 저장한다.
    int bufflen = 30 + lstrlen(mSyncOnlyPath) + lstrlen(mSyncExtraInfo);
    LPTSTR value = new TCHAR[bufflen];
    int sync_key;

    if (mSyncMethod & SYNC_METHOD_BACKUP)
    {
      sync_key = (mSyncMethod & SYNC_METHOD_AUTO) ? KEYB_SYNC_FULL_STATE : KEYB_SYNC_INSTANT_STATE;
      StringCchCopy(value, bufflen, ((mSyncMethod & SYNC_METHOD_AUTO) ? _T("F") : _T("I")));
    }
    else if (mSyncMethod & SYNC_METHOD_RESTORE)
    {
      sync_key = KEYB_SYNC_FULL_STATE;
      StringCchCopy(value, bufflen, _T("R"));
    }

    StringCchCat(value, bufflen, _T("|")); //DA_VALUE_SEPARATOR
    if ((mSyncOnlyPath != NULL) && (lstrlen(mSyncOnlyPath) > 0))
      StringCchCat(value, bufflen, mSyncOnlyPath);
    StringCchCat(value, bufflen, _T("|")); //DA_VALUE_SEPARATOR
    if ((mSyncExtraInfo != NULL) && (lstrlen(mSyncExtraInfo) > 0))
      StringCchCat(value, bufflen, mSyncExtraInfo);

    storage->StoreSettingValueStr(sync_key, value);
    delete[] value;
#endif

    StringCchCopy(msg, 300, _T("Start Sync Folder : "));
    StringCchCat(msg, 300, GetDisplayPathName(mpRelativePath));
    StringCchCat(msg, 300, _T(" : "));
    StringCchCat(msg, 300, mServerOID);
    if (lstrlen(mSyncOnlyPath ) > 0)
    {
      StringCchCat(msg, 300, _T(" , "));
      StringCchCat(msg, 300, mSyncOnlyPath);

      // check if renamed
      if (refreshPartialSyncRenamed(storage))
      {
        StringCchCat(msg, 300, _T("["));
        StringCchCat(msg, 300, mSyncOnlyPath);
        StringCchCat(msg, 300, _T("]"));
      }
   }
    StringCchCat(msg, 300, _T(" : "));
    StringCchCat(msg, 300, getSyncTypeName(mSyncMethod));

    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

    // history-by-sqlite3
    LPCTSTR name = GetRootPath();
    LPCTSTR pServerName = NULL;
    if (mServer.pFullPath != NULL)
    {
      pServerName = StrRChr(mServer.pFullPath, NULL, '>');
      if (pServerName != NULL)
        pServerName++;
      else
        pServerName = mServer.pFullPath;
    }

    int method = 0;
    if (mSyncMethod & SYNC_METHOD_BACKUP)
      method = (mSyncMethod & SYNC_METHOD_AUTO) ? SYNC_H_START_FULL_BACKUP : SYNC_H_START_INSTANT_BACKUP;
    else if (mSyncMethod & SYNC_METHOD_RESTORE)
      method = SYNC_H_START_RESTORE;

    StoreHistory(method, name, 2, NULL, pServerName);
    SetItemSynchronizing(GetRootPath(), 1, 0);
  }
  else
  {
    SetItemSynchronizing(GetRootPath(), 1, WITH_STORE_METAFILE);
  }

  // 복원준비에서 예외 시킨것을 여기서 클리어.
  if ((mSyncMethod & SYNC_METHOD_RESTORE) &&
    (mSyncOnlyPath != NULL) && (lstrlen(mSyncOnlyPath) > 0))
  {
    TCHAR msg[300];
    StringCchPrintf(msg, 300, _T("Backup excluded for %s by Restore"), mSyncOnlyPath);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    gpCoreEngine->SetSyncExcludeFolder(ROOT_ITEM_KEY_STR, mSyncOnlyPath, 3, FALSE);
  }

  // root object initial state
  mLocalState = ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST;
  mServerState = ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST;
#endif
  return NEED_CONTINUE;
}

int KSyncRootFolderItem::ContinueSync()
{
  if (mConflictResult & FILE_SYNC_ERR)
    return NEED_UI_FOR_SYNC_ERROR;

  LPCTSTR rootFolder = GetRootPath();
  if (mPhase <= PHASE_SCAN_OBJECTS) // scanning sub ifo objects
  {
#ifdef PURE_INSTANT_BACKUP
    if (mChildItemIndex == 0) // mSyncOnlyPath 까지의 폴더들 로드
    {
      if ((mSyncOnlyPath != NULL) && (lstrlen(mSyncOnlyPath) > 0))
      {
        LPTSTR path = AllocString(mSyncOnlyPath);
        LPTSTR p = path;

        while(lstrlen(path) > 0)
        {
          LPTSTR e = StrChr(p, '\\');
          if (e != NULL)
          {
            *e = '\0';
            mStorage->LoadChildMetaData(this, path, 0, 0);
          }
          else
          {
            break;
          }
          p = e + 1;
          *e = '\\';
        }
        delete[] path;

        // 여기서 로드된 항목은 스캔 과정 불필요 -> No
        // SetPhaseRecurse(PHASE_COMPARE_FILES-1);
        SetPhase(PHASE_SCAN_OBJECTS);
      }
    }

    int r = mStorage->LoadChildMetaData(this, mSyncOnlyPath, 
      KSyncRootStorage::WITH_ALLSUB|KSyncRootStorage::WITH_PAGING, mChildItemIndex);
#elif defined(USE_SYNC_STORAGE)
    int r = mStorage->LoadChildMetaData(this, NULL, 
      KSyncRootStorage::WITH_ALLSUB|KSyncRootStorage::WITH_PAGING, mChildItemIndex);
#endif

    ++mChildItemIndex;
    mSyncFolderCount = GetTotalCount(1);
    mSyncFileCount = GetTotalCount(0);

    if (r == KSyncRootStorage::R_CONTINUE)
      return NEED_CONTINUE;
    /*
    KSyncFolderItem *folderItem = NULL;
    int sync_need = 0; // 1 : sync all, 2 : sync child folder, 0 : no sync

    while (folderItem = (KSyncFolderItem *)GetNextChild(PHASE_SCAN_OBJECTS, 1))
    {
      sync_need = isSyncNeedFolder(folderItem);
      if (sync_need) // check partial-sync
      {
        // check if excluded folder
        sync_need = !IsExcludeFileMask(folderItem->mLocal.pFilename);
        if (sync_need) // check exclude mask
          break;
      }

      folderItem->ClearSubFiles();
      folderItem->SetPhase(PHASE_SCAN_OBJECTS);
      if (folderItem->mParent != NULL)
        folderItem->mParent->AddSyncFlag(SYNC_ITEM_HAS_EXCLUDED);
    };

    if (folderItem != NULL)
    {
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("ScanSubObjects=%s"), folderItem->GetBaseFolder());
      if (lstrlen(mSyncOnlyPath) > 0)
      {
        TCHAR str[60];
        StringCchPrintf(str, 60, _T(", sync_need=%d"), sync_need);
        StringCchCat(msg, 300, str);
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      BOOL done = FALSE;
      // load last state
      int flag = (mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT) ? 0 : SCAN_CLEAR_STATE;
      if (mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT)
        flag |= SCAN_CHANGED_FILE_ONLY;

      folderItem->scanSubObjects(&mPolicy, mBaseFolder, done, flag, this);
      if (done)
        folderItem->SetPhase(PHASE_SCAN_OBJECTS);

      mSyncFolderCount = GetTotalCount(1);
      mSyncFileCount = GetTotalCount(0);
      return NEED_CONTINUE;
    }
    */

    // 백업인때에는 대상 드라이브가 아닌것 지운다
    if (mSyncMethod & SYNC_METHOD_BACKUP)
    {
      RemoveUnBackupedItems(this, REMOVE_WITH_STORAGE);
    }

    mTargetFolderCount = mSyncFolderCount;
    mTargetFileCount = mSyncFileCount;

    mSyncFolderCount = 0;

    // scan object and go conflict-dialog
    if (mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT)
    {
      RemoveUnconflictItem();

      // remove excluded item
      RemoveExcludedItems(this, TRUE);

      UpdateFolderSyncResult(0, this, STORE_META);

#ifdef REFRESH_ITEMS_ON_RESOLVE_CONFLICT // if need update new state, goto PHASE_LOAD_FILES
      SetPhase(PHASE_LOAD_FILES-1);
      return NEED_CONTINUE;
#else
      return NEED_UI_FOR_CONFLICT;
#endif
    }

#ifdef _DEBUG
    TCHAR msg[120];
    StringCchPrintf(msg, 120, _T("Load prev metadata : %d"), mSyncFileCount);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
    StoreEllapsedTimeLog(_T("Load Metadatas"));

    mSyncFileCount = 0;
    SetPhase(PHASE_SCAN_OBJECTS+1);


#if 0 // 2017-09 : 안쪽만 스캔하면 바깥쪽으로 이동된 항목의 체크가 불가함.
    // 부분 동기화 바깥은 모두 무시하도록 한다
#ifndef PURE_INSTANT_BACKUP
    // 지금은 부분동기화 안쪽만 로드하니 불필요함
    if (!CheckScanFolderServerPartialSync(PHASE_SCAN_FOLDERS_SERVER))
    {
      // need scan from root folder
      SetPhase(PHASE_SCAN_FOLDERS_SERVER-1);
    }
#endif
#endif // 0
    return NEED_CONTINUE;
  }

  if (mPhase <= PHASE_SCAN_FOLDERS_SERVER) // scanning sub folders
  {
#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
    int r = processScanFolderData();
    if (r == R_FAIL_NEED_FULL_SYNC_PARTIAL)
      return NEED_UI_FOR_FULLSYNC;
    else if (r == R_MORE_CALL)
      return NEED_CONTINUE;
    else if (r > R_SUCCESS)
    {
      SetConflictResult(FILE_SYNC_SFS_ERROR);
      return NEED_UI_FOR_SYNC_ERROR;
    }
    // r == R_NOT_READY, R_SUCCESS
#endif

    KSyncFolderItem *folderItem = (KSyncFolderItem *)GetNextChild(PHASE_SCAN_FOLDERS_SERVER, 1);
    if (folderItem != NULL)
    {
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("ScanSubFolders=%s"), folderItem->GetBaseFolderName());

      // update root folder's fullPathIndex
      /* root folder is already updated
      if (folderItem->mParent == NULL)
        folderItem->RefreshServerNewState();
      */

      int need_folder = IsSyncNeedFolder(folderItem, PHASE_SCAN_FOLDERS_SERVER);
      if (need_folder > 0) // == 1) root=2, folder=1, 왜 루트는 스킵할까??
      {
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        int r = folderItem->scanSubFoldersServer(&mPolicy, this);
        if (r != R_SUCCESS)
          return r;

        // remove items which matched with ExcludeNameMask
        folderItem->RemoveExcludeNamedItems(gpCoreEngine, this, PHASE_SCAN_FOLDERS_SERVER);
      }
      else
      {
        StringCchCat(msg, 300, _T(" : skip"));
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }

      folderItem->SetPhase(PHASE_SCAN_FOLDERS_SERVER);
      return NEED_CONTINUE;
    }
    else
    {
      if (lstrlen(mSyncOnlyPath) > 0)
      {
        // 밖에서 이 폴더 안쪽으로 들어온 항목도 검사해야 한다.
        if (GetCountForMoveInOutOnServer(mSyncOnlyPath) > 0)
        {
          return NEED_UI_FOR_FULLSYNC;
        }
      }

      // some folders does not deploy folder-movement such as 1/2/3 -> 1/3.
      // because when processing 1/3, 1/2/3 does not created yet.
      // so we have to process again hear.(find 1/3 and 1/2/3, merge 1/3 to 1/2/3, and set MovedAs)
      //
      // all folders are found. process folder movement.
      // deploy rename information from "renamed.txt", and set to each item
      // DeployFolderMovement();

      // skip PHASE_SCAN_SERVER_FILES if item does not exist on server
      SetPhaseServerNotExistFolders(PHASE_SCAN_SERVER_FILES);

      // 백업인때에는 대상 드라이브가 아닌것 지운다
      if (mSyncMethod & SYNC_METHOD_BACKUP)
      {
        RemoveUnBackupedItems(this, REMOVE_WITH_STORAGE);
      }

      TCHAR str[120];
      StringCchPrintf(str, 120, _T("Scan Server found folders=%d, files=%d"), mSyncFolderCount, mSyncFileCount);
      StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_LOG);
    }
    mSyncTotalCount = 0;
    mSyncFolderCount = 0;
    mSyncFileCount = 0;
    SetPhase(PHASE_SCAN_FOLDERS_SERVER+1);

    // 실시간 백업에서는 로컬 스캔을 스킵한다.
    if ((mSyncMethod & (SYNC_METHOD_BACKUP|SYNC_METHOD_INSTANT)) ==
      (SYNC_METHOD_BACKUP|SYNC_METHOD_INSTANT))
    {
      SetPhaseRecurse(PHASE_SCAN_LOCAL_FILES+1, 0);
      SetPhase(PHASE_SCAN_FOLDERS_SERVER+1);
    }

    StoreEllapsedTimeLog(_T("Scan Server Folders"));
  }

#ifndef USE_SCAN_BY_HTTP_REQUEST
  // server file is scanned on PHASE_SCAN_FOLDERS_SERVER 
  if (mPhase <= PHASE_SCAN_SERVER_FILES) // scanning sub files on server
  {
    KSyncFolderItem *folderItem = (KSyncFolderItem *)GetNextChild(PHASE_SCAN_SERVER_FILES, 1);
    if (folderItem != NULL)
    {
      BOOL done = TRUE;
      int count = 0;
      if (IsSyncNeedFolder(folderItem)) // check partial-sync, child and child-of-child
      {
        TCHAR msg[300];
        StringCchPrintf(msg, 300, _T("ScanSubFilesServer=%s"), folderItem->GetBaseFolderName());
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        count = folderItem->scanSubFilesServer(&mPolicy, this, done);
      }
      else
      {
        folderItem->ClearSubFiles();
      }

      if (done)
      {
        folderItem->SetPhase(PHASE_SCAN_SERVER_FILES);
        ++mSyncTotalCount;
      }

      mSyncFileCount += count;
      return NEED_CONTINUE;
    }

#ifdef _DEBUG
    TCHAR msg[120];
    StringCchPrintf(msg, 120, _T("Load server files : %d"), GetTotalCount(0));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

    // if server_location is changed, clear server-side information
    // to sync new server location.
    if (mPolicy.GetSyncFlag() & SYNC_SERVER_LOCATION_CHANGED)
      ClearServerSideInfoCascade();

    mSyncFileCount = 0;
    mSyncTotalCount = 0;
    mSyncTotalSize = 0;
    mSyncTotalSizeProgress = 0;
    SetPhase(PHASE_SCAN_SERVER_FILES+1);
    StoreEllapsedTimeLog(_T("Scan Server Files"));
  }
#endif

#ifndef SCAN_FOLDER_FILE_AT_ONCE
  if (mPhase <= PHASE_SCAN_FOLDERS_LOCAL) // scanning sub folders on local
  {
    KSyncFolderItem *folderItem = (KSyncFolderItem *)GetNextChild(PHASE_SCAN_FOLDERS_LOCAL, 1);
    if (folderItem != NULL)
    {
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("ScanSubFolders local =%s"), folderItem->GetBaseFolderName());

      // update root folder's fullPathIndex
      /* root folder is already updated
      if (folderItem->mParent == NULL)
        folderItem->RefreshServerNewState();
      */

      if (IsSyncNeedFolder(folderItem))
      {
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        int count = folderItem->scanSubFoldersLocal(&mPolicy, this);
        mSyncFolderCount += count;

        // remove items which matched with ExcludeNameMask
        folderItem->RemoveExcludeNamedItems(gpCoreEngine, &mPolicy, PHASE_SCAN_FOLDERS_LOCAL);
      }
      else
      {
        StringCchCat(msg, 300, _T(" : skip"));
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }

      folderItem->SetPhase(PHASE_SCAN_FOLDERS_LOCAL);
      return NEED_CONTINUE;
    }
    else
    {
      TCHAR str[120];
      StringCchPrintf(str, 120, _T("Scan Local found folders=%d, files=%d"), mSyncFolderCount, mSyncFileCount);
      StoreLogHistory(_T(__FUNCTION__), str, SYNC_MSG_LOG);

      // some folders does not deploy folder-movement such as 1/2/3 -> 1/3.
      // because when processing 1/3, 1/2/3 does not created yet.
      // so we have to process again hear.(find 1/3 and 1/2/3, merge 1/3 to 1/2/3, and set MovedAs)
      //
      // all folders are found. process folder movement.
      // deploy rename information from "renamed.txt", and set to each item
      DeployFolderMovement();

      mSyncTotalCount = GetTotalCount(0);
    }
    mSyncTotalCount = 0;
    SetPhase(PHASE_SCAN_FOLDERS_LOCAL+1);
    StoreEllapsedTimeLog(_T("Scan Local Folders"));
  }
#endif // SCAN_FOLDER_FILE_AT_ONCE

  if (mPhase <= PHASE_SCAN_LOCAL_FILES) // scanning sub files
  {
    int count = 0;
    KSyncFolderItem *folderItem;

    do
    {
      folderItem = (KSyncFolderItem *)GetNextChild(PHASE_SCAN_LOCAL_FILES, 1);
      if (folderItem != NULL)
      {
        BOOL done = TRUE;
        int folder_count = 0;
        int file_count = 0;

        LPTSTR resultFilename = folderItem->AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
        int fullPathLength = lstrlen(resultFilename);
        delete[] resultFilename;
        if (fullPathLength >= MAX_FOLDER_PATH_LENGTH)
        {
          TCHAR msg[300];
          StringCchPrintf(msg, 300, _T("LongFolderName Scan Skip : %s"), folderItem->GetFilename());
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          folderItem->SetPhase(PHASE_ENDING);
          folderItem->SetResult(FILE_SYNC_OVER_PATH_LENGTH);
          folderItem->SetConflictResult(FILE_SYNC_OVER_PATH_LENGTH);
          folderItem->FreeChilds();
          return NEED_CONTINUE;
        }

        int need_folder = IsSyncNeedFolder(folderItem, PHASE_SCAN_LOCAL_FILES);
        if (need_folder == 1) // partial-sync, child and child-of-child
        {
          folderItem->RefreshLocalNewState(this);
          if (CheckNeedScanLocalFolder(folderItem))
          {
            if (folderItem->mLocalState & ITEM_STATE_NOW_EXIST)
              file_count = folderItem->scanSubFilesLocal(this, folder_count, done);
          }
          else
          {
            need_folder = 0;
          }
        }
        else if (need_folder == 2) // parent of partial sync
        {
          folderItem->RefreshLocalNewState(this);
        }
        else if (need_folder == 0)
        {
          TCHAR msg[300];
          StringCchPrintf(msg, 300, _T("ScanSubFilesLocal=%s : skip"), folderItem->GetBaseFolderName());
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          folderItem->FreeChilds();
        }

        if ((folder_count + file_count) > 0)
        {
          TCHAR msg[300];
          StringCchPrintf(msg, 300, _T("ScanSubFilesLocal=%s, folders=%d, files=%d"), folderItem->GetBaseFolderName(), folder_count, file_count);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          mSyncFileCount += file_count;
          mSyncFolderCount += folder_count;
        }

        if (done)
        {
          // remove items which matched with ExcludeNameMask
          folderItem->RemoveExcludeNamedItems(gpCoreEngine, this, PHASE_SCAN_LOCAL_FILES);

          folderItem->SetPhase(PHASE_SCAN_LOCAL_FILES);
          mSyncTotalCount++;
          count += file_count + folder_count;
        }
        else
        {
          break;
        }
      }
      else
      {
        break;
      }
    } while (count < NOF_LOCAL_FILES_PER_PAGE);
      
    if (folderItem != NULL)
    {
      return NEED_CONTINUE;
    }
    else
    {
      // mSyncOnlyPath 상위 폴더들의 상태를 업데이트한다
#ifdef PURE_INSTANT_BACKUP
      if ((mSyncMethod & SYNC_HIGH_PRIORITY) &&
        !(mFlag & SYNC_FLAG_MODIFY_SCANNED))
      {
        if (mSyncOnlyPath != NULL)
          UpdateParentOfSyncOnlyPath();

        AddSyncFlag(SYNC_FLAG_MODIFY_SCANNED);

        // ModifiedList에서 추가된 폴더들 로컬 스캔을 다시 해야 함. => 폴더 스캔 안함.
        // 폴더 경로명 달라지니 폴더 스캔 해야 한다.
        if (ReadyModifiedItemList() > 0)
          return NEED_CONTINUE;
      }

      if (mSyncMethod & SYNC_HIGH_PRIORITY)
      {
        // ModifiedList에 포함되지 않는 개체들은 모두 아웃.
        RemoveOutOfModifiedList(mModifiedItemList);
      }

#endif

#ifdef SCAN_FOLDER_FILE_AT_ONCE
      DeployFolderMovement();
#endif

#ifdef USE_BACKUP
      if (mSyncMethod & SYNC_METHOD_BACKUP)
      {
        // 이동 타겟이 되는 폴더는 살려두도록
        CheckMoveTargetFolders(this, mSyncMethod);
        // 대상 파일이 없는 폴더 스킵
        RemoveUnreferencedFolders(this, mSyncMethod);
      }

      mSyncFolderCount = GetTotalCount(1);

#ifdef _DEBUG
      if (mSyncMethod & SYNC_HIGH_PRIORITY)
        DumpSyncItems(0);
#endif
#endif

      TCHAR msg[120];
      StringCchPrintf(msg, 120, _T("Load local folders=%d, files=%d"), mSyncFolderCount, mSyncFileCount);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    // if server_location is changed, clear server-side information
    // to sync new server location.
    if (mPolicy.GetSyncFlag() & SYNC_SERVER_LOCATION_CHANGED)
      ClearServerSideInfoCascade();

    mSyncTotalCount = GetTotalCount(0);
    mSyncFileCount = 0;
    mSyncFolderCount = 0;
    mSyncTotalSize = 0;
    mSyncTotalSizeProgress = 0;
    SetPhase(PHASE_SCAN_LOCAL_FILES+1);
    StoreEllapsedTimeLog(_T("Scan Local Files"));
    return NEED_CONTINUE;
  }

  if (mPhase <= PHASE_COMPARE_FILES) // compare sub files
  {
    int count = 0;
    KSyncFolderItem *folderItem;

    do
    {
      folderItem = (KSyncFolderItem *)GetNextChild(PHASE_COMPARE_FILES, 1);
      if (folderItem != NULL)
      {
        BOOL done = TRUE;
        int file_count = 0;

#ifdef SYNC_ALL_DRIVE
      // 전체 시스템 대상인 경우에는 부분 동기화 폴더의 상위도 검사를 하여야 한다.
      if (IsSyncNeedFolder(folderItem, PHASE_COMPARE_FILES) >= 1) // check partial-sync
#else
      if (IsSyncNeedFolder(folderItem, PHASE_COMPARE_FILES) == 1) // check partial-sync
#endif
        {
          done = folderItem->compareFiles(/*rootFolder, &mPolicy*/ this, file_count, PHASE_COMPARE_FILES);
          mSyncFileCount += file_count;
        }
        else
        {
          TCHAR msg[300] = _T("Skip compare folder : ");
          LPCTSTR baseFolder = folderItem->GetBaseFolderName();
          StringCchCat(msg, 300, baseFolder);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        count += file_count + 1;

        // again if not done
        if (done)
          folderItem->SetPhase(PHASE_COMPARE_FILES);
        else
          break;
      }
      else
      {
        break;
      }
    } while(count < NUMBER_OF_ITEMS_COMPARE);

    mSyncTotalSize = GetTotalSize(FALSE);

    if (folderItem != NULL)
    {
      return NEED_CONTINUE;
    }
    else
    {
      mTargetFolderCount = GetTotalCount(1);
      mTargetFileCount = GetTotalCount(0);

      removeEndOfJobObject();

#ifdef USE_BACKUP
      // 대상 파일이 없는 폴더 스킵
      RemoveUnreferencedFolders(this, mSyncMethod);
      mSyncFolderCount = GetTotalCount(1);
#endif

      // apply rename information
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("Compare Done %d items"), GetTotalCount(0));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    // RemoveUnchanged(PHASE_CLEAN_UNCHANGED);
    // do not remove items. it will used when folder moved
    SetUnchangedItemPhase(PHASE_CREATE_FOLDERS-1, PHASE_MOVE_FOLDERS-1, PHASE_ENDING, FALSE);

    mSyncFileCount = 0;
    mSyncFolderCount = 0;

    // number of files to do loading
    mSyncTotalCount = 0;
    mSyncTotalSize = 0;
    SetPhase(PHASE_LOAD_FILES); // skip root item
    GetCandidateCount(PHASE_ENDING/*PHASE_LOAD_FILES*/, FALSE, mSyncTotalCount, mSyncTotalSize, mSyncTotalSizeProgress);

    // sync-start time update when start load files
    ::GetLocalTime(&mSyncStartTime);
    StoreEllapsedTimeLog(_T("Compare"));

#ifdef _DEBUG_CHANGE_ON_SYNC
    Sleep(1000);
#endif
    return NEED_CONTINUE;
  }

  // newly created folder
  if (mPhase <= PHASE_CREATE_FOLDERS)
  {
    KSyncItem *item = (KSyncItem *)GetNextChild(PHASE_CREATE_FOLDERS, 1, TRUE); // parent folder first
    if (item != NULL)
    {
      item->ReadyResolve(this, mPhase, mSyncMethod);
      return syncFileItem(item, PHASE_CREATE_FOLDERS);
    }
    else
    {
      SetPhase(PHASE_CREATE_FOLDERS+1); // skip root item
    }
  }

  // folder file movement
  if (mPhase <= PHASE_MOVE_FOLDERS)
  {
    // child object should moved firstly
    KSyncItem *item = (KSyncItem *)GetNextChild(PHASE_MOVE_FOLDERS, 2, TRUE); // parent folder first(160615)
    if (item != NULL)
    {
      item->ReadyResolve(this, mPhase, mSyncMethod);
      return syncFileItem(item, PHASE_MOVE_FOLDERS);
    }
    else
    {
      // now we can remove excluded folders
      RemoveExcludedItems(this, TRUE);

      // we should rename which is temporary-renamed
      if (RestoreTemporaryRenamed() > 0)
        return NEED_CONTINUE;
      SetPhase(PHASE_MOVE_FOLDERS+1); // skip root item
    }
  }

#ifdef USE_BACKUP_DELETE_EACH_OBJECT
  if (mPhase <= PHASE_DELETE_FILES) // sync each files
  {
    KSyncItem *item = (KSyncItem *)GetNextChild(PHASE_DELETE_FILES, 2); // child item first
    if (item != NULL)
    {
      // we have to update local-server info, and syncFileItem()
      item->ReadyResolve(this, mPhase, mSyncMethod);
      return syncFileItem(item, PHASE_DELETE_FILES);
    }
    else
    {
      SetPhase(PHASE_DELETE_FILES+1);
    }
  }
#endif

#ifndef SYNC_ALL_DRIVE
  if (!gpCoreEngine->IsSecurityDisk(rootFolder))
  {
    if (!CheckDiskFreeSpace(rootFolder, 100))
    {
      TCHAR msg[400];
      StringCchPrintf(msg, 400, _T("Disk full for %s"), rootFolder);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      OnCancelSync(FILE_SYNC_NO_CDISK_SPACE);
      return NEED_UI_FOR_SYNC_ERROR;
    }
  }
#endif

  if (mPhase <= PHASE_COPY_FILES) // sync each files
  {
    // sync all folders firstly
    KSyncItem *item = (KSyncItem *)GetNextChild(PHASE_COPY_FILES, 1); // child folder first
    if (item == NULL)
      item = (KSyncItem *)GetNextChild(PHASE_COPY_FILES, 2);

    if (item != NULL)
    {
      // we have to update local-server info, and syncFileItem()
      item->ReadyResolve(this, mPhase, mSyncMethod);
      return syncFileItem(item, PHASE_COPY_FILES);
    }
    else
    {
      UpdateFolderSyncResult(0, this, STORE_META);

      if (mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT)
        return NEED_UI_FOR_CONFLICT;

      // always update for support move/rename
      if (TRUE) // mPolicy.GetSyncFlag() & SYNC_SERVER_LOCATION_CHANGED)
      {
        // synchronization complete, update server information

#ifdef USE_SYNC_ENGINE
        LPTSTR server_path=NULL;
        LPTSTR fullPathIndex=NULL;

        if (gpCoreEngine->GetServerFolderSyncInfo(this, mServerOID, NULL, NULL, &server_path, &fullPathIndex, NULL))
          StoreRootFolderServerPath(mServerOID, server_path, fullPathIndex, FALSE);

        if (server_path != NULL)
          delete[] server_path;
        if (fullPathIndex != NULL)
          delete[] fullPathIndex;
#endif
      }

      // check conflict items
      TCHAR msg[128];
      int conflictCount = GetConflictCount(FALSE);
      StringCchPrintf(msg, 128, _T("End sync, method=%d, conflict-count=%d"), mSyncMethod, conflictCount);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      StoreEllapsedTimeLog(_T("Total Sync time"));

#ifndef USE_BACKUP // 백업에서는 PHASE_RETIRE에서 충돌창 처리한다
      if (mSyncMethod & SYNC_METHOD_MANUAL)
      {
        if ((mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR)) || 
          (mChildResult & (FILE_CONFLICT | FILE_SYNC_ERR)) ||
          (GetConflictCount(TRUE) > 0)) // conflict item exist in manual sync
          return NEED_UI_FOR_CONFLICT;
      }
#endif

      SetPhase(PHASE_RETIRE);
      return NEED_CONTINUE;
    }
  }

  if (mPhase <= PHASE_CONFLICT_FILES)
  {
    KSyncItem *item = (KSyncItem *)GetNextChild(PHASE_CONFLICT_FILES, 2);
    if (item != NULL)
    {
      // we have to update local-server info, and compareSyncFileItem()
      int r = item->ReadyResolve(this, mPhase, mSyncMethod);
      if (r == R_FAIL_NEED_FULL_SYNC_PARTIAL)
      {
        return NEED_UI_FOR_FULLSYNC;
      }

      return syncFileItem(item, PHASE_CONFLICT_FILES);
    }
    else
    {
      // we should rename which is temporary-renamed
      if (RestoreTemporaryRenamed() > 0)
        return NEED_CONTINUE;
    }
    UpdateFolderSyncResult(0, this, STORE_META);

    if ((mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR)) || 
      (mChildResult & (FILE_CONFLICT | FILE_SYNC_ERR)) ||
      GetConflictCount(TRUE) > 0) // conflict item exist
    {
      // set all item state as PHASE_LOAD_FILES
      // SetPhaseForceRecurse(PHASE_LOAD_FILES - 1);

      // 빨리 끝났으면 충돌처리창 Animation이 한두번 돌 정도 딜레이 준다.
      if (mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT)
      {
        SYSTEMTIME tm;
        ::GetLocalTime(&tm);

        long sec = TimeDifferenceSec(tm, mSyncBeginTime);
        if (sec < 3)
        {
          StoreLogHistory(_T(__FUNCTION__), _T("Wait"), SYNC_MSG_LOG);
          return NEED_CONTINUE;
        }
      }

      return NEED_UI_FOR_CONFLICT;
    }

    SetPhase(PHASE_RETIRE);
    return NEED_CONTINUE;
  }

#ifdef USE_BACKUP
  if (mPhase <= PHASE_RETIRE)
  {
    // if (mDeleteOnServerStack.size() > 0)
    if (mChildItemIndex == 0)
    {
      if ((mDOS_FolderList != NULL) || (mDOS_FileList != NULL))
      {
        mChildItemIndex = 1;
        RequestDeleteOnServerItemList();
        return NEED_CONTINUE;
      }
      else
      {
        mChildItemIndex = 2;
      }
    }

    // '삭제 요청 결과를 모두 처리하였음' 될때까지
    if (mChildItemIndex < 2)
      return NEED_CONTINUE;

    SetPhase(PHASE_RETIRE+1);

#ifdef _DEBUG_INSTANT_CONFLICT
    if (mSyncMethod & (SYNC_METHOD_BACKUP_RESTORE)) // backup & restore for debug
#else
    if (mSyncMethod & SYNC_METHOD_RESTORE) // full backup & restore only
#endif
    {
      if ((mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR)) || 
        (mChildResult & (FILE_CONFLICT | FILE_SYNC_ERR)) ||
        (GetConflictCount(TRUE) > 0)) // conflict item exist
      {
        // set all item state as PHASE_LOAD_FILES
        // SetPhaseForceRecurse(PHASE_LOAD_FILES - 1);
        return NEED_UI_FOR_CONFLICT;
      }
    }
  }

  // 임시 이름 변경 다시 원위치
  if (RestoreTemporaryRenamed() > 0)
    return NEED_CONTINUE;

  if (mPhase <= (PHASE_RETIRE+1))
  {
    if (mSyncMethod & SYNC_METHOD_RESTORE)
    {
      if (lstrlen(mRestoreFolderOID) > 0)
      {
        gpCoreEngine->RequestClearRestoreInfo(mRestoreFolderOID, RESTORE_STATUS_S);
        mRestoreFolderOID[0] = '\0';
        return NEED_CONTINUE;
      }
    }
  }
#else
  // 임시 이름 변경 다시 원위치
  if (RestoreTemporaryRenamed() > 0)
    return NEED_CONTINUE;
#endif

  if (mPhase <= PHASE_ENDING) // ending
  {
    // set child item's phase as PHASE_CONFLICT_FILES which is not changed.
    SetUnchangedItemPhase(PHASE_CREATE_FOLDERS-1, PHASE_MOVE_FOLDERS-1, PHASE_CONFLICT_FILES, FALSE);

    // update SyncResultStatus for all child folders, remove disabled items.
    UpdateFolderSyncResult(0, this, REMOVE_DISABLED|STORE_META);

#ifdef USE_BACKUP
    if (!(mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT))
    {
      if (mChildItemIndex == 0)
      {
        SetPhase(PHASE_ENDING);
        mChildItemIndex = (StoreBackupRestoreResult(0) == NEED_CONTINUE) ? 1 : 2;
      }
    }
    else
    {
      mChildItemIndex = 2;
    }

    if (mChildItemIndex >= 2)
      SetPhase(PHASE_ENDING + 1); // next phase
    else
      return NEED_CONTINUE;
#endif
  }

  if (mPhase <= PHASE_ENDING2) // ending
    return NOTHING_TODO;

  if(TRUE)
  {
    TCHAR msg[300];
    StringCchPrintf(msg, 64, _T("unknown phase %d"), mPhase);
    OutputDebugString(msg);
  }
  return NOTHING_TODO;
}

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)

int KSyncRootFolderItem::OnEndScanFolderItem(KSyncFolderItem* p, int result, KDownloaderError sfs_err, LPSTR data, long length, int flag, LPTSTR errorMessage)
{
  if ((data == NULL) || (result != R_SUCCESS))
  {
    TCHAR msg[256] = _T("Failure on getModifiedFolderNDocument");
    if (errorMessage != NULL)
    {
      StringCchCat(msg, 256, _T(" : "));
      StringCchCat(msg, 256, errorMessage);
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_EVENT_ERROR);

    OnCompleteServerItemScan(result);
    SetServerFolderScanResult(sfs_err);

    if (errorMessage != NULL)
      SetServerConflictMessage(errorMessage);
    return 0;
  }

  // store it to buffer
  m_ScanFolderData = new char[length+1];
  memcpy(m_ScanFolderData, data, length);
  m_ScanFolderData[length] = '\0';
  m_ScanFolderDataLength = length;
  m_ScanFolderIndex = 0;
  m_ScanFolderItem = p;
  m_ScanFolderFlag = flag;

  return R_SUCCESS;
}

#include "../HttpDownloader/KString.h"
#include "../HttpDownloader/KWinSockDownloader.h"

#define _DEBUG_SCAN_SERVER_FOLDERS
#define _DEBUG_SCAN_SERVER_FOLDERS_ITEM

int KSyncRootFolderItem::processScanFolderData()
{
  if ((m_ScanFolderData != NULL) && (m_ScanFolderDataLength > 0))
  {
    LPSTR data = m_ScanFolderData + m_ScanFolderIndex;

    if (m_ScanFolderIndex == 0)
    {
      // clear progress count
      mSyncFileCount = 0;
      mSyncFolderCount = 0;

      // parent item cache for scan parsing
      m_ScanParseLastParent = NULL;

      LPSTR e = strchr(data, SCAN_ITEM_DELEMETER);
      if ((e == NULL) || ((data + 100) < e))
      {
        StoreLogHistory(_T(__FUNCTION__), _T("cannot found server time break"), SYNC_MSG_LOG);
        if (data != NULL)
        {
          LPTSTR pStr = MakeUnicodeString(data);
          if (pStr != NULL)
          {
            StoreLogHistory(_T(__FUNCTION__), pStr, SYNC_MSG_LOG);
            delete[] pStr;
          }
        }
        OnCompleteServerItemScan(FILE_SYNC_SFS_ERROR);
        return R_FAIL_SCAN_SERVER_FOLDERS;
      }

      KString str;
      str.assign(data, (int) (e - data));
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

      SetPreviousScanTimeStamp(scan_time);

      m_ScanFolderIndex += (int) (e - data);
      data = e;

      if (m_ScanFolderFlag & SCAN_ONLY_CHANGED)
      {
        StoreLogHistory(_T(__FUNCTION__), _T("Reply only changed items"), SYNC_MSG_LOG);

        /*
         변동 없는 항목은 목록에 포함되지 않으므로, 
         스캔 대상과 하위의 항목이 모두 변동 없음을 기본 상태로 함.
         */
        m_ScanFolderItem->SetScanServerFolderState(0, WITH_RECURSE|WITH_ITSELF|WITH_CHILD);
      }
      else
      {
        StoreLogHistory(_T(__FUNCTION__), _T("Reply all items"), SYNC_MSG_LOG);
        /* 
          전체 목록을 조회한 경우에 포함되지 않은 항목은 삭제된 것이므로
          하위의 항목이 모두 삭제됨을 기본 상태로 함.
        */
        // 자신도 삭제되는 경우 있으니 자신도 삭제됨으로 설정함.
        // m_ScanFolderItem->SetScanServerFolderState(ITEM_EXIST, WITH_ITSELF);
        m_ScanFolderItem->SetScanServerFolderState(ITEM_DELETED, WITH_RECURSE|WITH_CHILD|WITH_ITSELF);
      }
    }

    long block_len = 0;
    int count = 0;
    int r = R_SUCCESS;
    LPSTR e;

    while ((m_ScanFolderIndex < m_ScanFolderDataLength) && (count < NUMBER_OF_ITEMS_SCAN_PARSE))
    {
      e = strchr(data, SCAN_ITEM_DELEMETER);
      if (e != NULL)
      {
        block_len = (int)(e - data);
        *e = '\0';
      }
      else
      {
        block_len = m_ScanFolderDataLength - m_ScanFolderIndex;
      }

      // create item from data
      KSyncItem *item = CreateFromStream(m_ScanFolderItem, m_ScanParseLastParent, data, block_len, r);
      if (item != NULL)
      {
        // 다음 항목의 parent도 동일할 확률이 높으니 이를 캐시해서 사용함.
        m_ScanParseLastParent = (KSyncFolderItem*)item->mParent;

        // update progress to status window
        if (item->IsFolder())
          ++mSyncFolderCount;
        else
          ++mSyncFileCount;
        count++;
      }
#if 0 // ignored item can be NULL
      else
      {
        LPTSTR item_str = MakeUnicodeString(data, block_len);
        int len = lstrlen(item_str) + 60;
        LPTSTR buff = new TCHAR[len];
        StringCchCopy(buff, len, _T("CreateFromStream error block : "));
        StringCchCat(buff, len, item_str);
        StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
        delete[] buff;
        delete[] item_str;

        OnCompleteServerItemScan(FILE_SYNC_SFS_ERROR);
        return R_FAIL_SCAN_SERVER_FOLDERS;
      }
#endif
      if (r != R_SUCCESS)
      {
        OnCompleteServerItemScan(FILE_SYNC_SFS_ERROR);
        return R_FAIL_SCAN_SERVER_FOLDERS;
      }

#ifdef _DEBUG_SCAN_POINTER
      TCHAR buff[100];
      StringCchPrintf(buff, 100, _T("data index=%d, total=%d"), m_ScanFolderIndex, m_ScanFolderDataLength);
      StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif

      if (e != NULL)
      {
        m_ScanFolderIndex += block_len + 1;
        data = e + 1;
      }
      else
      {
        m_ScanFolderIndex = m_ScanFolderDataLength;
      }
    }

    // all data is done
    if (m_ScanFolderDataLength == m_ScanFolderIndex)
    {
      // mScanServerState -> mServerState
      m_ScanFolderItem->SetServerStateByScanState();

      /*
      위치가 이동된 폴더는 다시 하위 항목 업데이트 하도록 phase를 바꾼다.
      위치가 이동된 폴더의 하위 폴더의 FullPathIndex도 당연히 변경되었으나,
      getModifiedFolderNDocument에서는 이 정보를 보내주지 않으니 다시 요청함.
      */

      /*
      동기화 제외 폴더로 설정되면 서버측 변경 정보가 무시되니
      다시 동기화 포함한 후에 동기화 시도하면 서버측 정보 받아오지 못함.
      따라서 동기화 포함하게 되면 다음 동기화에서 해당 폴더를 다시 스캔하여야 함.
      */

      // 변경된 내용만 받는 경우 && 새로 추가된 항목은 처리할 필요 없음 && 루트 폴더 조회한 경우에만
      if ((m_ScanFolderFlag & SCAN_ONLY_CHANGED) &&
        ((m_ScanFolderItem->mServerState & ITEM_STATE_EXISTING) == ITEM_STATE_EXISTING) &&
        (m_ScanFolderItem == this))
      {
        m_ScanFolderItem->SetPhaseForMovedFoldersInServer(PHASE_SCAN_FOLDERS_SERVER-1, PHASE_SCAN_FOLDERS_SERVER);
      }
      else
      {
        m_ScanFolderItem->SetPhaseRecurse(PHASE_SCAN_FOLDERS_SERVER, 0);
      }

      m_ScanFolderItem->SetPhase(PHASE_SCAN_FOLDERS_SERVER);
      if (m_ScanFolderItem->IsServerNeedRescan())
      {
        m_ScanFolderItem->SetServerNeedRescan(FALSE);
        m_ScanFolderItem->StoreMetafile(NULL, META_MASK_FLAG, 0);
      }

      delete[] m_ScanFolderData;
      m_ScanFolderData = NULL;
      m_ScanFolderDataLength = 0;
      m_ScanFolderIndex = 0;

      TCHAR buff[100];
      StringCchPrintf(buff, 100, _T("ScanFolderItem found folders=%d, files=%d"), 
        mSyncFolderCount, mSyncFileCount);
      StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);

      // 부분 동기화에서 바깥으로 이동된 항목이 있는지 검사한다 -> 전체 동기화 필요 오류.
      if (lstrlen(mSyncOnlyPath) > 0)
      {
        if (m_ScanFolderItem->GetCountForMoveInOutOnServer(mSyncOnlyPath) > 0)
        {
          return R_FAIL_NEED_FULL_SYNC_PARTIAL;
        }
        // 밖에서 이 폴더 안쪽으로 들어온 항목도 검사해야 한다.(ContinueSync에서)
      }

      m_ScanFolderItem = NULL;
      m_ScanParseLastParent = NULL;
    }
    return ((m_ScanFolderDataLength > m_ScanFolderIndex) ? R_MORE_CALL : R_SUCCESS);
  }
  return R_NOT_READY; // data not ready
}

KSyncItem* KSyncRootFolderItem::CreateFromStream(KSyncFolderItem *pRoot, KSyncFolderItem *lastParent, LPSTR d, long data_len, int& rtn)
{
  KString kstr;
  int str_len;
  char object_type = *d;
  KSyncItem *item = NULL;
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

    if (oid != NULL)
      item = pRoot->findItemByOID(oid);
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

      // scan 목록에 포함됨 플래그 셋
      item->AddSyncFlag(SYNC_FLAG_IT_SCANNED);

      if (childItemModified)
      {
        // 자식(1단계만) 항목이 모두 삭제됨을 기본 상태로 함.
        // 2017-09 : 전체 하위 항목이 삭제됨으로 해야 한다. => 다시 1단계만(WITH_RECURSE)
        item->SetScanServerFolderState(ITEM_DELETED, WITH_CHILD|EXCLUDE_MOVED);
      }

      // 자신은 존재함으로
      item->SetScanServerFolderState(ITEM_EXIST, WITH_ITSELF);
    }
    else
    {
      TCHAR msg[100];
      StringCchPrintf(msg, 100, _T("Error:Unchanged item(OID:%s) not found"), oid);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
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
    kstr.assign(d, (int) (e - d));
    int permissions = kstr.toInt(NULL, 16);

    d = e + 1;
    e = strchr(d, SCAN_VALUE_DELEMETER);
    if (e == NULL)
      goto end_all;
    kstr.assign(d, (int) (e - d));
    __int64 modifiedAt = kstr.toInt64();
    SYSTEMTIME systime;
    SystemTimeFromEPOCH_MS(systime, modifiedAt);

    d = e + 1;
    e = strchr(d, SCAN_VALUE_DELEMETER);
    if (e == NULL)
      goto end_all;
    parent_oid = MakeUnicodeString(d, (int)(e - d)); // parent OID

    KSyncFolderItem *p = NULL;
    if (lastParent != NULL)
      p = (KSyncFolderItem *)lastParent->findItemByOID(parent_oid);
    if (p == NULL)
      p = (KSyncFolderItem *)pRoot->findItemByOID(parent_oid);

    if (p == NULL)
    {
      if (StrCmp(oid, pRoot->mServerOID) == 0)
      {
        TCHAR msg[300];

        StringCchPrintf(msg, 300, _T("Server ScanRoot folder[%s] permission = 0x%x"), pRoot->GetFilename(), permissions);

        if (!KSyncItem::CheckPermissionRootFolder(permissions))
        {
          StringCchCat(msg, 300, _T(" : not enough permissions"));
          rtn = FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER; // R_FAIL_NO_PERMISSION_ON_SERVER_FOLDER;
        }
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        isRootFolder = TRUE;
      }
      else
      {
        //TCHAR buff[120];
        //StringCchPrintf(buff, 120, _T("skip name:%s, Buffer=%d"), name, m_ScanFolderIndex);
        //StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
        goto end_all;
      }
    }

    if (object_type == 'F')
    {
      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_folder;
      LPTSTR creatorOID = MakeUnicodeString(d, (int)(e - d)); // server folder creator

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);

      LPTSTR fullPathIndex = NULL;
      if (e != NULL)
      {
        fullPathIndex = MakeUnicodeString(d, (int)(e - d));
        d = e + 1;
        e = strchr(d, SCAN_VALUE_DELEMETER);
        if (e != NULL)
          kstr.assign(d, (int) (e - d));
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

      if (p != NULL)
      {
        item = p->findItemByOID(oid);

        if (item == NULL) // 새로 추가된 항목, 동일한 이름 존재 체크
          item = p->findChildByName(name);

        if (item != NULL)
        {
          // scan 목록에 포함됨 플래그 셋
          item->AddSyncFlag(SYNC_FLAG_IT_SCANNED);

          item->UpdateServerOID(oid, NULL, NULL);

          if (item->CheckServerRenamedAs(p->GetBaseFolder(), name))
            item->SetNewParent(p);

          if ((m_ScanFolderFlag & SCAN_ONLY_CHANGED) && childItemModified)
          {
            // 자식(1단계만) 항목이 모두 삭제됨을 기본 상태로 함.
            // 2017-09 : 전체 하위 항목이 삭제됨으로 해야 한다. 2018-07 : 1단계만 하도록 (WITH_RECURSE제외)
            item->SetScanServerFolderState(ITEM_DELETED, WITH_CHILD|EXCLUDE_MOVED);
          }
        }
        else
        {
          TCHAR fullname[MAX_PATH];
          MakeFullPathName(fullname, MAX_PATH, p->GetBaseFolder(), name);

          if (IsPartialSyncOn(fullname, TRUE))
          {
            item = new KSyncFolderItem(p, p->GetBaseFolder(), name, oid, NULL, NULL);
            item->LoadMetafile(GetStorage(), META_CLEAR_STATE);
            p->AddChild(item);
          }
          else
          {
            TCHAR msg[300];
            StringCchPrintf(msg, 300, _T("Skip on Partial Sync : %s"), fullname);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
          }
        }

        LPCTSTR parentPath = p->GetServerRelativePath();
        LPTSTR folderFullPath = AllocMakeFullPathName(parentPath, name);

        if (item != NULL)
        {
          item->UpdateServerNewFileInfo(0, systime, permissions,
            creatorOID, NULL, NULL, folderFullPath, fullPathIndex, name);

          //StringCchCopy(item->mServerNew.pFilename, MAX_PATH, name);

          item->SetServerScanState(ITEM_EXIST);

          // 이름은 동일해도 경로 바뀜으로 판단.
          if (item->IsFolderFullPathChanged())
          {
            if (item->mpServerRenamedAs != NULL)
              delete item->mpServerRenamedAs;
            item->mpServerRenamedAs = AllocString(folderFullPath);
          }

#if 0 // compare에서 업데이트 하도록
          // 마이그레이션 이후 첫번째 동기화
          if (mDbVersion < DB_VERSION_2)
          {
            item->addServerState(ITEM_STATE_NOW_EXIST);
            item->StoreMetafile(GetStorage(), META_MASK_SERVERSTATE, 0);
          }
#endif
          // item->addServerState(ITEM_STATE_NOW_EXIST);
        }
        if (folderFullPath != NULL)
          delete[] folderFullPath;
      }
      /* no need update. already update on start
      else if(isRootFolder)
      {
        pRoot->UpdateServerFileInfo(permissions, creatorOID, systime);
        info.pFullPathIndex = fullPathIndex;
      }
      */

      if ((item == NULL) && isRootFolder)
        item = pRoot;

      if (item != NULL)
      {
        if (childItemModified)
        {
          // 2017-09 : 전체 하위 항목이 삭제됨으로 해야 한다.=> 다시 1단계만(WITH_RECURSE)
          item->SetScanServerFolderState(ITEM_DELETED, WITH_CHILD|EXCLUDE_MOVED);
        }
        // 자신은 존재함으로
        item->SetScanServerFolderState(ITEM_EXIST, WITH_ITSELF);
      }

end_folder:
      if (creatorOID != NULL)
        delete[] creatorOID;
      if (fullPathIndex != NULL)
        delete[] fullPathIndex;
    }
    else if (object_type == 'D')
    {
      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_doc;
      LPTSTR lastModifier = MakeUnicodeString(d, (int)(e - d)); // server file last modifier

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_doc;
      LPTSTR file_oid = MakeUnicodeString(d, (int)(e - d)); // file OID

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e == NULL)
        goto end_doc;
      LPTSTR storage_oid = MakeUnicodeString(d, (int)(e - d)); // storage OID

      d = e + 1;
      e = strchr(d, SCAN_VALUE_DELEMETER);
      if (e != NULL)
        kstr.assign(d, (int) (e - d));
      else
        kstr.assign(d);
      __int64 fileSize = kstr.toInt64();

#ifdef _DEBUG_SCAN_SERVER_FOLDERS_ITEM
      TCHAR buff[512];
      StringCchPrintf(buff, 512, _T("Name:%s, OID=%s, permission=%d, folder=%s, file_oid=%s, storage_oid=%s, fileSize=%lld, Buffer=%d"),
        name, oid, permissions, parent_oid, file_oid, storage_oid, fileSize, m_ScanFolderIndex);
      StoreLogHistory(_T(__FUNCTION__), buff, SYNC_MSG_LOG);
#endif

      if (p != NULL)
      {
        item = p->findItemByOID(oid);

        if (item == NULL) // 새로 추가된 항목, 동일한 이름 존재 체크
          item = p->findChildByName(name);

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
          if (StrCmpI(item->mLocal.pFilename, name) == 0)
            pName = NULL;

          LPCTSTR parentPath = p->GetServerRelativePath();

          item->UpdateServerNewFileInfo(fileSize, systime, permissions, 
            lastModifier, file_oid, storage_oid, parentPath, NULL, pName);

          item->SetServerScanState(ITEM_EXIST);

#if 0 // compare에서 업데이트 하도록
          // 마이그레이션 이후 첫번째 동기화
          if (mDbVersion < DB_VERSION_2)
          {
            item->addServerState(ITEM_STATE_NOW_EXIST);
            item->StoreMetafile(GetStorage(), META_MASK_SERVERSTATE, 0);
          }
#endif
        }
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

int KSyncRootFolderItem::OnCompleteServerItemScan(int result)
{
  if (result == R_SUCCESS)
    SetPhase(PHASE_SCAN_FOLDERS_LOCAL-1);
  else
    OnCancelSync(result);

  return NEED_CONTINUE;
}

#endif // USE_SCAN_BY_HTTP_REQUEST & USE_SYNC_GUI

#ifdef USE_BACKUP
void KSyncRootFolderItem::OnRegistBackupEvent(int result)
{
   if (mPhase == PHASE_ENDING) // ending
  {
    if (mChildItemIndex == 1)
      mChildItemIndex = 2;
  }
  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("Result=%d, Phase=%d, index=%d"), result, mPhase, mChildItemIndex);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
}
#endif

int KSyncRootFolderItem::ContinueImmigration()
{
#ifdef USE_SYNC_ENGINE
  if (mPhase <= PHASE_SCAN_OBJECTS) // scanning sub ifo objects
  {
    KSyncRootStorage* storage = GetStorage();
    int r = storage->LoadChildMetaData(this, NULL, 
      KSyncRootStorage::WITH_ALLSUB|KSyncRootStorage::WITH_PAGING, mChildItemIndex);

    ++mChildItemIndex;
    mSyncFolderCount = GetTotalCount(1);
    mSyncFileCount = GetTotalCount(0);

    if (r == KSyncRootStorage::R_CONTINUE)
      return NEED_CONTINUE;

    SetPhase(PHASE_COMPARE_FILES);
    return NEED_CONTINUE;
  }
  else if (mPhase <= PHASE_COMPARE_FILES) // create new storage
  {
    // store to SyncRootStorageDB
    KSyncRootStorage* oldStorage = mStorage;

    LPTSTR server_oid = NULL;
    LPTSTR server_full_path = NULL;
    LPTSTR serverPathIndex = NULL;
    LPTSTR owner_oid = NULL;

    mStorage->LoadSettingValueStr(KEYB_SERVER_OID, &server_oid);
    mStorage->LoadSettingValueStr(KEYB_SERVER_FULLPATH, &server_full_path);
    mStorage->LoadSettingValueStr(KEYB_SERVER_FULLPATHINDEX, &serverPathIndex);
    mStorage->LoadSettingValueStr(KEYB_SYNC_FOLDER_OWNER_OID, &owner_oid);

    if (KSyncRootStorageDB::CreateMetaDataFolder(mStorage->mpBaseFolder, server_oid, server_full_path,
      serverPathIndex, owner_oid, &GetPolicy()) == R_SUCCESS)
    {
      moldStorage = mStorage;

      mStorage = new KSyncRootStorageDB(oldStorage->mpBaseFolder, FALSE);

      // delete old meta-folder
      /*
	    TCHAR name[KMAX_PATH];
	    MakeFullPathName(name, KMAX_PATH, oldStorage->mpBaseFolder, meta_folder_name);
	    if (IsDirectoryExist(name))
        EmptyDirectory(name, TRUE);
      delete oldStorage;
      */

      SetPhase(PHASE_LOAD_FILES-1);
    }
    else
    {
      TCHAR msg[200];

      StringCchPrintf(msg, 200, _T("MetaData immigration [%s] : failure"), oldStorage->mpBaseFolder);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    if (server_oid != NULL)
      delete[] server_oid;
    if (server_full_path != NULL)
      delete[] server_full_path;
    if (serverPathIndex != NULL)
      delete[] serverPathIndex;
    if (owner_oid != NULL)
      delete[] owner_oid;

    return((mPhase == (PHASE_LOAD_FILES-1)) ? NEED_CONTINUE : NOTHING_TODO);
  }
  else if (mPhase <= PHASE_LOAD_FILES) // store to database
  {
    KSyncFolderItem *folderItem = (KSyncFolderItem *)GetNextChild(PHASE_LOAD_FILES, 1);
    if (folderItem != NULL)
    {
      if (!folderItem->ContinueStoreMetafile(mStorage))
        folderItem->SetPhase(PHASE_LOAD_FILES); // done

      StoreLogHistory(_T(__FUNCTION__), _T("Store MetaDB"), SYNC_MSG_LOG);
      return NEED_CONTINUE;
    }

    // immigration version
    mStorage->StoreSettingValue(KEYB_META_DB_IMM, 1);

    TCHAR msg[200];
    StringCchPrintf(msg, 200, _T("MetaData immigration [%s] : %d"), moldStorage->mpBaseFolder, GetTotalCount(0));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

	  TCHAR name[KMAX_PATH];
	  MakeFullPathName(name, KMAX_PATH, moldStorage->mpBaseFolder, meta_folder_name);
	  if (IsDirectoryExist(name))
    {
      EmptyDirectoryFiles(name, META_DB_FILENAME);
      // EmptyDirectory(name, FALSE);
    }

    delete moldStorage;
    moldStorage = NULL;
    SetPhase(PHASE_ENDING);
  }
#endif // USE_SYNC_ENGINE
  return NOTHING_TODO;
}

int KSyncRootFolderItem::IsSyncNeedFolder(KSyncFolderItem *folderItem, int phase) // check partial-sync
{
  if (lstrlen(mSyncOnlyPath) > 0)
  {
    if (folderItem->IsSystemRoot())
      return 1; // 드라이브 스캔을 하여야 하니 1로

    LPTSTR p = mSyncOnlyPath;
    while (*p != NULL)
    {
      LPTSTR n = StrChr(p, PARTIAL_SYNC_FOLDER_SEPARATOR);
      if (n != NULL)
        *n = '\0';

      int r = isSyncNeedFolderEx(p, folderItem);

      if (n != NULL)
      {
        *n = PARTIAL_SYNC_FOLDER_SEPARATOR;
        p = n + 1;
      }
      else
      {
        p += lstrlen(p);
      }

      if (r != 0)
        return r;
    }
    return 0;
  }
  return 1;
}

int KSyncRootFolderItem::isSyncNeedFolderEx(LPCTSTR syncOnlyPath, KSyncFolderItem *folderItem)
{
  int sync_only_len = lstrlen(syncOnlyPath);
  if (sync_only_len > 0)
  {
    TCHAR resultFolder[MAX_PATH];
    folderItem->GetActualPathName(resultFolder, MAX_PATH, FALSE);
    LPCTSTR folder = resultFolder;

    if (lstrlen(folder) == 0) // this is root
      return 2;

    int folder_len = lstrlen(folder);
    // parent of sync-only-path
    if (folder_len < sync_only_len)
    {
      if ((StrCmpNI(folder, syncOnlyPath, folder_len) == 0) &&
        (syncOnlyPath[folder_len] == '\\'))
        return 2;
    }

    // sync-only-path or child of sync-only-path
    if ((folder_len >= sync_only_len) &&
      (StrCmpNI(folder, syncOnlyPath, sync_only_len) == 0))
    {
      return 1;
    }
    return 0;
  }
  return 1;
}

// 변경 목록의 상위 폴더들의 로컬 상태를 업데이트한다.
BOOL KSyncRootFolderItem::UpdateParentOfSyncOnlyPath()
{
  LPTSTR p = mSyncOnlyPath;
  if (p == NULL)
    return FALSE;

  while (*p != NULL)
  {
    LPTSTR n = StrChr(p, PARTIAL_SYNC_FOLDER_SEPARATOR);
    if (n != NULL)
      *n = '\0';

    KSyncItem* folderItem = FindChildItemCascade(p, 2, FALSE);
    while (folderItem != NULL)
    {
      if (folderItem->IsLocalFileExist())
        folderItem->addLocalState(ITEM_STATE_NOW_EXIST);
      folderItem = folderItem->mParent;
    }

    if (n != NULL)
    {
      *n = PARTIAL_SYNC_FOLDER_SEPARATOR;
      p = n + 1;
    }
    else
    {
      p += lstrlen(p);
    }
  }
  return TRUE;
}

// 변경 목록을 검사해서 폴더의 하위 항목을 스캔할 필요가 있는지 확인한다.
BOOL KSyncRootFolderItem::CheckNeedScanLocalFolder(KSyncFolderItem* folderItem)
{
#ifdef PURE_INSTANT_BACKUP
  if ((mSyncOnlyPath != NULL) && (mSyncMethod & SYNC_HIGH_PRIORITY))
  {
    if (folderItem->mPhase == 0) // 새로 추가된 항목
      return TRUE;

    if (folderItem->isLocalRenamedAs()) // 이름 변경된 항목
      return TRUE;

    KSyncRootStorage* storage = GetSafeStorage();
    // 변경목록에서 경로 검사
    KVoidPtrIterator it = mModifiedItemList.begin();
    while (it != mModifiedItemList.end())
    {
      ModifiedItemData* p = (ModifiedItemData*)*it++;

      if (StrCmpI(folderItem->mpRelativePath, p->pathName) == 0)
      {
        // 폴더가 백업되지 않았거나, 폴더 항목이 추가되거나 이동 대상이면 스캔할 필요 있음.
        if (!storage->IsMetafileExist(folderItem->mpRelativePath) ||
          (p->modifyType == DIR_ADDED) || (p->modifyType == DIR_MOVED_NEW))
          return TRUE;
      }
    }

    // 아니면 로컬 하위 스캔 불필요. 
    return FALSE;

    // 변경목록의 파일들만 조사
    /* 불필요 동작
    it = mModifiedItemList.begin();
    while (it != mModifiedItemList.end())
    {
      ModifiedItemData* p = (ModifiedItemData*)*it++;

      KSyncItem *item = FindChildItemCascade(p->pathName, 1, FALSE);
      if (item != NULL)
      {
        item->RefreshLocalNewState(this);
      }
    }
    */
  }
#endif // PURE_INSTANT_BACKUP
  return TRUE;
}

#define _DEBUG_MODIFIED_LIST

#ifdef USE_SYNC_STORAGE

int KSyncRootFolderItem::readyModifiedItem(LPTSTR pathName, DIR_MODIFY_EVENT modifyType)
{
  LPTSTR p = pathName;
  int fileOrFolder;
  int created_count = 0;

  if (modifyType == DIR_DELETED)
  {
    LPTSTR originName = NULL;
    LPTSTR target = pathName;

    // 경로명 그대로 준비하고, 삭제된 것으로 설정
    // metaDB에 존재하는지 확인
    fileOrFolder = GetStorage()->IsMetafileExist(pathName);
    if (fileOrFolder == 0)
    {
      // 이름 변경되었는지 확인
      if (AllocMovedRenamedOriginalName(pathName, &originName))
      {
        fileOrFolder = GetStorage()->IsMetafileExist(originName);
        target = originName;
      }
    }

    if (fileOrFolder > 0)
    {
      fileOrFolder--;
      // REFRESH_LOCALNEW 삭제 이벤트에서는 로컬 상태를 업데이트하지 않는다
      // (파일삭제+폴더삭제, 다른 폴더를 삭제된 이름으로 변경시, testcase-036)
      CreateChildItemCascade(target, fileOrFolder, this, REFRESH_RENAMED|SET_SCAN_DONE, &created_count);
      KSyncItem* item = FindChildItemCascade(target, fileOrFolder, FALSE);
      if (item != NULL)
      {
        item->maskLocalState(ITEM_STATE_NOW_EXIST, 0);
        item->maskLocalState(ITEM_STATE_PERSISTING, 0); // 오류 코드 클리어하고 다시 비교하도록.
      }
      else
      {
        TCHAR msg[256] = _T("Deleted item not found : ");
        StringCchCat(msg, 256, target);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }

    if (originName != NULL)
      delete[] originName;
  }
  else if (modifyType == DIR_MOVED)
  {
    // nothing to do
  }
  else
  {
    LPTSTR originName = NULL;
    LPTSTR target = pathName;
    KSyncItem* item = NULL;

    // 이름변경 되었으면 원래 경로로
    fileOrFolder = IsFileOrFolder(pathName);
    if (AllocMovedRenamedOriginalName(pathName, &originName))
    {
      target = originName;

      TCHAR msg[256] = _T("moved from : ");
      StringCchCat(msg, 256, originName);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    if (fileOrFolder < 0)
    {
      fileOrFolder = GetStorage()->IsMetafileExist(target);
      if (fileOrFolder > 0)
      {
        fileOrFolder--;
      }
      else
      {
        // 이름 변경이 되었으나 아직 등록되지 않은 상태,
        fileOrFolder = IsFileOrFolder(pathName); // 종류는 새 이름으로 판단하고, 경로 추가는 옛 이름으로.

        CreateChildItemCascade(target, fileOrFolder, this, REFRESH_LOCALNEW|SET_SCAN_DONE, &created_count);
        item = FindChildItemCascade(target, fileOrFolder, FALSE);
      }
    }
    if ((fileOrFolder >= 0) && (item == NULL))
    {
      CreateChildItemCascade(target, fileOrFolder, this, REFRESH_LOCALNEW|SET_SCAN_DONE, &created_count);
      item = FindChildItemCascade(target, fileOrFolder, FALSE);
    }

    if (item != NULL)
    {
      // 이동 및 이름 변경에서는 하위 항목에 대해 로컬 스캔이 필요하다(제외 폴더를 다른 이름으로 변경한 경우)
      if ((modifyType == DIR_MOVED_NEW) && item->IsFolder())
      {
        item->SetPhase(PHASE_SCAN_LOCAL_FILES - 1);
        created_count++;
      }
      item->maskLocalState(ITEM_STATE_PERSISTING, 0); // 오류 코드 클리어하고 다시 비교하도록.
      item->addLocalState(ITEM_STATE_NOW_EXIST);

      // 경로 변경 체크
      item->CheckLocalRenamedAsByOld(this);

      // 새 경로를 다시한번 확인
      if (modifyType == DIR_MOVED_NEW)
      {
        LPTSTR newpath = NULL;
        SplitPathNameAlloc(pathName, &newpath, NULL);
        CreateChildItemCascade(newpath, 1, this, REFRESH_LOCALNEW|SET_SCAN_NEED, &created_count);
        delete[] newpath;
      }
    }
    else
    {
      TCHAR msg[256] = _T("Target item not found : ");
      StringCchCat(msg, 256, target);
      if (originName != NULL)
      {
        StringCchCat(msg, 256, _T(", move as : "));
        StringCchCat(msg, 256, pathName);
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    if (originName != NULL)
      delete[] originName;
  }
  return created_count;
}

#endif

#ifdef PURE_INSTANT_BACKUP

int KSyncRootFolderItem::ReadyModifiedItemList()
{
  int created_count = 0;
  // 변경된 목록 경로에 대한 항목이 존재하지 않으면 생성한다.
  KVoidPtrIterator it = mModifiedItemList.begin();
  while (it != mModifiedItemList.end())
  {
    ModifiedItemData* p = (ModifiedItemData*)*it++;
    KSyncItem *item = NULL;

#ifdef _DEBUG_MODIFIED_LIST
    TCHAR msg[300];
    StringCchPrintf(msg, 300, _T("%s:%s"), p->pathName, getModifyEventName(p->modifyType));
#endif

#ifdef _DEBUG_MODIFIED_LIST
    if (p->modifyType == DIR_MOVED)
      StringCchCat(msg, 300, _T(" - skip"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

#ifdef _DEBUG_EXTRA // for test
    if (p->modifyType == DIR_MOVED)
      continue;
#endif
    created_count += readyModifiedItem(p->pathName, p->modifyType);
  }
  return created_count;
}
#endif // PURE_INSTANT_BACKUP

BOOL KSyncRootFolderItem::CheckScanFolderServerPartialSync(int phase)
{
  // 한 폴더만 동기화하는 경우
  if ((lstrlen(mSyncOnlyPath) > 0) &&
    (StrChr(mSyncOnlyPath, PARTIAL_SYNC_FOLDER_SEPARATOR) == NULL))
  {
    KSyncItem* item = FindChildItemCascade(mSyncOnlyPath, 1, FALSE);
    if ((item != NULL) && (lstrlen(item->mServerOID) > 0))
    {
      // exclude all items
      SetPhaseRecurse(PHASE_ENDING, 0);

      // include below this item
      if (item->IsFolder())
        ((KSyncFolderItem*)item)->SetPhaseRecurse(phase - 1, 0);
      else
        item->SetPhase(phase - 1);

      // include parent items
      ((KSyncFolderItem*)item)->SetPhaseRevRecurse(phase - 1);

      // include root item
      SetPhase(phase);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL KSyncRootFolderItem::IsPartialSyncOn(LPCTSTR pathname, BOOL isFolder)
{
  if ((pathname != NULL) && (lstrlen(pathname) > 0))
  {
    if (lstrlen(mSyncOnlyPath) > 0)
    {
      LPTSTR p = mSyncOnlyPath;
      while (*p != NULL)
      {
        LPTSTR n = StrChr(p, PARTIAL_SYNC_FOLDER_SEPARATOR);
        if (n != NULL)
          *n = '\0';

        BOOL r = IsPartialSyncOnEx(p, pathname, isFolder);

        if (n != NULL)
        {
          *n = PARTIAL_SYNC_FOLDER_SEPARATOR;
          p = n + 1;
        }
        else
        {
          p += lstrlen(p);
        }

        if (r)
          return TRUE;
      }
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL KSyncRootFolderItem::IsPartialSyncOnEx(LPCTSTR syncOnlyPath, LPCTSTR pathname, BOOL isFolder)
{
  if ((pathname != NULL) && (lstrlen(pathname) > 0))
  {
    int sync_only_len = lstrlen(syncOnlyPath);
    if (sync_only_len > 0)
    {
      int folder_len = lstrlen(pathname);
      if (pathname[folder_len-1] == '\\')
        folder_len--;

      // parent of sync-only-path
      if (folder_len < sync_only_len)
      {
        if ((StrCmpNI(pathname, syncOnlyPath, folder_len) == 0) &&
          (syncOnlyPath[folder_len] == '\\'))
        {
          return isFolder; // only folder item
        }
      }

      // sync-only-path or child of sync-only-path
      if ((folder_len >= sync_only_len) &&
        (StrCmpNI(pathname, syncOnlyPath, sync_only_len) == 0))
      {
        if ((folder_len > sync_only_len) && (pathname[sync_only_len] != '\\'))
          return FALSE;
        return TRUE;
      }
      return FALSE; // out of partial sync
    }
    return TRUE;
  }
  return FALSE;
}

BOOL KSyncRootFolderItem::refreshPartialSyncRenamed(KSyncRootStorage* storage)
{
  if ((mSyncOnlyPath != NULL) && (lstrlen(mSyncOnlyPath) > 0))
  {
    BOOL changed = FALSE;
    LPTSTR newOnlyPath = NULL;
    LPTSTR p = mSyncOnlyPath;
    while ((p != NULL) && (lstrlen(p) > 0))
    {
      LPTSTR n = StrChr(p, PARTIAL_SYNC_FOLDER_SEPARATOR);
      if (n != NULL)
        *n = '\0';

      LPTSTR renamedTarget = storage->FindRenamedName(p);

      if (renamedTarget != NULL)
      {
        newOnlyPath = AllocAddString(newOnlyPath, PARTIAL_SYNC_FOLDER_SEPARATOR, renamedTarget);
        delete[] renamedTarget;
        changed = TRUE;
      }
      else
      {
        newOnlyPath = AllocAddString(newOnlyPath, PARTIAL_SYNC_FOLDER_SEPARATOR, p);
      }
      p = n;
    }

    if (changed)
    {
      /*
      if (newOnlyPath != NULL)
        delete[] mSyncOnlyPath;
      mSyncOnlyPath = newOnlyPath;
      */
      // 바꾸지 말고 덧붙인다.
      LPTSTR paths = AllocAddString(mSyncOnlyPath, PARTIAL_SYNC_FOLDER_SEPARATOR, newOnlyPath);
      delete[] mSyncOnlyPath;
      mSyncOnlyPath = paths;
    }

    if (newOnlyPath != NULL)
      delete[] newOnlyPath;
    return changed;
  }
  return FALSE;
}

int KSyncRootFolderItem::CheckRootSyncable()
{
  int r = R_SUCCESS;

  if (RefreshServerState())
  {
    TCHAR msg[300];
    int perm = GetServerPermission();

    StringCchPrintf(msg, 300, _T("Server root folder[%s] permission = 0x%x"), GetFilename(), perm);
    //if ((perm & FOLDER_PERMISSIONS_FOR_SYNC) != FOLDER_PERMISSIONS_FOR_SYNC)
    if (!CheckPermissionRootFolder(perm))
    {
      StringCchCat(msg, 300, _T(" : not enough permissions"));
      r = FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER; // R_FAIL_NO_PERMISSION_ON_SERVER_FOLDER;
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
  else
  {
    r = FILE_SYNC_ROOTSERVER_FOLDER_NOT_EXIST; //R_FAIL_SERVER_FOLDER_NOT_EXIST;
  }
  return r;
}

int KSyncRootFolderItem::ContinueConflict(int flag)
{
  if (!(flag & DONE_AS_COMPLETE))
  {
    /*
    FreeSyncItemList(mConflictItemList);
    if (mCurrentFolderItem != NULL)
    {
      delete mCurrentFolderItem;
      mCurrentFolderItem = NULL;
    }

    mPhase = PHASE_SCAN_FILES;
    return ((mSyncFolderList.size() > 0) ? NEED_CONTINUE : NOTHING_TODO);
    */

    // should update SyncStatus by SyncResult
    UpdateFolderSyncResult(0, this, REMOVE_DISABLED|STORE_META); // |CLEAR_NOW_SYNCHRONIZING

    return NOTHING_TODO;
  }
  else
  {
    // number and size of files to load
    mSyncTotalCount = 0;
    mSyncTotalSize = 0;
    mSyncTotalSizeProgress = 0;

    // SetPhaseForceRecurse(PHASE_CONFLICT_FILES - 1);
    // SetDisabledItemPhase(PHASE_END_OF_JOB);

    // PHASE_END_OF_JOB for disabled item or (PHASE_CONFLICT_FILES - 1)
    SetEnable(TRUE);
    SetPhaseOnResolve(PHASE_CONFLICT_FILES, PHASE_MOVE_FOLDERS, PHASE_CONFLICT_FILES - 1);

    // skip no result items
    // SetUnchangedItemPhase(PHASE_CREATE_FOLDERS, PHASE_MOVE_FOLDERS, PHASE_CONFLICT_FILES, TRUE);

    SetPhase(PHASE_CONFLICT_FILES);// to skip root item
    GetCandidateCount(PHASE_ENDING/*PHASE_CONFLICT_FILES*/, TRUE, mSyncTotalCount, mSyncTotalSize, mSyncTotalSizeProgress);

    mSyncFileCount = 0;
    mSyncFolderCount = 0;
    mSyncDoneSize = 0;
    mSyncDoneSizeProgress = 0;
    mSyncProcessedSize = 0;

    // sync-start time update when start load files
    ::GetLocalTime(&mSyncStartTime);
    memcpy(&mSyncBeginTime, &mSyncStartTime, sizeof(mSyncStartTime));

    SetResult(0);
    SetConflictResult(0);

    // check server state again
    int err = CheckRootSyncable();
    if (err != 0)
    {
      OnCancelSync(err);
      return NEED_UI_FOR_SYNC_ERROR;
    }
  }
  return NEED_CONTINUE;
}

int KSyncRootFolderItem::LoadLastConflict()
{
  // load root-folder's metafile

  // show as synchronizing
  SetItemSynchronizing(GetRootPath(), 1, WITH_STORE_METAFILE);

  LoadServerPrevious();

  memset(&mLastSyncCompleteTime, 0, sizeof(mLastSyncCompleteTime));
  if (mSyncMethod & SYNC_METHOD_RESOLVE_CONFLICT)
  {
    GetRootLastSyncTime(NULL, &mLastSyncCompleteTime, NULL);
  }

  mSyncTotalCount = 0;
  mSyncFolderCount = 0;
  mSyncFileCount = 0;
  mSyncTotalSize = 0;
  mSyncDoneSize = 0;
  
  mSyncTotalSizeProgress = 0;
  mSyncDoneSizeProgress = 0;

  // root object initial state
  mLocalState = ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST;
  mServerState = ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST;

  // check server exist & permission
  int err = CheckRootSyncable();
  if (err != 0)
  {
    OnCancelSync(err);
    return NEED_UI_FOR_SYNC_ERROR;
  }

  return NEED_CONTINUE;
}

void KSyncRootFolderItem::StoreEllapsedTimeLog(LPCTSTR str)
{
  // ellapsed time
  TCHAR time_str[80];
  SYSTEMTIME currentTime;
  SYSTEMTIME ellapsed;
  TCHAR msg[300];

  ::GetLocalTime(&currentTime);
  TimeDifference(ellapsed, mSyncBeginTime, currentTime);
  GetEllapsedTimeString(time_str, 80, ellapsed, TRUE);

  StringCchCopy(msg, 300, str);
  StringCchCat(msg, 300, _T(" : "));

  StringCchCat(msg, 300, time_str);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
}

void KSyncRootFolderItem::CloseSync(int flag)
{
#ifdef USE_SYNC_ENGINE
  FreeChilds();

  TCHAR msg[300];
  StringCchCopy(msg, 300, GetFilename());

#ifdef USE_BACKUP
  if (mSyncMethod & SYNC_METHOD_RESTORE)
  {
    if (lstrlen(mRestoreFolderOID) > 0)
    {
      gpCoreEngine->RequestClearRestoreInfo(mRestoreFolderOID, RESTORE_STATUS_S);
      mRestoreFolderOID[0] = '\0';
    }
  }
#endif

  if (mSyncMethod & SYNC_METHOD_SETTING)
  {
    // store 
    StoreRootFolderServerPath(mServerOID, mServer.pFullPath, mServer.pFullPathIndex, FALSE);

    // store root-metafile
    StoreMetafile(GetStorage());

    gpCoreEngine->ShellNotifyIconChangeOverIcon(GetRootPath(), TRUE);

    StringCchCat(msg, 300, _T(" : Initialize Folder : Done"));
  }
  else
  {
    // clear sync-start flag & update OverlayIcon
    SetItemSynchronizing(GetRootPath(), 0, 0);
    if (flag & DONE_AS_COMPLETE)
      mChildResult = 0;
    StoreMetafile(GetStorage(), META_MASK_FILENAME|META_MASK_CONFLICT|META_MASK_SYNCRESULT|META_MASK_CHILD_RESULT, 0);

    SetItemSynchronizing(GetRootPath(), 0, SHELL_ICON_UPDATE_RECURSE);

    // full synchronization complete, clear rename history only full sync
    if (flag & DONE_AS_COMPLETE)
    {
      if (mDbVersion != CURRENT_DB_VERSION)
      {
        mDbVersion = CURRENT_DB_VERSION;
        mStorage->StoreSettingValue(KEYB_META_DB_IMM, mDbVersion);
      }
      ClearFileRenamed(mSyncOnlyPath);
    }

#ifdef USE_BACKUP
    // clear processed modified items
    storage->ClearAllModifyData(mModifiedKey);

    // find and clear OverridePath
#ifdef STORE_OVERRIDE_PATH
    if (mSyncMethod & SYNC_METHOD_RESTORE)
      CheckClearOverridePath(storage);
#endif
#endif

    // do not need store size
    // StoreMetafile(mBaseFolder, META_MASK_LOCALSIZE, 0);
    if (mSyncFileCount > 0)
      ShellNotifyECMChange(mServer.pFullPath);

    ::GetLocalTime(&mSyncDoneTime);
    UpdateRootLastSyncTime(flag);

    StringCchCat(msg, 300, _T(" : "));
    StringCchCat(msg, 300, getSyncTypeName(mSyncMethod));

    if (flag & DONE_AS_COMPLETE)
      StringCchCat(msg, 300, _T(" Complete"));
    else
      StringCchCat(msg, 300, _T(" Done"));

    // history-by-sqlite3
    LPCTSTR name = GetRootPath();
    StoreHistory(SYNC_H_END_SYNC, name, 2, NULL, NULL);
  }

#ifdef USING_SYNC_ONE_TRANSACTION
    mStorage->EndTransaction(TRUE);
#endif // USING_SYNC_ONE_TRANSACTION

  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
  clearCancelRenamedItem(TRUE);

  StoreEllapsedTimeLog(_T("Total Ellapsed time"));
#endif
}

void KSyncRootFolderItem::OnCancelSync(int sync_error)
{
  SetConflictResult(sync_error);

  StoreMetafile(GetStorage(), META_MASK_CONFLICT|META_MASK_SYNCRESULT, 0);

  gpCoreEngine->ShellNotifyIconChangeOverIcon(GetRootPath(), FALSE);

  if (mSyncFileCount > 0)
    ShellNotifyECMChange(mServer.pFullPath);

  // update last sync time even if failed.
  UpdateRootLastSyncTime(FALSE);

  // we have to load server folder name hear!!
  LoadServerPrevious();
}

// 사용자 강제 종료 : flag = CANCEL_BY_FORCE
int KSyncRootFolderItem::StoreBackupRestoreResult(int flag)
{
#if defined(USE_SYNC_GUI)
  BackupRestoreResultST* r = new BackupRestoreResultST;

  if ((mResult | mChildResult) & 
    (FILE_CONFLICT | FILE_SYNC_ERR))
    flag &= ~DONE_AS_COMPLETE;
  else
    flag = DONE_AS_COMPLETE;

  r->method = mSyncMethod;
  r->done_flag = flag;
  r->rootResult = GetConflictResult();
  r->failFolderCount = 0;
  r->failFileCount = 0;
  GetConflictCount(r->failFolderCount, r->failFileCount, FALSE);

  /*
  r->syncFolderCount = mSyncFolderCount - r->failFolderCount;
  r->syncFileCount = mSyncFileCount - r->failFileCount;
  */

  // 대상 전체 항목 개수와 실패한 항목 개수로 변경. 20170703
  r->syncFolderCount = mTargetFolderCount;
  r->syncFileCount = mTargetFileCount;

  StringCchCopy(r->versionFolderOID, MAX_OID, mServerOID);

  LPCTSTR name = GetRootPath();

  int history_event = 0;
  TCHAR msg[120];

  if (flag & DONE_AS_COMPLETE)
  {
    history_event = (mSyncMethod & SYNC_METHOD_RESTORE) ? SYNC_H_END_RESTORE : SYNC_H_END_BACKUP;
    StringCchPrintf(msg, 120, _T("Success"));
  }
  else
  {
    history_event = (mSyncMethod & SYNC_METHOD_RESTORE) ? SYNC_H_FAIL_RESTORE : SYNC_H_FAIL_BACKUP;
    StringCchPrintf(msg, 120, _T("Uncomplete"));
  }

  TCHAR str[120];
  StringCchPrintf(str, 120, _T(" folder(%d), file(%d), Faild folder(%d), file(%d)"), 
    r->syncFolderCount, r->syncFileCount, r->failFolderCount, r->failFileCount);
  StringCchCat(msg, 120, str);
  StoreHistory(history_event, name, 2, NULL, msg);

#ifdef USE_BACKUP
  // 전체 백업, 또는 복구
  if ((mSyncMethod & SYNC_METHOD_AUTO) ||
    (mSyncMethod & SYNC_METHOD_RESTORE))
  {
    /* not use folderOID
    if (r->method & SYNC_METHOD_RESTORE)
      StringCchCopy(r->restoreFolderOID, MAX_OID, mCurrentSyncRootFolderItem->mServerOID);
    else
      r->restoreFolderOID[0] = '\0';
    */
    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_NOTI_SYNC_EVENT, (LPARAM)r);
    return NEED_CONTINUE;
  }
  else
#endif
  {
    delete r;
  }
#endif // USE_SYNC_GUI
  return 0;
}

BOOL KSyncRootFolderItem::CalcNextSyncTime(SYSTEMTIME &time)
{
  SYSTEMTIME last_sync_time;
  SYSTEMTIME setting_time;
  SYSTEMTIME base;

  // check if auto-sync paused
  if (mPolicy.GetPaused())
    return FALSE;

  if (!(mPolicy.GetSyncMethod() & SYNC_METHOD_AUTO))
    return FALSE;

  /*
  KSyncRootStorage* storage = GetStorage(TRUE);
  int auto_sync_paused = 0;
  if (storage->LoadSettingValue(KEYB_AUTOSYNC_PAUSE, auto_sync_paused) && 
    (auto_sync_paused == 1))
    return FALSE;
  */

  memset(&last_sync_time, 0, sizeof(last_sync_time));
  memset(&setting_time, 0, sizeof(setting_time));

  GetRootLastSyncTime(&last_sync_time, NULL, &setting_time);
#ifdef _DEBUG_SYNC_TIME
  //memset(&last_sync_time, 0, sizeof(SYSTEMTIME));
  //memset(&setting_time, 0, sizeof(SYSTEMTIME));

  last_sync_time.wYear = 2017;
  last_sync_time.wMonth = 8;
  last_sync_time.wDay = 20;
  last_sync_time.wHour = 11;
  setting_time.wYear = 2017;
  setting_time.wMonth = 8;
  setting_time.wDay = 24;
  setting_time.wHour = 15;
  setting_time.wMinute = 44;

  mPolicy.SetSyncMethod(SYNC_METHOD_AUTO);
  mPolicy.SetRepeatEveryNDays(4);
  mPolicy.SetSyncTimeAt(660);
  mPolicy.SetSyncTimeEnd(690);
#endif

  gpCoreEngine->GetLoginTime(&base);
  if (IsValidSystemTime(last_sync_time))
  {
    TCHAR time_str[32];
    SystemTimeToTString(time_str, 32, last_sync_time);
    TCHAR msg[64] = _T("LastSync:");
    StringCchCat(msg, 64, time_str);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  if (IsValidSystemTime(setting_time))
  {
    TCHAR time_str[32];
    SystemTimeToTString(time_str, 32, setting_time);
    TCHAR msg[64] = _T("LastSet:");
    StringCchCat(msg, 64, time_str);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

#ifdef USE_BACKUP

  // 처음 시작일에 백업한다
  if (!IsValidSystemTime(last_sync_time))
  {
    gpCoreEngine->GetBackupStartTime(base);
    int sync_start_hm = mPolicy.GetSyncTimeAt() - 1; // before start
    base.wHour = sync_start_hm / 60;
    base.wMinute = sync_start_hm % 60;
    base.wSecond = 0;
    base.wMilliseconds = 0;

#ifdef _DEBUG
    TCHAR time_str[32];
    SystemTimeToTString(time_str, 32, base);
    TCHAR msg[64] = _T("BackupStart:");
    StringCchCat(msg, 64, time_str);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
  }
  else
  {
    memcpy(&base, &last_sync_time, sizeof(SYSTEMTIME));
  }

  if (IsValidSystemTime(setting_time) && // 시작 예정 시간이 이미 설정되어 있고 유효하면,
    (CompareSystemTimeDate(setting_time, last_sync_time) > 0) && // 설정일이 마지막 동기화 다음날이고,
    (CompareSystemTime(setting_time, last_sync_time) >= 0)) // 마지막 동기화 시간 다음이면, 그 값을 리턴한다.
  {
    BOOL setting_is_valid = TRUE;
    SYSTEMTIME current;
    ::GetLocalTime(&current);

    // 예정시간이 오늘이고, 시간 구간을 지난 경우
    if ((CompareSystemTimeDate(setting_time, current) == 0) &&
      mPolicy.IsTimeOver(current.wHour * 60 + current.wMinute))
      setting_is_valid = FALSE;

    // 예정시간이 오늘 또는 다음이고, 시간 구간 범위 안쪽이면 예정 시간을 리턴.
    if (setting_is_valid &&
      (CompareSystemTimeDate(setting_time, current) >= 0) &&
      mPolicy.IsMiddleOfTime(setting_time.wHour * 60 + setting_time.wMinute))
    {
      memcpy(&time, &setting_time, sizeof(setting_time));
      return TRUE;
    }
  }
  if (IsValidSystemTime(setting_time))
  {
    StoreLogHistory(_T(__FUNCTION__), _T("setting_time over"), SYNC_MSG_LOG);
  }

  // 설치일 + 유예일로부터 카운트한다.
  // 시간은 백업 시간대 종료후.
  gpCoreEngine->GetBackupStartTime(base);
  base.wHour = mPolicy.GetSyncTimeEnd() / 60;
  base.wMinute = mPolicy.GetSyncTimeEnd() % 60;
  base.wSecond = 30;

  if (mPolicy.CalcNextSyncTime(time, last_sync_time, base))
  {
    TCHAR time_last[32];
    TCHAR time_sync[32];

    SystemTimeToTString(time_last, 32, last_sync_time);
    SystemTimeToTString(time_sync, 32, time);

    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("FullBackup for %s, last=%s, next=%s"), 
      GetDisplayPathName(mLocal.pFilename), time_last, time_sync);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    // 시작 시간을 저장하여 둔다.
    char buff[32] = { 0 };
    SystemTimeToString(buff, 32, time);
    KSyncRootStorage* storage = GetStorage(FALSE);
    storage->StoreSettingValue(KEYB_LAST_SETTING, buff);

    return TRUE;
  }
  else
  {
    KSyncRootStorage* storage = GetStorage(TRUE);
    storage->StoreSettingValue(KEYB_LAST_SETTING, "");
    StoreLogHistory(_T(__FUNCTION__), _T("No Fullbackup policy"), SYNC_MSG_LOG);
  }

#else

  if (IsValidSystemTime(last_sync_time))
  {
    if (IsValidSystemTime(setting_time))
    {
      if (CompareSystemTimeDate(setting_time, last_sync_time) > 0)
        memcpy(&last_sync_time, &setting_time, sizeof(setting_time));
    }
  }
  else
  {
    memcpy(&last_sync_time, &setting_time, sizeof(setting_time));
  }

  if (mPolicy.CalcNextSyncTime(time, last_sync_time, base))
  {
    TCHAR time_last[32];
    TCHAR time_sync[32];
    SystemTimeToTString(time_last, 32, last_sync_time);
    SystemTimeToTString(time_sync, 32, time);

    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("AutoSync for %s, last=%s, next=%s"), 
      GetDisplayPathName(mLocal.pFilename), time_last, time_sync);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return TRUE;
  }
#endif // USE_BACKUP
  return FALSE;
}

void KSyncRootFolderItem::UpdateLastSyncTime()
{
  char buff[32] = { 0 };
  SYSTEMTIME time;
  ::GetLocalTime(&time);

  SystemTimeToString(buff, 32, time);
  KSyncRootStorage* storage = GetStorage(FALSE);
  storage->StoreSettingValue(KEYB_LAST_SYNC, buff);
}

int KSyncRootFolderItem::DeployFolderMovement()
{
  GetStorage()->DeployFolderMove(this);
  return 1;
}

BOOL KSyncRootFolderItem::OnFolderMovedInfo(LPTSTR oldName, LPTSTR newName)
{
  KSyncFolderItem *oldItem = (KSyncFolderItem *)FindChildItemCascade(oldName, 3);
  KSyncFolderItem *newItem = (KSyncFolderItem *)FindChildItemCascade(newName, 3);
  if ((oldItem != NULL) && (newItem != NULL) && (oldItem != newItem))
  {
    // if two items are exist already, skip merge.
    if (oldItem->ReadyMetafilename(GetStorage(), 0) &&
      newItem->ReadyMetafilename(GetStorage(), 0))
      return FALSE;

    if (oldItem->ReadyMetafilename(GetStorage(), 0) &&
      newItem->ReadyMetafilename(GetStorage(), METAFILE_CREATE_IF_NOT_EXIST))
    {
      // merge new item to old item
      newItem->MoveChildItems(oldItem);

      oldItem->SetLocalRenamedAs(newName);

      oldItem->MergeLocalInformation(newItem);

      if (newItem->mParent != NULL)
        oldItem->SetNewParentOID(newItem->mParent->mServerOID);

      RemoveItem(newItem);
      delete newItem;
      return TRUE;
    }
  }
  return FALSE;
}

int KSyncRootFolderItem::GetMovedRenamedOriginalName(LPCTSTR newRelativeName, LPTSTR originalRelativeName)
{
  KSyncRootStorage* s = GetStorage(TRUE);
  return s->FindOriginalNameT(newRelativeName, originalRelativeName, TRUE);

#if 0
  // find original filename which its new equal to relativePathName
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);

  KRenameStorage item(manage_file);
  return item.FindOriginalName(originalRelativeName, newRelativeName, TRUE);
#endif
}

int KSyncRootFolderItem::AllocMovedRenamedOriginalName(LPCTSTR newRelativeName, LPTSTR* originalRelativeName)
{
  KSyncRootStorage* s = GetStorage();
  return s->FindAllocOriginalNameT(newRelativeName, originalRelativeName, TRUE);
}

KSyncItem *KSyncRootFolderItem::GetMovedRenamedOriginal(LPCTSTR newRelativeName)
{
  TCHAR originName[KMAX_PATH];
  if (GetMovedRenamedOriginalName(newRelativeName, originName))
  {
    // 원 이름으로만 비교한다(크로스 리네임된 상태에서 바뀔 수 있음)
    // return FindChildItemCascade(originName, 3, 0); 
    KSyncItem* c= FindChildItemCascade(originName, 2, FALSE);

    if (c == NULL) // 개체가 로드되지 않은 경우.(실시간 백업에서 로드 안된 경우 있음)
    {
      ReadyChildItemCascade(this, originName);
      c= FindChildItemCascade(originName, 2, FALSE);
    }
    return c;
  }

  // check if folder name changed
  return NULL;
}

BOOL KSyncRootFolderItem::RemoveMovedRenamedOriginal(LPCTSTR newRelativeName)
{
  return GetStorage()->RemoveRenameItem(newRelativeName, NULL);
#if 0
  // find original filename which its new equal to relativePathName
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);

  KRenameStorage item(manage_file);
  return item.RemoveItem(newRelativeName, NULL);
#endif
}

LPTSTR KSyncRootFolderItem::AllocMovedRenamedAs(LPCTSTR oldRelativeName)
{
  return GetStorage()->AllocMovedRenamedAs(oldRelativeName, this);
#if 0
  LPTSTR newRelativeName = new TCHAR[KMAX_FULLPATH];
  KRenameStorage item(GetStorage());

  if (item.FindRenamedName(oldRelativeName, newRelativeName))
  {
    // if new filename not exist, remove it
    LPTSTR fullname = AllocPathName(mBaseFolder, newRelativeName);
    if (!IsFileExist(fullname))
    {
      RemoveMovedRenamedOriginal(newRelativeName);
      delete[] newRelativeName;
      newRelativeName = NULL;
    }
    else if (IsExcludeFileMask(newRelativeName))
    {
      delete[] newRelativeName;
      newRelativeName = NULL;
    }
    delete[] fullname;
  }
  return newRelativeName;
#endif
}

#if 0
BOOL KSyncRootFolderItem::GetMovedRenamedAs(LPCTSTR oldRelativeName, LPTSTR newRelativeName)
{
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);

  KRenameStorage item(manage_file);

  if (item.FindRenamedName(oldRelativeName, newRelativeName))
  {
    // if new filename not exist, remove it
    TCHAR fullname[MAX_PATH];
    MakeFullPathName(fullname, KMAX_PATH, mBaseFolder, newRelativeName);

    if (!IsFileExist(fullname))
    {
      RemoveMovedRenamedOriginal(newRelativeName);
      newRelativeName[0] = '\0';
      return FALSE;
    }

    if (!IsExcludeFileMask(newRelativeName))
      return TRUE;
  }
  return FALSE;
}
#endif

BOOL KSyncRootFolderItem::CheckRenamedInOut(LPCTSTR relativePath)
{
  return (GetStorage()->GetRenamedInOutCount(relativePath) > 0);
#if 0
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);

  KRenameStorage item(manage_file);
  return (item.GetRenamedInOutCount(relativePath) > 0);
#endif
}

void KSyncRootFolderItem::ClearFileRenamed(LPCTSTR subFolderSync)
{
  LPSTR subFolder = NULL;
  if ((subFolderSync != NULL) && (lstrlen(subFolderSync) > 0))
    subFolder = MakeUtf8String(subFolderSync);
  GetStorage()->ClearFileRenamed(subFolder);
#if 0
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);

  KRenameStorage item(manage_file);
  item.Clear();
#endif
}

BOOL KSyncRootFolderItem::ClearRenameFlag(LPCTSTR orgName, LPCTSTR newName)
{
  LPSTR oName = NULL;
  LPSTR nName = NULL;
  if (orgName != NULL)
    oName = MakeUtf8String(orgName);
  if (newName != NULL)
    nName = MakeUtf8String(newName);

  BOOL r = GetStorage()->ClearRenameFlag(oName, nName);

  if (oName != NULL)
    delete[] oName;
  if (nName != NULL)
    delete[] nName;
  return r;
}

// if server folder changed 'A' as 'A-r',
// replace renamed.txt 'A/B/a.txt=A/a.txt' as 'A-r/B/a.txt=A-r/a.txt' 
void KSyncRootFolderItem::UpdateRenameParentFolder(LPCTSTR prevRelative, LPCTSTR changedRelative)
{
  GetStorage()->UpdateRenameParentFolder(prevRelative, changedRelative);
#if 0
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);

  KRenameStorage item(manage_file);
  item.UpdateRenameParentFolder(prevRelative, changedRelative);
#endif
}

BOOL KSyncRootFolderItem::UpdateMetaFolderNameMapping(LPCTSTR lastRelativePath, LPCTSTR newRelativePath, BOOL includeSub)
{
  return GetStorage()->UpdateMetaFolderNameMapping(lastRelativePath, newRelativePath, includeSub);

#if 0
  TCHAR obj_path[KMAX_PATH];
  MakeFullPathName(obj_path, KMAX_PATH, mBaseFolder, meta_folder_name);
	MakeFullPathName(obj_path, KMAX_PATH, obj_path, object_folder_name);

  return UpdateMetaFolderNameMappingT(obj_path, lastRelativePath, newRelativePath, includeSub);
#endif
}

static TCHAR *GetPhaseName(int phase)
{
  typedef struct tagSyncPhaseName {
    int p;
    TCHAR *name;
  } SyncPhaseName;

  static TCHAR undef_name[64] = _T("");
  static SyncPhaseName phase_names[] = {
    {PHASE_COMPARE_FILES, _T("compare") },
    {PHASE_CLEAN_UNCHANGED, _T("cleanUnchanged") },
    {PHASE_CREATE_FOLDERS, _T("createNewFolder") },
    {PHASE_MOVE_FOLDERS, _T("moveTo") },
#ifdef USE_BACKUP_DELETE_EACH_OBJECT
    {PHASE_DELETE_FILES, _T("remove") },
#endif
    {PHASE_COPY_FILES, _T("copyDelete") },
    {PHASE_CONFLICT_FILES, _T("conflict") },
    {PHASE_ENDING, _T("ending") } };
  int count = sizeof(phase_names) / sizeof(SyncPhaseName);
  for(int i = 0; i < count; i++)
  {
    if (phase_names[i].p == phase)
      return phase_names[i].name;
  }
  StringCchPrintf(undef_name, 64, _T("Phase%d"), phase);
  return undef_name;
}

int KSyncRootFolderItem::syncFileItem(KSyncItem *item, int phase)
{
  if ((item == this) || item->IsExcludedSync()) // skip root folder
  {
    item->SetPhase(phase);
    return NEED_CONTINUE;
  }

  TCHAR msg[KMAX_LONGPATH];
  StringCchPrintf(msg, KMAX_LONGPATH, _T("phase=%s, path=%s"), 
    GetPhaseName(phase), item->GetPathString());

  //if (!(item->mFlag & SYNC_ITEM_RESTORE_OVERRIDE) && item->IsExcludedSync()) // skip excluded folder
  if (!item->IsOverridePath() && item->IsExcludedSync()) // skip excluded folder
  {
    item->SetPhase(phase);
    StringCchCat(msg, KMAX_LONGPATH, _T(" : excluded"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return NEED_CONTINUE;
  }

  if ((phase == PHASE_CONFLICT_FILES) && !item->mEnable)
    StringCchCat(msg, KMAX_LONGPATH, _T(" : disabled"));
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  mSyncItemFileSize = 0;
  mSyncProcessedSize = 0;
  if (!item->IsFolder())
    mSyncItemFileSize = item->GetSyncFileSize((phase == PHASE_CONFLICT_FILES));

  int r = item->mConflictResult;
  int sync_result = item->mResult;
  if ((phase != PHASE_CONFLICT_FILES) || item->mEnable)
  {
    r = item->doSyncItem(&mPolicy, GetRootPath(), phase);

    if (r & FILE_SYNC_THREAD_WAIT)
      return r;

    // if conflict or error occured, skip other phase
    if (r & (FILE_CONFLICT | FILE_SYNC_ERR))
      item->SetPhase(PHASE_ENDING);
  }
  else
  {
    // disabled item is end-of-job
    if (!item->mEnable)
      phase = PHASE_END_OF_JOB;

    item->SetPhase(phase);
  }

  if ((phase >= PHASE_LOAD_FILES) && (sync_result & (FILE_ACTION_MASK|FILE_CONFLICT|FILE_SYNC_ERR)))
    OnSyncFileItemDone(item, r, mSyncItemFileSize, phase);
  return NEED_CONTINUE;
}

void KSyncRootFolderItem::OnSyncFileItemDone(KSyncItem *item, int r, __int64 sync_size, int phase)
{
  // success or fail anyway, count and size will be incresed
  if (TRUE) // !(r & (FILE_CONFLICT | FILE_SYNC_ERR)))
  {
    if (item != this)
    {
      mSyncDoneSize += sync_size;
      mSyncDoneSizeProgress += sync_size + EMPTY_FILE_ESTIMATE_SIZE;
    }
    if (item->IsFolder())
      ++mSyncFolderCount;
    else
      ++mSyncFileCount;
  }
  mSyncProcessedSize = 0;

  if ((phase >= PHASE_COPY_FILES) &&
    !(r & (FILE_CONFLICT | FILE_SYNC_ERR)) && // do not remove conflict item or error item
    !(item->mFlag & SYNC_ITEM_WAIT_CLEAR) && // 서버에서 삭제처리하는 항목은 제거하지 않음
    !item->IsFolder() &&
    !item->IsTemporaryRenamed() && // temporary renamed item
    !item->ParentWillMove()) // parent will move
  {
    if (!RemoveItem(item))
    {
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("Remove synced item failure : path=%s\n"), item->GetPathString());
      OutputDebugString(msg);
    }
    else
    {
      delete item;
      item = NULL;
    }
  }
}


// check if .filesync folder exist &
// check if '.filesync/setting.txt' has valid server-OID
// return 1 if all condition match, return 2 if it has invalid server-OID
int KSyncRootFolderItem::IsSyncRootFolder(LPCTSTR pathName, BOOL onlyCurrentUser)
{
#ifdef SYNC_ALL_DRIVE
  return ((lstrlen(pathName) > 0) && (pathName[0] == '\\'));
#else
  return KSyncRootStorage::IsMatchRootFolder(pathName, onlyCurrentUser);
#endif

#if 0
  TCHAR path[KMAX_PATH];
  MakeFullPathName(path, KMAX_PATH, pathName, meta_folder_name);
	if (IsDirectoryExist(path))
  {
    // check server oid
    TCHAR setting_filename[KMAX_PATH];
    MakeFullPathName(setting_filename, KMAX_PATH, path, SYNC_ROOT_FOLDER_SETTING_FILENAME);

    if (onlyCurrentUser)
    {
      char owner[MAX_OID];
      if (ReadSettingKeyValue(setting_filename, SYNC_FOLDER_OWNER_OID, owner, MAX_USERNAME))
      {
        TCHAR ownerOid[MAX_OID] = { 0 };
        ToUnicodeString(ownerOid, MAX_OID, owner, (int)strlen(owner));
        if (StrCmp(mUserOid, ownerOid) != 0)
          return 0;
      }
    }

    char server_oid[MAX_OID] = { 0 };
    if(ReadSettingKeyValue(setting_filename, SERVER_OID_KEY, server_oid, MAX_OID))
    {
      if (strlen(server_oid) < 8) // sync initializing folder
        return 2;
      return 1;
    }
  }
#endif
  return 0;
}

#if 0
int KSyncRootFolderItem::ReadySyncMetaData(LPCTSTR path, LPCTSTR base_path, LPCTSTR object_path, KSyncPolicy *p)
{
	TCHAR filename[KMAX_PATH];
	WIN32_FIND_DATA fd;
	int count = 0;

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	MakeFullPathName(filename, KMAX_PATH, path, L_ASTERISK_STR);

	// find normal files
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

	HANDLE h = FindFirstFile(filename, &fd);
	LPTSTR server_id = NULL;
  SYSTEMTIME systemTime;

	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
        // check exclude policy
        if (p->IsExcludeFileMask(fd.cFileName))
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Excluded file on local=%s"), fd.cFileName);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        else
        {
				  MakeFullPathName(filename, KMAX_PATH, path, fd.cFileName);

          FileTimeToSystemTime(&fd.ftLastWriteTime, &systemTime);
          __int64 size = MAKE_INT64(fd.nFileSizeLow, fd.nFileSizeHigh);

				  CreateFileSyncMetaData(filename, base_path, object_path, server_id, systemTime, size, TRUE);
				  count++;
        }
			}

		} while (FindNextFile(h, &fd));

		FindClose(h);
	}

	fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	MakeFullPathName(filename, KMAX_PATH, path, L_ASTERISK_STR);

	h = FindFirstFile(filename, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ((StrCmp(fd.cFileName, L".") == 0) ||
					(StrCmp(fd.cFileName, L"..") == 0) ||
					(StrCmpI(fd.cFileName, meta_folder_name) == 0))
					continue;

        // check exclude policy
        if (p->IsExcludeFileMask(fd.cFileName))
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Excluded folder on local=%s"), fd.cFileName);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        else
        {
				  MakeFullPathName(filename, KMAX_PATH, path, fd.cFileName);

          FileTimeToSystemTime(&fd.ftLastWriteTime, &systemTime);
				  CreateFileSyncMetaData(filename, base_path, object_path, server_id, systemTime, 0, FALSE);

				  count += ReadySyncMetaData(filename, base_path, object_path, p);
        }
			}

		} while (FindNextFile(h, &fd));

		FindClose(h);
	}
	return count;
}
#endif

#if 0
static int MakeMD5HashFromFile(LPCTSTR filename, BYTE *bHash)
{
	HCRYPTPROV hCryptProv;
	HCRYPTHASH hHash;
	DWORD dwHashLen = 16;
	int rtn = 0;

	FILE *f = NULL;
	errno_t err = _tfopen_s(&f, filename, L"r");
	if (err == 0)
	{
		BYTE *buff = new BYTE[1024 * 16];

		if (buff != NULL)
		{
			if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
			{
				if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash))
				{
					size_t size;
					size_t total = 0;
					while ((size = fread(buff, 1024 * 16, 1, f)) > 0)
					{
						if (!CryptHashData(hHash, (const BYTE *)buff, size, 0))
						{
							OutputDebugString(L"CryptHashData error occured\n");
							break;
						}
						total += size;
					}

					if (CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0))
						rtn = (int)dwHashLen;
					CryptDestroyHash(hHash);
				}
				CryptReleaseContext(hCryptProv, 0);
			}
			delete[] buff;
		}
		fclose(f);
	}
	else {
		TCHAR msg[300];
		StringCchPrintf(msg, 300, L"cannot open file : %s, error=%d", filename, err);
		OutputDebugString(msg);
		StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
	}

	return rtn;
}
#endif

static void ByteToString(char *str_buff, int buff_size, BYTE *bin_data, int size)
{
	char buff[4];

	if (buff_size < (size * 2 + 1))
		size = (buff_size - 1) / 2;

	for (int i = 0; i < size; ++i)
	{
		sprintf_s(buff, 4, "%.2X", *bin_data++);
		memcpy(str_buff, buff, 2);
		str_buff += 2;
	}
	*str_buff = '\0';
}

static int CompareWithLocalFile(LPTSTR doc_name, LPTSTR storage_id, LPCTSTR local_path)
{
  TCHAR filename[KMAX_PATH];
  MakeFullPathName(filename, KMAX_PATH, local_path, doc_name);

  if ( !IsFileExist(filename) )
  {
    // TCHAR url[KMAX_PATH];
    // DWORD n = LocalSync_GetFileDownloadUrl(storage_id, url);

    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Need to download %s, local_file=%s"), doc_name, filename);
    OutputDebugString(msg);
  }
  return 0;
}

static KSyncItem *findSyncItemByName(KSyncItemArray array, LPCTSTR fileName)
{
  KSyncItemIterator it = array.begin();
  int index = 0;
  while(it != array.end())
  {
    if (StrCmpI((*it)->mLocal.pFilename, fileName) == 0)
      return *it;
    it++;
  }
  return NULL;
}

#if 0

BOOL KSyncRootFolderItem::IsExcludeFileMask(LPCTSTR name)
{
  return mPolicy.IsExcludeFileMask(name);
}


int KSyncRootFolderItem::scanSubFolder(LPCTSTR server_oid, LPCTSTR local_path)
{
  int count = 0;

  // scan server folders
  SYNCFOLDER syncFolder;
  memset(&syncFolder, 0, sizeof(syncFolder));
  if (LocalSync_GetFolder((LPTSTR)server_oid, &syncFolder) == 0)
  {
    SYNCFOLDER *pChilds = syncFolder.pChildFolders;
    for (int i = 0; i < syncFolder.nChildCount; i++)
    {
      if (!IsExcludeFileMask(pChilds[i].szName))
      {
        KSyncItem *item = new KSyncItem(pChilds[i].szName, pChilds[i].szOID, NULL, TRUE);
        mSyncFolderList.push_back(item);
        item->LoadMetafile(mBaseFolder);
        ++count;
      }
    }
    if(syncFolder.pChildFolders != NULL)
      LocalSync_DestroyFolderStruct(&syncFolder);
  }

  // scan local folders & files
	WIN32_FIND_DATA fd;
  TCHAR filename[KMAX_PATH];

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	MakeFullPathName(filename, KMAX_PATH, local_path, L_ASTERISK_STR);

	HANDLE h = FindFirstFile(filename, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
				if ((StrCmp(fd.cFileName, L".") == 0) ||
					(StrCmp(fd.cFileName, L"..") == 0) ||
					(StrCmpI(fd.cFileName, meta_folder_name) == 0))
          continue;

        if (!IsExcludeFileMask(fd.cFileName))
        {
          KSyncItem *item = findSyncItemByName(mSyncFolderList, fd.cFileName);
          if (item == NULL)
          {
            item = new KSyncItem(local_path, fd.cFileName, !!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
          }
          if (item != NULL)
          {
            item->LoadMetafile(mBaseFolder);
            mSyncFolderList.push_back(item);
            ++count;
          }
        }
      }
		} while (FindNextFile(h, &fd));
		FindClose(h);
	}
  return count;
}

int KSyncRootFolderItem::scanSubFiles(LPCTSTR server_oid, LPCTSTR local_path)
{
  int page = 0;
  DWORD r;
  TCHAR msg[256];
	int count = 0;

  // scan server files
  do
  {
    SYNCDOCUMENTLIST doc_list;
    memset(&doc_list, 0, sizeof(SYNCDOCUMENTLIST));
    r = LocalSync_GetDocuments((LPTSTR)server_oid, 10, page, &doc_list);
    // compare
    PSYNCDOCUMENT doc = doc_list.documents;
    for (int i = 0; i < doc_list.nDocumentCount; i++)
    {
        StringCchPrintf(msg, 256, _T("file name=%s, size=%s"), doc->szName, doc->szFileSize);
        OutputDebugString(msg);

        // check exclude mask
        if (!IsExcludeFileMask(doc->szName))
        {
          //n = CompareWithLocalFile(doc->szName, doc->szStorageFileID, local_path);
          KSyncItem *item = new KSyncItem(doc->szName, (LPTSTR)doc->szOID, doc->szStorageFileID, FALSE);
          item->LoadMetafile(mBaseFolder);
          mSyncItemList.push_back(item);
          count++;
        }
    }
    
    // clear
    if(doc_list.documents != NULL)
    {
      LocalSync_DestroyDocumentListStruct(&doc_list);
      /*
      doc = doc_list.documents;
      for (int i = 0; i < doc_list.nDocumentCount; i++)
        LocalSync_DestroyDocumentStruct(doc++);
      */
    }
    else
      break;
  } while(r == 0);

  // scan local files
	WIN32_FIND_DATA fd;
  TCHAR filename[KMAX_PATH];

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	MakeFullPathName(filename, KMAX_PATH, local_path, L_ASTERISK_STR);

	HANDLE h = FindFirstFile(filename, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do {
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
      {
        // check exclude mask
        if (!IsExcludeFileMask(fd.cFileName))
        {
          KSyncItem *item = findSyncItemByName(mSyncItemList, fd.cFileName);
          if (item == NULL)
          {
            item = new KSyncItem(local_path, fd.cFileName, FALSE);
          }
          if (item != NULL)
          {
            item->LoadMetafile(mBaseFolder);
            mSyncItemList.push_back(item);
            count++;
          }
        }
      }
		} while (FindNextFile(h, &fd));
		FindClose(h);
	}
  return count;
}

int KSyncRootFolderItem::recurseSyncFolder(LPCTSTR server_oid, LPCTSTR local_path)
{
  int page = 0;
  DWORD r;
  TCHAR msg[256];
  int n;
  KSyncItemArray array;

  // scan server files
  do
  {
    SYNCDOCUMENTLIST doc_list;
    memset(&doc_list, 0, sizeof(SYNCDOCUMENTLIST));
    r = LocalSync_GetDocuments((LPTSTR)server_oid, 10, page, &doc_list);
    // compare
    PSYNCDOCUMENT doc = doc_list.documents;
    for (int i = 0; i < doc_list.nDocumentCount; i++)
    {
        StringCchPrintf(msg, 256, _T("file name=%s, size=%s"), doc->szName, doc->szFileSize);
        OutputDebugString(msg);
        n = CompareWithLocalFile(doc->szName, doc->szStorageFileID, local_path);

        array.push_back(new KSyncItem(doc->szName, (LPTSTR)doc->szOID, doc->szStorageFileID, FALSE));
    }
    
    // clear
    if(doc_list.documents != NULL)
      LocalSync_DestroyDocumentListStruct(&doc_list);
    else
      break;
    /*
    doc = doc_list.documents;
    for (int i = 0; i < doc_list.nDocumentCount; i++)
      LocalSync_DestroyDocumentStruct(doc++);
    */
  } while(r == 0);

  // scan server folders
  SYNCFOLDER syncFolder;
  memset(&syncFolder, 0, sizeof(syncFolder));
  if (LocalSync_GetFolder((LPTSTR)server_oid, &syncFolder) == 0)
  {
    SYNCFOLDER *pChilds = syncFolder.pChildFolders;
    for (int i = 0; i < syncFolder.nChildCount; i++)
    {
       array.push_back(new KSyncItem(pChilds[i].szName, pChilds[i].szOID, NULL, TRUE));
    }
    if(syncFolder.pChildFolders != NULL)
      LocalSync_DestroyFolderStruct(&syncFolder);
  }

  // scan local folders & files
	WIN32_FIND_DATA fd;
  TCHAR filename[KMAX_PATH];
	int count = 0;

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	MakeFullPathName(filename, KMAX_PATH, local_path, L_ASTERISK_STR);

	HANDLE h = FindFirstFile(filename, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do {

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
      {
				if ((StrCmp(fd.cFileName, L".") == 0) ||
					(StrCmp(fd.cFileName, L"..") == 0) ||
					(StrCmpI(fd.cFileName, meta_folder_name) == 0))
          continue;
      }
      KSyncItem *item = findSyncItemByName(array, fd.cFileName);
      if (item == NULL)
      {
        item = new KSyncItem(local_path, fd.cFileName, !!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
        array.push_back(item);
      }

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
      {
        __int64 size = (__int64)fd.nFileSizeLow + ((__int64)fd.nFileSizeHigh << 32);
        item->SetLocalFileSize(size);
      }

      FileTimeToSystemTime(&fd.ftLastWriteTime, &item->mLocalFileTime);

      // compare file item
      compareFileItem(local_path, item);

      mSyncTotalSize += item->GetSyncFileSize();
		} while (FindNextFile(h, &fd));
		FindClose(h);
	}

  KSyncItemArray folders;

  // merge conflict items
  while (array.size() > 0)
  {
    KSyncItem *item = array.back();
    int r;

    if ((r = item->GetResult()) > FILE_NOT_CHANGED)
    {
      if (r >= FILE_CONFLICT)
        mConflictItemList.push_back(item);
      else
      {
        if (item->IsFolder())
          folders.push_back(new KSyncItem(*item));
        else
          mSyncItemList.push_back(item);
      }
      item = NULL;
    }
    else if (item->IsFolder())
    {
      folders.push_back(item);
      item = NULL;
    }

    if (item != NULL)
      delete item;
    array.pop_back();
  }

  // recurse child folders
  while (folders.size() > 0)
  {
    KSyncItem *item = folders.back();
    BOOL do_child = TRUE;

    if (item->IsServerNotExist())
      do_child = createServerFolder(item);
    else if (item->IsLocalNotExist())
      do_child = createLocalFolder(item);

    if (do_child)
    {
      TCHAR sub_oid[MAX_OID];
      StringCchCopy(sub_oid, MAX_OID, item->mServerOID);
      TCHAR sub_path[KMAX_PATH];
      MakeFullPathName(sub_path, KMAX_PATH, local_path, item->mLocal.pFilename);

      // count += recurseSyncFolder(sub_oid, sub_path);
      mSyncFolderList.push_back(new KSyncItem(sub_path, sub_oid, NULL, TRUE));
    }

    delete item;
    folders.pop_back();
  }
  return count;
}

int KSyncRootFolderItem::compareFileItem(LPCTSTR local_path, KSyncItem *item)
{
  if (StrCmp(local_path, item->mLocal.FullPath) != 0)
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("path not match %s, %s\n"), local_path, item->mLocal.FullPath);
    OutputDebugString(msg);
    return 0;
  }

  TCHAR filename[KMAX_PATH];
  MakeFullPathName(filename, KMAX_PATH, local_path, item->mLocal.pFilename);

  if (!item->ReadyMetafilename(mBaseFolder))
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Cannot make Metafile for %s\n"), filename);
    OutputDebugString(msg);
    return 0;
  }

  int r = FILE_NOT_CHANGED;
  if (item->IsNewlyAdded())
  {
    // There is no info file, it will be a newly added item
    item->setLocalState(ITEM_STATE_NEWLY_ADDED);

    // create info file
    item->StoreMetafile(mBaseFolder);

    // check if old SyncRootFolder
    if (item->IsFolder() && (IsSyncRootFolder(filename) > 0))
      r = FILE_RELEASE_PREV_SYNC_ROOT;
    else if (item->IsLocalNotExist())
      r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;
  }

  // gather local time info
  SYSTEMTIME current_local_time;
  SYSTEMTIME current_server_time;

  item->GetCurrentFileTime(current_local_time, current_server_time);

  if (r == FILE_NOT_CHANGED)
  {
    if (CompareSystemTime(current_local_time, item->mLocalFileTime) > 0) // local changed
    {
      if (CompareSystemTime(current_server_time, item->mServerFileTime) > 0) // server changed
        r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;
      else
        r = FILE_NEED_UPLOAD;
    }
    else
    {
      if (CompareSystemTime(current_server_time, item->mServerFileTime) > 0)// server changed
        r = FILE_NEED_DOWNLOAD;
    }
  }

  // for test
  r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;

  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("SyncCompare name=%s, result=%s"), item->mLocal.pFilename, GetSyncCompareResultString(r));
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  item->SetResult(r);
  return r;
}

#endif

void FreeSyncRootItemList(KSyncRootItemArray &array)
{
  while(array.size() > 0)
  {
    KSyncRootFolderItem *item = array.back();
    delete item;
    array.pop_back();
  }
}

BOOL KSyncRootFolderItem::IsExcludeFileMask(LPCTSTR filename)
{
  if (StrChr(filename, '\\') != NULL)
  {
    TCHAR pureName[KMAX_FILENAME];
    SplitPathName(filename, NULL, pureName);

    return (gpCoreEngine->IsExcludeFileMask(pureName) ||
        mPolicy.IsExcludeFileMask(pureName));
  }
  else
  {
  return (gpCoreEngine->IsExcludeFileMask(filename) ||
        mPolicy.IsExcludeFileMask(filename));
  }
}

BOOL KSyncRootFolderItem::IsExcludeFileMask(KSyncItem* item)
{
#ifdef USE_BACKUP
  // 백업에서는 파일명만 처리한다.
  if (item->IsFolder() == 0)
  {
    LPCTSTR name = item->GetFilename();
    if (gpCoreEngine->IsExcludeFileMask(name) ||
      mPolicy.IsExcludeFileMask(name))
      return TRUE;
  }
#else
  LPCTSTR name = item->GetFilename();
  if (gpCoreEngine->IsExcludeFileMask(name) ||
    mPolicy.IsExcludeFileMask(name))
    return TRUE;

  BOOL excluded = FALSE;
  LPTSTR pathName = item->AllocRelativePathName();
  if (lstrlen(pathName) > lstrlen(item->GetFilename()))
    excluded = mPolicy.IsExcludeFileMask(pathName);
  delete[] pathName;
  return excluded;
#endif
  return FALSE;
}

BOOL KSyncRootFolderItem::CheckExcludeNameMask()
{
  return (DisableExcludedFileMask(this) > 0);
}

BOOL KSyncRootFolderItem::SerializePolicy(BOOL bStore)
{
#ifdef USE_BACKUP
  KSyncRootStorageDB storage(mHomeFolder, !bStore);
  if (bStore)
    return storage.StoreSyncPolicy(mPolicy);
  else
    return storage.LoadSyncPolicy(mPolicy);
#else // USE_BACKUP
  LPCTSTR rootFolder = GetRootPath();
  #ifdef _DEBUG
    if (lstrlen(rootFolder) == 0)
    {
      TCHAR msg[512];
      StringCchCopy(msg, 512, _T("Empty Root : \\"));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      return FALSE;
    }
  #endif

#ifdef USE_SYNC_ENGINE
  // ready Storage class
  if ((rootFolder != NULL) && KSyncRootStorageFile::IsMatchRootFolder(rootFolder, TRUE))
  {
    KSyncRootStorageFile storage(rootFolder, FALSE);
    if (bStore)
      return storage.StoreSyncPolicy(mPolicy);
    else
      return storage.LoadSyncPolicy(mPolicy);

    /*
    TCHAR base_path[KMAX_PATH];
    StringCchCopy(base_path, KMAX_PATH, rootFolder);
    MakeFullPathName(base_path, KMAX_PATH, base_path, meta_folder_name);
    MakeFullPathName(base_path, KMAX_PATH, base_path, SYNC_ROOT_FOLDER_SETTING_FILENAME);

    if (bStore)
      return mPolicy.Store(base_path);
    else
      return mPolicy.Load(base_path);
    */
  }
  else
  {
    KSyncRootStorageDB storage(rootFolder, !bStore);
    if (bStore)
      return storage.StoreSyncPolicy(mPolicy);
    else
      return storage.LoadSyncPolicy(mPolicy);
  }
#else
    return 0;
#endif
#endif // USE_BACKUP
}

BOOL KSyncRootFolderItem::IsChangedAfterLastSync(SYSTEMTIME &lastModifiedTime)
{
  if ( !IsValidSystemTime(mLastSyncCompleteTime) || (CompareSystemTime(lastModifiedTime, mLastSyncCompleteTime) > 0))
    return TRUE;
  return FALSE;
}

void KSyncRootFolderItem::UpdateRootLastSyncTime(int flag)
{
  char buff[32] = { 0 };
  SystemTimeToString(buff, 32, mSyncDoneTime);

  // global last-sync-time is only updated when sync success.
#ifdef USE_BACKUP
  // 전체 백업 완료 시간만 기록
  /*
  if ((flag & DONE_AS_COMPLETE) && 
    ((GetSyncMethod() & SYNC_METHOD_FULL_BACKUP) == SYNC_METHOD_FULL_BACKUP))
  */

  // 미완료된 경우도 마지막 백업시간으로 기록한다.(17.05.24)
  if((GetSyncMethod() & SYNC_METHOD_FULL_BACKUP) == SYNC_METHOD_FULL_BACKUP)
#else
  // include partial sync
  if (flag & DONE_AS_COMPLETE)
#endif
  {
    // update app.txt. it will be shown at tray-popup
    AppendSettingKeyValue(KEY_LAST_SYNC, buff);
  }

  SystemTimeToString(buff, 32, mSyncDoneTime);
  KSyncRootStorage* storage = GetStorage();

  if ((flag & DONE_AS_COMPLETE) && (lstrlen(mSyncOnlyPath) == 0))
  {
#ifndef SYNCTIME_BASE_LAST_START
    // 백업 종료일 기준으로 다음 백업 계산할때.
    storage->StoreSettingValue(KEYB_LAST_SYNC, buff);
#endif

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
#ifdef USE_BACKUP
    // 백업에서는 전체백업일때에만 서버 스캔 타임을 저장한다.
    if (((mSyncMethod & SYNC_METHOD_FULL_BACKUP) == SYNC_METHOD_FULL_BACKUP) &&
      ((mSyncOnlyPath == NULL) || (lstrlen(mSyncOnlyPath) == 0)))
    {
      storage->StoreSettingValue(KEYB_LAST_SCAN_TIME, mPreviousScanTime);
      mLastScanTime = mPreviousScanTime;
    }
#else
    storage->StoreSettingValue(KEYB_LAST_SCAN_TIME, mPreviousScanTime);
    mLastScanTime = mPreviousScanTime;
#endif
#endif
  }

  if (GetSyncMethod() & SYNC_METHOD_AUTO)
  {
    storage->StoreSettingValue(KEYB_LAST_AUTOSYNC, buff);

#ifdef _DEBUG
    TCHAR msg[256];
    TCHAR timestr[32];
    SystemTimeToTString(timestr, 32, mSyncDoneTime);
    StringCchPrintf(msg, 256, _T("Autosync Done [%s] at "), GetRootPath());
    StringCchCat(msg, 256, timestr);
    StringCchCat(msg, 256, _T("\n"));
    OutputDebugString(msg);
#endif
  }
}

BOOL KSyncRootFolderItem::GetRootLastSyncTime(SYSTEMTIME *auto_time, SYSTEMTIME *sync_time, SYSTEMTIME *setting_time)
{
  BOOL r = FALSE;

  KSyncRootStorage* storage = GetStorage(TRUE);
  if (storage == NULL)
    return FALSE;

  LPSTR buff = NULL;
  if ((auto_time != NULL) && storage->LoadSettingValue(KEYB_LAST_AUTOSYNC, &buff))
  {
    if (StringToSystemTime(buff, *auto_time))
      r = TRUE;
    delete[] buff;
    buff = NULL;
  }
  if ((sync_time != NULL) && storage->LoadSettingValue(KEYB_LAST_SYNC, &buff))
  {
    if (StringToSystemTime(buff, *sync_time))
      r = TRUE;
    delete[] buff;
    buff = NULL;
  }
  if ((setting_time != NULL) && storage->LoadSettingValue(KEYB_LAST_SETTING, &buff))
  {
    if (StringToSystemTime(buff, *setting_time))
      r = TRUE;
    delete[] buff;
    buff = NULL;
  }

#if 0
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, mBaseFolder, meta_folder_name);
  MakeFullPathName(setting_file, KMAX_PATH, setting_file, SYNC_ROOT_FOLDER_SETTING_FILENAME);

  char buff[64] = { 0 };

  if ((auto_time != NULL) && ReadSettingKeyValue(setting_file, KEY_LAST_AUTOSYNC, buff, 64))
  {
    if (StringToSystemTime(buff, *auto_time))
      r = TRUE;
  }
  if ((sync_time != NULL) && ReadSettingKeyValue(setting_file, KEY_LAST_SYNC, buff, 64))
  {
    if (StringToSystemTime(buff, *sync_time))
      r = TRUE;
  }
  if ((setting_time != NULL) && ReadSettingKeyValue(setting_file, KEY_LAST_SETTING, buff, 64))
  {
    if (StringToSystemTime(buff, *setting_time))
      r = TRUE;
  }
#endif
  return r;
}

BOOL KSyncRootFolderItem::AddSyncingFileItem(LPCTSTR fileName)
{
#ifndef SYNC_ALL_DRIVE
  if (IsChildFolderOrFile(GetRootPath(), fileName, FALSE))
#endif
  {
    clearRegisteredOverTime();

    SYNCING_FILE_ITEM *item = new SYNCING_FILE_ITEM;
#ifdef SYNC_ALL_DRIVE
    StringCchCopy(item->relativeName, MAX_PATH, fileName);
#else
    StringCchCopy(item->relativeName, MAX_PATH, &fileName[lstrlen(GetRootPath())+1]);
#endif
    item->registedTick = GetTickCount();

    mSyncingItems.insert(mSyncingItems.begin(), item);
    return TRUE;
  }
  return FALSE;
}

int KSyncRootFolderItem::IsMonitoringSkipFileItem(LPCTSTR fileName)
{
#ifndef SYNC_ALL_DRIVE
  LPCTSTR rootFolder = GetRootPath();
  if (IsChildFolderOrFile(rootFolder, fileName, FALSE))
  {
    LPCTSTR relativeName = &fileName[lstrlen(rootFolder)+1];
#else
  {
    LPCTSTR relativeName = fileName;
#endif
    // .localsync.db file
    if (StrCmpN(relativeName, META_DB_FILENAME, lstrlen(META_DB_FILENAME)) == 0)
      return 1;

    // .filesync folder
    if (StrCmpN(relativeName, meta_folder_name, lstrlen(meta_folder_name)) == 0)
      return 1;

#ifdef USE_SYNC_STORAGE
    // 제외 폴더 경로인지 확인
    LPTSTR pathName = AllocString(fileName);
    int is_excluded = gpCoreEngine->IsExcludedFolderEx(GetSafeStorage(), NULL, pathName);
    delete[] pathName;
    if(is_excluded > 0)
      return 1;
#endif
    SFI_TYPE t = IsSyncingFileItemRelative(relativeName);
    return((t == SFI_SYNCING_FILE) ? 2 : 0);
  }
  return 0;
}

SFI_TYPE KSyncRootFolderItem::IsSyncingFileItemRelative(LPCTSTR relativeName)
{
    clearRegisteredOverTime();
    KVoidPtrIterator it = mSyncingItems.begin();
    while (it != mSyncingItems.end())
    {
      LP_SYNCING_FILE_ITEM item = (LP_SYNCING_FILE_ITEM)*it;
      if (StrCmp(item->relativeName, relativeName) == 0)
        return SFI_SYNCING_FILE;
      ++it;
    }
    return SFI_NO_ITEM;
}

SFI_TYPE KSyncRootFolderItem::IsSyncingFileItem(LPCTSTR pathName)
{
#ifdef SYNC_ALL_DRIVE
    return IsSyncingFileItemRelative(pathName);
#else
  LPCTSTR rootFolder = GetRootPath();
  if (IsChildFolderOrFile(rootFolder, pathName, FALSE))
  {
    LPCTSTR relativeName = &pathName[lstrlen(rootFolder)+1];
    SFI_TYPE t = IsSyncingFileItemRelative(relativeName);
    if (t == SFI_NO_ITEM)
      t = SFI_BELOW_SYNC_FOLDER;
  }
  return SFI_NO_ITEM;
#endif
}

// #define _DEBUG_SYNCING_ITEM

#define TICK_CHANGE_AFTER_REGIST_LIMIT 5000 // 5-sec

void KSyncRootFolderItem::clearRegisteredOverTime()
{
  KVoidPtrIterator it = mSyncingItems.begin();
  DWORD currentTick = GetTickCount();
  DWORD delay;

  while (it != mSyncingItems.end())
  {
    LP_SYNCING_FILE_ITEM item = (LP_SYNCING_FILE_ITEM) *it;
    delay = (item->registedTick <= currentTick) ? (currentTick - item->registedTick) : (0xFFFFFFFF - item->registedTick + currentTick);

    if (delay > TICK_CHANGE_AFTER_REGIST_LIMIT)
    {
#ifdef _DEBUG_SYNCING_ITEM
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("Clear change-register(%d) "), delay);
      StringCchCat(msg, 300, item->relativeName);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

      it = mSyncingItems.erase(it);
      delete item;
    }
    else
    {
      ++it;
    }
  }
}

void KSyncRootFolderItem::clearAllSyncingItem()
{
  KVoidPtrIterator it = mSyncingItems.begin();
  while (it != mSyncingItems.end())
  {
    LP_SYNCING_FILE_ITEM item = (LP_SYNCING_FILE_ITEM) *it;
    it = mSyncingItems.erase(it);
    delete item;
  }
}


void KSyncRootFolderItem::SavePausedState(BOOL paused)
{
  mPolicy.SetPaused(paused);

  GetStorage()->StoreSyncPaused((int)paused);

#if 0
  TCHAR setting_file[KMAX_PATH];
  StringCchCopy(setting_file, KMAX_PATH, mBaseFolder);
  MakeFullPathName(setting_file, KMAX_PATH, setting_file, meta_folder_name);
  MakeFullPathName(setting_file, KMAX_PATH, setting_file, SYNC_ROOT_FOLDER_SETTING_FILENAME);
  mPolicy.StorePaused(setting_file);
#endif
}

int KSyncRootFolderItem::StoreRootFolderServerPath(LPTSTR server_folder_oid, LPTSTR server_path, LPTSTR fullPathIndex, BOOL server_location_changed)
{
  KSyncRootStorage *storage = GetStorage();
  
  storage->BeginTransaction();

  if (server_location_changed)
  {
    storage->StoreSettingValueStr(KEYB_SERVER_NEW_OID, server_folder_oid);
  }
  else
  {
    storage->StoreSettingValueStr(KEYB_SERVER_OID, server_folder_oid);
    storage->StoreSettingValueStr(KEYB_SERVER_NEW_OID, _T(""));
  }
  storage->StoreSettingValueStr(KEYB_SERVER_FULLPATH, server_path);
  storage->StoreSettingValueStr(KEYB_SERVER_FULLPATHINDEX, fullPathIndex);

  storage->EndTransaction(TRUE);

  // TODO: update to Server.pFullPath, pFullPathIndex

#if 0
#ifdef USE_SQLITE_META
  if (server_location_changed)
  {
    sql3_StoreSettingKeyValue(GetStorage(), KEYB_SERVER_NEW_OID, server_folder_oid);
  }
  else
  {
    sql3_StoreSettingKeyValue(GetStorage(), KEYB_SERVER_OID, server_folder_oid);
    sql3_StoreSettingKeyValue(GetStorage(), KEYB_SERVER_NEW_OID, "");
  }
  sql3_StoreSettingKeyValue(GetStorage(), KEYB_SERVER_FULLPATH, server_path);
  sql3_StoreSettingKeyValue(GetStorage(), KEYB_SERVER_FULLPATHINDEX, fullPathIndex);

#else
  TCHAR name[KMAX_PATH];
  MakeFullPathName(name, KMAX_PATH, mBaseFolder, meta_folder_name);
  MakeFullPathName(name, KMAX_PATH, name, SYNC_ROOT_FOLDER_SETTING_FILENAME);

  char str[KMAX_PATH_A];
  ToUtf8String(str, 256, server_folder_oid, lstrlen(server_folder_oid));

  if (server_location_changed)
  {
    StoreSettingKeyValue(name, SERVER_NEW_OID_KEY, str);
  }
  else
  {
    StoreSettingKeyValue(name, SERVER_OID_KEY, str);
    StoreSettingKeyValue(name, SERVER_NEW_OID_KEY, "");
  }

  ToUtf8String(str, KMAX_PATH_A, server_path, lstrlen(server_path));
  StoreSettingKeyValue(name, SERVER_FULLPATH_KEY, str);

  ToUtf8String(str, KMAX_PATH_A, server_path, lstrlen(fullPathIndex));
  StoreSettingKeyValue(name, SERVER_FULLPATHINDEX_KEY, str);
#endif
#endif
	return R_SUCCESS;
}

void KSyncRootFolderItem::LoadServerPrevious()
{
  KSyncRootStorage *storage = GetStorage();
  LPSTR str = NULL;

  if (mServer.pFullPath != NULL)
  {
    delete[] mServer.pFullPath;
    mServer.pFullPath = NULL;
  }
  if (mServer.pFullPathIndex != NULL)
  {
    delete[] mServer.pFullPathIndex;
    mServer.pFullPathIndex = NULL;
  }

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
  mLastScanTime = 0;
#endif

  if (storage != NULL)
  {
    if (storage->LoadSettingValue(KEYB_SERVER_OID, &str))
    {
      ToUnicodeString(mServerOID, MAX_OID, str, (int)strlen(str));
      delete[] str;
    }

    if (storage->LoadSettingValue(KEYB_SERVER_FULLPATH, &str))
    {
      mServer.pFullPath = MakeUnicodeString(str);
      delete[] str;
    }
    if (storage->LoadSettingValue(KEYB_SERVER_FULLPATHINDEX, &str))
    {
      mServer.pFullPathIndex = MakeUnicodeString(str);
      delete[] str;
    }

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
    storage->LoadSettingValue(KEYB_LAST_SCAN_TIME, mLastScanTime);
#endif
  }

#if 0
#ifdef USE_SQLITE_META
  LPSTR str = sql3_LoadSettingKeyValueStr(GetStorage(), KEYB_SERVER_FULLPATH);
  if (strlen(str) > 0)
  {
    ToUnicodeString(mServer.FullPath, KMAX_PATH, str, (int)strlen(str));
    delete[] str;
  }
  else
    mServer.FullPath[0] = '\0';
#else
  TCHAR name[KMAX_PATH];
  MakeFullPathName(name, KMAX_PATH, mBaseFolder, meta_folder_name);
  MakeFullPathName(name, KMAX_PATH, name, SYNC_ROOT_FOLDER_SETTING_FILENAME);

  char str[KMAX_PATH] = { 0 };
  ReadSettingKeyValue(name, SERVER_FULLPATH_KEY, str, KMAX_PATH);
  if (strlen(str) > 0)
    ToUnicodeString(mServer.FullPath, KMAX_PATH, str, (int)strlen(str));
  else
    mServer.FullPath[0] = '\0';
#endif
#endif
}

int KSyncRootFolderItem::MergeSyncOnlyPath(LPCTSTR relativePath)
{

  return 0;
}

int KSyncRootFolderItem::SetSyncOnlyPath(LPCTSTR relativePath)
{
  LPCTSTR rootFolder = GetRootPath();
#ifdef USE_SYNC_STORAGE
  if (mSyncOnlyPath != NULL)
  {
    delete[] mSyncOnlyPath;
    mSyncOnlyPath = NULL;
  }
  if ((relativePath != NULL) && (lstrlen(relativePath) > 0))
  {
    mSyncOnlyPath = AllocString(relativePath);

#ifndef SYNC_ALL_DRIVE
    LPTSTR orgRelativePath = NULL;
    if (KSyncCoreBase::IsAlreadySyncedFolderEx(this, relativePath, &orgRelativePath))
    {
      if (orgRelativePath != NULL)
      {
        // 원 경로와 새 경로명을 더헤 SyncOnlyPath로 한다
        delete[] mSyncOnlyPath;
        int len = lstrlen(relativePath) + lstrlen(orgRelativePath) + 2;
        mSyncOnlyPath = new TCHAR[len];
        StringCchCopy(mSyncOnlyPath, len, relativePath);

        StringCchCat(mSyncOnlyPath, len, _T(";"));
        StringCchCat(mSyncOnlyPath, len, orgRelativePath);
      }
    }
    else
    {
      return R_FAIL_NEED_FULL_SYNC_PARTIAL; // R_FAIL_NEED_FULL_SYNC_VERGIN;
    }

#if 0
    // append original name also.
    TCHAR originalRelativePath[MAX_PATH];
    if (GetMovedRenamedOriginalName(relativePath, originalRelativePath))
    {
      // check if original folder already synced.
      // MakeFullPathName(fullpath, MAX_PATH, rootFolder, originalRelativePath);
      if (!KSyncCoreBase::IsAlreadySyncedFolder(GetStorage(), rootFolder, originalRelativePath))
        return R_FAIL_NEED_FULL_SYNC_VERGIN;

      delete[] mSyncOnlyPath;
      int len = lstrlen(relativePath) + lstrlen(originalRelativePath) + 2;
      mSyncOnlyPath = new TCHAR[len];
      StringCchCopy(mSyncOnlyPath, len, relativePath);

      StringCchCat(mSyncOnlyPath, len, _T(";"));
      StringCchCat(mSyncOnlyPath, len, originalRelativePath);
    }
    else
    {
      // check if sub folder already synced.
      // MakeFullPathName(fullpath, MAX_PATH, rootFolder, relativePath);
      if (!KSyncCoreBase::IsAlreadySyncedFolder(GetStorage(), rootFolder, relativePath))
        return R_FAIL_NEED_FULL_SYNC_VERGIN;
    }
#endif

    // check if some files are moved into or out from partial sync
    BOOL in_out_moved = CheckRenamedInOut(relativePath);
    if (in_out_moved) 
      return R_FAIL_NEED_FULL_SYNC_PARTIAL; // we need full sync
#endif // SYNC_ALL_DRIVE
  }
#endif // USE_SYNC_STORAGE

  return R_SUCCESS;
}

void KSyncRootFolderItem::SetSyncExtraInfo(LPCTSTR info)
{
  AllocCopyString(&mSyncExtraInfo, info);
}

KSyncItem* KSyncRootFolderItem::CreateReadyChildMetafile(LPCTSTR relativePathName, BOOL isFolder)
{
  LPTSTR path = new TCHAR[lstrlen(relativePathName)+1];
  TCHAR name[KMAX_PATH];
  SplitPathName(relativePathName, path, name);
  KSyncItem *item = NULL;

  if (isFolder)
    item = new KSyncFolderItem(NULL, name);
  else
    item = new KSyncItem(NULL, name);

  item->SetRelativePathName(relativePathName);
  BOOL meta_ready = item->LoadMetafile(GetStorage(), META_MASK_FLAG|META_MASK_SERVEROID);
  if (!meta_ready) // check if parent renamed
  {
    if (lstrlen(path) > 0)
    {
      // 중간 폴더가 이름변경된 상태 + 하위 폴더 이름 변경 발생
      LPTSTR renamedPathName = new TCHAR[KMAX_FULLPATH];
      if (GetMovedRenamedOriginalName(relativePathName, renamedPathName))
      {
        // 이전 경로로 FolderItem 로드한다
        delete item;

        LPTSTR npath = new TCHAR[lstrlen(renamedPathName)+1];
        SplitPathName(renamedPathName, npath, name);

        if (isFolder)
          item = new KSyncFolderItem(NULL, name);
        else
          item = new KSyncItem(NULL, name);

        item->SetRelativePathName(renamedPathName);
        meta_ready = item->LoadMetafile(GetStorage(), META_MASK_FLAG|META_MASK_SERVEROID);

        delete[] npath;
      }
      delete[] renamedPathName;
    }
  }
  if (!meta_ready)
  {
    delete item;
    item = NULL;
  }
  delete[] path;
  return item;
}

// return == 0 if oldFileName is out of this RootFolderItem
// return == 1 if oldFileName is child of this RootFolderItem
// return == 2 if it is on .filesync
// return == 3 if it is root folder
int KSyncRootFolderItem::OnFileRenamed(LPCTSTR oldFileName, LPCTSTR newFileName)
{
#ifdef SYNC_ALL_DRIVE
  return 1;
#else
  LPCTSTR rootFolder = GetRootPath();
  int base_len = lstrlen(rootFolder);
  /*
  if ((lstrlen(oldFileName) >= base_len) &&
    (StrNCmpI(mBaseFolder, oldFileName, base_len) == 0)) // _tcsncmp
  */
  if (IsChildFolderOrFile(rootFolder, oldFileName, FALSE))
  {
    if (lstrlen(oldFileName) == base_len)
    {
      // root folder is moved, renamed
      return 3;
    }

    TCHAR oName[KMAX_PATH];
    TCHAR nName[KMAX_PATH] = { 0 };

    StringCchCopy(oName, KMAX_PATH, &oldFileName[base_len+1]);

    if((lstrlen(newFileName) > base_len) &&
      (StrNCmpI(rootFolder, newFileName, base_len) == 0)) // _tcsncmp
      StringCchCopy(nName, KMAX_PATH, &newFileName[base_len+1]);

#ifdef USE_SQLITE_META
    if (StrNCmpI(oName, META_DB_FILENAME, lstrlen(META_DB_FILENAME)) == 0) // _tcsncmp
      return 2;
    if (StrNCmpI(oName, DOWNTEMP_FOLDER_NAME, lstrlen(DOWNTEMP_FOLDER_NAME)) == 0) // _tcsncmp
      return 2;

#else
    if (StrNCmpI(oName, meta_folder_name, lstrlen(meta_folder_name)) == 0) // _tcsncmp
      return 2;
#endif
    //StoreFileMonitorSequence(oName, nName, FM_FILE_RENAME);
    return 1;
  }
  return 0;
#endif
}

BOOL KSyncRootFolderItem::OnStoreRenamed(LPCTSTR oldFileName, LPCTSTR newFileName, BOOL forceAppend)
{
  int appendable = KSyncRootStorage::KRS_APPENDABLE;
  int refresh_modified = 0;
#ifdef SYNC_ALL_DRIVE
  LPCTSTR or_name = oldFileName;
  LPCTSTR nr_name = newFileName;
#else
  int base_len = lstrlen(GetRootPath());
  LPCTSTR or_name = &oldFileName[base_len+1];
  LPCTSTR nr_name = &newFileName[base_len+1];
#endif

  // check if it is a synchronized file
  BOOL isFolder = IsDirectoryExist(newFileName);
  KSyncItem* item = NULL;
  BOOL cleared = FALSE;

  if (!forceAppend)
  {
    item = CreateReadyChildMetafile(or_name, isFolder);
    if (item == NULL)
      item = CreateReadyChildMetafile(nr_name, isFolder);

    if ((item == NULL) ||
      !item->IsAlreadySynced()) // not synced yet
      appendable = 0;
  }

  if (TRUE) // appendable)
  {
    KSyncRootStorage *storage = GetStorage();
    LPTSTR srcRelativePathName = NULL;

    storage->BeginTransaction();
    if (item != NULL)
    {
      // 메타 데이터에 이름 변경이 셋팅된 경우,
      // oldFileName이 이미 변경되었으면, 원본 경로로 바꾼다.
      srcRelativePathName = item->AllocRelativePathName();

      // 원래 경로가 먼저 변경되었는지
      LPTSTR changedPath = storage->FindRenamedName(srcRelativePathName);

      if (lstrcmp(srcRelativePathName, or_name) != 0)
      {
        storage->SetRenameT(srcRelativePathName, nr_name, appendable);
        if (isFolder)
        {
          storage->SetRenameT(((changedPath != NULL) ? changedPath : srcRelativePathName),
            nr_name, KSyncRootStorage::KRS_CHANGE_PATH);
        }
        or_name = NULL;
      }

      if (changedPath != NULL)
        delete[] changedPath;
      delete item;
      item = NULL;
    }
    else
    {
#if 0
      // check if MS-OFFICE temp file -> move to KSyncCoreEngine
      if (!isFolder && isMsOfficeTempStore(oldFileName, newFileName))
      {
        // rename 원본 찾기.
        TCHAR originName[KMAX_PATH];
        BOOL cleared = FALSE;
        if (GetMovedRenamedOriginalName(nr_name, originName))
          cleared = storage->ClearRenameT(originName, NULL); // nr_name
        else
          cleared = storage->ClearRenameT(nr_name, NULL); // nr_name

        if (cleared) // nr_name
        {
          or_name = NULL;
          cleared = TRUE;
        }
      }
#endif
    }

    if (or_name != NULL)
    {
      storage->StoreRenameT(or_name, nr_name, appendable);

      if (isFolder)
        storage->StoreRenameT(or_name, nr_name, KSyncRootStorage::KRS_CHANGE_PATH);
    }

#ifdef USE_INSTANT_SYNC
    // update ModifiedTable also
    refresh_modified = storage->UpdateModifiedFilename(oldFileName, newFileName);
#endif

    storage->EndTransaction(TRUE);

#ifdef _DEBUG
    storage->dumpRenamedItems();
#endif

    if (srcRelativePathName != NULL)
      delete[] srcRelativePathName;
  }

  TCHAR msg[512];
  StringCchPrintf(msg, 512, _T("SyncItem '%s' renamed as '%s' appended=%d, cleared=%d, refresh_modified=%d"), 
    oldFileName, newFileName, appendable, cleared, refresh_modified);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  return !cleared;
}

int KSyncRootFolderItem::OnFileRenameRemoved(LPCTSTR newFileName)
{
  int base_len = lstrlen(GetRootPath());
  /*
  if ((lstrlen(newFileName) >= base_len) &&
    (StrNCmpI(mBaseFolder, newFileName, base_len) == 0)) // _tcsncmp
  */
  if (IsChildFolderOrFile(GetRootPath(), newFileName, FALSE))
  {
    if (lstrlen(newFileName) == base_len)
    {
      // root folder is moved ??
      return 0;
    }

    return 1;
  }
  return 0;
}

BOOL KSyncRootFolderItem::OnRemoveRenamed(LPCTSTR newFileName, LPCTSTR oldFileName)
{
  int base_len = lstrlen(GetRootPath());
  TCHAR nName[KMAX_PATH] = { 0 };
  TCHAR oName[KMAX_PATH] = { 0 };
  StringCchCopy(nName, KMAX_PATH, &newFileName[base_len+1]);
  StringCchCopy(oName, KMAX_PATH, &oldFileName[base_len+1]);
    
  KSyncRootStorage *storage = GetStorage();
  int n = storage->RemoveRenameItem(nName, oName);
#if 0
  TCHAR manage_file[KMAX_PATH];
  GetRenameManageFilename(manage_file);
  KRenameStorage sitem(manage_file);

  int n = sitem.RemoveItem(nName, oName);
#endif

  if (n > 0)
  {
    TCHAR msg[512];
    if (n == KSyncRootStorage::R_RENAME_RESTORED) //restored
      StringCchPrintf(msg, 512, _T("SyncItem [%s] Rename Restored [%s]"), newFileName, oldFileName);
    else if (n == KSyncRootStorage::R_RENAME_REMOVED)
      StringCchPrintf(msg, 512, _T("SyncItem [%s] Rename Removed"), newFileName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return TRUE;
  }
  return FALSE;
}


// 동기화 작업에 의해 이름변경된 내용, 저장했다가 동기화 마지막에 DB에서 클리어하기 위해서
BOOL KSyncRootFolderItem::OnStoreCancelRenamed(LPCTSTR oldFileName, LPCTSTR newFileName)
{
  if ((oldFileName != NULL) && (newFileName != NULL))
  {
    if (oldFileName[1] == ':')
    {
      oldFileName += lstrlen(GetRootPath());
      if (*oldFileName == '\\')
        oldFileName++;
    }
    if (newFileName[1] == ':')
    {
      newFileName += lstrlen(GetRootPath()) ;
      if (*newFileName == '\\')
        newFileName++;
    }
    if ((lstrlen(oldFileName) > 0) && (lstrlen(newFileName) > 0))
    {
      int len = lstrlen(oldFileName) + lstrlen(newFileName) + 2;
      LPTSTR str = new TCHAR[len];
      StringCchCopy(str, len, oldFileName);
      StringCchCat(str, len, _T("|")); // DA_VALUE_SEPARATOR
      StringCchCat(str, len, newFileName);

      mRenamedStack.push_back(str);
      return TRUE;
    }
  }
  return FALSE;
}

void KSyncRootFolderItem::clearCancelRenamedItem(BOOL clearData)
{
  KSyncRootStorage *storage = GetStorage(TRUE);
  if ((storage == NULL) || storage->m_readOnly)
    return;

  storage = GetStorage(FALSE); // as write mode
  KVoidPtrIterator it = mRenamedStack.begin();

  while (it != mRenamedStack.end())
  {
    LPTSTR oldname = (LPTSTR) *it;
    if (clearData)
    {
      LPTSTR newname = StrChr(oldname, DA_VALUE_SEPARATOR);
      if (newname != NULL)
      {
        *newname = '\0';
        newname++;
        int n = storage->RemoveRenameItem(newname, oldname);

#ifdef _DEBUG
        TCHAR msg[600];
        StringCchPrintf(msg, 600, _T("ClearRenamed %s : %s, %s"), oldname, newname,
          ((n == KSyncRootStorage::R_RENAME_REMOVED) ? _T("ok") : _T("not found")));
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
      }
    }
    it = mRenamedStack.erase(it);
    delete[] oldname;
  }
  mRenamedStack.clear();
}

BOOL existValueString(LPSTR pList, LPSTR itemOID)
{
  if ((pList != NULL) && (strlen(pList) > 0))
  {
    LPSTR d = pList;
    int slen = (int)strlen(itemOID);

    while(d != NULL)
    {
      LPSTR e = strchr(d, DA_COMMA_SEPARATOR);
      int len = 0;

      if (e != NULL)
        len = (int) (e - d);
      else
        len = (int) strlen(d);

      if ((len == slen) && (strncmp(d, itemOID, len) == 0))
        return TRUE;

      if (e != NULL)
        d = e + 1;
      else
        d = NULL;
    }
  }
  return FALSE;
}

BOOL existValueString(LPSTR pList, LPCTSTR itemOID)
{
  if ((pList != NULL) && (strlen(pList) > 0))
  {
    char oid[MAX_OID];
    ToUtf8String(oid, MAX_OID, itemOID, lstrlen(itemOID));

    return existValueString(pList, oid);
  }
  return FALSE;
}

void appendValueString(LPSTR* pList, int& nListSize, LPCTSTR itemOID)
{
  if ((*pList == NULL) ||
    (((int)strlen(*pList) + MAX_OID + 1) > nListSize))
  {
    size_t size = nListSize + 1024;
    char* n = new char[size];
    if ((*pList != NULL) && (strlen(*pList) > 0))
    {
      strcpy_s(n, size, *pList);
      delete[] *pList;
    }
    else
    {
      n[0] = '\0';
    }
    *pList = n;
    nListSize = (int) size;
  }
  size_t n = strlen(*pList);
  if (n > 0)
  {
    (*pList)[n++] = DA_COMMA_SEPARATOR; //DA_VALUE_SEPARATOR; : cannot use '|' on http url parameter
    (*pList)[n] = '\0';
  }
  ToUtf8String(*pList + n, MAX_OID, itemOID, lstrlen(itemOID));
}

#ifdef USE_BACKUP

BOOL KSyncRootFolderItem::RegistDeleteOnServer(KSyncItem* item)
{
  // mDeleteOnServerStack.push_back(item);
  if (item->IsFolder())
  {
    if (!existValueString(mDOS_FolderList, item->mServerOID))
      appendValueString(&mDOS_FolderList, mDOS_FolderListSize, item->mServerOID);
  }
  else
  {
    if (!existValueString(mDOS_FileList, item->mServerOID))
      appendValueString(&mDOS_FileList, mDOS_FileListSize, item->mServerOID);
  }
  return TRUE;
}

int KSyncRootFolderItem::RequestDeleteOnServerItemList()
{
  /*
  int file_count = 0;
  int folder_count = 0;

  KVoidPtrIterator it = mDeleteOnServerStack.begin();
  while(it != mDeleteOnServerStack.end())
  {
    KSyncItem* item = (KSyncItem*) *it++;
    if (item->IsFolder())
      ++folder_count;
    else
      ++file_count;
  }

  int folder_buff_size = 0;
  int file_buff_size = 0;
  LPSTR pFolderList = NULL;
  LPSTR pFileList = NULL;

  if (folder_count > 0)
  {
    folder_buff_size = folder_count * MAX_OID + 10;
    pFolderList = new char[folder_buff_size];
    pFolderList[0] = '\0';
  }
  if (file_count > 0)
  {
    file_buff_size = file_count * MAX_OID + 10;
    pFileList = new char[file_buff_size];
    pFileList[0] = '\0';
  }

  it = mDeleteOnServerStack.begin();
  char oid[MAX_OID];
  while(it != mDeleteOnServerStack.end())
  {
    KSyncItem* item = (KSyncItem*) *it;
    
    ToUtf8String(oid, MAX_OID, item->mServerOID, lstrlen(item->mServerOID));
    if (item->IsFolder())
    {
      if (strlen(pFolderList) > 0)
        kstrcatChar(pFolderList, folder_buff_size, DA_VALUE_SEPARATOR);
        //strcat_s(pFolderList, folder_buff_size, "|");

      strcat_s(pFolderList, folder_buff_size, oid);
    }
    else
    {
      if (strlen(pFileList) > 0)
        kstrcatChar(pFileList, file_buff_size, DA_VALUE_SEPARATOR);
        // strcat_s(pFileList, file_buff_size, "|");

      strcat_s(pFileList, file_buff_size, oid);
    }
    it = mDeleteOnServerStack.erase(it);
  }

  if (gpCoreEngine->RequestRemoveFiles(this, pFolderList, pFileList) == R_SUCCESS)
  {
    return (folder_count + file_count);
  }
  else
  {
    delete[] pFolderList;
    delete[] pFileList;

    SetDeleteOnServerItemsFail(FILE_SYNC_FAIL_REQUEST_DELETE);
    return 0;
  }
  */
  int len = 0;
  if(mDOS_FolderList != NULL)
    len += (strlen(mDOS_FolderList) + 1) / MAX_OID;
  if(mDOS_FileList != NULL)
    len += (strlen(mDOS_FileList) + 1) / MAX_OID;

  if (len > 0)
  {
    if (gpCoreEngine->RequestRemoveFiles(this, mDOS_FolderList, mDOS_FileList) == R_SUCCESS)
    {
      return len;
    }
    else
    {
      SetDeleteOnServerItemsFail(FILE_SYNC_FAIL_REQUEST_DELETE);
    }
  }
  return 0;
}

void KSyncRootFolderItem::OnDeleteOnServerItem(int isFolder, LPSTR data)
{
  LPSTR p = data;
  TCHAR serverOID[MAX_OID];

  while(p != NULL)
  {
    LPSTR e = strchr(p, DA_VALUE_SEPARATOR);
    if (e != NULL)
      *e = '\0';

    LPSTR v = strchr(p, DA_RESULT_SEPARATOR);
    if (v != NULL)
    {
      *v = '\0';
      v++;
    }

    if ((strlen(p) == 11) && (v != NULL))
    {
      BOOL r = FALSE;
      ToUnicodeString(serverOID, MAX_OID, p, strlen(p));

      if (v[0] == 'S')
      {
        r = ClearDeleteOnServerItemAsDone(mDOS_FileList, p);
        if (!r)
          r = ClearDeleteOnServerItemAsDone(mDOS_FolderList, p);

        if (!r)
        {
          OutputDebugString(_T("Cannot found ClearDeleteOnServerItemAsDone : "));
          OutputDebugString(serverOID);
          OutputDebugString(_T("\n"));
        }
      }
      else if (v[0] == 'P') // passed, already removed
      {
        r = 1;
      }

      KSyncItem* item = FindChildItemByOIDCascade(serverOID);
      if (item == NULL)
      {
        TCHAR msg[256] = _T("Unknown OID : ");
        StringCchCat(msg, 256, serverOID);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      else
      {
        LPTSTR pError = NULL;
        if (!r)
        {
          if (v[0] == 'F')
          {
            v++;
            pError = MakeUnicodeString(v);
          }
        }
        item->OnDeleteServerItem(r, pError);

        if (!r)
        {
          TCHAR msg[256] = _T("requestFileRemove OID : ");
          StringCchCat(msg, 256, serverOID);
          StringCchCat(msg, 256, _T(", Fail"));
          if (pError != NULL)
          {
            StringCchCat(msg, 256, _T(", Error : "));
            StringCchCat(msg, 256, pError);
          }
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
      }
    }
    else
    {
      TCHAR msg[256] = _T("No result code : ");
      LPTSTR str = MakeUnicodeString(p);
      if (str != NULL)
      {
        StringCchCat(msg, 256, str);
        delete[] str;
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    if (e != NULL)
      p = e + 1;
    else
      p = NULL;
  }
}

void KSyncRootFolderItem::OnRequestDeleteOnServerItemList(LPSTR data, long length)
{
  if ((data != NULL) && (length > 0))
  {
#if 0
    // success item
    LPSTR p = data;
    while(p != NULL)
    {
      LPSTR e = strchr(data, DA_PARAMETER_SEPARATOR);
      if (e != NULL)
        *e = '\0';

      LPSTR d = strchr(data, DA_NAME_SEPARATOR);
      if (d != NULL)
      {
        *d = '\0';
        if (strcmp(data, DA_NAME_DOCUMENT_OID) == 0)
        {
          OnDeleteOnServerItem(0, d+1);
        }
        else if (strcmp(data, DA_NAME_FOLDER_OID) == 0)
        {
          OnDeleteOnServerItem(1, d+1);
        }
        else
        {
          TCHAR msg[256] = _T("Unknown name : ");
          LPTSTR t_data = MakeUnicodeString(data);
          if (t_data != NULL)
            StringCchCat(msg, 256, t_data);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
      }

      if (e != NULL)
        p = e + 1;
      else
        break;
    }
#else
    // 파일 폴더 구분없이 OID:결과|OID:결과
    LPSTR e = strchr(data, DA_PARAMETER_SEPARATOR);
    if (e != NULL)
      *e = '\0';
    OnDeleteOnServerItem(0, data);
#endif
  }

  // 삭제 요청 결과를 모두 처리하였음.
  mChildItemIndex = 2;
  // remaining items are fail to delete
  // 모든 항목의 결과가 S,F로 나오니 다시 설정할 필요 없음.
  // SetDeleteOnServerItemsFail(FILE_SYNC_FAIL_REQUEST_DELETE);
}

void KSyncRootFolderItem::SetDeleteOnServerItemResult(LPSTR p)
{
  TCHAR itemOID[MAX_OID];
  int len;
  while (p != NULL)
  {
    LPSTR e = strchr(p, DA_VALUE_SEPARATOR);
    if (e != NULL)
      len = e - p;
    else
      len = strlen(p);
    ToUnicodeString(itemOID, MAX_OID, p, e - p);
    if (itemOID[0] > ' ')
    {
      KSyncItem* item = (KSyncItem*) findItemByOID(itemOID);
      if (item == NULL)
      {
        item = LoadItemFromStorage(itemOID);
      }

      if (item != NULL)
      {
        TCHAR msg[400];
        StringCchPrintf(msg, 400, _T("requestFileRemove fail : "));
        LPTSTR pathName = item->AllocRelativePathName();
        StringCchCat(msg, 400, pathName);
        if (pathName != NULL)
          delete[] pathName;
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        item->SetResult(FILE_SYNC_FAIL_REQUEST_DELETE);
        item->SetConflictResult(FILE_SYNC_FAIL_REQUEST_DELETE);
      }
    }

    if (e != NULL)
      p = e + 1;
    else
      break;
  }
}

BOOL KSyncRootFolderItem::ClearDeleteOnServerItemAsDone(LPSTR list, LPSTR itemOID)
{
  if (list != NULL)
  {
    char* p = strstr(list, itemOID);
    if (p != NULL)
    {
      *p = ' '; // mark space as done
      return TRUE;
    }
  }
  return FALSE;
}

void KSyncRootFolderItem::SetDeleteOnServerItemsFail(int r)
{
  /*
  KVoidPtrIterator it = mDeleteOnServerStack.begin();
  while(it != mDeleteOnServerStack.end())
  {
    KSyncItem* item = (KSyncItem*) *it++;

    item->SetResult(FILE_SYNC_FAIL_REQUEST_DELETE);
    item->SetConflictResult(FILE_SYNC_FAIL_REQUEST_DELETE);
  }
  mDeleteOnServerStack.clear();
  */

  if (mDOS_FolderList != NULL)
  {
    SetDeleteOnServerItemResult(mDOS_FolderList);
    delete[] mDOS_FolderList;
    mDOS_FolderList = NULL;
  }
  if (mDOS_FileList != NULL)
  {
    SetDeleteOnServerItemResult(mDOS_FileList);
    delete[] mDOS_FileList;
    mDOS_FileList = NULL;
  }
}
#endif // USE_BACKUP

#if 0
// change relative path as available by using renamed.txt
BOOL KSyncRootFolderItem::GetValidLocalRenamePath(LPTSTR newRelativePath)
{
  TCHAR fullPath[KMAX_PATH];
  MakeFullPathName(fullPath, KMAX_PATH, mBaseFolder, newRelativePath);

  if (!IsDirectoryExist(fullPath))
  {
    TCHAR manage_file[KMAX_PATH];
    GetRenameManageFilename(manage_file);
    KRenameStorage sitem(manage_file);

    TCHAR orgPath[KMAX_PATH];
    if (sitem.FindOriginalName(orgPath, newRelativePath, TRUE))
    {
      StringCchCopy(newRelativePath, KMAX_PATH, orgPath);
      return TRUE;
    }
  }
  return FALSE;
}
#endif

