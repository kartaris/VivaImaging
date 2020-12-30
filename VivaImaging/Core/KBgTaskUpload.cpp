#include "stdafx.h"
#include "KBgTaskUpload.h"

#include "../Core/KSyncCoreEngine.h"
#include "../Core/KSettings.h"
#include "../Platform/Platform.h"

KBgTaskUpload::KBgTaskUpload(CWnd* owner, BgTaskType task, LPCTSTR arg)
  :KBgTaskLoader(owner, task, arg)
{
  mDoneFolder = 0;
  mTotalFolder = 0;
  mDoneFile = 0;
  mTotalFile = 0;
  mEcmDocTypeInfo = NULL;
  mSaveFormat = TIF_MULTI_PAGE;
  mFilename = NULL;
  mpImageFolder = NULL;
  //memset(&mRootFolderInfo, 0, sizeof(UploadFolderInfo));
}

KBgTaskUpload::~KBgTaskUpload(void)
{
  if (mFilename != NULL)
  {
    delete[] mFilename;
    mFilename = NULL;
  }
}

static int getPathDepth(LPCTSTR pathName)
{
  int depth = 0;
  LPCTSTR p = pathName;
  while (p != NULL)
  {
    p = StrChr(p, '\\');
    if (p != NULL)
    {
      p++;
      depth++;
    }
  }
  return depth;
}
/*
void freeFolderInfo(UploadFolderInfo* pFolderInfo)
{
  if (pFolderInfo != NULL)
  {
    if (pFolderInfo->folderIndex != NULL)
      delete[] pFolderInfo->folderIndex;
    if (pFolderInfo->name != NULL)
      delete[] pFolderInfo->name;
    delete pFolderInfo;
  }
}
*/

void KBgTaskUpload::SetUploadInfo(LPCTSTR filename, LPCTSTR workingPath, KImageFolder* rootImageFolder, 
  KEcmDocTypeInfo* info, ImagingSaveFormat saveFormat, UINT flag)
{
  mFilename = AllocString(filename);
  mWorkingPath = AllocString(workingPath);
  mpImageFolder = rootImageFolder;
  mEcmDocTypeInfo = info,
  mSaveFormat = saveFormat;
  mFlag = flag;
  mFlag |= GO_UPLOAD_ONLY_MODIFIED;
}

void KBgTaskUpload::Do()
{
  TCHAR folderName[MAX_PATH];
  TCHAR folderIndex[MAX_PATH];

  if ((lstrlen(mArg) == 0) ||
    !gpCoreEngine->SyncServerGetFolderName(mArg, folderName, FALSE, folderIndex))
  {
    mError += _T("server folder not found");
    return;
  }
  else
  {
    TCHAR msg[256] = _T("Start upload destination : ");
    StringCchCat(msg, 256, mArg);
    StringCchCat(msg, 256, _T(", "));
    StringCchCat(msg, 256, folderName);

    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  // check if DocType is supported
  if (mEcmDocTypeInfo != NULL)
  {
    // mEcmDocTypeInfo->mDocTypeOid

  }

  // if clean folder
  EcmFolderItem* rootfolder = NULL;
  if (mFlag & GO_CLEARFOLDER_BEFOREUPLOAD)
  {
    TCHAR parentFolderOID[MAX_OID];
    TCHAR parentFolderIndex[MAX_PATH];

    if (gpCoreEngine->GetFolderInfo(mArg, parentFolderOID, folderName))
    {
      LPTSTR pErrorMsg = NULL;
      TCHAR folderOID[MAX_OID];

      gpCoreEngine->SyncServerGetFolderName(parentFolderOID, NULL, FALSE, parentFolderIndex);

      gpCoreEngine->SyncServerDeleteFolder(parentFolderOID, parentFolderIndex, &pErrorMsg);

      SERVER_INFO folder_info;
      gpCoreEngine->SyncServerCreateFolder(NULL, parentFolderOID, parentFolderIndex,
        folderName, folderOID, folder_info, &pErrorMsg);

      rootfolder = new EcmFolderItem(NULL, NULL, folderOID, folder_info.pFullPathIndex);
      //mFolderList.push_back(rootfolder);
      mEcmRoot = rootfolder;
    }
    else
    {
      mError += _T("server folder not found");
      return;
    }
  }
  else
  {
    rootfolder = new EcmFolderItem(NULL, NULL, mArg, folderIndex);
    // mFolderList.push_back(rootfolder);
    mEcmRoot = rootfolder;

    CString download_err;

    int result = DownloadFolderList(download_err);

    // parse received pakets and generate object-tree
    if (IsContinue() && (result == R_SUCCESS) && (mDownloadLength > 0))
    {
      parseFolderNDocument();
    }
    else
    {
      if (download_err.GetLength() > 0)
        mError += download_err;
      return;
    }
  }
  
  // make folder and file list
  if (mpImageFolder != NULL)
  {
    // body index file
    EcmFileItem* file = (EcmFileItem*)findFileItemByName(NULL, mFilename);
    if (file == NULL)
    {
      file = new EcmFileItem(rootfolder, mFilename, NULL, NULL, NULL);
      ((EcmFolderItem*)mEcmRoot)->AddChild(file);
      // mFileList.push_back(file);
    }

    if (file != NULL)
    {
      file->SetNeedUpload(TRUE);

      KMetadataInfo* pInfo = mpImageFolder->GetMetaDataInfo(0);
      file->SetMetadataInfo(pInfo);
    }

    if ((mSaveFormat != TIF_MULTI_PAGE) &&
      (mSaveFormat != TIFF_MULTI_PAGE))
    {
      KImageDocVector pages;

      mpImageFolder->GetPageList(pages, FOLDER_ONLY);
      KImageDocIterator it = pages.begin();
      while (it != pages.end())
      {
        KImageFolder* p = (KImageFolder*)*(it++);

        EcmFolderItem* folder = (EcmFolderItem*)findFolderItemByName(p->GetName());
        if (folder == NULL)
        {
          folder = new EcmFolderItem(rootfolder, p->GetName(), NULL, NULL);
          folder->SetNeedUpload(TRUE);
          ((EcmFolderItem*)mEcmRoot)->AddChild(folder);
          //mFolderList.push_back(folder);
        }
      }
      pages.clear();

      mpImageFolder->GetPageList(pages, IMAGE_ONLY);
      it = pages.begin();
      while (it != pages.end())
      {
        KImageBase* p = *(it++);

        LPCTSTR parentName = NULL;
        if (p->GetParent() != NULL)
          parentName = p->GetParent()->GetName();

        EcmFileItem* file = (EcmFileItem*)findFileItemByName(parentName, p->GetName());
        BOOL need_upload = FALSE;
        TCHAR reason[32] = _T("");

        if (file != NULL)
        {
          // compare last oid
          TCHAR docOID[MAX_OID];
          TCHAR fileOID[MAX_OID];
          TCHAR storageOID[MAX_STGOID];

          if (((KImagePage *)p)->GetEcmInfo(docOID, fileOID, storageOID))
          {
            // check if server item changed after downloaded
            if ((CompareStringNC(docOID, file->fileOID) != 0) ||
              (CompareStringNC(fileOID, file->fileOID) != 0) ||
              (CompareStringNC(storageOID, file->storageOID) != 0))
            {
              StringCchCopy(reason, 32, _T("server file modified"));
              need_upload = TRUE;
            }
            else
            {
              StringCchCopy(reason, 32, _T("same"));
            }

            if (!need_upload &&  p->IsModified())
            {
              StringCchCopy(reason, 32, _T("local file modified"));
              need_upload = TRUE;
            }
          }
          else
          {
            StringCchCopy(reason, 32, _T("new local file"));
            need_upload = TRUE;
          }
        }
        else
        {
          StringCchCopy(reason, 32, _T("Not exist server"));
          need_upload = TRUE;
        }

        if (file == NULL)
        {
          EcmFolderItem* folder = rootfolder;
          if (parentName != NULL)
            (EcmFolderItem*)findFolderItemByName(parentName);

          file = new EcmFileItem(folder, ((KImagePage*)p)->GetLocalFilename(), NULL, NULL, NULL);
          // file->pMetadata = &(p->mMetadataContainer);

          file->SetMetadataInfo(&((KImagePage*)p)->GeDocExtAttributes());

          // mFileList.push_back(file);
          folder->AddChild(file);
          need_upload = TRUE;
        }

        TCHAR msg[256] = _T("Upload : ");
        StringCchCat(msg, 256, p->GetName());
        if (need_upload)
        {
          StringCchCat(msg, 256, _T(", needed, "));
          StringCchCat(msg, 256, reason);
          file->SetNeedUpload(TRUE);
        }
        else
        {
          StringCchCat(msg, 256, _T(", skip, "));
          StringCchCat(msg, 256, reason);
        }
        SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }
  }

  mTotalFolder = GetLoadNeededFolder(ITEM_NEED_UPLOAD);
  mTotalFile = GetLoadNeededFile(ITEM_NEED_UPLOAD);

  sendProgressValue();

#if 1
  EcmBaseItem* item;
  while (IsContinue() && 
    ((item = ((EcmFolderItem*)mEcmRoot)->GetNextItem(ITEM_NEED_UPLOAD)) != NULL))
  {
    if (item->IsNeedUpload())
    {
      int r;

      if (item->IsFolder())
        r = createFolder((EcmFolderItem*)item);
      else
        r = uploadFile((EcmFileItem*)item);
      if (r == R_SUCCESS)
      {
        item->SetNeedUpload(FALSE);
        mDoneFile++;
#ifdef _DEBUG
        Sleep(1000);
#endif
      }
      else
      {
        item->SetNeedUpload(FALSE);
      }
      sendProgressValue();
    }
  }
  SetDone();

#else

  if (mTotalFolder > 0)
  {
    // create folders
    KVoidPtrIterator it = mFolderList.begin();
    while (IsContinue() && (it != mFolderList.end()))
    {
      EcmFolderItem* folder = (EcmFolderItem*)*(it++);
      if (folder->IsNeedUpload() && (createFolder(folder) == R_SUCCESS))
      {
        mDoneFolder++;
#ifdef _DEBUG_SLOW
        Sleep(1000);
#endif
      }
      sendProgressValue();
    }
  }

  if (IsContinue() && (mTotalFile > 0))
  {
    // upload files
    KVoidPtrIterator it = mFileList.begin();
    while (IsContinue() && (it != mFileList.end()))
    {
      EcmFileItem* file = (EcmFileItem*)*(it++);

      if (file->IsNeedUpload() && (uploadFile(file) == R_SUCCESS))
      {
        mDoneFile++;
#ifdef _DEBUG
        Sleep(1000);
#endif
      }
      sendProgressValue();
    }
    SetDone();
  }
#endif
}

void KBgTaskUpload::sendProgressValue()
{
  if (mOwner != NULL)
  {
    HANDLE h = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(TaskProgressInfo));
    TaskProgressInfo* info = (TaskProgressInfo*)GlobalLock(h);
    info->totalFolders = mTotalFolder;
    info->totalFiles = mTotalFile;
    info->doneFolders = mDoneFolder;
    info->doneFiles = mDoneFile;

    GlobalUnlock(h);
    mOwner->PostMessage(MSG_BGTASK_MESSAGE, BGTASK_PROGRESS, (LPARAM)h);
  }
}

int KBgTaskUpload::createFolder(EcmFolderItem* folder) //UploadFolderInfo* pFolderInfo)
{
  SERVER_INFO out_child_folder_info;
  LPTSTR pErrorMsg = NULL;
  int rtn = R_SUCCESS;
  int msgflag = SYNC_MSG_LOG;

  // check if already exist
  if (lstrlen(folder->folderOID) > 0)
  {
    TCHAR folderIndex[MAX_PATH];
    TCHAR folderName[MAX_PATH];
    LPTSTR pErrorMsg = NULL;

    gpCoreEngine->SyncServerGetFolderName(folder->folderOID, folderName, FALSE, folderIndex);

    // check 
    TCHAR msg[256];

    if ((folder->mParent != NULL) && (lstrcmp(folderName, folder->mName) != 0))
    {
      StringCchPrintf(msg, 256, _T("Name changed : "));
      StringCchCat(msg, 256, folder->mName);
      StringCchCat(msg, 256, _T(" > "));
      StringCchCat(msg, 256, folderName);
    }
    else if (lstrcmp(folderIndex, folder->folderFullPathIndex) != 0)
    {
      StringCchPrintf(msg, 256, _T("PathIndex changed : "));
      StringCchCat(msg, 256, folder->folderFullPathIndex);
      StringCchCat(msg, 256, _T(" > "));
      StringCchCat(msg, 256, folderIndex);
    }
    else
    {
      StringCchPrintf(msg, 256, _T("Folder exist : "));
      StringCchCat(msg, 256, (folder->mName != NULL) ? folder->mName : _T("\\"));
    }
    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return R_SUCCESS;
  }

  TCHAR msg[256] = _T("Create Folder : ");
  StringCchCat(msg, 256, folder->mName);
  EcmFolderItem* parent = (EcmFolderItem*)folder->mParent;

  if (gpCoreEngine->SyncServerCreateFolder(NULL, parent->folderOID, parent->folderFullPathIndex,
    folder->mName, folder->folderOID, out_child_folder_info, &pErrorMsg))
  {
    folder->folderFullPathIndex = AllocString(out_child_folder_info.pFullPathIndex);
    StringCchCat(msg, 256, _T(" Ok"));
  }
  else
  {
    StringCchCat(msg, 256, _T(" : fail"));
    if (pErrorMsg != NULL)
    {
      StringCchCat(msg, 256, _T(", error"));
      StringCchCat(msg, 256, pErrorMsg);
    }
    msgflag |= SYNC_EVENT_ERROR;
    rtn = R_FAIL_INTERNAL;
  }
  SendLogMessage(_T(__FUNCTION__), msg, msgflag);

  if (pErrorMsg != NULL)
    delete[] pErrorMsg;
  return rtn;
}


int KBgTaskUpload::uploadFile(EcmFileItem* file)
{
  int r = R_SUCCESS;
  LPTSTR localFileName = AllocPathName(mWorkingPath, file->mName);
  TCHAR msg[256];

  if (file->mParent == NULL)
  {
    StringCchPrintf(msg, 256, _T("No parent folder found for %s, upload canceled"), file->mName);
    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    return R_FAIL_INTERNAL;
  }

  EcmFolderItem* parent = (EcmFolderItem*)file->mParent;
  LPTSTR pErrorMsg = NULL;

  // 문서유형정보를 포함하여 업로드
  r = gpCoreEngine->SyncServerUploadFile(parent->folderOID, parent->folderFullPathIndex,
    file->docOID, file->fileOID, file->storageOID,
    file->mName, localFileName, mEcmDocTypeInfo, file->pMetadata, &pErrorMsg);

  if (r == R_SUCCESS)
  {
    StringCchPrintf(msg, 256, _T("Upload done %s, storageOID=%s, fileOID=%s"), file->mName, file->storageOID, file->fileOID);
    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    // send upload result info
    KVoidPtrArray fileList;
    fileList.push_back(file);
    HANDLE h = createListData(fileList);

    mOwner->PostMessage(MSG_BGTASK_MESSAGE, BGTASK_UPLOAD_DONE, (LPARAM)h);
  }
  else
  {
    StringCchPrintf(msg, 256, _T("Upload fail %s"), file->mName);
    if (pErrorMsg != NULL)
    {
      StringCchCat(msg, 256, _T(", error : "));
      StringCchCat(msg, 256, pErrorMsg);
    }
    SendLogMessage(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
  }

  if (localFileName != NULL)
    delete[] localFileName;
  if (pErrorMsg != NULL)
    delete[] pErrorMsg;

  return r;
}
