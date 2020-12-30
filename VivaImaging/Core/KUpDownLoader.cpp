#include "stdafx.h"
#include <tchar.h>
#include "KUpDownLoader.h"

#include "KConstant.h"
#include "KSettings.h"
#include "KSyncCoreEngine.h"
#include "../Platform/Platform.h"

#include "ThreadPool.h"
#include <strsafe.h>
#include <Shlwapi.h>

#ifdef USE_HTTP_DOWNLOADER
#include "../HttpDownloader/KWinSockDownloader.h"
#include "../HttpDownloader/KWnetDownloader.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define R_WARN_SERVER_BUSY -2

KUpDownLoader::KUpDownLoader(KSyncItem* root, UpDownLoaderJob job, LPCTSTR url, LPCTSTR localFileName, LPCTSTR serverOID,
  LPCTSTR serverFileOID, LPCTSTR serverStorageOID, LPCTSTR parentFolderOID, LPCTSTR parentFullPathIndex, __int64 timeStamp, void *p)
  : mpRoot(root), mJob(job), mObject(p)
{
  if (url != NULL)
    StringCchCopy(mServerUrl, KMAX_PATH, url);
  else
    mServerUrl[0] = '\0';
  StringCchCopy(mLocalFileName, KMAX_PATH, localFileName);
  StringCchCopy(mServerOID, MAX_OID, serverOID);
  StringCchCopy(mServerFileOID, MAX_OID, serverFileOID);
  StringCchCopy(mServerStorageOID, MAX_STGOID, serverStorageOID);
  if (parentFolderOID != NULL)
    StringCchCopy(mParentFolderOID, MAX_OID, parentFolderOID);
  else
    mParentFolderOID[0] = '\0';
  mParentFullPathIndex = AllocString(parentFullPathIndex);

  mTimeStamp = timeStamp;
  memset(&mUploadFileDetails, 0, sizeof(mUploadFileDetails));
  memset(&mInitialLastModified, 0, sizeof(SYSTEMTIME));

  mErrorMessage = NULL;
  mScanData = NULL;
  mScanDataLength = 0;
  mScanFlag = 0;
  mResult = -1;
  mStopping = FALSE;
#ifdef USE_HTTP_DOWNLOADER
  mServerFolderScanResult = SFS_NO_ERROR;
  mDownloader = NULL;
#endif

  mParam1 = NULL;
  mParam2 = NULL;

  mState = LOADER_INIT;
  mDone = FALSE;
  mRetry = 0;
  mModifiedOnLoading = FALSE;

#ifdef USE_HTTP_DOWNLOADER
  if (mJob >= FOLDER_SCAN)
    mDownloader = new KWnetDownloader(); // KWinSockDownloader();
#endif

  // Create a manual-reset event that is not signaled at first to indicate 
  // the stopped signal of the service.
#ifndef USE_SCAN_BY_WORKER_THREAD
  m_hStoppedEvent = NULL;
  if (mJob < FOLDER_SCAN)
#endif
  {
    m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStoppedEvent == NULL)
    {
      StoreLogHistory(_T(__FUNCTION__), _T("CreateEvent fail"), SYNC_MSG_LOG);
        //throw GetLastError();
    }
  }
}

KUpDownLoader::KUpDownLoader(KSyncItem* root, UpDownLoaderJob job, LPCTSTR url, LPCTSTR localFileName,
  LPCTSTR serverOID, LPCTSTR p1, LPCTSTR p2)
  : mpRoot(root), mJob(job), mObject(NULL)
{
  if (url != NULL)
    StringCchCopy(mServerUrl, KMAX_PATH, url);
  else
    mServerUrl[0] = '\0';
  StringCchCopy(mLocalFileName, KMAX_PATH, localFileName);
  StringCchCopy(mServerOID, MAX_OID, serverOID);
  mServerFileOID[0] = '\0';
  mServerStorageOID[0] = '\0';
  mParentFolderOID[0] = '\0';
  mParentFullPathIndex = NULL;
  mTimeStamp = 0;
  memset(&mUploadFileDetails, 0, sizeof(mUploadFileDetails));
  memset(&mInitialLastModified, 0, sizeof(SYSTEMTIME));

  mParam1 = AllocString(p1);
  mParam2 = AllocString(p2);

  mErrorMessage = NULL;
  mScanData = NULL;
  mScanDataLength = 0;
  mScanFlag = 0;
  mResult = -1;
  mStopping = FALSE;
#ifdef USE_HTTP_DOWNLOADER
  mServerFolderScanResult = SFS_NO_ERROR;
  mDownloader = NULL;
#endif

  mState = LOADER_INIT;
  mDone = FALSE;
  mRetry = 0;
  mModifiedOnLoading = FALSE;

#ifdef USE_HTTP_DOWNLOADER
  if (mJob >= FOLDER_SCAN)
    mDownloader = new KWnetDownloader();
#endif

  // Create a manual-reset event that is not signaled at first to indicate 
  // the stopped signal of the service.
#ifndef USE_SCAN_BY_WORKER_THREAD
  m_hStoppedEvent = NULL;
  if (mJob < FOLDER_SCAN)
#endif
  {
    m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStoppedEvent == NULL)
    {
      StoreLogHistory(_T(__FUNCTION__), _T("CreateEvent fail"), SYNC_MSG_LOG);
      //  throw GetLastError();
    }
  }
}

KUpDownLoader::~KUpDownLoader(void)
{
  if (mScanData != NULL)
  {
    GlobalFree(mScanData);
    mScanData = NULL;
  }
  if (mParentFullPathIndex != NULL)
  {
    delete[] mParentFullPathIndex;
    mParentFullPathIndex = NULL;
  }
  if (mParam1 != NULL)
  {
    delete[] mParam1;
    mParam1 = NULL;
  }
  if (mParam2 != NULL)
  {
    delete[] mParam2;
    mParam2 = NULL;
  }

#ifdef USE_HTTP_DOWNLOADER
  if (mDownloader != NULL)
  {
    mDownloader->Clear();
    delete mDownloader;
    mDownloader = NULL;
  }
#endif

  if (mErrorMessage != NULL)
  {
    delete[] mErrorMessage;
    mErrorMessage = NULL;
  }
}

BOOL KUpDownLoader::GetProcessedFolderFileCount(int& folders, int& files)
{
#ifdef USE_HTTP_DOWNLOADER
  if ((mDownloader != NULL) && (mJob >= FOLDER_SCAN))
  {
    return mDownloader->GetProcessedFolderFileCount(folders, files);
  }
#endif

  return FALSE;
}

void KUpDownLoader::Start()
{
#ifndef USE_SCAN_BY_WORKER_THREAD
  if (mJob >= FOLDER_SCAN)
  {
    StartAsIn();
    mState = LOADER_ING;
    return;
  }
#endif

  CThreadPool::QueueUserWorkItem(&KUpDownLoader::ServiceWorkerThread, this);
}

static char base64Char(char c)
{
  if (c < 26)
    return(c + 'A');
  else if(c < 52)
    return(c - 26 + 'a');
  else if (c < 62)
    return(c - 52 + '0');
  else if (c == 62)
    return('+');
  else // if (c == 63)
    return('/');
}

static void EncodeBase64(char* buff, int buff_size, BYTE* src, int length)
{
  while(length > 0)
  {
    char d[3];
    if (length >= 3)
    {
      memcpy(d, src, 3);
      length -= 3;
      src += 3;
    }
    else
    {
      memset(d, 0, 3);
      memcpy(d, src, length);
      src += length;
      length = 0;
    }

    if (buff_size > 4)
    {
      // first 6 bit
      *buff++ = base64Char((d[0] >> 2) & 0x3F);
      *buff++ = base64Char(((d[0] << 4) & 0x30) | ((d[1] >> 4) & 0x0F));
      *buff++ = base64Char(((d[1] << 2) & 0x3C) | ((d[2] >> 6) & 0x03));
      *buff++ = base64Char(d[2] & 0x03F);
      buff_size -= 4;
    }
    else
    {
      break;
    }
  }
  *buff = '\0';
}

BOOL KUpDownLoader::StartAsIn()
{
  BOOL rtn = FALSE;
#ifdef USE_HTTP_DOWNLOADER
  if (mDownloader != NULL)
  {
    int length = 400;
    char buff[400] = { 0 };
    char *alloc_request = NULL;
    char *requestContext = buff;

    if (mJob == FOLDER_SCAN)
    {
      LPSTR folderOID = MakeUtf8String(mServerOID);

      // sprintf_s(requestContext, 150, "getModifiedFolderNDocument?folderOID=%s", folderOID); // without session key
      sprintf_s(requestContext, length, "localsync/getModifiedFolderNDocument?folderOID=%s", folderOID); // with session key

      if (folderOID != NULL)
        delete[] folderOID;
    }
#ifdef USE_BACKUP
    else if ((mJob == REQUEST_BACKUP_POLICY) || (mJob == REFRESH_BACKUP_POLICY))
    {
      sprintf_s(requestContext, length, "localsync/getBackupPolicies");
    }
    else if (mJob == REGIST_BACKUP_EVENT)
    {
      LPSTR param = MakeUtf8String(mParam1);
      sprintf_s(requestContext, length, "localsync/registBackupEvent?%s", param);
      if (param != NULL)
        delete[] param;
    }
    else if (mJob == REQUEST_RESTORE_INFO)
    {
      LPSTR param = MakeUtf8String(mParam1);
      sprintf_s(requestContext, length, "localsync/getRestoreInfo?restore-root-folder=%s", param);
      if (param != NULL)
        delete[] param;
    }
    else if (mJob == CLEAR_RESTORE_INFO)
    {
      LPSTR p1 = MakeUtf8String(mParam1);
      LPSTR p2 = MakeUtf8String(mParam2);
      sprintf_s(requestContext, length, "localsync/updateRestoreInfo?restore-folder=%s&status=%s", p1, p2);
      if (p1 != NULL)
        delete[] p1;
      if (p2 != NULL)
        delete[] p2;
    }
    else if (mJob == REQUEST_TRAFFIC_LEVEL)
    {
      sprintf_s(requestContext, length, "localsync/getTrafficLevel");
    }
    else if (mJob == REQUEST_UNREADED_MESSAGE)
    {
      sprintf_s(requestContext, length, "localsync/getUnreadedMessage");
    }
    else if (mJob == REQUEST_REMOVE_FILES)
    {
      LPSTR param1 = (LPSTR) mParam1;
      LPSTR param2 = (LPSTR) mParam2;
      int len = 60;
      if (param1 != NULL)
        len += strlen(param1);
      if (param2 != NULL)
        len += strlen(param2);

      if (len >= length)
      {
        alloc_request = new char[len+1];
        requestContext = alloc_request;
      }

      requestContext[0] = '\0';
      sprintf_s(requestContext, len, "localsync/requestFileRemove?");
      if (param1 != NULL)
      {
        strcat_s(requestContext, len, DA_NAME_FOLDER_OID);
        kstrcatChar(requestContext, len, DA_NAME_SEPARATOR);
        strcat_s(requestContext, len, param1);
      }
      if (param2 != NULL)
      {
        if (param1 != NULL)
          strcat_s(requestContext, len, "&");

        strcat_s(requestContext, len, DA_NAME_DOCUMENT_OID);
        kstrcatChar(requestContext, len, DA_NAME_SEPARATOR);
        strcat_s(requestContext, len, param2);
      }
    }
    else if (mJob == REQUEST_USE_BACKUP_FOLDER)
    {
      LPSTR p1 = MakeUtf8String(mParam1);
      LPSTR p2 = MakeUtf8String(mParam2);
      sprintf_s(requestContext, length, "localsync/requestUseBackupFolder?root-folder-oid=%s&pc-id=%s", p1, p2);
      if (p1 != NULL)
        delete[] p1;
      if (p2 != NULL)
        delete[] p2;
    }
#endif

    if (mTimeStamp > 0)
    {
      char str[60];
      sprintf_s(str, 60, "&dateTime=%lld", mTimeStamp);
      strcat_s(requestContext, length, str);
    }

    KString context(requestContext);

    LPSTR baseUrl = MakeUtf8String(mServerUrl);
    mDownloader->SetBaseUrl(baseUrl);
    delete[] baseUrl;

    rtn = mDownloader->DownloadRequest(context);

    mScanFlag = (mTimeStamp > 0) ? SCAN_ONLY_CHANGED : 0;

    if (lstrlen(mLocalFileName) > 0) // session-key value
    {
      char encoded_session_key[128];
      char *session_key = MakeUtf8String(mLocalFileName);
      EncodeBase64(encoded_session_key, 128, (BYTE *)session_key, strlen(session_key));
      delete[] session_key;

      mDownloader->SetSessionKey(encoded_session_key);
    }

    if (alloc_request != NULL)
      delete[] alloc_request;
  }
#endif // USE_HTTP_DOWNLOADER

  return rtn;
}

BOOL KUpDownLoader::ContinueAsIn()
{
#ifdef USE_HTTP_DOWNLOADER
  KStringList out_completedItems;
  KDownloaderError err = SFS_NO_ERROR;

  if (mDownloader->ProcessDownload(out_completedItems))
    return TRUE;

  int result = mDownloader->GetResult();
  if (result == 200)
  {
    if (mDownloader->m_ReceiveCount > 0)
      mScanData = mDownloader->GetDownloadData(mScanDataLength);
  }
  else
  {
    err = mDownloader->GetErrorCode();
    LPCSTR msg = mDownloader->GetErrorMessage();
    if (msg != NULL)
      mErrorMessage = MakeUnicodeString(msg);
  }

#ifdef _DEBUG_BUSY_RETRY
  if ((mJob == REQUEST_UNREADED_MESSAGE) && (mRetry < 5))
    result = 503;
#endif

  if (result == 200) // (mScanData != NULL)
  {
    mResult = R_SUCCESS;
  }
  else if (result == 503) // need retry
  {
    mResult = R_WARN_SERVER_BUSY;
    mDownloader->Clear();
  }
  else
  {
    mResult = FILE_SYNC_SFS_ERROR;
    mServerFolderScanResult = err;
  }
#endif // USE_HTTP_DOWNLOADER

#ifdef USE_SCAN_BY_WORKER_THREAD
  if (mResult >= R_SUCCESS)
    SetEvent(m_hStoppedEvent);
#endif

  return FALSE;
}

void KUpDownLoader::ServiceWorkerThread(void)
{
  // Perform main service function here...
#ifdef _DEBUG
  long start = GetTickCount();
#endif

#ifdef PERFORMANCE_ANALYSYS
    DWORD perf_start = GetTickCount();
#endif

  mState = LOADER_ING;

#ifdef USE_SCAN_BY_WORKER_THREAD
  if (mJob >= FOLDER_SCAN)
  {
    while (StartAsIn())
    {
      // KStringList out_completedItems;
      BOOL bContinue = TRUE;

      while(bContinue && !IsStopping())
      {
        bContinue = ContinueAsIn();
        if (!bContinue)
          Sleep(50);
      }

      if (mResult != R_WARN_SERVER_BUSY)
        break;

      Sleep(10000); // retry after 10 sec
      ++mRetry;
    }
  }
  else if (mJob == FILE_DOWN)
#else
  if (mJob == FILE_DOWN)
#endif
  {
    ERROR_ST err;
    LocalSync_InitEnv(&err);

#ifdef _DEBUG
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("LocalSync_FileDownload : file=%s, documentOID=%s, storageOID=%s\n"),
        mLocalFileName, mServerFileOID, mServerStorageOID);
      OutputDebugString(msg);
#endif

    if (!LocalSync_FileDownload(&err, mServerFileOID, mServerStorageOID, mLocalFileName))
    {
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("Failure on LocalSync_FileDownload : file=%s, documentOID=%s, storageOID=%s : error=%s\n"),
        mLocalFileName, mServerFileOID, mServerStorageOID, err.fault_string);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      if ((err.fault_string != NULL) && (lstrlen(err.fault_string) > 0))
      {
        AllocCopyStringTrim(&mErrorMessage, err.fault_string);
      }
      mResult = FILE_SYNC_DOWNLOAD_FAIL;
    }
    else
    {
      mResult = R_SUCCESS;
    }

    LocalSync_CleanEnv(&err);
  }
  else if (mJob == FILE_UP)
  {
    ERROR_ST err;
    LocalSync_InitEnv(&err);

#ifdef _DEBUG
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("LocalSync_FileUpload : file=%s, storageOID=%s\n"),
      mLocalFileName, mServerStorageOID);
    OutputDebugString(msg);
#endif

    // checkout first
    mResult = R_SUCCESS;
    if ((lstrlen(mServerFileOID) > 0) && (lstrlen(mServerStorageOID) > 0)) // replace existing file
    {
      bool IsCheckOut = FALSE;
      if (!LocalSync_IsCheckOut(&err, mServerFileOID, mParentFolderOID, mParentFullPathIndex, &IsCheckOut))
      {
        AllocCopyStringTrim(&mErrorMessage, err.fault_string);
        mResult = FILE_SYNC_ERROR_CHECKOUT;
      }
      else
      {
        if (IsCheckOut)
        {
          mResult = FILE_SYNC_ERROR_CHECKOUT;
        }
      }
      LocalSync_CleanEnv(&err);

      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("CheckOut %s for fileOID=%s, folderOID=%s, folderFullPathIndex=%s"),
        ((mResult == R_SUCCESS) ? _T("OK") : _T("Fail")),
        mServerFileOID, mParentFolderOID, mParentFullPathIndex);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      if (mErrorMessage != NULL)
      StoreLogHistory(_T(__FUNCTION__), mErrorMessage, SYNC_MSG_LOG);
    }

    // 업로드중 파일 변경되면 retry하는 부분을 위해 현재의 lastModified를 받아놓는다.
    GetFileModifiedTime(mLocalFileName, &mInitialLastModified, NULL);

    memset(&mUploadFileDetails, 0, sizeof(mUploadFileDetails));
    if ((mResult == R_SUCCESS) &&
      !LocalSync_FileUpload2(&err, mServerStorageOID, mLocalFileName, &mUploadFileDetails))
    {
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("Failure on LocalSync_FileUpload : %s : error=%s, fault_code=0x%x\n"), mLocalFileName, err.fault_string, err.fault_code);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      if ((err.fault_string != NULL) && (lstrlen(err.fault_string) > 0))
      {
        AllocCopyStringTrim(&mErrorMessage, err.fault_string);
      }

      // if ((mErrorMessage != NULL) && (CompareStringNC(mErrorMessage, _T("EAStorageServerNetworkOverloaded")) == 0))
      if (err.fault_code == ECM_AGENT_SERVER_BUSY)
        mResult = FILE_SYNC_UPLOAD_BUSY_REJECT;
      else
        mResult = FILE_SYNC_UPLOAD_FAIL;
    }
    else
    {
#ifdef _DEBUG // enable upload done log
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("LocalSync_FileUpload done : local-file=%s, serverStorageOID=%s"), mLocalFileName, mServerStorageOID);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
      mResult = R_SUCCESS;
    }

    LocalSync_CleanEnv(&err);
  }

#ifdef _DEBUG_UPDOWN_SPEED
  long end = GetTickCount();
  long ellapsed = (start <= end) ? (end - start) : (0xFFFFFFFF - start + end);

  TCHAR msg[120];
  StringCchPrintf(msg, 120, _T("UpDownLoader thread Ellapsed = %d\n"), ellapsed);
  OutputDebugString(msg);
#endif

#ifdef PERFORMANCE_ANALYSYS
    gpCoreEngine->performanceCheck_AddTransferTime(perf_start, GetTickCount());
#endif

  // Signal the stopped event.
  SetEvent(m_hStoppedEvent);

  // set done flag before PostMessage
  mDone = TRUE;

  // broadcast to main window, UpDownLoader thread complete
  // 타이머를 시작할 필요 없음.
  if ((mJob == FILE_UP) || (mJob == FILE_DOWN))
  {
#ifdef _DEBUG
    StoreLogHistory(_T(__FUNCTION__), _T("Post Loader Done"), SYNC_MSG_LOG);
#endif
    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_START_TIMER, 0);
  }
}

BOOL KUpDownLoader::IsDone()
{
#ifndef USE_SCAN_BY_WORKER_THREAD
  if (mJob >= FOLDER_SCAN)
  {
    OutputDebugString(_T("continue folder scan receive\n"));
    return !ContinueAsIn();
  }
#endif

  return mDone;
}

void KUpDownLoader::OnStop()
{
  /*
  TCHAR msg[300] = _T("KUpDownLoader in OnStop : ");
  StringCchCat(msg, 300, mLocalFileName);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  */
  // Indicate that the service is stopping and wait for the finish of the 
  // main service function (ServiceWorkerThread).

#ifdef USE_HTTP_DOWNLOADER
  if ((mJob >= FOLDER_SCAN) && (mDownloader != NULL))
    mDownloader->CloseDownloadSocket(mResult != R_SUCCESS);
#endif

  mState = LOADER_CLOSING;
  //m_fStopping = TRUE;

  if (m_hStoppedEvent != NULL)
  {
    if (WaitForSingleObject(m_hStoppedEvent, 10 * 1000 /*INFINITE*/) != WAIT_OBJECT_0)
    {
      StoreLogHistory(_T(__FUNCTION__), _T("Killed forcely"), SYNC_MSG_LOG);
      //throw GetLastError();
    }
  }
}

BOOL KUpDownLoader::OnClose()
{
#ifdef USE_HTTP_DOWNLOADER
  if ((mJob >= FOLDER_SCAN) && (mDownloader != NULL))
  {
    delete mDownloader;
    mDownloader = NULL;
  }
#endif
  return TRUE;
}

