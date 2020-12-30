#include "StdAfx.h"
#include <tchar.h>
#include "KSyncFolderItem.h"
#include "KSyncPolicy.h"
#include "KSyncCoreBase.h"

#ifdef USE_SYNC_ENGINE
#include "KSyncCoreEngine.h"
#include "KSyncRootStorageFile.h"
#endif

#include "KConstant.h"
#include "KSettings.h"

#include "Platform/Platform.h"

// DestinyLocalSyncLib
#ifdef USE_SYNC_ENGINE
#include "DestinyLocalSyncLib/common/DestinyLocalSyncIFShared.h"
#endif

#include <strsafe.h>
#include <Shlwapi.h>

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

KSyncFolderItem::KSyncFolderItem(KSyncItem *p)
  : KSyncItem(p)
{
#ifdef USE_BASE_FOLDER
  mBaseFolder = NULL;
#endif
  mLocalScanHandle = NULL;
  mChildItemIndex = 0;
  mResolveFlag = 0;
  mpDummyLocalPath = NULL;
  mpDummyServerPath = NULL;
}

KSyncFolderItem::KSyncFolderItem(KSyncItem *p, LPCTSTR filename)
  : KSyncItem(p, filename)
{
#ifdef USE_BASE_FOLDER
  if ((p != NULL) && (p->IsFolder() < 2))
  {
    LPCTSTR pBase = ((KSyncFolderItem*)p)->GetBaseFolder();
    int len = lstrlen(pBase) + lstrlen(filename) + 2;
    mBaseFolder = new TCHAR[len];
    MakeFullPathName(mBaseFolder, len, pBase, filename);
  }
  else
  {
    mBaseFolder = AllocString(filename);
  }
#endif

#ifdef _DEBUG
  if ((StrChr(filename, '\\') != NULL) && !IsAbsolutePathName(filename))
    StoreLogHistory(_T(__FUNCTION__), _T("filename has path, change constructor to KSyncFolderItem(p, path, name)"), SYNC_MSG_LOG);
#endif

  mLocalScanHandle = NULL;
  mChildItemIndex = 0;
  mResolveFlag = 0;
  mpDummyLocalPath = NULL;
  mpDummyServerPath = NULL;
}

KSyncFolderItem::KSyncFolderItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename)
  : KSyncItem(p, pathname, filename)
{
#ifdef USE_BASE_FOLDER
  mBaseFolder = AllocMakeFullPathName(pathname, filename);
#endif
  mLocalScanHandle = NULL;
  mChildItemIndex = 0;
  mResolveFlag = 0;
  mpDummyLocalPath = NULL;
  mpDummyServerPath = NULL;
}

KSyncFolderItem::KSyncFolderItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid)
  : KSyncItem(p, pathname, filename, server_oid, server_file_oid, server_storage_oid)
{
#ifdef USE_BASE_FOLDER
  mBaseFolder = AllocMakeFullPathName(pathname, filename);
#endif
  mLocalScanHandle = NULL;
  mChildItemIndex = 0;
  mResolveFlag = 0;
  mpDummyLocalPath = NULL;
  mpDummyServerPath = NULL;
}

KSyncFolderItem::~KSyncFolderItem(void)
{
  FreeChilds();
#ifdef USE_BASE_FOLDER
  if (mBaseFolder != NULL)
  {
    delete[] mBaseFolder;
    mBaseFolder = NULL;
  }
#endif
  if (mLocalScanHandle != NULL)
  {
    FindClose(mLocalScanHandle);
    mLocalScanHandle = NULL;
  }
  if (mpDummyLocalPath != NULL)
  {
    delete[] mpDummyLocalPath;
    mpDummyLocalPath = NULL;
  }
  if (mpDummyServerPath != NULL)
  {
    delete[] mpDummyServerPath;
    mpDummyServerPath = NULL;
  }
}

KSyncFolderItem &KSyncFolderItem::operator=(KSyncFolderItem &item)
{
  *((KSyncItem *)this) = (KSyncItem &)item;
#ifdef USE_BASE_FOLDER
  mBaseFolder = AllocString(item.mBaseFolder);
#endif
  mResolveFlag = item.mResolveFlag;
  mpDummyLocalPath = AllocString(item.mpDummyLocalPath);
  mpDummyServerPath = AllocString(item.mpDummyServerPath);
  return *this;
}

KSyncItem *KSyncFolderItem::DuplicateItem()
{
  KSyncFolderItem *nitem = new KSyncFolderItem(mParent);
  *nitem = *this;
  return nitem;
}

void KSyncFolderItem::SetEnableRecurse(BOOL e)
{
  KSyncItem::SetEnableRecurse(e);

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = (KSyncItem *)*it;
    item->SetEnableRecurse(e);
    ++it;
  }
}

BOOL KSyncFolderItem::GetEnableChild()
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = (KSyncItem *)*it;
    if (item->GetEnable())
      return TRUE;
    if (item->IsFolder() && item->GetEnableChild())
      return TRUE;
    ++it;
  }
  return FALSE;
}

BOOL KSyncFolderItem::IsExcludedSyncRecur()
{
  if (KSyncItem::IsExcludedSyncRecur())
    return TRUE;

  if (mParent != NULL)
    return mParent->IsExcludedSyncRecur();
  return FALSE;
}

void KSyncFolderItem::SetConflictResultAction(int r, int flag)
{
  KSyncItem::SetConflictResultAction(r, flag);

  if (flag & WITH_RECURSE)
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem *item = (KSyncItem *)*it;
      item->SetConflictResultAction(r, flag);
      ++it;
    }
  }
}

BOOL KSyncFolderItem::IsFolderFullPathChanged()
{
  if ( ((mServer.pFullPathIndex == NULL) || (lstrlen(mServer.pFullPathIndex) == 0)) &&
    ((mServerNew.pFullPathIndex != NULL) && (lstrlen(mServerNew.pFullPathIndex) > 0)) )
  {
      return FALSE; // this is newly added folder
  }
  return ((mServer.pFullPathIndex != NULL) && (lstrlen(mServer.pFullPathIndex) > 0) &&
    (mServerNew.pFullPathIndex != NULL) && (lstrlen(mServerNew.pFullPathIndex) > 0)
    && (StrCmpI(mServer.pFullPathIndex, mServerNew.pFullPathIndex) != 0));
}

void KSyncFolderItem::SetScanServerFolderState(int s, int flag)
{
  if (!(flag & EXCLUDE_MOVED) || !isServerMovedAs())
    KSyncItem::SetScanServerFolderState(s, flag);

  if (flag & WITH_CHILD)
  {
    if (flag & WITH_RECURSE)
    {
      KSyncItemIterator it = mChildList.begin();
      while(it != mChildList.end())
      {
        KSyncItem *item = (KSyncItem *)*it;
        item->SetScanServerFolderState(s, flag|WITH_ITSELF);
        ++it;
      }
    }
    else
    {
      KSyncItemIterator it = mChildList.begin();
      while(it != mChildList.end())
      {
        KSyncItem *item = (KSyncItem *)*it;

        if (!(flag & EXCLUDE_MOVED) || !item->isServerMovedAs())
        {
          item->SetScanServerFolderState(s, WITH_ITSELF);
        }
        ++it;
      }
    }
  }
}

void KSyncFolderItem::SetServerStateByScanState()
{
  KSyncItem::SetServerStateByScanState();

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = (KSyncItem *)*it;
    item->SetServerStateByScanState();
    ++it;
  }
}

// relative path + name
void KSyncFolderItem::SetRelativePathName(LPCTSTR pathname)
{
  KSyncItem::SetRelativePathName(pathname);
#ifdef USE_BASE_FOLDER
  if (mBaseFolder != NULL)
    delete[] mBaseFolder;
  mBaseFolder = AllocString(pathname);
#endif
}

void KSyncFolderItem::ClearRelativePathName(int flag)
{
  KSyncItem::ClearRelativePathName(flag);

  if (flag & SET_WITH_RECURSIVE)
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem *item = (KSyncItem *)*it;
      item->ClearRelativePathName(flag);
      ++it;
    }
  }
}

void KSyncFolderItem::UpdateRelativePath()
{
#ifdef USE_BASE_FOLDER
  KSyncItem::SetRelativePathName(mBaseFolder);
#endif
}

void KSyncFolderItem::SetLocalPath(LPCTSTR baseFolder, LPCTSTR rootFolder, int depth)
{
  /*
  if (depth > 0)
    KSyncItem::SetLocalPath(path, baseFolder, depth);

  // TODO : change as relative path
  MakeFullPathName(mBaseFolder, KMAX_PATH, path, mLocal.Filename);
  */

  KSyncItem::SetLocalPath(baseFolder, rootFolder, depth);
#ifdef USE_BASE_FOLDER
  mBaseFolder = AllocString(mpRelativePath);
#endif
}

void KSyncFolderItem::OnPathChanged()
{
  KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)GetRootFolderItem();

  // 제외 폴더인 경우에는 루트 폴더가 없을 수 있음.
  if ((rootFolder == NULL) || (rootFolder->IsFolder() < 2))
    return;

  /*
  int rootlen = lstrlen(rootFolder->GetBaseFolder()) + 1;
  TCHAR lastRelativePath[KMAX_PATH];
  StringCchCopy(lastRelativePath, KMAX_PATH, &mBaseFolder[rootlen]);
  MakeFullPathName(mBaseFolder, KMAX_PATH, GetPath(), mLocal.Filename);
  */

#ifdef USE_BASE_FOLDER
  LPTSTR lastBaseFolder = mBaseFolder;
  mBaseFolder = AllocTreePathName();

  if ((lastBaseFolder != NULL) && (StrCmpI(mBaseFolder, mBaseFolder) != 0))
    rootFolder->UpdateMetaFolderNameMapping(lastBaseFolder, mBaseFolder, FALSE);

  if (lastBaseFolder != NULL)
    delete[] lastBaseFolder;
#else
  LPTSTR lastBaseFolder = mpRelativePath;
  mpRelativePath = AllocTreePathName();

  if ((lastBaseFolder != NULL) && (StrCmpI(lastBaseFolder, mpRelativePath) != 0))
    rootFolder->UpdateMetaFolderNameMapping(lastBaseFolder, mpRelativePath, FALSE);

  if (lastBaseFolder != NULL)
    delete[] lastBaseFolder;
#endif
}

void KSyncFolderItem::OnPathChanged(LPCTSTR folder, LPCTSTR rootFolder, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow)
{
  KSyncItem::OnPathChanged(folder, rootFolder, depth, prev, changed, refreshNow);
#ifdef USE_BASE_FOLDER
  if (mBaseFolder != NULL)
    delete[] mBaseFolder;
  mBaseFolder = AllocTreePathName();
#endif
}

#if 0 //deprecated
void KSyncFolderItem::GetLocalPath(LPTSTR path)
{
  StringCchCopy(path, KMAX_PATH, GetBaseFolder());
}
#endif

void KSyncFolderItem::UpdateChildItemPath(KSyncRootFolderItem *root, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow)
{
  KSyncRootStorage* s = NULL;
  if (depth == 1)
  {
    // update path of old-pathname from rename table 
    // rename 테이블에서 '이전경로+이전파일명 -> 새경로+새파일명'  -> '새경로+이전파일명 -> 새경로+새파일명' 이렇게 업데이트
    // 파일의 경로는 앞의 UpdateChildItemPath에서 이미 업데이트된 상태라서 여기서 변경하지 않으면 파일의 rename 처리 안됨.
    // KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    s = root->GetStorage();
    s->BeginTransaction();
    StoreLogHistory(_T(__FUNCTION__), _T("BeginTransaction"), SYNC_MSG_LOG);
  }

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = (KSyncItem *)*it;

    item->OnPathChanged(GetBaseFolder(), root->GetRootPath(), depth, prev, changed, refreshNow);

    if (item->IsFolder())
      ((KSyncFolderItem *)item)->UpdateChildItemPath(root, depth+1, prev, changed, refreshNow);

    ++it;
  }

  if (depth == 1)
  {
    // update path of old-pathname from rename table 
    // rename 테이블에서 '이전경로+이전파일명 -> 새경로+새파일명'  -> '새경로+이전파일명 -> 새경로+새파일명' 이렇게 업데이트
    // 파일의 경로는 앞의 UpdateChildItemPath에서 이미 업데이트된 상태라서 여기서 변경하지 않으면 파일의 rename 처리 안됨.
    s->UpdateRenameParentFolder(prev, changed);
    s->EndTransaction(TRUE);
    StoreLogHistory(_T(__FUNCTION__), _T("EndTransaction"), SYNC_MSG_LOG);

    // update overlay icon
#ifdef _NEED_ICON_UPDATE // dont need update icon
    LPTSTR fullPath = AllocAbsolutePathName(rootFolder, GetBaseFolder());
    gpCoreEngine->ShellNotifyIconChangeOverIcon(fullPath, TRUE);
    delete[] fullPath;
#endif
  }
}

int KSyncFolderItem::UpdateServerRenamedAs(LPCTSTR orgRelativePath, LPCTSTR changedRelativePath)
{
  int count = KSyncItem::UpdateServerRenamedAs(orgRelativePath, changedRelativePath);

  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    KSyncItem* item = *it;
    count += item->UpdateServerRenamedAs(orgRelativePath, changedRelativePath);
    ++it;
  }
  return count;
}

LPCTSTR KSyncFolderItem::GetBaseFolderName()
{
  LPCTSTR p = GetBaseFolder();
  return GetDisplayPathName(p);
}

void KSyncFolderItem::AddChild(KSyncItem *item)
{
  mChildList.push_back(item);
}

void KSyncFolderItem::AppendChild(KSyncItem *after, KSyncItem *item)
{
  if (after != NULL)
  {
    KSyncItemIterator it = mChildList.begin();
    while (it != mChildList.end())
    {
      if ((*it) == after)
      {
        ++it;
        mChildList.insert(it, item);
        return;
      }
      ++it;
    }
  }
  mChildList.push_back(item);
}

KSyncItem *KSyncFolderItem::GetNextChild(int phase, int type, BOOL parentFirst)
{
  if(parentFirst)
  {
    if ((mPhase < phase) && IsMatchType(type))
    {
      if (IsNeedSyncPhase(phase))
        return this;
      else
        SetPhase(phase);
    }
  }

  // if it has error, do not process child objects
  if (mServerState & (ITEM_STATE_NO_PERMISSION|ITEM_STATE_ERROR))
    return NULL;

  // scan child
  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    KSyncItem *child = (KSyncFolderItem *)*it;
    if (child->IsFolder())
    {
      KSyncFolderItem *subfolder = (KSyncFolderItem *)child;
      if ((subfolder->mPhase < PHASE_END_OF_JOB) && !IsExcludedSync())
      {
        KSyncItem *n = subfolder->GetNextChild(phase, type, parentFirst);
        if (n != NULL)
          return n;
      }
    }
    else
    {
      if ((child->mPhase < phase) && child->IsMatchType(type))
      {
        if (child->IsNeedSyncPhase(phase))
          return child;
        else
          child->SetPhase(phase);
      }
    }
    ++it;
  }

  // this
  if(!parentFirst)
  {
    if ((mPhase < phase) && IsMatchType(type))
    {
      if (IsNeedSyncPhase(phase))
        return this;
      else
        SetPhase(phase);
    }
  }

  return NULL;
}

/*
KSyncItem *KSyncFolderItem::GetNextPriorityChild(int phase, int type)
{
  // matched type & not be deleted
  if ((mPhase < phase) && IsMatchType(type))
    return this;

  // if it has error, do not process child objects
  if (mServerState & (ITEM_STATE_NO_PERMISSION|ITEM_STATE_ERROR))
    return NULL;

  // scan child
  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    if (((*it)->mPhase < phase) && (*it)->IsMatchType(type))
    {
        return *it;
    }
    if ((*it)->IsFolder())
    {
      KSyncFolderItem *subfolder = (KSyncFolderItem *)*it;
      KSyncItem *n = subfolder->GetNextPriorityChild(phase, type);
      if (n != NULL)
        return n;
    }

    ++it;
  }
  return NULL;
}
*/

KSyncItem *KSyncFolderItem::FindItemExist(KSyncItem *item)
{
  if (item == this)
    return this;

  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    if ((*it) == item)
      return item;

    if ((*it)->IsFolder())
    {
      KSyncFolderItem *subfolder = (KSyncFolderItem *)*it;
      KSyncItem *n = subfolder->FindItemExist(item);
      if (n != NULL)
        return n;
    }
    ++it;
  }
  return NULL;
}

int KSyncFolderItem::scanSubObjects(KSyncRootStorage* storage, LPCTSTR metaFolder, KSyncPolicy *p, LPCTSTR rootFolder, BOOL &done, int flag, KSyncFolderItem* rootP)
{
#ifdef USE_SYNC_ENGINE
	int count = 0;
  int skip_count = 0;
	WIN32_FIND_DATA fd;

  DWORD meta_mask = /*(flag & SCAN_CLEAR_STATE) ? META_CLEAR_STATE : */ META_MASK_ALL;
  // need always state load

  int max_item_for_iterration = (flag & KSyncRootStorage::WITH_PAGING) ? 100 : 0x0FFFFFFF;
  
  done = FALSE;
  while (count < max_item_for_iterration)
  {
	  memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	  fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

    if (mLocalScanHandle == NULL)
    {
      TCHAR filemask[KMAX_PATH];

	    MakeFullPathName(filemask, KMAX_PATH, metaFolder, L_ASTERISK_STR);
      StringCchCat(filemask, KMAX_PATH, INFO_EXTENSION);

      mLocalScanHandle = FindFirstFile(filemask, &fd);
      if (mLocalScanHandle == INVALID_HANDLE_VALUE)
        mLocalScanHandle = NULL;
      if (mLocalScanHandle == NULL)
      {
        done = TRUE;
        break;
      }
    }
    else
    {
      if (!FindNextFile(mLocalScanHandle, &fd))
      {
        FindClose(mLocalScanHandle);
        mLocalScanHandle = NULL;
        done = TRUE;
        break;
      }
    }

		if ( ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
      (lstrlen(fd.cFileName) > 0) && (StrCmp(fd.cFileName, ROOT_METAFILENAME) != 0) )
    {
      TCHAR filename[KMAX_PATH];
      TCHAR metafilename[KMAX_LONGPATH];
      char buff[KMAX_PATH];

      // metafilename
      MakeFullPathName(metafilename, KMAX_LONGPATH, metaFolder, fd.cFileName);
      BOOL isFolder;

#ifdef USE_SHORT_METAFILE_NAME_LENGTH
      // read relative name from metafile
      filename[0] = '\0';
      if (ReadSettingKeyValue(metafilename, META_RELATIVE_FILENAME, buff, KMAX_PATH) &&
        (strlen(buff) > 0))
        ToUnicodeString(filename, KMAX_PATH, buff, (int)strlen(buff));

      if (lstrlen(filename) == 0)
        continue;

      TCHAR fullname[KMAX_LONGPATH];
      MakeFullPathName(fullname, KMAX_LONGPATH, rootFolder, filename);

      // META_RELATIVE_FILENAME can be old-path when parent folder is moved
      // so replace relative path to this->mBaseFolder
      SplitPathName(fullname, NULL, filename);
      MakeFullPathName(fullname, KMAX_LONGPATH, GetBaseFolder(), filename);
#else
      // 
      TCHAR fullname[KMAX_LONGPATH];
      KSyncRootStorageFile::GetOriginalFileNameFromMetafile(fullname, KMAX_LONGPATH, rootFolder, metafilename);
#endif

      if (ReadSettingKeyValue(metafilename, META_ITEM_TYPE, buff, 16))
      {
        isFolder = (buff[0] == '1');
      }
      else
      {
        isFolder = IsDirectoryExist(fullname);
      }

      TCHAR pathname[KMAX_PATH];
      SplitPathName(fullname, pathname, filename);

      KSyncItem *item = NULL;

      if ((rootP == NULL) || (rootP->IsPartialSyncOn(fullname, isFolder)))
      {
        // pathname should be relative-path
        TCHAR relativePathName[KMAX_PATH];
        MakeRelativePathName(relativePathName, KMAX_PATH, rootFolder, pathname);

        if (isFolder)
          item = new KSyncFolderItem(this, relativePathName, filename);
        else
          item = new KSyncItem(this, relativePathName, filename);
      }
      else
      {
        TCHAR msg[300];
        StringCchPrintf(msg, 300, _T("Skip on Partial Sync : %s"), fullname);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }

#ifdef _OLD_
      // in Resolve-Conflict mode, compare metafile-time and last-sync-complete-time
      if (!isFolder && (flag & SCAN_CHANGED_FILE_ONLY))
      {
        SYSTEMTIME lastModifiedTime;
        FileTimeToLocalTime(fd.ftLastWriteTime, lastModifiedTime);
        if (!((KSyncRootFolderItem *) rootP)->IsChangedAfterLastSync(lastModifiedTime))
        {
          skip_count++;
          delete item;
          item = NULL;
        }
      }
#endif

      if (item != NULL)
      {
        item->SetMetafilename(metafilename);
        item->LoadMetafile(storage, meta_mask);

#if 0
        LPTSTR actualfilename;
        if (mParent != NULL)
        {
          // MakeFullPathName(actualfilename, KMAX_LONGPATH, rootFolder, item->mpRelativePath); // old local full name
          actualfilename = AllocPathName(rootFolder, item->mpRelativePath);
        }
        else
        {
          // item->GetFullPathName(actualfilename, KMAX_LONGPATH);
          actualfilename = item->AllocFullPathName();
        }
#endif

        LPTSTR lastLocalRename = NULL;

        if (item->mpLocalRenamedAs != NULL)
          lastLocalRename = AllocString(item->mpLocalRenamedAs);

        item->CheckLocalRenamedAsByOld();
        if ((item->mpLocalRenamedAs != NULL) && (lstrlen(lastLocalRename) > 0))
        {
          // update local rename info
          if (StrCmp(lastLocalRename, item->mpLocalRenamedAs) != 0)
            storage->StoreMetafile(item, META_MASK_RENAMED, 0);
        }

        if (lastLocalRename != NULL)
          delete[] lastLocalRename;

        if (flag & SCAN_CLEAR_LOCAL_STATE)
        {
          // 양쪽 추가된 경우, 다시 시작하면 양쪽 수정됨으로 오인됨.
          // item->maskLocalState(ITEM_STATE_WAS_EXIST|ITEM_STATE_NOW_EXIST, ITEM_STATE_WAS_EXIST);

          item->maskLocalState(ITEM_STATE_WAS_EXIST|ITEM_STATE_NOW_EXIST|ITEM_STATE_PERSISTING, 0);
          if (IsValidSystemTime(item->mLocal.FileTime))
            item->addLocalState(ITEM_STATE_WAS_EXIST);
        }

        if (flag & SCAN_CLEAR_SERVER_STATE)
        {
          item->maskServerState(ITEM_STATE_WAS_EXIST|ITEM_STATE_NOW_EXIST|ITEM_STATE_PERSISTING, 0);
          if (IsValidSystemTime(item->mServer.FileTime))
            item->addServerState(ITEM_STATE_WAS_EXIST);
        }
        else
        {
          // in resolve-conflict, do not clear state
        }

        // initialize current state
        /* do not change state
        if (!(item->mLocalState & ITEM_STATE_NEWLY_ADDED) && IsValidSystemTime(item->mLocal.FileTime))
          item->addLocalState(ITEM_STATE_WAS_EXIST);
        if (!(item->mServerState & ITEM_STATE_NEWLY_ADDED) && IsValidSystemTime(item->mServer.FileTime))
          item->addServerState(ITEM_STATE_WAS_EXIST);
        **/

        /**
        BOOL server_exist;
        if (isFolder)
          server_exist = (lstrlen(item->mServer.FullPathIndex) > 0);
        else
          server_exist = (lstrlen(item->mServer.FileOID) > 0);
        //item->mServerState &= ~ITEM_STATE_WAS_EXIST;
        if (!(item->mServerState & ITEM_STATE_NEWLY_ADDED) && server_exist)
          item->addServerState(ITEM_STATE_WAS_EXIST);
        **/

        mChildList.push_back(item);
        count++;
      }
    }
	}

#ifdef _OLD_
  if ((flag & SCAN_CHANGED_FILE_ONLY) && (skip_count > 0))
  {
    TCHAR msg[300];
    StringCchPrintf(msg, 300, _T("Skip scanSubObjects on Resolve Sync : %d"), skip_count);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
#endif
  return count; // need continue scan
#else
return 0;
#endif
}

int KSyncFolderItem::scanSubFoldersServer(KSyncPolicy *p, KSyncRootFolderItem* rootP)
{
  int count = 0;
#ifdef USE_SYNC_ENGINE

#ifdef USE_SCAN_BY_HTTP_REQUEST
  int r = gpCoreEngine->StartScanFolder(rootP, this);
  if (r == R_SUCCESS)
    return FILE_SYNC_THREAD_WAIT;
#else
  // scan server folders
  XSYNCFOLDERLIST_ST *syncFolderList = NULL;
  ERROR_ST err;

  if (lstrlen(mServerOID) > 0)
  {
    LocalSync_InitEnv(&err);
    if (LocalSync_GetFolders(&err, mServerOID, &syncFolderList))
    {
      XSYNCFOLDER_ST *pChilds = syncFolderList->folders;
      for (int i = 0; i < syncFolderList->folderCount; i++)
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("folder OID=%s : child=%s"), mServerOID, pChilds[i].name);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

#if 0
        if (gpCoreEngine->IsExcludeFileMask(pChilds[i].name) ||
          p->IsExcludeFileMask(pChilds[i].name))
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Excluded folder on server=%s"), pChilds[i].name);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        else
#endif
        {
          KSyncItem *item = findItemByOID(pChilds[i].oID);
          if (item == NULL)
          {
            item = findChildByName(pChilds[i].name);
            if (item != NULL)
              item->UpdateServerOID(pChilds[i].oID, NULL, NULL);
          }
          else
          {
            if (item->CheckServerRenamedAs(GetBaseFolder(), pChilds[i].name))
              item->SetNewParent(this);
          }

          if(item == NULL)
          {
            TCHAR fullname[MAX_PATH];
            MakeFullPathName(fullname, MAX_PATH, GetBaseFolder(), pChilds[i].name);
            if ((rootP == NULL) || (rootP->IsPartialSyncOn(fullname, TRUE)))
            {
              item = new KSyncFolderItem(this, GetBaseFolder(), pChilds[i].name, pChilds[i].oID, NULL, NULL);
              item->LoadMetafile(rootP->GetStorage(), META_CLEAR_STATE);
              mChildList.push_back(item);
            }
            else
            {
              TCHAR msg[300];
              StringCchPrintf(msg, 300, _T("Skip on Partial Sync : %s"), fullname);
              StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
            }
          }

          if (item != NULL)
          {
            FILETIME ft;
            SYSTEMTIME fileTime;
            ft.dwLowDateTime = pChilds[i].lastModifiedAtLow;
            ft.dwHighDateTime = pChilds[i].lastModifiedAtHigh;
            FileTimeToSystemTime(&ft, &fileTime);

            item->UpdateServerNewFileInfo(0, fileTime, pChilds[i].permissions,
              pChilds[i].creatorOID, NULL, NULL, pChilds[i].folderFullPath, pChilds[i].fullPathIndex, pChilds[i].name);

            StringCchCopy(item->mServerNew.Filename, MAX_PATH, pChilds[i].name);
            item->addServerState(ITEM_STATE_NOW_EXIST);
            ++count;
          }
        }

        if ((i % 100) == 99) // yield CPU to other apps
          Sleep(10);
      }
      LocalSync_DestroyXSYNCFOLDERLIST_ST(syncFolderList);
      LocalSync_CleanEnv(&err);
    }
  }

  // remove excluded item
  /* do not remove it because it should be moved when parent renamed
  RemoveExcludedItems(FALSE);
  */
  SetPhaseExcludedItems(FALSE, PHASE_MOVE_FOLDERS+1);

#endif // USE_SCAN_BY_HTTP_REQUEST
#endif
  return R_SUCCESS;
}

#ifndef SCAN_FOLDER_FILE_AT_ONCE

int KSyncFolderItem::scanSubFoldersLocal(KSyncPolicy *p, KSyncRootFolderItem* rootP)
{
  int count = 0;

  // scan local folders
	WIN32_FIND_DATA fd;
  TCHAR filename[KMAX_PATH];
  TCHAR pathname[KMAX_PATH];

	memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

  // scan with real folder path if it or parent renamed
  if (!GetCurrentFolder(pathname) ||
    (lstrlen(pathname) >= (KMAX_PATH - 2)))
  {
    TCHAR msg[300];
    StringCchPrintf(msg, 300, _T("Long folder scan canceled=%s"), GetBaseFolder());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return 0;
  }

#ifdef SYNC_ALL_DRIVE
  if (IsSystemRoot())
  {
    // find available drives
    TCHAR fullname[4] = _T("A:\\");
    for (int i = 'C'; i <= 'Z'; i++)
    {
      fullname[0] = i;

      if (gpCoreEngine->IsTargetDrive(fullname) && !gpCoreEngine->IsPolicyExcludedPath(fullname))
      {
        if ((rootP == NULL) || (rootP->IsPartialSyncOn(fullname, TRUE)))
        {
          fullname[2] = '\0';
          KSyncItem *item = new KSyncFolderItem(this, NULL, fullname);
          fullname[2] = '\\';
          item->LoadMetafile(NULL, META_CLEAR_STATE);
          mChildList.push_back(item);
          ++count;

          item->addLocalState(ITEM_STATE_NOW_EXIST);

          SYSTEMTIME fileTime;
          SYSTEMTIME accessTime;
          if (GetFileModifiedTime(fullname, &fileTime, &accessTime))
            item->UpdateLocalNewFileInfo(0, fileTime, accessTime, mUserOid);
        }
      }
    }
  }
  else
#endif
  {
    MakeFullPathName(filename, KMAX_PATH, pathname, L_ASTERISK_STR);

    HANDLE h = FindFirstFile(filename, &fd);
    if (h != INVALID_HANDLE_VALUE)
	  {
		  int i = 0;
		  do {
			  if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
          (lstrlen(fd.cFileName) > 0))
        {
				  if ((StrCmp(fd.cFileName, L".") == 0) ||
					  (StrCmp(fd.cFileName, L"..") == 0) ||
					  (StrCmpI(fd.cFileName, meta_folder_name) == 0) ||
					  (StrCmpI(fd.cFileName, DOWNTEMP_FOLDER_NAME) == 0))
            continue;
          TCHAR fullname[MAX_PATH];
          MakeFullPathName(fullname, MAX_PATH, pathname, fd.cFileName);

#ifdef SYNC_ALL_DRIVE
          if (IsSystemDirectory(fullname))
            continue;

          if (gpCoreEngine->IsPolicyExcludedPath(fullname))
            continue;
#endif

#if 0
          if (gpCoreEngine->IsExcludeFileMask(fd.cFileName) ||
            p->IsExcludeFileMask(fd.cFileName))
          {
            TCHAR msg[256];
            StringCchPrintf(msg, 256, _T("Excluded folder on local=%s"), fd.cFileName);
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
          }
          else
#endif
          {
            // check moved or renamed first
            KSyncItem *item = findMovedRenamedOrigin(fd.cFileName);

            if (item == NULL)
              item = findChildByName(fd.cFileName);

            if (item == NULL)
            {
              if ((rootP == NULL) || (rootP->IsPartialSyncOn(fullname, TRUE)))
              {
                item = new KSyncFolderItem(this, GetBaseFolder(), fd.cFileName); // TODO::check need
                item->LoadMetafile(NULL, META_CLEAR_STATE);
                mChildList.push_back(item);
              }
              else
              {
                TCHAR msg[300];
                StringCchPrintf(msg, 300, _T("Skip on Partial Sync : %s"), fullname);
                StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
              }
            }
            if (item != NULL)
            {
              SYSTEMTIME fileTime;
              FileTimeToSystemTime(&fd.ftLastWriteTime, &fileTime);
              SYSTEMTIME accessTime;
              FileTimeToSystemTime(&fd.ftLastAccessTime, &accessTime);

              item->addLocalState(ITEM_STATE_NOW_EXIST);
              item->UpdateLocalNewFileInfo(0, fileTime, accessTime, mUserOid);

              ++count;
            }
          }
        }
        ++i;
        if ((i % 100) == 99) // yield CPU to other apps
          Sleep(10);

		  } while (FindNextFile(h, &fd));
		  FindClose(h);
	  }
  }
  SetPhaseExcludedItems(FALSE, PHASE_MOVE_FOLDERS+1);
  return count;
}
#endif // SCAN_FOLDER_FILE_AT_ONCE

int KSyncFolderItem::ClearSubFiles()
{
  int deleted = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if((*it)->IsFolder())
    {
      ++it;
    }
    else
    {
      KSyncItem *item = *it;
      it = mChildList.erase(it);
      delete item;
      ++deleted;
    }
  }
  return deleted;
}

void KSyncFolderItem::SetPhaseExcludedItems(BOOL bRecurr, int phase)
{
  BOOL has_excluded = FALSE;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
     if(item->IsExcludedSync())
    {
      item->SetPhase(phase);
      has_excluded = TRUE;
    }
     else
     {
       if(bRecurr && item->IsFolder())
       {
         ((KSyncFolderItem *)item)->SetPhaseExcludedItems(TRUE, phase);
       }
     }
    it++;
  }

  if (has_excluded)
    AddSyncFlag(SYNC_ITEM_HAS_EXCLUDED);
}

void KSyncFolderItem::RemoveExcludedItems(KSyncFolderItem* root, BOOL bRecurr)
{
  BOOL has_excluded = FALSE;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
     if(item->IsExcludedSync())
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Excluded sub-folder=%s"), item->GetFilename());
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      // 로컬측 이름 변경되었으면 여기서 업데이트한다.
      if (item->isLocalRenamedAs())
      {
        item->renameServerObjectAs(FALSE);
      }

      // check if exist. if folder does not exist, retire object.
      // 제외 폴더가 삭제되어도 retire 하지 않는다. 다시 생겨도 여전히 제외 폴더 유지되도록
      /*
      if (item->IsFolder() && !item->IsLocalFileExist()) // !IsDirectoryExist(((KSyncFolderItem *)item)->GetBaseFolder()))
      {
        // KSyncRootFolderItem *root = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
        item->StoreMetafile(((KSyncRootFolderItem *)root)->GetStorage(), 0, METAFILE_DELETE);
      }
      */
      it = mChildList.erase(it);
      delete item;

      has_excluded = TRUE;
    }
     else
     {
       if(bRecurr && item->IsFolder())
       {
         ((KSyncFolderItem *)item)->RemoveExcludedItems(root, TRUE);
       }
       it++;
     }
  }

  if (has_excluded)
    AddSyncFlag(SYNC_ITEM_HAS_EXCLUDED);
}

#ifdef USE_INSTANT_SYNC
static int IsModifiedListBelow(KVoidPtrArray& modifiedList, LPCTSTR itemPath)
{
  int item_path_length = lstrlen(itemPath);
  KVoidPtrIterator it = modifiedList.begin();
  while (it != modifiedList.end())
  {
    ModifiedItemData* p = (ModifiedItemData*)*it++;
    int modified_len = lstrlen(p->pathName);

    // parent of modified item
    if (modified_len > item_path_length)
    {
      if ((StrCmpNI(itemPath, p->pathName, item_path_length) == 0) &&
        (p->pathName[item_path_length] == '\\'))
        return 2;
    }
    else if (modified_len == item_path_length)
    {
      if (StrCmpI(itemPath, p->pathName) == 0)
        return 1;
    }
    if (modified_len < item_path_length)// && (p->modifyType == DIR_ADDED))
    {
      if ((StrCmpNI(itemPath, p->pathName, modified_len) == 0) &&
        (itemPath[modified_len] == '\\'))
        return 3;
    }
  }
  return 0;
}

void KSyncFolderItem::RemoveOutOfModifiedList(KVoidPtrArray& modifiedList)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it++;
    int r = IsModifiedListBelow(modifiedList, item->mpRelativePath);
    if ((r == 0) && item->isLocalRenamedAs())
      r = IsModifiedListBelow(modifiedList, item->mpLocalRenamedAs);

    if (r == 0)
    {
      // PHASE_END_OF_JOB으로 하면 DB에서 제거되니 PHASE_ENDING으로 한다.
      if (item->IsFolder())
        ((KSyncFolderItem*)item)->SetPhaseRecurse(PHASE_ENDING, 0);
      else
        item->SetPhase(PHASE_ENDING);

#ifdef _DEBUG
      TCHAR msg[256] = _T("Skipped : ");
      StringCchCat(msg, 256, item->mpRelativePath);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
    }

    if ((item != NULL) && item->IsFolder())
    {
      ((KSyncFolderItem*)item)->RemoveOutOfModifiedList(modifiedList);
    }
  }
}
#endif

void KSyncFolderItem::RetireChildObjects(LPCTSTR baseFolder)
{
  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)GetRootFolderItem();
  KSyncRootStorage* s = rootFolder->GetStorage();

#ifdef _DEBUG_TRANSACTION
  DWORD start = GetTickCount();
#endif

  s->BeginTransaction();
  RetireChildObjectsRecurr(s, baseFolder);
  s->EndTransaction(TRUE);

#ifdef _DEBUG_TRANSACTION
  DWORD end = GetTickCount();
  DWORD ellapsed = (start <= end) ? (end - start) : (0xFFFFFFFF - end + start);

  TCHAR msg[256];
  StringCchPrintf(msg, 256, _T("Ellapsed = %d(ms)"), (int)ellapsed);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

}

void KSyncFolderItem::RetireChildObjectsRecurr(KSyncRootStorage* s, LPCTSTR baseFolder)
{
  KSyncItemIterator it = mChildList.begin();

  while(it != mChildList.end())
  {
     if((*it)->IsFolder())
       ((KSyncFolderItem *)(*it))->RetireChildObjectsRecurr(s, baseFolder);

     (*it)->StoreMetafile(s, 0, METAFILE_DELETE);
     ++it;
  }
  // itself
  StoreMetafile(s, 0, METAFILE_DELETE);
}

void KSyncFolderItem::RemoveExcludeNamedItems(KSyncCoreBase *engine, KSyncRootFolderItem *root, int restorePhase)
{
  BOOL has_ex_item = FALSE;
  TCHAR resultName[MAX_PATH];
  int syncMethod = root->GetSyncMethod();

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it++;
    int ex = 0;

    if (root->IsExcludeFileMask(item)) // by Merge
    /*
    if (engine->IsExcludeFileMask(item->mLocal.Filename) ||
      p->IsExcludeFileMask(item))
    */
    {
      ex |= 1;
    }

    // 백업할때만
    // 복원할때 로컬파일 이름변경된 것 : 로컬 상태에 따라 별도 처리되도록 놔둔다
#ifdef USE_BACKUP_EXCLUDE_RENAME
    if (item->validLocalRenamedAs() && (syncMethod & SYNC_METHOD_BACKUP))
#else
    if (item->validLocalRenamedAs())
#endif
    {
#ifdef USE_BACKUP
      // 대상명 -> 비대상명 이름 변경된 것은 삭제 처리를 위해 남겨둔다
      if(ex & 1) // previously renamed -> changed normal
#else
      if (engine->IsExcludedPathName(item->mpLocalRenamedAs, item->IsFolder(), &root->GetPolicy())) // by Merge
		/* 
      SplitPathName(item->mpLocalRenamedAs, NULL, resultName);
      if (engine->IsExcludeFileMask(resultName) ||
        p->IsExcludeFileMask(resultName) ||
        p->IsExcludeFileMask(item->mpLocalRenamedAs))
		*/	
      {
        ex |= 2;
      }
      else if(ex & 1) // previously renamed -> changed normal
#endif
      {
        SplitPathName(item->mpLocalRenamedAs, NULL, resultName);
        item->UpdateRenameLocal(GetRootPath(), resultName);

        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Excluded out Name %s"), item->GetFilename());
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        item->mPhase = restorePhase; // change phase forcely
        ex = 0;
      }
    }
    // 복원할때만.
    // 백업시 서버 파일 이름변경된 것 : 로컬 상태에 따라 별도 처리되도록 놔둔다
#ifdef USE_BACKUP_EXCLUDE_RENAME
    if (item->isServerRenamedAs() && (syncMethod & SYNC_METHOD_RESTORE))
#else
    if (item->isServerRenamedAs())
#endif
    {
      if (engine->IsExcludedPathName(item->mpServerRenamedAs, item->IsFolder(), &root->GetPolicy())) // by Merge
		/*
      SplitPathName(item->mpServerRenamedAs, NULL, resultName);
      if (engine->IsExcludeFileMask(resultName) ||
        p->IsExcludeFileMask(resultName) ||
        p->IsExcludeFileMask(item->mpServerRenamedAs))
		*/
        ex |= 4;
    }

     if(ex)
    {
      TCHAR msg[KMAX_LONGPATH];
      if (ex & 1)
      {
        StringCchPrintf(msg, KMAX_LONGPATH, _T("Excluded by Name %s"), item->GetFilename());
      }
      else if (ex & 2)
      {
        SplitPathName(item->mpLocalRenamedAs, NULL, resultName);
        StringCchPrintf(msg, KMAX_LONGPATH, _T("Excluded by Name %s - [%s]"), item->GetFilename(), resultName);
      }
      else if (ex & 4)
      {
        SplitPathName(item->mpServerRenamedAs, NULL, resultName);
        StringCchPrintf(msg, KMAX_LONGPATH, _T("Excluded by Name %s - [%s]"), item->GetFilename(), resultName);
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      // if local-rename, change metafile
      if (ex & 2)
      {
        SplitPathName(item->mpLocalRenamedAs, NULL, resultName);
        item->UpdateRenameLocal(GetRootPath(), resultName);
      }
      else if (ex & 4)
      {
        item->StoreMetafile(NULL, META_MASK_RENAMED, 0);
      }

      // do not remove, but change phase
      item->SetPhase(PHASE_END_OF_JOB);
      has_ex_item = TRUE;
    }
  }

  if (has_ex_item)
    AddSyncFlag(SYNC_ITEM_HAS_EXCLUDED);
}

int KSyncFolderItem::DisableExcludedFileMask(KSyncRootFolderItem* root)
{
  int count = 0;

  if (IsFolder() < 2)
  {
    if (GetEnable() && root->IsExcludeFileMask(this))
    {
      SetEnable(FALSE);
      ++count;
    }
  }

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    if(item->IsFolder())
    {
      count += ((KSyncFolderItem *)item)->DisableExcludedFileMask(root);
    }
    else
    {
      if (item->GetEnable() && root->IsExcludeFileMask(item))
      {
        item->SetEnable(FALSE);
        ++count;
      }
    }
    ++it;
  }
  return count;
}

void KSyncFolderItem::SetPhaseServerNotExistFolders(int phase)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    if(item->IsFolder() && item->IsServerNotExist())
    {
      item->SetPhase(phase);
      ((KSyncFolderItem *)item)->SetPhaseServerNotExistFolders(phase);
    }
    ++it;
  }
}

/*
  서버측 위치가 이동된 폴더 항목의 phase를 바꾼다.
  : 이름만 변경된 케이스는 FullPathIndex가 변경되지 않으니 스킵.
  : 하위에 서브 폴더가 없는 항목도 FullPathIndex를 업데이트할 필요 없으니 스킵.
  : OnEndScanFolderItem 이후에 실행됨.
  : phase가 바뀐 항목은 끝 노드까지 모두 업데이트되니 recursive할 필요 없다.
*/
void KSyncFolderItem::SetPhaseForMovedFoldersInServer(int phase, int default_phase)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    item->SetPhase(default_phase);
    if(item->IsFolder())
    {
      if (item->IsServerNeedRescan())
      {
        TCHAR msg[256] = _T("Need rescan : ");
        StringCchCat(msg, 256, item->mpRelativePath);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }

      if (item->IsServerNeedRescan() || 
        (((KSyncFolderItem*)item)->IsFolderFullPathChanged() &&
        (((KSyncFolderItem*)item)->GetTotalCount(1) > 0)))
      {
        item->SetPhase(phase);
      }
      else
      {
        ((KSyncFolderItem *)item)->SetPhaseForMovedFoldersInServer(phase, default_phase);
      }
    }
    ++it;
  }
}

int KSyncFolderItem::GetCountForMoveInOutOnServer(LPCTSTR partialPath, int path_len)
{
  if (path_len < 0)
    path_len = lstrlen(partialPath);

  int count = 0;
  if (IsFolder() < 2)
    count += KSyncItem::GetCountForMoveInOutOnServer(partialPath, path_len);

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    count += item->GetCountForMoveInOutOnServer(partialPath, path_len);
    ++it;
  }
  return count;
}

#ifndef USE_SCAN_BY_HTTP_REQUEST
// return >= 1 for scanning done, 
int KSyncFolderItem::scanSubFilesServer(KSyncPolicy *policy, KSyncFolderItem* rootP, BOOL &done)
{
	int count = 0;
#ifdef USE_SYNC_ENGINE

  // scan server files
  if (lstrlen(mServerOID) > 0)
  {
    ERROR_ST err;
    XSYNCDOCUMENTLIST_ST *doc_list = NULL;
    LocalSync_InitEnv(&err);

    if (!LocalSync_GetDocuments(&err, mServerOID, NOF_DOCUMENT_PER_PAGE, mChildItemIndex + 1, &doc_list))
    {
      TCHAR msg[256] = _T("LocalSync_GetDocuments : OID=");
      StringCchCat(msg, 256, mServerOID);
      StringCchCat(msg, 256, _T(", error="));
      StringCchCat(msg, 256, err.fault_string);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      setServerState(ITEM_STATE_ERROR);
      LocalSync_CleanEnv(&err);
      done = TRUE;
      return 0;
    }
    if ((doc_list == NULL) || (doc_list->documentCount <= 0))
    {
      if (doc_list != NULL)
        LocalSync_DestroyXSYNCDOCUMENTLIST_ST(doc_list);
      done = TRUE;
      return 0;
    }

    // compare
    XSYNCDOCUMENT_ST *doc = doc_list->documents;
    for (int i = 0; i < doc_list->documentCount; i++)
    {
      // check sync exclude mask
#if 0
      if (gpCoreEngine->IsExcludeFileMask(doc->name) ||
        policy->IsExcludeFileMask(doc->name)) // check exclude mask
      {
        StringCchPrintf(msg, 256, _T("Excluded file on server=%s"), doc->name);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      else
#endif
      {
        KSyncItem *item = findItemByOID(doc->oID);

        if (item == NULL)
        {
          item = findChildByName(doc->name);
        }
        else if (item->mPhase < PHASE_END_OF_JOB)
        {
          if (item->CheckServerRenamedAs(GetBaseFolder(), doc->name))
          {
            // if item does not synced yet, clear rename
            if (!IsValidSystemTime(item->mServer.FileTime))
            {
              if (item->mpServerRenamedAs != NULL)
              {
                delete[] item->mpServerRenamedAs;
                item->mpServerRenamedAs = NULL;
              }
              item = NULL;
            }
            else
            {
              if (item->mParent != this)
                item->SetNewParent(this);
            }

            // we have to ready server full path

          }
        }

        if (item == NULL)
        {
          LPTSTR pathName = AllocPathName(GetBaseFolder(), doc->name);

          if ((rootP == NULL) || (rootP->IsPartialSyncOn(pathName, TRUE)))
          {
            item = new KSyncItem(this, NULL, doc->name, (LPTSTR)doc->oID, NULL, NULL); // , doc->fileOID, doc->storageFileID);

            mChildList.push_back(item);
            item->LoadMetafile(NULL, META_CLEAR_STATE);
          }
          delete pathName;
        }
        else
        {
          item->SetServerOID(doc->oID);
        }

        if (item != NULL)
        {
          FILETIME ft;
          SYSTEMTIME fileTime;
          ft.dwLowDateTime = doc->lastModifiedAtLow;
          ft.dwHighDateTime = doc->lastModifiedAtHigh;
          FileTimeToSystemTime(&ft, &fileTime);
          __int64 fileSize = FileSizeFromStringDoubleT(doc->fileSize);

          item->addServerState(ITEM_STATE_NOW_EXIST);

          LPCTSTR pName = doc->name;
          if (StrCmpI(item->mLocal.Filename, doc->name) == 0)
            pName = NULL;

          item->UpdateServerNewFileInfo(fileSize, fileTime, doc->permissions, 
            doc->lastModifierOID, doc->fileOID, doc->storageFileID, doc->folderFullPath, NULL, pName);
        }
      }
      doc++;
      count++;
    }
    
    // clear
    int totalCount = doc_list->totalCount;
    LocalSync_CleanEnv(&err);
    LocalSync_DestroyXSYNCDOCUMENTLIST_ST(doc_list);
    ++mChildItemIndex;
    done = (NOF_DOCUMENT_PER_PAGE * mChildItemIndex >= totalCount);
    LocalSync_CleanEnv(&err);
  }
  else
    done = TRUE;
#endif
  return count;
}
#endif // USE_SCAN_BY_HTTP_REQUEST

BOOL KSyncFolderItem::IsLocalScanSkipFile(KSyncFolderItem* root, LPCTSTR cFileName, BOOL isNewItem)
{
  if (StrCmpI(cFileName, META_DB_FILENAME) == 0)
    return TRUE;

  // 대상 -> 비대상 이름 변경 처리를 위해 신규 항목만 제외하도록 한다.
	// 백업에서 추가됨. by Merge
  if (isNewItem)
  {
    if (((KSyncRootFolderItem*)root)->IsExcludeFileMask(cFileName))
      return TRUE;
  }
  return FALSE;
}

// return number of counts scanned
int KSyncFolderItem::scanSubFilesLocal(KSyncFolderItem* rootP, int& folder_count, BOOL &done)
{
	int count = 0;
  int init_count = 0;
#ifdef USE_SYNC_ENGINE

#if defined(SCAN_FOLDER_FILE_AT_ONCE) && defined(SYNC_ALL_DRIVE)
  if (IsSystemRoot())
  {
    // find available drives
    TCHAR fullname[4] = _T("A:");
    for (int i = 'C'; i <= 'Z'; i++)
    {
      fullname[0] = i;

      if (gpCoreEngine->IsTargetDrive(fullname))
      {
        if ((rootP == NULL) || (rootP->IsPartialSyncOn(fullname, TRUE)))
        {
          KSyncItem *item = NULL;
          //fullname[2] = '\0';
          item = findChildByName(fullname);

          if (item == NULL)
          {
            item = new KSyncFolderItem(this, NULL, fullname);
            item->LoadMetafile(NULL, META_CLEAR_STATE);
            mChildList.push_back(item);
          }
          ++folder_count;
          //fullname[2] = '\\';

          item->addLocalState(ITEM_STATE_NOW_EXIST);

          SYSTEMTIME fileTime;
          SYSTEMTIME accessTime;
          if (GetFileModifiedTime(fullname, &fileTime, &accessTime))
            item->UpdateLocalNewFileInfo(0, fileTime, accessTime, mUserOid);
        }
      }
    }
    done = TRUE;
    return count;
  }
#endif // SCAN_FOLDER_FILE_AT_ONCE

	WIN32_FIND_DATA fd;
  LPTSTR baseFullPath = NULL;
  int len = GetCurrentFolder(NULL, 0);
  if (len > 0)
  {
    baseFullPath = new TCHAR[len + 1];
    GetCurrentFolder(baseFullPath, len + 1);
  }

  while ((count < NOF_LOCAL_FILES_PER_PAGE) && (init_count < 10))
  {
	  memset(&fd, 0, sizeof(WIN32_FIND_DATA));
	  fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    if (mLocalScanHandle == NULL)
    {
      LPCTSTR rootFolder = rootP->GetRootPath();
      int len = lstrlen(baseFullPath) + lstrlen(rootFolder);
      if (len >= MAX_FOLDER_PATH_LENGTH)
      {
        done = TRUE;
        delete[] baseFullPath;
        return 0;
      }
      len += 6;
      LPTSTR fileFindPath = new TCHAR[len];
      MakeFullPathName(fileFindPath, len, rootFolder, baseFullPath);

      // scan with real folder path if it or parent renamed
      MakeFullPathName(fileFindPath, len, fileFindPath, L_ASTERISK_STR);

	    mLocalScanHandle = FindFirstFile(fileFindPath, &fd);
      if (mLocalScanHandle == INVALID_HANDLE_VALUE)
        mLocalScanHandle = NULL;

      delete[] fileFindPath;

      if (mLocalScanHandle == NULL)
      {
        done = TRUE;
        delete[] baseFullPath;
        return 0;
      }
      else
      {
        addLocalState(ITEM_STATE_NOW_EXIST);
      }
    }
    else
    {
      if (!FindNextFile(mLocalScanHandle, &fd))
      {
        FindClose(mLocalScanHandle);
        mLocalScanHandle = NULL;
        mChildItemIndex += count + folder_count;
        done = TRUE;
        delete[] baseFullPath;
        return count;
      }
    }

#ifdef SCAN_FOLDER_FILE_AT_ONCE
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
			if ((StrCmp(fd.cFileName, L".") == 0) ||
				(StrCmp(fd.cFileName, L"..") == 0) ||
				(StrCmpI(fd.cFileName, meta_folder_name) == 0) ||
				(StrCmpI(fd.cFileName, DOWNTEMP_FOLDER_NAME) == 0))
        continue;

      if (fd.cFileName[0] == '$')
        continue;

      LPTSTR pathName = AllocPathName(baseFullPath, fd.cFileName);

#ifdef SYNC_ALL_DRIVE
      // this is full path on SYNC_ALL_DRIVE
      if (IsSystemDirectory(pathName))
      {
        TCHAR msg[256] = _T("System folder skip : ");
        StringCchCat(msg, 256, pathName);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        delete[] pathName;
        continue;
      }

      if (gpCoreEngine->IsPolicyExcludedPath(pathName))
      {
        TCHAR msg[256] = _T("Excluded folder skip : ");
        StringCchCat(msg, 256, pathName);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        delete[] pathName;
        continue;
      }
#endif

      // check moved or renamed first
      KSyncItem *item = findMovedRenamedOrigin(fd.cFileName);
      int syncMethod = ((KSyncRootFolderItem*)rootP)->GetSyncMethod();
      if (item == NULL)
        item = findChildByName(fd.cFileName);

      if (item == NULL)
      {
#ifdef USE_BACKUP
        if (((rootP == NULL) || (rootP->IsPartialSyncOn(pathName, TRUE))) &&
          (syncMethod & SYNC_METHOD_BACKUP)) // 복구에서는 로컬에만 존재하는 폴더는 생략한다.
#else
        if ((rootP == NULL) || rootP->IsPartialSyncOn(pathName, TRUE))
#endif
        {
          item = new KSyncFolderItem(this, GetBaseFolder(), fd.cFileName); // TODO::check need
          item->LoadMetafile(NULL, META_CLEAR_STATE);
          mChildList.push_back(item);
        }
        else
        {
          TCHAR msg[300];
          if (syncMethod & SYNC_METHOD_RESTORE)
            StringCchPrintf(msg, 300, _T("Skip empty backup folder : %s"), pathName);
          else
            StringCchPrintf(msg, 300, _T("Skip on Partial Sync : %s"), pathName);

          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
      }
      if (item != NULL)
      {
        SYSTEMTIME fileTime;
        FileTimeToSystemTime(&fd.ftLastWriteTime, &fileTime);
        SYSTEMTIME accessTime;
        FileTimeToSystemTime(&fd.ftLastAccessTime, &accessTime);

        item->addLocalState(ITEM_STATE_NOW_EXIST);
        item->UpdateLocalNewFileInfo(0, fileTime, accessTime, mUserOid);

        ++folder_count;
      }
      if (pathName != NULL)
        delete[] pathName;
    }
		else if (lstrlen(fd.cFileName) > 0)
#else
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
      (lstrlen(fd.cFileName) > 0))
#endif // SCAN_FOLDER_FILE_AT_ONCE
    {
      __int64 fileSize = MAKE_INT64(fd.nFileSizeLow, fd.nFileSizeHigh);

#if 0
      // a.txt를 ~a.txt로 rename하면 여기서 목록에서 빠지고, 그러면 삭제 파일로 오인됨.
      // 따라서 제외파일은 검색 완료후 일괄처리하여야 함

      // check sync exclude mask
      if (gpCoreEngine->IsExcludeFileMask(fd.cFileName) ||
        policy->IsExcludeFileMask(fd.cFileName))
      {
        TCHAR msg[300];
        StringCchPrintf(msg, 300, _T("Excluded file on local=%s"), fd.cFileName);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      else
#endif

      if (!IsLocalScanSkipFile(rootP, fd.cFileName, FALSE))
      {
        /* for test
        TCHAR fullname[KMAX_PATH];
        MakeFullPathName(fullname, KMAX_PATH, mBaseFolder, fd.cFileName);
        __int64 fileSizex = GetFileSize(fullname);
        */

        // check moved or renamed first
        KSyncItem *item = findMovedRenamedOrigin(fd.cFileName);

        // if item does not synced yet, clear rename
        if ((item != NULL) && !IsValidSystemTime(item->mLocal.FileTime))
        {
          if (item->mpLocalRenamedAs != NULL)
          {
            delete[] item->mpLocalRenamedAs;
            item->mpLocalRenamedAs = NULL;
          }
          item = NULL;
        }

        if (item == NULL)
          item = findChildByName(fd.cFileName);

        if ((item == NULL) &&
          !IsLocalScanSkipFile(rootP, fd.cFileName, TRUE)) // 신규의 비대상 파일은 스킵
        {
          TCHAR fullname[MAX_PATH];
          MakeFullPathName(fullname, KMAX_PATH, GetBaseFolder(), fd.cFileName);

          if ((rootP == NULL) || (rootP->IsPartialSyncOn(fullname, TRUE)))
          {
            item = new KSyncItem(this, GetBaseFolder(), fd.cFileName);
            mChildList.push_back(item);
            item->LoadMetafile(NULL, META_CLEAR_STATE);
            init_count++;
          }
        }

        if (item != NULL)
        {
          SYSTEMTIME fileTime;
          FileTimeToSystemTime(&fd.ftLastWriteTime, &fileTime);
          SYSTEMTIME accessTime;
          FileTimeToSystemTime(&fd.ftLastAccessTime, &accessTime);

          item->addLocalState(ITEM_STATE_NOW_EXIST);
          item->UpdateLocalNewFileInfo(fileSize, fileTime, accessTime, mUserOid);
          ++count;
        }
      }
    }
	}
  mChildItemIndex += count + folder_count;
  done = FALSE;
  delete[] baseFullPath;
  return count; // need continue scan
#else
  done = TRUE;
#endif
  return count;
}

void KSyncFolderItem::ClearServerSideInfoCascade()
{
  if (mParent != NULL)
    clearServerSideInfo();

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      ((KSyncFolderItem *)(*it))->ClearServerSideInfoCascade();
    }
    else
      ((KSyncFolderItem *)(*it))->clearServerSideInfo();
    ++it;
  }
}

KSyncItem *KSyncFolderItem::findMovedRenamedOrigin(LPCTSTR name)
{
  TCHAR newfilename[KMAX_LONGPATH];
	// MakeFullPathName(newfilename, KMAX_PATH, mBaseFolder, name);
  //MakeFullPathName(newfilename, KMAX_PATH, ((KSyncFolderItem *)mParent)->GetBaseFolder(), name);

  TCHAR actualPath[KMAX_LONGPATH];
  GetActualPathName(actualPath, KMAX_LONGPATH, FALSE);
	MakeFullPathName(newfilename, KMAX_LONGPATH, actualPath, name);

  KSyncRootFolderItem *rootItem = (KSyncRootFolderItem *)GetRootFolderItem();

  // find item which was renamed/moved as newfilename
  KSyncItem *item = rootItem->GetMovedRenamedOriginal(newfilename);
  if (item != NULL)
    item->CheckLocalRenamedAs(actualPath, name);

  return item;
}

BOOL KSyncFolderItem::MoveSyncItem(KSyncItem *item, LPTSTR relativePathName, BOOL pathHasName)
{
  KSyncFolderItem *lastParent = (KSyncFolderItem *)item->mParent;

  KSyncFolderItem *p = NULL;
  if (lstrlen(relativePathName) > 0)
  {
    if (pathHasName)
    {
      LPTSTR parentPath = NULL;
      SplitPathNameAlloc(relativePathName, &parentPath, NULL);

      p = (KSyncFolderItem *)FindChildItemCascade(parentPath, 3); // folder or root folder
      if (p == NULL)
      {
        TCHAR msg[256] = _T("InternalError : cannot found parent folder : ");
        StringCchCat(msg, 256, relativePathName);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }

      if (parentPath != NULL)
        delete[] parentPath;
    }
    else
    {
      p = (KSyncFolderItem *)FindChildItemCascade(relativePathName, 1);
      if (p == NULL)
      {
        TCHAR msg[256] = _T("InternalError : cannot found folder : ");
        StringCchCat(msg, 256, relativePathName);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }
  }
  else
  {
    p = GetRootFolderItem();
  }

  if (p == lastParent)
    return FALSE;

  if ((p != NULL) && p->IsFolder())
  {
    if (lastParent != NULL)
    {
      if (!lastParent->RemoveItem(item))
      {
        TCHAR msg[400] = _T("RemoveItem fail : ");
        StringCchCat(msg, 400, item->GetFilename());
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }

    item->SetParent(p);
    p->AddChild(item);

    if (item->IsFolder() == 1)
      item->OnPathChanged();

    return TRUE;
  }

  TCHAR msg[400] = _T("Cannot found parent item for relative path : ");
  StringCchCat(msg, 400, relativePathName);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  return FALSE;
}

#if 0
BOOL KSyncFolderItem::CreateFileSyncMetaData(LPCTSTR filename, LPCTSTR base_path,
  LPCTSTR object_path, LPCTSTR server_id, SYSTEMTIME& datetime, __int64 size, BOOL isFile)
{
	int file_length = lstrlen(filename);
	int base_length = lstrlen(base_path);

	if (file_length > base_length)
	{
    TCHAR path[KMAX_PATH];
    TCHAR name[KMAX_PATH];
    SplitPathName(filename, path, name);

    KSyncItem *item = NULL;
    if (isFile)
      item = new KSyncItem(NULL, path, name);
    else
      item = new KSyncFolderItem(NULL, path, name);

    item->setLocalState(ITEM_STATE_NEWLY_ADDED);
    item->SetLocalFileSize(size);
    item->mLocal.FileTime = datetime;
    item->mResult = FILE_NEED_UPLOAD; // initial state is UPLOAD

    item->StoreMetafile(NULL);
    delete item;

		if (isFile)
		{
      TCHAR msg[512] = _T("Initialize meta-info for file : ");
      StringCchCat(msg, 512, filename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
		}
    else
    {
      TCHAR msg[512] = _T("Initialize meta-info for folder : ");
      StringCchCat(msg, 512, filename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
		return TRUE;
	}
	return false;
}

// return 0 to continue, return 1 if all done.
int KSyncFolderItem::initializeFiles(LPCTSTR baseFolder, KSyncPolicy *p, int phase, BOOL withChild)
{
  int count = 0;
  TCHAR object_path[KMAX_PATH];
  MakeFullPathName(object_path, KMAX_PATH, baseFolder, meta_folder_name);
  MakeFullPathName(object_path, KMAX_PATH, object_path, object_folder_name);

  if (mResult == 0)
  {
    setLocalState(ITEM_STATE_NEWLY_ADDED);
    mResult = FILE_NEED_UPLOAD; // initial state is UPLOAD
    StoreMetafile(NULL);
  }

  while(withChild)
  {
    KSyncItem *item = GetNextChild(phase, 2);

    if (count > 20)
    {
#ifdef _DEBUG
      TCHAR msg[300] = _T("Initialize meta-info for 20 items on ");
      StringCchCat(msg, 512, mLocal.Filename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
      return 0;
    }
    // compare files
    if (item != NULL)
    {
      if (item->mParent == NULL) // skip root folder
      {
        item->SetPhase(phase);
        continue;
      }

      item->setLocalState(ITEM_STATE_NEWLY_ADDED);
      item->SetLocalFileSize(item->mLocalNew.FileSize);
      item->mLocal.FileTime = item->mLocalNew.FileTime;
      item->mResult = FILE_NEED_UPLOAD; // initial state is UPLOAD
      item->StoreMetafile(NULL, META_MASK_FILENAME|META_MASK_LOCAL|META_MASK_LASTUSER|
        META_MASK_LOCALSTATE|META_MASK_SYNCRESULT|META_MASK_TYPE);

#ifdef _DEBUG_INIT
      TCHAR filename[KMAX_PATH];
      MakeFullPathName(filename, KMAX_PATH, GetBaseFolder(), item->mLocal.Filename);
      TCHAR msg[300] = _T("Initialize meta-info for ");
		  if (item->IsFolder())
        StringCchCat(msg, 300, _T("file : "));
      else
        StringCchCat(msg, 300, _T("folder : "));
      StringCchCat(msg, 512, filename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif

      item->SetPhase(phase);
      if (!item->IsFolder())
      {
        RemoveItem(item);
        delete item;
      }
    }
    else
      break;
    ++count;
  }
  return 1;
}

int KSyncFolderItem::mergeOldNewInfo()
{
  if (IsFolderFullPathChanged())
  {
    // folder location changed
    addServerState(ITEM_STATE_LOCATION_CHANGED);
    OutputDebugString(_T("folder FullPathIndex changed : "));
    OutputDebugString(GetBaseFolder());
    OutputDebugString(_T("\n"));
  }
  MergeServerInfo(mServer,  mServerNew);
  return 0;
}

#endif

// return  TRUE if all files are done
BOOL KSyncFolderItem::compareFiles(/*LPCTSTR baseFolder, KSyncPolicy *p */ KSyncRootFolderItem* root, int &count, int phase)
{
  // do not merge hear, after sync - then merge
  /*
  if (mChildItemIndex == 0)
  {
    // merge folder info on root itself
    mergeOldNewInfo();
  }
  */

  LPCTSTR baseFolder = root->GetRootPath();
  KSyncRootStorage* storage = root->GetStorage();

  if ((mChildItemIndex == 0) && (mParent != NULL))
  {
    /*
    TCHAR msg[300];
    StringCchPrintf(msg, 300, _T("compareFiles=%s"), GetBaseFolder());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    */

    // check folder rights
    if (mServerState & ITEM_STATE_NOW_EXIST) // server folder exist already
    {
      if (!CheckPermissionSubFolder(GetServerPermission()))
      {
        TCHAR msg[256] = _T("Server folder has not enough permissions : ");
        StringCchCat(msg, 256, GetFilename());
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        addServerState(ITEM_STATE_NO_PERMISSION);
        mResult = FILE_SYNC_NO_FOLDER_PERMISSION;
        ++mChildItemIndex;
        return TRUE;
      }
    }
  }

  if (mChildItemIndex == 0)
  {
    if (root->GetDBVersion() < DB_VERSION_2)
      StoreMetafile(storage, META_MASK_SERVERSTATE|META_MASK_LOCALSTATE, 0);

    compareSyncFileItem(baseFolder, root->GetSyncMethod());
    removeEndOfJobObject();

    // 제거될 항목이면 하위 항목 처리할 필요 없음.
    // 20170923 - 동기화에서는 제거되더라도 하위 항목이 변경, 이동 확인이 필요함.
#ifdef USE_BACKUP
    if (mResult & FILE_NEED_RETIRED)
      return TRUE;
#endif
  }

  KSyncItemIterator it = mChildList.begin();
  if (mChildItemIndex >= (int)mChildList.size())
    return TRUE;

  if (mChildItemIndex > 0)
    it += mChildItemIndex;

  count = 0;
  BOOL done = TRUE;
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (!item->IsFolder())
    {
      /*
      TCHAR msg[256] = _T("compareSyncFileItem :");
      StringCchCat(msg, 256, item->mLocal.Filename);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      */

      if (item->mPhase < phase)
      {
        if (root->GetDBVersion() < DB_VERSION_2)
          item->StoreMetafile(storage, META_MASK_SERVERSTATE|META_MASK_LOCALSTATE, 0);
        int r = item->compareSyncFileItem(baseFolder, root->GetSyncMethod());
        item->SetPhase(phase);
        ++count;
      }
    }

    ++mChildItemIndex;
    ++it;
    if (count >= NUMBER_OF_ITEMS_COMPARE)
    {
      done = FALSE;
      break;
    }
  }
  mLocal.FileSize = GetTotalSize(FALSE);
  return done;
}

int KSyncFolderItem::RemoveUnchanged(int phase)
{
  SetPhase(phase);

  KSyncFolderItem *folder;
  // do on child first
  while((folder = (KSyncFolderItem *)GetNextChild(phase, 1)) != NULL)
  {
    folder->RemoveUnchanged(phase);
  };

  // remove file items
  KSyncItem *item;
  while((item = GetNextChild(phase, 0)) != NULL)
  {
    if (item->GetResult() == FILE_NOT_CHANGED)
    {
      RemoveItem(item);
      delete item;
    }
    else
      item->SetPhase(phase);
  }
  return ((int)mChildList.size());
}

int KSyncFolderItem::RemoveUnconflictItem()
{
  SetPhase(PHASE_CLEAN_UNCONFLICT);
  StoreLogHistory(_T(__FUNCTION__), _T("Phase 1"), SYNC_MSG_LOG);

  KSyncFolderItem *folder;
  // do on child first
  while((folder = (KSyncFolderItem *)GetNextChild(PHASE_CLEAN_UNCONFLICT, 1)) != NULL)
  {
    folder->RemoveUnconflictItem();

    // do not remove folder object. others can be moved into hear
    /*
    if ((folder->GetTotalCount(2) == 0) && // is empty
      (folder->GetConflictResult() == 0))
    {
      RemoveItem(folder);
      delete folder;
    }
    */
  };

  StoreLogHistory(_T(__FUNCTION__), _T("Phase 2"), SYNC_MSG_LOG);
  // remove file items
  KSyncItem *item;
  while((item = GetNextChild(PHASE_CLEAN_UNCONFLICT, 0)) != NULL)
  {
    if (item->GetConflictResult() == 0)
    {
      RemoveItem(item);
      delete item;
    }
    else
      item->SetPhase(PHASE_CLEAN_UNCONFLICT);
  }
  StoreLogHistory(_T(__FUNCTION__), _T("Phase 3"), SYNC_MSG_LOG);
  return ((int)mChildList.size());
}

void KSyncFolderItem::removeEndOfJobObject()
{
  KSyncRootFolderItem *rootItem =  (KSyncRootFolderItem *)GetRootFolderItem();
  KSyncRootStorage* s = rootItem->GetStorage();
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->mPhase == PHASE_END_OF_JOB)
    {
      TCHAR msg[256] = _T("remove : ");
      StringCchCat(msg, 256, item->mpRelativePath);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      if (((item->mConflictResult & FILE_SYNC_ERR) != FILE_SYNC_NOT_EXIST_DRIVE) &&
        !item->IsExcludedSync())
      {
        item->StoreMetafile(s, 0, METAFILE_DELETE);
      }
      it = mChildList.erase(it);
      delete item;
    }
    else
    {
      ++it;
    }
  }
}

void KSyncFolderItem::SetPhase(int p)
{
  KSyncItem::SetPhase(p);
  mChildItemIndex = 0;
}

void KSyncFolderItem::SetPhaseRecurse(int p, int mask)
{
  if (!(mask & EXCLUDE_CHANGED_ITEM) || ((mResult == 0) && !(mFlag & SYNC_ITEM_MOVE_TARGET)))
    SetPhase(p);

  KSyncItemIterator it = mChildList.begin();
  int index = 0;
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;

    if (item->IsFolder())
      ((KSyncFolderItem *)item)->SetPhaseRecurse(p, mask);
    else if (!(mask & EXCLUDE_CHANGED_ITEM) ||
      !(item->mResult & FILE_NEED_BOTH_MOVE)) // 이동될 항목은 바꾸지 않는다.
      item->SetPhase(p);
    ++it;
  }
}

int KSyncFolderItem::SetPhaseRetryRecurse(int p, int mask)
{
  int count = 0;

  if ((mPhase < PHASE_END_OF_JOB) &&  (mResult & (FILE_CONFLICT | FILE_SYNC_ERR)))
  {
    SetPhase(p);
    count++;
  }

  KSyncItemIterator it = mChildList.begin();
  int index = 0;
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;

    if (item->IsFolder())
    {
      count += ((KSyncFolderItem *)item)->SetPhaseRetryRecurse(p, mask);
    }
    else if ((item->mPhase < PHASE_END_OF_JOB) &&
      (item->mResult & (FILE_CONFLICT | FILE_SYNC_ERR)))
    {
      item->SetPhase(p);
      count++;
    }
    ++it;
  }
  return count;
}

void KSyncFolderItem::SetPhaseRevRecurse(int p)
{
  SetPhase(p);
  if (mParent != NULL)
    ((KSyncFolderItem *)mParent)->SetPhaseRevRecurse(p);
}

void KSyncFolderItem::SetPhaseForceRecurse(int p)
{
  SetPhaseForce(p);
  KSyncItemIterator it = mChildList.begin();
  int index = 0;
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
      ((KSyncFolderItem *)(*it))->SetPhaseForceRecurse(p);
    else
      (*it)->SetPhaseForce(p);
    ++it;
  }
}

int KSyncFolderItem::RemoveUnBackupedItems(KSyncRootFolderItem *root, int flag)
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (item->IsFolder())
    {
      LPCTSTR baseFolder = ((KSyncFolderItem*)item)->GetBaseFolder();
      if (item->IsOverridePath())
        baseFolder = item->mpOverridePath;

      if ((baseFolder != NULL) &&
        (item->mPhase < PHASE_END_OF_JOB) &&
        (!gpCoreEngine->IsTargetDrive(baseFolder) || // 대상 드라이브가 아닌 드라이브
        gpCoreEngine->IsPolicyExcludedPath(baseFolder))) // 백업 제외 폴더
      {
        StoreLogHistory(_T(__FUNCTION__), baseFolder, SYNC_MSG_LOG);

        // 백업에서는 처리할 필요 없으니 걍 안보이게 한다.
        // 재사용될 수 있으니 DB 삭제는 안한다.
        item->SetConflictResultRecurse(FILE_SYNC_NOT_EXIST_DRIVE, PHASE_END_OF_JOB, SET_WITH_ITSELF|SET_WITH_RECURSIVE);
        /*
        delete item;
        item = NULL;
        */
        ++count;
      }
      else
      {
        count += ((KSyncFolderItem*)item)->RemoveUnBackupedItems(root, flag);
      }
    }

    if (item == NULL)
      it = mChildList.erase(it);
    else
      ++it;
  }
  return count;
}

void KSyncFolderItem::SetConflictResultRecurse(int result, int phase, int flag)
{
  if (flag & SET_WITH_ITSELF)
  {
    KSyncItem::SetConflictResultRecurse(result, phase, flag);
  }

  KSyncItemIterator it = mChildList.begin();
  int index = 0;
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (!(flag & EXCLUDE_OUTGOING) ||
      !IsOutGoingChild(item))
    {
      item->SetConflictResultRecurse(result, phase, (flag | SET_WITH_ITSELF));
    }
    ++it;
  }
}

KSyncItem *KSyncFolderItem::findItemByOID(LPCTSTR serverOID)
{
  if ((lstrlen(mServerOID) > 0) &&
    (StrCmp(mServerOID, serverOID) == 0))
    return this;

  KSyncItemIterator it = mChildList.begin();
  int index = 0;
  while(it != mChildList.end())
  {
    if ((lstrlen((*it)->mServerOID) > 0) &&
      (StrCmp((*it)->mServerOID, serverOID) == 0))
      return *it;
    ++it;
  }

  // find from root if it was moved
  KSyncFolderItem *rootItem = GetRootFolderItem();
  if (rootItem != NULL)
    return rootItem->FindChildItemByOIDCascade(serverOID);
  return NULL;
}

KSyncItem *KSyncFolderItem::findChildByName(LPCTSTR fileName)
{
  KSyncItemIterator it = mChildList.begin();
  int index = 0;
  while(it != mChildList.end())
  {
    if (StrCmpI((*it)->GetFilename(), fileName) == 0)
      return *it;
    ++it;
  }
  return NULL;
}

BOOL KSyncFolderItem::MergeChild(KSyncItem *item)
{
  KSyncItem *existing = findChildByName(item->GetFilename());
  if (existing != NULL)
  {
    existing->MergeItem(item);
    delete item;
    return TRUE;
  }
  else
  {
    AddChild(item);
    return FALSE;
  }
}

int KSyncFolderItem::MoveChildItems(KSyncFolderItem *dest)
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    it = mChildList.erase(it);
    dest->MergeChild(item);
    count++;
  }
  mChildList.clear();
  return count;
}

BOOL KSyncFolderItem::RemoveItem(KSyncItem *item)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if (*it == item)
    {
      mChildList.erase(it);
      return TRUE;
    }
    else if ((*it)->IsFolder())
    {
      if (((KSyncFolderItem *)(*it))->RemoveItem(item))
        return TRUE;
    }
    ++it;
  }
  return FALSE;
}

void KSyncFolderItem::FreeChilds()
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    delete item;
    it = mChildList.erase(it);
    /*
    if (item->IsFolder())
    {
      ((KSyncFolderItem *)item)->FreeChilds();
    }
    */
  }
}

BOOL KSyncFolderItem::IsItemExist(LPCTSTR fullname)
{
  return IsDirectoryExist(fullname);
}

int KSyncFolderItem::compareSyncFileItem(LPCTSTR baseFolder, int syncMethod)
{
  if (IsFolder() == 2) // skip root folder item
    return R_SUCCESS;

  int r = R_SUCCESS;
  int lastResult = mResult | mConflictResult;

  if (!ReadyMetafilename(NULL, 0))
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("No Metafile for %s\n"), GetBaseFolder());
    OutputDebugString(msg);
  }

#ifdef USE_BACKUP
  if (mParent->IsSystemRoot())
    //((mBaseFolder != NULL) && (lstrlen(mBaseFolder) > 0) && (mBaseFolder[lstrlen(mBaseFolder)-1] == ':'))
  {
    LPCTSTR targetPath = mpRelativePath;
    if (IsOverridePath())
      targetPath = mpOverridePath;

#ifdef _DEBUG_DRIVES
    if (targetPath[0] > 'C') // 대상 드라이브가 아닌 드라이브
#else
    if (!gpCoreEngine->IsTargetDrive(targetPath)) // 대상 드라이브가 아닌 드라이브
#endif
    {
      if (syncMethod & SYNC_METHOD_RESTORE)
      {
        // 하위 항목도 모두 오류로
        SetConflictResultPhase(FILE_SYNC_NOT_EXIST_DRIVE, PHASE_ENDING, 
          SET_WITH_ITSELF|SET_LOCAL_DELETED|SET_WITH_RECURSIVE|STORE_META);
      }
      else
      {
        // 백업에서는 처리할 필요 없으니 걍 지운다.
        SetConflictResultPhase(FILE_SYNC_NOT_EXIST_DRIVE, PHASE_END_OF_JOB, SET_WITH_ITSELF|SET_WITH_RECURSIVE);
      }
      r = FILE_SYNC_NOT_EXIST_DRIVE;
      goto compare_done;
    }
  }
#endif

  if (checkAddedRenamedSyncError(0))
    return 0;

  if (mLocalState == ITEM_STATE_NOW_EXIST) // local folder is added newly
    addLocalState(ITEM_STATE_NEWLY_ADDED);
  if (mServerState == ITEM_STATE_NOW_EXIST) // server folder is added newly
    addServerState(ITEM_STATE_NEWLY_ADDED);

  if ((mLocalState & ITEM_STATE_EXISTING) == ITEM_STATE_WAS_EXIST) // info exist & folder not exist => deleted
    addLocalState(ITEM_STATE_DELETED);
  if (mServerState == ITEM_STATE_WAS_EXIST) // info exist & folder not exist => deleted
  {
    // check if NoACL
    if (RefreshServerNewState() == R_SUCCESS)
    {
      if (mServerNew.Permissions == 0)
        r = FILE_SYNC_NO_FOLDER_PERMISSION;
    }
    if (r == 0)
      addServerState(ITEM_STATE_DELETED);
  }

  if (mLocalState & ITEM_STATE_NEWLY_ADDED)
  {
#ifndef USE_BACKUP
    // There is no info file, it will be a newly added item
    // create info file
    StoreMetafile(NULL);
#endif

#ifndef SYNC_ALL_DRIVE
    // check if old SyncRootFolder
    if (IsFolder() && (KSyncRootFolderItem::IsSyncRootFolder(GetBaseFolder()) > 0))
      r = FILE_RELEASE_PREV_SYNC_ROOT;
    else if (IsLocalNotExist())
      r = FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED;
#endif
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
    delete[] mpServerRenamedAs;
    mpServerRenamedAs = NULL;
  }

  if (isLocalRenamedAs())
    addLocalState(ITEM_STATE_MOVED);
  if (isServerRenamedAs())
    addServerState(ITEM_STATE_MOVED);
  
  if (r == 0)
  {
    r = makeResultFromState(0, syncMethod);

    // check filename length if it is normal
    if ((r & (FILE_CONFLICT | FILE_SYNC_ERR)) == 0)
    {
      if ((r & FILE_NEED_RETIRED) == 0)
      {
        // check result name
        LPTSTR resultFilename = AllocSyncResultName(((r & FILE_NEED_UPLOAD) ? AS_LOCAL : 0) | AS_FULLPATH);

        if (lstrlen(resultFilename) >= MAX_FOLDER_PATH_LENGTH)
        {
          TCHAR msg[KMAX_LONGPATH+100];
          StringCchPrintf(msg, KMAX_LONGPATH+100, _T("LongFolderName Skip : %s, len=%d"), resultFilename, lstrlen(resultFilename));
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          RefreshLocalFileInfo(mUserOid);
          FreeChilds();
          r = FILE_SYNC_OVER_PATH_LENGTH;
        }

        delete[] resultFilename;
      }
    }
    if (r == FILE_NEED_UPLOAD)
    {
      if(mServerState & ITEM_STATE_NOW_EXIST)
        r = 0;
    }

    // ignore same folder is added newly
    if ((r & FILE_CONFLICT) == FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED)
      r &= ~FILE_CONFLICT;
  }

  int mask = META_MASK_CONFLICT | META_MASK_SYNCRESULT;

  if (r == (FILE_NEED_DOWNLOAD | FILE_NEED_UPLOAD)) // 양쪽 새로 추가된 폴더
  {
    r = 0;
    lastResult = -1;
    mLocalState = mServerState = ITEM_STATE_EXISTING;
    //로컬측 날짜 업데이트 필요
    RefreshLocalFileInfo(mUserOid);

    mask = META_MASK_ALL;
  }

  SetResult(r | (mResult & FILE_NOW_SYNCHRONIZING));

  mConflictResult = mResult & (FILE_CONFLICT | FILE_SYNC_ERR);
  mEnable = !(mConflictResult & FILE_SYNC_ERR);

  if (lastResult != (mResult | mConflictResult))
  {
    if (mLocalState & ITEM_STATE_NEWLY_ADDED)
      mask = META_MASK_ALL;
    StoreMetafile(NULL, mask);
  }

#ifdef USE_BACKUP
compare_done:
#endif
  TCHAR msg[600];
  StringCchPrintf(msg, 600, _T("name=%s, result=%s"), GetFilename(), GetSyncCompareResultString(r));
  if (r & FILE_NEED_LOCAL_MOVE)
  {
    StringCchCat(msg, 600, _T(" as Server : "));
    StringCchCat(msg, 600, (mpServerRenamedAs == NULL) ? _T("NULL") : mpServerRenamedAs);
  }
  else if (r & FILE_NEED_SERVER_MOVE)
  {
    StringCchCat(msg, 600, _T(" as Local : "));
    StringCchCat(msg, 600, (mpLocalRenamedAs == NULL) ? _T("NULL") : mpLocalRenamedAs);
  }
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  /*
  if (r & (FILE_CONFLICT | FILE_SYNC_ERR))
  {
    StringCchPrintf(msg, 600, _T("%s : retired or conflict error, child item skipped(x)"), GetFilename());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    // 하위에 변경된 파일이 있는지 검사를 하여야 하니 phase 바꾸지 않음(20160528)
    // SetConflictResultRecurse(0, PHASE_CONFLICT_FILES, 0);
  }
  */

  if (r & FILE_NEED_RETIRED)
  {
    StringCchPrintf(msg, 600, _T("%s : retired or conflict error"), GetFilename());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    // 바깥으로 이동되지 않는 child들은 phase 끝으로 이동.
    // 20160528-하위에 변경된 파일이 있는지 검사를 하여야 하니 phase 바꾸지 않음
    // SetConflictResultRecurse(0, PHASE_CONFLICT_FILES, EXCLUDE_OUTGOING);
  }

  return r;
}

void KSyncFolderItem::SetUnchangedItemPhase(int added_phase, int renamed_phase, int normal_phase, BOOL parentCheckNeed)
{
  KSyncItem::SetUnchangedItemPhase(added_phase, renamed_phase, normal_phase, parentCheckNeed);

  // folder not changed, but folderFullPathIndex changed, update on metafile
  if ((mPhase == renamed_phase) && (mServerState & ITEM_STATE_LOCATION_CHANGED))
  {
    // KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    StoreMetafile(NULL, META_MASK_FILENAME|META_MASK_SERVERPATHINDEX, 0);
  }

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *(it++);
    item->SetUnchangedItemPhase(added_phase, renamed_phase, normal_phase, parentCheckNeed);
  }

  // if it has child which is changed, then parent need check
  if (parentCheckNeed)
  {
    if (hasChangedChild())
      KSyncItem::SetUnchangedItemPhase(added_phase-1, renamed_phase-1, normal_phase-1, FALSE);
  }
}

void KSyncFolderItem::SetCascadedItemFlag(LPCTSTR pathName, int flag)
{
  LPCTSTR p = StrChr(pathName, '\\');
  if (p != NULL)
  {
    int len = (int)(p - pathName);
    KSyncItemIterator it = mChildList.begin();
    int count = 0;
    while(it != mChildList.end())
    {
      KSyncItem *item = *(it++);
      if (item->IsFolder() &&
        (lstrlen(item->GetFilename()) == len) &&
        (StrCmpN(item->GetFilename(), pathName, len) == 0))
      {
        item->mFlag |= flag;
        p++;
        ((KSyncFolderItem*)item)->SetCascadedItemFlag(p, flag);
      }
    }
  }
}

int KSyncFolderItem::CheckMoveTargetFolders(KSyncFolderItem* rootItem, int syncMethod)
{
  KSyncItemIterator it = mChildList.begin();
  int count = 0;
  while(it != mChildList.end())
  {
    KSyncItem *item = *(it++);
    if (syncMethod & SYNC_METHOD_BACKUP)
    {
      if (item->mpLocalRenamedAs != NULL)
      {
        rootItem->SetCascadedItemFlag(item->mpLocalRenamedAs, SYNC_ITEM_MOVE_TARGET);
        count++;
      }
    }
    else if (syncMethod & SYNC_METHOD_RESTORE)
    {
      if (item->mpServerRenamedAs != NULL)
      {
        rootItem->SetCascadedItemFlag(item->mpServerRenamedAs, SYNC_ITEM_MOVE_TARGET);
        count++;
      }
    }

    if (item->IsFolder())
    {
      count += ((KSyncFolderItem*)item)->CheckMoveTargetFolders(rootItem, syncMethod);
    }
  }
  return count;
}

int KSyncFolderItem::RemoveUnreferencedFolders(KSyncRootFolderItem *root, int syncMethod)
{
  BOOL enabled = FALSE;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    BOOL bRemove = FALSE;

    if (item->IsFolder())
    {
      BOOL is_excluded_path = gpCoreEngine->IsPolicyExcludedPath(item->mpRelativePath);

      if (!is_excluded_path)
        ((KSyncFolderItem*)item)->RemoveUnreferencedFolders(root, syncMethod);

      // 하위 항목이 비어있거나, 제외 경로명이면
      if ( (((KSyncFolderItem*)item)->GetTotalCount(0) == 0) ||
        is_excluded_path)
      {
        if (item->mFlag & SYNC_ITEM_MOVE_TARGET)
        {
          // 제거하지 않고 phase만 예외로 설정.
          // but, 없는 폴더는 생성 작업이 진행되어야 하니 phase 바꾸지 않음
          //((KSyncFolderItem*)item)->SetPhaseRecurse(PHASE_ENDING, 0);
        }
        else
        {
          /*
          if ((item->mLocalState & ITEM_STATE_NEWLY_ADDED) && (syncMethod & SYNC_METHOD_BACKUP))
            bRemove = TRUE;

          if ((item->mServerState & ITEM_STATE_NEWLY_ADDED) && (syncMethod & SYNC_METHOD_RESTORE))
            bRemove = TRUE;
          */
          bRemove = TRUE;
        }
      }

      // 하위 폴더가 MOVE_TARGET이면 parent도 MOVE_TARGET.
      if (item->mFlag & SYNC_ITEM_MOVE_TARGET)
        mFlag |= SYNC_ITEM_MOVE_TARGET;
    }
    else
    {
     bRemove = root->IsExcludeFileMask(item);
    }

    // 오류 항목.
    /*
    if ((item->GetConflictResult() | item->mChildResult) & (FILE_CONFLICT | FILE_SYNC_ERR))
      bRemove = FALSE;
    */

    if (bRemove)
    {
      StoreLogHistory(_T(__FUNCTION__), item->mpRelativePath, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
      it = mChildList.erase(it);
      delete item;
    }
    else
    {
      ++it;
    }
  }
  return (mEnable || enabled);
}

// return TRUE if item is enabled
BOOL KSyncFolderItem::SetPhaseOnResolve(int parent_phase, int renamed_phase, int normal_phase)
{
  BOOL enabled = FALSE;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *(it++);
    if (item->SetPhaseOnResolve(parent_phase, renamed_phase, normal_phase))
      enabled = TRUE;
  }

  if (mEnable)
    SetPhaseForce(normal_phase);
  else
    SetPhaseForce(enabled ? parent_phase : PHASE_END_OF_JOB);
  return (mEnable || enabled);
}

BOOL KSyncFolderItem::hasChangedChild()
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *(it++);
     if (item->GetResult() != FILE_NOT_CHANGED)
     {
       return TRUE;
     }
  }
  return FALSE;
}

int KSyncFolderItem::GetUnchangedItemCount(int type)
{
  int count = 0;

  if ((GetResult() == FILE_NOT_CHANGED) && IsMatchType(type))
    ++count;

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *(it++);
    if (item->IsFolder())
    {
      count += ((KSyncFolderItem *)item)->GetUnchangedItemCount(type);
    }
    else
    {
      if ((item->GetResult() == FILE_NOT_CHANGED) && item->IsMatchType(type))
        ++count;
    }
  }
  return count;
}

BOOL KSyncFolderItem::CreateServerFolderIfDeleted(LPCTSTR baseFolder)
{
  if (mParent == NULL)
    return TRUE;

  BOOL r = TRUE;
  if (mParent != NULL)
  {
    r = ((KSyncFolderItem *)mParent)->CreateServerFolderIfDeleted(baseFolder);
    if (!r)
      return FALSE;
  }

  if (GetDepth() <= 1) // not for drive items
	return r;

  //if ((mServer.pFullPathIndex == NULL) || (lstrlen(mServer.pFullPathIndex) == 0))
  if (GetServerFullPathIndex() == NULL)
  {
    if (!createServerFolder(mParent, &mpServerConflictMessage))
    {
      r = FALSE;
    }
    else
    {
      mServerState = ITEM_STATE_EXISTING;
      StoreMetafile(NULL, META_MASK_ALL, 0);
    }
  }
  return r;
}

int KSyncFolderItem::ReadyResolve(KSyncItem *rootItem, int phase, int syncMethod)
{
  if (IsSystemRoot())
    return R_SUCCESS;

  // 정상 폴더가 변경된 경우, 전체 동기화 유도.
  if (GetResult() == FILE_NOT_CHANGED)
  {
    {
      RefreshServerNewState();

      // check if server FullPathIndex changed
      if (IsFolderFullPathChanged())
      /*
      if ((lstrlen(mServer.FullPathIndex) > 0) && (lstrlen(mServerNew.FullPathIndex) > 0) &&
        (StrCmp(mServer.FullPathIndex, mServerNew.FullPathIndex) != 0))
      */
      {
        addServerState(ITEM_STATE_LOCATION_CHANGED);
        // child item cannot upload
      }

      /** no need because fullPathIndex checked
      if ((lstrlen(mServer.FullPath) > 0) && (lstrlen(mServerNew.FullPath) > 0))
      {
        if (StrCmpI(mServer.FullPath, mServerNew.FullPath) != 0)
        {
          mServerState |= ITEM_STATE_MOVED;
        }
      }
      **/
    }

    if((mLocalState & ITEM_STATE_MOVED) || (mServerState & ITEM_STATE_MOVED))
    {
      addServerState(ITEM_STATE_LOCATION_CHANGED);
      return R_FAIL_NEED_FULL_SYNC_PARTIAL;
    }
    return R_SUCCESS;
  }

  // 한쪽 삭제 오류에서 재생성 선택.
  int child_user_action = 0;
  int last_conflict = mConflictResult;
  if (mConflictResult & FILE_SYNC_ERR)
  {
    if (mServerState & ITEM_STATE_DELETED)
    {
      if (mConflictResult & FILE_NEED_UPLOAD)
        child_user_action = FILE_NEED_UPLOAD;
      else if (mConflictResult & FILE_NEED_DOWNLOAD)
        child_user_action = FILE_NEED_LOCAL_DELETE;
    }
    if (mLocalState & ITEM_STATE_DELETED)
    {
      if (mConflictResult & FILE_NEED_DOWNLOAD)
        child_user_action = FILE_NEED_DOWNLOAD;
      else if (mConflictResult & FILE_NEED_UPLOAD)
        child_user_action = FILE_NEED_SERVER_DELETE;
    }
  }
  // 한쪽삭제-한쪽편집 충돌에서 복사 선택하면 하위항목도 복사하도록.
  if (mConflictResult & (FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED))
    child_user_action = (mConflictResult & FILE_USER_ACTION_MASK);

  KSyncItem::ReadyResolve(rootItem, phase, syncMethod);
  // TODO : 
  // ?? MakeFullPathName(GetBaseFolder(), KMAX_PATH, mLocal.FullPath, mLocal.Filename);

  /*
  if (mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR))
    user_action = (mConflictResult & FILE_USER_ACTION_MASK);
  */

  if ((child_user_action != 0) && mEnable)
  {
    int flag = EXCLUDE_OUTGOING; // 밖으로 이동될 애들은 결과를 바꾸지 않는다.
    if (last_conflict & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)
      flag |= SET_SERVER_DELETED;
    else if (last_conflict & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)
      flag |= SET_LOCAL_DELETED;

    child_user_action |= FILE_NOW_ORPHAN;

    // set all child object same ConflictResult
    SetConflictResultRecurse(child_user_action, PHASE_CONFLICT_FILES-1, flag);
  }
  return R_SUCCESS;
}

int KSyncFolderItem::doSyncItem(KSyncPolicy *p, LPCTSTR baseFolder, int phase)
{
  BOOL changed = FALSE;
  TCHAR msg[256] = { 0 };
  int d = 0;
  int r = R_SUCCESS;  
  BOOL cancel_child_objects = FALSE;
  BOOL deleteMeta = FALSE;

  SetPhase(phase);

  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)GetRootFolderItem();
  LPTSTR relativePath = AllocRelativePathName();
  LPTSTR target = NULL;
  
  if (phase < PHASE_CONFLICT_FILES) // PHASE_CREATE_FOLDERS, PHASE_MOVE_FOLDERS, PHASE_COPY_FILES
  {
    d = GetResult();
    if (d & (FILE_CONFLICT | FILE_SYNC_ERR))
    {
      r = (d & (FILE_CONFLICT | FILE_SYNC_ERR));

       // if deleted, do not continue childs
      // cancel_child_objects = !!(d & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED);

      cancel_child_objects = TRUE;
      StringCchPrintf(msg, 256, _T("path_name=%s "), relativePath);
      changed = TRUE;
      goto end_sync_copy;
    }
  }
  else if (phase == PHASE_CONFLICT_FILES)
  {
    // 충돌, 오류, 이름변경, 새항목으로 추가 등이 아닌 항목은 스킵.
    /*
    if ((d & (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)) == 0)
      mEnable = FALSE;
    */

    if (!mEnable)
    {
      StringCchPrintf(msg, 256, _T("path_name=%s,skipped"), relativePath);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
      delete[] relativePath;

      return R_SUCCESS;
    }
    d = GetConflictResult();
    int user_action = (d & FILE_USER_ACTION_MASK);
    if ((d & (FILE_CONFLICT | FILE_SYNC_ERR)) && (user_action == 0))
    {
      r = (d & (FILE_CONFLICT | FILE_SYNC_ERR));
      cancel_child_objects = TRUE;

      StringCchPrintf(msg, 256, _T("path_name=%s "), relativePath);

      changed = TRUE;
      goto end_sync_copy;
    }

    // check if rename need, 이름변경이 사용자 선택에 의해 반대로 적용되는 경우
    LPTSTR oldpathname = AllocSyncResultName(AS_LOCAL);
    LPTSTR newpathname = AllocSyncResultName(0);
    if (StrCmpI(oldpathname, newpathname) != 0)
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

          // mpLocalRenamedAs : 이것은 지우면 이동 원복이 안된다. 
          // clearRenameInfo();
          if (mpServerRenamedAs != NULL)
            delete[] mpServerRenamedAs;

          mpServerRenamedAs = allocServerSideRelativePath(rootFolder, GetServerRelativePath(), GetServerFileName());
        }
      }
    }

    if (oldpathname != NULL)
      delete[] oldpathname;
    if (newpathname != NULL)
      delete[] newpathname;

#if 0
    // in case of FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED, user select upload, then create it as new folder
    if ((mServerState & ITEM_STATE_DELETED) &&
      (mLocalState & ITEM_STATE_MOVED) &&
      (d & FILE_NEED_UPLOAD))
    {
      /*
      mLocalState &= ~ITEM_STATE_MOVED;
      mLocalState |= ITEM_STATE_NEWLY_ADDED;

      // update local path & name
      TCHAR fullpath[MAX_PATH];
      GetSyncResultName(fullpath, MAX_PATH, AS_LOCAL);
      SplitPathName(fullpath, mLocal.FullPath, mLocal.Filename);
      StringCchCopy(mBaseFolder, MAX_PATH, fullpath);

      MoveMetafileAsRename(baseFolder);
      mLocalRenamedAs[0] = '\0';
      */
      renameServerObjectAs(FALSE);

      d &= ~(FILE_CONFLICT|FILE_SYNC_ERR);
    }

    /*
    if (d & FILE_SYNC_ERR)
    {
      if ((mServerState & ITEM_STATE_DELETED) && (d & FILE_NEED_UPLOAD))
      {
        mServerState &= ~ITEM_STATE_DELETED;
        d &= ~FILE_NEED_LOCAL_DELETE;
      }
      if ((mLocalState & ITEM_STATE_DELETED) && (d & FILE_NEED_DOWNLOAD))
      {
        mLocalState &= ~ITEM_STATE_DELETED;
        d &= ~FILE_NEED_SERVER_DELETE;
      }
    }
    */
#endif
  }

  // 로컬삭제이나 하위에 수정 파일이 있는 경우, 
  // ConflictDialog에서 삭제 또는 다운로드를 선택한 경우.
  if ((d & FILE_NEED_LOCAL_DELETE) && 
    ((phase != PHASE_CONFLICT_FILES) || (d & FILE_NEED_DOWNLOAD)))
  {
    if (!IsChildDeleteOk(TRUE))
    {
      d = 0;
      r = FILE_SYNC_DELETE_FOLDER_FAIL_CHILD_MODIFIED;
      cancel_child_objects = TRUE;
      changed = TRUE;
      StringCchPrintf(msg, 256, _T("folder [%s] has modified sub items. remove "), GetBaseFolder());
      goto end_sync_copy;
    }
  }

  if ((d & FILE_NEED_SERVER_DELETE) && 
    ((phase != PHASE_CONFLICT_FILES) || (d & FILE_NEED_UPLOAD)))
  {
    if (!IsChildDeleteOk(FALSE))
    {
      d = 0;
      r = FILE_SYNC_DELETE_FOLDER_FAIL_CHILD_MODIFIED;
      cancel_child_objects = TRUE;
      changed = TRUE;
      StringCchPrintf(msg, 256, _T("folder [%s] has modified or excluded sub item. remove "), GetBaseFolder());
      goto end_sync_copy;
    }
  }

  d &= ~(FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE);

  if (d & FILE_SAME_MOVED)
  {
    // int rootlen = lstrlen(rootFolder->GetBaseFolder()) + 1;

    LPTSTR oldRelativePath = AllocString(mpRelativePath);

    updateMetaAsRenamed();
    d &= ~(FILE_NEED_BOTH_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED);
    changed = TRUE;
    // MakeFullPathName(mBaseFolder, KMAX_PATH, mLocal.FullPath, mLocal.Filename);

    // update fmaplist.txt
    rootFolder->UpdateMetaFolderNameMapping(oldRelativePath, GetBaseFolder(), TRUE);

    // folder name change -> should update all child object's ifo
    UpdateChildItemPath(rootFolder, 1, NULL, NULL, TRUE);
    delete[] oldRelativePath;

    goto end_sync_copy;
  }
  else
  {
    if ((mLocalState & ITEM_STATE_MOVED) && (mServerState & ITEM_STATE_MOVED))
    {
      if (d & FILE_NEED_UPLOAD) // use local name
        d |= FILE_NEED_SERVER_MOVE;
      else if (d & FILE_NEED_DOWNLOAD)
        d |= FILE_NEED_LOCAL_MOVE;
    }
    else if (mLocalState & ITEM_STATE_MOVED)
    {
      d |= FILE_NEED_SERVER_MOVE;
    }
    else if (mServerState & ITEM_STATE_MOVED)
    {
      d |= FILE_NEED_LOCAL_MOVE;
    }
  }

  if (d & (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE))
  {
    if (d & FILE_NEED_SERVER_MOVE) // isLocalRenamedAs())
    {
      if (mServerState & ITEM_STATE_DELETED) // d & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)
      {
        // not need server rename, but need update local ifo
        int err = renameServerObjectAs(FALSE);
        mServerState &= ~ITEM_STATE_DELETED;
        r &= ~(FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED);
        d &= ~(FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED);
        if (err != R_SUCCESS)
          r = err;
      }
      else
      {
        if (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD) // no upload permission or fail to rename server-file
        {
          if (mpLocalRenamedAs == NULL) //이름 변경 정보가 없으면 현재의 이름으로
            mpLocalRenamedAs = AllocRelativePathName();
          target = AllocString(mpLocalRenamedAs);
          r = renameServerObjectAs(TRUE); // FILE_SYNC_SERVER_RENAME_FAIL
          if (r == 0)
          {
            // folder rename is upload done
            d &= ~(FILE_NEED_UPLOAD|FILE_CONFLICT);
          }
          changed = TRUE; // 오류든 성공이든 metafile update
        }
        else
        {
          r = isNeedMoveServerObjectAs() ? FILE_SYNC_NO_MOVE_PERMISSION : FILE_SYNC_NO_RENAME_PERMISSION;
        }
      }
    }
    if (d & FILE_NEED_LOCAL_MOVE) // isServerRenamedAs())
    {
      if (mLocalState & ITEM_STATE_DELETED) // (d & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)
      {
        renameLocalObjectAs(FALSE);
        mLocalState &= ~ITEM_STATE_DELETED;
        r &= ~(FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED);
        d &= ~(FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED);
      }
      else
      {
        if (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD) // no download permission or fail to rename local-file
        {
          if (mpServerRenamedAs == NULL) //이름 변경 정보가 없으면 현재의 이름으로
            mpServerRenamedAs = AllocRelativePathName();
          target = AllocString(mpServerRenamedAs);
          r = renameLocalObjectAs(TRUE);
          if (r == R_SUCCESS)
          {
            // folder rename is download done
            d &= ~(FILE_NEED_DOWNLOAD|FILE_CONFLICT);
          }
          changed = TRUE;
        }
        else
        {
          //r = FILE_SYNC_NO_DOWNLOAD_PERMISSION;
          r = FILE_SYNC_NO_LOCAL_PERMISSION;
        }
      }
    }

    if (r != R_SUCCESS)
    {
      cancel_child_objects = TRUE;
      goto end_sync_copy;
    }
  }

  // in PHASE_MOVE_FOLDERS, we process folder movement only
  if (phase == PHASE_MOVE_FOLDERS)
  {
    mLocalState &= ~ITEM_STATE_MOVED;
    mServerState &= ~ITEM_STATE_MOVED;
    d &= ~(FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE);

    goto end_sync_copy;
  }

  if ((d & FILE_NEED_UPLOAD) &&
    (mParent != NULL))
  {
    StringCchPrintf(msg, 256, _T("path=%s,create_on_server"), GetPathString());
    // MakeFullPathName(mBaseFolder, KMAX_PATH, GetPath(), mLocal.Filename);
    // check if local-folder exist
    //TCHAR fullPath[KMAX_PATH];
    //MakeFullPathName(fullPath, KMAX_PATH, GetPath(), mLocal.Filename);

    // 상위 경로가 바뀔 수 있다.
    // LPTSTR fullPath = AllocFullPathName();
    // LPTSTR fullPath = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
    LPTSTR fullPath = AllocRenamedPathName(AS_LOCAL|AS_FULLPATH);

    if (!IsDirectoryExist(fullPath))
    {
      r = FILE_SYNC_NO_LOCAL_FOLDER_FOUND;
      cancel_child_objects = TRUE;
      delete[] fullPath;
      goto end_sync_copy;
    }
    BOOL able = (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD);
    if (able)
      able = CheckPermissionFolderUpload(mParent->GetServerPermission(), GetServerPermission());

    if (able)
    {
      if (!createServerFolder(mParent, &mpServerConflictMessage))
      {
        r = FILE_SYNC_CREATE_SERVER_FOLDER_FAIL;
      }
      else
      {
        changed = TRUE;
        mServerState = mLocalState = ITEM_STATE_EXISTING;
        d &= ~(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE|FILE_NEED_UPLOAD);

        // 서버측 정보 올바르니 다시 업데이트할 필요 없음. 20171117
        // RefreshServerState();

        StoreHistory(SYNC_H_CREATE_SERVER_FOLDER, fullPath, 1, NULL, mServer.pFullPath);
      }
    }
    else
    {
      StringCchCat(msg, 256, _T(":no_write_server_permission"));
      r = FILE_SYNC_NO_CREATE_FOLDER_PERMISSION;
    }
    delete[] fullPath;

    if (r != R_SUCCESS)
    {
      cancel_child_objects = TRUE;
      goto end_sync_copy;
    }
  }
  else if (d & FILE_NEED_DOWNLOAD)
  {
    StringCchPrintf(msg, 256, _T("path=%s,create_on_local"), GetPathString());
    if (IsOverridePath())
    {
      StringCchCat(msg, 256, _T(", Over:"));
      StringCchCat(msg, 256, mpOverridePath);
    }

    BOOL able = (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD);
    if (able)
      able = CheckPermissionFolderDownload(mParent->GetServerPermission(), GetServerPermission());
    if(able)
    {
      if (mServerState & ITEM_STATE_DELETED)
        r = FILE_SYNC_NO_SERVER_FOLDER_FOUND;
      else
        r = createLocalFolder();

      if (r == R_SUCCESS)
      {
        // 다른 경로로 다운로드한 경우 서버측 정보 클리어해서 새로 추가된 항목으로 처리되도록.
        if ((d & FILE_SYNC_ERR) == FILE_SYNC_NOT_EXIST_DRIVE)
        {
          clearServerSideInfo();
          mServerState = 0;
        }
        else
        {
          RefreshServerState();
          mServerState = ITEM_STATE_EXISTING;
        }

        changed = TRUE;
        mLocalState = ITEM_STATE_EXISTING;
        // ClearOverridePath();

        d &= ~(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE);
        RefreshLocalFileInfo(mUserOid);
        StoreHistory(SYNC_H_CREATE_LOCAL_FOLDER, rootFolder->GetRootPath(), relativePath, 1, mServer.UserOID, NULL);
      }
    }
    else
    {
      StringCchCat(msg, 256, _T(":no_write_local_permission"));
      r = FILE_SYNC_NO_LOCAL_WRITE_PERMISSION;
    }

    if (r != R_SUCCESS)
    {
      cancel_child_objects = TRUE;
      goto end_sync_copy;
    }
  }
  
  // if delete failure, no retire
  deleteMeta = !!(d & (FILE_NEED_RETIRED | FILE_NEED_LOCAL_DELETE | FILE_NEED_SERVER_DELETE));

  if (d & FILE_NEED_RETIRED)
  {
    d &= ~(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE);

    LPTSTR filepathname = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);
    if ((p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD) && (IsFileOrFolder(filepathname) == 1)) 
      d |= FILE_NEED_LOCAL_DELETE;
    delete[] filepathname;

    if ((p->GetSyncFlag() & SYNC_ENABLE_UPLOAD) &&
      IsServerFileExist() && (mServerState & ITEM_STATE_NOW_EXIST))
      d |= FILE_NEED_SERVER_DELETE;
  }

  if (d & FILE_NEED_LOCAL_DELETE)
  {
    StringCchPrintf(msg, 256, _T("path=%s,delete_on_local"), GetPathString());

    if ((mLocalState & ITEM_STATE_DELETED) || !IsLocalFileExist()) 
    {
      changed = TRUE;
    }
    else
    {
      BOOL able = (p->GetSyncFlag() & SYNC_ENABLE_DOWNLOAD);
      if (able)
      {
        r = deleteLocalFolder(baseFolder);
        if (r == R_SUCCESS)
        {
          deleteLocalChildsRecursive(baseFolder, EXCLUDE_OUTGOING);
          changed = TRUE;
          StoreHistory(SYNC_H_DELETE_LOCAL_FOLDER, rootFolder->GetRootPath(), relativePath, 1, NULL, NULL);
        }
      }
      else
      {
        StringCchCat(msg, 256, _T(":no_delete_local_folder_permission"));
        r = FILE_SYNC_NO_DELETE_LOCAL_FOLDER_PERMISSION;
      }
    }
  }
  if (d & FILE_NEED_SERVER_DELETE)
  {
    StringCchPrintf(msg, 256, _T("path=%s,remove_on_server"), GetPathString());
    if ((mServerState & ITEM_STATE_DELETED) || !IsServerFileExist())
    {
      changed = TRUE;
    }
    else
    {
      BOOL able = (p->GetSyncFlag() & SYNC_ENABLE_UPLOAD);
      if (able)
        able = CheckPermissionFolderDelete(mParent->GetServerPermission(), GetServerPermission());
      if (able)
      {
#ifdef USE_BACKUP
        // 백업폴더 삭제요청, 부 폴더가 이미 삭제요청된 경우에는 할 필요가 없겠다.
        if (!IsParentAlreadyRegistDeleteOnServer())
        {
          rootFolder->RegistDeleteOnServer(this);
          AddSyncFlag(SYNC_ITEM_REQUEST_DELET_ONSERVER);
        }
        else
        {
          StringCchCat(msg, 256, _T(" : parent deleted, skip"));
        }
        // 기존 폴더와 병합되는 이동인 경우에, 같이 이동된 하위 파일은 살려두어야 한다.
        SetPhaseRecurse(PHASE_END_OF_JOB, EXCLUDE_CHANGED_ITEM);

        StoreHistory(SYNC_H_DELETE_SERVER_FOLDER, rootFolder->GetRootPath(), relativePath, 1, NULL, NULL);
        r = R_SUCCESS;
        goto clean_rtn;
#else
        r = deleteServerFolder(); // FILE_SYNC_DELETE_SERVER_FOLDER_FAIL
        if (r == R_SUCCESS)
        {
          // delete metafile for all child objects
          deleteLocalChildsRecursive(baseFolder, EXCLUDE_OUTGOING);
          changed = TRUE;
          StoreHistory(SYNC_H_DELETE_SERVER_FOLDER, rootFolder->GetRootPath(), relativePath, 1, NULL, NULL);
        }
        else
        {
          d &= ~FILE_NEED_RETIRED;
          deleteMeta = FALSE;
        }
#endif
      }
      else
      {
        StringCchCat(msg, 256, _T(":no_delete_server_folder_permission"));
        r = FILE_SYNC_NO_DELETE_SERVER_FOLDER_PERMISSION;
      }
    }
  }

end_sync_copy:

  if (d & FILE_NEED_REFRESH_PATH)
  {
    RefreshPathChanged(rootFolder->GetRootPath());
    d &= ~FILE_NEED_REFRESH_PATH;
    r = R_SUCCESS;
    mLocalState &= ~ITEM_STATE_REFRESH_PATH;
  }

  if (d & FILE_NEED_RETIRED)
  {
    deleteMeta = TRUE;
    d &= ~FILE_NEED_RETIRED;
  }
  /*
  if (r == 0)
    r = d;
  */
  r &= ~FILE_ACTION_MASK;
  if (r == R_SUCCESS)
    changed = TRUE;
  SetResult(r);
  SetConflictResult(r);

  if (r == 0)
  {
    mChildResult = 0;
    if (mpServerConflictMessage != NULL)
    {
      delete[] mpServerConflictMessage;
      mpServerConflictMessage = NULL;
    }

    if (IsValidSystemTime(mLocalNew.FileTime) && !IsValidSystemTime(mLocal.FileTime))
      mLocal.FileTime = mLocalNew.FileTime;
    if (IsValidSystemTime(mServerNew.FileTime) && !IsValidSystemTime(mServer.FileTime))
      mServer.FileTime = mServerNew.FileTime;

    FreeServerInfo(mServerNew); // 동기화 완료되면 NewInfo 필요없음.

    if (lstrlen(msg) > 0)
      StringCchCat(msg, 256, _T(":done"));
  }
  else
  {
    if (lstrlen(msg) > 0)
      StringCchCat(msg, 256, _T(":fail"));

    int state = GetHistoryStateFromSyncResult(r, 0);
    StoreHistory(state, rootFolder->GetRootPath(), relativePath, 1, NULL, target);
  }

  int meta_flag = (deleteMeta ? METAFILE_DELETE : METAFILE_CREATE_IF_NOT_EXIST);
  if (r == R_SUCCESS)
    meta_flag = METAFILE_CLEAR_MODIFIED;

  if (changed || deleteMeta || (lstrlen(msg) > 0))
  {
    StoreMetafile(NULL, META_MASK_ALL, meta_flag);
  }

  if (lstrlen(msg) > 0)
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

  if (deleteMeta)
  {
    StringCchPrintf(msg, 256, _T("path=%s,user=%s,retired"), GetPathString(), mLocal.UserOID);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);

    // retire child object also
    RetireChildObjects(rootFolder->GetRootPath());
  }
  else
  {
#ifdef _NEED_ICON_UPDATE // dont need update icon
    LPCTSTR pathName = AllocAbsolutePathName(GetBaseFolder());
    gpCoreEngine->ShellNotifyIconChangeOverIcon(pathName, FALSE);
    delete[] pathName;
#endif
  }

  // if sync job failed, skip all children objects
  if (cancel_child_objects)
  {
    // 서버쪽 폴더에 권한이 있으나 폴더를 만들지 못하는 이슈
    // set all children items phase as done-conflict.
    // There is no need to process children items when sync-error occured on parent folder.
    
    //SetConflictResultRecurse(0, PHASE_CONFLICT_FILES, 0);
    //SetConflictResultRecurse(FILE_SYNC_PARENT_FOLDER_FAIL, PHASE_CONFLICT_FILES, 0);

    // 하위 항목의 충돌상태를 유지하도록, 충돌 상태는 바꾸지 않고 phase만 바꾼다.(20160531)
    SetPhaseRecurse(PHASE_CONFLICT_FILES, 0);
  }
#ifdef USE_BACKUP
clean_rtn:
#endif
  if (target != NULL)
    delete[] target;
  if (relativePath != NULL)
    delete[] relativePath;

  return r;
}

BOOL KSyncFolderItem::OnDeleteServerItem(BOOL r, LPCTSTR pError)
{
  KSyncItem::OnDeleteServerItem(r, pError);

  if (r)
  {
    KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)GetRootFolderItem();
    RetireChildObjects(rootFolder->GetRootPath());
  }
  return TRUE;
}

BOOL KSyncFolderItem::isNeedMoveServerObjectAs()
{
  TCHAR changedName[KMAX_PATH];
  TCHAR changedLocation[KMAX_PATH];
  SplitPathName(mpLocalRenamedAs, changedLocation, changedName);

  TCHAR parentOID[MAX_OID];
  TCHAR changedParentOID[MAX_OID];
  TCHAR folderName[KMAX_PATH];
  if (gpCoreEngine->GetFolderInfo(mServerOID, parentOID, folderName))
  {
    KSyncRootFolderItem *root = (KSyncRootFolderItem *)GetRootFolderItem();
    root->GetFolderOIDCascade(changedLocation, changedParentOID);

    if (StrCmp(changedParentOID, parentOID) != 0)
      return TRUE;
  }
  return FALSE;
}

#define CHECK_NAME_ON_SERVER_BEFORE_RENAME

int KSyncFolderItem::renameServerObjectAs(BOOL renameObject)
{
#ifdef USE_SYNC_ENGINE
  // local file was moved/renamed by user.
  // so, we have to move/rename on server location
  int success = 0;
  KSyncRootFolderItem *root = (KSyncRootFolderItem *)GetRootFolderItem();

  if (renameObject)
  {
    TCHAR changedName[KMAX_PATH];
    TCHAR changedLocation[KMAX_PATH];
    SplitPathName(mpLocalRenamedAs, changedLocation, changedName);

    TCHAR changedParentOID[MAX_OID];
    TCHAR currentParentOID[MAX_OID];
    TCHAR folderName[KMAX_PATH];
    KSyncFolderItem *serverCurrentParent = (KSyncFolderItem *)mParent;

#ifdef CHECK_NAME_ON_SERVER_BEFORE_RENAME
    if (gpCoreEngine->GetFolderInfo(mServerOID, currentParentOID, folderName))
#else
    // do not query document name again
    StringCchCopy(folderName, KMAX_PATH, GetServerFileName());
#endif
    {
      // find parent by mLocalRenamedAs
      KSyncFolderItem *newParent = root->GetFolderOIDCascade(changedLocation, changedParentOID);

      if (mNewParent != NULL)
        serverCurrentParent = (KSyncFolderItem *)mNewParent;

      if ((newParent != NULL) && StrCmp(changedParentOID, currentParentOID) != 0)
      {
        TCHAR currentName[KMAX_PATH];
        TCHAR tempName[KMAX_PATH];
        BOOL restored_on_end = FALSE;
        StringCchCopy(currentName, KMAX_PATH, GetServerFileName());

        // check if new-parent-folder has same named child
        if (newParent->CheckIfChildNameExist(currentName, this, 0, FALSE))
        {
          int r = newParent->CheckIfChildNameExist(changedName, this, 1, FALSE);

          if (r == 2) // 동일한 이름이 존재하고 그것이 바뀌지 않는 것이면 임시 이름 바꾸기로 해결되지 못하니 충돌로 처리함
          {
            return FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS; // FILE_SYNC_ALREADY_EXISTS_ON_SERVER;
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

          LPCTSTR server_path = GetServerRelativePath();
          TCHAR msg[512] = _T("Server Folder '");
          StringCchCat(msg, 512, GetDisplayPathName(server_path));
          StringCchCat(msg, 512, _T("' renamed temporarily '"));
          StringCchCat(msg, 512, tempName);
          StringCchCat(msg, 512, _T("'"));

          if (gpCoreEngine->ChangeFolderName(mServerOID, GetServerFullPathIndex(), currentName, tempName, &mpServerConflictMessage))
          {
            if (mpLocalRenamedAs != NULL)
              delete[] mpLocalRenamedAs;
            mpLocalRenamedAs = AllocMakeFullPathName(changedLocation, tempName);

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
          }
          else
          {
            StringCchCat(msg, 512, _T(" failure"));
            StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
            return FILE_SYNC_NO_MOVE_PERMISSION;
          }
        }

        // do not refresh server info, just use current state.
        /*
        TCHAR oldpath[KMAX_PATH];
        TCHAR newpath[KMAX_PATH];
        TCHAR newpathIndex[KMAX_PATHINDEX];
        int srcParentPermission = 0;
        int destParentPermission = 0;
        if (!gpCoreEngine->GetServerFolderSyncInfo(mParent->mServerOID, &srcParentPermission, NULL, oldpath, NULL, NULL))
          StringCchCopy(oldpath, KMAX_PATH, mParent->mServerOID);

        if (!gpCoreEngine->GetServerFolderSyncInfo(changedParentOID, &destParentPermission, NULL, newpath, newpathIndex, NULL))
          StringCchCopy(newpath, KMAX_PATH, changedParentOID);
        */

        TCHAR msg[512] = _T("Server Folder '");
        StringCchCat(msg, 512, GetFilename());
        StringCchCat(msg, 512, _T("' moved '"));

        // StringCchCat(msg, 512, serverCurrentParent->mServer.pFullPath);
        LPTSTR pServerPath = serverCurrentParent->AllocServerSidePathName();
        StringCchCat(msg, 512, GetDisplayPathName(pServerPath));
        delete[] pServerPath;

        StringCchCat(msg, 512, _T("' to '"));

        // StringCchCat(msg, 512, ((newParent->mServer.pFullPath == NULL) ? _T("\\") : newParent->mServer.pFullPath));
        pServerPath = newParent->AllocServerSidePathName();
        StringCchCat(msg, 512, GetDisplayPathName(pServerPath));
        delete[] pServerPath;

        StringCchCat(msg, 512, _T("'"));

        BOOL able = CheckPermissionFolderMove(serverCurrentParent->GetServerPermission(), newParent->GetServerPermission(), GetServerPermission());

        if (able && (gpCoreEngine->MoveItemTo(mServerOID, changedParentOID, newParent->GetServerFullPathIndex(), TRUE, &mpServerConflictMessage) == R_SUCCESS))
        {
          StringCchCat(msg, 512, _T(" : success"));
          success = 1;

          // change new pathname
#if 0   // bug, no need path
          TCHAR newpath[KMAX_PATH];
          LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
          StringCchCopy(newpath, KMAX_PATH, rootPath);
          MakeFullPathName(newpath, KMAX_PATH, newpath, mpLocalRenamedAs);
          SplitPathNameAlloc(newpath, /*mLocal.FullPath*/NULL, &mLocal.pFilename);
#endif
          SplitPathNameAlloc(mpLocalRenamedAs, /*mLocal.FullPath*/NULL, &mLocal.pFilename);

          // need update server info, and clear ServerNew
          RefreshServerState();
          FreeServerInfo(mServerNew);
        }
        else
        {
          StringCchCat(msg, 512, _T(" : failure"));
          if (!able)
            StringCchCat(msg, 512, _T(", no_permission"));
        }

        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        if (!able)
        {
          return FILE_SYNC_NO_MOVE_PERMISSION;
        }
      }
    }
#ifdef CHECK_NAME_ON_SERVER_BEFORE_RENAME
    else
    {
      TCHAR msg[512] = _T("Server Folder '");
      StringCchCat(msg, 512, mServerOID);
      StringCchCat(msg, 512, _T(" unaccessable"));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      return FILE_SYNC_NO_RENAME_PERMISSION;
    }
#endif

    if (StrCmpI(changedName, folderName) != 0)
    {
      TCHAR msg[512] = _T("Server Folder '");
      StringCchCat(msg, 512, mServerOID);
      StringCchCat(msg, 512, _T("' renamed as'"));
      StringCchCat(msg, 512, changedName);
      StringCchCat(msg, 512, _T("'"));

      BOOL able = CheckPermissionFolderRename(mParent->GetServerPermission(), GetServerPermission());

      if (able)
      {
        BOOL restored_on_end = FALSE;
        TCHAR tempRename[KMAX_PATH];

        if (((KSyncFolderItem*)mParent)->CheckIfChildNameExist(changedName, this, 0, FALSE))
        {
          if (((KSyncFolderItem*)mParent)->CheckIfChildNameExist(changedName, this, 1, FALSE) == 2)
          {
            StoreLogHistory(_T(__FUNCTION__), _T("Error : Same name exist"), SYNC_MSG_LOG);
            return FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS;
          }

          LPTSTR changed = ReadyUniqueLocalName(changedName);
          if (changed != NULL)
          {
            StringCchCopy(tempRename, KMAX_PATH, changedName);
            StringCchCopy(changedName, KMAX_PATH, changed);
            restored_on_end = TRUE;
          }
        }

        if (gpCoreEngine->ChangeFolderName(mServerOID, GetServerFullPathIndex(), folderName, changedName, &mpServerConflictMessage))
        {
          StringCchCat(msg, 512, _T(" : success"));
          success |= 2;

          // change new filename
          AllocCopyString(&mLocal.pFilename, changedName);

          if (restored_on_end)
            SetServerTemporaryRenamed(tempRename);
        }
      }
      else
      {
        StringCchCat(msg, 512, _T(" : failure"));
        if (!able)
          StringCchCat(msg, 512, _T(", no_permission"));
      }

      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      if (!able)
      {
        return FILE_SYNC_NO_RENAME_PERMISSION;
      }
    }
  }
  else
  {
    success = 3;
    
    TCHAR newpath[KMAX_PATH];
    LPCTSTR rootPath = ((KSyncFolderItem *)mParent)->GetRootPath();
    MakeFullPathName(newpath, KMAX_PATH, rootPath, mpLocalRenamedAs);
    SplitPathNameAlloc(newpath, /*mLocal.FullPath*/NULL, &mLocal.pFilename);
  }

  LPTSTR orgRelativePath = AllocString(mpRelativePath);

  if (success & 1)
  {
    // 자신의 이름을 포함한 경로명
    root->MoveSyncItem(this, mpLocalRenamedAs, TRUE);
  }

  if (success)
  {
    // move/rename ifo metafile
    MoveMetafileAsRename(orgRelativePath, 0, root->GetStorage());

    // update fmaplist.txt
    root->UpdateMetaFolderNameMapping(orgRelativePath, GetBaseFolder(), TRUE);

    // remove from KRenameStorage
    // 폴더의 이동 정보를 삭제하면 하위 항목 처리할 때 제대로된 renameAs를 찾지 못함.
    // root->RemoveMovedRenamedOriginal(mpLocalRenamedAs);
    // 대신에 플래그를 클리어한다.
    root->ClearRenameFlag(NULL, mpLocalRenamedAs);

    if (mpServerRenamedAs != NULL)
    {
      delete[] mpServerRenamedAs;
      mpServerRenamedAs = NULL;
    }
    if (mpLocalRenamedAs != NULL)
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
    if (mpServerConflictMessage != NULL)
    {
      delete[] mpServerConflictMessage;
      mpServerConflictMessage = NULL;
    }

    mLocalState &= ~ITEM_STATE_MOVED;
    mServerState &= ~ITEM_STATE_MOVED;

    mServerMoveToOID[0] = '\0';

    // update serverFullPath, pathIndex
    if (renameObject)
    {
      StoreHistory(SYNC_H_RENAME_SERVER_FOLDER, root->GetRootPath(), orgRelativePath, 1, NULL, mpRelativePath);

      RefreshServerState();
      FreeServerInfo(mServerNew);
    }
    else
    {
      clearServerSideInfo();
    }

    FreeLocalInfo(mLocalNew);
    // change mBaseFolder
    SetRelativePathName(mpRelativePath);

    StoreMetafile(root->GetStorage(), META_MASK_ALL, METAFILE_CLEAR_MODIFIED);

    if (!IsServerTemporaryRenamed())
    {
      // update path for all child items
      BOOL refreshNow = TRUE; // (root->mPhase == PHASE_CONFLICT_FILES);
      gpCoreEngine->SendMessageToMain(WM_SYNC_NOTIFY, SYNC_BUSY_START, (LPARAM)0);
      UpdateChildItemPath(root, 1, orgRelativePath, GetBaseFolder(), refreshNow);
      gpCoreEngine->SendMessageToMain(WM_SYNC_NOTIFY, SYNC_BUSY_END, (LPARAM)0);
    }
  }

  if (orgRelativePath != NULL)
  {
    delete[] orgRelativePath;
    orgRelativePath = NULL;
  }

  if (success)
    return R_SUCCESS;

#endif
  return FILE_SYNC_SERVER_RENAME_FAIL;
}

int KSyncFolderItem::renameLocalObjectAs(BOOL renameObject)
{
#ifdef USE_SYNC_ENGINE
  LPTSTR oldpathname = NULL;
  LPTSTR newpathname = NULL;

  LPCTSTR relativePath;
  if ((mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
    relativePath = mpLocalRenamedAs;
  else
    relativePath = GetBaseFolder();
  oldpathname = AllocPathName(((KSyncFolderItem *)mParent)->GetRootPath(), relativePath);

  KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)GetRootFolderItem();

  if ((mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    // relative path for new-parent now
    if ((mNewParent != NULL) && (mNewParent != mParent))
    {
      TCHAR newRelativePath[KMAX_PATH] = { 0 };
      if (rootFolder->GetItemRelativePath(mNewParent, newRelativePath)) // TODO : change to long-filename
      {
        TCHAR name[KMAX_FILENAME];
        SplitPathName(mpServerRenamedAs, NULL, name);
        if (mpServerRenamedAs != NULL)
          delete[] mpServerRenamedAs;
        mpServerRenamedAs = AllocPathName(newRelativePath, name);
      }
    }

#if 0
    // check valid path on mServerRenamedAs
    TCHAR newRelativePath[KMAX_PATH] = { 0 };
    TCHAR newRelativeName[KMAX_PATH] = { 0 };
    SplitPathName(mServerRenamedAs, newRelativePath, newRelativeName);

    if ((lstrlen(newRelativePath) > 0) && (lstrlen(newRelativeName) > 0) &&
      rootFolder->GetValidLocalRenamePath(newRelativePath))
      MakeFullPathName(mServerRenamedAs, KMAX_PATH, newRelativePath, newRelativeName);
#endif
    newpathname = AllocPathName(((KSyncFolderItem *)mParent)->GetRootPath(), mpServerRenamedAs);
  }
  else
  {
    newpathname = AllocSyncResultName(AS_FULLPATH);
  }

  if(StrCmpI(oldpathname, newpathname) == 0)
  {
    // changed only FullPathIndex, no need actual move
    renameObject = FALSE;

    TCHAR msg[300] = _T("Moved/Renamed as same : ");
    StringCchCat(msg, 300, newpathname);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  int success = 0;
  BOOL restored_on_end = FALSE;
  int last_err = 0;

  if (renameObject)
  {
    // check if same named folder exist.
    TCHAR changedName[KMAX_FILENAME];
    LPTSTR changedPath = new TCHAR[lstrlen(newpathname) + 1];

    SplitPathName(newpathname, changedPath, changedName);
    if (IsDirectoryExist(newpathname))
    {
      LPTSTR relativeChangedPath = AllocRelativePathName(changedPath);
      KSyncFolderItem* newParent = (KSyncFolderItem*)rootFolder->FindChildItemCascade(relativeChangedPath, 3);
      delete[] relativeChangedPath;
      if ((newParent != NULL) && 
        (newParent->CheckIfChildNameExist(changedName, NULL, 2, TRUE) == 2))
      {
        TCHAR msg[300] = _T("Already same folder exist on : ");
        StringCchCat(msg, 300, newParent->mpRelativePath);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        delete[] changedPath;
        return FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS;
      }

      LPTSTR changed = ReadyUniqueLocalName(newpathname);
      if (changed != NULL)
      {
        delete[] newpathname;
        newpathname = changed;
        restored_on_end = TRUE;
      }
    }
    delete[] changedPath;

    StoreLogHistory(_T(__FUNCTION__), newpathname, SYNC_MSG_LOG);
    // store temporarily and clear it on sync-end
    // rootFolder->OnStoreCancelRenamed(oldpathname, newpathname);
    rootFolder->AddSyncingFileItem(oldpathname);

    last_err = KMoveFile(oldpathname, newpathname);
    if (last_err == 0)
    {
      success = 1;
      if (restored_on_end)
        SetLocalTemporaryRenamed(changedName);
    }
  }
  else
  {
    success = 1;
  }

  if (success)
  {
    LPTSTR changedRelative = NULL;
    LPTSTR orgRelativePath = NULL;
    LPTSTR movePath = NULL;

    if (renameObject)
    {
      changedRelative = AllocRelativePathName(newpathname);
      orgRelativePath = AllocString(mpRelativePath);

      SplitPathNameAlloc(changedRelative, &movePath, &mLocal.pFilename);

      // change location in object tree
      rootFolder->MoveSyncItem(this, movePath, FALSE);

      // move metafile to new location
      MoveMetafileAsRename(orgRelativePath, 0, rootFolder->GetStorage());

      // update fmaplist.txt
      rootFolder->UpdateMetaFolderNameMapping(orgRelativePath, GetBaseFolder(), TRUE);
    }

    // change mpRelativePath
    // SetRelativePathName(changedRelative);

    if (mpServerRenamedAs != NULL)
    {
      delete[] mpServerRenamedAs;
      mpServerRenamedAs = NULL;
    }
    if (mpLocalRenamedAs != NULL)
    {
      delete[] mpLocalRenamedAs;
      mpLocalRenamedAs = NULL;
    }
    if (mpServerConflictMessage != NULL)
    {
      delete[] mpServerConflictMessage;
      mpServerConflictMessage = NULL;
    }
    mLocalState &= ~ITEM_STATE_MOVED;
    mServerState &= ~ITEM_STATE_MOVED;

    // RefreshServerState();
    UpdateServerNewInfo();

    StoreMetafile(rootFolder->GetStorage());

    // change path of child items on each metafiles.
    // TODO : should update child folder's fullPathIndex
    if (!IsLocalTemporaryRenamed())
    {
      BOOL refreshNow = TRUE; // (rootFolder->mPhase == PHASE_CONFLICT_FILES);
      gpCoreEngine->SendMessageToMain(WM_SYNC_NOTIFY, SYNC_BUSY_START, (LPARAM)0);
      UpdateChildItemPath(rootFolder, 1, orgRelativePath, changedRelative, refreshNow);
      gpCoreEngine->SendMessageToMain(WM_SYNC_NOTIFY, SYNC_BUSY_END, (LPARAM)0);

      // 전체 대상중 mpServerRenamedAs 가 orgRelativePath를 포함하고 있으면 이 부분을 changedRelative으로 변경
      // 폴더가 'A/B'에서 /A/BB'로 변경되었고, 파일이 'A/a.txt'가 'A/B/a.txt'로 바뀐 상태에서 폴더 처리되면 'A/B/a.txt'를 'A/BB/a.txt'로 업데이트하도록 처리함.
      rootFolder->UpdateServerRenamedAs(orgRelativePath, changedRelative);
    }

    // update renamed.txt
    rootFolder->UpdateRenameParentFolder(orgRelativePath, changedRelative);

    StoreHistory(SYNC_H_RENAME_LOCAL_FOLDER, oldpathname, 1, NULL, GetBaseFolder());

    if (orgRelativePath != NULL)
      delete[] orgRelativePath;
    if (changedRelative != NULL)
      delete[] changedRelative;
    if (movePath != NULL)
      delete[] movePath;

    if (oldpathname != NULL)
      delete[] oldpathname;
    if (newpathname != NULL)
      delete[] newpathname;
    return R_SUCCESS;
  }

  //int last_err = GetLastError();
  TCHAR msg[600];
  StringCchPrintf(msg, 600, _T("MoveFolder(%s, %s) fail, last_error = %d"), oldpathname, newpathname, last_err);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  // TODO : last_error=03이 발생했는데도 동기화 오류로 집계되지 않는 문제 있음.
  if (oldpathname != NULL)
    delete[] oldpathname;
  if (newpathname != NULL)
    delete[] newpathname;

  int rtn;
  if (last_err == ERROR_ACCESS_DENIED) // 5, access denied
    rtn = FILE_SYNC_LOCAL_ACCESS_DENIED;
  else if (last_err == ERROR_ALREADY_EXISTS) // 183, already exist
    rtn = FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS;
  else if (last_err == ERROR_SHARING_VIOLATION) // 32
    rtn = FILE_SYNC_LOCAL_RENAME_SHARING_VIOLATION;
  else
    rtn = FILE_SYNC_LOCAL_RENAME_FAIL;

  int state = GetHistoryStateFromSyncResult(rtn, 0);
  StoreHistory(state, rootFolder->GetRootPath(), mpRelativePath, 1, NULL, NULL);
  return rtn;
#endif
  return FILE_SYNC_LOCAL_RENAME_FAIL;
}

BOOL KSyncFolderItem::IsExistServerRenamed(LPCTSTR relativeName)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *(it++);
    if ((item->mpServerRenamedAs != NULL) &&
      (lstrlen(item->mpServerRenamedAs) > 0) && 
      (StrCmp(item->mpServerRenamedAs, relativeName) == 0))
      return TRUE;

    if (item->IsFolder())
    {
      BOOL r = ((KSyncFolderItem *)item)->IsExistServerRenamed(relativeName);
      if (r)
        return TRUE;
    }
  }
  return FALSE;
}

BOOL KSyncFolderItem::IsOutGoingChild(KSyncItem *c)
{
  if (c->isLocalRenamedAs())
  {
    LPTSTR filename = c->AllocSyncResultName(AS_LOCAL);
    BOOL isChild = IsChildFolderOrFile(GetBaseFolder(), filename, FALSE);
    delete[] filename;
    if (!isChild)
      return TRUE;
  }
  if (c->isServerRenamedAs())
  {
    LPTSTR filename = c->AllocSyncResultName(0);
    BOOL isChild = IsChildFolderOrFile(GetBaseFolder(), filename, FALSE);
    delete[] filename;
    if (!isChild)
      return TRUE;
  }
  return FALSE;
}

BOOL KSyncFolderItem::createServerFolder(KSyncItem *parentItem, LPTSTR* OUT pErrorMsg)
{
#ifdef USE_SYNC_ENGINE

#ifdef _DEBUG_CREATE_FOLDER_FAIL
  if (mLocal.Filename[0] == 'A')
  {
    if (pErrorMsg != NULL)
      StringCchCopy(pErrorMsg, 256, _T("Unknown create-foder failure"));
    return FALSE;
  }
#endif

  if (lstrlen(parentItem->mServerOID) <= 0)
    return FALSE;

  KSyncRootFolderItem *root =  (KSyncRootFolderItem *)GetRootFolderItem();
  if( gpCoreEngine->SyncServerCreateFolder(root, parentItem->mServerOID, parentItem->GetServerFullPathIndex(),
    GetFilename(), mServerOID, mServerNew, pErrorMsg) )
  {
    if ((mServerNew.Permissions == 0) || (lstrlen(mServerNew.pFullPathIndex) == 0))
    {
      /*
      BOOL rtn = gpCoreEngine->GetServerFolderSyncInfo(mServerOID, &mServerNew.Permissions, 
        mServerNew.UserOID, mServerNew.FullPath, mServerNew.FullPathIndex, &mServerNew.FileTime);
      */
      FreeServerInfo(mServerNew);
      BOOL rtn = gpCoreEngine->GetServerFolderSyncInfo(root, mServerOID, mServerNew);
    }

    AllocCopyString(&mServerNew.pFilename, mLocal.pFilename);

    StringCchCopy(mLocal.UserOID, MAX_OID, mServerNew.UserOID);
    StringCchCopy(mServer.UserOID, MAX_OID, mServerNew.UserOID);
    addServerState(ITEM_STATE_NOW_EXIST);
    CopyServerInfo(mServer, mServerNew);

    TCHAR msg[600];
    StringCchPrintf(msg, 600, _T("createServerFolder %s"), mServerNew.pFullPath);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return TRUE;
  }
  else
  {
    TCHAR msg[600];
    StringCchPrintf(msg, 600, _T("createServerFolder fail : %s"), mServerNew.pFullPath);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
#endif
  return FALSE;
}

int KSyncFolderItem::createLocalFolder()
{
  int r = R_SUCCESS;

#ifdef SYNC_ALL_DRIVE
  LPCTSTR pathName = mpRelativePath;
  if (IsOverridePath())
    pathName = mpOverridePath;

  //if (GetDepth() == 1) // for drive items
  if (IsDrivePath(pathName))
  {
    if (!gpCoreEngine->IsTargetDrive(pathName))
    {
      // 하위 항목도 모두 오류로
      SetConflictResultPhase(FILE_SYNC_NOT_EXIST_DRIVE, PHASE_ENDING, 
        SET_WITH_ITSELF|SET_LOCAL_DELETED|SET_WITH_RECURSIVE|STORE_META);
      return FILE_SYNC_NOT_EXIST_DRIVE;
    }
    // 성공한 것으로 간주
    return r;
  }
  if ((mPhase == PHASE_CONFLICT_FILES) && !gpCoreEngine->IsTargetDrive(pathName))
  {
    // 하위 항목도 모두 오류로
    SetConflictResultPhase(FILE_SYNC_NOT_EXIST_DRIVE, PHASE_ENDING, 
      SET_WITH_ITSELF|SET_LOCAL_DELETED|SET_WITH_RECURSIVE|STORE_META);
    return FILE_SYNC_NOT_EXIST_DRIVE;
  }
#else
  LPCTSTR pathName = AllocFullPathName();
#endif

  if (!IsDirectoryExist(pathName))
  {
    if (CreateDirectory(pathName))
    {
      StringCchCopy(mLocal.UserOID, MAX_OID, mServerNew.UserOID);
      GetFileModifiedTime(pathName, &mLocal.FileTime, &mLocal.AccessTime);
      mLocal.FileSize = 0;
      CopyServerInfo(mServer, mServerNew);
    }
    else
    {
      TCHAR msg[256];

      int err = GetLastError();
      StringCchPrintf(msg, 256, _T("createLocalFolder failure on %s, error=%d"), pathName, err);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      r = (err == 5) ? FILE_SYNC_NO_LOCAL_PERMISSION : FILE_SYNC_CREATE_LOCAL_FOLDER_FAIL;
    }
  }
#ifndef SYNC_ALL_DRIVE
  delete[] pathName;
#endif
  return R_SUCCESS;
}

int KSyncFolderItem::deleteLocalChildsRecursive(LPCTSTR baseFolder, int flag)
{
  int r = R_SUCCESS;
  KSyncItemIterator it = mChildList.begin();

  // TODO : should use KSyncRootStorage

  while (it != mChildList.end())
  {
    int d = R_SUCCESS;
    KSyncItem *c = *it;

    if (flag & EXCLUDE_OUTGOING)
    {
      if (IsOutGoingChild(c))
        d = 1;
      /*
      TCHAR filename[KMAX_PATH];
      if (c->isLocalRenamedAs())
      {
        c->GetSyncResultRName(filename, KMAX_PATH, AS_LOCAL);
        if (!IsChildFolderOrFile(GetBaseFolder(), filename, FALSE))
          d = 1;
      }
      if (c->isServerRenamedAs())
      {
        c->GetSyncResultRName(filename, KMAX_PATH, 0);
        if (!IsChildFolderOrFile(GetBaseFolder(), filename, FALSE))
          d = 1;
      }
      */
    }

    if (d == R_SUCCESS)
    {
      if (c->IsFolder())
      {
        d = ((KSyncFolderItem *)c)->deleteLocalChildsRecursive(baseFolder, flag);
      }
      else
      {
        if (flag & DELETE_LOCAL_FILES)
        {
          LPTSTR fullname = c->AllocFullPathName();
          d = gpCoreEngine->SyncLocalDeleteFile(fullname);
          delete[] fullname;
        }
        if (d == R_SUCCESS)
          c->StoreMetafile(NULL, 0, METAFILE_DELETE);
      }
      if (d == R_SUCCESS)
      {
        it = mChildList.erase(it);
        delete c;
      }
      else
      {
        // failed item is out of sync
        c->SetPhase(PHASE_END_OF_JOB);
        r = R_FAIL_DELETE_CHILDS;
      }
    }
    if (d != R_SUCCESS)
      ++it;
  }

  if (r == R_SUCCESS)
  {
    if (flag & DELETE_LOCAL_FILES)
    {
      LPTSTR fullpath = AllocFullPathName();
      r = gpCoreEngine->SyncLocalDeleteFolder(fullpath);
      delete[] fullpath;
    }
    if (r == R_SUCCESS)
      StoreMetafile(NULL, 0, METAFILE_DELETE);
  }
  return r;
}

int KSyncFolderItem::deleteLocalFolder(LPCTSTR rootFolder)
{
#ifdef USE_SYNC_ENGINE
  LPTSTR fullname = AllocSyncResultName(AS_LOCAL|AS_FULLPATH);

  int r = gpCoreEngine->SyncLocalDeleteFolder(fullname);
  /*
  if (r != R_SUCCESS)
    r = deleteLocalChildsRecursive(rootFolder, DELETE_LOCAL_FILES);
  */

  if (r != R_SUCCESS)
  {
    TCHAR msg[600];
    int err = GetLastError();
    StringCchPrintf(msg, 600, _T("deleteLocalFolder failure on %s, error=%d"), fullname, err);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    r = FILE_SYNC_CHILD_DELETE_FAIL;
  }
  delete[] fullname;
  return r;
#else
  return R_SUCCESS;
#endif
}

int KSyncFolderItem::deleteServerFolder()
{
#ifdef USE_SYNC_ENGINE
  if (!gpCoreEngine->SyncServerDeleteFolder(mServerOID, GetServerFullPathIndex(), &mpServerConflictMessage))
    return FILE_SYNC_DELETE_SERVER_FOLDER_FAIL;
  return R_SUCCESS;
#else
  return FALSE;
#endif
}

BOOL KSyncFolderItem::RefreshServerState()
{
#ifdef USE_SYNC_ENGINE
  if (lstrlen(mServerOID) > 0)
  {
    FreeServerInfo(mServer);
    KSyncRootFolderItem *root = (KSyncRootFolderItem *)GetRootFolderItem();
    if (root == this)
      root = NULL;
    return (gpCoreEngine->GetServerFolderSyncInfo(root, mServerOID, mServer) == 0);
  }
#endif
  return FALSE;
}

int KSyncFolderItem::RefreshServerNewState()
{
#ifdef USE_SYNC_ENGINE
  if (lstrlen(mServerOID) > 0)
  {
    FreeServerInfo(mServerNew);

    KSyncRootFolderItem *root = (KSyncRootFolderItem *)GetRootFolderItem();
    if (root == this)
      root = NULL;
    int r = gpCoreEngine->GetServerFolderSyncInfo(root, mServerOID, mServerNew);
    return ((r == R_SUCCESS) ? R_SUCCESS : ITEM_STATE_DELETED);
  }
#endif
  return 0;
}

void KSyncFolderItem::CloneLocalRename(KSyncRootFolderItem *root, KSyncItem *pair)
{
  KSyncItem::CloneLocalRename(root, pair);

  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    (*it)->SetLocalPath(GetBaseFolder(), root->GetRootPath(), 1);
    KSyncItem *child = NULL;

    if (pair != NULL)
    {
      child = (*it)->DuplicateItem();

      child->SetParent(pair);
      child->SetLocalPath(((KSyncFolderItem *)pair)->GetBaseFolder(), root->GetRootPath(), 1);
      ((KSyncFolderItem *)pair)->AddChild(child);
    }

    (*it)->CloneLocalRename(root, child);
    ++it;
  }
}

void KSyncFolderItem::CloneServerRename(KSyncRootFolderItem *root, KSyncItem *pair)
{
  KSyncItem::CloneServerRename(root, pair);

  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    (*it)->SetLocalPath(GetBaseFolder(), root->GetRootPath(), 1);
    KSyncItem *child = NULL;
    if (pair != NULL)
    {
      child = (*it)->DuplicateItem();

      child->SetParent(pair);
      child->SetLocalPath(((KSyncFolderItem *)pair)->GetBaseFolder(), root->GetRootPath(), 1);
      ((KSyncFolderItem *)pair)->AddChild(child);
    }

    (*it)->CloneServerRename(root, child);
    ++it;
  }
}

void KSyncFolderItem::CloneSeparate(KSyncRootFolderItem *root, KSyncItem *pair)
{
  KSyncItem::CloneSeparate(root, pair);

  if (pair != NULL)
  {
    KSyncItemIterator it = mChildList.begin();
    while (it != mChildList.end())
    {
      (*it)->SetLocalPath(GetBaseFolder(), root->GetRootPath(), 1);
      KSyncItem *child = (*it)->DuplicateItem();

      child->SetParent(pair);
      child->SetLocalPath(((KSyncFolderItem *)pair)->GetBaseFolder(), root->GetRootPath(), 1);
      ((KSyncFolderItem *)pair)->AddChild(child);

      (*it)->CloneSeparate(root, child);

      ++it;
    }
  }
}

int KSyncFolderItem::GetCurrentFolder(LPTSTR pathname, int length)
{
  if (mParent == NULL)
  {
    if (pathname != NULL)
      pathname[0] = '\0';
    if (IsSystemRoot())
    {
      return 1;
    }
    else
    {
      LPTSTR path = GetBaseFolder();
      if ((pathname != NULL) && (path != NULL))
        StringCchCopy(pathname, length, path);
      return lstrlen(path);
    }
  }
  else
  {
    LPCTSTR b, f;

    int len = 0;
    if (lstrlen(mpLocalRenamedAs) > 0)
    {
      b = NULL; // GetRootPath(); should be relative path
      f = mpLocalRenamedAs;

      if (f != NULL)
        len += lstrlen(f);
    }
    else
    {
      len = ((KSyncFolderItem *)mParent)->GetCurrentFolder(pathname, length);
      len += lstrlen(mLocal.pFilename);
      b = pathname;
      f = mLocal.pFilename;
    }
    len += 2;

    if (pathname != NULL)
      MakeFullPathName(pathname, length, b, f);
    return len;
  }
}

int KSyncFolderItem::GetConflictCount(BOOL includeDisabled)
{
  int r = 0;
  int count = 0;

  if (includeDisabled || (mPhase < PHASE_END_OF_JOB))
  {
    r = GetConflictResult();
    if (r & (FILE_SYNC_RENAMED|FILE_CONFLICT|FILE_SYNC_ERR))
    {
      // return 1; // 자신이 충돌이면 하위 개체는 카운트하지 않음.
      // 자신이 충돌이어도 하위의 충돌 항목 카운트 함(20160531)
      ++count;
    }
  }

  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      count += ((KSyncFolderItem *)(*it))->GetConflictCount(includeDisabled);
    }
    else
    {
      if (includeDisabled || ((*it)->mPhase < PHASE_END_OF_JOB))
      {
        r = (*it)->GetConflictResult();
        if (r & (FILE_SYNC_RENAMED|FILE_CONFLICT|FILE_SYNC_ERR))
          ++count;
      }
    }
    ++it;
  }
  return count;
}

void KSyncFolderItem::GetConflictCount(int& folderCount, int& fileCount, BOOL includeDisabled)
{
  int r = 0;

  if (includeDisabled || (mPhase < PHASE_END_OF_JOB))
  {
    r = GetConflictResult();
    if (r & (FILE_SYNC_RENAMED|FILE_CONFLICT|FILE_SYNC_ERR))
    {
      // return 1; // 자신이 충돌이면 하위 개체는 카운트하지 않음.
      // 자신이 충돌이어도 하위의 충돌 항목 카운트 함(20160531)
      ++folderCount;
    }
  }

  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      ((KSyncFolderItem *)(*it))->GetConflictCount(folderCount, fileCount, includeDisabled);
    }
    else
    {
      if (includeDisabled || ((*it)->mPhase < PHASE_END_OF_JOB))
      {
        r = (*it)->GetConflictResult();
        if (r & (FILE_SYNC_RENAMED|FILE_CONFLICT|FILE_SYNC_ERR))
          ++fileCount;
      }
    }
    ++it;
  }
}

int KSyncFolderItem::GetTotalCount(int type)
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      count += ((KSyncFolderItem *)(*it))->GetTotalCount(type);
    }
    if ((*it)->IsMatchType(type))
    {
        ++count;
    }
    ++it;
  }
  return count;
}

__int64 KSyncFolderItem::GetTotalSize(BOOL conflictResult)
{
  __int64 total_size = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      total_size += ((KSyncFolderItem *)(*it))->GetTotalSize(conflictResult);
    }
    else
    {
      total_size += (*it)->GetSyncFileSize(conflictResult);
    }
    ++it;
  }
  return total_size;
}

__int64 KSyncFolderItem::GetTotalSizeProgress(BOOL conflictResult)
{
  __int64 total_size = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      total_size += ((KSyncFolderItem *)(*it))->GetTotalSizeProgress(conflictResult) ;
      total_size += EMPTY_FILE_ESTIMATE_SIZE; // for zero file counting
    }
    else
    {
      total_size += (*it)->GetSyncFileSize(conflictResult);
      total_size += EMPTY_FILE_ESTIMATE_SIZE; // for zero file counting
    }
    ++it;
  }
  return total_size;
}

void KSyncFolderItem::GetCandidateCount(int phase, BOOL conflictResult, int &count, __int64 &sizePure, __int64 &sizeProgress)
{
  if ((mPhase < phase) && (mResult != FILE_NOT_CHANGED) && (IsFolder() < 2))
      // ((mConflictResult & (FILE_CONFLICT|FILE_SYNC_ERR)) == 0))
  {
    ++count;
    sizeProgress += EMPTY_FILE_ESTIMATE_SIZE;
  }
  // scan child
  KSyncItemIterator it = mChildList.begin();
  while (it != mChildList.end())
  {
    KSyncItem* item = *it;

    if(!item->IsExcludedSync()) // 제외 폴더는 동기화 카운트에서 제외한다.
    {
      if (item->IsFolder())
      {
        KSyncFolderItem *subfolder = (KSyncFolderItem *)item;
        subfolder->GetCandidateCount(phase, conflictResult, count, sizePure, sizeProgress);
      }
      else
      {
        if ((item->mPhase < phase) && (item->mResult != FILE_NOT_CHANGED))
          //(((*it)->mConflictResult & (FILE_CONFLICT|FILE_SYNC_ERR)) == 0))
        {
          ++count;
          __int64 file_size = item->GetSyncFileSize(conflictResult);
          sizePure += file_size;
          sizeProgress += file_size + EMPTY_FILE_ESTIMATE_SIZE;
        }
      }
    }
    ++it;
  }
}

#if 0
int KSyncFolderItem::GetResultItemCount(int resultMask)
{
  int count = 0;

  if (mConflictResult & resultMask)
  {
    if (mConflictResult & FILE_SYNC_ERR) // if sync_error, just on sync_error
    {
      if (resultMask & FILE_SYNC_ERR)
        ++count;
    }
    else
    {
      if (mConflictResult & resultMask)
        ++count;
    }
  }
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      count += ((KSyncFolderItem *)(*it))->GetResultItemCount(resultMask);
    }
    else
    {
      int r = (*it)->GetConflictResult();
      if (r & FILE_SYNC_ERR)
      {
        if (resultMask & FILE_SYNC_ERR)
          ++count;
      }
      else
      {
        if (r & resultMask)
          ++count;
      }
    }
    ++it;
  }
  return count;
}
#endif

// check if it has modified children or excluded files
BOOL KSyncFolderItem::IsChildDeleteOk(BOOL onLocal)
{
  // 제외된 파일이 있어도 삭제 가능하도록 함.
  /*
  if (mFlag & (SYNC_ITEM_EXCLUDED | SYNC_ITEM_HAS_EXCLUDED))
    return FALSE;
  */

  KSyncItemIterator it = mChildList.begin();
  KSyncItem *child;
  while(it != mChildList.end())
  {
    child = *it;

    // 제외된 파일이 있어도 삭제 가능하도록 함.
    /*
    if (child->mFlag & (SYNC_ITEM_EXCLUDED | SYNC_ITEM_HAS_EXCLUDED))
      return FALSE;
    */

    if (child->IsFolder())
    {
      if(!((KSyncFolderItem *)child)->IsChildDeleteOk(onLocal))
        return FALSE;
    }
    else
    {
      // check if file is modified or added newly
      if (child->IsNeedUpdate(onLocal))
        return FALSE;

      // 파일이 열려 있음.
      if (child->mFlag & SYNC_ITEM_EDITING_NOW)
        return FALSE;
    }
    ++it;
  }
  return TRUE;
}

KSyncFolderItem *KSyncFolderItem::GetRootFolderItem()
{
  if (mResolveFlag == HIDDEN_ROOT_FOLDER)
  {
    KSyncItemIterator it = mChildList.begin();
    KSyncItem *child;
    while(it != mChildList.end())
    {
      child = *it;
      if (child->IsFolder() && !((KSyncFolderItem*)child)->IsDummyCloneFolder())
        return (KSyncFolderItem *)child;
      ++it;
    }
  }

  if (mParent == NULL)
    return this;
  return ((KSyncFolderItem *)mParent)->GetRootFolderItem();
}

LPCTSTR KSyncFolderItem::GetRootPath()
{
#ifdef SYNC_ALL_DRIVE
  if ((lstrlen(mpRelativePath) >= 2) && (mpRelativePath[1] == ':')) // this is DriveItem
  {
    return mpRelativePath;
  }
  else
  {
    if (mParent != NULL)
      return ((KSyncFolderItem *)mParent)->GetRootPath();

    return mpRelativePath;
  }
#else
  KSyncFolderItem *item = GetRootFolderItem();
  if (item != NULL)
    return item->GetRootPath();

#ifdef _DEBUG
  if (lstrlen(mpRelativePath) == 0)
    OutputDebugString(_T("Empty Root Path on GetRootPath()\n"));
#endif
  return GetBaseFolder();
#endif
}

KSyncItem *KSyncFolderItem::FindChildItemCascade(LPCTSTR relativePathName, int matchType, int flag)
{
  if ((relativePathName == NULL) || (lstrlen(relativePathName) == 0))
  {
    if (matchType & IsFolder())
      return this;
    return NULL;
  }

  TCHAR name[KMAX_FILENAME];
  LPTSTR subPath = new TCHAR[lstrlen(relativePathName)+1];
  SplitNameSubPath(relativePathName, name, subPath);

  KSyncItem *c;

  if (lstrlen(subPath) > 0)
  {
    c = FindChildItem(name, !!(flag & WITH_RENAMED));
    if ((c != NULL) && (c->IsFolder() > 0))
        c = ((KSyncFolderItem*)c)->FindChildItemCascade(subPath, matchType, flag);
  }
  else
  {
    // this is leaf node
    c = FindChildItem(name, !!(flag & WITH_RENAMED));
  }
  delete[] subPath;
  return c;

#if 0
  LPTSTR s = StrChr(relativePathName, '\\');
  if (s != NULL)
  {
    *s = '\0';
  }
  KSyncItem *c = FindChildItem(relativePathName, FALSE);

  if (c == NULL)
    c = FindChildItem(relativePathName, TRUE);

  if (c != NULL)
  {
    if ((matchType == 2) || (!!c->IsFolder() == matchType))
    {
      if (c->IsFolder() && (s != NULL))
      {
        c = ((KSyncFolderItem *)c)->FindChildItemCascade(s+1, matchType);
        *s = '\\';
      }
    }
    else
    {
      c = NULL;
    }
  }
  return c;
#endif
}

KSyncItem *KSyncFolderItem::FindRenamedChildItem(LPCTSTR relativePathName, BOOL onLocal, int matchType)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if ((matchType == 2) || (!!item->IsFolder() == matchType))
    {
      if (onLocal && (item->mpLocalRenamedAs != NULL) && 
        (StrCmpI(item->mpLocalRenamedAs, relativePathName) == 0))
        return item;
    
      if (!onLocal && (item->mpServerRenamedAs != NULL) && 
        (StrCmpI(item->mpServerRenamedAs, relativePathName) == 0))
        return item;
    }

    if (item->IsFolder())
    {
      KSyncItem *r = ((KSyncFolderItem *)item)->FindRenamedChildItem(relativePathName, onLocal, matchType);
      if (r != NULL)
        return r;
    }
    ++it;
  }
  return NULL;
}

// 빈 개체들을 순서대로 만들어 넣는다
// REFRESH_LOCALNEW 1
// REFRESH_RENAMED : 로컬측 이름 변경을 확인한다
// SET_SCAN_DONE // 새로 추가된 항목은 스캔 완료된 것으로 설정

KSyncItem *KSyncFolderItem::CreateChildItemCascade(LPCTSTR relativePathName, int matchType, KSyncRootFolderItem* root, int flag, int *created_count)
{
  LPTSTR subPath = new TCHAR[lstrlen(relativePathName) + 1];
  TCHAR name[KMAX_FILENAME];
  SplitNameSubPath(relativePathName, name, subPath);

  KSyncItem* r = findChildByName(name);

  if (r == NULL)
  {
    if ((lstrlen(subPath) > 0) || (matchType > 0))
    {
      KSyncFolderItem* c = new KSyncFolderItem(this, GetBaseFolder(), name);
      AddChild(c);

      if (flag & REFRESH_LOCALNEW)
      {
        c->RefreshLocalNewState(root);
      }
      if (flag & REFRESH_RENAMED)
      {
#ifdef _DEBUG
        TCHAR msg[256] = _T("New : ");
        StringCchCat(msg, 256, c->mpRelativePath);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
        c->CheckLocalRenamedAsByOld(root);
      }
      if (flag & SET_SCAN_DONE)
        c->SetPhase(PHASE_SCAN_LOCAL_FILES);
      if (flag & SET_SCAN_NEED)
        c->SetPhase(PHASE_SCAN_LOCAL_FILES - 1);

      if (created_count != NULL)
        (*created_count)++;

      if (lstrlen(subPath) > 0)
        r = c->CreateChildItemCascade(subPath, matchType, root, flag, created_count);
      else
        r = c;
    }
    else
    {
      r = new KSyncItem(this, GetBaseFolder(), name);
      AddChild(r);

      if (created_count != NULL)
        (*created_count)++;

      if (flag & REFRESH_LOCALNEW)
      {
        r->LoadMetafile(root->GetSafeStorage());
        r->RefreshLocalNewState(root);
      }
      if (flag & REFRESH_RENAMED)
      {
#ifdef _DEBUG
        TCHAR msg[256] = _T("New : ");
        StringCchCat(msg, 256, r->mpRelativePath);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
        r->CheckLocalRenamedAsByOld(root);
      }
    }
  }
  else
  {
    if (flag & REFRESH_LOCALNEW)
    {
      r->RefreshLocalNewState(root);
    }
    if (flag & REFRESH_RENAMED)
    {
      r->CheckLocalRenamedAsByOld(root);
    }

    if ((lstrlen(subPath) > 0) && (r->IsFolder() > 0))
      r = ((KSyncFolderItem*)r)->CreateChildItemCascade(subPath, matchType, root, flag, created_count);
  }

  delete[] subPath;
  return r;
}



void KSyncFolderItem::StoreExcludedSyncCascade(KSyncRootStorage *s, BOOL exclude, int recursive)
{
  SetExcludedSync(exclude);
  s->StoreMetafile(this, META_MASK_FLAG, 0);

  if (recursive)
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem *item = *it;
      if (item->IsFolder())
      {
        ((KSyncFolderItem *)item)->StoreExcludedSyncCascade(s, exclude, recursive);
      }
      ++it;
    }
  }
}

KSyncItem *KSyncFolderItem::FindChildItem(LPCTSTR name, BOOL withRename)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    if (StrCmpI(item->GetFilename(), name) == 0)
    {
      return item;
    }
    else if (withRename && (item->mpLocalRenamedAs != NULL) && (lstrlen(item->mpLocalRenamedAs) > 0))
    {
      TCHAR renamed[KMAX_PATH];
      SplitPathName(item->mpLocalRenamedAs, NULL, renamed);
      if (StrCmpI(renamed, name) == 0)
        return item;
    }
    ++it;
  }
  return NULL;
}

KSyncItem *KSyncFolderItem::FindChildItemByOIDCascade(LPCTSTR serverOID)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if (StrCmp((*it)->mServerOID, serverOID) == 0)
    {
      return *it;
    }
    if ((*it)->IsFolder())
    {
      KSyncItem *c = ((KSyncFolderItem *)(*it))->FindChildItemByOIDCascade(serverOID);
      if (c != NULL)
        return c;
    }
    ++it;
  }
  return NULL;
}

KSyncFolderItem *KSyncFolderItem::GetFolderOIDCascade(LPTSTR pathname, LPTSTR OUT serverOID)
{
  if (lstrlen(pathname) == 0)
  {
    StringCchCopy(serverOID, MAX_OID, mServerOID);
    return this;
  }
  LPTSTR s = StrChr(pathname, '\\');
  if (s != NULL)
  {
    *s = '\0';
  }
  KSyncItem *c = FindChildItem(pathname, FALSE);
  if (c == NULL) // compare with renamed
    c = FindChildItem(pathname, TRUE);

  if (s != NULL)
  {
    *s = '\\';
    s++;
  }

  if ((c != NULL) && c->IsFolder())
  {
    if (s != NULL)
      return ((KSyncFolderItem *)c)->GetFolderOIDCascade(s, serverOID);
    else
    {
      StringCchCopy(serverOID, MAX_OID, c->mServerOID);
      return (KSyncFolderItem *)c;
    }
  }
  return NULL;
}

BOOL KSyncFolderItem::CheckServerFolderExist(BOOL createIfNotExist)
{
#ifdef USE_SYNC_ENGINE
  if (lstrlen(mServerOID) == 0)
  {
    BOOL r = TRUE;
    LPTSTR error_msg = NULL;

    if (mParent != NULL)
      r = ((KSyncFolderItem *)mParent)->CheckServerFolderExist(createIfNotExist);

    if (r && createIfNotExist)
    {
      KSyncRootFolderItem *root =  NULL;
     
      if (mParent != NULL)
        root = (KSyncRootFolderItem *)GetRootFolderItem();

      r = gpCoreEngine->SyncServerCreateFolder(root, mParent->mServerOID,
        mParent->GetServerFullPathIndex(), GetFilename(), mServerOID, mServer, &error_msg);

      if (error_msg != NULL)
        delete[] error_msg;
    }
    return r;
  }
#endif
  return TRUE;
}


// 동기화 진행중 오류 발생하면 상위 폴더가 삭제된 경우인지 체크하여, 
// 삭제되었으면 해당 폴더 더이상 진행되지 않도록 오류 처리.
BOOL KSyncFolderItem::CheckLocalFolderExistOnSyncing()
{
  LPTSTR fullPathName = AllocAbsolutePathName();
  BOOL exist = IsDirectoryExist(fullPathName);
  delete[] fullPathName;

  if (!exist)
  {
    SetConflictResult(FILE_SYNC_NO_LOCAL_FOLDER_FOUND);
    SetResult(FILE_SYNC_NO_LOCAL_FOLDER_FOUND);
    addLocalState(ITEM_STATE_ERROR);

    SetPhase(PHASE_CONFLICT_FILES);
    
    if (mParent != NULL)
      ((KSyncFolderItem *)mParent)->CheckLocalFolderExistOnSyncing();
    return FALSE;
  }
  return TRUE;
}

BOOL KSyncFolderItem::CheckServerFolderExistOnSyncing()
{
  int r = gpCoreEngine->SyncServerCheckFolderExist(mServerOID, mServer.pFullPathIndex, NULL);
  if (r != R_SUCCESS)
  {
    if (r == R_FAIL_SERVER_FOLDER_NOT_EXIST)
    {
      TCHAR msg[256] = _T("Skip NoExist Folder : ");
      StringCchCat(msg, 256, mpRelativePath);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      // 오류 코드가 로그아웃, 오프라인 등의 문제는 스킵하고
      // 폴더 없음인 경우에만 폴더 오류로 처리한다.
      SetConflictResult(FILE_SYNC_NO_SERVER_FOLDER_FOUND);
      SetResult(FILE_SYNC_NO_SERVER_FOLDER_FOUND);
      addServerState(ITEM_STATE_ERROR);

      SetPhase(PHASE_CONFLICT_FILES);
    
      if (mParent != NULL)
        ((KSyncFolderItem *)mParent)->CheckServerFolderExistOnSyncing();
    }
    return FALSE;
  }
  return TRUE;
}

int KSyncFolderItem::GetResolveChildItemCount()
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsResolveVisible())
      ++count;
    ++it;
  }
  return count;
}

KSyncItem *KSyncFolderItem::GetResolveChildItem(int index)
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsResolveVisible())
    {
      if (count == index)
        return *it;
      ++count;
    }
    ++it;
  }
  return NULL;
}

void KSyncFolderItem::GetResolveChildItems(KSyncItemArray &itemArray, int type, int flag)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->IsResolveVisible() && 
      ((type == 2) || (!!item->IsFolder() == !!type)))
    {
      BOOL avail = TRUE;

      if ((flag & EX_DUMMY) && 
        (item->IsFolder() > 0) && ((KSyncFolderItem*)item)->IsDummyCloneFolder())
      {
        avail = FALSE;
      }

      if (flag & IN_SINGLE_DUMMY)
      {
        avail = FALSE;
        if ((item->IsFolder() > 0) && ((KSyncFolderItem*)item)->IsDummyCloneFolder())
        {
          KSyncItem* origin = NULL;
          if (item->mParent != NULL)
            origin = ((KSyncFolderItem*)item->mParent)->GetNormalChildItem(item->GetFilename());
          avail = (origin == NULL) || !origin->IsResolveVisible();
        }
      }

      if (avail)
        itemArray.push_back(*it);

    }
    ++it;
  }
}

int KSyncFolderItem::GetChildItemArray(KSyncItemArray &itemArray, int type, int resultMask, BOOL recursive)
{
  int count = 0;

  if (GetConflictResult() & resultMask)
  {
    itemArray.push_back(this);
    ++count;
  }
  else // if parent is included, skip child objects.
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      if ((*it)->IsFolder())
      {
        if (recursive)
        {
          KSyncFolderItem *sub = (KSyncFolderItem *) *it;
          count += sub->GetChildItemArray(itemArray, type, resultMask, recursive);
        }
      }
      else if ((*it)->IsMatchType(type) && ((*it)->GetConflictResult() & resultMask))
      {
        itemArray.push_back(*it);
        ++count;
      }
      ++it;
    }
  }
  return count;
}

// flag : REMOVE_DISABLED, STORE_META, SET_CONFLICT_CHILD
int KSyncFolderItem::UpdateFolderSyncResult(int depth, KSyncRootFolderItem *rootFolder, int flag) // mBaseFolder LPCTSTR rootFolder)
{
  // 충돌된 폴더 하위의 항목도 충돌 트리에 나타나도록
  if (flag & SET_CONFLICT_CHILD)
  {
    if (mResult != 0)
      AddConflictResult(FILE_SYNC_BELOW_CONFLICT | (mResult & FILE_USER_ACTION_MASK));
  }
  if (mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR))
    flag |= SET_CONFLICT_CHILD;

  if (rootFolder == NULL)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("empty root folder"), SYNC_MSG_LOG);
    return 0;
  }

  KSyncRootStorage *s = rootFolder->GetStorage();
  if ((flag & STORE_META) && (depth == 0))
    s->BeginTransaction();

  /*
  if (flag & CLEAR_NOW_SYNCHRONIZING)
    mResult &= ~FILE_NOW_SYNCHRONIZING;
  */

  // update all child folder
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    if ((*it)->IsFolder())
    {
      ((KSyncFolderItem *)(*it))->UpdateFolderSyncResult(depth + 1, rootFolder, flag);
    }
    ++it;
  }

  int last_child_result = mChildResult;
  int last_result = mResult;

  it = mChildList.begin();
  mChildResult = 0;
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    mChildResult |= (item->mConflictResult & FILE_SYNC_RESOLVE_VISIBLE_FLAG);

    if (item->IsFolder())
    {
      mChildResult |= item->mChildResult;
    }
    else
    {
      // 충돌된 폴더 하위의 항목도 충돌 트리에 나타나도록
      if (flag & SET_CONFLICT_CHILD)
      {
        if (item->mResult != 0)
          item->AddConflictResult(FILE_SYNC_BELOW_CONFLICT | (item->mResult & FILE_USER_ACTION_MASK));
      }
    }

    // remove disabled item
    if ((flag & REMOVE_DISABLED) && (item->mEnable == FALSE) && (item->mChildResult == 0))
    {
      it = mChildList.erase(it);
      delete item;
    }
    else
    {
      ++it;
    }
  }

  if ((flag & STORE_META) &&
    ((last_child_result != mChildResult) || (last_result != mResult)))
  {
    if ((s != NULL) && s->IsMetafileExist(this))
      s->StoreMetafile(this, META_MASK_SYNCRESULT|META_MASK_CHILD_RESULT, 0);
  }

  if ((flag & STORE_META) && (depth == 0))
    s->EndTransaction(TRUE);
  return mResult;
}

BOOL KSyncFolderItem::GetItemRelativePath(KSyncItem *pItem, LPTSTR relativePath)
{
  int initial_path_len = lstrlen(relativePath);

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    MakeFullPathName(relativePath, KMAX_PATH, relativePath, (*it)->GetFilename());
    if (*it == pItem)
    {
      return TRUE;
    }
    if ((*it)->IsFolder())
    {
      BOOL r = ((KSyncFolderItem *)(*it))->GetItemRelativePath(pItem, relativePath);
      if (r)
        return r;
    }
    relativePath[initial_path_len] = '\0';
    ++it;
  }
  return FALSE;
}

int KSyncFolderItem::GetPathNameLength(int type)
{
  int len = 0;
  if (mParent != NULL)
  {
    len = ((KSyncFolderItem *)mParent)->GetPathNameLength(type);
    len += lstrlen(GetFilename()) + 1;
  }
  else
  {
    if (type == 1) // local path
      len = lstrlen(GetBaseFolder()) + 1; //lstrlen(GetPath()) + lstrlen(mLocal.Filename) + 1;
    else if (type == 2) // server path
      len += lstrlen(GetServerRelativePath()) + 1;
    else
      len = 1; 
  }
  return len;
}

void KSyncFolderItem::GetPathName(LPTSTR buff, int len, int type)
{
  if (mParent != NULL)
  {
    ((KSyncFolderItem *)mParent)->GetPathName(buff, len, type);
    MakeFullPathName(buff, len, buff, GetFilename());
  }
  else
  {
    if (type == 1)
      StringCchCopy(buff, len, GetBaseFolder());
    else if(type == 2)
    {
      LPCTSTR p = GetServerRelativePath();
      if (p != NULL)
        StringCchCopy(buff, len, p);
      else
        buff[0] = '\0';
    }
    else
      buff[0] = '\0';
  }
}

#if 0
int KSyncFolderItem::GetRenamedPathName(LPTSTR buff, int len, int flag)
{
  int r = 0;

  if (mParent != NULL)
    r = ((KSyncFolderItem *)mParent)->GetRenamedPathName(buff, len, flag);

  if ((flag & AS_LOCAL) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
  {
    if (buff != NULL)
    {
      LPCTSTR p = GetRootPath();
      MakeFullPathName(buff, len, p, mpLocalRenamedAs);
    }
    LPCTSTR rp = GetRootPath();
    int n = lstrlen(rp) + lstrlen(mpLocalRenamedAs) + 2 ;
    if (n > r)
      r = n;
  }
  else if (!(flag & AS_LOCAL) && (mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
  {
    if (buff != NULL)
      StringCchCopy(buff, len, mpServerRenamedAs);
    int n = lstrlen(mpServerRenamedAs) + 1;
    if (n > r)
      r = n;
  }
  else
  {
    // no rename info
#ifdef SYNC_ALL_DRIVE
    LPCTSTR p = (mParent == NULL) ? NULL : GetFilename();
#else
    LPCTSTR p = (mParent == NULL) ? GetRootPath() : GetFilename();
#endif
    if (buff != NULL)
    {
      if (lstrlen(buff) > 0) // parent has renamed info
      {
        MakeFullPathName(buff, len, buff, p);
        r++;
      }
      else
      {
        if (p != NULL)
          StringCchCopy(buff, len, p);
        else
          buff[0] = '\0';
      }
    }
    r += lstrlen(p) + 1;
  }
  return r;
}
#endif

BOOL KSyncFolderItem::IsParentRenamed(int flag)
{
  if ((flag & AS_LOCAL) && (mpLocalRenamedAs != NULL) && (lstrlen(mpLocalRenamedAs) > 0))
    return TRUE;
  else if (!(flag & AS_LOCAL) && (mpServerRenamedAs != NULL) && (lstrlen(mpServerRenamedAs) > 0))
    return TRUE;

  if (mParent != NULL)
    return ((KSyncFolderItem*)mParent)->IsParentRenamed(flag);
  return FALSE;
}

/**
* @brief 동기화 충돌 처리창에서 이동된 항목을 따로 보여주는 더미 폴더에서 이동된 폴더 정보를 이름에 덧붙인다.
*/
static void attachDummyPath(LPTSTR name, int length, LPTSTR dummyPath)
{
  if (FALSE) // CompareParentFolderPath(name, dummyPath) == 0)
  {
    TCHAR dummyName[KMAX_PATH];
    SplitPathName(dummyPath, NULL, dummyName);

    StringCchCat(name, length, _T("[->"));
    if (dummyPath != NULL)
      StringCchCat(name, length, dummyName);
  }
  else
  {
    StringCchCat(name, length, _T("[->\\"));
    if (dummyPath[0] == '\\')
      dummyPath++;
    if (dummyPath != NULL)
      StringCchCat(name, length, dummyPath);
  }
  StringCchCat(name, length, _T("]"));
}

#if 0
void KSyncFolderItem::GetSyncResultRName(LPTSTR name, int length, int flag)
{
  KSyncItem::GetSyncResultRName(name, length, flag);
  /*
  if (flag & WITH_DUMMY_PATH)
  {
    if (IsDummyCloneFolder())
    {
      TCHAR rPathName[KMAX_PATH];
      KSyncItem::GetSyncResultRName(rPathName, KMAX_PATH, (flag & ~AS_NAME_ONLY));

      if (flag & AS_LOCAL)
      {
        // attach dummy local path if it differ
        if ((mpDummyLocalPath != NULL) && (lstrlen(mpDummyLocalPath) > 0))
        {
          if (StrCmpI(rPathName, mpDummyLocalPath) != 0)
            attachDummyPath(name, length, mpDummyLocalPath);
        }
        return;
      }
      else if (!(flag & AS_LOCAL))
      {
        if ((mpDummyServerPath != NULL) && (lstrlen(mpDummyServerPath) > 0))
        {
          if (StrCmpI(rPathName, mpDummyServerPath) != 0)
            attachDummyPath(name, length, mpDummyServerPath);
        }
        return;
      }
    }
  }*/
}
#endif

LPTSTR KSyncFolderItem::AllocSyncMoveResultName(int flag)
{
  LPTSTR name = NULL;
  if (IsDummyCloneFolder())
  {
    if (flag & AS_LOCAL)
    {
      // attach dummy local path if it differ
      if ((mpDummyLocalPath != NULL) && (lstrlen(mpDummyLocalPath) > 0))
      {
        if (flag & AS_NAME_ONLY)
        {
          SplitPathNameAlloc(mpDummyLocalPath, NULL, &name);
        }
        else
        {
          name = AllocString(mpDummyLocalPath);
        }
      }
    }
    else if (!(flag & AS_LOCAL))
    {
      if ((mpDummyServerPath != NULL) && (lstrlen(mpDummyServerPath) > 0))
      {
        if (flag & AS_NAME_ONLY)
        {
          SplitPathNameAlloc(mpDummyServerPath, NULL, &name);
        }
        else
        {
          name = AllocString(mpDummyServerPath);
        }
      }
    }
  }
  return name;
}

// find none-dummy item which has name
KSyncItem* KSyncFolderItem::GetNormalChildItem(LPCTSTR name)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    if (item->IsFolder() && 
      ((name == NULL) || (StrCmpI(name, item->GetFilename()) == 0)))
    {
      if (!((KSyncFolderItem*)item)->IsDummyCloneFolder())
        return item;
    }
    ++it;
  }
  return NULL;
}

int KSyncFolderItem::FindDummyPairItems(KSyncItem* origin, KSyncItemArray& dummyItems)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    if (item->IsFolder() && (origin != item) &&
      ((KSyncFolderItem*)item)->IsDummyCloneFolder() &&
      (StrCmpI(origin->GetFilename(), item->GetFilename()) == 0))
    {
      dummyItems.push_back(item);
    }
    ++it;
  }
  return (int)dummyItems.size();
}

KSyncItem* KSyncFolderItem::GetTreeDisplayChildItem(int index)
{
  // index starts from 1.
  --index;
  return getTreeDisplayChildItemR(index);
}

KSyncItem* KSyncFolderItem::getTreeDisplayChildItemR(int& index)
{
  if (IsVisible() && IsResolveVisible())
  {
    if (index == 0)
      return this;
    --index;
  }

#ifdef CONFLICT_TREE_ORDER_BY_DEPTH
  if (IsOpened())
  {
    KSyncItem *result = NULL;
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem *item = *it;

      if (item->IsResolveVisible())
      {
        if (index == 0)
          return item;

        if (item->IsFolder())
        {
          KSyncItem *result = ((KSyncFolderItem*)item)->getTreeDisplayChildItemR(index);
          if (result != NULL)
            return result;
        }
        else
          --index;
      }
      ++it;
    }
  }
#else
  // child document
  if (IsOpened())
  {
    KSyncItem *result = NULL;
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem *item = *it;

      if (item->IsResolveVisible() && (item->IsFolder() == 0))
      {
        if (index == 0)
          return item;
        --index;
      }
      ++it;
    }
  }

  // dummy-pair
  KSyncItemArray dummyPairArray;
  if ((mParent != NULL) && !IsDummyCloneFolder())
  {
    ((KSyncFolderItem*)mParent)->FindDummyPairItems(this, dummyPairArray);

    size_t dnum = dummyPairArray.size();
    for(size_t i = 0; i < dnum; i++)
    {
      KSyncItem* d = dummyPairArray.at(i);
      if (d->IsVisible())
      {
        if (index == 0)
          return d;
        --index;

        // child document of dummy-pair
        if (((KSyncFolderItem*)d)->IsOpened())
        {
          KSyncItemArray itemArray;
          ((KSyncFolderItem*)d)->GetResolveChildItems(itemArray, 0, 0);
          size_t num = itemArray.size();
          if (index < (int)num)
          {
            KSyncItem* c = itemArray.at(index);
            index = 0;
            return c;
          }
          index -= (int)num;
        }
      }
    }
  }

  // all child folders
  if (IsOpened())
  {
    KSyncItemArray itemArray;
    GetResolveChildItems(itemArray, 1, EX_DUMMY);

    size_t dnum = dummyPairArray.size();
    for(size_t i = 0; i < dnum; i++)
    {
      KSyncItem* d = dummyPairArray.at(i);
      if (((KSyncFolderItem*)d)->IsOpened())
      {
        ((KSyncFolderItem*)d)->GetResolveChildItems(itemArray, 1, EX_DUMMY);
      }
    }

    GetResolveChildItems(itemArray, 1, IN_SINGLE_DUMMY);

    // sort it

    size_t num = itemArray.size();
    for(size_t i = 0; i < num; i++)
    {
      KSyncItem* c = itemArray.at(i);
      KSyncItem* result = ((KSyncFolderItem*)c)->getTreeDisplayChildItemR(index);
      if (result != NULL)
        return result;
    }
  }
#endif
  return NULL;
}

BOOL KSyncFolderItem::IsResolveVisible()
{
  if (KSyncItem::IsResolveVisible())
    return TRUE;
#ifdef _DEBUG_RESOLVE
  return TRUE;
#else
  if (mChildResult & FILE_SYNC_RESOLVE_VISIBLE_FLAG)
    return TRUE;
  return FALSE;
#endif
}

// set folder closed if it has sync-result, others opened
void KSyncFolderItem::InitializeResolveState(KSyncItemArray& clones, int flag)
{
  if (flag & FORCE_OPEN_FOLDER)
    SetOpened(TRUE); // !(mResult & (FILE_CONFLICT | FILE_SYNC_ERR)));

  InitResolveState(flag);

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (item->IsFolder())
      ((KSyncFolderItem*)item)->InitializeResolveState(clones, flag);
    else
      item->InitResolveState(flag);

    // check if it moved to different folder location on local and server
    // create clone folder, move child item to it.

    int lo_flag = item->GetItemStateMovedRenamed(TRUE);
    int sv_flag = item->GetItemStateMovedRenamed(FALSE);

    if ((flag & MAKE_CLONE_FOLDER) &&
      (item->mResult & (FILE_CONFLICT | FILE_SYNC_ERR)) &&
      ((lo_flag|sv_flag) & ITEM_STATE_MOVED))
      // (item->isLocalRenamedAs() || item->isServerRenamedAs()))
    {
      LPTSTR newLocalPath = NULL;
      LPTSTR newServerPath = NULL;

      LPTSTR currentPath = new TCHAR[lstrlen(item->mpRelativePath)+1];
      SplitPathName(item->mpRelativePath, currentPath, NULL);
      BOOL localMoved = FALSE;
      BOOL serverMoved = FALSE;

      if (item->isLocalRenamedAs())
      {
        int len = lstrlen(item->mpLocalRenamedAs)+1;
        newLocalPath = new TCHAR[len];
        SplitPathName(item->mpLocalRenamedAs, newLocalPath, NULL);
        localMoved = (CompareRelativePath(currentPath, newLocalPath) != 0);
        if (!localMoved)
        {
          delete[] newLocalPath;
          newLocalPath = NULL;
        }
        else if (lstrlen(newLocalPath) == 0) // moved to root
          StringCchCopy(newLocalPath, len, ROOT_ITEM_KEY_STR);
      }
      if (item->isServerRenamedAs())
      {
        int len = lstrlen(item->mpServerRenamedAs)+1;
        newServerPath = new TCHAR[len];
        SplitPathName(item->mpServerRenamedAs, newServerPath, NULL);
        serverMoved = (CompareRelativePath(currentPath, newServerPath) != 0);
        if (!serverMoved)
        {
          delete[] newServerPath;
          newServerPath = NULL;
        }
        else if (lstrlen(newServerPath) == 0) // moved to root
          StringCchCopy(newServerPath, len, _T("\\"));
      }

      // only move to other folder
      if (localMoved || serverMoved)
      {
        if (TRUE) // IsFolder() < 2)
        {
          KSyncFolderItem* clone = GetMovedCloneFolder(clones, this, newLocalPath, newServerPath);
          if (clone != NULL)
          {
            clone->AddChild(item);
            item->SetParent(clone);

            it = mChildList.erase(it);
            item = NULL;
          }
        }
        /*
        else if (item->IsFolder())
        {
          KSyncFolderItem* clone = GetMovedCloneFolder(clones, (KSyncFolderItem*)item, newLocalPath, newServerPath);

          it = mChildList.erase(it);
          item = NULL;
        }
        */
      }

      delete[] currentPath;
      if (newLocalPath != NULL)
        delete[] newLocalPath;
      if (newServerPath != NULL)
        delete[] newServerPath;
    }

    if (item != NULL)
      ++it;
  }
}

void KSyncFolderItem::RestoreClones(KSyncItemArray& clones)
{
  // store clone object as child
  if (clones.size() > 0)
  {
    KSyncItemIterator it = clones.begin();
    while(it != clones.end())
    {
      KSyncItem *item = *it;
      if (item->mParent == NULL)
        AddChild(item);
      else
        ((KSyncFolderItem*)item->mParent)->AddChild(item);
      it = clones.erase(it);
    }
    clones.clear();
  }
}

int KSyncFolderItem::RemoveDummyCloneFolders()
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (item->IsFolder())
    {
      count += ((KSyncFolderItem*)item)->RemoveDummyCloneFolders();

      if (((KSyncFolderItem*)item)->IsDummyCloneFolder())
      {
        // get original parent
        KSyncFolderItem* org_parent = (KSyncFolderItem*)GetNormalChildItem(item->GetFilename());
        if (org_parent != NULL)
          ((KSyncFolderItem*)item)->MoveChildItemTo(org_parent);

        delete item;
        it = mChildList.erase(it);
        item = NULL;
        ++count;
      }
    }

    if (item != NULL)
      ++it;
  }
  return count;
}

// 충돌, 오류, 이름변경, 새항목으로 추가 등이 아닌 항목은 스킵.
int KSyncFolderItem::CheckEnableOnResolveConflict()
{
  int count = 0;
  if ((mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)) == 0)
  {
    if (mEnable)
      mEnable = FALSE;
    count++;
  }

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (item->IsFolder())
    {
      count += ((KSyncFolderItem*)item)->CheckEnableOnResolveConflict();
    }
    else
    {
      if ((item->mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)) == 0)
      {
        if (item->mEnable)
          item->mEnable = FALSE;
        count++;
      }
    }
    ++it;
  }
  return count;
}

int KSyncFolderItem::MoveChildItemTo(KSyncFolderItem *newParent)
{
  int count = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    item->SetParent(newParent);
    newParent->AddChild(item);
    ++count;
    ++it;
  }
  mChildList.clear();
  return count;
}


KSyncFolderItem* KSyncFolderItem::GetMovedCloneFolder(KSyncItemArray& clones, KSyncFolderItem* org, LPCTSTR newLocalPath, LPCTSTR newServerPath)
{
  // find existing already
  KSyncItemIterator it = clones.begin();
  while(it != clones.end())
  {
    KSyncItem *item = *it;
    if (((KSyncFolderItem*)item)->IsMatchDummyClonePath(newLocalPath, newServerPath))
    {
      return (KSyncFolderItem*)item;
    }
    ++it;
  }

  KSyncFolderItem* clone;
  if (org->IsFolder() == 2)
    clone = new KSyncRootFolderItem(org->GetRootPath());
  else
    clone = new KSyncFolderItem(this);

  *clone = *org;
  clone->SetDummyCloneFolder();
  clone->SetDummyClonePath(newLocalPath, newServerPath);
  clones.push_back(clone);
  return clone;
}

// check if parent does not checked, but child is checked
BOOL KSyncFolderItem::IsChildCheckedWithoutParent(LPTSTR *p)
{
  if (!GetEnable() && IsConflictResolved() && (IsFolder() < 2)) // should be synced before child
  {
    // 드라이브 경로 없음 오류일때에는 하위 항목만 처리 가능하다.
    if (((mConflictResult & FILE_SYNC_ERR) != FILE_SYNC_NOT_EXIST_DRIVE) && GetEnableChild())
    {
      if (p != NULL)
      {
        TCHAR *baseFolder = GetBaseFolder();
        int len = lstrlen(baseFolder) + 3;
        if (*p != NULL)
          delete[] *p;

        *p = new TCHAR[len];
        if (baseFolder == NULL)
          StringCchCopy(*p, len, ROOT_ITEM_KEY_STR);
        else
          StringCchCopy(*p, len, baseFolder);
      }
      return TRUE;
    }
  }

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->IsFolder() && 
      ((KSyncFolderItem *)item)->IsChildCheckedWithoutParent(p))
      return TRUE;
    ++it;
  }
  return FALSE;
}

KSyncItem* KSyncFolderItem::CheckIfExistOnVirtualTop(LPTSTR relativePathName, BOOL onLocal, KSyncItem* ex)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->IsFolder() == 2)
    {
      KSyncItem *n = ((KSyncFolderItem*)item)->FindChildItemCascade(relativePathName, 2);
      if ((n != NULL) && (n != ex))
        return n;
    }
    ++it;
  }

  // 이동 및 rename된 개체 존재 검사.
  it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->IsFolder() == 2)
    {
      KSyncItem *n = ((KSyncFolderItem*)item)->FindRenamedChildItem(relativePathName, onLocal, 2);
      if (n != NULL)
        return n;
    }
    ++it;
  }

  return NULL;
}

// f1 항목을 서버(로컬)로 적용하려는데, 이미 동일한 파일이 존재하는 경우
BOOL KSyncFolderItem::CheckIfCrossConflict(KSyncFolderItem*root, BOOL applyToServer, LPTSTR *name)
{
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;

    if (item->GetEnable() && (item->IsFolder() < 2))
    {
      if ((applyToServer && item->IsLocalFileExist() && (item->mLocalState & ITEM_STATE_EXISTING)) ||
        (!applyToServer && item->IsServerFileExist() && (item->mServerState & ITEM_STATE_EXISTING)))
      {
        LPTSTR applyPathName = item->AllocSyncResultName(applyToServer ? AS_LOCAL : 0);

        KSyncItem* s = root->CheckIfExistOnVirtualTop(applyPathName, (applyToServer ? FALSE : TRUE), item);
        if ((s != NULL) && (s != item))
        {
          if (name != NULL)
          {
            *name = applyPathName;
            return TRUE;
          }
        }

        delete[] applyPathName;
      }
    }
    ++it;
  }

  it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->IsFolder() && 
      ((KSyncFolderItem *)item)->CheckIfCrossConflict(root, applyToServer, name))
      return TRUE;
    ++it;
  }

  return FALSE;
}

int KSyncFolderItem::SortChildList(int sortColumn, int sortAscent)
{
  int child_count = (int)mChildList.size();
  if (sortAscent)
    sortColumn |= ITEM_SORT_ASCENT;

  if (child_count > 1)
  {
    KSyncItemArray a;
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      a.push_back(*it);
      ++it;
    }
    mChildList.clear();

    it = a.begin();
    while(it != a.end())
    {
      KSyncItem* item = *it;
      it = a.erase(it);

      if (mChildList.size() > 0)
      {
        KSyncItemIterator it = mChildList.begin();
        while(it != mChildList.end())
        {
          if (CompareSyncItem(item, *it, sortColumn) < 0)
          {
            mChildList.insert(it, item);
            item = NULL;
            break;
          }
          ++it;
        }
      }
      if (item != NULL)
        mChildList.push_back(item);
    }
  }

  // recursive
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->IsFolder())
    {
      int child_s = ((KSyncFolderItem *)item)->SortChildList(sortColumn, sortAscent);
      if (child_s > child_count)
        child_count = child_s;
    }
    ++it;
  }

  return child_count;
}

BOOL KSyncFolderItem::HasSelected()
{
  if (KSyncItem::HasSelected())
    return TRUE;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    if (item->HasSelected())
      return TRUE;
    ++it;
  }
  return FALSE;
}

void KSyncFolderItem::EnableSelectedItem()
{
  KSyncItem::EnableSelectedItem();

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem* item = *it;
    item->EnableSelectedItem();
    ++it;
  }
}

void KSyncFolderItem::SetResolveFlagMask(int a, int o, BOOL withChild)
{
  KSyncItem::SetResolveFlagMask(a, o, withChild);

  if (withChild)
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem* item = *it;
      if (item->IsResolveVisible())
        item->SetResolveFlagMask(a, o, withChild);
      ++it;
    }
  }
}

int KSyncFolderItem::GetResolveFlagArea(int a, int o, int index, int* area, int flag)
{
  index = KSyncItem::GetResolveFlagArea(a, o, index, area, flag);
  //++index;

  if (IsOpened() && IsResolveVisible() && (flag & WITH_CHILD_DOCUMENT))
  {
#ifdef CONFLICT_TREE_ORDER_BY_DEPTH
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem* item = *it;
  
      if (item->IsResolveVisible())
        index = item->GetResolveFlagArea(a, o, index, area, flag);
      ++it;
    }
#else

  // child document
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem* item = *it;
      if (item->IsResolveVisible() && (item->IsFolder() == 0))
      {
        index = item->GetResolveFlagArea(a, o, index, area, flag);
      }
      ++it;
    }

  // all child folders
  if (flag & WITH_CHILD_FOLDER)
  {
    // dummy-pair
    KSyncItemArray dummyPairArray;
    if ((mParent != NULL) && !IsDummyCloneFolder())
    {
      ((KSyncFolderItem*)mParent)->FindDummyPairItems(this, dummyPairArray);

      size_t dnum = dummyPairArray.size();
      for(size_t i = 0; i < dnum; i++)
      {
        KSyncItem* d = dummyPairArray.at(i);
        if (d->IsVisible())
        {
          index = d->GetResolveFlagArea(a, o, index, area, WITH_CHILD_DOCUMENT);
        }
      }
    }

    KSyncItemArray itemArray;
    GetResolveChildItems(itemArray, 1, EX_DUMMY);

    size_t dnum = dummyPairArray.size();
    for(size_t i = 0; i < dnum; i++)
    {
      KSyncItem* d = dummyPairArray.at(i);
      if (((KSyncFolderItem*)d)->IsOpened())
      {
        ((KSyncFolderItem*)d)->GetResolveChildItems(itemArray, 1, EX_DUMMY);
      }
    }

    GetResolveChildItems(itemArray, 1, IN_SINGLE_DUMMY);

    // sort it

    size_t num = itemArray.size();
    for(size_t i = 0; i < num; i++)
    {
      KSyncItem* c = itemArray.at(i);
      index = c->GetResolveFlagArea(a, o, index, area, WITH_CHILD_DOCUMENT|WITH_CHILD_FOLDER);
    }
  }

#endif
  }
  return index;
}

int KSyncFolderItem::GetChildRowCount()
{
  int ncount = 0;

  if (IsVisible() && IsResolveVisible())
    ncount = 1;

  if (IsOpened())
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem* item = *it;
      if (item->IsResolveVisible())
        ncount += item->GetChildRowCount();
      ++it;
    }
  }
  return ncount;
}

/*
r = 1 : 동일한 이름 존재하지만 이름이 변경되거나 삭제될 것임.
r = 2 : 동일한 이름이 존재하고 변경되지 않을것임.
checkIfMoved = 0 : 상태 체크 안함, 1 : 로컬 상태, 2 : 서버 상태
*/
// 동일한 이름이 존재하고 그것이 최종 이름이면 임시 이름 바꾸기로 해결되지 못하니 충돌로 처리함
int KSyncFolderItem::CheckIfChildNameExist(LPCTSTR childName, KSyncItem* except, int checkIfMoved, BOOL onLocal)
{
  int rtn = 0;
  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    if (item != except)
    {
      if ((StrCmpI(item->GetFilename(), childName) == 0) ||
        ((lstrlen(item->mLocalNew.pFilename) > 0) && (StrCmpI(item->mLocalNew.pFilename, childName) == 0)))
      {
        if (!item->IsServerTemporaryRenamed()) // 임시 이름으로 바뀐 것이면 무시해도 됨.
        {
          if (checkIfMoved)
          {
            if (checkIfMoved == 2)
            {
              if (item->mResult & FILE_NEED_LOCAL_DELETE) // 삭제될 항목이면 1로
                return 1;
              if ( !((item->mServerState & ITEM_STATE_MOVED) || item->IsLocalTemporaryRenamed()) )
              return 2; // 동일한 이름의 항목이 이동되지 않을 것이므로 오류로 처리하도록.
            }

            if (checkIfMoved == 1)
            {
              if (item->mResult & FILE_NEED_SERVER_DELETE) // 삭제될 항목이면 1로
                return 1;
              if (!((item->mLocalState & ITEM_STATE_MOVED) || item->IsServerTemporaryRenamed()) )
                return 2; // 동일한 이름의 항목이 이동되지 않을 것이므로 오류로 처리하도록.
            }
          }
          return 1;
        }
      }
    }
    ++it;
  }

  if (onLocal)
  {
    // 로컬 폴더에 같은 이름이 존재하는지 확인(제외 폴더의 경우에 존재할 수 있음.)
    LPTSTR pathName = AllocFullPathName();
    LPTSTR fullName = AllocMakeFullPathName(pathName, childName);
    if (IsFileExist(fullName))
      rtn = 2;
    delete[] fullName;
    delete[] pathName;
  }
  return rtn;
}

BOOL KSyncFolderItem::ReadyUniqueChildName(LPCTSTR childName, LPTSTR resultName, int side, BOOL onLocal)
{
  StringCchCopy(resultName, KMAX_PATH, childName);
  int n = lstrlen(resultName) + 1;
  StringCchCat(resultName, KMAX_PATH, _T(".AAA"));

  do
  {
    if (!CheckIfChildNameExist(resultName, NULL, side, onLocal))
      return TRUE;
  } while (MakeNextSequenceName(4, &resultName[n]));

  /*
  LPCTSTR pExt = StrRChr(childName, NULL, '.');
  TCHAR name[KMAX_PATH];
  if (pExt != NULL)
    StringCchCopyN(name, KMAX_PATH, childName, pExt - childName);
  else
    StringCchCopy(name, KMAX_PATH, childName);

  int n = lstrlen(name) + 1;
  StringCchCat(name, KMAX_PATH, _T(".AAA"));

  do
  {
    StringCchCopy(resultName, KMAX_PATH, name);
    if (pExt != NULL)
      StringCchCat(resultName, KMAX_PATH, pExt);

    if (!CheckIfChildNameExist(resultName, FALSE))
      return TRUE;
  } while (MakeNextSequenceName(4, &name[n]));
  */
  return FALSE;
}

LPTSTR KSyncFolderItem::ReadyUniqueLocalName(LPCTSTR pathname)
{
  int n = lstrlen(pathname) + 1;
  LPTSTR newName = new TCHAR[n + 5];
  StringCchCopy(newName, n + 5, pathname);
  StringCchCat(newName, n + 5, _T(".AAA"));

  do
  {
    if (!IsDirectoryExist(newName))
      return newName;
  } while (MakeNextSequenceName(4, &newName[n]));
  return NULL;
}

int KSyncFolderItem::RestoreTemporaryRenamed()
{
  int count = 0;

  if (mParent != NULL)
  {
    LPTSTR prevRelativeName = AllocRelativePathName();
    KSyncRootFolderItem *rootFolder =  (KSyncRootFolderItem *)GetRootFolderItem();
    if (IsServerTemporaryRenamed())
    {
      TCHAR msg[512] = _T("Server Folder '");
      StringCchCat(msg, 512, GetFilename());
      StringCchCat(msg, 512, _T("' to '"));
      StringCchCat(msg, 512, mpServerTempRenamed);
      StringCchCat(msg, 512, _T("' tempRename restore"));

      if (gpCoreEngine->ChangeFolderName(mServerOID, GetServerFullPathIndex(), GetFilename(), mpServerTempRenamed, NULL))
      {
        ++count;
        RefreshServerState();
        AllocCopyString(&mLocal.pFilename, mpServerTempRenamed);
        delete[] mpServerTempRenamed;
        mpServerTempRenamed = NULL;
        StringCchCat(msg, 512, _T(" done"));
      }
      else
      {
        StringCchCat(msg, 512, _T(" failure"));
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    if (IsLocalTemporaryRenamed())
    {
      TCHAR msg[512] = _T("Local Folder '");
      StringCchCat(msg, 512, GetFilename());
      StringCchCat(msg, 512, _T("' to '"));
      StringCchCat(msg, 512, mpLocalTempRenamed);
      StringCchCat(msg, 512, _T("' tempRename restore"));

      LPTSTR newPathName = AllocPathName(mParent->GetPath(), mpLocalTempRenamed);
      LPTSTR orgFullPath = AllocAbsolutePathName(newPathName);
      LPTSTR tmpFullPath = AllocAbsolutePathName();
      delete[] newPathName;

      rootFolder->OnStoreCancelRenamed(tmpFullPath, orgFullPath);
      int last_err = KMoveFile(tmpFullPath, orgFullPath);
      if (last_err == 0)
      {
        ++count;
        AllocCopyString(&mLocal.pFilename, mpLocalTempRenamed);
        delete[] mpLocalTempRenamed;
        mpLocalTempRenamed = NULL;
        StringCchCat(msg, 512, _T(" done"));
      }
      else
      {
        StringCchCat(msg, 512, _T(" failure"));
      }

      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    if (count > 0)
    {
      // mpRelativePath will be updated on MoveMetafileAsRename
      //SetLocalPath(mParent->GetPath(), NULL, 1);

      MoveMetafileAsRename(prevRelativeName, WITH_STORE_METAFILE, rootFolder->GetStorage());

      UpdateChildItemPath(rootFolder, 1, prevRelativeName, GetBaseFolder(), TRUE);

      StoreMetafile(NULL, META_MASK_FILENAME, 0);
    }
    delete[] prevRelativeName;
  }

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    count += item->RestoreTemporaryRenamed();
    if (count > 0) // process partially
      break;
    ++it;
  }
  return count;
}

int KSyncFolderItem::SetLocalRename(LPCTSTR baseFolder, LPCTSTR str)
{
  LPTSTR lastRelativePath = AllocString(GetBaseFolder());
  int r = KSyncItem::SetLocalRename(baseFolder, str);
  if (r == R_SUCCESS)
  {
    // update path for all child items
    KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    UpdateChildItemPath(rootFolder, 1, lastRelativePath, GetBaseFolder(), TRUE);
  }
  delete[] lastRelativePath;
  return r;
}

int KSyncFolderItem::SetServerRename(LPCTSTR baseFolder, LPCTSTR str)
{
  LPTSTR lastRelativePath = AllocString(GetBaseFolder());
  int r = KSyncItem::SetServerRename(baseFolder, str);
  if (r == R_SUCCESS)
  {
    // update path for all child items
    KSyncRootFolderItem *rootFolder = (KSyncRootFolderItem *)((KSyncFolderItem *)mParent)->GetRootFolderItem();
    UpdateChildItemPath(rootFolder, 1, lastRelativePath, GetBaseFolder(), TRUE);
  }
  delete[] lastRelativePath;
  return r;
}

void KSyncFolderItem::SetDummyClonePath(LPCTSTR localPath, LPCTSTR serverPath)
{
  if (mpDummyLocalPath != NULL)
    delete mpDummyLocalPath;
  if (mpDummyServerPath != NULL)
    delete mpDummyServerPath;
  mpDummyLocalPath = AllocString(localPath);
  mpDummyServerPath = AllocString(serverPath);
}

BOOL KSyncFolderItem::IsMatchDummyClonePath(LPCTSTR localPath, LPCTSTR serverPath)
{
  if (IsDummyCloneFolder())
  {
    if ((localPath == NULL) && (serverPath == NULL))
      return TRUE;

    if (((localPath == NULL) || (lstrlen(localPath) == 0)) != 
      ((mpDummyLocalPath == NULL) || (lstrlen(mpDummyLocalPath) == 0)))
      return FALSE;

    if (((serverPath == NULL) || (lstrlen(serverPath) == 0)) != 
      ((mpDummyServerPath == NULL) || (lstrlen(mpDummyServerPath) == 0)))
      return FALSE;

    //if ((localPath != NULL) && (mpDummyLocalPath != NULL))
    if ((localPath != NULL) && (lstrlen(localPath) > 0) &&
      (mpDummyLocalPath != NULL) && (lstrlen(mpDummyLocalPath) > 0))
    {
      if (StrCmpI(mpDummyLocalPath, localPath) != 0)
        return FALSE;
    }
    if ((serverPath != NULL) && (lstrlen(serverPath) > 0) &&
      (mpDummyServerPath != NULL) && (lstrlen(mpDummyServerPath) > 0))
    {
      if (StrCmpI(mpDummyServerPath, serverPath) != 0)
        return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL KSyncFolderItem::ContinueStoreMetafile(KSyncRootStorage* storage)
{
  if (mChildItemIndex == 0)
    StoreMetafile(storage, META_MASK_ALL);

  KSyncItemIterator it = mChildList.begin();
  if(mChildItemIndex > 0)
    it += mChildItemIndex;
  int count = 0;
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    if (item->IsFolder() == 0)
    {
      item->StoreMetafile(storage, META_MASK_ALL);
      ++count;
      if(count > NUMBER_OF_ITEMS_META_STORE)
        return TRUE;
    }
    ++it;
    ++mChildItemIndex;
  }
  return FALSE;
}

void KSyncFolderItem::SetOverridePath(int callingDepth, KSyncRootStorage* s, LPCTSTR baseFolder)
{
  if (callingDepth == 0)
    s->BeginTransaction();

  KSyncItem::SetOverridePath(callingDepth, s, baseFolder);

  /*
  if (mBaseFolder != NULL)
    delete[] mBaseFolder;
  mBaseFolder = AllocString(mpRelativePath);
  */

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;

    item->SetOverridePath(callingDepth, s, mpOverridePath);
    ++it;
  }

  if (callingDepth == 0)
    s->EndTransaction(TRUE);
}

void KSyncFolderItem::CheckClearOverridePath(KSyncRootStorage* s)
{
  if (IsOverridePath())
  {
    SetOverridePath(0, s, NULL);
  }
  else
  {
    KSyncItemIterator it = mChildList.begin();
    while(it != mChildList.end())
    {
      KSyncItem *item = *it;
      if (item->IsFolder())
      {
        ((KSyncFolderItem*)item)->CheckClearOverridePath(s);
      }
      ++it;
    }
  }
}

#ifdef _DEBUG

void KSyncFolderItem::DumpSyncItems(int depth)
{
  KSyncItem::DumpSyncItems(depth);

  KSyncItemIterator it = mChildList.begin();
  while(it != mChildList.end())
  {
    KSyncItem *item = *it;
    item->DumpSyncItems(depth+1);
    ++it;
  }
}
#endif
