#include "StdAfx.h"
#include <tchar.h>
#include "KSyncCoreBase.h"
#include "KSettings.h"

#include "../Platform/Reg.h"
#include "../Platform/Platform.h"

#include "DestinyLocalSyncLib/common/DestinyLocalSyncIFShared.h"

#include <Strsafe.h>
#include <Shlwapi.h>
#include <WinDef.h>

#ifndef BOOL
#define BOOL int
#endif

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

#ifdef FOR_DEBUG
#error "개발 진행중이니 릴리즈하지 마세요."
#endif

KSyncCoreBase* gpCoreEngine = NULL;

LPTSTR str_permission_err[] = {
    _T("No permission to access ECM Disk"),
    _T("No permission to access Security Disk"),
    _T("No permission to access Local Disk")
};

KSyncCoreBase::KSyncCoreBase(void)
{
  mStatus = SYNC_STATUS_INIT;
  mUserFlag = 0;
  mSecurityDrive = 0;
  mEcmDrive = 0;
  mEcmVirtualDrive = 0;

  mUseLocalSync = 0;
  mMaxSyncFileSizeMB = 0;

  if (gpCoreEngine == NULL)
    gpCoreEngine = this;
}

KSyncCoreBase::~KSyncCoreBase(void)
{
  if (gpCoreEngine == this)
    gpCoreEngine = NULL;
}

BOOL KSyncCoreBase::Ready(BOOL asMain)
{
  mUserFlag = 0;
  mSecurityDrive = 0;
  mUserOid[0] = '\0';
  mHomeFolder[0] = '\0';
  return TRUE;
}

BOOL KSyncCoreBase::IsSyncAllowed()
{
#ifdef USE_BACKUP
  if (mUseAutoSync == 0) // 0 : not available, 1 : available
    return FALSE;
#endif
  return TRUE;
}

BOOL KSyncCoreBase::CheckLanguage()
{
  WORD locale = RegGetDestinyAgentLanguage(); // 0x409;
  if (locale != mResourceLanguage)
  {
    mResourceLanguage = locale;
    return TRUE;
  }
  return FALSE;
}

BOOL KSyncCoreBase::GetHomeFolder(BOOL createIfNotExist, BOOL force)
{
  if (force)
    mHomeFolder[0] = 0;
  if (mHomeFolder[0] == 0)
  {
    TCHAR baseFolder[MAX_PATH];
    StringCchCopy(baseFolder, KMAX_PATH, DESTINY_HOME_FOLDER);

    if (!IsDirectoryExist(baseFolder) && createIfNotExist)
      CreateDirectory(baseFolder);

    if (IsDirectoryExist(baseFolder))
    {
      if (lstrlen(mUserOid) > 0)
      {
        MakeFullPathName(mHomeFolder, KMAX_PATH, baseFolder, mUserOid);
        if (IsDirectoryExist(mHomeFolder))
          return TRUE;
        if (createIfNotExist && CreateDirectory(mHomeFolder))
          return TRUE;
      }
    }
  }
  return (mHomeFolder[0] != 0);
}

BOOL KSyncCoreBase::IsECMAgentReady()
{
  BOOL bAlive = FALSE;

  HANDLE hMutexForPipe = CreateMutex(NULL, FALSE, mutexNameECMAgent);
  if (hMutexForPipe)
  {
    bAlive = (GetLastError() == ERROR_ALREADY_EXISTS);

    ReleaseMutex(hMutexForPipe);
    CloseHandle(hMutexForPipe);
  }
  return bAlive;
}

BOOL KSyncCoreBase::GetSecurityDisk(LPTSTR pathname)
{
  if (mSecurityDrive > 'A')
  {
    pathname[0] = mSecurityDrive;
    pathname[1] = ':';
    pathname[2] = '\0';
    return IsDirectoryExist(pathname);
  }
  return FALSE;
}

BOOL KSyncCoreBase::IsSecurityDisk(LPCTSTR pathname)
{
  if ((pathname != NULL) && (lstrlen(pathname) > 2))
    return((mSecurityDrive == toupper(pathname[0])) && (pathname[1] == ':'));
  return FALSE;
}

BOOL KSyncCoreBase::IsSecurityDiskReady()
{
  if (mSecurityDrive > 'A')
  {
    TCHAR path[4] = _T("A:\\");
    path[0] = mSecurityDrive;
    return IsDirectoryExist(path);
  }
  return FALSE;
}

typedef std::vector<TCHAR *> KTStringArray;
typedef std::vector<TCHAR *>::iterator KTStringIterator;

static void appendByOrder(KTStringArray &file_list, TCHAR *in_file)
{
  KTStringIterator it = file_list.begin();
  while(it != file_list.end())
  {
    TCHAR *item_file = *it;
    if (CompareStringICNC(item_file, in_file) < 0)
    {
      file_list.insert(it, in_file);
      return;
    }
    ++it;
  }
  file_list.insert(it, in_file);
}

static void clearKTStringArray(KTStringArray &file_list)
{
  KTStringIterator it = file_list.begin();
  while(it != file_list.end())
  {
    TCHAR *item_file = *it;
    delete[] item_file;
    ++it;
  }
  file_list.clear();
}

static void insertItemArrayByTime(KSyncItemArray &itemArray, KSyncItem *new_item)
{
  /** do not sort
  KSyncItemIterator it = itemArray.begin();
  while(it != itemArray.end())
  {
    KSyncItem *item_file = *it;
    if (CompareSystemTime(item_file->mLocal.FileTime, new_item->mLocal.FileTime) > 0)
    {
      itemArray.insert(it, new_item);
      return;
    }
    ++it;
  }
  itemArray.insert(it, new_item);
  */
  
  // itemArray.push_back(new_item);
  itemArray.insert(itemArray.begin(), new_item);
}


int KSyncCoreBase::GetSyncAvailabilityByDrivePermission(LPCTSTR baseFolder)
{
  int sync_flag = 0;

  if (mUserFlag & FLAG_RW_ECM_DISK)
  {
    if (mUserFlag & FLAG_WRITABLE_ECM_DISK) // server is writable
      sync_flag |= SYNC_ENABLE_UPLOAD;

    if (baseFolder[0] == '\\') // for all system drive
    {
      if (mUserFlag & FLAG_WRITABLE_NORMAL_DISK)
        sync_flag |= SYNC_ENABLE_DOWNLOAD;
    }
    else if (baseFolder[1] == ':')
    {
      if (IsSecurityDisk(baseFolder))
      {
        // 20150508 새 드라이브 보안정책
        // 보안드라이브 접근불가이면 업로드/다운로드 불가
        if ((mUserFlag & FLAG_RW_SECURITY_DISK) == 0)
          sync_flag = 0;

        else if (mUserFlag & FLAG_WRITABLE_SECURITY_DISK)
          sync_flag |= SYNC_ENABLE_DOWNLOAD;
      }
      else
      {
        // 20150508 새 드라이브 보안정책
        // 로컬드라이브 접근불가이어도 업로드 가능.
        if (mUserFlag & FLAG_WRITABLE_NORMAL_DISK)
          sync_flag |= SYNC_ENABLE_DOWNLOAD;
      }
    }
  }
  return sync_flag;
}

