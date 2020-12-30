/**
* @file KSyncItem.cpp
* @date 2015.02
* @brief KSyncItem class cpp file
*/
#include "StdAfx.h"
#include <tchar.h>
#include "KSyncItem.h"
#include "KSyncPolicy.h"
#include "KSyncCoreBase.h"

#ifdef USE_SYNC_ENGINE
#include "KSyncCoreEngine.h"
#endif
#include "KConstant.h"
#include "KSettings.h"
#include "KSqliteUtil.h"

#ifdef USE_SYNC_STORAGE
#include "KSyncRootStorageFile.h"
#endif

#include "Platform/Platform.h"

#include <strsafe.h>
#include <Shlwapi.h>

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

// #define _TEST_CONFLICT_

LPCTSTR gRootPathDisplayString = _T("\\");

void ClearLocalInfo(LOCAL_INFO &local)
{
  memset(&local, 0, sizeof(local));
  local.FileSize = -1;
}

void ClearServerInfo(SERVER_INFO &server)
{
  memset(&server, 0, sizeof(server));
  server.FileSize = -1;
  server.Permissions = -1;
}

void FreeLocalInfo(LOCAL_INFO &local)
{
  if (local.pFilename != NULL)
  {
    delete[] local.pFilename;
    local.pFilename = NULL;
  }
}

void FreeServerInfo(SERVER_INFO &dst)
{
  if (dst.pFullPathIndex != NULL)
  {
    delete[] dst.pFullPathIndex;
    dst.pFullPathIndex = NULL;
  }
  if (dst.pFullPath != NULL)
  {
    delete[] dst.pFullPath;
    dst.pFullPath = NULL;
  }
  if (dst.pFilename != NULL)
  {
    delete[] dst.pFilename;
    dst.pFilename = NULL;
  }
  ClearServerInfo(dst);
}

void MergeLocalInfo(LOCAL_INFO &dst, LOCAL_INFO &src)
{
  if ((src.pFilename != NULL) && (lstrlen(src.pFilename) > 0))
  {
    AllocCopyString(&dst.pFilename, src.pFilename);
  }
  /*
  if (lstrlen(src.FullPath) > 0)
    StringCchCopy(dst.FullPath, KMAX_PATH, src.FullPath);
    */
  if (lstrlen(src.UserOID) > 0)
    StringCchCopy(dst.UserOID, KMAX_PATH, src.UserOID);
  if (src.FileSize >= 0)
    dst.FileSize = src.FileSize;

  if (IsValidSystemTime(src.FileTime))
    dst.FileTime = src.FileTime;
  if (IsValidSystemTime(src.AccessTime))
    dst.AccessTime = src.AccessTime;
}

void CopyLocalInfo(LOCAL_INFO &dst, LOCAL_INFO &src)
{
  FreeLocalInfo(dst);
  memcpy(&dst, &src, sizeof(LOCAL_INFO));
  dst.pFilename = AllocString(src.pFilename);
}

void CopyServerInfo(SERVER_INFO &dst, SERVER_INFO &src)
{
  FreeServerInfo(dst);

  memcpy(&dst, &src, sizeof(SERVER_INFO));
  dst.pFilename = AllocString(src.pFilename);
  dst.pFullPath = AllocString(src.pFullPath);
  dst.pFullPathIndex = AllocString(src.pFullPathIndex);
}

void MergeServerInfo(SERVER_INFO &dst, SERVER_INFO &src)
{
  if (lstrlen(src.FileOID) > 0)
    StringCchCopy(dst.FileOID, MAX_OID, src.FileOID);
  if (lstrlen(src.StorageOID) > 0)
    StringCchCopy(dst.StorageOID, MAX_STGOID, src.StorageOID);

  /*
  if (lstrlen(src.FullPath) > 0)
    StringCchCopy(dst.FullPath, KMAX_PATH, src.FullPath);
  if (lstrlen(src.FullPathIndex) > 0)
    StringCchCopy(dst.FullPathIndex, KMAX_PATH, src.FullPathIndex);
    */

  AllocCopyString(&dst.pFilename, src.pFilename);
  AllocCopyString(&dst.pFullPath, src.pFullPath);
  AllocCopyString(&dst.pFullPathIndex, src.pFullPathIndex);

  if (lstrlen(src.UserOID) > 0)
    StringCchCopy(dst.UserOID, MAX_OID, src.UserOID);
  if (src.FileSize >= 0)
    dst.FileSize = src.FileSize;
  if (IsValidSystemTime(src.FileTime))
    dst.FileTime = src.FileTime;
  if (src.Permissions >= 0)
    dst.Permissions = src.Permissions;
}

LPCTSTR GetDisplayPathName(LPCTSTR folder_path)
{
  return(((folder_path != NULL) && (lstrlen(folder_path) > 0)) ? folder_path : gRootPathDisplayString);
}

KSyncItem::KSyncItem(KSyncItem *p)
{
  init(p);
}

KSyncItem::KSyncItem(KSyncItem *p, LPCTSTR filename)
{
  init(p);

  SplitPathNameAlloc(filename, NULL, &mLocal.pFilename);
  // AllocCopyString(&mLocal.pFilename, filename);
  StringCchCopy(mLocal.UserOID, MAX_OID, _T("Unknown"));
}

KSyncItem::KSyncItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename)
{
  init(p);

  if (mpRelativePath != NULL)
    delete[] mpRelativePath;
  mpRelativePath = AllocPathName(pathname, filename);

  AllocCopyString(&mLocal.pFilename, filename);

  StringCchCopy(mLocal.UserOID, MAX_OID, _T("Unknown"));
  //mIsFolder = isFolder;
}

KSyncItem::KSyncItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid)
{
  init(p);

  AllocCopyString(&mLocal.pFilename, filename);

  mpRelativePath = AllocPathName(pathname, filename);

  StringCchCopy(mLocal.UserOID, MAX_OID, _T("Unknown"));

  if (server_oid != NULL)
    StringCchCopy(mServerOID, MAX_OID, server_oid);
  if (server_file_oid != NULL)
    StringCchCopy(mServer.FileOID, MAX_OID, server_file_oid);

  if (server_storage_oid != NULL)
    StringCchCopy(mServer.StorageOID, MAX_STGOID, server_storage_oid);
}

KSyncItem::~KSyncItem(void)
{
  FreeAllocated();
}

/**
* @brief 서버측의 권한 정보 문자열을 값으로 바꿀때 사용한다.
*/
static int permission_value_array[] = {
  SFP_LOOKUP,
  SFP_READ,
  SFP_WRITE,
  SFP_DELETE,
  SFP_SETUP,
  SFP_INSERT_CONTAINER,
  SFP_INSERT_ELEMENT,
  SFP_LOOKUP_ELEMENT,
  SFP_READ_ELEMENT,
  SFP_WRITE_ELEMENT,
  SFP_DELETE_ELEMENT,
  SFP_SETUP_ELEMENT,
  SFP_FULL_SETUP
};

/**
* @brief 서버측의 권한 정보 문자열을 값으로 바꿀때 사용한다.
*/
static LPTSTR permission_str_array[] = {
  _T("l"),
  _T("r"),
  _T("w"),
  _T("d"),
  _T("s"),
  _T("ic"),
  _T("ie"),
  _T("le"),
  _T("re"),
  _T("we"),
  _T("de"),
  _T("se"),
  _T("fs")
};

int KSyncItem::String2Permission(LPCTSTR permissions)
{
  LPCTSTR p = permissions;
  int len;
  int pv = 0;
  int count = sizeof(permission_str_array) / sizeof(LPTSTR);

  if(p == NULL)
    return 0;

  while(*p != '\0')
  {
    LPCTSTR n = StrChr(p, ',');
    if (n != NULL)
      len = (int)(n - p);
    else
      len = lstrlen(p);

    for (int i = 0; i < count; i++)
    {
      if (StrNCmpI(p, permission_str_array[i], len) == 0) // _tcsncmp
      {
        pv |= permission_value_array[i];
        break;
      }
    }
    p += len;
    if (*p == ',')
      p++;
  }
  return pv;
}

BOOL KSyncItem::CheckPermissionRootFolder(int permission)
{
  return (permission & SFP_LOOKUP);
}

BOOL KSyncItem::CheckPermissionSubFolder(int permission)
{
  return (permission & SFP_LOOKUP);
}

BOOL KSyncItem::CheckPermissionFolderUpload(int parent_permission, int permission)
{
  return (parent_permission & SFP_INSERT_CONTAINER);
}

BOOL KSyncItem::CheckPermissionFolderDownload(int parent_permission, int permission)
{
  return (parent_permission & SFP_LOOKUP);
}

BOOL KSyncItem::CheckPermissionFolderDelete(int parent_permission, int permission)
{
  return (permission & SFP_DELETE);
}

BOOL KSyncItem::CheckPermissionFolderMove(int src_parent_permission, int dest_parent_permission, int permission)
{
  return ((dest_parent_permission & SFP_INSERT_CONTAINER) && (permission & SFP_DELETE));
}

BOOL KSyncItem::CheckPermissionFolderRename(int parent_permission, int permission)
{
  return (permission & SFP_WRITE);
}

BOOL KSyncItem::CheckPermissionFileInsert(int parent_permission, int permission)
{
  return (parent_permission & SFP_INSERT_ELEMENT);
}

BOOL KSyncItem::CheckPermissionFileUpload(int parent_permission, int permission)
{
  return ((parent_permission & SFP_LOOKUP) && (permission & SFP_WRITE));
}

BOOL KSyncItem::CheckPermissionFileDownload(int parent_permission, int permission)
{
  return ((parent_permission & SFP_LOOKUP) && (permission & SFP_READ));
}

BOOL KSyncItem::CheckPermissionFileDelete(int parent_permission, int permission)
{
  return ((parent_permission & SFP_LOOKUP) && (permission & SFP_DELETE));
}

BOOL KSyncItem::CheckPermissionFileMove(int src_parent_permission, int dest_parent_permission, int permission)
{
  return ((dest_parent_permission & SFP_INSERT_ELEMENT) && (permission & SFP_DELETE));
}

BOOL KSyncItem::CheckPermissionFileRename(int parent_permission, int permission)
{
  return ((parent_permission & SFP_LOOKUP) && (permission & SFP_WRITE));
}

LPTSTR KSyncItem::allocServerSideRelativePath(KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName)
{
  LPCTSTR serverRootPath = ((KSyncRootFolderItem*)rootItem)->GetServerFullPath();
  return AllocServerSideRelativePath(serverRootPath, serverFullPath, fileName);
}

LPTSTR KSyncItem::AllocServerSideRelativePath(LPCTSTR serverRootPath, LPCTSTR serverFullPath, LPCTSTR fileName)
{
  LPTSTR relativePath = NULL;
  int serverRootLen = (serverRootPath != NULL) ? lstrlen(serverRootPath) : 0;
  if (serverRootLen > 0)
  {
    if ((serverRootLen < lstrlen(serverFullPath)) &&
      (StrCmpN(serverRootPath, serverFullPath, serverRootLen) == 0))
    {
      serverRootLen++;
      relativePath = AllocString(&serverFullPath[serverRootLen]);
      ReplaceChar(relativePath, '>', '\\');
    }
  }
  else
  {
    relativePath = AllocString(serverFullPath);
    ReplaceChar(relativePath, '>', '\\');
  }

  if (fileName == NULL)
    return relativePath;

  LPTSTR mergedPath = AllocPathName(relativePath, fileName);
  delete[] relativePath;
  return mergedPath;
}

/*
void KSyncItem::getServerSideFullPathName(LPTSTR fullName, int size, KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName)
{
  TCHAR relativePath[MAX_PATH] = { 0 };

  StringCchCopy(relativePath, MAX_PATH, GetServerRelativePath());

  LPCTSTR baseFolder = ((KSyncFolderItem *)rootItem)->GetBaseFolder();
  if (lstrlen(relativePath) > 0)
  {

    MakeFullPathName(fullName, KMAX_PATH, baseFolder, relativePath);
    MakeFullPathName(fullName, KMAX_PATH, fullName, fileName);
  }
  else
  {
    MakeFullPathName(fullName, KMAX_PATH, baseFolder, fileName);
  }
}

LPTSTR KSyncItem::allocServerSidePathName(BOOL bFull, KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName)
{
  LPTSTR relativePath = NULL;

  if (rootItem == NULL)
    rootItem = ((KSyncFolderItem *)mParent)->GetRootFolderItem();

  LPCTSTR serverRootPath = rootItem->GetServerFullPath();
  int serverRootLen = lstrlen(serverRootPath);
  if (serverRootLen > 0)
  {
    if ((serverRootLen < lstrlen(serverFullPath)) &&
      (StrCmpN(serverRootPath, serverFullPath, serverRootLen) == 0))
    {
      serverRootLen++;

      AllocCopyString(&relativePath, &serverFullPath[serverRootLen]);
      ReplaceChar(relativePath, '>', '\\');
    }
    else
    {
      OutputDebugString(_T("Bad server full path : "));
      OutputDebugString(serverFullPath);
      OutputDebugString(_T("\n"));
    }
  }

  LPCTSTR baseFolder = ((KSyncFolderItem *)rootItem)->GetBaseFolder();
  int len = lstrlen(relativePath) + lstrlen(fileName) + 5;
  if (bFull)
    len += lstrlen(baseFolder);
  LPTSTR fullName = new TCHAR[len];

  if (bFull)
    StringCchCopy(fullName, len, baseFolder);
  else
    fullName[0] = '\0';

  if ((relativePath != NULL) && (lstrlen(relativePath) > 0))
    MakeFullPathName(fullName, len, fullName, relativePath);

  MakeFullPathName(fullName, len, fullName, fileName);

  delete[] relativePath;
  return fullName;
}
*/

BOOL KSyncItem::isPathChanged(LPCTSTR oldpathname, LPCTSTR newpathname)
{
  LPCTSTR op = StrRChr(oldpathname, NULL, '\\');
  LPCTSTR np = StrRChr(newpathname, NULL, '\\');
  if ((op != NULL) && (np != NULL))
  {
    int olen = (int)(op - oldpathname);
    int nlen = (int)(np - newpathname);
    if ((olen != nlen) ||
      (StrCmpNI(oldpathname, newpathname, nlen) != 0))
      return TRUE;
    /*
    *op = '\0';
    *np = '\0';
    if (StrCmpI(oldpathname, newpathname) != 0)
      path_changed = TRUE;
    *op = '\\';
    *np = '\\';
    */
  }
  if ((op == NULL) != (np == NULL))
    return TRUE;
  return FALSE;
}

BOOL KSyncItem::isNameChanged(LPCTSTR oldpathname, LPCTSTR newpathname)
{
  LPCTSTR op = StrRChr(oldpathname, NULL, '\\');
  LPCTSTR np = StrRChr(newpathname, NULL, '\\');
  if (op == NULL)
    op = oldpathname;
  else
    ++op;
  if (np == NULL)
    np = newpathname;
  else
    ++np;

  return (StrCmpI(op, np) != 0);
}


KSyncItem &KSyncItem::operator=(KSyncItem &item)
{
  mParent = item.mParent;
  mNewParent = item.mNewParent;

  mFlag = item.mFlag;

  memset(&mLocal, 0, sizeof(mLocal));
  memset(&mLocalNew, 0, sizeof(mLocalNew));
  CopyLocalInfo(mLocal, item.mLocal);
  CopyLocalInfo(mLocalNew, item.mLocalNew);

  memset(&mServer, 0, sizeof(mServer));
  memset(&mServerNew, 0, sizeof(mServerNew));
  CopyServerInfo(mServer, item.mServer);
  CopyServerInfo(mServerNew, item.mServerNew);

  memcpy(&mServerOID, &item.mServerOID, sizeof(mServerOID));
  mpServerConflictMessage = AllocString(item.mpServerConflictMessage);

  mpLocalRenamedAs = AllocString(item.mpLocalRenamedAs);
  mpServerRenamedAs = AllocString(item.mpServerRenamedAs);
  mpRelativePath = AllocString(item.mpRelativePath);

  /*
  memcpy(mMetafilename, item.mMetafilename, sizeof(mMetafilename));
  memcpy(mRelativeName, item.mRelativeName, sizeof(mRelativeName));
  memcpy(mLocalRenamedAs, item.mLocalRenamedAs, sizeof(mLocalRenamedAs));
  memcpy(mServerRenamedAs, item.mServerRenamedAs, sizeof(mServerRenamedAs));
  */

  memcpy(mServerMoveToOID, item.mServerMoveToOID, sizeof(mServerMoveToOID));

  mpServerTempRenamed = AllocString(item.mpServerTempRenamed);
  mpLocalTempRenamed = AllocString(item.mpLocalTempRenamed);

  mResult = item.mResult;
  mChildResult = item.mChildResult;

  mLocalState = item.mLocalState;
  mServerState = item.mServerState;
  mConflictResult = item.mConflictResult;

  mPhase = item.mPhase;
  mEnable = item.mEnable;

  return *this;
}

KSyncItem *KSyncItem::DuplicateItem()
{
  KSyncItem *nitem = new KSyncItem(mParent);
  *nitem = *this;
  return nitem;
}

void KSyncItem::init(KSyncItem *parent)
{
  mParent = parent;
  mNewParent = NULL;
  // mStoreFlag = 0;

  ClearLocalInfo(mLocal);
  ClearLocalInfo(mLocalNew);

  mFlag = 0;
  mResolveFlag = 0;

  ClearServerInfo(mServer);
  ClearServerInfo(mServerNew);

  mServerOID[0] = '\0';
  mpServerConflictMessage = NULL;

  mpRelativePath = NULL;
  mpLocalRenamedAs = NULL; // local file was renamed by user
  mpServerRenamedAs = NULL; // server file was renamed by user
  mpMetafilename = NULL;

  mServerMoveToOID[0] = '\0';
  mpServerTempRenamed = NULL;
  mpLocalTempRenamed = NULL;

  mResult = 0;
  mChildResult = 0;
  mLocalState = ITEM_STATE_NONE;
  mServerState = ITEM_STATE_NONE;
  mConflictResult = 0;

  mServerScanState = ITEM_NO_CHANGE;

  mPhase = 0;
  mEnable = TRUE;
  mpOverridePath = NULL;
}

void KSyncItem::FreeAllocated()
{
  if (mpRelativePath != NULL)
  {
    delete[] mpRelativePath;
    mpRelativePath = NULL;
  }
  if (mpMetafilename != NULL)
  {
    delete[] mpMetafilename;
    mpMetafilename = NULL;
  }
  clearRenameInfo();

  FreeLocalInfo(mLocal);
  FreeLocalInfo(mLocalNew);
  FreeServerInfo(mServer);
  FreeServerInfo(mServerNew);

  if (mpServerTempRenamed != NULL)
  {
    delete[] mpServerTempRenamed;
    mpServerTempRenamed = NULL;
  }
  if (mpLocalTempRenamed != NULL)
  {
    delete[] mpLocalTempRenamed;
    mpLocalTempRenamed = NULL;
  }
  if (mpServerConflictMessage != NULL)
  {
    delete[] mpServerConflictMessage;
    mpServerConflictMessage = NULL;
  }
  if (mpOverridePath != NULL)
  {
    delete[] mpOverridePath;
    mpOverridePath = NULL;
  }
}

void FreeSyncItemList(KSyncItemArray &array)
{
  while(array.size() > 0)
  {
    KSyncItem *item = array.back();
    delete item;
    array.pop_back();
  }
}

void FreeSyncItemListTop(KSyncItemArray &array, int maxCount, BOOL top)
{
  if (top)
  {
    KSyncItemIterator it = array.begin();
    while((int)array.size() > maxCount)
    {
      KSyncItem *item = *it;
      it = array.erase(it);
      delete item;
    }
  }
  else
  {
    while((int)array.size() > maxCount)
    {
      KSyncItem *item = array.back();
      array.pop_back();
      delete item;
    }
  }
}

/**
* @brief 자신 또는 부 폴더가 삭제 요청이 처리되었는지 확인함.
*/
BOOL KSyncItem::IsParentAlreadyRegistDeleteOnServer()
{
  if (mFlag & SYNC_ITEM_REQUEST_DELET_ONSERVER)
    return TRUE;
  if (mParent != NULL)
    return mParent->IsParentAlreadyRegistDeleteOnServer();
  return FALSE;
}

BOOL KSyncItem::IsNeedSyncPhase(int phase)
{
#ifdef USE_BACKUP
  if (IsSystemRoot() && (phase >= PHASE_COMPARE_FILES))
    return FALSE;
#else
  if ((IsFolder() == 2) && (phase >= PHASE_CREATE_FOLDERS))
    return FALSE;
#endif

  if (IsExcludedSync())
    return FALSE;

  if (phase == PHASE_CREATE_FOLDERS)
  {
#ifdef USE_BACKUP
    if(
      (mLocalState & ITEM_STATE_NEWLY_ADDED) ||
      (mServerState & ITEM_STATE_NEWLY_ADDED) ||
      ((mLocalState == ITEM_STATE_EXISTING) && (mServerState & ITEM_STATE_DELETED)) ||
      /* 로컬삭제에 의한 서버 삭제는 이름변경("~#*.*")으로 처리되니 여기서 처리하지 않음. => 상위 폴더만 처리 */
      ((mLocalState & ITEM_STATE_DELETED) && (mServerState == ITEM_STATE_EXISTING)) )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
#else
    return ((mLocalState & ITEM_STATE_NEWLY_ADDED) || (mServerState & ITEM_STATE_NEWLY_ADDED));
#endif
  }
  if (phase == PHASE_MOVE_FOLDERS)
  {
    // ITEM_STATE_REFRESH_PATH : 상위 폴더 경로가 바뀌어 이 개체의 경로도 업데이트 하여야 함.
    return ((mLocalState & (ITEM_STATE_MOVED|ITEM_STATE_REFRESH_PATH)) || (mServerState & ITEM_STATE_MOVED));
  }
  if (phase == PHASE_COPY_FILES)
  {
    return (mResult != 0);
  }
  return TRUE;
}

__int64 KSyncItem::GetSyncFileSize(BOOL conflictResult)
{
  __int64 size = 0;
  int result = conflictResult ? mConflictResult : mResult;
  if (result & FILE_NEED_DOWNLOAD)
    size = GetServerFileSize();
  else if (result & FILE_NEED_UPLOAD)
    size = GetLocalFileSize();
  else // if (result & FILE_CONFLICT)
  {
    __int64 sever_size = (mServerNew.FileSize >= 0) ? mServerNew.FileSize : mServer.FileSize;
    __int64 local_size = (mLocalNew.FileSize >= 0) ? mLocalNew.FileSize : mLocal.FileSize;
    size = ((local_size > sever_size) ? local_size : sever_size);
  }
  // to calculate progress-bar
  return size;
}


BOOL KSyncItem::ParseDateString(char *str)
{
  char *s = strchr(str, ' ');
  if (s != NULL)
  {
    int y, m, d;

    *s = '\0';
    if (sscanf_s(str, "%d-%d-%d", &y, &m, &d) == 3)
    {
      mLocal.FileTime.wYear = (WORD)y;
      mLocal.FileTime.wMonth = (WORD)m;
      mLocal.FileTime.wDay = (WORD)d;

      int hh, mm, ss, ms;
      str = s + 1;
      if (sscanf_s(str, "%d:%d:%d:%d", &hh, &mm, &ss, &ms) == 4)
      {
        mLocal.FileTime.wHour = (WORD)hh;
        mLocal.FileTime.wMinute = (WORD)mm;
        mLocal.FileTime.wSecond = (WORD)ss;
        mLocal.FileTime.wMilliseconds = (WORD)ms;
        return TRUE;
      }
    }
  }
  return FALSE;
}

BOOL KSyncItem::ParseHistoryString(char *str)
{
  char *s = strchr(str, '=');
  if (s != NULL)
  {
    *s = '\0';
    s++;
    /*
    if (strcmp(str, "path") == 0)
      ToUnicodeString(mLocal.FullPath, KMAX_PATH, s, (int)strlen(s));
    */
    if (strcmp(str, "name") == 0)
    {
      if (mLocal.pFilename != NULL)
        delete[] mLocal.pFilename;
      mLocal.pFilename = MakeUnicodeString(s, (int)strlen(s));
      //ToUnicodeString(mLocal.Filename, KMAX_PATH, s, (int)strlen(s));
    }
    else if (strcmp(str, "user") == 0)
    {
      ToUnicodeString(mLocal.UserOID, MAX_OID, s, (int)strlen(s));
    }
    else
      return FALSE;
  }
  else
  {
    if (strcmp(str, "upload_done") == 0)
      mResult = FILE_NEED_UPLOAD;
    else if (strcmp(str, "download_done") == 0)
      mResult = FILE_NEED_DOWNLOAD;
    else
      return FALSE;
  }
  return TRUE;
}

LPCTSTR KSyncItem::GetPath()
{
  return mpRelativePath;
  /*
  if (mParent != NULL)
    return ((KSyncFolderItem*)mParent)->GetBaseFolder();
  else if (mpRelativePath != NULL)
    return mpRelativePath;
  return NULL;
  */
}

LPCTSTR KSyncItem::GetPathString()
{
  LPCTSTR str = GetPath();
  if ((str == NULL) || (lstrlen(str) == 0))
    return gRootPathDisplayString;
  return str;
}

LPCTSTR KSyncItem::GetFilename()
{
  return ((lstrlen(mLocal.pFilename) > 0) ? mLocal.pFilename : mServer.pFilename);
}

SYSTEMTIME *KSyncItem::GetLocalFileTime()
{
  return (IsValidSystemTime(mLocalNew.FileTime) ? &mLocalNew.FileTime : &mLocal.FileTime);
}

void KSyncItem::SetRelativePathName(LPCTSTR pathname)
{
#if defined(_DEBUG) && defined(USE_SYNC_GUI)
  if (IsFolder() != 2)
    ASSERT(pathname[1] != ':');
#endif

  if (mpRelativePath != NULL)
  {
    if ((pathname != NULL) && 
      (StrCmpI(pathname, mpRelativePath) == 0))
      return;
    delete[] mpRelativePath;
    mpRelativePath = NULL;
  }
  if (pathname != NULL)
  {
    int len = lstrlen(pathname) + 1;
    mpRelativePath = new TCHAR[len];
    StringCchCopy(mpRelativePath, len, pathname);
  }
}

void KSyncItem::ClearRelativePathName(int flag)
{
  if (mpRelativePath != NULL)
  {
    delete[] mpRelativePath;
    mpRelativePath;
  }
}

void KSyncItem::UpdateRelativePath()
{
  int len = 0;
  if (mLocal.pFilename != NULL)
    len += lstrlen(mLocal.pFilename) + 1;
  LPCTSTR baseFolder = ((KSyncFolderItem *)mParent)->GetBaseFolder();
  if (baseFolder != NULL)
  {
    // it should be relative path
#if defined(_DEBUG) && defined(USE_SYNC_GUI)
    ASSERT(baseFolder[1] != ':');
#endif
    len += lstrlen(baseFolder) + 1;
  }
  if (mpRelativePath != NULL)
    delete[] mpRelativePath;
  mpRelativePath = new TCHAR[len];
  MakeFullPathName(mpRelativePath, len, baseFolder, mLocal.pFilename);
}

void KSyncItem::ChangeRelativePathName(KSyncRootFolderItem* rootFolder, LPCTSTR relativePathName, BOOL moveInTree)
{
  LPTSTR path = new TCHAR[lstrlen(relativePathName)];
  TCHAR name[KMAX_PATH];
  SplitPathName(relativePathName, path, name);

  AllocCopyString(&mLocal.pFilename, name);
  AllocCopyString(&mServer.pFilename, name);

  LPTSTR currentPath;
  if (mpRelativePath != NULL)
  {
    currentPath = new TCHAR[lstrlen(mpRelativePath)];
    SplitPathName(mpRelativePath, currentPath, NULL);
  }
  else
  {
    LPTSTR relativePath = AllocRelativePathName();
    currentPath = new TCHAR[lstrlen(relativePath)];
    SplitPathName(relativePath, currentPath, NULL);
    delete[] relativePath;
  }

  if (moveInTree && (CompareRelativePath(currentPath, path) != 0))
  {
    rootFolder->MoveSyncItem(this, path, FALSE);
  }

  SetRelativePathName(relativePathName);

  delete[] path;
  delete[] currentPath;

  if (moveInTree)
    OnPathChanged();
}

LPCTSTR KSyncItem::GetServerFileOID()
{
  if (lstrlen(mServerNew.FileOID) > 0)
    return mServerNew.FileOID;
  return mServer.FileOID;
}

LPCTSTR KSyncItem::GetServerStorageOID()
{
  if (lstrlen(mServerNew.StorageOID) > 0)
    return mServerNew.StorageOID;
  return mServer.StorageOID;
}

LPCTSTR KSyncItem::GetServerRelativePath()
{
  if ((mServerNew.pFullPath != NULL) && (lstrlen(mServerNew.pFullPath) > 0))
    return mServerNew.pFullPath;
  return mServer.pFullPath;
}

LPCTSTR KSyncItem::GetServerFullPathIndex()
{
  if ((mServerNew.pFullPathIndex != NULL) && (lstrlen(mServerNew.pFullPathIndex) > 0))
    return mServerNew.pFullPathIndex;
  return mServer.pFullPathIndex;
}

LPTSTR KSyncItem::AllocServerRelativePathName()
{
  LPTSTR pathName = NULL;

  if (IsFolder() != 2)
  {
    LPCTSTR path = GetServerRelativePath();
    pathName = AllocString(path);
  }
  else
  {
    pathName = new TCHAR[2];
    pathName[0] = '\\';
    pathName[1] = '\0';
  }
  return pathName;
}

void KSyncItem::SetLocalPath(LPCTSTR baseFolder, LPCTSTR rootFolder, int depth)
{
  if (mpRelativePath != NULL)
    delete[] mpRelativePath;
  mpRelativePath = AllocPathName(baseFolder, mLocal.pFilename);
}

BOOL KSyncItem::RefreshLocalFileInfo(LPCTSTR user)
{
  if (IsSystemRoot())
    return TRUE;

  LPTSTR fullname = AllocFullPathName();
  BOOL r = RefreshLocalFileInfo(fullname, user);
  delete[] fullname;
  return r;
}

BOOL KSyncItem::RefreshLocalFileInfo(LPCTSTR fullname, LPCTSTR user)
{
  if(user != NULL)
    StringCchCopy(mLocal.UserOID, MAX_OID, user);

  mLocal.FileSize = IsFolder() ? 0 : GetFileSize(fullname);
  return GetFileModifiedTime(fullname, &mLocal.FileTime, &mLocal.AccessTime);
}

BOOL KSyncItem::RefreshLocalNewFileInfo(LPCTSTR user, KSyncItem* rootItem)
{
  StringCchCopy(mLocalNew.UserOID, MAX_OID, user);

  // refresh local rename info 
  // 상위 폴더가 이동 처리 완료된 상태에서 호출되면 이 부분은 비정상 동작할 수 있음.
  if (CheckLocalRenamedAsByOld(rootItem))
  {
    int len = 50 + lstrlen(mpLocalRenamedAs);
    LPTSTR msg = new TCHAR[len];
    StringCchCopy(msg, len, _T("Refresh LocalRenamed : "));
    StringCchCat(msg, len, mpLocalRenamedAs);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    delete[] msg;
  }

  LPTSTR fullname = NULL;

  if (mParent != NULL)
  {
    LPTSTR actualPath = mParent->AllocActualPathName(TRUE);
    // parent pathname with renamedAs

    fullname = AllocPathName(actualPath, mLocal.pFilename);

    if (!IsItemExist(fullname) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
    {
      delete[] fullname;
      fullname = AllocAbsolutePathName(mpLocalRenamedAs);

      if (!IsItemExist(fullname))
      {
        TCHAR name[KMAX_FILENAME];
        SplitPathName(mpLocalRenamedAs, NULL, name);
        MakeFullPathName(fullname, KMAX_PATH, actualPath, name /*mLocal.Filename*/);
      }
    }

    LPTSTR relativePathName = AllocRelativePathName();
    if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0) &&
      (StrCmpI(relativePathName, mpLocalRenamedAs) == 0))
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
    else if (IsItemExist(fullname)) // TODO : 다른 폴더가 이 이름으로 변경되는 경우가 있다.
    {
      if (mpLocalRenamedAs != NULL)
        delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = AllocRelativePathName(fullname);

      if (StrCmpI(mpLocalRenamedAs, relativePathName) == 0)
      {
        delete[] mpLocalRenamedAs;
        mpLocalRenamedAs = NULL;
      }
    }
    delete[] relativePathName;
    delete[] actualPath;
  }
  else
  {
    AllocCopyString(&fullname, mLocal.pFilename);
  }

  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    SplitPathNameAlloc(fullname, NULL, &mLocalNew.pFilename);
  }

  BOOL rtn = FALSE;
  if (IsFolder())
  {
    mLocalNew.FileSize = 0;

    if (IsDirectoryExist(fullname))
      rtn = GetFileModifiedTime(fullname, &mLocalNew.FileTime, &mLocalNew.AccessTime);
    else
    {
      memset(&mLocalNew.FileTime, 0, sizeof(SYSTEMTIME));
      memset(&mLocalNew.AccessTime, 0, sizeof(SYSTEMTIME));
    }
  }
  else
  {
    mLocalNew.FileSize = GetFileSize(fullname);

    rtn = GetFileModifiedTime(fullname, &mLocalNew.FileTime, &mLocalNew.AccessTime);
  }

  delete[] fullname;
  return rtn;
#if 0
  if (CheckLocalRenamedAsByOld())
  {
    TCHAR msg[256] = _T("Refresh LocalRenamed : ");
    StringCchCat(msg, 256, mLocalRenamedAs);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  TCHAR fullname[KMAX_PATH];
  // current pathname
  if (mParent != NULL)
  {
    TCHAR pathname[KMAX_PATH];
    BOOL item_exist = TRUE;
    mParent->GetActualPathName(pathname, KMAX_PATH);
    MakeFullPathName(fullname, KMAX_PATH, pathname, mLocal.pFilename);
    if (!IsItemExist(fullname) && (lstrlen(mLocalRenamedAs) > 0))
    {
      GetAbsolutePathName(fullname, mLocalRenamedAs);
      if (!IsItemExist(fullname))
      {
        TCHAR name[KMAX_PATH];
        SplitPathName(mLocalRenamedAs, NULL, name);
        MakeFullPathName(fullname, KMAX_PATH, pathname, mLocal.pFilename);
      }
    }

    if ((lstrlen(mLocalRenamedAs) > 0) && (StrCmpI(mRelativeName, mLocalRenamedAs) == 0))
      mLocalRenamedAs[0] = '\0';
    else if (IsItemExist(fullname))
      GetRelativePathName(mLocalRenamedAs, fullname);
  }

  mLocalNew.FileSize = GetFileSize(fullname);

  if (lstrlen(mLocalRenamedAs) > 0)
  {
    SplitPathNameAlloc(fullname, &mLocalNew.pFullPath, &mLocalNew.pFilename);
  }

  return GetFileModifiedTime(fullname, &mLocalNew.FileTime, &mLocalNew.AccessTime);
#endif
}

void KSyncItem::RefreshLocalNewState(KSyncRootFolderItem* root)
{
  if (IsFolder() < 2)
  {
    /*
    if (root->GetSyncMethod() & SYNC_METHOD_INSTANT)
      StoreLogHistory(_T(__FUNCTION__), mpRelativePath, SYNC_MSG_LOG);
    */
    if (RefreshLocalNewFileInfo(mUserOid, root))
      addLocalState(ITEM_STATE_NOW_EXIST);
    else
      maskLocalState(ITEM_STATE_NOW_EXIST, 0);
  }
}

BOOL KSyncItem::IsLocalModified()
{
  return (IsValidSystemTime(mLocalNew.FileTime) && IsValidSystemTime(mLocal.FileTime) &&
    (CompareSystemTime(mLocalNew.FileTime, mLocal.FileTime) != 0)); // local file changed
}

BOOL KSyncItem::IsLocalModifiedAgain(BOOL update)
{
  BOOL modified = FALSE;
  SYSTEMTIME currFileTime;

#if 0
  // 상위 폴더 이름변경된 경우에는 AllocMovedRenamedAs() 동작하지 않음
  LPTSTR relativePathName = AllocRelativePathName();
  LPTSTR renamedAs = NULL;
  
  if (mLocalState & ITEM_STATE_EXISTING)
    renamedAs = AllocMovedRenamedAs(relativePathName);
  if (renamedAs != NULL)
  {
    TCHAR msg[256] = _T("Refresh LocalRenamed : ");
    StringCchCat(msg, 256, renamedAs);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
  delete[] relativePathName;

  if (CompareRelativePath(renamedAs, mpLocalRenamedAs) != 0)
  {
    if (mpLocalRenamedAs = NULL)
      delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = renamedAs;
    renamedAs = NULL;
  }
  if (renamedAs != NULL)
    delete[] renamedAs;
#else
  if (!(mLocalState & ITEM_STATE_EXISTING))
  {
    if (mpLocalRenamedAs = NULL)
      delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }
#endif

  /*
  if (mpLocalRenamedAs != NULL)
  {
    if ((lstrlen(renamedAs) != lstrlen(mpLocalRenamedAs)) ||
      ((lstrlen(renamedAs) > 0) && (StrCmpI(renamedAs, mpLocalRenamedAs) != 0)))
    {
      if (update)
      {
        delete[] mpLocalRenamedAs;
        mpLocalRenamedAs = NULL;

        int len = lstrlen(renamedAs);
        if (len > 0)
        {
          mpLocalRenamedAs = new TCHAR[len];
          StringCchCopy(mpLocalRenamedAs, len, renamedAs);
        }
      }
      else
      {
        if (renamedAs != NULL)
          delete[] renamedAs;
        return TRUE;
      }
      modified = TRUE;
    }
  }
  */

  LPTSTR fullname = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
  GetFileModifiedTime(fullname, &currFileTime, NULL);

  if(IsValidSystemTime(mLocalNew.FileTime))
  {
    if (CompareSystemTime(mLocalNew.FileTime, currFileTime) != 0) // local file changed
    {
      modified = TRUE;
      if (update)
        memcpy(&mLocalNew.FileTime, &currFileTime, sizeof(currFileTime));
    }
  }
  else 
  {
    if (CompareSystemTime(mLocal.FileTime, currFileTime) != 0) // local file changed
    {
      modified = TRUE;
      if (update)
        memcpy(&mLocalNew.FileTime, &currFileTime, sizeof(currFileTime));
    }
  }
  delete[] fullname;
  return modified;
}

BOOL KSyncItem::IsServerModified()
{
  // 기존 서버 정보가 없으면 변경 없음으로 한다.
  if ((lstrlen(mServer.StorageOID) == 0) || (lstrlen(mServer.FileOID) == 0))
    return FALSE;

  if ((lstrlen(mServerNew.StorageOID) > 0) && (StrCmpI(mServer.StorageOID, mServerNew.StorageOID) != 0))
    return TRUE;
  if ((lstrlen(mServerNew.FileOID) > 0) && (StrCmpI(mServer.FileOID, mServerNew.FileOID) != 0))
    return TRUE;
  return FALSE;
}

BOOL KSyncItem::IsModifiedNow()
{
  BOOL rtn = FALSE;
  LPTSTR fullname = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
  if (GetFileModifiedTime(fullname, &mLocalNew.FileTime, &mLocalNew.AccessTime))
    rtn = IsLocalModified();
  delete[] fullname;
  return rtn;
}

BOOL KSyncItem::IsNeedUpdate(BOOL onLocal)
{
  if (onLocal)
  {
    if ((mLocalState & (ITEM_STATE_MOVED|ITEM_STATE_UPDATED)) ||
      (mLocalState & ITEM_STATE_NEWLY_ADDED))
    return TRUE;
  }
  else
  {
    if ((mServerState & (ITEM_STATE_MOVED|ITEM_STATE_UPDATED)) ||
      (mServerState & ITEM_STATE_NEWLY_ADDED))
    return TRUE;
  }
  return FALSE;
}

void KSyncItem::UpdateServerNewInfo()
{
  MergeServerInfo(mServer, mServerNew);
  // clear serverNew
  FreeServerInfo(mServerNew);
}

void KSyncItem::UpdateLocalNewInfo(LPCTSTR baseFolder)
{
  MergeLocalInfo(mLocal, mLocalNew);

  // update relative name
  /*
  TCHAR filename[KMAX_PATH];
  MakeFullPathName(filename, KMAX_PATH, mLocal.FullPath, mLocal.Filename);
  int file_length = lstrlen(filename);
  int base_length = lstrlen(baseFolder);
  StringCchCopyN(mRelativeName, KMAX_PATH, &filename[base_length + 1], (file_length - base_length - 1));
  */

  // clear serverNew
  FreeLocalInfo(mLocalNew);
}

/* unused function
static void cropUntilSameFolder(LPTSTR s1, LPTSTR s2, LPTSTR rm)
{
  TCHAR p1[KMAX_PATH];
  TCHAR p2[KMAX_PATH];
  TCHAR n1[KMAX_PATH];
  TCHAR n2[KMAX_PATH];

  rm[0] = '\0';
  while((StrChr(s1, '\\') != NULL) && (StrChr(s2, '\\') != NULL))
  {
    SplitPathName(s1, p1, n1);
    SplitPathName(s2, p2, n2);

    if (StrCmpI(n1, n2) != 0)
      break;

    // rm = n1 + rm;
   MakeFullPathName(n2, KMAX_PATH, n1, rm);
   StringCchCopy(rm, KMAX_PATH, n2);

    s1[lstrlen(p1)] = '\0';
    s2[lstrlen(p2)] = '\0';
  }
}
*/


int KSyncItem::GetCountForMoveInOutOnServer(LPCTSTR partialPath, int path_len)
{
  int count = 0;
  if (path_len < 0)
    path_len = lstrlen(partialPath);

  if (isServerRenamedAs())
  {
    BOOL old_in = (lstrlen(mpRelativePath) >= path_len) && (StrCmpNI(mpRelativePath, partialPath, path_len) == 0);
    BOOL new_in = (lstrlen(mpServerRenamedAs) >= path_len) && (StrCmpNI(mpServerRenamedAs, partialPath, path_len) == 0);
    if (old_in != new_in)
      count++;
  }
  return count;
}

void KSyncItem::OnPathChanged(LPCTSTR folder, LPCTSTR rootFolder, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow)
{
  LPTSTR prevRelativeName = AllocRelativePathName();

  if (refreshNow)
    MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE);

  int meta_mask = 0;
  // update local rename path(파일 이름 및 중간 폴더 변경된 경우)
  // prev : 'A\B'
  // changed : 'A\BL'
  // oldPath : 'A\B\C\d.txt', after 'A\BL\C\d.txt'
  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    if (IsChildFolderOrFile(prev, mpLocalRenamedAs, FALSE))
    /*
    if ((lstrlen(prev) < lstrlen(mpLocalRenamedAs)) &&
      (StrCmpNI(prev, mLocalRenamedAs, lstrlen(prev)) == 0) &&
      (mLocalRenamedAs[lstrlen(prev)] == '\\'))
    */
    {
      TCHAR remained[KMAX_PATH];
      MakeRelativePathName(remained, KMAX_PATH, prev, mpLocalRenamedAs);

      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = AllocPathName(changed, remained);
      // MakeFullPathName(, MAX_PATH, changed, remained); // 'A\BL' + 'C\d.txt'

      meta_mask = META_MASK_RENAMED;
    }
  }

  if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    if (IsChildFolderOrFile(prev, mpServerRenamedAs, FALSE))
    /*
    if ((lstrlen(prev) < lstrlen(mpLocalRenamedAs)) &&
      (StrCmpNI(prev, mLocalRenamedAs, lstrlen(prev)) == 0) &&
      (mLocalRenamedAs[lstrlen(prev)] == '\\'))
    */
    {
      TCHAR remained[KMAX_PATH];
      MakeRelativePathName(remained, KMAX_PATH, prev, mpServerRenamedAs);

      delete[] mpServerRenamedAs;
      mpServerRenamedAs = AllocPathName(changed, remained);
      // MakeFullPathName(mpServerRenamedAs, MAX_PATH, changed, remained); // 'A\BL' + 'C\d.txt'


      if (mServerNew.pFullPath != NULL)
      {
        delete[] mServerNew.pFullPath;
        mServerNew.pFullPath = AllocString(mpServerRenamedAs);
      }

      meta_mask = META_MASK_NEW_SERVER_SIDE|META_MASK_RENAMED;
    }
  }

  /*
  if (lstrlen(mLocalRenamedAs) > 0) // 'A\B\C\d2.txt'
  {
    TCHAR oldRelativeName[KMAX_PATH];
    TCHAR newRelativeName[KMAX_PATH];
    GetRelativePathName(oldRelativeName, oldPath); // 'A\B\C\d.txt'
    GetRelativePathName(newRelativeName, newPath); // 'A\K\C\d.txt'

    TCHAR removedName[KMAX_PATH];
    // remove tail if it is same
    cropUntilSameFolder(oldRelativeName, newRelativeName, removedName); // 'A\B' & '\A\K', remove 'C\d.txt'

    // change LocalRenamed 
    MakeFullPathName(mLocalRenamedAs, MAX_PATH, newRelativeName, removedName); // 'A\K' + 'C\d2.txt'

    GetRelativePathName(newRelativeName, newPath);
    if (StrCmpI(newRelativeName, mLocalRenamedAs) == 0)
      mLocalRenamedAs[0] = '\0';
  }
  */

#ifdef USE_FMAPLIST_TXT
  // update fmaplist.txt
  int base_len = lstrlen(rootFolder) + 1;
  KSyncRootFolderItem *rootItem =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  rootItem->UpdateMetaFolderNameMapping(prevRelativeName, mpRelativePath, TRUE);
#endif

  // update server full path name, index
  if (refreshNow)
  {
    UpdatePathChangeOnMetafile(rootFolder, meta_mask, METAFILE_PATH_CHANGED);
  }
  else
  {
    mLocalState |= ITEM_STATE_REFRESH_PATH;
    SetPhase(PHASE_MOVE_FOLDERS);
    mResult |= FILE_NEED_REFRESH_PATH;
  }

  delete[] prevRelativeName;
}

void KSyncItem::RefreshPathChanged(LPCTSTR rootFolder)
{
  LPTSTR prevRelativeName = AllocRelativePathName();
  MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE);
  delete[] prevRelativeName;

  UpdatePathChangeOnMetafile(rootFolder, 0, METAFILE_PATH_CHANGED);
}

void KSyncItem::UpdatePathChangeOnMetafile(LPCTSTR baseFolder, int update_mask, int flag)
{
  if (TRUE) // mResult == FILE_NOT_CHANGED)
  {
    //StringCchCopy(mServer.FullPath, KMAX_PATH, mServerNew.FullPath);
    //update_mask |= META_MASK_FILENAME|META_MASK_SERVERPATH;

    if (IsFolder())
    {
      // 폴더인 경우에만 서버 경로 확인한다.
      RefreshServerNewState();

      update_mask |= META_MASK_SERVERPATHINDEX;

      if (((mServer.pFullPathIndex == NULL) != (mServerNew.pFullPathIndex == NULL)) ||
        (StrCmp(mServer.pFullPathIndex, mServerNew.pFullPathIndex) != 0))
      {
        TCHAR msg[256] = _T("FullPathIndex : ");
        StringCchCat(msg, 256, mServer.pFullPathIndex);
        StringCchCat(msg, 256, _T(" changed to "));
        StringCchCat(msg, 256, mServerNew.pFullPathIndex);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        AllocCopyString(&mServer.pFullPathIndex, mServerNew.pFullPathIndex);

        // 이전값이니 mServerNew.pFullPathIndex를 클리어하고 저장한다.
        delete[] mServerNew.pFullPathIndex;
        mServerNew.pFullPathIndex = NULL;
        update_mask |= META_MASK_NEW_SERVERPATHINDEX;
      }
    }

    if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
    {
      LPTSTR pathName = AllocRelativePathName();
      if (StrCmpI(mpLocalRenamedAs, pathName) == 0)
      {
        delete[] mpLocalRenamedAs;
        mpLocalRenamedAs = NULL;
        mLocalState &= ~ITEM_STATE_MOVED;
      }
      delete[] pathName;
    }

    StoreMetafile(NULL, update_mask, flag);
  }
}

void KSyncItem::UpdateServerOID(LPCTSTR serverOID, LPCTSTR fileOID, LPCTSTR storageOID)
{
  if (serverOID != NULL)
    StringCchCopy(mServerOID, MAX_OID, serverOID);
  if (fileOID != NULL)
    StringCchCopy(mServer.FileOID, MAX_OID, fileOID);
  if (storageOID != NULL)
    StringCchCopy(mServer.StorageOID, MAX_STGOID, storageOID);
}

void KSyncItem::UpdateServerFileInfo(int perm, LPCTSTR lastModifierOID, SYSTEMTIME &fileTime)
{
  mServer.Permissions = perm;
  StringCchCopy(mServer.UserOID, MAX_OID, lastModifierOID);
  mServer.FileTime = fileTime;
}

void KSyncItem::UpdateServerNewFileInfo(int perm, LPCTSTR lastModifierOID, LPCTSTR serverFullPath, LPTSTR fullPathIndex)
{
  mServerNew.Permissions = perm;

  if (lastModifierOID != NULL)
    StringCchCopy(mServerNew.UserOID, MAX_OID, lastModifierOID);
  else
    mServerNew.UserOID[0] = '\0';

  /*
  if (serverFullPath != NULL)
    StringCchCopy(mServerNew.FullPath, KMAX_PATH, serverFullPath);
  else
    mServerNew.FullPath[0] = '\0';

  if (fullPathIndex != NULL)
    StringCchCopy(mServerNew.FullPathIndex, KMAX_PATHINDEX, fullPathIndex);
  else
    mServerNew.FullPathIndex[0] = '\0';
  */

  if (mServerNew.pFullPath != NULL)
  {
    delete[] mServerNew.pFullPath;
    mServerNew.pFullPath = NULL;
  }

#ifndef USE_SCAN_BY_HTTP_REQUEST
  if (mParent != NULL)
  {
    KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    mServerNew.pFullPath = allocServerSideRelativePath(root, serverFullPath, NULL);
  }
  else
#endif // serverFullPath was changed to relative path
  {
    mServerNew.pFullPath = AllocString(serverFullPath);
  }

  if (mServerNew.pFullPathIndex != NULL)
    delete[] mServerNew.pFullPathIndex;
  mServerNew.pFullPathIndex = AllocString(fullPathIndex);
}

BOOL KSyncItem::UpdateServerNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, LPCTSTR permissions, 
  LPCTSTR lastModifierOID, LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR serverFullPath, LPCTSTR serverFullPathIndex, LPCTSTR name)
{
  int perm = String2Permission(permissions);

  return UpdateServerNewFileInfo(fileSize, fileTime, perm, 
    lastModifierOID, fileOID, storageOID, serverFullPath, serverFullPathIndex, name);
}

BOOL KSyncItem::UpdateServerNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, int permissions, 
  LPCTSTR lastModifierOID, LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR serverFullPath, LPCTSTR serverFullPathIndex, LPCTSTR name)
{
  mServerNew.FileSize = fileSize;
  mServerNew.FileTime = fileTime;
  if (lastModifierOID != NULL)
    StringCchCopy(mServerNew.UserOID, MAX_OID, lastModifierOID);
  else
    mServerNew.UserOID[0] = '\0';

  if (fileOID != NULL)
    StringCchCopy(mServerNew.FileOID, MAX_OID, fileOID);
  else
    mServerNew.FileOID[0] = '\0';
  if (storageOID != NULL)
    StringCchCopy(mServerNew.StorageOID, MAX_STGOID, storageOID);
  else
    mServerNew.StorageOID[0] = '\0';

  /*
  if (serverFullPath != NULL)
    StringCchCopy(mServerNew.FullPath, KMAX_PATH, serverFullPath);
  else
    mServerNew.FullPath[0] = '\0';
  if (serverFullPathIndex != NULL)
    StringCchCopy(mServerNew.FullPathIndex, KMAX_PATH, serverFullPathIndex);
  else
    mServerNew.FullPathIndex[0] = '\0';
  */

#ifndef USE_SCAN_BY_HTTP_REQUEST
  if (mServerNew.pFullPath != NULL)
    delete[] mServerNew.pFullPath;
  if (mParent != NULL)
  {
    KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    mServerNew.pFullPath = allocServerSideRelativePath(root, serverFullPath, NULL);
  }
  else
  {
    mServerNew.pFullPath = AllocString(serverFullPath);
  }
#else
  if (StrCmp(mServerNew.pFullPath, serverFullPath) != 0)
    AllocCopyString(&mServerNew.pFullPath, serverFullPath);
#endif

  if (StrCmp(mServerNew.pFullPathIndex, serverFullPathIndex) != 0)
    AllocCopyString(&mServerNew.pFullPathIndex, serverFullPathIndex);

  if (StrCmp(mServerNew.pFilename, name) != 0)
    AllocCopyString(&mServerNew.pFilename, name);

  mServerNew.Permissions = permissions;
  return TRUE;
}

BOOL KSyncItem::UpdateLocalNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, SYSTEMTIME &accessTime, LPCTSTR user)
{
  mLocalNew.FileSize = fileSize;
  mLocalNew.FileTime = fileTime;
  mLocalNew.AccessTime = accessTime;
  StringCchCopy(mLocal.UserOID, MAX_OID, user);
  return TRUE;
}

void KSyncItem::MergeLocalInformation(KSyncItem *nItem)
{
  mLocalState |= nItem->mLocalState;

  //mLocalNew = nItem->mLocalNew;
  CopyLocalInfo(mLocalNew, nItem->mLocalNew);
}

BOOL KSyncItem::SetLocalRenamedAs(LPCTSTR newRelativeName)
{
  if ((mpLocalRenamedAs == NULL) || (lstrlen(mpLocalRenamedAs) == 0))
  {
    AllocCopyString(&mpLocalRenamedAs, newRelativeName);
    return TRUE;
  }
  else
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Already have LocalRenamedAs : %s : %s"), mpLocalRenamedAs, newRelativeName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return FALSE;
  }
}

BOOL KSyncItem::SetNewParentOID(LPTSTR newParentServerOID)
{
  if (lstrlen(mServerMoveToOID) == 0)
  {
    StringCchCopy(mServerMoveToOID, MAX_OID, newParentServerOID);
    return TRUE;
  }
  else
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Already have mServerMoveToOID : %s : %s"), mServerMoveToOID, newParentServerOID);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return FALSE;
  }
}

#if 0 // replaced with AllocRelativePathName()
BOOL KSyncItem::GetRelativePathName(LPTSTR OUT relativeName, LPCTSTR IN path, LPCTSTR IN name)
{
  if (mParent != NULL)
  {
    TCHAR newfilename[KMAX_PATH];
    MakeFullPathName(newfilename, KMAX_PATH, path, name);
    LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
    int root_len = lstrlen(rootPath);
    if (root_len > 0)
      root_len++; // for directory delimeter
    StringCchCopy(relativeName, KMAX_PATH, &newfilename[root_len]);
    return TRUE;
  }
  return FALSE;
}

BOOL KSyncItem::GetRelativePathName(LPTSTR OUT relativeName, LPCTSTR IN fullname)
{
  if (mParent != NULL)
  {
    LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
    int root_len = lstrlen(rootPath);
    if (root_len > 0)
      root_len++; // for directory delimeter
    StringCchCopy(relativeName, KMAX_PATH, &fullname[root_len]);
    return TRUE;
  }
  return FALSE;
}
#endif

LPTSTR KSyncItem::AllocFullPathName()
{
#ifdef SYNC_ALL_DRIVE
  return AllocRelativePathName();
#else
  LPTSTR relativePathName = AllocRelativePathName();
  LPTSTR fullPathName = AllocAbsolutePathName(relativePathName);
  delete[] relativePathName;
  return fullPathName;
#endif
}

LPTSTR KSyncItem::AllocTreePathName()
{
  int len = 0;
  LPTSTR name = NULL;
  if (IsFolder() < 2)
  {
    if (mParent != NULL)
      len = ((KSyncFolderItem *)mParent)->GetPathNameLength(0);
    len += lstrlen(GetFilename()) + 2;
    name = new TCHAR[len];

    if (mParent != NULL)
      ((KSyncFolderItem *)mParent)->GetPathName(name, len, 0);
    else
      name[0] = '\0';

    MakeFullPathName(name, len, name, GetFilename());
  }
  return name;
}

LPTSTR KSyncItem::AllocRelativePathName()
{
  LPTSTR name = NULL;

  if (IsFolder() < 2)
  {
    LPCTSTR filename = GetFilename();

    if (mpRelativePath != NULL)
    {
      name = AllocString(mpRelativePath);
    }
    else if (mParent != NULL)
    {
      int len = lstrlen(filename) + 1;
#if 0
      LPCTSTR baseFolder = ((KSyncFolderItem *)mParent)->AllocActualPathName(FALSE);
      if (baseFolder != NULL)
      {
        // it should be relative path
#if defined(_DEBUG) && !defined(SYNC_ALL_DRIVE)
        ASSERT(baseFolder[1] != ':');
#endif
        len += lstrlen(baseFolder) + 1;
      }
      name = new TCHAR[len];
      MakeFullPathName(name, len, baseFolder, mLocal.pFilename);
      if (baseFolder != NULL)
        delete[] baseFolder;
#else
      LPCTSTR baseFolder = ((KSyncFolderItem *)mParent)->GetBaseFolder();
      if (baseFolder != NULL)
        len += lstrlen(baseFolder) + 1;
      name = new TCHAR[len];
      MakeFullPathName(name, len, baseFolder, filename);
#endif
    }
    else
    {
      name = AllocString(filename);
    }
  }
  return name;
}

LPTSTR KSyncItem::AllocRelativePathName(LPCTSTR fullPathName)
{
  LPTSTR path = NULL;
#if defined(SYNC_ALL_DRIVE)
  path = AllocString(fullPathName);
#else
  LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
  int len = lstrlen(fullPathName) - lstrlen(rootPath);
  if (len > 0)
  {
    path = new TCHAR[len];
    StringCchCopy(path, len, fullPathName);
    LPCTSTR p = &fullPathName[lstrlen(rootPath)+1];
    StringCchCopy(path, len, p);
  }
#endif
  return path;
}

LPTSTR KSyncItem::AllocRelativePathName(LPCTSTR fullPath, LPCTSTR fileName)
{
  LPTSTR pathName = NULL;
#if defined(SYNC_ALL_DRIVE)
  if (IsAbsolutePathName(fileName))
  {
    pathName = AllocString(fileName);
  }
  else
  {
    int len = lstrlen(fullPath) + lstrlen(fileName) + 1;
    pathName = new TCHAR[len];
    MakeFullPathName(pathName, len, pathName, fileName);
  }
#else
  LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
  int len = lstrlen(fullPath) - lstrlen(rootPath) + lstrlen(fileName) + 1;
  if (len > 0)
  {
    pathName = new TCHAR[len];
    if (lstrlen(fullPath) > lstrlen(rootPath))
      StringCchCopy(pathName, len, &fullPath[lstrlen(rootPath)+1]);
    else
      pathName[0] = '\0';
    MakeFullPathName(pathName, len, pathName, fileName); //kylee 2017.02
  }
#endif
  return pathName;
}

LPTSTR KSyncItem::AllocAbsolutePathName()
{
  LPTSTR relativePath = AllocRelativePathName();
#ifdef SYNC_ALL_DRIVE
  return relativePath;
#else
  LPTSTR fullPathName = AllocAbsolutePathName(relativePath);
  delete[] relativePath;
  return fullPathName;
#endif
}

LPTSTR KSyncItem::AllocAbsolutePathName(LPCTSTR relativePathName)
{
#ifdef SYNC_ALL_DRIVE
  return AllocString(relativePathName);
#else
  LPCTSTR rootPath;

  if (mParent != NULL)
    rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
  else
    rootPath = ((KSyncFolderItem *)this)->GetRootPath(); // this is root folder item

  int len = lstrlen(relativePathName) + lstrlen(rootPath) + 2;
  LPTSTR path = new TCHAR[len];
  MakeFullPathName(path, len, rootPath, relativePathName);
  return path;
#endif
}

/* deprecated : AllocMakeFullPathName
LPTSTR KSyncItem::AllocAbsolutePathName(LPCTSTR rootFolder, LPCTSTR relativePathName)
{
  int len = lstrlen(relativePathName) + lstrlen(rootFolder) + 2;
  LPTSTR path = new TCHAR[len];
  MakeFullPathName(path, len, rootFolder, relativePathName);
  return path;
}
*/

int KSyncItem::GetActualPathName(LPTSTR fullname, int size, BOOL abs)
{
  int len = 0;

  if (mParent != NULL)
    len = mParent->GetActualPathName(fullname, size, abs) + 1;

  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0)) // renamed
  {
    LPCTSTR rootPath = NULL;
    if ((mParent != NULL) && abs)
    {
      rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
      len = lstrlen(rootPath) + 1;
    }

    if (fullname != NULL)
      MakeFullPathName(fullname, size, rootPath, mpLocalRenamedAs);

    len += lstrlen(mpLocalRenamedAs) + 1;
  }
  else
  {
    LPCTSTR name = GetFilename();
    if (fullname != NULL)
      MakeFullPathName(fullname, size, fullname, name);
    len += lstrlen(name) + 2;
  }
  return len;
}

LPTSTR KSyncItem::AllocActualPathName(BOOL abs)
{
  int len = GetActualPathName(NULL, 0, abs);
  if (len > 0)
  {
    LPTSTR name = new TCHAR[len];
    GetActualPathName(name, len, abs);
    return name;
  }
  return NULL;
}

BOOL KSyncItem::GetAbsolutePathName(LPTSTR OUT fullname, LPCTSTR IN relativeName)
{
  if (mParent != NULL)
  {
    LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
    MakeFullPathName(fullname, KMAX_PATH, rootPath, relativeName);
    return TRUE;
  }
  return FALSE;
}

LPTSTR KSyncItem::AllocMovedRenamedAs(LPCTSTR relativeName)
{
  if (mParent != NULL)
  {
    KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    return root->AllocMovedRenamedAs(relativeName);
  }
  return NULL;
}

#if 0
BOOL KSyncItem::GetLocalRenamedAsByOld(LPCTSTR oldPath, LPCTSTR oldName, LPTSTR renamedAs)
{
  LPCTSTR baseFolder = ((KSyncFolderItem *)mParent)->GetRootPath();
  TCHAR relativeName[MAX_PATH] = { 0 };
  int base_len = lstrlen(baseFolder) + 1;
  if (base_len < lstrlen(oldPath))
    StringCchCopy(relativeName, MAX_PATH, &oldPath[base_len]);
  MakeFullPathName(relativeName, KMAX_PATH, relativeName, oldName);

  KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  renamedAs[0] = '\0';
  return root->GetMovedRenamedAs(relativeName, renamedAs);
}
#endif

BOOL KSyncItem::CheckLocalRenamedAsByOld(KSyncItem* rootItem)
{
  if (mpLocalRenamedAs != NULL)
  {
    delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }

  if (mLocalState & ITEM_STATE_EXISTING)
  {
    LPTSTR oldRelativeName = AllocRelativePathName();
    if ((rootItem == NULL) && (mParent != NULL))
      rootItem = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

    if (rootItem != NULL)
    {
      mpLocalRenamedAs = rootItem->AllocMovedRenamedAs(oldRelativeName);

      // rename table 항목의 플래그를 0으로 설정.
      if (mpLocalRenamedAs != NULL)
        ((KSyncRootFolderItem *)rootItem)->ClearRenameFlag(NULL, mpLocalRenamedAs);
    }

    delete[] oldRelativeName;
  }

  CheckLocalRenamedItself();
  return (mpLocalRenamedAs != NULL);
}

BOOL KSyncItem::CheckLocalRenamedAs(LPCTSTR newPath, LPCTSTR newName)
{
  BOOL rtn = FALSE;
  LPTSTR orgPathName = AllocRelativePathName();
  LPTSTR newPathName = AllocPathName(newPath, newName);

  if (mpLocalRenamedAs != NULL)
  {
    delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }

  if (StrCmpI(orgPathName, newPathName) != 0)
  {
    BOOL is_changed = TRUE;

    // 상위 폴더가 바뀐 것이고 자신은 변함이 없는 경우 체크
    if ((newPath != NULL) && (lstrlen(newPath) > 0) && (mParent != NULL) &&
      (StrCmpI(newName, mLocal.pFilename) == 0))
    {
      LPTSTR parent_path = mParent->AllocActualPathName(FALSE);
      if (StrCmpI(parent_path, newPath) == 0)
        is_changed = FALSE;
      delete[] parent_path;
    }

    if (is_changed)
    {
      mpLocalRenamedAs = newPathName;
      newPathName = NULL;

      TCHAR msg[512] = _T("Local File [");
      StringCchCat(msg, 512, orgPathName);
      StringCchCat(msg, 512, _T("] Move/Rename detected as ["));
      StringCchCat(msg, 512, mpLocalRenamedAs);
      StringCchCat(msg, 512, _T("]"));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      rtn = TRUE;
    }
  }

  delete[] orgPathName;
  if (newPathName != NULL)
    delete[] newPathName;
  return rtn;
}

BOOL KSyncItem::CheckLocalRenamedItself()
{
  if (isLocalRenamedAs())
  {
    BOOL is_changed = TRUE;

    if (StrCmpI(mpRelativePath, mpLocalRenamedAs) != 0)
    {
      LPTSTR newName= NULL;
      LPTSTR newPath = NULL;
      SplitPathNameAlloc(mpLocalRenamedAs, &newPath, &newName);

      // 상위 폴더가 바뀐 것이고 자신은 변함이 없는 경우 체크
      if ((mParent != NULL) &&
        (StrCmpI(newName, mLocal.pFilename) == 0))
      {
        LPTSTR parent_path = mParent->AllocActualPathName(FALSE);
        if (StrCmpI(parent_path, newPath) == 0)
          is_changed = FALSE;
        delete[] parent_path;
      }
      if (newPath != NULL)
        delete[] newPath;
      if (newName != NULL)
        delete[] newName;
    }
    else
    {
      is_changed = FALSE;
    }

    if(!is_changed)
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
  }

  return isLocalRenamedAs();
}

BOOL KSyncItem::CheckServerRenamedAs(LPCTSTR newPath, LPCTSTR newName)
{
  BOOL rtn = FALSE;
  LPTSTR orgPathName = AllocRelativePathName(); // mpRelativeName
  LPTSTR newPathName = AllocPathName(newPath, newName);

  LPTSTR orgName = NULL;
  SplitPathNameAlloc(orgPathName, NULL, &orgName);

  if (mpServerRenamedAs != NULL)
  {
    delete[] mpServerRenamedAs;
    mpServerRenamedAs = NULL;
  }

  // TODO: 이름은 동일해도 경로 바뀜으로 판단.
  if (StrCmpI(orgPathName, newPathName) != 0)
  {
    BOOL is_changed = TRUE;

    // 상위 폴더가 바뀐 것이고 자신은 변함이 없는 경우 체크
    if ((newPath != NULL) && (lstrlen(newPath) > 0) && (mParent != NULL) &&
      (StrCmpI(newName, orgName) == 0))
    {
      LPTSTR parent_path = mParent->AllocActualPathName(FALSE);
      if (StrCmpI(parent_path, newPath) == 0)
        is_changed = FALSE;
      delete[] parent_path;
    }

    if (is_changed)
    {
      mpServerRenamedAs = newPathName;
      TCHAR msg[512] = _T("Server File [");
      StringCchCat(msg, 512, orgPathName);
      StringCchCat(msg, 512, _T("] Move/Rename detected as ["));
      StringCchCat(msg, 512, mpServerRenamedAs);
      StringCchCat(msg, 512, _T("]"));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      newPathName = NULL;
      rtn = TRUE;
    }
  }

  delete[] orgPathName;
  if (orgName != NULL)
    delete[] orgName;
  if (newPathName != NULL)
    delete[] newPathName;
  return rtn;
}

int KSyncItem::UpdateServerRenamedAs(LPCTSTR orgRelativePath, LPCTSTR changedRelativePath)
{
    if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > lstrlen(orgRelativePath)) &&
      IsChildFolderOrFile(orgRelativePath, mpServerRenamedAs, FALSE))
    {
      int len = lstrlen(mpServerRenamedAs) - lstrlen(orgRelativePath) + lstrlen(changedRelativePath) + 2;
      LPTSTR newRenamedAs = new TCHAR[len];
      MakeFullPathName(newRenamedAs, len, changedRelativePath, &mpServerRenamedAs[lstrlen(orgRelativePath)]);

      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("%s => %s"), mpServerRenamedAs, newRenamedAs);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      delete[] mpServerRenamedAs;
      mpServerRenamedAs = newRenamedAs;

      AllocCopyString(&mServerNew.pFullPath, mpServerRenamedAs);
      return 1;
    }
    return 0;
}

BOOL KSyncItem::isLocalRenamedItself()
{
  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    TCHAR name[MAX_PATH];
    SplitPathName(mpLocalRenamedAs, NULL, name);
    if (StrCmpI(name, mLocal.pFilename) != 0)
      return TRUE;
  }
  return FALSE;
}

BOOL KSyncItem::isLocalRenamedAs()
{
  return ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0));
}

BOOL KSyncItem::isServerRenamedAs()
{
  return ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0));
}

BOOL KSyncItem::isServerMovedAs()
{
  return ((mNewParent != NULL) && (mNewParent != mParent));
}

// check if renamed file exist, if not exist, clear rename info
BOOL KSyncItem::validLocalRenamedAs()
{
  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    LPTSTR fullPathName = AllocAbsolutePathName(mpLocalRenamedAs);
    if (!IsFileExist(fullPathName))
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
    delete[] fullPathName;
    return (mpLocalRenamedAs != NULL);

    /*
    LPCTSTR rootFolder = ((KSyncFolderItem *)mParent)->GetRootPath();

    TCHAR newfilename[KMAX_PATH];
    MakeFullPathName(newfilename, KMAX_PATH, rootFolder, mpLocalRenamedAs);
    if (IsFileExist(newfilename))
      return TRUE;

    mpLocalRenamedAs[0] = '\0';
    */
  }
  return FALSE;
}

void KSyncItem::SetMetafilename(TCHAR *filename)
{
  AllocCopyString(&mpMetafilename, filename);
  // StringCchCopy(mMetafilename, KMAX_METAPATH, filename);
}

BOOL KSyncItem::IsNewlyAdded()
{
  return (!IsValidSystemTime(mLocal.FileTime) && !IsValidSystemTime(mServer.FileTime));
}

void KSyncItem::SetScanServerFolderState(int s, int flag)
{
  if (flag & WITH_ITSELF)
    SetServerScanState(s);
}

void KSyncItem::SetServerStateByScanState()
{
  mServerState &= ITEM_STATE_EXISTING;
  if (mServerScanState == ITEM_DELETED)
  {
    maskServerState(ITEM_STATE_NOW_EXIST, 0);
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Deleted from server : %s"), GetFilename());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
  else if (mServerScanState == ITEM_EXIST)
  {
    addServerState(ITEM_STATE_NOW_EXIST);
  }

  // 이동 정보에서 새로운 경로명, 이름을 뽑아낸다.
  if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    SplitPathNameAlloc(mpServerRenamedAs, NULL, &mServerNew.pFilename);
    AllocCopyString(&mServerNew.pFullPath, mpServerRenamedAs);
  }
}

BOOL KSyncItem::GetCurrentFileTime(SYSTEMTIME &local_time, SYSTEMTIME &server_time)
{
  LPTSTR fullPathName = AllocAbsolutePathName();
  GetFileModifiedTime(fullPathName, &local_time, NULL);
  delete[] fullPathName;

  /*
  TCHAR filename[KMAX_LONGPATH];
  GetFullPathName(filename, KMAX_LONGPATH);
  GetFileModifiedTime(filename, &local_time, NULL);
  */

  // TODO

  // fake for testing
  // AddSystemTime(item->mLocal.FileTime, last_update_local_time, 1, 2, 30);
  // AddSystemTime(item->mServer.FileTime, last_update_server_time, 1, 2, 30);
  // AddSystemTime(server_time, local_time, 1, 2, 0);

  return TRUE;
}

void KSyncItem::RefreshOldNew()
{
  if (IsValidSystemTime(mLocalNew.FileTime) && !IsValidSystemTime(mLocal.FileTime))
    mLocal.FileTime = mLocalNew.FileTime;
  if (IsValidSystemTime(mServerNew.FileTime) && !IsValidSystemTime(mServer.FileTime))
    mServer.FileTime = mServerNew.FileTime;

  mLocal.FileSize = mLocalNew.FileSize;
  mServer.FileSize = mServerNew.FileSize;

  if (lstrlen(mServerNew.UserOID) > 0)
    StringCchCopy(mServer.UserOID, MAX_OID, mServerNew.UserOID);
  /*
  if (lstrlen(mServerNew.FullPath) > 0)
    StringCchCopy(mServer.FullPath, MAX_OID, mServerNew.FullPath);
  */
}

int KSyncItem::CompareItem(KSyncItem *item, int column, BOOL orderAscend)
{
  int r = CompareItem(item, column);
  if(!orderAscend)
  {
    if (r > 0)
      return -1;
    else if(r < 0)
      return 1;
  }
  return r;
}

// 1 if this > item
int KSyncItem::CompareItem(KSyncItem *item, int column)
{
  if ((column == 0) || (column == 3))
    return StrCmpI(mLocal.pFilename, item->mLocal.pFilename); // 1 if mLocal.Filename > item->mLocal.Filename
  else if (column == 1) // local time
    return CompareSystemTime(GetLocalTime(), item->GetLocalTime());
  else if (column == 2) // local size
  {
    if (GetLocalFileSize() > item->GetLocalFileSize())
      return 1;
    else if (GetLocalFileSize() < item->GetLocalFileSize())
      return -1;
    return 0;
  }
  else if (column == 4)
  {
    TCHAR user_id[64];
    TCHAR item_user_id[64];

    gpCoreEngine->GetUserName(GetServerUser(), user_id);
    gpCoreEngine->GetUserName(item->GetServerUser(), item_user_id);

    return StrCmp(user_id, item_user_id);
  }
  else if (column == 5)
    return CompareSystemTime(GetServerTime(), item->GetServerTime());
  else if (column == 6)
  {
    if (GetServerFileSize() > item->GetServerFileSize())
      return 1;
    else if (GetServerFileSize() < item->GetServerFileSize())
      return -1;
    return 0;
  }
  return 0;
}

BOOL KSyncItem::MakeMetafilename(LPCTSTR baseFolder)
{
  /*
  TCHAR name[KMAX_PATH];
  TCHAR localFilename[KMAX_PATH];
  MakeFullPathName(localFilename, KMAX_PATH, mLocal.FullPath, mLocal.Filename);
	StringCchCopy(name, KMAX_PATH, &localFilename[lstrlen(baseFolder) + 1]);
  */
#ifdef USE_SYNC_STORAGE
  LPTSTR rPathName = AllocRelativePathName();
	LPTSTR meta_path = KSyncRootStorageFile::GetMetaFolderName(baseFolder, rPathName, TRUE);

  TCHAR name[KMAX_FILENAME];
  SplitPathName(rPathName, NULL, name);

  if (mpMetafilename != NULL)
    delete[] mpMetafilename;
  int len = lstrlen(meta_path) + lstrlen(name) + 10;
  mpMetafilename = new TCHAR[len];

  MakeFullPathName(mpMetafilename, len, meta_path, name);
  StringCchCat(mpMetafilename, len, INFO_EXTENSION);

  delete[] meta_path;
  delete[] rPathName;
#endif
  return TRUE;
}

// return TRUE if MetaData exist
BOOL KSyncItem::ReadyMetafilename(KSyncRootStorage *s, int metafile_flag)
{
  if (s == NULL)
    s = GetSafeStorage();

  if (s != NULL)
    return s->ReadyMetafilename(this, metafile_flag);
  return TRUE;
}

BOOL KSyncItem::LoadMetafile(KSyncRootStorage *s, DWORD mask)
{
  if (s == NULL)
    s = GetSafeStorage();

  if (s == NULL)
    return FALSE;

  if (!ReadyMetafilename(s, 0))
    return FALSE;

  LPTSTR rPathName = AllocRelativePathName();
  BOOL rtn = s->LoadMetafile(this, rPathName, mask);

  if (rPathName != NULL)
    delete rPathName;
  return rtn;
}

KSyncRootStorage* KSyncItem::GetSafeStorage()
{
  KSyncRootFolderItem* rootFolder;
  if (IsFolder() == 2)
    rootFolder = (KSyncRootFolderItem*) this;
  else if (mParent != NULL)
    rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  else
  {
    TCHAR msg[120];
    StringCchPrintf(msg, 120, _T("No Storage for none-root item : %s"), mLocal.pFilename);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return NULL;
  }
  return rootFolder->GetStorage();
}

BOOL KSyncItem::StoreMetafile(KSyncRootStorage *s, DWORD mask, int metafile_flag)
{
  if (s == NULL)
    s = GetSafeStorage();
  if (s == NULL)
    return FALSE;
  if (!ReadyMetafilename(s, metafile_flag))
    return FALSE;

  return s->StoreMetafile(this, mask, metafile_flag);
}

BOOL KSyncItem::MoveMetafileAsRename(LPCTSTR prevRelativePathName, int flag, KSyncRootStorage *s)
{
  if (s == NULL)
    s = GetSafeStorage();

  return s->MoveMetafileAsRename(this, prevRelativePathName, flag);
}

BOOL KSyncItem::CanBeReadable(BOOL onLocal)
{
  DWORD mask = (onLocal) ? FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED :
    FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED;

  if ((mResult & mask) || (mConflictResult & mask))
    return FALSE;

  int conflict = (mConflictResult & FILE_SYNC_ERR);
  if (!onLocal)
  {
    if ( (conflict == FILE_SYNC_NO_DOWNLOAD_PERMISSION) ||
      (conflict == FILE_SYNC_CREATE_SERVER_FOLDER_FAIL) ||
      (conflict == FILE_SYNC_NO_PERMISSION_ECM_DISK) ||
      (conflict == FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER) ||
      (conflict == FILE_SYNC_ROOTSERVER_FOLDER_NOT_EXIST) ||
      (conflict == FILE_SYNC_NO_FOLDER_PERMISSION) ||
      (conflict == FILE_SYNC_NO_SERVER_PERMISSION) )
    return FALSE;

    if (!IsServerFileExist())
      return FALSE;

    if (!(GetServerPermission() & SFP_READ))
      return FALSE;
  }
  return TRUE;
}

BOOL KSyncItem::CanBeRenamed(BOOL onLocal)
{
  /* now sync-error can be renamed
  if (mConflictResult & FILE_SYNC_ERR) // conflicted item cannot be renamed
    return FALSE;
  */

  DWORD mask = (onLocal) ? FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED :
    FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED;

  if ((mResult & mask) || (mConflictResult & mask))
    return FALSE;

  if (!onLocal)
  {
    if (!(GetServerPermission() & SFP_WRITE))
      return FALSE;
  }
  return TRUE;
}

void KSyncItem::SetServerOID(LPCTSTR oid)
{
   StringCchCopy(mServerOID, MAX_OID, oid);
}

void KSyncItem::SetItemSynchronizing(LPCTSTR baseFolder, int startSync, int flag)
{
  LPTSTR fullpath = AllocFullPathName();

  if (IsFileExist(fullpath))
  {
    if (startSync < 2)
    {
      int currentResult = mResult;
      if (startSync)
        mResult |= FILE_NOW_SYNCHRONIZING;
      else
        mResult &= ~FILE_NOW_SYNCHRONIZING;
      if ((currentResult != mResult) && (flag & WITH_STORE_METAFILE))
        StoreMetafile(NULL, META_MASK_SYNCRESULT|META_MASK_FILENAME, 0);
    }

    gpCoreEngine->ShellNotifyIconChangeOverIcon(fullpath, !!(flag & SHELL_ICON_UPDATE_RECURSE));
  }
  delete[] fullpath;
}

int KSyncItem::makeResultFromState(int userAction, int syncMethod)
{
  int r = 0;
  if (mServerState & ITEM_STATE_NO_PERMISSION)
  {
    /*
    mServerState &= ~ITEM_STATE_NO_PERMISSION;
    userAction = makeResultFromState(0);
    mServerState |= ITEM_STATE_NO_PERMISSION;
    */
    r = (userAction & FILE_NEED_UPLOAD) ? FILE_SYNC_NO_UPLOAD_PERMISSION : FILE_SYNC_NO_DOWNLOAD_PERMISSION;
  }
  else if (mServerState & ITEM_STATE_LOCATION_CHANGED) // ITEM_STATE_PARENT_MOVED 
  {
    r = FILE_SYNC_PARENT_FOLDER_MOVED;
  }
  else if ((mLocalState & ITEM_STATE_ERROR) || (mServerState & ITEM_STATE_ERROR))
  {
    r = FILE_SYNC_ERROR_FAIL;
  }
#ifdef USE_BACKUP
  else if ((mLocalState & ITEM_STATE_MOVED) && (mServerState & ITEM_STATE_MOVED))
  {
    // conflict only when renamed differently
    if ((mpLocalRenamedAs != NULL) && (mpServerRenamedAs != NULL) &&
      (StrCmpI(mpLocalRenamedAs, mpServerRenamedAs) != 0))
    {
      r = FILE_SAME_MOVED;
      if (syncMethod & SYNC_METHOD_BACKUP)
      {
        // upload as new file
        clearServerSideInfo();
        r |= FILE_NEED_UPLOAD;
      }
      else
      {
        // 복원할때는 기존 로컬 파일은 놔두고 서버 파일을 새로 다운받는다.
        clearLocalSideInfo();
        r |= FILE_NEED_DOWNLOAD;
      }
    }
    else
    {
      r = FILE_SAME_MOVED;
    }
  }
  else if (mLocalState == ITEM_STATE_NONE) // 기록 없음
  {
    if (syncMethod & SYNC_METHOD_BACKUP)
    {
      r = 0; // don't care
    }
    else
    {
      if(mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED|ITEM_STATE_NEWLY_ADDED))
        r = FILE_NEED_DOWNLOAD;
    }
  }
  else if (mLocalState & ITEM_STATE_UPDATED)
  {
    if (syncMethod & SYNC_METHOD_BACKUP)
    {
      r = FILE_NEED_UPLOAD;
      if(mServerState & ITEM_STATE_MOVED)
      {
        // upload as new file
        clearServerSideInfo();
      }
    }
    else
    {
      if(mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED|ITEM_STATE_NEWLY_ADDED))
        r = FILE_NEED_DOWNLOAD;
      else if (mServerState == ITEM_STATE_EXISTING) // 변동없음.
        r = FILE_NEED_DOWNLOAD;
    }
  }
  else if(mLocalState & ITEM_STATE_DELETED)
  {
    if (syncMethod & SYNC_METHOD_BACKUP)
    {
      r = FILE_NEED_RETIRED;
      // 이동이 아니고, 추가되었거나 존재하면 삭제로 처리한다
      if( !(mServerState & ITEM_STATE_MOVED) &&
        (mServerState & (ITEM_STATE_NOW_EXIST|ITEM_STATE_NEWLY_ADDED)))
        r |= FILE_NEED_SERVER_DELETE;
    }
    else
    {
      if (mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED|ITEM_STATE_NEWLY_ADDED))
        r = FILE_NEED_DOWNLOAD;
      else if (mServerState == ITEM_STATE_EXISTING) // 변동없음.
        r = FILE_NEED_DOWNLOAD;
    }
  }
  else if(mLocalState & ITEM_STATE_NEWLY_ADDED)
  {
    if ((mServerState & ITEM_STATE_NEWLY_ADDED) && IsFolder())
    {
      r = 0;
      //상태만 업데이트
      r = FILE_NEED_DOWNLOAD | FILE_NEED_UPLOAD;
    }
    else
    {
      if (syncMethod & SYNC_METHOD_BACKUP)
      {
        r = FILE_NEED_UPLOAD;
      }
      else
      {
        if(mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED|ITEM_STATE_NEWLY_ADDED))
          r = FILE_NEED_DOWNLOAD;
      }
    }
  }
  else if (mLocalState & ITEM_STATE_MOVED)
  {
    if (syncMethod & SYNC_METHOD_BACKUP)
    {
      r = FILE_NEED_SERVER_MOVE;

#ifdef USE_BACKUP 
      // 비대상 파일로 이름 변경된 경우에 삭제로 처리한다
      if (isLocalRenamedAs())
      {
        KSyncRootFolderItem* rootItem = (KSyncRootFolderItem*)((KSyncFolderItem *)mParent)->GetRootFolderItem();
        if (gpCoreEngine->IsExcludedPathName(mpLocalRenamedAs, IsFolder(), &rootItem->GetPolicy()))
        {
          mLocalState &= ~ITEM_STATE_MOVED;
          r = FILE_NEED_RETIRED;

          TCHAR msg[256] = _T("Retired by rename as out of target : ");
          StringCchCat(msg, 256, mpLocalRenamedAs);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
        }
      }
#endif
    }
    else
    {
      // 서버측 위치로 다운로드 필요 -> 새파일로 다운로드
      if(mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED|ITEM_STATE_NEWLY_ADDED))
      {
        clearLocalSideInfo();
        r = FILE_NEED_DOWNLOAD;
      }
      else if (mServerState == ITEM_STATE_EXISTING) // 변동없음
        r = FILE_NEED_DOWNLOAD;
    }
  }
  else // 변동 없음
  {
    if (syncMethod & SYNC_METHOD_BACKUP)
    {
      if((mServerState & (ITEM_STATE_DELETED|ITEM_STATE_MOVED)) ||
        (mServerState == 0))
      {
        r = FILE_NEED_UPLOAD;
        if(mServerState & ITEM_STATE_MOVED)
        {
          // upload as new file
          clearServerSideInfo();
        }
      }
      else
        r = 0;
    }
    else
    {
      if(mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED|ITEM_STATE_NEWLY_ADDED))
        r = FILE_NEED_DOWNLOAD;
    }
  }
#else
  else if ((mLocalState & ITEM_STATE_MOVED) && (mServerState & ITEM_STATE_MOVED))
  {
    // conflict only when renamed differently
    if ((mpLocalRenamedAs != NULL) && (mpServerRenamedAs != NULL) &&
      (StrCmpI(mpLocalRenamedAs, mpServerRenamedAs) != 0))
    {
      r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;
    }
    else
    {
      r = FILE_SAME_MOVED;
      if (mLocalState & ITEM_STATE_UPDATED)
      {
        if(mServerState & ITEM_STATE_UPDATED)
          r |= FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;
        else if(mServerState & ITEM_STATE_DELETED)
          r |= FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED;
        else
          r |= FILE_NEED_UPLOAD;
      }
      else
      {
        if(mServerState & ITEM_STATE_UPDATED)
          r |= FILE_NEED_DOWNLOAD;
        else if(mServerState & ITEM_STATE_DELETED)
          r |= FILE_NEED_LOCAL_DELETE | FILE_NEED_RETIRED;
      }
    }
  }
  else if (mLocalState == ITEM_STATE_NONE) // local file not exist
  {
    if(mServerState & ITEM_STATE_NOW_EXIST)
      r = FILE_NEED_DOWNLOAD;
    else if((mServerState == ITEM_STATE_NONE) || (mServerState & ITEM_STATE_DELETED))
      r = FILE_NEED_RETIRED;
  }
  else if (mLocalState & ITEM_STATE_UPDATED)
  {
    if(mServerState & ITEM_STATE_UPDATED)
      r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;
    else if(mServerState & ITEM_STATE_DELETED)
      r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED;
    else
      r = FILE_NEED_UPLOAD;
  }
  else if(mLocalState & ITEM_STATE_DELETED)
  {
    if(mServerState & ITEM_STATE_DELETED)
      r = FILE_NEED_RETIRED;
    else if(mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_MOVED))
      r = FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED;
    else if(mServerState & ITEM_STATE_NOW_EXIST)
      r = FILE_NEED_SERVER_DELETE | FILE_NEED_RETIRED;
    else // server file not exist
      r = FILE_NEED_RETIRED;
  }
  else if(mLocalState & ITEM_STATE_NEWLY_ADDED)
  {
    if(mServerState & ITEM_STATE_NEWLY_ADDED)
      r = FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED;
    else
      r = FILE_NEED_UPLOAD;
  }
  else if (mLocalState & ITEM_STATE_MOVED)
  {
    if(mServerState & ITEM_STATE_DELETED)
      r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED;
    else if(mServerState & ITEM_STATE_UPDATED)
      r = FILE_NEED_DOWNLOAD;
  }
  else
  {
    if(mServerState & ITEM_STATE_UPDATED)
      r = FILE_NEED_DOWNLOAD;
    else if(mServerState & ITEM_STATE_DELETED)
      r = FILE_NEED_LOCAL_DELETE | FILE_NEED_RETIRED;
    else if(mServerState & ITEM_STATE_NEWLY_ADDED)
      r = FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED;
    else if(mServerState & ITEM_STATE_MOVED)
      r = FILE_NEED_LOCAL_MOVE; // ignore upload/download
    else if(mServerState == ITEM_STATE_NONE) // when server location changed
      r = FILE_NEED_UPLOAD;
  }
#endif

  if (mLocalState & ITEM_STATE_REFRESH_PATH)
    r |= FILE_NEED_REFRESH_PATH;

  // 사용자 선택 방향이 있으면 먼저 적용.
  if(userAction & FILE_USER_ACTION_MASK)
    return (userAction & FILE_USER_ACTION_MASK);

  //if (((r & FILE_CONFLICT) == 0) && !(r & FILE_SAME_MOVED))
  if (!(r & FILE_SAME_MOVED) && !(r & FILE_NEED_RETIRED)) // 이동과 삭제
  {
    if (mLocalState & ITEM_STATE_MOVED)
    {
#ifndef USE_BACKUP // 백업에서는 충돌 없음
      if(mServerState & ITEM_STATE_DELETED)
        r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED;
#endif
      /*
      else if (!(mServerState & ITEM_STATE_EXISTING))
        r = FILE_NEED_UPLOAD;
      */
      if (r == 0)
        r |= FILE_NEED_SERVER_MOVE;
    }
    if (mServerState & ITEM_STATE_MOVED)
    {
#ifndef USE_BACKUP // 백업에서는 충돌 없음
      if(mLocalState & ITEM_STATE_DELETED)
        r = FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED;
#endif
      /*
      else if (!(mLocalState & ITEM_STATE_EXISTING))
        r = FILE_NEED_DOWNLOAD;
      */
      if (r == 0)
        r |= FILE_NEED_LOCAL_MOVE;
    }
  }
  return r;
}

BOOL KSyncItem::IsItemExist(LPCTSTR fullname)
{
  return IsFileExist(fullname);
}

BOOL KSyncItem::IsSyncFileExist(BOOL onLocal)
{
  if (onLocal)
  {
    return IsLocalFileExist();
  }
  else
  {
    BOOL item_exist = IsServerFileExist() || 
      (GetConflictResult() & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED);
    int conflict_err = (GetConflictResult() & FILE_SYNC_ERR);
    if (!item_exist && (conflict_err != 0) && IsValidSystemTime(mServerNew.FileTime))
    {
      item_exist = TRUE;
    }

    if (item_exist && (mServerState & ITEM_STATE_DELETED))
      item_exist = FALSE;
    return item_exist;
  }
}

int KSyncItem::GetDepth()
{
  if (mParent == NULL)
    return 0;
  return (mParent->GetDepth() + 1);
}

int KSyncItem::compareSyncFileItem(LPCTSTR baseFolder, int syncMethod)
{
  TCHAR msg[KMAX_LONGPATH+100];
  LPTSTR filename = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
  int r = 0;
  KSyncRootStorage* storage = GetSafeStorage();
  if (IsFileEditing(filename))
  {
    if (syncMethod & SYNC_METHOD_RESTORE) // 복원작업:복원오류
    {
      StringCchPrintf(msg, 400, _T("Editing file restore fail, %s"), filename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

      r = FILE_SYNC_EDITING_FILE_SKIP;
      goto end_compare;
    }
    else if (syncMethod & SYNC_METHOD_BACKUP) // 백업작업:업로드
    {
    }
    else // (syncMethod & SYNC_METHOD_SYNC) // 동기화 : 스킵
    {
    // do not update conflict status when it is in editing
    /*
    if (mConflictResult != 0)
      StoreMetafile(baseFolder, META_MASK_CONFLICT | META_MASK_SYNCRESULT);
    */

    if (IsServerModified() || isServerRenamedAs())
    {
      StoreMetafile(storage, META_MASK_NEW_SERVER_SIDE|META_MASK_RENAMED, 0);
    }

    StringCchPrintf(msg, 400, _T("Editing file skipped, %s"), filename);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
    SetPhase(PHASE_ENDING); // out of sync
    AddSyncFlag(SYNC_ITEM_EDITING_NOW);
    delete[] filename;
    return R_SUCCESS;
    }
  }

  if (checkAddedRenamedSyncError(0))
  {
    delete[] filename;
    return 0;
  }

  // KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

  if (lstrlen(filename) > KMAX_PATH)
  {
    r = FILE_SYNC_OVER_PATH_LENGTH;
    goto end_compare;
  }

  if (!ReadyMetafilename(storage, METAFILE_CREATE_IF_NOT_EXIST))
  {
    StringCchPrintf(msg, KMAX_LONGPATH+100, _T("Cannot make Metafile for %s\n"), filename);
    // OutputDebugString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    SetResult(FILE_SYNC_CANNOT_MAKE_METAFILE);
    mConflictResult = FILE_SYNC_CANNOT_MAKE_METAFILE;
    delete[] filename;
    return 0;
  }

  // if metafile does not exist, store relative file name to create metafile to avoid same metafile name.
  if ((mpMetafilename != NULL) && !IsFileExist(mpMetafilename))
    StoreMetafile(storage, META_MASK_FILENAME, 0);

  compareLocalServerState();
  int lastResult = mResult;
  int lastConflictResult = mConflictResult;

#ifdef _TEST_CONFLICT_
  // for test
  SYSTEMTIME time = GetLocalTime();
  mLocal.FileTime = time;
  int od = (rand() % 10);
  AddSystemTime(mLocalNew.FileTime, mLocal.FileTime, od, 2, 52);
  time = GetServerTime();
  mServer.FileTime = time;
  od = (rand() % 10);
  AddSystemTime(mServerNew.FileTime, mServer.FileTime, od, 2, 52);
  int test_result = (rand() % 5);
  if(test_result == 1) // modified each
  {
    setLocalState(ITEM_STATE_UPDATED);
    setServerState(ITEM_STATE_UPDATED);
  }
  else if(test_result == 2) // local-delete, server-modified
  {
    setLocalState(ITEM_STATE_DELETED);
    setServerState(ITEM_STATE_UPDATED);
  }
  else if(test_result == 3) // local-delete, server-modified
  {
    setLocalState(ITEM_STATE_UPDATED);
    setServerState(ITEM_STATE_DELETED);
  }
  else if(test_result == 4) // local-delete, server-modified
  {
    setLocalState(ITEM_STATE_NEWLY_ADDED);
    setServerState(ITEM_STATE_NEWLY_ADDED);
  }
#endif

#ifdef _DEBUG // for UpdateDocument test
  // addLocalState(ITEM_STATE_UPDATED);
#endif

  r = makeResultFromState(0, syncMethod);

end_compare:
  delete[] filename;

  // check filename length if it is normal
  if ((r & (FILE_CONFLICT | FILE_SYNC_ERR)) == 0)
  {
    if ((r & FILE_NEED_RETIRED) == 0)
    {
      // check result name
      LPTSTR resultFilename = AllocSyncResultName(((r & FILE_NEED_UPLOAD) ? AS_LOCAL : 0) | AS_FULLPATH);
      int len = lstrlen(resultFilename);
      delete[] resultFilename;
      if (len > MAX_FILE_PATH_LENGTH)
      {
        StringCchPrintf(msg, KMAX_LONGPATH+100, _T("LongFileName Skip : %s, len=%d"), mLocal.pFilename, len);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        mResult = (r & FILE_NEED_UPLOAD) ? FILE_SYNC_CANNOT_UPLOAD_LONGPATH : FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH;
        mConflictResult = mResult;
        mEnable = !(mConflictResult & FILE_SYNC_ERR);
        return 0;
      }
    }
  }

  if (r != FILE_NOT_CHANGED)
    SetResult(r);

  StringCchPrintf(msg, KMAX_LONGPATH+100, _T("name=%s, result=%s, LS=0x%x, SS=0x%x"), mLocal.pFilename, 
    GetSyncCompareResultString(r), mLocalState, mServerState);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  SetResult(r);

  // default conflict item result.
#if 0
  mConflictResult = mResult & (FILE_CONFLICT | FILE_SYNC_ERR);
  if (mResult & FILE_CONFLICT)
  {
    SYSTEMTIME local = GetLocalTime();
    SYSTEMTIME server = GetServerTime();
    BOOL local_new = (CompareSystemTime(local, server) > 0);

    if (mResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)
    {
      mConflictResult |= (local_new ? FILE_NEED_UPLOAD : FILE_NEED_DOWNLOAD); // server is recent item
    }
    else if (mResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED) // local modified, server deleted
    {
      mConflictResult |= FILE_NEED_UPLOAD; // or delete local file
    }
    else if (mResult & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED) // local deleted, server modified
    {
      mConflictResult |= FILE_NEED_DOWNLOAD; // or delete server file
    }
    else if (mResult & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED) // local added, server added
    {
      mConflictResult |= (local_new ? FILE_NEED_UPLOAD : FILE_NEED_DOWNLOAD);
    }
  }
#endif
  mConflictResult = mResult | (mConflictResult & FILE_SYNC_RENAMED);

  // check Moved/Renamed on local & server
  if (isLocalRenamedAs() && isServerRenamedAs())
  {
    if (!(mResult & FILE_SAME_MOVED))
    {
      // compare server renamed time and local create time.
      // it should be conflict

      // no direction proposal
      /*
      if (CompareSystemTime(mLocalNew.AccessTime, mServerNew.FileTime) > 0) // local is later then server
        mResult |= FILE_NEED_UPLOAD;
      else
        mResult |= FILE_NEED_DOWNLOAD;
      */

      mConflictResult |= (mResult & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD|FILE_SAME_MOVED));
    }
  }

  if ((lastResult != mResult) || (lastConflictResult != mConflictResult))
  {
    // 결과를 업데이트할 때 서버 오류 메시지도 클리어한다
    if (mpServerConflictMessage != NULL)
    {
      delete[] mpServerConflictMessage;
      mpServerConflictMessage = NULL;
    }
    StoreMetafile(storage, META_MASK_CONFLICT|META_MASK_SYNCRESULT|META_MASK_LOCALSTATE|META_MASK_SERVERSTATE|META_MASK_RENAMED);
  }

  /* do not disable when sync error
  mEnable = !(mConflictResult & FILE_SYNC_ERR);
  */
  return r;
}

BOOL KSyncItem::checkNewFolderExcluded(LPTSTR newPathName)
{
  KSyncRootFolderItem* root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  if (root != NULL)
  {
    KSyncItem *item = root->FindChildItemCascade(newPathName, 3, WITH_RENAMED|ONLY_EXISTING);
    if ((item != NULL) && item->IsExcludedSyncRecur())
      return TRUE;
  }
  return FALSE;
}

void KSyncItem::compareLocalServerState()
{
  TCHAR msg[256];
  int server_no_permission = (mServerState & ITEM_STATE_NO_PERMISSION);

  int lo_cr = mLocalState & (ITEM_STATE_CONFLICT_RENAMED | ITEM_STATE_REFRESH_PATH);
  int sv_cr = mServerState & (ITEM_STATE_CONFLICT_RENAMED | ITEM_STATE_RESTORED);

  mLocalState = mLocalState & ITEM_STATE_EXISTING;
  mServerState = mServerState & ITEM_STATE_EXISTING;
  if (mLocalState == ITEM_STATE_NOW_EXIST) // local file is added newly
  {
    addLocalState(ITEM_STATE_NEWLY_ADDED);
    if (!IsLocalFileExist())
      addLocalState(ITEM_STATE_DELETED);
  }
  else if (mLocalState == ITEM_STATE_WAS_EXIST) // info exist & file not exist => deleted
    addLocalState(ITEM_STATE_DELETED);
  /*
  if ((mLocalState & (ITEM_STATE_IFO_EXIST|ITEM_STATE_NEWLY_ADDED)) == ITEM_STATE_NEWLY_ADDED)
  {
    // There is no info file, it will be a newly added item
    mLocal.FileSize = mLocalNew.FileSize;
    memcpy(&mLocal.FileTime, &mLocalNew.FileTime, sizeof(mLocalNew.FileTime));
    mServer.FileSize = mServerNew.FileSize;
    StringCchCopy(mServer.UserOID, MAX_OID, mServerNew.UserOID);
    // create info file
    StoreMetafile(baseFolder);
  }
  */
  else if(mLocalState == (ITEM_STATE_NOW_EXIST|ITEM_STATE_WAS_EXIST))
  {
    if(!IsFolder() && IsLocalModified())
      // (CompareSystemTime(mLocalNew.FileTime, mLocal.FileTime) != 0)) // local changed
    {
      addLocalState(ITEM_STATE_UPDATED);
      TCHAR oldtime[64];
      TCHAR newtime[64];
      SystemTimeToTString(oldtime, 64, mLocal.FileTime);
      SystemTimeToTString(newtime, 64, mLocalNew.FileTime);

      StringCchPrintf(msg, 256, _T("old Modified=%s, new Modified=%s"), oldtime, newtime);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

#ifdef _DEBUG // for UpdateDocument test
    // addLocalState(ITEM_STATE_UPDATED);
#endif
  }
  
  if (mServerState == ITEM_STATE_NOW_EXIST) // server file is added newly
    addServerState(ITEM_STATE_NEWLY_ADDED);
  else if (mServerState == ITEM_STATE_WAS_EXIST) // info exist & file not exist => deleted
    addServerState(ITEM_STATE_DELETED);
  else if(mServerState == (ITEM_STATE_NOW_EXIST|ITEM_STATE_WAS_EXIST))
  {
    BOOL modified = FALSE;

    if(IsFolder())
    {
      if ((mServer.pFullPathIndex != NULL) && (lstrlen(mServer.pFullPathIndex) > 0) &&
        (mServerNew.pFullPathIndex != NULL) && (lstrlen(mServerNew.pFullPathIndex) > 0))
      modified = (StrCmp(mServer.pFullPathIndex, mServerNew.pFullPathIndex) != 0);
    }
    else
    {
      modified = IsServerModified();
    }

    if (modified)
    {
      addServerState(ITEM_STATE_UPDATED);

      if (StrCmp(mServer.StorageOID, mServerNew.StorageOID) != 0)
        StringCchPrintf(msg, 256, _T("old storageOID=%s, new storageOID=%s"), mServer.StorageOID, mServerNew.StorageOID);
      else if (StrCmp(mServer.FileOID, mServerNew.FileOID) != 0)
        StringCchPrintf(msg, 256, _T("old fileOID=%s, new fileOID=%s"), mServer.FileOID, mServerNew.FileOID);
      else if(IsFolder())
        StringCchPrintf(msg, 256, _T("old fullPathIndex=%s, new fullPathIndex=%s"), mServer.pFullPathIndex, mServerNew.pFullPathIndex);
      else
        msg[0] = '\0';

      if (lstrlen(msg) > 0)
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    else if(mLocalState == 0)
    {
      // 로컬 생성이 실패한 경우 다시 시도하도록
      addServerState(ITEM_STATE_UPDATED);
    }
  }

  // if item is deleted, clear rename info
  if (mLocalState & ITEM_STATE_DELETED)
  {
    if (mpLocalRenamedAs != NULL)
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
  }
  if (mServerState & ITEM_STATE_DELETED)
  {
    if (mpServerRenamedAs != NULL)
    {
      delete[] mpServerRenamedAs;
      mpServerRenamedAs = NULL;
    }
  }
  if ( validLocalRenamedAs() ) // isLocalRenamedAs())
  {
    // 이동된 곳이 제외 폴더이면 삭제로 처리
    if(checkNewFolderExcluded(mpLocalRenamedAs))
      addLocalState(ITEM_STATE_DELETED);
    else
      addLocalState(ITEM_STATE_MOVED);
  }
  if ( isServerRenamedAs())
  {
    if(checkNewFolderExcluded(mpServerRenamedAs))
      addServerState(ITEM_STATE_DELETED);
    else
      addServerState(ITEM_STATE_MOVED);
  }
  /*
  if (mConflictResult == (FILE_NEED_UPLOAD|FILE_SYNC_RENAMED))
    mLocalState |= ITEM_STATE_NEWLY_ADDED;
  else if (mConflictResult == (FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED))
    mServerState |= ITEM_STATE_NEWLY_ADDED;
  */
  if (lo_cr & ITEM_STATE_CONFLICT_RENAMED)
    mLocalState |= ITEM_STATE_CONFLICT_RENAMED|ITEM_STATE_NEWLY_ADDED;
  if (sv_cr & ITEM_STATE_CONFLICT_RENAMED)
    mServerState |= ITEM_STATE_CONFLICT_RENAMED|ITEM_STATE_NEWLY_ADDED;

  if (lo_cr & ITEM_STATE_REFRESH_PATH)
    mLocalState |= ITEM_STATE_REFRESH_PATH;

  if (sv_cr & ITEM_STATE_RESTORED) // 삭제후 다시 복구된 경우.
  {
    StringCchPrintf(msg, 256, _T("Deleted & Restored, %s"), mpRelativePath);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  if (mConflictResult & FILE_NOW_ORPHAN)
  {
    if (mConflictResult & FILE_NEED_UPLOAD)
    {
      if ((sv_cr & ITEM_STATE_RESTORED) == 0) // 삭제후 다시 복구된 경우가 아닐때.
        mLocalState |= ITEM_STATE_UPDATED; // set as updated to enable upload
    }
    else if (mConflictResult & FILE_NEED_DOWNLOAD)
    {
      mServerState |= ITEM_STATE_UPDATED; // set as updated to enable download
    }
  }

  mServerState |= server_no_permission;
}

BOOL KSyncItem::checkAddedRenamedSyncError(int user_action)
{
#ifndef USE_BACKUP
  //TCHAR filename[KMAX_LONGPATH];
  //GetFullPathName(filename, KMAX_LONGPATH);
  LPTSTR pathName = AllocFullPathName();

  // at first, check if server-newly-added, local-exist-same(previous item renamed)
  bool active = ((user_action == 0) || (user_action & FILE_NEED_DOWNLOAD));
  if (active &&
    ((mServerState & ITEM_STATE_EXISTING) == ITEM_STATE_NOW_EXIST) && // server file is added newly
    ((mLocalState & ITEM_STATE_EXISTING) == 0))
  {
    if (IsFileExistAttribute(pathName))
    {
      SetResult(FILE_SYNC_ALREADY_EXISTS_ON_LOCAL);
      mConflictResult = FILE_SYNC_ALREADY_EXISTS_ON_LOCAL;
      delete[] pathName;
      return TRUE;
    }
  }
  delete[] pathName;

  active = ((user_action == 0) || (user_action & FILE_NEED_UPLOAD));
  if (active &&
    ((mLocalState & ITEM_STATE_EXISTING) == ITEM_STATE_NOW_EXIST) && // local file is added newly
    ((mServerState & ITEM_STATE_EXISTING) == 0))
  {
    // TCHAR relativeName[KMAX_LONGPATH];
    // GetRelativePathName(relativeName, filename);
    pathName = AllocRelativePathName();
    KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    if ((root != NULL) && root->IsExistServerRenamed(pathName))
    {
      SetResult(FILE_SYNC_ALREADY_EXISTS_ON_SERVER);
      mConflictResult = FILE_SYNC_ALREADY_EXISTS_ON_SERVER;
      delete[] pathName;
      return TRUE;
    }
    delete[] pathName;
  }
#endif
  return FALSE;
}

void KSyncItem::SetUnchangedItemPhase(int added_phase, int renamed_phase, int normal_phase, BOOL parentCheckNeed)
{
  if (GetResult() == FILE_NOT_CHANGED)
  {
    if (isLocalRenamedAs() || isServerRenamedAs())
      SetPhase(renamed_phase);
    else
      SetPhase(normal_phase);
  }
}

void KSyncItem::SetConflictResultRecurse(int result, int phase, int flag)
{
  if (!(flag & SET_SELECTED_ONLY) || IsSelected())
  {
    int meta_flag = META_MASK_CONFLICT | META_MASK_SYNCRESULT;
    if (flag & SET_SERVER_DELETED)
      meta_flag |= META_MASK_SERVER_SIDE|META_MASK_RENAMED;
    if (flag & SET_LOCAL_DELETED)
      meta_flag |= META_MASK_LOCAL_SIDE|META_MASK_RENAMED;

    result = GetResult() | result;

    // 로컬삭제 상태에서 업로드 선택하면 삭제 취소
    if ((result & FILE_NEED_UPLOAD) && (mResult & FILE_NEED_LOCAL_DELETE))
      result &= ~FILE_NEED_LOCAL_DELETE;
    if ((result & FILE_NEED_DOWNLOAD) && (mResult & FILE_NEED_SERVER_DELETE))
      result &= ~FILE_NEED_SERVER_DELETE;

    SetResult(result);
    SetConflictResult(result);

    if (flag & SET_SERVER_DELETED)
      clearServerSideInfo();
    if (flag & SET_LOCAL_DELETED)
    {
      clearLocalSideInfo();
      mServerState |= ITEM_STATE_NEWLY_ADDED;
    }
    SetPhaseForce(phase); // 역으로 바꾼다.
    SetEnable(TRUE);

    if (flag & STORE_META)
      StoreMetafile(NULL, meta_flag);
  }
}

// return TRUE if item is enabled
BOOL KSyncItem::SetPhaseOnResolve(int parent_phase, int renamed_phase, int normal_phase)
{
  if (mEnable)
  {
    if (isLocalRenamedAs() || isServerRenamedAs())
      SetPhaseForce(renamed_phase);
    else
      SetPhaseForce(normal_phase);
  }
  else
    SetPhaseForce(PHASE_END_OF_JOB);
  return mEnable;
}

int KSyncItem::ReadyResolve(KSyncItem *root, int phase, int syncMethod)
{
  if (!mEnable)
    return R_SUCCESS;

  // check if it is renamed during sync
#if 0
  TCHAR fullname[KMAX_LONGPATH];
  TCHAR relativeName[KMAX_PATH];
  MakeFullPathName(fullname, KMAX_LONGPATH, ((KSyncRootFolderItem *)root)->GetBaseFolder(), mpRelativePath); // old local full name
  GetRelativePathName(relativeName, fullname);
  if (((KSyncRootFolderItem *)root)->GetMovedRenamedAs(relativeName, mLocalRenamedAs))
    mLocalState |= ITEM_STATE_MOVED;
#else
  LPTSTR relativeName = AllocRelativePathName();

  // 2017-09 : 여기서 로컬 이름변경 새로 업데이트 하여야 함.
  if (mpLocalRenamedAs != NULL)
  {
    delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }
  // 동일한 이름으로 서버에 추가된 것 때문에 로컬 존재하는 것만 rename 정보 갱신.
  if (mLocalState & ITEM_STATE_EXISTING)
    mpLocalRenamedAs = AllocMovedRenamedAs(relativeName);

  // 상위 폴더가 이름 변경 처리 완료된 상태에서는 AllocMovedRenamedAs()에서 새 경로명을 가져오지 못함
  // -> 정상

  if ((mpLocalRenamedAs != NULL) &&
    !(mLocalState & ITEM_STATE_EXISTING))
  {
    delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }

  if (mpLocalRenamedAs != NULL)
  {
    TCHAR msg[200];
    StringCchPrintf(msg, 200, _T("localRenamedAs : %s"), mpLocalRenamedAs);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    mLocalState |= ITEM_STATE_MOVED;
  }
  delete[] relativeName;
#endif

  int user_action = (mConflictResult & FILE_USER_ACTION_MASK);
  int d = 0;
  BOOL server_changed = FALSE;
  BOOL local_changed = FALSE;

  if (phase == PHASE_CONFLICT_FILES)
  {
    TCHAR msg[300];

    StringCchPrintf(msg, 300, _T("%s, user action"), GetFilename());
    if (user_action & FILE_NEED_DOWNLOAD)
      StringCchCat(msg, 300, _T(", download"));
    if (user_action & FILE_NEED_UPLOAD)
      StringCchCat(msg, 300, _T(", upload"));
    if (user_action & FILE_NEED_LOCAL_DELETE)
      StringCchCat(msg, 300, _T(", local-delete"));
    if (user_action & FILE_NEED_SERVER_DELETE)
      StringCchCat(msg, 300, _T(", server-delete"));
    if (user_action == 0)
      StringCchCat(msg, 300, _T(", none"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  // check if server-file is modified again after sync-compare. 150922
  // when user force to upload in resolve conflict.
  if (phase == PHASE_CONFLICT_FILES)
  {
    server_changed = IsServerModified();

    SERVER_INFO serverInfo;
    memset(&serverInfo, 0, sizeof(serverInfo));
    int d = (CheckServerModifiedAgain(serverInfo) & 0x11FF); // ignore NO_PERMISSION hear

    if (d != 0)
    {
      addServerState(d);
      if ((d == ITEM_STATE_UPDATED) && (user_action & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD)))
      {
        // modified after sync-compare
        addServerState(ITEM_STATE_RE_UPDATED);

        // 충돌 처리에서 서버로 적용한 경우에, 다시 RE_UPDATE 발생하지 않도록 mServerlNew에 업데이트.
        MergeServerInfo(mServerNew, serverInfo);

        FreeServerInfo(serverInfo);
        return R_SUCCESS;
      }
    }

    if (d & ITEM_STATE_DELETED) // server file is removed
    {
      mServerState &= ~(ITEM_STATE_NOW_EXIST|ITEM_STATE_UPDATED|ITEM_STATE_MOVED);
      addServerState(ITEM_STATE_DELETED);
    }

    if (mServerState & ITEM_STATE_RE_UPDATED)
      server_changed = TRUE;

    if ((mConflictResult & FILE_SYNC_ERR) == FILE_SYNC_NOT_EXIST_DRIVE)
      server_changed = TRUE;

    // permission update
    if (mServerNew.Permissions != serverInfo.Permissions)
    {
      mServerNew.Permissions = serverInfo.Permissions;
      //OutputDebugString(_T("permission changed"));
    }
    FreeServerInfo(serverInfo);
  }

  KSyncRootFolderItem *rootItem = (KSyncRootFolderItem *)root;
  LPCTSTR baseFolder = rootItem->GetBaseFolder();

  if (IsLocalFileExist() && (mLocalState & ITEM_STATE_EXISTING))
  {
    // check if local-file is modified again after sync-compare
    if (IsFolder() == 0)
      local_changed = IsLocalModified();
    /*
    UpdateLocalNewInfo(baseFolder);
    RefreshLocalNewFileInfo(mUserOid);
    */
    if (IsLocalModifiedAgain(FALSE) && (user_action & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD)))
    {
      // modified after sync-compare
      addLocalState(ITEM_STATE_RE_UPDATED);

      // 충돌 처리에서 로컬로 적용한 경우에, 다시 RE_UPDATE 발생하지 않도록 mLocalNew에 업데이트.
      if (phase == PHASE_CONFLICT_FILES)
        IsLocalModifiedAgain(TRUE);
      return R_SUCCESS;
    }

    if (IsFolder() == 0)
    {
      LPTSTR resultFilename = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
      if (IsFileEditing(resultFilename))
        AddSyncFlag(SYNC_ITEM_EDITING_NOW);
      else
        RemoveSyncFlag(SYNC_ITEM_EDITING_NOW);
      delete[] resultFilename;
    }
  }
  else
  {
    memset(&mLocalNew.FileTime, 0, sizeof(mLocalNew.FileTime));
  }

  // if (!IsValidSystemTime(mLocalNew.FileTime)) // file is removed
  // ResolveConflict 모드일때 mLocalNew.FileTime가 NULL인 경우 있어서 실제 파일 존재하는지 확인.

  // check if it is moved into excluded folder
  if (isLocalRenamedAs()) // (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    LPTSTR movedPath = NULL;
    SplitPathNameAlloc(mpLocalRenamedAs, &movedPath, NULL);
    if (lstrlen(movedPath) > 0)
    {
      KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
      KSyncFolderItem *p = (KSyncFolderItem *) rootFolder->FindChildItemCascade(movedPath, 1);

      /*
      if (p == NULL)
      {
        // 자신의 부모가 이동된 경우.
        if ((mParent != NULL) && mParent->IsLocalMovedAs(movedPath))
          p = (KSyncFolderItem *)mParent;
      }
      */

      if (p == NULL)
      {
        TCHAR msg[300];
        StringCchCopy(msg, 300, mLocal.pFilename);
        StringCchCat(msg, 300, _T(" local moved nowhere to "));
        StringCchCat(msg, 300, mpLocalRenamedAs);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        delete[] mpLocalRenamedAs;
        mpLocalRenamedAs = NULL; // moved to nowhere or excluded folder.
        mLocalState &= ~ITEM_STATE_MOVED;

        if (!(mLocalState & ITEM_STATE_NEWLY_ADDED))
          mLocalState &= ~ITEM_STATE_NOW_EXIST; // 삭제 상태로 처리
      }
      delete[] movedPath;
    }
  }

  if (isServerRenamedAs())
  {
    LPTSTR movedPath = NULL;
    SplitPathNameAlloc(mpServerRenamedAs, &movedPath, NULL);
    if (lstrlen(movedPath) > 0)
    {
      KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
      KSyncFolderItem *p = (KSyncFolderItem *) rootFolder->FindChildItemCascade(movedPath, 1);

#if 0
      /* 폴더 이동시에 하위 항목의 mpServerRenamedAs가 새 경로로 업데이트되니 아래는 불필요함. */

      if (p == NULL) // 폴더가 이미 이동된 상태일 수 있음
      {
        LPTSTR updatedPath = rootFolder->AllocMovedRenamedAs(movedPath);
        if (updatedPath != NULL)
        {
          p = (KSyncFolderItem *) rootFolder->FindChildItemCascade(updatedPath, 1);
          delete[] updatedPath;
        }
      }
#endif

      if ((p == NULL) || p->IsExcludedSync())
      {
        TCHAR msg[300];
        StringCchCopy(msg, 300, mLocal.pFilename);
        StringCchCat(msg, 300, _T(" server moved nowhere to "));
        StringCchCat(msg, 300, mpServerRenamedAs);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        delete[] mpServerRenamedAs;
        mpServerRenamedAs = NULL; // moved to nowhere or excluded folder.

        mServerState &= ~ITEM_STATE_MOVED;
        mServerState &= ~ITEM_STATE_NOW_EXIST; // 삭제 상태로 처리
        // mServerState |= ITEM_STATE_DELETED;
      }
      delete[] movedPath;
    }
  }

  /*
  동기화 제외 폴더로 설정되면 서버측 변경 정보가 무시되니
  다시 동기화 포함한 후에 동기화 시도하면 서버측 정보 받아오지 못함.
  따라서 동기화 포함하게 되면 다음 동기화에서 해당 폴더를 다시 스캔하여야 함.
  */

  // 동일한 이름을 가진 항목 존재할 수 있으니, 새로 추가된 것은 검사 안함
  if ((mLocalState & ITEM_STATE_WAS_EXIST) && !(mLocalState & ITEM_STATE_DELETED))
  {
    if (!IsLocalFileExist())
      mLocalState &= ~(ITEM_STATE_NOW_EXIST|ITEM_STATE_UPDATED|ITEM_STATE_MOVED);
    else
      mLocalState |= ITEM_STATE_NOW_EXIST;
  }

  if (mLocalState & ITEM_STATE_RE_UPDATED)
    local_changed = TRUE;

  if (checkAddedRenamedSyncError(user_action))
    return R_SUCCESS;

  compareLocalServerState();

  if (server_changed)
    addServerState(ITEM_STATE_UPDATED);
  if (local_changed && (mLocalState & ITEM_STATE_NOW_EXIST))
    addLocalState(ITEM_STATE_UPDATED);

#if 1
  int rootLen = lstrlen(baseFolder);
  if (rootLen > 0)
    rootLen++; // for directory delimeter

  // check valid local-rename
  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    TCHAR actualPathName[MAX_PATH];
    mParent->GetActualPathName(actualPathName, MAX_PATH, FALSE);
    MakeFullPathName(actualPathName, KMAX_PATH, actualPathName, mLocal.pFilename);

    if (StrCmpI(mpLocalRenamedAs, actualPathName) == 0)
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;

      mResult &= ~FILE_NEED_LOCAL_MOVE;
      mLocalState &= ~ITEM_STATE_MOVED;
      if (IsFolder())
        mLocalState &= ~ITEM_STATE_UPDATED;
    }
  }
#endif

#if 0 // done on RefreshLocalNewFileInfo
  // check local renamed
  TCHAR fullname[KMAX_PATH];
  MakeFullPathName(fullname, KMAX_PATH, baseFolder, mRelativeName); // old local full name

  if ((lstrlen(mLocalRenamedAs) == 0) &&
  // if ( // !(mLocalState & ITEM_STATE_DELETED) &&
    rootItem->GetMovedRenamedAs(&fullname[rootLen], mLocalRenamedAs))
  {
    mLocalState |= ITEM_STATE_MOVED;
    mLocalState &= ~ITEM_STATE_DELETED;
    MakeFullPathName(fullname, KMAX_PATH, baseFolder, mLocalRenamedAs);
  }
  if (lstrlen(mLocalRenamedAs) > 0)
    RefreshLocalNewFileInfo(mUserOid);
#endif

  // check server is renamed
  if (mServerState & (ITEM_STATE_NOW_EXIST|ITEM_STATE_WAS_EXIST))
  {
    if (!(mServerState & ITEM_STATE_DELETED) &&
      (lstrlen(mServer.pFullPathIndex) > 0) && (lstrlen(mServerNew.pFullPathIndex) > 0))
    {
      if (StrCmp(mServer.pFullPathIndex, mServerNew.pFullPathIndex) != 0)
      {
        mServerState |= ITEM_STATE_MOVED;
        if (IsFolder())
        {
          LPCTSTR serverRelativeName = mServerNew.pFullPath;

          if ((mpServerRenamedAs != NULL) && (StrCmpI(serverRelativeName, mpServerRenamedAs) != 0))
          {
            AllocCopyString(&mpServerRenamedAs, serverRelativeName);
          }
        }
      }
    }

#if 1
    if (mServerState & ITEM_STATE_MOVED)
    {
      LPTSTR newPath = NULL;
      LPTSTR newName = NULL;
      LPTSTR serverPathName = AllocServerSidePathName();
      SplitPathNameAlloc(serverPathName, &newPath, &newName);
      CheckServerRenamedAs(newPath, newName);

      if (newPath != NULL)
        delete[] newPath;
      if (newName != NULL)
        delete[] newName;

        if (mpServerRenamedAs == NULL) // clear rename
        {
          mServerState &= ~(ITEM_STATE_MOVED|ITEM_STATE_UPDATED);
          mLocalState |= ITEM_STATE_REFRESH_PATH;
        }

      // LPTSTR localPathName = AllocRelativePathName();

      // 자신이 이동된 것인지, 부모가 이동된 것인지 확인

      /*
      // 서버측 경로명이 바뀌었는지 확인 생략, 경로명이 동일하더라도 FullPathIndex를 업데이트하여야 한다.
      if (StrCmp(serverPathName, localPathName) != 0)
      {
        mServerState |= ITEM_STATE_MOVED;
        if (mpServerRenamedAs != NULL)
          delete[] mpServerRenamedAs;
        mpServerRenamedAs = serverPathName;
        serverPathName = NULL;
      }
      else
      {
        mServerState &= ~ITEM_STATE_MOVED;
        if (mpServerRenamedAs != NULL)
        {
          delete[] mpServerRenamedAs;
          mpServerRenamedAs = NULL;
        }
        if (IsFolder())
          mServerState &= ~ITEM_STATE_UPDATED;
      }
      */

      // delete[] localPathName;

      if (serverPathName != NULL)
        delete[] serverPathName;
    }
#endif
  }

  // if user force to delete, no need rename/move
  if ((user_action & (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE)) == 
    (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE))
  {
    mServerState &= ~ITEM_STATE_MOVED;
    mLocalState &= ~ITEM_STATE_MOVED;
  }

  if ((mConflictResult & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED) &&
    ((mServerState & ITEM_STATE_MOVED) || (mLocalState & ITEM_STATE_MOVED)))
  {
    // Split each other
    KSyncItem *nitem = DuplicateItem();

    CloneLocalRename(rootItem, nitem);
    ((KSyncFolderItem *)mParent)->AppendChild(this, nitem);
    return R_SUCCESS;
  }

  // sync error item, user force upload/download
  if (mConflictResult & FILE_SYNC_ERR)
  {
    if ((mLocalState & ITEM_STATE_UPDATED) || 
      (mServerState & ITEM_STATE_UPDATED))
    {
      if (mConflictResult & FILE_NEED_UPLOAD)
      {
        if (mLocalState & ITEM_STATE_EXISTING)
          mLocalState |= ITEM_STATE_UPDATED;

        // 충돌처리인 경우 서버측 UPDATE, DELETED 플래그 삭제, 아니면 다시 충돌 발생.
        if (phase == PHASE_CONFLICT_FILES)
          mServerState &= ~(ITEM_STATE_UPDATED|ITEM_STATE_DELETED);
      }
      if (mConflictResult & FILE_NEED_DOWNLOAD)
        mServerState |= ITEM_STATE_UPDATED;
    }
    else if ((mLocalState & ITEM_STATE_MOVED) ||
      (mServerState & ITEM_STATE_MOVED))
    {
      if (mConflictResult & FILE_NEED_UPLOAD)
      {
        d |= FILE_NEED_SERVER_MOVE;
        mLocalState |= ITEM_STATE_MOVED;
        mServerState &= ~ITEM_STATE_MOVED;
      }
      if (mConflictResult & FILE_NEED_DOWNLOAD)
      {
        d |= FILE_NEED_LOCAL_MOVE;
        mServerState |= ITEM_STATE_MOVED;
        mLocalState &= ~ITEM_STATE_MOVED;
      }
    }
  }

  d = makeResultFromState(mConflictResult & FILE_USER_ACTION_MASK, syncMethod);
  if (user_action)
  {
#ifdef USE_BACKUP
    // 서버측이 존재하지 않을때, 로컬로 반영하면 생략.
    if ((user_action == FILE_NEED_DOWNLOAD) &&
      ((mServerState & ITEM_STATE_DELETED) || !(mServerState & ITEM_STATE_EXISTING)))
      user_action = 0;
    // 로컬이 존재하지 않을때 서버로 반영하면 삭제로 동작.
    else if ((user_action == FILE_NEED_UPLOAD) && 
      ((mLocalState & ITEM_STATE_DELETED) || !(mLocalState & ITEM_STATE_EXISTING)))
      user_action = FILE_NEED_SERVER_DELETE|FILE_NEED_LOCAL_DELETE;
#else
    // 서버측이 존재하지 않을때, 로컬로 반영하면 삭제로 동작.
    if ((user_action == FILE_NEED_DOWNLOAD) &&
      ((mServerState & ITEM_STATE_DELETED) || !(mServerState & ITEM_STATE_EXISTING)))
      user_action = FILE_NEED_SERVER_DELETE|FILE_NEED_LOCAL_DELETE;
    // 로컬이 존재하지 않을때 서버로 반영하면 삭제로 동작.
    else if ((user_action == FILE_NEED_UPLOAD) && 
      ((mLocalState & ITEM_STATE_DELETED) || !(mLocalState & ITEM_STATE_EXISTING)))
      user_action = FILE_NEED_SERVER_DELETE|FILE_NEED_LOCAL_DELETE;
#endif
    d = (d & ~FILE_USER_ACTION_MASK) | user_action;
    if (!(mLocalState & ITEM_STATE_EXISTING) && (d & FILE_NEED_LOCAL_DELETE))
      d &= ~FILE_NEED_LOCAL_DELETE;
    if (!(mServerState & ITEM_STATE_EXISTING) && (d & FILE_NEED_SERVER_DELETE))
      d &= ~FILE_NEED_SERVER_DELETE;

    // in the case of SYNC_ERROR
    if (mConflictResult & FILE_SYNC_ERR)
    {
      if (mServerState & ITEM_STATE_DELETED)
      {
        if (mConflictResult & FILE_NEED_UPLOAD)
        {
          d = FILE_NEED_UPLOAD;
          //mServerState &= ~ITEM_STATE_DELETED; // clear after done
        }
        else if (mConflictResult & FILE_NEED_DOWNLOAD)
          d = FILE_NEED_LOCAL_DELETE;
      }

      if (mLocalState & ITEM_STATE_DELETED)
      {
        if (mConflictResult & FILE_NEED_DOWNLOAD)
        {
          if (!(mServerState & ITEM_STATE_DELETED))
            d = FILE_NEED_DOWNLOAD;
          // mLocalState &= ~ITEM_STATE_DELETED; // clear after done
        }
        else if (mConflictResult & FILE_NEED_UPLOAD)
          d = FILE_NEED_SERVER_DELETE;
      }
    }

    // 양쪽 정상 상태인데 user_action이 있으면 무시한다
    if ((mLocalState == ITEM_STATE_EXISTING) && (mServerState == ITEM_STATE_EXISTING))
    {
      d = 0;
    }
    else
    {
      if (d & (FILE_NEED_LOCAL_DELETE | FILE_NEED_SERVER_DELETE))
        d |= FILE_NEED_RETIRED;
      else
        d &= ~FILE_NEED_RETIRED;
    }
  }

  // Copy phase에서 copy/delete 행위 없으면 이동 클리어.(파일 교차 이름변경시 문제 발생)
  if ((phase >= PHASE_COPY_FILES) &&
    ((d & (FILE_NEED_DOWNLOAD|FILE_NEED_UPLOAD|FILE_NEED_LOCAL_DELETE | FILE_NEED_SERVER_DELETE)) == 0))
    d &= ~(FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE);

  // 서버측 경로와 DB의 경로을 업데이트하여야 함.
  if ((d == 0) && (mLocalState & ITEM_STATE_REFRESH_PATH))
    d |= FILE_NEED_REFRESH_PATH;

  if ((d == 0) && (mResult != 0))
  {
    d = FILE_NEED_REFRESH_META;
  }

  SetConflictResult(d);
  SetResult(d);

  return R_SUCCESS;
}

int KSyncItem::doSyncItem(KSyncPolicy *p, LPCTSTR baseFolder, int phase)
{
  int d = 0;
  int r = 0;
  TCHAR msg[600];
  int meta_mask = 0;
  BOOL deleteMeta = FALSE;
  int renamed = 0;

  // clear sync error message
  if (mpServerConflictMessage != NULL)
  {
    delete[] mpServerConflictMessage;
    mpServerConflictMessage = NULL;
  }

#ifdef RENAME_ON_SYNC_TIME
  if ((lstrlen(mLocalRename) > 0) ||
    ((lstrlen(mServerRename) > 0) && (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD)))
  {
    // make clone object
    KSyncItem *item = new KSyncItem(mParent);
    *item = *this;

    if (lstrlen(mLocalRename) > 0)
    {
      // current item : rename & upload
      // clone item : download
      CloneLocalRename(item);
    }
    if ((lstrlen(mServerRename) > 0) && (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD))
    {
      // rename first to avoid upload & download conflict
      item->renameServer(mServerRename);
      item->mServerRename[0] = '\0';

      item->CloneServerRename(this);
    }

    ((KSyncFolderItem *)mParent)->AddChild(item);
  }
#endif

  SetPhase(phase);
  if (phase == PHASE_LOAD_FILES)
  {
    d = GetResult();
    d &= (FILE_SAME_MOVED| FILE_NEED_SERVER_MOVE| FILE_NEED_LOCAL_MOVE);
  }
  else if ((phase == PHASE_MOVE_FOLDERS) || (phase == PHASE_COPY_FILES))
  {
    d = GetResult();

    if (d & (FILE_CONFLICT | FILE_SYNC_ERR))
    {
      d &= (FILE_CONFLICT | FILE_SYNC_ERR);
      r = d;

      StringCchPrintf(msg, 600, _T("name=%s "), mLocal.pFilename);
      goto end_sync_copy;
    }

    // Copy phase에서 copy/delete 행위 없으면 이동 클리어.(파일 교차 이름변경시 문제 발생)
    if ((phase == PHASE_COPY_FILES) &&
      ((d & (FILE_NEED_DOWNLOAD|FILE_NEED_UPLOAD|FILE_NEED_LOCAL_DELETE | FILE_NEED_SERVER_DELETE)) == 0))
      d &= ~(FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE);

    // Move phase에서는 삭제 및 retire 처리하지 않는다.
    if (phase == PHASE_MOVE_FOLDERS)
    {
      d &= ~(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE|FILE_NEED_RETIRED);
    }
  }
  else if (phase == PHASE_CONFLICT_FILES)
  {
    d = GetConflictResult();
    if ((d & FILE_CONFLICT) &&
      !(d & FILE_USER_ACTION_MASK)) // upload, download, delete-local, delete-server
    {
      // mEnable = 0; 
      // user did not select direction in conflict-state, it should be new conflict
      r = (d & FILE_CONFLICT);
      goto end_sync_copy;
    }

    if (!mEnable)
    {
      StringCchPrintf(msg, 400, _T("path=%s, skipped"), GetPathString());
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
      return R_SUCCESS;
    }

    // check if rename need, 이름변경이 사용자 선택에 의해 반대로 적용되는 경우
    if (StrCmpI(GetLocalFileName(), GetServerFileName()) != 0)
    {
      if ((d & FILE_NEED_UPLOAD) && (mServerState != 0))
      {
        if ((mpLocalRenamedAs == NULL) || (lstrlen(mpLocalRenamedAs) == 0) || !(mLocalState & ITEM_STATE_MOVED))
        {
          mLocalState |= ITEM_STATE_MOVED;
          mServerState &= ~ITEM_STATE_MOVED;

          clearRenameInfo();

          TCHAR pathname[MAX_PATH];
          MakeFullPathName(pathname, MAX_PATH, ((KSyncFolderItem *)mParent)->GetBaseFolder(), GetLocalFileName());
          mpLocalRenamedAs = AllocString(pathname);
        }
      }
      else if ((d & FILE_NEED_DOWNLOAD) && (mLocalState != 0))
      {
        if ((mpServerRenamedAs == NULL) || (lstrlen(mpServerRenamedAs) == 0) || !(mServerState & ITEM_STATE_MOVED))
        {
          mServerState |= ITEM_STATE_MOVED;
          mLocalState &= ~ITEM_STATE_MOVED;

          clearRenameInfo();

          mpServerRenamedAs = allocServerSideRelativePath(((KSyncFolderItem *)mParent)->GetRootFolderItem(),
            GetServerRelativePath(), GetServerFileName());
        }
      }
    }
  }

  if (!(d & FILE_NEED_REFRESH_PATH))
  {
    // if file is editing, skip syncing
    LPTSTR resultFilename = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);

#ifdef USE_BACKUP
    if ((p->GetSyncMethod() & SYNC_METHOD_RESTORE) &&
      IsFileEditing(resultFilename))
    {
      r = FILE_SYNC_EDITING_FILE_SKIP;
      delete[] resultFilename;
      goto end_sync_copy;
    }

    if (phase == PHASE_CONFLICT_FILES)// (p->GetSyncMethod() & SYNC_METHOD_RESOLVE_CONFLICT)
    {
      // check drive not ready
      if (!gpCoreEngine->IsTargetDrive(resultFilename)) // 대상 드라이브가 아닌 드라이브
      {
        r = FILE_SYNC_NOT_EXIST_DRIVE;
        delete[] resultFilename;
        goto end_sync_copy;
      }
    }

#else
    if (IsFileEditing(resultFilename))
    {
      StringCchPrintf(msg, 400, _T("Editing file skipped, %s"), resultFilename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
      delete[] resultFilename;
      return R_SUCCESS;
    }
#endif
    int filename_len = lstrlen(resultFilename);
    delete[] resultFilename;
    if (filename_len >= KMAX_PATH)
    {
      r = FILE_SYNC_OVER_PATH_LENGTH;
      goto end_sync_copy;
    }
  }

  if (d & FILE_NEED_RETIRED)
  {
    d &= ~(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE);

    LPTSTR filepathname = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
    if ((p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD) && IsFileExist(filepathname))
      d |= FILE_NEED_LOCAL_DELETE;
    delete[] filepathname;

    if ((p->GetSyncFlag() & SYNC_ENABLE_UPLOAD) &&
      IsServerFileExist() && (mServerState & ITEM_STATE_NOW_EXIST))
      d |= FILE_NEED_SERVER_DELETE;
  }
  // check rename & up/download
  d &= ~(FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE);

  if (d & FILE_SAME_MOVED)
  {
      StringCchPrintf(msg, 400, _T("path=%s, renamed as same"), GetPathString());
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
  }
  else
  {
    if ((mLocalState & ITEM_STATE_MOVED) && (mServerState & ITEM_STATE_MOVED))
    {
      if (d & FILE_NEED_UPLOAD) // use local name
        d |= FILE_NEED_SERVER_MOVE;
      else if (d & FILE_NEED_DOWNLOAD)
        d |= FILE_NEED_LOCAL_MOVE;

      // d &= ~(FILE_NEED_UPLOAD | FILE_NEED_DOWNLOAD | FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
    }
    else
    {
      if (((d & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD)) == 0) &&
        !(d & FILE_NEED_RETIRED))
      {
        if (mLocalState & ITEM_STATE_MOVED)
        {
          d |= FILE_NEED_SERVER_MOVE;
        }
        else if (mServerState & ITEM_STATE_MOVED)
        {
          d |= FILE_NEED_LOCAL_MOVE;
        }
      }
      else
      {
        if (d & FILE_NEED_UPLOAD)
        {
          if (phase == PHASE_CONFLICT_FILES)
          {
            if ((mLocalState & ITEM_STATE_MOVED) || (mServerState & ITEM_STATE_MOVED))
              d |= FILE_NEED_SERVER_MOVE;
          }
          else
          {
            if (mLocalState & ITEM_STATE_MOVED) //&& (mServerState & ITEM_STATE_NOW_EXIST))
              d |= FILE_NEED_SERVER_MOVE;
            else if (mServerState & ITEM_STATE_MOVED) //&& (mLocalState & ITEM_STATE_NOW_EXIST))
              d |= FILE_NEED_LOCAL_MOVE;
          }
        }
        if (d & FILE_NEED_DOWNLOAD)
        {
          if (phase == PHASE_CONFLICT_FILES)
          {
            // 로컬이 이름변경된 상태에서 충돌 처리에서 다운로드를 하면 로컬측 이름을 원래대로 바꾸도록.
            if ((mLocalState & ITEM_STATE_MOVED) || (mServerState & ITEM_STATE_MOVED))
              d |= FILE_NEED_LOCAL_MOVE;
          }
          else
          {
          if (mServerState & ITEM_STATE_MOVED) //&& (mLocalState & ITEM_STATE_NOW_EXIST))
            d |= FILE_NEED_LOCAL_MOVE;
          else if (mLocalState & ITEM_STATE_MOVED) //&& (mServerState & ITEM_STATE_NOW_EXIST))
            d |= FILE_NEED_SERVER_MOVE;
          }
        }
      }
    }
  }

  // return if no need to do
  if (d == 0)
    return R_SUCCESS;

  if (d & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD))
  {
    // check if valid size for upload/download
    __int64 fileSize = GetSyncFileSize(phase == PHASE_CONFLICT_FILES);

    //LPTSTR resultFilename = AllocSyncResultName(((d & FILE_NEED_UPLOAD) ? AS_LOCAL : 0) | AS_FULLPATH);
    LPTSTR resultFilename = AllocSyncResultName(AS_LOCAL | AS_FULLPATH);
    BOOL excluded = gpCoreEngine->IsSyncExcludeFileCondition(resultFilename, fileSize);
    delete[] resultFilename;

    if (excluded)
    {
      d = FILE_SYNC_OVER_FILE_SIZE_LIMIT;
      r = (d & FILE_SYNC_ERR);
      StringCchPrintf(msg, 400, _T("path=%s"), GetPathString());
      goto end_sync_copy;
    }
  }

  if (d & (FILE_NEED_LOCAL_MOVE | FILE_NEED_DOWNLOAD))
  {
    // check result name
    LPTSTR resultFilename = AllocSyncResultName(AS_LOCAL | AS_FULLPATH);
    int fullPathLength = lstrlen(resultFilename);
    delete[] resultFilename;

    if (fullPathLength > MAX_FILE_PATH_LENGTH)
    {
      StringCchPrintf(msg, KMAX_LONGPATH+100, _T("LongFileName Skip(len=%d):%s"), fullPathLength, mLocal.pFilename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      d = FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH;
      r = FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH;
      goto end_sync_copy;
    }
  }


#ifdef RENAME_ON_SYNC_TIME
  if (lstrlen(mLocalRename) > 0)
  {
    if (renameLocal(baseFolder, mLocalRename))
    {
      d &= ~(FILE_CONFLICT | FILE_SYNC_ERR);
      renamed = 1;
    }
  }
  if ((lstrlen(mServerRename) > 0) && (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD))
  {
    if (renameServer(mServerRename))
    {
      d &= ~(FILE_CONFLICT | FILE_SYNC_ERR);
      renamed = 2;
    }
  }
#endif

  if ((d & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD)) && (mServerState & ITEM_STATE_RE_UPDATED))
  {
    r = FILE_SYNC_SERVER_MODIFIED_AFTER_COMPARE;
    StoreLogHistory(_T(__FUNCTION__), _T("Server file modified after comparison"), SYNC_MSG_LOG|SYNC_MSG_HISTORY);
    goto end_sync_copy;
  }
  // mLocalState |= ITEM_STATE_RE_UPDATED;
  if ((d & (FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD)) && (mLocalState & ITEM_STATE_RE_UPDATED))
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Local file modified after comparison"), SYNC_MSG_LOG|SYNC_MSG_HISTORY);
    r = FILE_SYNC_LOCAL_MODIFIED_AFTER_COMPARE;
    goto end_sync_copy;
  }

  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

  // do renaming
  if (d & FILE_SAME_MOVED)
  {
    // if (StrCmpI(mLocalRenamedAs, mServerRenamedAs) == 0)
    updateMetaAsRenamed();
    r &= ~(FILE_NEED_BOTH_MOVE|FILE_SAME_MOVED|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
    d &= ~(FILE_NEED_BOTH_MOVE|FILE_SAME_MOVED|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
    meta_mask = META_MASK_FILENAME;
  }
  else if (d & FILE_NEED_BOTH_MOVE)
  {
    if (/*isLocalRenamedAs()*/ (d & FILE_NEED_SERVER_MOVE) )
    {
      BOOL rename_need = TRUE;
      BOOL able = TRUE;
      if (d & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)
      {
        rename_need = FALSE;
        able = TRUE;
      }
      else
      {
        able = (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD);
        int server_file_perm = 0xFFFF;
        if (mServerState & ITEM_STATE_NOW_EXIST) // only file exist
          server_file_perm = GetServerPermission();

        if (able)
          able = CheckPermissionFileRename(mParent->GetServerPermission(), server_file_perm);
      }
      if (able)
      {
        // 업로드시 새 파일명이 전달되니 여기서 실제 이름 변경이 필요없음.
        // if it will be uploaded, no need rename hear
        if ((d & FILE_NEED_UPLOAD) && 
          ((mLocalState & (ITEM_STATE_UPDATED|ITEM_STATE_NEWLY_ADDED)) || 
            (mServerState & ITEM_STATE_DELETED) ||
            ((mServerState & ITEM_STATE_EXISTING) == 0)))
        {
          rename_need = FALSE;
          StoreLogHistory(_T(__FUNCTION__), _T("renameServerObject skip because it will be uploaded"), SYNC_MSG_LOG|SYNC_MSG_HISTORY);
        }

        r = renameServerObjectAs(rename_need); // FILE_SYNC_LOCAL_RENAME_FAIL;
        if (r == R_SUCCESS)
        {
          r &= ~(FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
          d &= ~(FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);

          // if file does not changed on each side, no-download
          if (rename_need && // 이름변경 되었으며, 파일이 바뀌지 않았으면
            !(mServerState & ITEM_STATE_UPDATED) && !(mLocalState & ITEM_STATE_UPDATED))
            d &= ~FILE_NEED_UPLOAD;
          else
            mLocalState |= ITEM_STATE_UPDATED;
        }
        else
        {
          d = r;
        }
      }
      else
      {
        r = FILE_SYNC_NO_RENAME_PERMISSION;
      }
    }
    else if (/*isServerRenamedAs()*/ (d & FILE_NEED_LOCAL_MOVE) )
    {
      if (d & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)
      {
        // not need local renme, but need update local ifo
        renameLocalObjectAs(FALSE); // FILE_SYNC_LOCAL_RENAME_FAIL;
        r &= ~(FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED);
        d &= ~(FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED);
      }
      else
      {
        BOOL able = (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD);
        if (able)
        {
          if (IsLocalFileExist())
          {
            r = renameLocalObjectAs(TRUE); // FILE_SYNC_LOCAL_RENAME_FAIL;
            if (r == R_SUCCESS)
            {
              r &= ~(FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
              d &= ~(FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);

              // if file does not changed on each side, no-download
              if (!(mServerState & ITEM_STATE_UPDATED) && !(mLocalState & ITEM_STATE_UPDATED))
                d &= ~FILE_NEED_DOWNLOAD;
              else
                mServerState |= ITEM_STATE_UPDATED; // to force download

              meta_mask = META_MASK_FILENAME;
            }
          }
          else 
          {
            r &= ~(FILE_NEED_LOCAL_MOVE);
            d &= ~(FILE_NEED_LOCAL_MOVE);
          }
        }
        else
        {
          r = FILE_SYNC_NO_LOCAL_PERMISSION; // FILE_SYNC_NO_RENAME_PERMISSION;
        }
      }
    }

    if (r & FILE_SYNC_ERR)
      goto end_sync_copy;
  }

  // do upload-download
  if ((d & FILE_NEED_UPLOAD) && ((mLocalState & (ITEM_STATE_UPDATED|ITEM_STATE_NEWLY_ADDED)) || (mServerState & ITEM_STATE_DELETED)))
  {
    StringCchPrintf(msg, 400, _T("path=%s,user=%s,start_upload"), GetPathString(), mLocal.UserOID);
    // check permission
    BOOL able = FALSE;

    BOOL server_exist = IsServerFileExist() && (mServerState & ITEM_STATE_NOW_EXIST);

    /*
    if ((mConflictResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED) ||
      (mResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED))
      server_exist = FALSE;
    */

    if(mServerState & ITEM_STATE_DELETED)
      server_exist = FALSE;

    if (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD)
    {
      // if parent-on-server is delete, create it first
      ((KSyncFolderItem *)mParent)->CreateServerFolderIfDeleted(baseFolder);

      if(server_exist)
        able = CheckPermissionFileUpload(mParent->GetServerPermission(), GetServerPermission());
        // able = ((mParent->GetServerPermission() & SFP_WRITE_CHILD) && (GetServerPermission() & SFP_WRITE));
      else
        able = CheckPermissionFileInsert(mParent->GetServerPermission(), GetServerPermission());
        // able = (mParent->GetServerPermission() & SFP_INSERT_FILE);

      TCHAR str[64];
      StringCchPrintf(str, 64, _T("(folder:0x%x, file:0x%x)"), mParent->GetServerPermission(), GetServerPermission());
      StringCchCat(msg, 400, str);
    }

    if (able)
    {
      if (!server_exist) // upload as new file
        clearServerSideInfo();

      r = uploadFileItem();
      if (r != R_SUCCESS)
        StringCchCat(msg, 400, _T(":fail"));
      else
        StringCchCat(msg, 400, _T(":ok"));
    }
    else
    {
      r = FILE_SYNC_NO_UPLOAD_PERMISSION;
      StringCchCat(msg, 400, _T(":fail, no_permission"));
    }

    // OutputDebugString(msg);
    if ((mpServerConflictMessage != NULL) && (lstrlen(mpServerConflictMessage) > 0))
    {
      StringCchCat(msg, 400, _T("-"));
      StringCchCat(msg, 400, mpServerConflictMessage);
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

    d &= ~FILE_NEED_UPLOAD;

#ifdef THREADED_LOADING
    if (r == R_SUCCESS)
    {
      SetItemSynchronizing(baseFolder, 1, WITH_STORE_METAFILE);
      return FILE_SYNC_THREAD_WAIT;
    }
#endif
  }
  else if ((d & FILE_NEED_DOWNLOAD) && ((mServerState & (ITEM_STATE_UPDATED|ITEM_STATE_NEWLY_ADDED)) || (mLocalState & ITEM_STATE_DELETED)) )
  {
    StringCchPrintf(msg, 400, _T("path=%s,user=%s,start_download"), GetPathString(), mServerNew.UserOID);

    if (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD)
    {
      BOOL able = CheckPermissionFileDownload(mParent->GetServerPermission(), GetServerPermission());
      // (mParent->GetServerPermission() & SFP_READ_CHILD) && (GetServerPermission() & SFP_READ)

      TCHAR str[64];
      StringCchPrintf(str, 64, _T("(folder:0x%x, file:0x%x)"), mParent->GetServerPermission(), GetServerPermission());
      StringCchCat(msg, 400, str);

      if(able)
      {
        r = downloadFileItem();
        if (r != R_SUCCESS)
          StringCchCat(msg, 400, _T(":fail"));
        else
          StringCchCat(msg, 400, _T(":ok"));
      }
      else
      {
        r = FILE_SYNC_NO_DOWNLOAD_PERMISSION;
        StringCchCat(msg, 400, _T(":fail, no_permission"));
      }
    }
    else
    {
      r = FILE_SYNC_NO_LOCAL_WRITE_PERMISSION;
      StringCchCat(msg, 400, _T(":fail, no_write_local_permission"));
    }

    // OutputDebugString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

    d &= ~FILE_NEED_DOWNLOAD;
#ifdef THREADED_LOADING
    if (r == R_SUCCESS)
    {
      SetItemSynchronizing(baseFolder, 1, WITH_STORE_METAFILE);
      return FILE_SYNC_THREAD_WAIT;
    }
#endif
  }
  if ((d & FILE_NEED_DOWNLOAD) && (mServerState == 0) && (phase == PHASE_CONFLICT_FILES)) // server file not exist, but user select download
  {
    r = FILE_SYNC_DOWNLOAD_FAIL;
  }

  if (d & FILE_NEED_LOCAL_DELETE)
  {
    LPTSTR name = AllocSyncResultName(AS_LOCAL);
    StringCchPrintf(msg, 400, _T("pathname=%s, delete_on_local"), name);
    BOOL able = (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD);
    delete[] name;

    if (able)
    {
      LPTSTR filename = AllocSyncResultName(AS_FULLPATH|AS_LOCAL);
      r = gpCoreEngine->SyncLocalDeleteFile(filename);
      delete[] filename;

      if (r != R_SUCCESS)
      {
        StringCchCat(msg, 400, _T(":fail"));
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

        GetLastErrorMessageString(GetLastError(), msg, 400);
      }
      else
      {
        if (mpLocalRenamedAs != NULL)
        {
          delete mpLocalRenamedAs;
          mpLocalRenamedAs = NULL;
        }
        FreeLocalInfo(mLocalNew);
        StoreHistory(SYNC_H_DELETE_LOCAL_FILE, filename, 0, NULL, NULL);
      }
    }
    else
    {
      r = FILE_SYNC_NO_DELETE_LOCAL_FILE_PERMISSION;
      StringCchCat(msg, 400, _T(":fail, no_permission"));
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
  }
  if (d & FILE_NEED_SERVER_DELETE)
  {
    LPTSTR name = AllocSyncResultName(0);
    StringCchPrintf(msg, 400, _T("pathname=%s, remove_on_server"), name);
    delete[] name;

    BOOL able = FALSE;

    if (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD)
      able = CheckPermissionFileDelete(mParent->GetServerPermission(), GetServerPermission());
      //able = (mParent->GetServerPermission() & SFP_READ_CHILD) && (GetServerPermission() & SFP_DELETE);

    // check permission : 폴더의 READ 권한 & 파일의 DELETE 권한
    if (able)
    {
      if (mpServerConflictMessage != NULL)
      {
        delete[] mpServerConflictMessage;
        mpServerConflictMessage = NULL;
      }

      KSyncFolderItem* parent = (KSyncFolderItem*)((mNewParent != NULL) ? mNewParent : mParent);
      if (lstrlen(mServerOID) == 0)
      {
        r = R_SUCCESS;
      }
#ifdef USE_BACKUP
      else if(rootFolder->RegistDeleteOnServer(this))
      {
        // 삭제 요청 결과를 받은 후, 실제 Meta data 삭제하기 위해 제거는 하지 않는다.
        d &= ~FILE_NEED_RETIRED;

        // KSyncRootFolderItem::OnSyncFileItemDone 에서 제거되지 않도록.
        AddSyncFlag(SYNC_ITEM_WAIT_CLEAR);

        // 삭제될 것이니 마무리 처리 필요 없음.
        return R_SUCCESS;
      }
#else
      else if(gpCoreEngine->SyncServerDeleteFile(mServerOID, mServer.FileOID, parent->mServerOID, 
        parent->GetServerFullPathIndex(), &mpServerConflictMessage))
      {
        r = R_SUCCESS;
        if (mpServerRenamedAs != NULL)
        {
          delete[] mpServerRenamedAs;
          mpServerRenamedAs = NULL;
        }
        FreeServerInfo(mServerNew);
      }
#endif
      else
      {
        StringCchCat(msg, 400, _T(":fail_server"));
        r = FILE_SYNC_SERVER_REMOVE_FAIL;
      }
    }
    else
    {
      r = FILE_SYNC_NO_DELETE_SERVER_FILE_PERMISSION;
      StringCchCat(msg, 400, _T(":fail, no_permission"));
    }

    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
    if (r == R_SUCCESS)
    {
      StoreHistory(SYNC_H_DELETE_SERVER_FILE, rootFolder->GetRootPath(), name, 0, NULL, NULL);
    }
  }

  // if delete failure, no retire
  if ((d & (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE)) && (r != R_SUCCESS))
    d &= ~FILE_NEED_RETIRED;

  if (d & FILE_NEED_REFRESH_PATH)
  {
    RefreshPathChanged(rootFolder->GetRootPath());
    d &= ~FILE_NEED_REFRESH_PATH;
    r = R_SUCCESS;
    // UpdatePathChangeOnMetafile(LPCTSTR baseFolder, int update_mask, int flag);
    mLocalState &= ~ITEM_STATE_REFRESH_PATH;
  }

  // do retire
  if (d & FILE_NEED_RETIRED)
  {
    d &= ~FILE_NEED_RETIRED;
    deleteMeta = TRUE;
  }

end_sync_copy:
  r &= ~FILE_ACTION_MASK;
  if (r == R_SUCCESS)
  {
    // clear status
    if (phase >= PHASE_COPY_FILES)
    {
      // no need to clear state 161109
      // mLocalState = mServerState = ITEM_STATE_NONE;
      //mResult = mConflictResult = 0;
    }
    if (phase == PHASE_LOAD_FILES)
    {
      d = GetResult();
      r = d & ~(FILE_SAME_MOVED| FILE_NEED_LOCAL_MOVE| FILE_NEED_SERVER_MOVE);
    }

    if (mpServerConflictMessage != NULL)
    {
      delete[] mpServerConflictMessage;
      mpServerConflictMessage = NULL;
    }
  }
  else
  {
    if ((r & (FILE_CONFLICT | FILE_SYNC_ERR)) == 0)
      r |= (d & ~FILE_ACTION_MASK);

    if (d & (FILE_CONFLICT | FILE_SYNC_ERR))
    {
      // update server info
      RefreshServerNewState();

      meta_mask |= META_MASK_LOCAL | META_MASK_SERVER | META_MASK_FILEOID | 
        META_MASK_LASTUSER | META_MASK_SERVERUSER | META_MASK_LOCALSTATE | META_MASK_SERVERSTATE | 
        META_MASK_RENAMED;

      // save with new server info
      meta_mask |= META_MASK_NEW_LOCAL | META_MASK_NEW_SERVER | 
        META_MASK_NEW_SERVERUSER | META_MASK_NEW_SERVERPATH | 
        META_MASK_NEW_SERVERPATHINDEX | META_MASK_NEW_FILEOID;
    }
  }
  SetResult(r);
  SetConflictResult(r);

  if (r != R_SUCCESS)
  {
    LPTSTR name = AllocSyncResultName(AS_LOCAL);
    int state = GetHistoryStateFromSyncResult(r, 0);
    KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    StoreHistory(state, rootFolder->GetRootPath(), name, 0, NULL, mpServerConflictMessage);
    delete[] name;
  }
  else
  {
    // clear from ModifyTable 
    // storage->ClearModified(LPCTSTR pathName)
  }

  if ((r == R_SUCCESS) && (d & (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE))
    && (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD))
    deleteMeta = TRUE;

  if (deleteMeta)
  {
    StringCchPrintf(msg, 600, _T("path=%s,user=%s,retired"), GetPathString(), mLocal.UserOID);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
  }

  meta_mask |= META_MASK_CONFLICT | META_MASK_SYNCRESULT | META_MASK_FILENAME | META_MASK_SERVEROID | META_MASK_RENAMED | META_MASK_LOCALSTATE | META_MASK_SERVERSTATE;
  int meta_flag = (deleteMeta ? METAFILE_DELETE : 0);
  if (r == R_SUCCESS)
  {
    meta_mask = META_MASK_ALL;
    meta_flag |= METAFILE_CLEAR_MODIFIED;
  }
  StoreMetafile(NULL, (r == R_SUCCESS) ? META_MASK_ALL : meta_mask, meta_flag);

  if (!deleteMeta)
  {
    // update overlay icon
    LPTSTR filepathname = AllocFullPathName();
    gpCoreEngine->ShellNotifyIconChangeOverIcon(filepathname, FALSE);
    delete[] filepathname;
  }
  return r;
}

BOOL KSyncItem::OnDeleteServerItem(BOOL r, LPCTSTR pError)
{
  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  LPTSTR relativePath = AllocRelativePathName();

  if (r)
  {
    StoreHistory(SYNC_H_DELETE_SERVER_FOLDER, rootFolder->GetRootPath(), relativePath, 1, NULL, NULL);
    StoreMetafile(NULL, META_MASK_ALL, METAFILE_DELETE);
  }
  else
  {
    StoreHistory(SYNC_H_DELETE_SERVER_FOLDER, rootFolder->GetRootPath(), relativePath, 1, NULL, pError);
    SetResult(FILE_SYNC_SERVER_REMOVE_FAIL);
    SetConflictResult(FILE_SYNC_SERVER_REMOVE_FAIL);
    SetServerConflictMessage(pError);

    StoreMetafile(NULL, META_MASK_CONFLICT | META_MASK_SYNCRESULT, 0);
  }

  if (relativePath != NULL)
    delete[] relativePath;
  return TRUE;
}

BOOL KSyncItem::RefreshServerState()
{
  if (lstrlen(mServerOID) == 0)
    return FALSE;
  return (gpCoreEngine->GetServerDocumentSyncInfo(mServerOID, mServer) == 0);
}

int KSyncItem::CheckServerModifiedAgain(SERVER_INFO& serverInfo)
{
  // check if parent path changed
  if ((mParent != NULL) &&
    mParent->IsFolderFullPathChanged())
  {
    TCHAR msg[400];
    StringCchPrintf(msg, 400, _T("Parent FullPathIndex changed [%s] to [%s]"), mParent->mServer.pFullPathIndex, mParent->mServerNew.pFullPathIndex);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return ITEM_STATE_LOCATION_CHANGED;
  }
  if (lstrlen(mServerOID) == 0)
    return 0;

  int r;
  BOOL deleted = FALSE;

  if (IsFolder())
  {
    KSyncRootFolderItem *root =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    r = gpCoreEngine->GetServerFolderSyncInfo(root, mServerOID, serverInfo);
    if (r == 0)
    {
      LPCTSTR filename = GetServerFileName();
      if (StrCmp(filename, serverInfo.pFilename) != 0)
      {
        r = ITEM_STATE_UPDATED;
      }
      else
      {
        LPCTSTR fullpath_index = GetServerFullPathIndex();
        if ((fullpath_index != NULL) && StrCmp(fullpath_index, serverInfo.pFullPathIndex) != 0)
        {
          r = ITEM_STATE_LOCATION_CHANGED;
        }
        else
        {
          // 삭제되었으나 다시 복구됨.
          if (mServerState & ITEM_STATE_DELETED)
          {
            mServerState &= ~ITEM_STATE_DELETED;
            mServerState |= ITEM_STATE_NOW_EXIST | ITEM_STATE_RESTORED;
          }
        }
      }
    }
    else
    {
      deleted = !!(r & ITEM_STATE_DELETED);
    }
  }
  else
  {
    r = gpCoreEngine->GetServerDocumentSyncInfo(mServerOID, serverInfo);
    if (r == 0)
    {
      TCHAR filename[KMAX_PATH];
      StringCchCopy(filename, KMAX_PATH, GetServerFileName());

      if (lstrlen(mServerNew.FileOID) == 0)
      {
        if ((StrCmp(mServer.FileOID, serverInfo.FileOID) != 0) ||
          (StrCmp(mServer.StorageOID, serverInfo.StorageOID) != 0) ||
          (StrCmp(filename, serverInfo.pFilename) != 0))
        {
          r = ITEM_STATE_UPDATED;
        }
        else if ((StrCmp(mServer.FileOID, serverInfo.FileOID) == 0) ||
          (StrCmp(mServer.StorageOID, serverInfo.StorageOID) == 0))
        {
          // 삭제되었으나 다시 복구됨.
          if (mServerState & ITEM_STATE_DELETED)
          {
            mServerState &= ~ITEM_STATE_DELETED;
            mServerState |= ITEM_STATE_NOW_EXIST | ITEM_STATE_RESTORED;
          }
        }
      }
      else
      {
        if ((StrCmp(mServerNew.FileOID, serverInfo.FileOID) != 0) ||
          (StrCmp(mServerNew.StorageOID, serverInfo.StorageOID) != 0) ||
          (StrCmp(filename, serverInfo.pFilename) != 0))
        {
          r = ITEM_STATE_UPDATED;
        }
        else if ((StrCmp(mServer.FileOID, serverInfo.FileOID) == 0) ||
          (StrCmp(mServer.StorageOID, serverInfo.StorageOID) == 0))
        {
          // 삭제되었으나 다시 복구됨.
          if (mServerState & ITEM_STATE_DELETED)
          {
            mServerState &= ~ITEM_STATE_DELETED;
            mServerState |= ITEM_STATE_NOW_EXIST | ITEM_STATE_RESTORED;
          }
        }
      }
    }
    else
    {
      deleted = !!(r & ITEM_STATE_DELETED);
      if ((r & ITEM_STATE_DELETED) != (mServerState & ITEM_STATE_DELETED))
        r = ITEM_STATE_UPDATED;
      if (deleted)
        r = ITEM_STATE_DELETED;
    }
  }

  if (r == ITEM_STATE_UPDATED)
  {
    TCHAR msg[400];
    if (deleted)
      StringCchPrintf(msg, 400, _T("Deleted now=%s"), mLocal.pFilename);
    else
      StringCchPrintf(msg, 400, _T("Changed as fileOID=%s, storageOID=%s, name=%s"),
        serverInfo.FileOID, serverInfo.StorageOID, serverInfo.pFilename);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
  return r;
}

int KSyncItem::RefreshServerNewState()
{
  // check if parent path changed
  if ((mParent != NULL) &&
    mParent->IsFolderFullPathChanged())
  {
    TCHAR msg[400];
    StringCchPrintf(msg, 400, _T("Parent FullPathIndex changed [%s] to [%s]"), mParent->mServer.pFullPathIndex, mParent->mServerNew.pFullPathIndex);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return ITEM_STATE_LOCATION_CHANGED;
  }

  if (lstrlen(mServerOID) == 0)
    return 0;

  SERVER_INFO serverInfo;
  memset(&serverInfo, 0, sizeof(SERVER_INFO));
  int r = gpCoreEngine->GetServerDocumentSyncInfo(mServerOID, serverInfo);
  if (r == 0)
  {
    if (lstrlen(mServerNew.FileOID) == 0)
    {
      if ((StrCmp(mServer.FileOID, serverInfo.FileOID) != 0) ||
        (StrCmp(mServer.StorageOID, serverInfo.StorageOID) != 0))
      {
        CopyServerInfo(mServerNew, serverInfo);
        r = ITEM_STATE_UPDATED;
      }
    }
    else
    {
      if ((StrCmp(mServerNew.FileOID, serverInfo.FileOID) != 0) ||
        (StrCmp(mServerNew.StorageOID, serverInfo.StorageOID) != 0))
      {
        CopyServerInfo(mServerNew, serverInfo);
        r = ITEM_STATE_UPDATED;
      }
    }
  }
  FreeServerInfo(serverInfo);
  return r;
}

BOOL KSyncItem::IsMatchType(int type)
{
  if (type == 2)
    return TRUE;
  return (!!IsFolder() == type);
}

void KSyncItem::MergeItem(KSyncItem *item)
{
}


BOOL KSyncItem::willBeDeleted(int phase)
{
  int d = 0;
  if (phase == PHASE_COPY_FILES)
    d = GetResult();
  else if (phase == PHASE_CONFLICT_FILES)
    d = GetConflictResult();

  return ((d & FILE_NEED_LOCAL_DELETE) || (d & FILE_NEED_SERVER_DELETE));
}

BOOL KSyncItem::ToggleConflictResult()
{
  // 이름변경 또는 분리로 생성된 것, 상위 폴더 오류로 동기화 멈춘것은 토글하지 않는다
  // 상위 폴더 오류라해도 방향 바꿀 수 있음. 20160607
  if (mConflictResult & (FILE_SYNC_RENAMED)) // FILE_SYNC_BELOW_CONFLICT
    return FALSE;

  int r = (mConflictResult & FILE_ACTION_MASK);

#ifdef CASE_BY_CONFLICT
  if (mResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)
  {
#ifdef RENAME_ON_SYNC_TIME
    if (IsRenamed())
    {
      r = FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD;
    }
    else
#endif
    {
      // FILE_NEED_DOWNLOAD & FILE_NEED_UPLOAD
      r++;
      if(r > FILE_NEED_UPLOAD)
        r = FILE_NEED_DOWNLOAD;
    }
  }
  else if (mResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED) // local modified, server deleted
  {
#ifdef RENAME_ON_SYNC_TIME
    if (IsRenamed())
    {
      r = FILE_NEED_UPLOAD;
    }
    else
#endif
    {
      // FILE_NEED_UPLOAD or FILE_NEED_LOCAL_DELETE
      r = (r == FILE_NEED_UPLOAD) ? FILE_NEED_LOCAL_DELETE : FILE_NEED_UPLOAD;
    }
  }
  else if (mResult & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED) // local deleted, server modified
  {
#ifdef RENAME_ON_SYNC_TIME
    if (IsRenamed())
    {
      r = FILE_NEED_DOWNLOAD;
    }
    else
#endif
    {
      // FILE_NEED_DOWNLOAD or FILE_NEED_SERVER_DELETE
      r = (r == FILE_NEED_DOWNLOAD) ? FILE_NEED_SERVER_DELETE : FILE_NEED_DOWNLOAD;
    }
  }
  else if (mResult & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED) // local added, server added
  {
#ifdef RENAME_ON_SYNC_TIME
    if (IsRenamed())
    {
      r = FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD;
    }
    else
#endif
    {
      // FILE_NEED_DOWNLOAD & FILE_NEED_UPLOAD
      r++;
      if(r > FILE_NEED_UPLOAD)
        r = FILE_NEED_DOWNLOAD;
    }
  }
  else if(mResult & FILE_SYNC_ERR) // for sync-error, download/upload/delete/none
  {
    if (r == 0)
      r = FILE_NEED_DOWNLOAD;
    else if (r == FILE_NEED_DOWNLOAD)
      r = FILE_NEED_UPLOAD;
    else if (r == FILE_NEED_UPLOAD)
      r = FILE_NEED_SERVER_DELETE|FILE_NEED_LOCAL_DELETE;
    else
      r = 0;
  }
#else
  if (r == 0)
    r = FILE_NEED_DOWNLOAD;
  else if (r == FILE_NEED_DOWNLOAD)
    r = FILE_NEED_UPLOAD;
  else if (r == FILE_NEED_UPLOAD)
    r = FILE_NEED_SERVER_DELETE|FILE_NEED_LOCAL_DELETE;
  else
    r = 0;
#endif

  if (r != 0)
    r |= FILE_ACTION_USER_SELECT;
  mConflictResult = (mConflictResult & ~FILE_ACTION_MASK) | r;
  return TRUE;
}

int KSyncItem::GetItemStateMovedRenamed(BOOL onLocal)
{
  LPTSTR newpathname = NULL;
  if (onLocal && (mLocalState & ITEM_STATE_MOVED))
  {
    newpathname = AllocSyncResultName(AS_LOCAL);
  }
  else if (!onLocal && (mServerState & ITEM_STATE_MOVED))
  {
    newpathname = AllocSyncResultName(0);
  }

  int state = 0;
  if (newpathname != NULL)
  {
   if (isPathChanged(mpRelativePath, newpathname))
   {
     TCHAR newPath[KMAX_PATH];
     SplitPathName(newpathname, newPath, NULL);

    // 상위 폴더가 바뀐 것이고 자신은 변함이 없는 경우 체크
     BOOL is_changed = TRUE;
    if ((lstrlen(newPath) > 0) && (mParent != NULL))
    {
      LPTSTR parent_path = mParent->AllocActualPathName(FALSE);
      if (StrCmpI(parent_path, newPath) == 0)
        is_changed = FALSE;
      delete[] parent_path;
    }

    if (is_changed)
      state |= ITEM_STATE_MOVED;
   }

   if (isNameChanged(mpRelativePath, newpathname))
     state |= ITEM_STATE_RENAMED;
  }
  delete[] newpathname;
  return state;
}

void KSyncItem::SetConflictResultAction(int r, int flag)
{
  mConflictResult = (mConflictResult & ~FILE_USER_ACTION_MASK) | r;
}

int KSyncItem::GetConflictResultMethod()
{
#if 0 // previous two-column selector mode
  if (mConflictResult & FILE_SYNC_ERR)
  {
    int r = (mConflictResult & FILE_USER_ACTION_MASK);
    if (r == FILE_NEED_UPLOAD)
      return 0;
    else if (r == FILE_NEED_DOWNLOAD)
      return 1;
    else if (r == (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE))
      return 2;
    else
      return 3;
  }
  else if (mConflictResult & FILE_CONFLICT)
  {
    int r = (mConflictResult & FILE_USER_ACTION_MASK);
    if (r == FILE_NEED_UPLOAD)
      return 0;
    else if (r == FILE_NEED_DOWNLOAD)
      return 1;
    else if (r & (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE))
      return 2;
    else
    {
#if 0
      // show each conflict status
      if (mConflictResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)
        return 5;
      else if (mConflictResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)
        return 6;
      else if (mConflictResult & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)
        return 7;
      else // if (mConflictResult & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED)
        return 4;
#else
      // show all conflict as one icon
      return 5;
#endif
    }
  }
  else
  {
    int r = (mConflictResult & FILE_USER_ACTION_MASK);
    if (r == FILE_NEED_UPLOAD)
      return 0;
    else if (r == FILE_NEED_DOWNLOAD)
      return 1;
    else if (r == (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE))
      return 2;
  }
#else
  if (mConflictResult & FILE_SYNC_ERR)
    return 1;
  else if (mConflictResult & FILE_CONFLICT)
    return 2;
  else
    return 0;
#endif
  return -1;
}

void KSyncItem::SetServerConflictMessage(LPCTSTR msg)
{
  if((msg == NULL) || (lstrlen(msg) == 0))
  {
    if (mpServerConflictMessage != NULL)
    {
      delete[] mpServerConflictMessage;
      mpServerConflictMessage = NULL;
    }
  }
  else
  {
    AllocCopyString(&mpServerConflictMessage, msg);
    ReplaceControlChars(mpServerConflictMessage);
  }
}

void KSyncItem::clearServerSideInfo()
{
  FreeServerInfo(mServer);
  FreeServerInfo(mServerNew);
  /*
  mServer.FileSize = 0;
  mServerNew.FileSize = -1;
  memset(&mServer.FileTime, 0, sizeof(mServer.FileTime));

  mServer.Filename[0] = '\0';
  mServer.UserOID[0] = '\0';
  mServerNew.UserOID[0] = '\0';
  mServer.Permissions = 0;
  mServerOID[0] = mServer.FileOID[0] = mServer.StorageOID[0] = '\0';
  mServer.FullPath[0] = '\0';
  */
  mServerState = 0;
  mServerOID[0] = '\0';

  if (mpServerRenamedAs != NULL)
  {
    delete[] mpServerRenamedAs;
    mpServerRenamedAs = NULL;
  }
}

void KSyncItem::clearLocalSideInfo()
{
  LPTSTR pFilename = mLocal.pFilename;
  mLocal.pFilename = NULL;
  FreeLocalInfo(mLocal);
  FreeLocalInfo(mLocalNew);
  mLocal.pFilename = pFilename;

  /*
  mLocal.UserOID[0] = '\0';
  mLocal.FileSize = 0;
  mLocalNew.FileSize = -1;
  memset(&mLocal.FileTime, 0, sizeof(mLocal.FileTime));
  memset(&mLocalNew.FileTime, 0, sizeof(mLocalNew.FileTime));
  memset(&mLocalNew.AccessTime, 0, sizeof(mLocalNew.FileTime));
  mMetafilename[0] = '\0';
  */
  mLocalState = 0;
  if (mpLocalRenamedAs != NULL)
  {
    delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }
}

void KSyncItem::clearRenameInfo()
{
  if (mpLocalRenamedAs != NULL)
  {
    delete[] mpLocalRenamedAs;
    mpLocalRenamedAs = NULL;
  }
  if (mpServerRenamedAs != NULL)
  {
    delete[] mpServerRenamedAs;
    mpServerRenamedAs = NULL;
  }
}

void KSyncItem::CloneLocalRename(KSyncRootFolderItem *root, KSyncItem *pair)
{
  if ((mLocalState & ITEM_STATE_MOVED) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
    ChangeRelativePathName(root, mpLocalRenamedAs, TRUE);
  else
    UpdateRelativePath();

  clearServerSideInfo();
  clearRenameInfo();
  setLocalState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED);
  setServerState(0);
  mConflictResult = FILE_NEED_UPLOAD|FILE_SYNC_RENAMED;
  mResult = FILE_NEED_UPLOAD;
  SetServerConflictMessage(NULL);
  SetEnable(TRUE);

  StoreMetafile(NULL, META_MASK_ALL, 0);

  if (pair != NULL)
  {
    /*
    TCHAR lastPath[KMAX_PATH] = { 0 };
    LPCTSTR pair_path = pair->GetPath();

    if (pair_path != NULL)
      StringCchCopy(lastPath, KMAX_PATH, pair_path);
    */
    if ((pair->mpServerRenamedAs != NULL) && (lstrlen(pair->mpServerRenamedAs) > 0))
      pair->ChangeRelativePathName(root, pair->mpServerRenamedAs, TRUE);
    else
      pair->UpdateRelativePath();

    CopyServerInfo(pair->mServer, pair->mServerNew);
    pair->setLocalState(0);
    pair->setServerState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED);
    pair->mConflictResult = FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED;
    pair->mResult = FILE_NEED_DOWNLOAD;
    pair->SetServerConflictMessage(NULL);
    pair->clearLocalSideInfo();
    pair->clearRenameInfo();
    pair->SetEnable(TRUE);

    if (pair->mpMetafilename != NULL)
    {
      delete[] pair->mpMetafilename;
      pair->mpMetafilename = NULL;
    }
    pair->StoreMetafile(NULL, META_MASK_ALL, 0);
  }
}

void KSyncItem::CloneServerRename(KSyncRootFolderItem *root, KSyncItem *pair)
{
  if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
    ChangeRelativePathName(root, mpServerRenamedAs, TRUE);
  else
    UpdateRelativePath();

  clearLocalSideInfo();
  clearRenameInfo();

  setServerState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED); // ITEM_STATE_WAS_EXIST
  setLocalState(0);
  mConflictResult = FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED;
  mResult = FILE_NEED_DOWNLOAD;
  SetServerConflictMessage(NULL);
  SetEnable(TRUE);

  StoreMetafile(NULL, META_MASK_ALL, 0);

  if (pair != NULL)
  {
    if (pair->mLocalState & ITEM_STATE_MOVED)
      pair->ChangeRelativePathName(root, pair->mpLocalRenamedAs, TRUE);
    else
      pair->UpdateRelativePath();

    if (pair->mLocalState & ITEM_STATE_DELETED)
    {
      // update mBaseFolder
      pair->setLocalState(ITEM_STATE_DELETED);
      pair->setServerState(0);
      pair->mConflictResult = FILE_NEED_RETIRED|FILE_SYNC_RENAMED;
      pair->mResult = FILE_NEED_RETIRED;
    }
    else
    {
      pair->setLocalState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_WAS_EXIST|ITEM_STATE_CONFLICT_RENAMED);
      pair->setServerState(0);
      pair->mConflictResult = FILE_NEED_UPLOAD|FILE_SYNC_RENAMED;
      pair->mResult = FILE_NEED_UPLOAD;
    }

    pair->clearServerSideInfo();
    pair->clearRenameInfo();
    pair->SetServerConflictMessage(NULL);
    pair->SetEnable(TRUE);

    if (pair->mpMetafilename != NULL)
    {
      delete[] pair->mpMetafilename;
      pair->mpMetafilename = NULL;
    }
    pair->StoreMetafile(NULL, META_MASK_ALL, 0);
  }
}

void KSyncItem::CloneSeparate(KSyncRootFolderItem *root, KSyncItem *pair)
{
  //remove last key
  StoreMetafile(root->GetStorage(), 0, METAFILE_DELETE);

  if (mLocalState & ITEM_STATE_MOVED)
    ChangeRelativePathName(root, mpLocalRenamedAs, TRUE);
  else
    UpdateRelativePath();

  clearServerSideInfo();

  setLocalState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED);
  setServerState(0);
  mConflictResult = FILE_NEED_UPLOAD|FILE_SYNC_RENAMED;
  mResult = FILE_NEED_UPLOAD;
  SetServerConflictMessage(NULL);
  clearRenameInfo();
  SetEnable(TRUE);

  StoreMetafile(NULL, META_MASK_ALL, 0);

  if (pair != NULL)
  {
    /*
    TCHAR lastPath[KMAX_PATH] = { 0 };
    LPCTSTR pair_path = pair->GetPath();

    if (pair_path != NULL)
      StringCchCopy(lastPath, KMAX_PATH, pair_path);
    */
    if ((pair->mpServerRenamedAs != NULL) && (lstrlen(pair->mpServerRenamedAs) > 0))
    {
      CopyServerInfo(pair->mServer, pair->mServerNew);
      pair->ChangeRelativePathName(root, pair->mpServerRenamedAs, TRUE);
    }
    else
      pair->UpdateRelativePath();

    pair->setLocalState(0);
    pair->setServerState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED);
    pair->mConflictResult = FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED;
    pair->mResult = FILE_NEED_DOWNLOAD;
    pair->SetServerConflictMessage(NULL);
    pair->clearLocalSideInfo();
    pair->clearRenameInfo();
    pair->SetEnable(TRUE);

    pair->StoreMetafile(NULL, META_MASK_ALL, 0);
  }
}


BOOL KSyncItem::IsAlreadySynced()
{
  if (mFlag & SYNC_ITEM_EXCLUDED)
    return FALSE;

  if (lstrlen(mServerOID) == 0)
    return FALSE;

  return TRUE;
}

LPTSTR KSyncItem::AllocServerSidePathName()
{
  LPCTSTR path = GetServerRelativePath();
  return AllocString(path);
  /*
  if (IsFolder())
    return AllocString(path);
  LPCTSTR name = GetServerFileName();
  return AllocPathName(path, name);
  */
}

int KSyncItem::uploadFileItem()
{
#ifdef USE_SYNC_ENGINE
  BOOL success = FALSE;
  LPTSTR filename = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);

  if (mpServerConflictMessage != NULL)
  {
    delete[] mpServerConflictMessage;
    mpServerConflictMessage = NULL;
  }

  if (!IsFileExist(filename))
  {
    delete[] filename;
    return FILE_SYNC_UPLOAD_FAIL;
  }

#ifdef _WITH_SEPARATED_CHECKOUT
  // checkout and upload is done on bg-thread
  if ((lstrlen(GetServerFileOID()) > 0) && (lstrlen(GetServerStorageOID()) > 0)) // replace existing file
  {
    int n = gpCoreEngine->CheckOutDocument(mServerOID, GetServerFileOID(), 
      mParent->mServerOID, mParent->GetServerFullPathIndex(), &mpServerConflictMessage);
    if (n == 0)
    {
      delete[] filename;
      return FILE_SYNC_ERROR_CHECKOUT;
    }
  }
  else
  {
    mServer.StorageOID[0] = '\0';
    mServer.FileOID[0] = '\0';
  }
#endif

  // for test
#ifdef _DEBUG_ERRORED
  return FILE_SYNC_ERROR_CHECKOUT;
#endif

  if (lstrlen(mServerNew.StorageOID) > 0)
    StringCchCopy(mServer.StorageOID, MAX_STGOID, mServerNew.StorageOID);

#ifdef _WITH_SEPARATED_CHECKOUT
  int r = gpCoreEngine->StartUploadItemFile(filename, mServer.StorageOID, this);
#else
  int r = gpCoreEngine->StartUploadItemFile(filename, mServerOID, GetServerFileOID(), mServer.StorageOID,
    mParent->mServerOID, mParent->GetServerFullPathIndex(), this);
#endif
  delete[] filename;
  return r;
#else
  return FILE_SYNC_UPLOAD_FAIL;
#endif // USE_SYNC_ENGINE
}

BOOL KSyncItem::checkRelativeNameChanged()
{
  BOOL changed = FALSE;
  LPTSTR prevRelativeName = AllocRelativePathName();
  LPTSTR currRelativeName = AllocSyncResultName(0);
  SplitPathNameAlloc(currRelativeName, NULL, &mLocal.pFilename);

  BOOL path_changed = isPathChanged(prevRelativeName, currRelativeName);
  if (path_changed)
  {
    // move item in folder tree & move ifo file
    KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    rootFolder->MoveSyncItem(this, currRelativeName, TRUE);
  }

  if (StrCmpI(prevRelativeName, currRelativeName) != 0)
  {
    // SetRelativePathName(currRelativeName);
    MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE);
    changed = TRUE;
  }
  delete[] prevRelativeName;
  delete[] currRelativeName;
  return changed;
}

int KSyncItem::downloadFileItem()
{
#ifdef USE_SYNC_ENGINE
#ifdef _DEBUG_DOWNLOAD_ERR
  static int download_count = 0;
#endif

  LPTSTR filename = AllocSyncResultName(AS_FULLPATH);
  int len = lstrlen(filename);

#ifdef _DEBUG_DOWNLOAD_ERR
  if (IsOverridePath())
  {
    download_count++;
    if ((download_count & 0x007) == 7)
      return FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH;
  }
#endif

  if (len > KMAX_PATH)
  {
    delete[] filename;
	  return FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH;
  }

  int r = gpCoreEngine->StartDownloadItemFile(filename, mServerOID, GetServerFileOID(), GetServerStorageOID(), this);
  delete[] filename;

  if (r == R_SUCCESS)
    return R_SUCCESS;
  else if (r == R_FAIL_DOWNLOAD_NO_CDISK_SPACE)
    return FILE_SYNC_NO_CDISK_SPACE;
  else if (r == R_FAIL_NO_PERMISSION_TO_LOCAL_WRITE)
    return FILE_SYNC_NO_LOCAL_PERMISSION;

#endif // USE_SYNC_ENGINE
    return FILE_SYNC_DOWNLOAD_FAIL;
}

// After upload successfuly
BOOL KSyncItem::UpdateDocumentItemOID(LPCTSTR filename, LPCTSTR serverStorageOID)
{
  // update new storage OID
  StringCchCopy(mServer.StorageOID, MAX_STGOID, serverStorageOID);

  // when both side modified & upload, use newFileOID
  if (lstrlen(mServerNew.FileOID) > 0)
    StringCchCopy(mServer.FileOID, MAX_OID, mServerNew.FileOID);

  int r = gpCoreEngine->UpdateUploadDocumentItem(*this, mParent->mServerOID, mParent->GetServerFullPathIndex());

  return (r == R_SUCCESS);
}

TCHAR tag_history_upload[] = _T("upload");
TCHAR tag_history_download[] = _T("download");
TCHAR tag_history_done[] = _T("done");
TCHAR tag_history_fail[] = _T("fail");
TCHAR tag_history_name[] = _T("name");
TCHAR tag_history_path[] = _T("path");
TCHAR tag_history_user[] = _T("user");

int KSyncItem::OnLoadDone(BOOL isDown, int result, LPCTSTR baseFolder, LPCTSTR conflictMessage)
{
  TCHAR msg[300];
  /*
  if (result != R_SUCCESS)
  {
    StringCchPrintf(msg, 300, _T("Error = 0x%x"), result);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
  }
  */
  if (isDown)
  {
    StringCchCopy(msg, 300, tag_history_download);
    if (result != R_SUCCESS)
    {
      if (result == R_FAIL_NO_PERMISSION_TO_LOCAL_WRITE)
        mConflictResult = FILE_SYNC_NO_LOCAL_PERMISSION;
      else if (result == R_FAIL_DOWNLOAD_WRITE_FAIL)
        mConflictResult = FILE_SYNC_DOWNLOAD_WRITE_FAIL;
      else if (result == R_FAIL_LOCAL_MODIFIED_AFTER_COMPARE)
        mConflictResult = FILE_SYNC_LOCAL_MODIFIED_AFTER_COMPARE;
      else
        mConflictResult = FILE_SYNC_DOWNLOAD_FAIL;
    }
  }
  else
  {
    StringCchCopy(msg, 300, tag_history_upload);
    if (result != R_SUCCESS)
    {
#if 0
      BOOL create_new = ((lstrlen(mServer.StorageOID) == 0) || (lstrlen(mServer.FileOID) == 0)); // newly created
      if (!create_new) // unlock checkouted doc
      {
        gpCoreEngine->UnlockDocument(GetServerFileOID(), NULL);
      }
#endif

      mConflictResult = FILE_SYNC_UPLOAD_FAIL;
    }
  }
  checkRelativeNameChanged();

  if ((result == R_SUCCESS) && (mpServerConflictMessage != NULL))
  {
    delete[] mpServerConflictMessage;
    mpServerConflictMessage = NULL;
  }

  //LPTSTR filename = AllocFullPathName();

  StringCchCat(msg, 300, _T("_"));
  if (result == R_SUCCESS)
  {
    StringCchCat(msg, 300,tag_history_done);

    // MergeLocalInfo(mLocal, mLocalNew);
    if (isDown)
      RefreshLocalNewFileInfo(mUserOid);
    UpdateLocalNewInfo(baseFolder);

    // 다른 경로로 다운로드한 경우 서버측 정보 클리어해서 새로 추가된 항목으로 처리되도록.
    if ((mConflictResult & FILE_SYNC_ERR) == FILE_SYNC_NOT_EXIST_DRIVE)
    {
      clearServerSideInfo();
      mServerState = 0;
    }
    else
    {
      UpdateServerNewInfo();
      mServerState = ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST;
    }

    mLocalState = ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST;
    mResult = 0;
    mConflictResult = 0;

    clearRenameInfo();
  }
  else
  {
    mResult = mConflictResult;
    StringCchCat(msg, 300,tag_history_fail);

    TCHAR errstr[30];
    StringCchPrintf(errstr, 30, _T("(0X%X, Error=%d)"), mConflictResult, result);
    StringCchCat(msg, 300,errstr);

    if ((conflictMessage != NULL) && (lstrlen(conflictMessage) > 0))
      AllocCopyString(&mpServerConflictMessage, conflictMessage);
  }

  StringCchCat(msg, 300, _T(","));
  StringCchCat(msg, 300, tag_history_path);
  StringCchCat(msg, 300, _T("="));
  LPCTSTR p = GetPath();
  StringCchCat(msg, 300, (p != NULL) ? p : _T("NULL"));

  /* path에 포함되어 있음
  StringCchCat(msg, 300, _T(","));
  StringCchCat(msg, 300, tag_history_name);
  StringCchCat(msg, 300, _T("="));
  StringCchCat(msg, 300, GetFilename());
  */

  if (result == R_SUCCESS)
  {
    StringCchCat(msg, 300, _T(","));
    StringCchCat(msg, 300, tag_history_user);
    StringCchCat(msg, 300, _T("="));
    StringCchCat(msg, 300, isDown ? mServer.UserOID : mLocal.UserOID);
  }

  if ((result != R_SUCCESS) && (conflictMessage != NULL) && (lstrlen(conflictMessage) > 0))
  {
    StringCchCat(msg, 300, _T(","));
    StringCchCat(msg, 300, conflictMessage);
  }

  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

#ifdef USE_SYNC_ENGINE
  // history-by-sqlite3
  LPTSTR name = AllocSyncResultName(isDown ? 0 : AS_LOCAL);
  int state = GetHistoryStateFromSyncResult(result, (isDown ? SYNC_H_DOWNLOAD : SYNC_H_UPLOAD));
  
  // KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  LPCTSTR lastUser = isDown ? mServer.UserOID : NULL;
  StoreHistory(state, baseFolder, name, 0, lastUser, mpServerConflictMessage);
  delete[] name;
#endif

  int mask = META_MASK_CONFLICT | META_MASK_SYNCRESULT;
  if (result == R_SUCCESS)
    mask = META_MASK_ALL;
  else if(IsValidSystemTime(mServerNew.FileTime)) // store new info
    mask |= META_MASK_SERVERSTATE|META_MASK_SERVEROID | META_MASK_NEW_SERVER_SIDE | META_MASK_NEW_LOCAL_SIDE;

  StoreMetafile(NULL, mask, 0);

  // synchronizing flag is cleared when mResult = 0, so we can only refresh on shell
  // anyway clear synchronizing
  // 메타데이터 업데이트 이후 shell icon update
  SetItemSynchronizing(baseFolder, 0, FALSE);

  if ((mConflictResult == FILE_SYNC_UPLOAD_FAIL) || // check if parent folder is deleted
    (mConflictResult == FILE_SYNC_DOWNLOAD_FAIL))
  {
    int f = (mConflictResult == FILE_SYNC_UPLOAD_FAIL) ? AS_LOCAL : 0;
    LPTSTR name = AllocSyncResultName(f|AS_FULLPATH);

    /*
    if (!IsFileExist(name))
    {
      ((KSyncFolderItem *)mParent)->CheckLocalFolderExistOnSyncing();
    }
    else
    */
    ((KSyncFolderItem *)mParent)->CheckServerFolderExistOnSyncing();
    delete[] name;
  }

  return mConflictResult;
}

void KSyncItem::OnLoadCanceled(LPCTSTR baseFolder)
{
  SetItemSynchronizing(baseFolder, 0, WITH_STORE_METAFILE);
}

// check local file is modified after sync-start
BOOL KSyncItem::CheckModifiedAfterSyncCompare(LPCTSTR filename)
{
  SYSTEMTIME currentFileTime;
  TCHAR msg[256];

  GetFileModifiedTime(filename, &currentFileTime, NULL);

  BOOL changed_after_compare = FALSE;
  if (IsValidSystemTime(mLocalNew.FileTime))
  {
    changed_after_compare = !!CompareSystemTime(mLocalNew.FileTime, currentFileTime);

    if (changed_after_compare)
    {
      TCHAR oldtime[64];
      TCHAR newtime[64];
      SystemTimeToTString(oldtime, 64, mLocalNew.FileTime);
      SystemTimeToTString(newtime, 64, currentFileTime);

      StringCchPrintf(msg, 256, _T("LocalNew.FileTime=%s, current file time=%s"), oldtime, newtime);
    }
  }
  else if (IsValidSystemTime(mLocal.FileTime))
  {
    changed_after_compare = !!CompareSystemTime(mLocal.FileTime, currentFileTime);

    if (changed_after_compare)
    {
      TCHAR oldtime[64];
      TCHAR newtime[64];
      SystemTimeToTString(oldtime, 64, mLocal.FileTime);
      SystemTimeToTString(newtime, 64, currentFileTime);

      StringCchPrintf(msg, 256, _T("Local.FileTime=%s, current file time=%s"), oldtime, newtime);
    }
  }
  else if (!IsOverridePath())
  {
    changed_after_compare = TRUE; // file is newly added.
    StringCchCopy(msg, 256, _T("Local FileTime empty"));
  }

  if (changed_after_compare) // local file changed 
  {
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    SetResult(FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
    SetConflictResult(FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
    return TRUE;
  }
  return FALSE;
}

#define CHECK_NAME_ON_SERVER_BEFORE_RENAME

int KSyncItem::renameServerObjectAs(BOOL renameObject)
{
#ifdef USE_SYNC_ENGINE
  // local file was moved/renamed by user.
  // so, we have to move/rename on server location
  TCHAR msg[512];
  int success = 0;

  LPTSTR prevRelativeName = AllocRelativePathName();
  LPTSTR oldpathname = AllocSyncResultName(0);
  LPTSTR newpathname = AllocSyncResultName(AS_LOCAL);
  BOOL path_changed = isPathChanged(oldpathname, newpathname);

  /*
  TCHAR changedName[KMAX_FILENAME];
  LPTSTR changedLocation = new TCHAR[lstrlen(mpLocalRenamedAs) + 1];
  SplitPathName(mpLocalRenamedAs, changedLocation, changedName);
  */
  LPTSTR changedName = NULL;
  LPTSTR changedLocation = NULL;
  SplitPathNameAlloc(newpathname, &changedLocation, &changedName);
  LPTSTR oldName = NULL;
  LPTSTR oldLocation = NULL;
  SplitPathNameAlloc(oldpathname, &oldLocation, &oldName);

  int rtn = R_SUCCESS;

  KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

  if (renameObject)
  {
    TCHAR documentName[KMAX_PATH];
#ifdef CHECK_NAME_ON_SERVER_BEFORE_RENAME
    if (gpCoreEngine->GetNameOfDocumentOID(mServerOID, documentName))
#else
    // do not query document name again
    StringCchCopy(documentName, KMAX_PATH, GetServerFileName());
#endif

    if (path_changed)
    {
      TCHAR currentPathOID[MAX_OID];
      BOOL able = TRUE;

      // find new parent folder item
      KSyncFolderItem *newParent = rootFolder->GetFolderOIDCascade(changedLocation, mServerMoveToOID);
      rootFolder->GetFolderOIDCascade(oldLocation, currentPathOID);

      if ((newParent != NULL) && (lstrlen(mServerMoveToOID) > 0))
      {
        TCHAR currentName[KMAX_PATH];
        TCHAR tempName[KMAX_PATH];
        BOOL restored_on_end = FALSE;
        StringCchCopy(currentName, KMAX_PATH, GetServerFileName());

        LPTSTR poldpath = NULL;
        LPTSTR pnewpath = NULL;
        LPTSTR pnewpathIndex = NULL;

        // check if new-parent-folder has same named child
        if (newParent->CheckIfChildNameExist(currentName, this, 0, FALSE))
        {
          int r = newParent->CheckIfChildNameExist(changedName, NULL, 1, FALSE);
          if (r == 2) // 동일한 이름이 존재하고 그것이 최종 이름이면 임시 이름 바꾸기로 해결되지 못하니 충돌로 처리함
          {
            rtn = FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS;
            goto end_rtn;
          }
          if (r)
          {
            // make temporary filename
            newParent->ReadyUniqueChildName(changedName, tempName, 2, FALSE);
            restored_on_end = TRUE;
          }
          else
          {
            StringCchCopy(tempName, KMAX_PATH, changedName);
          }

          TCHAR msg[512] = _T("Server File '");
          StringCchCat(msg, 512, GetDisplayPathName(GetServerRelativePath()));
          StringCchCat(msg, 512, _T(">"));
          StringCchCat(msg, 512, currentName);
          StringCchCat(msg, 512, _T("' renamed temporarily '"));
          StringCchCat(msg, 512, tempName);
          StringCchCat(msg, 512, _T("'"));

          if (gpCoreEngine->ChangeDocumentName(mServerOID, tempName, mParent->mServerOID, 
            mParent->GetServerFullPathIndex(), &mpServerConflictMessage))
          {
            if (mpLocalRenamedAs != NULL)
              delete[] mpLocalRenamedAs;
            mpLocalRenamedAs = AllocPathName(changedLocation, tempName);

            // MakeFullPathName(mLocalRenamedAs, KMAX_PATH, changedLocation, tempName);
            if (restored_on_end)
            {
              SetServerTemporaryRenamed(changedName);
            }
            else
            {
              // rename done hear
              AllocCopyString(&mServerNew.pFilename, changedName);
            }
            StringCchCat(msg, 512, _T(" done"));
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
            AllocCopyString(&mLocal.pFilename, tempName);
            StringCchCopy(changedName, KMAX_PATH, tempName);
          }
          else
          {
            StringCchCat(msg, 512, _T(" failure"));
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
            rtn = FILE_SYNC_NO_MOVE_PERMISSION;
            goto end_rtn;
          }
        }

        int srcParentPermission = 0;
        int destParentPermission = 0;

        if (!gpCoreEngine->GetServerFolderSyncInfo(rootFolder, currentPathOID, &srcParentPermission, NULL, &poldpath, NULL, NULL))
        {
          StringCchCopy(msg, 512, _T("current Server Folder '"));
          StringCchCat(msg, 512, mParent->mServerOID);
          StringCchCat(msg, 512, _T("' not found"));
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        if (!gpCoreEngine->GetServerFolderSyncInfo(rootFolder, mServerMoveToOID, &destParentPermission, NULL, &pnewpath, &pnewpathIndex, NULL))
        {
          StringCchCopy(msg, 512, _T("destination Server Folder '"));
          StringCchCat(msg, 512, mServerMoveToOID);
          StringCchCat(msg, 512, _T("' not found"));
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
          // StringCchCopy(changedLocation, KMAX_PATH, mServerMoveToOID);
        }
        StringCchCopy(msg, 512, _T("Server File '"));
        StringCchCat(msg, 512, mLocal.pFilename);
        StringCchCat(msg, 512, _T("' moved '"));
        StringCchCat(msg, 512, GetDisplayPathName(poldpath));
        StringCchCat(msg, 512, _T("' to '"));
        StringCchCat(msg, 512, GetDisplayPathName(pnewpath));
        StringCchCat(msg, 512, _T("'"));

        able = CheckPermissionFileMove(srcParentPermission, destParentPermission, GetServerPermission());

        if (able && (gpCoreEngine->MoveItemTo(mServerOID, mServerMoveToOID, pnewpathIndex, FALSE, &mpServerConflictMessage) == R_SUCCESS))
        {
          StringCchCat(msg, 512, _T(" : success"));
          success = 1;

          // changed location is changedLocation
          /*
          if (changedLocation != NULL)
            delete[] changedLocation;
          changedLocation = AllocPathName(((KSyncFolderItem *)mParent)->GetRootPath(), mpLocalRenamedAs);
          */
        }
        else
        {
          StringCchCat(msg, 512, _T(" : failure"));
          if (!able)
            StringCchCat(msg, 512, _T(", no_permission"));
        }

        if (poldpath != NULL)
          delete[] poldpath;
        if (pnewpath != NULL)
          delete[] pnewpath;
        if (pnewpathIndex != NULL)
          delete[] pnewpathIndex;
      }
      else
      {
        StringCchCopy(msg, 512, _T("Server Folder '"));
        StringCchCat(msg, 512, changedLocation);
        StringCchCat(msg, 512, _T("' not found : failure"));
      }

      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      if (!able)
      {
        rtn = FILE_SYNC_NO_MOVE_PERMISSION;
        goto end_rtn;
      }
    }

    // check rename
    if (StrCmpI(changedName, documentName) != 0)
    {
      TCHAR msg[512] = _T("Server File '");
      StringCchCat(msg, 512, documentName);
      StringCchCat(msg, 512, _T("' renamed as'"));

      BOOL able = CheckPermissionFileRename(mParent->GetServerPermission(), GetServerPermission());

      if (able)
      {
        TCHAR orgName[KMAX_PATH];
        TCHAR tempName[KMAX_PATH];
        BOOL restored_on_end = FALSE;

        // 동일한 이름이 존재하고 그것이 최종 이름이면 임시 이름 바꾸기로 해결되지 못하니 충돌로 처리함
        // 비교할때 자기 자신은 제외
        if (((KSyncFolderItem*)mParent)->CheckIfChildNameExist(changedName, this, 0, FALSE))
        {
          if (((KSyncFolderItem*)mParent)->CheckIfChildNameExist(changedName, this, 1, FALSE) == 2)
          {
            rtn = FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS;
            goto end_rtn;
          }

          ((KSyncFolderItem*)mParent)->ReadyUniqueChildName(changedName, tempName, 2, FALSE);
          restored_on_end = TRUE;
          StringCchCopy(orgName, KMAX_PATH, changedName);

          delete[] changedName;
          changedName = AllocString(tempName);
          // StringCchCopy(changedName, KMAX_PATH, tempName);
        }

        StringCchCat(msg, 512, changedName);
        StringCchCat(msg, 512, _T("'"));

        if (gpCoreEngine->ChangeDocumentName(mServerOID, changedName, mParent->mServerOID, 
          mParent->GetServerFullPathIndex(), &mpServerConflictMessage))
        {
          StringCchCat(msg, 512, _T(" : success"));
          success |= 2;

          if (restored_on_end)
            SetServerTemporaryRenamed(orgName);

          AllocCopyString(&mLocal.pFilename, changedName);
        }
        else
        {
          StringCchCat(msg, 512, _T(" : failure"));
        }
      }
      else
      {
        StringCchCat(msg, 512, _T(", no_permission"));
      }

      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      if (!able)
      {
        rtn = FILE_SYNC_NO_RENAME_PERMISSION;
        goto end_rtn;
      }
    }
  }
  else
  {
    // 양쪽 모두 이름 변경된 상태에서 업로드 선택한 경우, 서버측 이름 변경된 것으로 정리하여야 함.
    if (path_changed)
      success = 1;

    SplitPathNameAlloc(newpathname, /*mLocal.FullPath*/NULL, &mLocal.pFilename);
    success |= 2;
  }

  if (success & 1)
  {
    // new location is changedLocation
    rootFolder->MoveSyncItem(this, changedLocation, FALSE);
  }

  if (success)
    MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE, rootFolder->GetStorage());

  if (success)
  {
    // remove from KRenameStorage
    rootFolder->RemoveMovedRenamedOriginal(newpathname); // not a temp name, mpLocalRenamedAs

    clearRenameInfo();

    mLocalState &= ~ITEM_STATE_MOVED;
    mServerState &= ~ITEM_STATE_MOVED;
  }
  else
    rtn = FILE_SYNC_SERVER_RENAME_FAIL;

end_rtn:
  if (renameObject)
  {
    int state = GetHistoryStateFromSyncResult(rtn, SYNC_H_RENAME_SERVER_FILE);
    int len = lstrlen(newpathname) + lstrlen(mpServerConflictMessage) + 10;
    LPTSTR buff = new TCHAR[len];
    StringCchCopy(buff, len, newpathname);
    if ((state != SYNC_H_RENAME_SERVER_FILE) && (mpServerConflictMessage != NULL) &&
      (lstrlen(mpServerConflictMessage) > 0))
    {
      StringCchCat(buff, len, _T(", error : "));
      StringCchCat(buff, len, mpServerConflictMessage);
    }
    StoreHistory(state, rootFolder->GetRootPath(), prevRelativeName, 0, NULL, buff);
    delete[] buff;
  }
  if (changedLocation != NULL)
    delete[] changedLocation;
  if (changedName != NULL)
    delete[] changedName;
  if (oldLocation != NULL)
    delete[] oldLocation;
  if (oldName != NULL)
    delete[] oldName;

  delete[] oldpathname;
  delete[] newpathname;
  delete[] prevRelativeName;
  return rtn;

#endif
  return FILE_SYNC_SERVER_RENAME_FAIL;
}

int KSyncItem::renameLocalObjectAs(BOOL renameObject)
{
#ifdef USE_SYNC_ENGINE
  LPTSTR prevRelativeName = AllocRelativePathName();
  LPTSTR oldpathname = AllocSyncResultName(AS_LOCAL);
  LPTSTR newpathname = AllocSyncResultName(0);

  if (StrCmpI(oldpathname, newpathname) == 0)
  {
    clearRenameInfo();
    mLocalState &= ~ITEM_STATE_MOVED;
    mServerState &= ~ITEM_STATE_MOVED;

    delete[] prevRelativeName;
    delete[] oldpathname;
    delete[] newpathname;
    return R_SUCCESS;
  }

  BOOL path_changed = isPathChanged(oldpathname, newpathname);
  int success = 0;
  int last_err = 0;
  BOOL restored_on_end = FALSE;
  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  LPTSTR newfile = NULL;
  LPTSTR oldfile = NULL;

  if (renameObject)
  {
    // check if same named folder exist.
    TCHAR changedPath[KMAX_PATH];
    TCHAR changedName[KMAX_PATH];

    SplitPathName(newpathname, changedPath, changedName);
    KSyncFolderItem* newParent = (KSyncFolderItem*)rootFolder->FindChildItemCascade(changedPath, 1);

    if ((newParent != NULL) && (newParent != mParent) &&
      (newParent->CheckIfChildNameExist(changedName, NULL, 2, TRUE) == 2))
    {
      return FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS;
    }

    newfile = AllocAbsolutePathName(newpathname);
    oldfile = AllocAbsolutePathName(oldpathname);

    if (IsFileExist(newfile))
    {
      LPTSTR changed = ReadyUniqueLocalName(newfile);
      if (changed != NULL)
      {
        delete[] newfile;
        newfile = changed;
        restored_on_end = TRUE;
        SplitPathName(changed, NULL, changedName);
      }
    }

    // Modify Table에 등록되지 않도록
    rootFolder->AddSyncingFileItem(newfile); // NEED ??

    // int root_len = lstrlen(rootFolder->GetBaseFolder()) + 1;
    // save 'new=old' to do not stored on rename history
    // rootFolder->OnStoreRenamed(newfile, oldfile, TRUE);
    // 저장할 필요 없음. OnFileRenamed 발생해도 DB에 저장되지 않음.
    // rootFolder->OnStoreCancelRenamed(oldpathname, newpathname);

    if (!IsFileExist(oldfile) || ((last_err = KMoveFile(oldfile, newfile)) == 0))
    {
      rootFolder->OnStoreCancelRenamed(oldfile, newfile);
      success = 2;
      if (restored_on_end)
        SetLocalTemporaryRenamed(changedName);
    }
  }
  else
  {
    success = 2;
  }

  if (success)
  {
    if (path_changed)
    {
      // move item in folder tree & move ifo file
      // 새로운 경로는 newpathname에...
      rootFolder->MoveSyncItem(this, newpathname, TRUE);
    }

    TCHAR msg[600] = _T("Move done [");
    StringCchCat(msg, 600, GetDisplayPathName(oldpathname));
    StringCchCat(msg, 600, _T("] to ["));
    StringCchCat(msg, 600, GetDisplayPathName(newpathname));
    StringCchCat(msg, 600, _T("]"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    SplitPathNameAlloc(newpathname, /*mLocal.FullPath*/NULL, &mLocal.pFilename);

    // StringCchCopy(mLocalNew.FullPath, KMAX_PATH, mLocal.FullPath);
    AllocCopyString(&mLocalNew.pFilename, mLocal.pFilename);

    clearRenameInfo();
    mLocalState &= ~ITEM_STATE_MOVED;
    mServerState &= ~ITEM_STATE_MOVED;

    // move/rename ifo metafile
    MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE, rootFolder->GetStorage());

    // remove from KRenameStorage
    rootFolder->RemoveMovedRenamedOriginal(mpLocalRenamedAs);
  }

  int rtn = R_SUCCESS;
  if (!success)
  {
    // int last_err = GetLastError();

    TCHAR msg[600];
    StringCchPrintf(msg, 600, _T("MoveFile(%s, %s) fail, last_error = %d"), oldpathname, newpathname, last_err);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    if (last_err == ERROR_ACCESS_DENIED) // 5, access denied
      rtn = FILE_SYNC_NO_LOCAL_PERMISSION;
    else if (last_err == ERROR_ALREADY_EXISTS) // 183, already exist
      rtn = FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS;
    else if (last_err == ERROR_SHARING_VIOLATION) // 32
      rtn = FILE_SYNC_LOCAL_RENAME_SHARING_VIOLATION;
    else
      rtn = FILE_SYNC_LOCAL_RENAME_FAIL;
  }

  if (renameObject)
  {
    int state = GetHistoryStateFromSyncResult(rtn, SYNC_H_RENAME_LOCAL_FILE);
    StoreHistory(state, rootFolder->GetRootPath(), prevRelativeName, 0, NULL, newpathname);
  }

  if (newfile != NULL)
    delete[] newfile;
  if(oldfile != NULL)
    delete[] oldfile;

  if (prevRelativeName != NULL)
    delete[] prevRelativeName;
  delete[] oldpathname;
  delete[] newpathname;

  return rtn;
#endif
  return FILE_SYNC_LOCAL_RENAME_FAIL;
}

void KSyncItem::updateMetaAsRenamed()
{
  LPTSTR prevRelativeName = AllocRelativePathName();
  LPTSTR locpathname = AllocSyncResultName(AS_LOCAL);
  BOOL path_changed = isPathChanged(prevRelativeName, locpathname);
  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

  if (path_changed)
  {
    // move item in folder tree & move ifo file
    rootFolder->MoveSyncItem(this, mpServerRenamedAs, TRUE);
  }

  SplitPathNameAlloc(locpathname, NULL, &mLocal.pFilename); // 파일명도 교체.
  SetRelativePathName(locpathname);

  // remove from KRenameStorage
  rootFolder->RemoveMovedRenamedOriginal(locpathname);

  clearRenameInfo();
  mLocalState &= ~ITEM_STATE_MOVED;
  mServerState &= ~ITEM_STATE_MOVED;

  MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE);
  delete[] prevRelativeName;
  delete[] locpathname;
}

BOOL KSyncItem::UpdateRenameLocal(LPCTSTR baseFolder, LPCTSTR name)
{
  // rename ifo file
  AllocCopyString(&mLocal.pFilename, name);

  KSyncRootStorage *storage = GetSafeStorage();
  if (storage != NULL)
  {
    storage->MoveMetafileAsRename(this, NULL, WITH_STORE_METAFILE);
    StoreMetafile(storage, META_MASK_FILENAME, 0); // 필요없을듯
  }
  return TRUE;
}

BOOL KSyncItem::renameLocal(LPCTSTR baseFolder, LPCTSTR name, BOOL renameNow)
{
  BOOL r = FALSE;
#ifdef USE_SYNC_ENGINE
  TCHAR msg[512];

  LPTSTR prevMetaName = AllocRelativePathName();
  LPTSTR oldpathname = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
  LPTSTR currentPath = new TCHAR[lstrlen(oldpathname) + 1];
  SplitPathName(oldpathname, currentPath, NULL);

  int len = lstrlen(currentPath) + lstrlen(name) + 2;
  LPTSTR newpathname = new TCHAR[len];
  MakeFullPathName(newpathname, len, currentPath, name);

  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

  try
  {
    if (renameNow)
    {
      CFile::Rename(oldpathname, newpathname);
      StringCchCopy(msg, 512, _T("File renamed "));

      rootFolder->GetStorage()->ClearRenameT(oldpathname, NULL);
    }
    else
    {
      StringCchCopy(msg, 512, _T("Alredy renamed "));
    }

    // change in tree
    LPCTSTR rootPath = rootFolder->GetRootPath();
    LPTSTR relativePath = &newpathname[lstrlen(rootPath)+1];
    ChangeRelativePathName(rootFolder, relativePath, TRUE);

    StringCchCopy(mLocal.UserOID, MAX_OID, mUserOid);
    GetFileModifiedTime(newpathname, &mLocalNew.FileTime, &mLocalNew.AccessTime);
    mLocal.FileTime = mLocalNew.FileTime;
    mLocal.AccessTime = mLocalNew.AccessTime;

    if (mpLocalRenamedAs != NULL)
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
    KSyncRootStorage *storage = rootFolder->GetStorage();
    storage->MoveMetafileAsRename(this, prevMetaName, WITH_STORE_METAFILE);
    r = TRUE;
  }
  catch(CFileException *e)
  {
    StringCchPrintf(msg, 512, _T("File rename failure(error=%x) "), e->m_cause);
  }

  StringCchCat(msg, 512, oldpathname);
  StringCchCat(msg, 512, _T(" as "));
  StringCchCat(msg, 512, name);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  delete[] newpathname;
  delete[] oldpathname;
  delete[] currentPath;
  delete[] prevMetaName;

#endif
  return r;
}

BOOL KSyncItem::renameServer(LPCTSTR baseFolder, LPCTSTR name, BOOL renameNow)
{
  BOOL r = FALSE;
#ifdef USE_SYNC_ENGINE
  TCHAR msg[512] = _T("Server File renamed ");

  if(renameNow)
  {
    BOOL done = FALSE;

    if (mParent != NULL)
    {
      if (IsFolder())
        done = gpCoreEngine->ChangeFolderName(mServerOID, GetServerFullPathIndex(), _T("?"), name, &mpServerConflictMessage);
      else
        done = gpCoreEngine->ChangeDocumentName(mServerOID, name, mParent->mServerOID,
          mParent->GetServerFullPathIndex(), &mpServerConflictMessage);
    }

    if (done)
    {
      LPTSTR prevMetaName = AllocRelativePathName();
      AllocCopyString(&mLocal.pFilename, name);
      OnPathChanged();

      RefreshServerState();
      CopyServerInfo(mServerNew, mServer); //should same

#if 0
      // rename ifo file
      TCHAR oldpathname[KMAX_METAPATH];
      StringCchCopy(oldpathname, KMAX_METAPATH, mMetafilename);
      MakeMetafilename(baseFolder);
      KMoveFile(oldpathname, mMetafilename);
      StoreMetafile(NULL, META_MASK_FILENAME); //###mServer.FileTime
#else
      KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
      KSyncRootStorage *storage = rootFolder->GetStorage();
      storage->MoveMetafileAsRename(this, prevMetaName, WITH_STORE_METAFILE);
#endif

      if (mpServerRenamedAs != NULL)
      {
        delete[] mpServerRenamedAs;
        mpServerRenamedAs = NULL;
      }
      r = TRUE;
      delete[] prevMetaName;
    }
    else
    {
      StringCchCopy(msg, 512, _T("Server File rename failure "));
    }
  }
  else
  {
    StringCchCopy(msg, 512, _T("Already "));
    RefreshServerState();
    AllocCopyString(&mLocal.pFilename, mServerNew.pFilename);
    OnPathChanged();
    r = TRUE;
  }

  StringCchCat(msg, 512, mLocal.pFilename);
  StringCchCat(msg, 512, _T(" as "));
  StringCchCat(msg, 512, name);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
  return r;
}

LPCTSTR KSyncItem::GetLocalFileName()
{
  return (((mLocalNew.pFilename != NULL) && (lstrlen(mLocalNew.pFilename) > 0)) ? mLocalNew.pFilename : mLocal.pFilename);
}

LPCTSTR KSyncItem::GetServerFileName()
{
  if((mServerNew.pFilename != NULL) && (lstrlen(mServerNew.pFilename) > 0))
    return mServerNew.pFilename;
  else if((mServer.pFilename != NULL) && (lstrlen(mServer.pFilename) > 0))
    return mServer.pFilename;
  return mLocal.pFilename;
}

#ifdef RENAME_ON_SYNC_TIME
BOOL KSyncItem::IsRenamed()
{
  return ((lstrlen(mLocalRename) > 0) || (lstrlen(mServerRename) > 0));
}

#endif

void KSyncItem::SetServerTemporaryRenamed(LPCTSTR realName)
{
  AllocCopyString(&mpServerTempRenamed, realName);
}

void KSyncItem::SetLocalTemporaryRenamed(LPCTSTR realName)
{
  AllocCopyString(&mpLocalTempRenamed, realName);
}

LPTSTR KSyncItem::ReadyUniqueLocalName(LPCTSTR pathname)
{
  int n = lstrlen(pathname) + 1;
  LPTSTR newName = new TCHAR[n + 5];
  StringCchCopy(newName, n + 5, pathname);
  StringCchCat(newName, n + 5, _T(".AAA"));

  do
  {
    if (!IsFileExist(newName))
      return newName;
  } while (MakeNextSequenceName(4, &newName[n]));
  return NULL;
}

BOOL KSyncItem::IsTemporaryRenamed()
{
  return (IsServerTemporaryRenamed() || IsLocalTemporaryRenamed());
}

BOOL KSyncItem::ParentWillMove()
{
  if (mParent != NULL)
  {
    if (mParent->isLocalRenamedAs() || mParent->isServerRenamedAs())
      return TRUE;
    return mParent->ParentWillMove();
  }
  return FALSE;
}

int KSyncItem::RestoreTemporaryRenamed()
{
  int rtn = 0;
  LPTSTR prevRelativeName = AllocRelativePathName();
  if (IsServerTemporaryRenamed())
  {
    TCHAR msg[512] = _T("Server File '");
    StringCchCat(msg, 512, mLocal.pFilename);
    StringCchCat(msg, 512, _T("' to '"));
    StringCchCat(msg, 512, mpServerTempRenamed);
    StringCchCat(msg, 512, _T("' tempRename restore"));

    TCHAR* errmsg = NULL;

    if (gpCoreEngine->ChangeDocumentName(mServerOID, mpServerTempRenamed, mParent->mServerOID, 
      mParent->GetServerFullPathIndex(), &errmsg))
    {
      StringCchCat(msg, 512, _T(" done"));
      AllocCopyString(&mLocal.pFilename, mpServerTempRenamed);
      delete[] mpServerTempRenamed;
      mpServerTempRenamed = NULL;
      rtn = 1;
    }
    else
    {
      StringCchCat(msg, 512, _T(" failure:"));
      StringCchCat(msg, 512, errmsg);
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  if (IsLocalTemporaryRenamed())
  {
    TCHAR msg[512] = _T("Local File '");
    StringCchCat(msg, 512, mLocal.pFilename);
    StringCchCat(msg, 512, _T("' to '"));
    StringCchCat(msg, 512, mpLocalTempRenamed);
    StringCchCat(msg, 512, _T("' tempRename restore"));

    LPTSTR curPathName = AllocAbsolutePathName();
    LPTSTR tempName = AllocPathName(GetPath(), mpLocalTempRenamed);
    LPTSTR tempPathName = AllocAbsolutePathName(tempName);
    delete[] tempName;

    KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

    // Modify Table에 등록되지 않도록
    root->AddSyncingFileItem(curPathName);
    int last_err = KMoveFile(tempPathName, curPathName);
    if (last_err == 0)
    {
      root->OnStoreCancelRenamed(tempPathName, curPathName);

      StringCchCat(msg, 512, _T(" done"));
      // StringCchCopy(mLocal.Filename, KMAX_PATH, mpLocalTempRenamed);
      delete[] mpLocalTempRenamed;
      mpLocalTempRenamed = NULL;
      rtn = 1;
    }
    else
    {
      StringCchCat(msg, 512, _T(" failure"));
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    delete[] curPathName;
    delete[] tempPathName;
  }

  if (rtn)
  {
    MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE);
    StoreMetafile(NULL, META_MASK_FILENAME, 0); //필요없을듯
  }
  delete[] prevRelativeName;
  return rtn;
}

BOOL KSyncItem::IsLocalFileExist()
{
  LPTSTR filename = AllocSyncResultName(AS_LOCAL|AS_FULLPATH|AS_NO_OVERRIDE);
  BOOL r = FALSE;
  if (lstrlen(filename) > 0)
  {
    r = (IsFileOrFolder(filename) >= 0);
    /*
    if (IsFolder())
      r = IsDirectoryExist(filename);
    else
      r = IsFileExist(filename);
    */

    /*
    SYSTEMTIME *time = GetLocalFileTime();
    if ((time != NULL) && IsValidSystemTime(*time)) // 업로드 안된 상태에서는 mLocal.FileTime이 empty.
    {
      TCHAR filepathname[KMAX_PATH];
      GetSyncResultName(filepathname, KMAX_PATH, 0);
      return IsFileExist(filepathname);
    }
    */

    // 상위 폴더가 이동 상태일 경우를 위해
    if (!r)
    {
      LPTSTR newpath = AllocRenamedPathName(AS_LOCAL|AS_FULLPATH);

      if ((newpath != NULL) && (IsFileOrFolder(newpath) >= 0))
        r = TRUE;
      if (newpath != NULL)
        delete[] newpath;
    }
  }
  delete[] filename;
  return r;
}

BOOL KSyncItem::IsServerFileExist()
{
  if (lstrlen(mServerOID) > 0)
  {
    if (IsFolder())
      return TRUE;

    return ( ((lstrlen(mServer.FileOID) > 0) || (lstrlen(mServerNew.FileOID) > 0)) &&
                ((lstrlen(mServer.StorageOID) > 0) || (lstrlen(mServerNew.StorageOID) > 0)) );
  }
  return FALSE;
}

LPTSTR KSyncItem::AllocSyncResultName(int flag)
{
  LPTSTR r = NULL;

  if (IsOverridePath() && !(flag & AS_NO_OVERRIDE))
  {
    r = AllocString(mpOverridePath);
  }
  else if ((flag & AS_LOCAL) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    LPTSTR p = NULL;

    if (flag & AS_NAME_ONLY)
    {
      p = StrRChr(mpLocalRenamedAs, NULL, '\\');
      if (p != NULL)
        ++p;
    }
    if (p == NULL)
      p = mpLocalRenamedAs;
    r = AllocString(p);
  }
  else if (!(flag & AS_LOCAL) && (mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    LPTSTR p = NULL;

    if (flag & AS_NAME_ONLY)
    {
      p = StrRChr(mpServerRenamedAs, NULL, '\\');
      if (p != NULL)
        ++p;
    }
    if (p == NULL)
      p = mpServerRenamedAs;
    r = AllocString(p);
  }
  else
  {
    LPTSTR baseFolder = NULL;
    if ((mParent != NULL) && !(flag & AS_NAME_ONLY))
      baseFolder = ((KSyncFolderItem*)mParent)->GetBaseFolder();
    int len = 0;

    if (baseFolder != NULL)
      len += lstrlen(baseFolder) + 1;

    LPTSTR name = NULL;
    if (!(flag & AS_LOCAL))// !onLocal)
    {
      if (lstrlen(mServerNew.pFilename) > 0)
        name = mServerNew.pFilename;
      else if (lstrlen(mServer.pFilename) > 0)
        name = mServer.pFilename;
    }

    if ((name == NULL) || (lstrlen(name) == 0))
      name = mLocal.pFilename;

    if (name != NULL)
      len += lstrlen(name) + 1;

    r = new TCHAR[len];
    MakeFullPathName(r, len, baseFolder, name);
  }

#ifndef SYNC_ALL_DRIVE
  if (flag & AS_FULLPATH)
  {
    LPCTSTR rootPath = NULL;
    if (mParent != NULL)
    {
      rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();

      int len = lstrlen(r) + lstrlen(rootPath) + 2;
      LPTSTR fullname = new TCHAR[len];
      MakeFullPathName(fullname, len, rootPath, r);

      if (r != NULL)
        delete[] r;
      r = fullname;
    }
  }
#endif
  return r;
}

LPTSTR KSyncItem::AllocRenamedPathName(int flag)
{
  LPTSTR pathname = NULL;
  int len = GetRenamedPathName(NULL, 0, flag) + 1;
  if (len > 0)
  {
    pathname = new TCHAR[len];
    pathname[0] = '\0';
    len = GetRenamedPathName(pathname, len, flag);
    len = lstrlen(pathname);
  }
  return pathname;
}

int KSyncItem::GetRenamedPathName(LPTSTR buff, int len, int flag)
{
  int r = 0;
  LPCTSTR rootPath = NULL;

  if (mParent != NULL)
  {
    r = ((KSyncFolderItem *)mParent)->GetRenamedPathName(buff, len, flag);
    rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
  }

  if ((flag & AS_LOCAL) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    if (buff != NULL)
    {
      if (flag & AS_FULLPATH)
        MakeFullPathName(buff, len, rootPath, mpLocalRenamedAs);
      else
        StringCchCopy(buff, len, mpLocalRenamedAs);
    }
    int n = lstrlen(mpLocalRenamedAs) + 1;
    if (flag & AS_FULLPATH)
      n += lstrlen(rootPath) + 1;

    if (n > r)
      r = n;
  }
  else if (!(flag & AS_LOCAL) && (mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    if (buff != NULL)
    {
      if (flag & AS_FULLPATH)
        MakeFullPathName(buff, len, rootPath, mpServerRenamedAs);
      else
        StringCchCopy(buff, len, mpServerRenamedAs);
    }
    int n = lstrlen(mpServerRenamedAs) + 1;
    if (flag & AS_FULLPATH)
      n += lstrlen(rootPath);
    if (n > r)
      r = n;
  }
  else
  {
    // no rename info
    LPCTSTR p = NULL;
    if (IsFolder() == 2)
      p = ((KSyncFolderItem*)this)->GetRootPath();
    else
      p = GetFilename();

    if (buff != NULL)
    {
      if (lstrlen(buff) > 0) // parent has renamed info
      {
        MakeFullPathName(buff, len, buff, p);
        r++;
      }
      else
        StringCchCopy(buff, len, p);
    }
    r += lstrlen(p) + 1;
  }
  return r;
}

#if 0
LPTSTR KSyncItem::AllocSyncResultFullName(int flag)
{
  LPTSTR relativeName = AllocSyncResultName(flag);
#ifdef SYNC_ALL_DRIVE
  return relativeName;
#else
  LPTSTR r = NULL;

  if (relativeName != NULL)
  {
    LPCTSTR rootPath = NULL;
    if (mParent != NULL)
    {
      rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();

      int len = lstrlen(relativeName) + lstrlen(rootPath) + 2;
      r = new TCHAR[len];
      MakeFullPathName(r, len, rootPath, relativeName);
    }

    delete[] relativeName;
  }
  return r;
#endif
}
#endif

#if 0 // deprecated : use AllocSyncResultName()

void KSyncItem::GetSyncResultRName(LPTSTR name, int length, int flag)
{
  if ((flag & AS_LOCAL) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    if (flag & AS_NAME_ONLY)
      SplitPathName(mpLocalRenamedAs, NULL, name);
    else
      StringCchCopy(name, length, mpLocalRenamedAs);
  }
  else if (!(flag & AS_LOCAL) && (mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    if (flag & AS_NAME_ONLY)
      SplitPathName(mpServerRenamedAs, NULL, name);
    else
      StringCchCopy(name, length, mpServerRenamedAs);
  }
  else
  {
    name[0] = '\0';
    LPTSTR baseFolder = NULL;
    LPTSTR realPath = NULL;
    if ((mParent != NULL) && !(flag & AS_NAME_ONLY))
    {
      int len = ((KSyncFolderItem*)mParent)->GetRenamedPathName(NULL, 0, flag);

      // 상위에서 경로 변경된 경우,
      baseFolder = ((KSyncFolderItem*)mParent)->GetBaseFolder();
      if (len > 0)
      {
        len++;
        realPath = new TCHAR[len];
        realPath[0] = '\0';
        ((KSyncFolderItem*)mParent)->GetRenamedPathName(realPath, len, flag);
        int r = lstrlen(realPath);
#ifdef _DEBUG
        if (len < r)
        {
          TCHAR msg[120];
          StringCchPrintf(msg, 120, _T("GetSyncResultRName alloc=%d, used=%d\n"), len, r);
          OutputDebugString(msg);
        }
#endif
        baseFolder = realPath;
      }
      else
      {
        baseFolder = ((KSyncFolderItem*)mParent)->GetBaseFolder();
      }
    }

    if (!(flag & AS_LOCAL))// !onLocal)
    {
      if (lstrlen(mServerNew.pFilename) > 0)
        MakeFullPathName(name, length, baseFolder, mServerNew.pFilename);
      else if (lstrlen(mServer.pFilename) > 0)
        MakeFullPathName(name, length, baseFolder, mServer.pFilename);
    }

    if (lstrlen(name) == 0)
      MakeFullPathName(name, length, baseFolder, mLocal.pFilename);

    if (realPath != NULL)
      delete[] realPath;

    if ((lstrlen(name) == 0) && !(flag & AS_NAME_ONLY))
      GetActualPathName(name, length, FALSE);
  }
}

void KSyncItem::GetSyncResultName(LPTSTR name, int length, int flag)
{
#ifdef SYNC_ALL_DRIVE
  GetSyncResultRName(name, length, flag);
#else
  TCHAR rpathname[KMAX_LONGPATH];
  GetSyncResultRName(rpathname, KMAX_LONGPATH, flag);

  LPCTSTR rootPath = NULL;
  if (mParent != NULL)
  {
    rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
    MakeFullPathName(name, length, rootPath, rpathname);
  }
  else
  {
    rootPath = ((KSyncFolderItem *)this)->GetRootPath();
    StringCchCopy(name, length, rootPath);
  }
#endif
}
#endif

BOOL KSyncItem::IsValidFilename(LPCTSTR filename)
{
  KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
  if (root != NULL)
    return !root->IsExcludeFileMask(filename);
  return TRUE;
}

int KSyncItem::SetLocalRename(LPCTSTR baseFolder, LPCTSTR str)
{
  int r = R_FAIL_RENAME_FAILURE;
  KSyncFolderItem *orgParent = (KSyncFolderItem *)mParent;

  LPTSTR relativeName = AllocSyncResultName(AS_LOCAL);
  int len = lstrlen(relativeName) + 80 + lstrlen(str);
  LPTSTR msg = new TCHAR[len];
  StringCchCopy(msg, len, _T("target=["));
  StringCchCat(msg, len, relativeName);
  StringCchCat(msg, len, _T("] "));

  KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();

  if ((lstrlen(str) > 0) && IsValidFilename(str))
  {
    // check if folder changed
    if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
    {
      LPTSTR prevPath = AllocString(GetPath());
      TCHAR changedpath[KMAX_PATH];
      SplitPathName(mpLocalRenamedAs, changedpath, NULL);

      if (CompareRelativePath(prevPath, changedpath) != 0)
      {
        rootFolder->MoveSyncItem(this, changedpath, FALSE);
        // OnPathChanged ??
      }
      if (prevPath != NULL)
        delete[] prevPath;
    }

    if ((r = IsDuplicateFilename(str, TRUE)) > R_SUCCESS)
    {
      StringCchCat(msg, len, _T("Fail:Same file exist"));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      delete[] msg;
      return r;
    }

    KSyncItem *nitem = DuplicateItem();

    if ((orgParent != mParent) && (nitem != NULL))
    {
      ((KSyncFolderItem *)mParent)->RemoveItem(nitem);
    }

    StringCchCat(msg, len, _T("Rename as=["));
    StringCchCat(msg, len, str);
    StringCchCat(msg, len, _T("] "));

    if (renameLocal(baseFolder, str, TRUE))
    {
      if ((mConflictResult & FILE_SYNC_RENAMED) ||
        (mConflictResult & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED) ||
        // 동기화 오류중 서버 파일이 없는 경우에도 복제하지 않음.(###)
        !IsSyncFileExist(FALSE))
      {
        // do not make clone, only rename itself
        delete nitem;
        nitem = NULL;
      }

      if(nitem != NULL)
      {
        ((KSyncFolderItem *)orgParent)->AppendChild(this, nitem);
        nitem->SetParent(orgParent);
        nitem->SetEnable(TRUE);
      }

      CloneLocalRename(rootFolder, nitem);

      StoreHistory(SYNC_H_RENAME_LOCAL_ON_CONFLICT, rootFolder->GetRootPath(), mpRelativePath, 0, NULL, relativeName);

      StringCchCat(msg, len, _T(": success"));
      r = R_SUCCESS;
    }
    else
    {
      delete nitem;
      nitem = NULL;
      StringCchCat(msg, len, _T(": fail"));
      r = R_FAIL_RENAME_FAILURE;
    }
  }
  delete[] relativeName;

  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  delete[] msg;
  return r;
}

int KSyncItem::SetServerRename(LPCTSTR baseFolder, LPCTSTR str)
{
  int r = R_FAIL_RENAME_FAILURE;
  if ((lstrlen(str) > 0) && IsValidFilename(str))
  {
    KSyncFolderItem *orgParent = (KSyncFolderItem *)mParent;
    KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    LPTSTR relativeName = AllocSyncResultName(0);

    // check if folder changed
    if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
    {
      /*
      TCHAR fullname[KMAX_PATH];
      MakeFullPathName(fullname, KMAX_PATH, rootFolder->GetBaseFolder(), mpServerRenamedAs);
      TCHAR changedpath[KMAX_PATH];
      SplitPathName(fullname, changedpath, NULL);
      if (StrCmpI(mLocal.FullPath, changedpath) != 0)
      {
        TCHAR movePath[KMAX_PATH];
        SplitPathName(mpServerRenamedAs, movePath, NULL);
        rootFolder->MoveSyncItem(this, movePath);
        StringCchCopy(mLocal.FullPath, KMAX_PATH, changedpath);
      }
      */

      TCHAR changedpath[KMAX_PATH];
      SplitPathName(mpServerRenamedAs, changedpath, NULL);
      LPTSTR prevPath = AllocString(GetPath());
      if (CompareRelativePath(prevPath, changedpath) != 0)
      {
        rootFolder->MoveSyncItem(this, changedpath, FALSE);
        // StringCchCopy(mLocal.FullPath, KMAX_PATH, changedpath);
      }
      delete[] prevPath;
      delete[] mpServerRenamedAs;
      mpServerRenamedAs = NULL;
    }

    if ((r = IsDuplicateFilename(str, FALSE)) > R_SUCCESS)
      return r;

    KSyncItem *nitem = DuplicateItem();

    if ((orgParent != mParent) && (nitem != NULL))
    {
      ((KSyncFolderItem *)mParent)->RemoveItem(nitem);
    }

    if (renameServer(baseFolder, str, TRUE))
    {
      if ((mConflictResult & FILE_SYNC_RENAMED) ||
        (mConflictResult & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED) ||
        // 동기화 오류중 로컬 파일이 없는 경우에도 복제하지 않음.(###)
        !(mLocalState & ITEM_STATE_EXISTING) ||
        !nitem->IsSyncFileExist(TRUE))
      {
        // do not make clone, only rename itself
        delete nitem;
        nitem = NULL;
      }

      if (nitem != NULL)
      {
        ((KSyncFolderItem *)orgParent)->AppendChild(this, nitem);
        nitem->SetParent(orgParent);
        nitem->SetEnable(TRUE);
      }

      CloneServerRename(rootFolder, nitem);

      StoreHistory(SYNC_H_RENAME_SERVER_ON_CONFLICT, rootFolder->GetRootPath(), mpRelativePath, 0, NULL, relativeName);

      r = R_SUCCESS;
    }
    else
    {
      delete nitem;
      nitem = NULL;
      r = R_FAIL_RENAME_FAILURE;
    }

    delete[] relativeName;
  }
  return r;
}

int KSyncItem::RenameSeparate(KSyncRootFolderItem* root)
{
  // check if able to separate
  LPTSTR localPathName = NULL;
  if (IsSyncFileExist(TRUE))
    localPathName = AllocSyncResultName(AS_LOCAL);
  LPTSTR serverPathName = NULL;
  if (IsSyncFileExist(FALSE))
    serverPathName = AllocSyncResultName(0);

  if (CompareRelativePath(localPathName, serverPathName) == 0)
  {
    if (localPathName != NULL)
      delete[] localPathName;
    if (serverPathName != NULL)
      delete[] serverPathName;
    return R_FAIL_SEPARATE_FAILURE;
  }
  KSyncRootStorage *storage = root->GetStorage();

  storage->StoreMetafile(this, 0, METAFILE_DELETE);

  KSyncItem *svitem = NULL;
  if ((localPathName != NULL) && (serverPathName != NULL))
  {
    svitem = DuplicateItem();

    ((KSyncFolderItem *)mParent)->AddChild(svitem);
    svitem->SetParent(mParent);
    svitem->SetEnable(TRUE);

    CloneSeparate(root, svitem);

    StoreHistory(SYNC_H_REGIST_AS_NEW, root->GetRootPath(), svitem->mpRelativePath, 0, NULL, mpRelativePath);

#if 0
    ((KSyncFolderItem *)mParent)->AddChild(svitem);
    svitem->SetParent(mParent);
    svitem->SetEnable(TRUE);

    // check if renamed/moved on server
    if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
    {
      svitem->ChangeRelativePathName(root, mpServerRenamedAs, TRUE);
    }

    CopyServerInfo(svitem->mServer, svitem->mServerNew);

    svitem->setServerState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED); // ITEM_STATE_WAS_EXIST
    svitem->setLocalState(0);
    svitem->mConflictResult = FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED;
    svitem->mResult = FILE_NEED_DOWNLOAD;
    svitem->clearLocalSideInfo();
    svitem->clearRenameInfo();

    storage->StoreMetafile(svitem, META_MASK_ALL, 0);
#endif
  }
  else
  {
    // one-side is deleted, then clear deleted side
    if (serverPathName != NULL) // local is deleted
    {
      if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
        ChangeRelativePathName(root, mpServerRenamedAs, TRUE);

      CopyServerInfo(mServer, mServerNew);
      setServerState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED); // ITEM_STATE_WAS_EXIST
      setLocalState(0);
      mConflictResult = FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED;
      mResult = FILE_NEED_DOWNLOAD;
      clearLocalSideInfo();
      clearRenameInfo();

      storage->StoreMetafile(this, META_MASK_ALL, 0);
    }

    if (localPathName != NULL)
    {
      // check if renamed/moved on local
      if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
        ChangeRelativePathName(root, mpLocalRenamedAs, TRUE);

      setLocalState(ITEM_STATE_NEWLY_ADDED|ITEM_STATE_WAS_EXIST|ITEM_STATE_CONFLICT_RENAMED);
      setServerState(0);
      mConflictResult = FILE_NEED_UPLOAD|FILE_SYNC_RENAMED;
      mResult = FILE_NEED_UPLOAD;
      clearServerSideInfo();
      clearRenameInfo();
      storage->StoreMetafile(this, META_MASK_ALL, 0);

      delete[] localPathName;
    }

  }

  if (serverPathName != NULL)
    delete[] serverPathName;

  return R_SUCCESS;
}

int KSyncItem::IsDuplicateFilename(LPCTSTR str, BOOL onLocal)
{
#ifdef USE_SYNC_ENGINE
  TCHAR filepathname[KMAX_PATH];
  MakeFullPathName(filepathname, KMAX_PATH, GetPath(), str);
  if (IsFileExist(filepathname))
    return R_FAIL_SAME_FILE_EXIST_LOCAL;

  if (mParent == NULL)
    return R_FAIL_CANNOT_RENAME_ROOT;
  
  if (onLocal)
  {
    // check driver permission
    int perm = (gpCoreEngine->IsSecurityDisk(filepathname)) ? FLAG_WRITABLE_SECURITY_DISK : FLAG_WRITABLE_NORMAL_DISK;
    if ((gpCoreEngine->mUserFlag & perm) == 0)
      return R_FAIL_NO_PERMISSION_TO_RENAME;

    MakeFullPathName(filepathname, KMAX_PATH, GetPath(), mLocal.pFilename);
    if (IsFileExist(filepathname) && !IsFileWritable(filepathname))
      return R_FAIL_NO_PERMISSION_TO_RENAME;
  }
  else
  {
    // check driver permission
    if ((gpCoreEngine->mUserFlag & FLAG_WRITABLE_ECM_DISK) == 0)
      return R_FAIL_NO_PERMISSION_TO_RENAME;

    if (lstrlen(mParent->mServerOID) <= 0)
      return(IsFolder() ?  R_FAIL_CANNOT_RENAME_FOLDER : R_FAIL_CANNOT_RENAME_FILE);

    // check permission
    if (!CheckPermissionFolderRename(mParent->GetServerPermission(), GetServerPermission()))
      return R_FAIL_NO_PERMISSION_TO_RENAME;
  }

  // 폴더인 경우에도 처리되는지 확인 필요??
  if (gpCoreEngine->IsFileExistServer(mParent->mServerOID, mServerOID, str))
    return R_FAIL_SAME_FILE_EXIST_SERVER;

#endif
  return R_SUCCESS;
}

int KSyncItem::CompareSyncItem(KSyncItem* a, KSyncItem* b, int sortFlag)
{
  int column = (sortFlag & ITEM_SORT_COLUMN);
  int r = 0;

  if (a->IsFolder() != b->IsFolder())
  {
    if (a->IsFolder() > b->IsFolder())
      r = 1;
    else
      r = -1;
  }
  else
  {
    if ((column == 0) || (column == 3)) // local name
    {
      int name_flag = AS_NAME_ONLY | ((column == 0) ? AS_LOCAL : 0);
      LPTSTR aname = a->AllocSyncResultName(name_flag);
      LPTSTR bname = b->AllocSyncResultName(name_flag);

      r = StrCmpI(aname, bname);
      delete[] aname;
      delete[] bname;
    }
    else if (column == 1)
    {
      r = CompareSystemTime(a->mLocal.FileTime, b->mLocal.FileTime);
    }
    else if (column == 2)
    {
      if (a->mLocal.FileSize > b->mLocal.FileSize)
        r = 1;
      else if (a->mLocal.FileSize < b->mLocal.FileSize)
        r = -1;
    }
    else if (column == 4)
    {
      TCHAR aname[KMAX_FILENAME];
      TCHAR bname[KMAX_FILENAME];
      gpCoreEngine->GetUserName(a->GetServerUser(), aname);
      gpCoreEngine->GetUserName(b->GetServerUser(), bname);

      r = StrCmp(aname, bname);
    }
    else if (column == 5)
    {
      r = CompareSystemTime(a->mServer.FileTime, b->mServer.FileTime);
    }
    else if (column == 6)
    {
      if (a->mServer.FileSize > b->mServer.FileSize)
        r = 1;
      else if (a->mServer.FileSize < b->mServer.FileSize)
        r = -1;
    }
  }
  if (sortFlag & ITEM_SORT_ASCENT)
    r = -r;
  return r;
}

BOOL KSyncItem::AddConflictResult(int flag)
{
  if ((mConflictResult & flag) != flag)
  {
    mConflictResult |= flag;
    StoreMetafile(NULL, META_MASK_CONFLICT, 0);
    return TRUE;
  }
  return FALSE;
}

void KSyncItem::EnableSelectedItem()
{
  SetEnable(IsSelected());
}

// set enabled which has sync-result
void KSyncItem::InitResolveState(int flag)
{
  if (flag & FORCE_CHECK_ENABLE)
    SetEnable(mResult != FILE_NOT_CHANGED);
}

void KSyncItem::SetResolveFlagMask(int a, int o, BOOL withChild)
{
  mResolveFlag &= a;
  mResolveFlag |= o;
}

int KSyncItem::GetResolveFlagArea(int a, int o, int index, int* area, int flag)
{
  if (IsVisible() && IsResolveVisible())
  {
    if ((mResolveFlag & a) == o)
    {
      if (area[0] < 0)
        area[0] = index;
      if (area[1] < index)
        area[1] = index;
    }
    return (index + 1);
  }
  return index;
}

/*
BOOL KSyncItem::ClearOverridePath()
{
  if (IsOverridePath())
  {
    mFlag &= ~SYNC_ITEM_OVERRIDED;
    if (mpOverridePath != NULL)
    {
      delete[] mpOverridePath;
      mpOverridePath = NULL;
    }
#ifdef STORE_OVERRIDE_PATH
    StoreMetafile(NULL, META_MASK_FLAG|META_MASK_OVERRIDE, 0);
#endif
    return TRUE;
  }
  return FALSE;
}
*/

BOOL KSyncItem::IsOverridePath()
{
  return ((mFlag & SYNC_ITEM_OVERRIDED) &&
    (mpOverridePath != NULL) && (lstrlen(mpOverridePath) > 0));
}

void KSyncItem::SetOverridePath(int callingDepth, KSyncRootStorage* s, LPCTSTR baseFolder)
{
  if (mpOverridePath != NULL)
  {
    delete[] mpOverridePath;
    mpOverridePath = NULL;
  }

  if (baseFolder != NULL)
  {
    if (GetDepth() == 1)
    {
      // 드라이브를 다른 폴더로 다운로드할 때엔 경로명에 하위 항목을 바로 넣는다.
      mpOverridePath = AllocString(baseFolder);
    }
    else
    {
      LPCTSTR name = GetFilename();
      mpOverridePath = AllocPathName(baseFolder, name);
    }
    mFlag |= SYNC_ITEM_OVERRIDED;
  }
  else
  {
    mFlag &= ~SYNC_ITEM_OVERRIDED;
  }

#ifdef STORE_OVERRIDE_PATH
  StoreMetafile(NULL, META_MASK_FLAG|META_MASK_OVERRIDE, 0);
#endif
}

#ifdef _DEBUG

void KSyncItem::DumpSyncItems(int depth)
{
  TCHAR msg[256];
  int p = 0;

  for(p = 0; p < depth; p++)
  {
    msg[p] = ' ';
  }

  StringCchPrintf(&msg[p], 256-p, _T("%s(0x%x,0x%x)"), mpRelativePath, mLocalState, mServerState);
  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    StringCchCat(&msg[p], 256-p, _T("(renamed as "));
    StringCchCat(&msg[p], 256-p, mpLocalRenamedAs);
    StringCchCat(&msg[p], 256-p, _T(")"));
  }
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
}
#endif

