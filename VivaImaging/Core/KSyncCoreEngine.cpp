#include "stdafx.h"
#include <tchar.h>

#include "KSyncCoreEngine.h"
#include "KSettings.h"
#include "KConstant.h"
#include "KMetadataInfo.h"

#include "../Platform/Reg.h"

#ifdef USE_SYNC_STORAGE
#include "KSyncRootStorageFile.h"
#include "KSyncRootStorageDB.h"
#endif

// DestinyLocalSyncLib
#include "DestinyLocalSyncLib/common/DestinyLocalSyncIFShared.h"
#ifdef USE_SYNC_OVERLAY
#include "../LocalSyncOverIcon/LocalSyncStatus.h"
#endif

#ifdef USE_SYNC_GUI
#include "PanNsIdl/PanNsIdlUtil.h"
#endif

#include <strsafe.h>
#include <Shlwapi.h>
#include <WinDef.h>
#include <Shellapi.h>

#ifndef BOOL
#define BOOL int
#endif

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

// #define _DEBUG_LOGOFF

// static BOOL dlsl_IsFolderExist(LPTSTR folder_oid, LPTSTR sub_folder_name);

LRESULT __stdcall CallbackFileSyncProgress(LS_NOTIFY_MESSAGE message, WPARAM wParam, LPARAM lParam)
{
  LRESULT rtn = 1;

  if (message == LSNM_FileSyncProgress)
  {
    LPCWSTR filename = (LPCWSTR) wParam;
    int progress = (int) lParam;

#ifdef _DEBUG
    TCHAR msg[300];
    StringCchPrintf(msg, 300, _T("File Up/Down %s : %d\n"), filename, progress);
    OutputDebugString(msg);
#endif

    if (gpCoreEngine != NULL)
    {
      rtn = gpCoreEngine->OnFileSyncProgress(filename, progress);
      if (rtn == 0)
      {
        TCHAR msg[300] = _T("File Up/Down Stopped : ");
        StringCchCat(msg, 300, filename);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#ifdef _DEBUG
        // StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        OutputDebugString(msg);
        OutputDebugString(_T("\n"));
#endif
      }
    }
  }
  else if (message == LSNM_AfterLogin)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("LSNM_AfterLogin"), SYNC_MSG_LOG);

    //if (TRUE)gpCoreEngine->mStatus < SYNC_STATUS_IDLE)
    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_LOGIN, 0);

    if (wParam != NULL)
      gpCoreEngine->SetLoginUserInfo((struct SessionValue*)wParam);
  }
  else if (message == LSNM_SessionKeyChanged)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("LSNM_SessionKeyChanged"), SYNC_MSG_LOG);

    if (wParam != NULL)
      gpCoreEngine->SetLoginUserInfo((struct SessionValue*)wParam);
  }
  else if (message == LSNM_AfterLogout)
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 128, _T("LSNM_AfterLogout(wp=0x%llx, lp=0x%llx)"), wParam, lParam);
    LPARAM key = LOGOUT_BY_USER;
    if (lParam == LSLS_SESSION_TIMEOUT)
    {
      key = LOGOUT_BY_TIMEOUT;
      StringCchCat(msg, 128, _T(" timeout"));
    }
    else if (lParam == LSLS_LOCAL_LOGIN)
    {
      key = LOGOUT_BY_LOCAL_LOGIN;
      StringCchCat(msg, 128, _T(" local-login"));
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

   gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_LOGOUT, key);
    gpCoreEngine->PostMessageToModal(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_LOGOUT, 0);
  }
  else if (message == LSNM_DDPolicyInfo)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("LSNM_DDPolicyInfo"), SYNC_MSG_LOG);
    gpCoreEngine->UpdateServerPolicyInfo((void *)wParam, TRUE);
  }
  else if (message == LSNM_LocaleChanged)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("LSNM_LocaleChanged"), SYNC_MSG_LOG);
    // gpCoreEngine->RefreshSyncPolicyInfo(REFRESH_LOCALE|REFRESH_FORCE);
    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_LOCALE_CHANGED, 0);
  }
  else if (message == LSNM_DriveReadyToService)
  {
    TCHAR msg[128];// = _T("LSNM_DriveReadyToService");
    StringCchPrintf(msg, 128, _T("LSNM_DriveReadyToService(wp=0x%llx, lp=0x%llx)"), wParam, lParam);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_DRIVE_READY, 0);

#ifdef USE_SECURE_DISK_BACKUP
    // 보안드라이브 백업 여부에 따라 모니터링 업데이트
    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_DEVICE_READY, 0);
#endif
  }
#ifdef USE_SYNC_GUI
  else if (message == LSNM_MenuClicked)
  {
    /*
    StringCchPrintf(msg, 256, _T("Tray ContextMenu %d\n"), (int)wParam);
    OutputDebugString(msg);
    */
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Tray ContextMenu %d"), (int)wParam);

    if (lstrlen(mUserOid) > 0) // only when login_state
    {
      gpCoreEngine->PostMessageToMain(WM_TRAYICON_MESSAGE, wParam, lParam);
    }
    else
    {
      StringCchCat(msg, 256, _T(" : Ignored"));
    }
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
#endif
  return rtn;
}

KSyncCoreEngine::KSyncCoreEngine(void)
{
#if defined(USE_SCAN_BY_HTTP_REQUEST)
  m_SessionKey[0] = '\0';
  m_RpcServerUrl = NULL;
#endif

  mEngineReady = 0;

#ifdef USE_LOGIN_MUTEX
  mhLoginMutex = NULL;
#endif

#ifdef USE_SYNC_GUI
  mCurrentSyncRootFolderItem = NULL;
  mhModalMutex = NULL;
  mpModalWnd = NULL;
  memset(&m_login_at, 0, sizeof(m_login_at));
  memset(&m_timeout_logout_at, 0, sizeof(m_timeout_logout_at));
  mAgentTimeoutLimit = (int)(30000 * 0.8); // default timeout value

#ifdef USE_LAZY_UPLOAD
  mCurrentTrafficLevel = mMaxTrafficLevel = 0;
  mTrafficLevelIdx = 0;
  memset(mTrafficLevelStack, 0, sizeof(int) * TRAFFIC_LEVEL_MEAN_STEP);
#endif

  mServerBusy = FALSE;

#endif // USE_SYNC_GUI

#ifdef _DEBUG_AA
  mUseAutoSync = 1;
#endif

#if defined(DIR_MONITOR_ON_ENGINE) || defined(DRIVE_MONITOR_ON_ENGINE)
  mpMonitoringSyncFolders = NULL;
#endif
}

KSyncCoreEngine::~KSyncCoreEngine(void)
{
  ClearInternal();
}

void KSyncCoreEngine::ClearInternal()
{
#if defined(USE_SCAN_BY_HTTP_REQUEST)
  m_SessionKey[0] = '\0';
  if (m_RpcServerUrl != NULL)
  {
    delete[] m_RpcServerUrl;
    m_RpcServerUrl = NULL;
  }
#endif

#ifdef USE_SYNC_GUI
  mCurrentSyncRootFolderItem = NULL;
  if (mScheduledSyncList.size() > 0)
    FreeSyncRootItemList(mScheduledSyncList);
  FreeUserNames();
#endif
}

void KSyncCoreEngine::ClearExternal()
{
  StoreLogHistory(_T(__FUNCTION__), _T("Engine Closed"), SYNC_MSG_LOG);
#if defined(DIR_MONITOR_ON_ENGINE) || defined(DRIVE_MONITOR_ON_ENGINE)
  mDirectoryMonitor.FreeAllMonitorItem();

  if (mpMonitoringSyncFolders != NULL)
  {
    delete[] mpMonitoringSyncFolders;
    mpMonitoringSyncFolders = NULL;
  }
#endif
}

BOOL KSyncCoreEngine::Ready(BOOL asMain)
{
  KSyncCoreBase::Ready(asMain);

  if (mStatus == SYNC_STATUS_INIT)
  {
    if (!LocalSync_DLLInit(asMain ? CallbackFileSyncProgress : NULL))
    // 아직 이미징용 LSIF 준비 안됨.
    // if (!LocalSync_DLLInitEx(asMain ? CallbackFileSyncProgress : NULL, (IF_TYPE)((int)IF_TYPE::Imaging + mSessionIdx)))
    {
      TCHAR msg[] = _T("Failure on LocalSync_DLLInit()");
      OutputDebugString(msg);
      OutputDebugString(_T("\n"));
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_EVENT_ERROR);
      return FALSE;
    }

    mStatus = SYNC_STATUS_OFFLINE;
    // clear tray sync-state icon
#ifdef USE_SYNC_GUI
    if (asMain)
      BroadcastToMonitor(BM_SYNC_DONE, 0);
#endif
  }

  if (mStatus == SYNC_STATUS_OFFLINE)
  {
    InitializeServerSetting(3, 0); // periodic check
  }
  return TRUE;
}

BOOL KSyncCoreEngine::CheckEngineReady(int state)
{
  mEngineReady |= state;

  return ((mStatus < SYNC_STATUS_LOGGED) &&
    (mEngineReady == (ENGINE_LOGIN | ENGINE_DRIVE_READY)));
}

DWORD KSyncCoreEngine::MakeSessionID()
{
  mSessionIdx = RegCreateSessionID();
  return mSessionIdx;
}

void KSyncCoreEngine::ClearSessionID()
{
  if (mSessionIdx != INVALID_SESSION_ID)
    RegClearSessionID(mSessionIdx);
  mSessionIdx = INVALID_SESSION_ID;
}

BOOL KSyncCoreEngine::OnLogged(BOOL bOn)
{
  if (bOn)
  {
    if (m_RpcServerUrl != NULL)
    {
      delete[] m_RpcServerUrl;
      m_RpcServerUrl = NULL;
    }

    // refresh Streaming Server IP and address
    TCHAR url[KMAX_PATH] = { 0 };
    RegGetServerFolderScanUrl(url, KMAX_PATH);

    if (lstrlen(url) > 0)
    {
      AllocCopyString(&m_RpcServerUrl, url);
    }

  }
  else
  {
    ClearInternal();
  }

  mStatus = SYNC_STATUS_IDLE; // 동기화 준비하고 대기 상태
  return TRUE;
}

BOOL KSyncCoreEngine::CheckAutoLogin(BOOL autoLogin)
{
  TCHAR user_id[MAX_USERNAME];

  TCHAR msg[64];
  StringCchPrintf(msg, 64, _T("autoLogin=%d"), autoLogin);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  if (!LocalSync_GetLogInID(/*mUserId*/user_id, mUserOid,!!autoLogin))
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Fail on LocalSync_GetLogInID()"), SYNC_MSG_LOG);

    // is need InitializeServerSetting???
    InitializeServerSetting(0, LSLS_LOGOUT);
    return FALSE;
  }
  return TRUE;
}


void KSyncCoreEngine::InitializeServerSetting(int set, int logoutMethod)
{
  TCHAR msg[256] = _T("LocalSyncEngine ");
  BOOL byTimeOut = (logoutMethod == LSLS_SESSION_TIMEOUT);

  mHomeFolder[0] = '\0';
  mUseLocalSync = FALSE;

  mUserFlag = 0;
  mSecurityDrive = ' ';
  mEcmDrive = ' ';
  mEcmVirtualDrive = ' ';
  gECMDriveName[0] = '\0';
  gSecurityDriveName[0] = '\0';

  mMaxSyncFileSizeMB = -1; // not check
  //mUserId[0] = '\0';
  mUserOid[0] = '\0';

  mStatus = SYNC_STATUS_OFFLINE;

  if (set > 0)
  {

#ifdef _DEBUG_LOGOFF
    if (set == 4) // fake login
    {
      StringCchCopy(mUserOid, MAX_OID, _T("5level"));
      mUseLocalSync = 1;

      mUserFlag = FLAG_READABLE_NORMAL_DISK|FLAG_WRITABLE_NORMAL_DISK |
        FLAG_READABLE_ECM_DISK|FLAG_WRITABLE_ECM_DISK;

      mSecurityDrive = ' ';
      mEcmDrive = 'S';
      mEcmVirtualDrive = ' ';

      goto ready_login_ready;
    }
#endif

    TCHAR user_id[MAX_USERNAME];
    if (LocalSync_GetLogInID(/*mUserId*/user_id, mUserOid, false))
    {
      GetHomeFolder(FALSE); // create it only needed

      // always use
      mUseLocalSync = 1;
      // ECM 권한이 있으면
      // mUseLocalSync = (mUserFlag & FLAG_READABLE_ECM_DISK);

      if (set > 1)
        RefreshSyncPolicyInfo(REFRESH_ALL|REFRESH_FORCE);
    }
    else
    {
      if (set < 3)
        StoreLogHistory(_T(__FUNCTION__), _T("Fail on LocalSync_GetLogInID()"), SYNC_MSG_LOG);
    }

    if (mUseLocalSync)
    {
      GetServerConfigValues();
    }
    else
    {
      //StoreLogHistory(_T(__FUNCTION__), _T("NotUseLocalSync"), SYNC_MSG_LOG);
    }

#ifdef USE_SYNC_GUI
    // update login time on normal login
    if (!byTimeOut)
      GetLocalTime(&m_login_at);
#endif

    GetHomeFolder(FALSE); // create it only needed

    if (lstrlen(mUserOid) > 0)
    {
      mStatus = SYNC_STATUS_CONNECTED;

      TCHAR msg[120] = _T("Connected with ");
      StringCchCat(msg, 120, mUserName);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

#ifdef USE_LOGIN_MUTEX
      // create Login Mutex
      if (mhLoginMutex == NULL)
      {
        mhLoginMutex = CreateMutex(NULL, TRUE, LOCALSYNC_LOGIN_MUTEX);
      }
#endif
    }
  }
  else
  {
    // do not clear login information, it will be used on OverIcon, ShellExt
#ifdef USE_LOGIN_MUTEX
    if (mhLoginMutex != NULL)
    {
      ReleaseMutex(mhLoginMutex);
      CloseHandle(mhLoginMutex);
      mhLoginMutex = NULL;
    }
#endif
  }
}

// temporarily used
const TCHAR SERVER_KEY_INCLUDE_MASKS[] = _T("file.EnableFileExtensions");
const TCHAR SERVER_KEY_USE_INCLUDE_MASKS[] = _T("file.UseFileExtensionCheck");

void KSyncCoreEngine::GetServerConfigValues()
{
  // when need to limit file size
#ifdef USE_MAX_ECM_FILE_SIZE
  LPTSTR str;

  str = GetServerConfig(SERVER_KEY_USE_FILE_SIZE_CHECK);
  if (str != NULL)
  {
    if (_tcsicmp(str, _T("true")) == 0)
    {
      LPTSTR val = GetServerConfig(SERVER_KEY_MAX_FILE_SIZE);
      if (val != NULL)
      {
        if (lstrlen(val) > 0)
        {
          _stscanf_s(val, _T("%d"), &mMaxSyncFileSizeMB);
        }
        delete[] val;
      }
    }
    delete[] str;
  }
#endif
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

int String2Permission(LPCTSTR permissions)
{
  LPCTSTR p = permissions;
  int len;
  int pv = 0;
  int count = sizeof(permission_str_array) / sizeof(LPTSTR);

  if (p == NULL)
    return 0;

  while (*p != '\0')
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
BOOL IsNormalStatus(LPCTSTR folderStatus)
{
  if ((folderStatus != NULL) && (lstrlen(folderStatus) > 0))
    return (StrChr(folderStatus, 'N') != NULL);
  return FALSE;
}

BOOL CopyServerFolderInfo(KSyncRootFolderItem *root, SERVER_INFO& OUT info, XSYNCFOLDER_ST* syncFolder, BOOL asAbsPath)
{
  BOOL rtn = FALSE;
  memset(&info, 0, sizeof(SERVER_INFO));
  if (syncFolder != NULL)
  {
    if (IsNormalStatus(syncFolder->folderStatus))
    {
      info.Permissions = String2Permission(syncFolder->permissions);
      StringCchCopy(info.UserOID, MAX_OID, syncFolder->creatorOID);
      AllocCopyString(&info.pFilename, syncFolder->name);

#ifdef _USE_SYNC_ENGINE
      if (asAbsPath)
      {
        info.pFullPath = AllocString(syncFolder->folderFullPath);
      }
      else if (root != NULL)
      {
        info.pFullPath = KSyncItem::AllocServerSideRelativePath(root->GetServerFullPath(), syncFolder->folderFullPath, NULL);
      }
#else
      info.pFullPath = AllocString(syncFolder->folderFullPath);
#endif

      info.pFullPathIndex = AllocString(syncFolder->fullPathIndex);

      FILETIME ft;
      ft.dwHighDateTime = syncFolder->lastModifiedAtHigh;
      ft.dwLowDateTime = syncFolder->lastModifiedAtLow;
      FileTimeToSystemTime(&ft, &info.FileTime);
      rtn = TRUE;
    }
  }
  return rtn;
}

#define THREAD_CALL_AGENT_GUI
//#define CALL_AGENT_GUI

#ifdef THREAD_CALL_AGENT_GUI
#include <process.h>

unsigned __stdcall procSelectFolderThread(void *p)
{
  THREAD_CALL_DATA *cd = (THREAD_CALL_DATA *)p;

  ERROR_ST err;
  LocalSync_InitEnv(&err);
  BOOL r = LocalSync_SelectServerFolder(&err, cd->folder_oid, (LONGLONG)cd->hwnd);
  LocalSync_CleanEnv(&err);

  if (cd->hwnd != NULL)
  {
    if (!r) // if canceled
      cd->folder_oid[0] = '\0';
    PostMessage(cd->hwnd, THREADED_CALL_RESULT, SELECT_FOLDER, (LPARAM)cd);
  }

  //OutputDebugString(_T("endThreadProc\n"));
  StoreLogHistory(_T(__FUNCTION__), cd->folder_oid, SYNC_MSG_LOG);
  return r;
}
#endif // THREAD_CALL_AGENT_GUI

// SyncRootFolder will be created on this folder
BOOL KSyncCoreEngine::SelectSyncServerFolderLocation(HWND hwnd, LPCTSTR localFolder, LPCTSTR server_folder_oid)
{
#ifdef THREAD_CALL_AGENT_GUI
  THREAD_CALL_DATA *p = (THREAD_CALL_DATA *)malloc(sizeof(THREAD_CALL_DATA));
  memset(p, 0, sizeof(THREAD_CALL_DATA));
  p->hwnd = hwnd;
  if (localFolder != NULL)
    StringCchCopy(p->local_path, MAX_PATH, localFolder);
  if (server_folder_oid != NULL)
    StringCchCopy(p->folder_oid, MAX_OID, server_folder_oid);

  //OutputDebugString(_T("beginThread\n"));
  uintptr_t r = _beginthreadex(NULL, 0, procSelectFolderThread, p, 0, NULL);

  TCHAR msg[128];
  StringCchPrintf(msg, 128, _T("Thread procSelectFolderThread call result : %lld"), r);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  return FALSE;

#elif defined(CALL_AGENT_GUI)
  ERROR_ST err;
  LocalSync_InitEnv(&err);
  BOOL r = LocalSync_SelectServerFolder(&err, (LPTSTR)server_folder_oid, (LONGLONG)hwnd);
  LocalSync_CleanEnv(&err);
  return r;
#else
  // direct map to test folder for test
  StringCchCopy(server_folder_oid, MAX_OID, _T("1KhCch0i0p_")); // _T("1Kh1sRAK09u"));
  return TRUE;
#endif
}

BOOL KSyncCoreEngine::SyncServerCreateFolder(KSyncRootFolderItem *root, LPCTSTR IN server_folder_oid, LPCTSTR IN server_folder_index, LPCTSTR IN name,
  LPTSTR OUT out_child_folder_oid, SERVER_INFO& OUT out_child_folder_info, LPTSTR* OUT pErrorMsg)
{
#ifdef _DEBUG
  if ((server_folder_index == NULL) || (lstrlen(server_folder_index) == 0))
  {
    OutputDebugString(_T("No ServerFullPathIndex on ServerCreateFolder"));
    return FALSE;
  }
  if ((server_folder_oid == NULL) || (lstrlen(server_folder_oid) == 0))
  {
    OutputDebugString(_T("No ServerOID on ServerCreateFolder"));
    return FALSE;
  }
#endif

  ERROR_ST err;
  XSYNCFOLDER_ST* syncFolder = NULL;
  LocalSync_InitEnv(&err);
  BOOL r = LocalSync_CreateFolder(&err, server_folder_oid, name, server_folder_index, &syncFolder);
  if (!r)
  {
#ifdef USE_BACKUP
#endif // USE_BACKUP
    if (err.fault_code == ECM_AGENT_SAME_FOLDER_EXIST) // if folder already exist, use it anyway
    {
      LocalSync_CleanEnv(&err);
      XSYNCFOLDERLIST_ST* syncFolderList = NULL;
      if (LocalSync_GetFolders(&err, server_folder_oid, &syncFolderList) &&
        (syncFolderList->folders != NULL))
      {
        XSYNCFOLDER_ST *pChilds = *(syncFolderList->folders);
        for (int i = 0; i < syncFolderList->folderCount; i++)
        {
          if (CompareStringICNC(pChilds[i].name, name) == 0)
          {
            StringCchCopy(out_child_folder_oid, MAX_OID, pChilds[i].oid);
            CopyServerFolderInfo(root, out_child_folder_info, &pChilds[i], FALSE);
            break;
          }
        }
        LocalSync_DestroyXSYNCFOLDERLIST_ST(syncFolderList);
      }

      if (lstrlen(out_child_folder_info.pFullPathIndex) > 0)
      {
        TCHAR msg[512];
        StringCchPrintf(msg, 512, _T("Resume LocalSync_CreateFolder : %s - %s"), out_child_folder_oid, name);
        LocalSync_CleanEnv(&err);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        return TRUE;
      }
    }

    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_CreateFolder : %s - %s : error=%s"), server_folder_oid, name, err.fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    if (pErrorMsg != NULL)
      AllocCopyStringTrim(pErrorMsg, err.fault_string);
  }
  else
  {
    StringCchCopy(out_child_folder_oid, MAX_OID, syncFolder->oid);

    CopyServerFolderInfo(root, out_child_folder_info, syncFolder, FALSE);

    LocalSync_DestroyXSYNCFOLDER_ST(syncFolder);
  }
  LocalSync_CleanEnv(&err);
  return r;
}

BOOL KSyncCoreEngine::SyncServerGetFolderName(LPCTSTR server_folder_oid, LPTSTR folder_name, BOOL fullPath, LPTSTR path_index)
{
  ERROR_ST err;
  XSYNCFOLDER_ST *syncFolder = NULL;

  if ((server_folder_oid == NULL) || (lstrlen(server_folder_oid) == 0))
    return 0;

  LocalSync_InitEnv(&err);
  BOOL r = LocalSync_GetFolder(&err, server_folder_oid, &syncFolder);
  if (r)
  {
    StringCchCopy(folder_name, KMAX_PATH, fullPath ? syncFolder->folderFullPath : syncFolder->name);
    if (path_index != NULL)
      StringCchCopy(path_index, KMAX_PATH, syncFolder->fullPathIndex);
  }
  else
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_GetFolder : %s : error=%s"), server_folder_oid, err.fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
  }
  LocalSync_DestroyXSYNCFOLDER_ST(syncFolder);
  LocalSync_CleanEnv(&err);
  return r;
}

int KSyncCoreEngine::SyncServerGetDocTypesForFolder(LPCTSTR serverFolderOID, LPTSTR* pDocTypes)
{
  int r = R_SUCCESS;
  ERROR_ST err;
  LPWSTR outRes = NULL;

  LocalSync_InitEnv(&err);
  if (LocalSync_GetDocTypeInfosByFolderOID(&err, serverFolderOID, &outRes))
  {
    int len = lstrlen(outRes);
    *pDocTypes = new TCHAR[len + 1];
    StringCchCopy(*pDocTypes, len + 1, outRes);
    LocalSync_DestroyStringW(outRes);
  }
  else
  {
    r = FAILURE_ON_GETDOCTYPES;
  }
  LocalSync_CleanEnv(&err);
  return r;
}

int KSyncCoreEngine::SyncServerGetDocTypeData(LPCTSTR docTypeOID, void* ppXDocType)
{
  int r = R_SUCCESS;
  ERROR_ST err;
  LPWSTR outRes = NULL;

  LocalSync_InitEnv(&err);
  if (LocalSync_GetDocTypeData(&err, docTypeOID, (XDOCTYPE_ST **)ppXDocType))
  {

    //LocalSync_DestroyXDOCTYPE_ST(pXDocType);
  }
  else
  {
    r = FAILURE_ON_GETDOCTYPEDATA;
  }
  LocalSync_CleanEnv(&err);
  return r;
}

int KSyncCoreEngine::SyncServerGetDocTypeDatasForAll(void* ppXDocType, int* pCount)
{
  int r = R_SUCCESS;
  ERROR_ST err;
  LPWSTR outRes = NULL;
  int docTypeSize = 0;

  LocalSync_InitEnv(&err);
  if (LocalSync_GetAllDocType(&err, &docTypeSize, (XDOCTYPE_ST**)ppXDocType))
  {
    if (pCount != NULL)
      *pCount = docTypeSize;
  }
  else
  {
    r = FAILURE_ON_GETDOCTYPES;
  }
  LocalSync_CleanEnv(&err);
  return r;
}

int KSyncCoreEngine::SyncServerFreeDocTypeData(void* pXDocType)
{
  if (pXDocType != NULL)
  {
    LocalSync_DestroyXDOCTYPE_ST((XDOCTYPE_ST*)pXDocType);
    return R_SUCCESS;
  }
  return 1;
}

int KSyncCoreEngine::SyncServerFreeDocTypeData(void* pXDocType, int size)
{
  if (pXDocType != NULL)
  {
    LocalSync_DestroyXDOCTYPEARRAY_ST((XDOCTYPE_ST*)pXDocType, size);
    return R_SUCCESS;
  }
  return 1;
}

BOOL CreateDocumentWithExtAttr(BOOL bCreate, ERROR_ST* err, LPCWSTR szFolderOID, LPCWSTR szFileName, LONGLONG fileSize, LPCWSTR serverFileOID, LPCWSTR szStorageFileID,
  LONGLONG localLastModifiedAt, LONGLONG localCreatedAt, LPCWSTR folderIndex, KEcmDocTypeInfo* pInfo, KMetadataInfo* pMetadata, OUT XSYNCDOCUMENT_ST** ppXDoc)
{
  XSYNCDOCUMENTWITHEXTATTR_ST xdoc;
  XSYNCDOCUMENTWITHEXTATTR_ST* pXDoc = NULL;

  int numberOfColumns = pInfo->GetColumnNumber();

  memset(&xdoc, 0, sizeof(XSYNCDOCUMENTWITHEXTATTR_ST));
  TCHAR sizestr[32];
  StringCchPrintf(sizestr, 32, _T("%lld"), fileSize);

  xdoc.folderOID = szFolderOID;
  xdoc.folderFullPath = folderIndex;
  xdoc.creatorOID = mUserOid;
  xdoc.name = szFileName;
  xdoc.fileOID = serverFileOID;
  xdoc.fileSize = sizestr;
  xdoc.storageFileID = szStorageFileID;
  xdoc.lastModifierOID = mUserOid;
  xdoc.lastModifierName = mUserName;
  xdoc.creatorName = mUserName;
  xdoc.docStatus = _T("N");
  xdoc.permissions = _T("r,w,d,s");
  xdoc.lastModifiedAt = localLastModifiedAt;
  xdoc.localCreatedAt = localCreatedAt;
  xdoc.docTypeOID = pInfo->mDocTypeOid;
  XEXTCOLUMNVALUE_ST* pColumns = (XEXTCOLUMNVALUE_ST *) new XEXTCOLUMNVALUE_ST[numberOfColumns];
  xdoc.extColumnValuesCNT = numberOfColumns;
  xdoc.extColumnValues = &pColumns;

  int i = 0;
  TCHAR msg[256];

  StringCchPrintf(msg, 256, _T("CreateDoc with DocType : %s"), pInfo->mDocTypeOid);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  KEcmExtDocTypeColumnIterator it = pInfo->mArray.begin();

  while (it != pInfo->mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    MetaKeyNameValue* v = pMetadata->Find(p->key);

    pColumns[i].update = true;
    pColumns[i].key = AllocString(p->key);
    pColumns[i].value = (v != NULL) ? AllocString(v->value) : NULL;
    pColumns[i].valuetype = AllocString(KEcmDocTypeInfo::GetValueTypeName(p->type));

    StringCchPrintf(msg, 256, _T("DocType column : %s, %s, %s"), p->key, pColumns[i].value, pColumns[i].valuetype);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    i++;
  }

  BOOL result;
  
  if (bCreate)
    result = LocalSync_CreateDocWithExtAttr(err, &xdoc, &pXDoc);
  else
    result = LocalSync_UpdateDocWithExtAttr(err, &xdoc, &pXDoc);

  if (result == FALSE)
  {
    StringCchPrintf(msg, 256, _T("LocalSync_CreateDocWithExtAttr failure : %s"), err->fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  return result;
}

int KSyncCoreEngine::SyncServerUploadFile(LPCTSTR folderOid, LPCTSTR folderIndex, LPTSTR docOID, LPTSTR serverFileOID,
  LPTSTR serverStorageOID, LPCTSTR name, LPCTSTR localFileName, KEcmDocTypeInfo* pInfo, KMetadataInfo* pMetadata, LPTSTR* pErrorMessage)
{
  ERROR_ST err;
  BOOL new_document = TRUE;

  LocalSync_InitEnv(&err);
  if ((lstrlen(serverFileOID) > 0) && (lstrlen(serverStorageOID) > 0)) // replace existing file
  {
    bool IsCheckOut = FALSE;
    if (!LocalSync_IsCheckOut(&err, serverFileOID, folderOid, folderIndex, &IsCheckOut) || IsCheckOut)
    {
      TCHAR msg[256] = _T("Checkout fail on : ");
      if (name != NULL)
      {
        StringCchCat(msg, 256, name);
        StringCchCat(msg, 256, _T(","));
      }
      if (err.fault_string != NULL)
      {
        StringCchCat(msg, 256, err.fault_string);
      }
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
      LocalSync_CleanEnv(&err);
      return FILE_SYNC_ERROR_CHECKOUT;
    }
    LocalSync_CleanEnv(&err);

    TCHAR msg[256] = _T("Checkout done on : ");
    StringCchCat(msg, 256, name);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    // save ExtDoc always
    // new_document = FALSE;
  }

  LocalSyncUploadFileDetails2 uploadFileDetails;
  memset(&uploadFileDetails, 0, sizeof(LocalSyncUploadFileDetails2));
  TCHAR new_serverStorageOID[MAX_STGOID] = _T("1234567890");
  int r = 0;
  TCHAR msg[512];

  if (lstrlen(serverStorageOID) > 0)
    StringCchCopy(new_serverStorageOID, MAX_STGOID, serverStorageOID);

  if (!LocalSync_FileUpload3(&err, new_serverStorageOID, localFileName, &uploadFileDetails))
  {
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_FileUpload : %s : error=%s, fault_code=0x%x\n"), localFileName, err.fault_string, err.fault_code);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    if ((pErrorMessage != NULL) && (err.fault_string != NULL) && (lstrlen(err.fault_string) > 0))
    {
      AllocCopyStringTrim(pErrorMessage, err.fault_string);
    }

    // if ((mErrorMessage != NULL) && (CompareStringNC(mErrorMessage, _T("EAStorageServerNetworkOverloaded")) == 0))
    r = (err.fault_code == ECM_AGENT_SERVER_BUSY) ? FILE_SYNC_UPLOAD_BUSY_REJECT : FILE_SYNC_UPLOAD_FAIL;
  }
  else
  {
    StringCchCopy(serverStorageOID, MAX_STGOID, new_serverStorageOID);

    BOOL a = FALSE;
    XSYNCDOCUMENT_ST *syncDoc = NULL;
    __time64_t localModifiedTime = MAKE_INT64(uploadFileDetails.localLastWriteTime.dwLowDateTime, uploadFileDetails.localLastWriteTime.dwHighDateTime);
    __time64_t localCreatedTime = MAKE_INT64(uploadFileDetails.localCreationTime.dwLowDateTime, uploadFileDetails.localCreationTime.dwHighDateTime);

    // 임시, CreateDocumentWithExtAttr 오류
    //pInfo = NULL;

    if (new_document)
    {
      if ((pMetadata != NULL) && (pMetadata->mVector.size() > 0) &&
        (pInfo != NULL))
      {
        a = CreateDocumentWithExtAttr(TRUE, &err, folderOid, name, uploadFileDetails.localFileSize, serverFileOID,
          serverStorageOID, localModifiedTime, localCreatedTime, folderIndex, pInfo, pMetadata, &syncDoc);
      }
      else
      {
        a = LocalSync_CreateDocument2(&err, folderOid, name, uploadFileDetails.localFileSize, serverStorageOID,
          localModifiedTime, localCreatedTime, folderIndex, &syncDoc);
      }
    }
    else
    {
      if ((pMetadata != NULL) && (pMetadata->mVector.size() > 0) &&
        (pInfo != NULL))
      {
        a = CreateDocumentWithExtAttr(FALSE, &err, folderOid, name, uploadFileDetails.localFileSize, serverFileOID,
          serverStorageOID, localModifiedTime, localCreatedTime, folderIndex, pInfo, pMetadata, &syncDoc);
      }
      else
      {
        a = LocalSync_UpdateDocument(&err, serverFileOID, name, uploadFileDetails.localFileSize,
          serverStorageOID, localModifiedTime, folderOid, folderIndex, &syncDoc);
      }
    }

    if ((pErrorMessage != NULL) && err.fault_occurred && (err.fault_string != NULL) && (lstrlen(err.fault_string) > 0))
    {
      AllocCopyStringTrim(pErrorMessage, err.fault_string);
    }


    if (a && (syncDoc != NULL) && (syncDoc->fileOID != NULL))
    {
      StringCchCopy(serverFileOID, MAX_OID, syncDoc->fileOID);
      StringCchCopy(docOID, MAX_OID, syncDoc->oid);
      StringCchCopy(serverStorageOID, MAX_STGOID, syncDoc->storageFileID);
    }

    LocalSync_DestroyXSYNCDOCUMENT_ST(syncDoc);

    if (a)
    {
      StringCchPrintf(msg, 512, _T("LocalSync_FileUpload done : local-file=%s, fileOID=%s, storageOID=%s"),
        localFileName, serverFileOID, serverStorageOID);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      r = R_SUCCESS;
    }
    else
    {
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("Failure on LocalSync_CreateDocument : %s : error=%s, fault_code=0x%x\n"),
        localFileName, err.fault_string, err.fault_code);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      r = FILE_SYNC_UPLOAD_FAIL;
      //r = new_document ? FILE_SYNC_CREATE_DOCUMENT_FAIL : FILE_SYNC_UPDATE_DOCUMENT_FAIL;
    }
  }

  LocalSync_CleanEnv(&err);
  return r;
}

int KSyncCoreEngine::SyncServerDownloadFile(LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR localFileName, LPTSTR* pErrorMessage)
{
  int r = R_SUCCESS;
  ERROR_ST err;
  LocalSync_InitEnv(&err);

#ifdef _DEBUG
  TCHAR msg[512];
  StringCchPrintf(msg, 512, _T("LocalSync_FileDownload : file=%s, documentOID=%s, storageOID=%s\n"),
    localFileName, fileOID, storageOID);
  OutputDebugString(msg);
#endif

  if (!LocalSync_FileDownload(&err, fileOID, storageOID, localFileName))
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_FileDownload : file=%s, documentOID=%s, storageOID=%s : error=%s\n"),
      localFileName, fileOID, storageOID, err.fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    if ((pErrorMessage != NULL) && (err.fault_string != NULL) && (lstrlen(err.fault_string) > 0))
    {
      AllocCopyStringTrim(pErrorMessage, err.fault_string);
    }
    r = FILE_SYNC_DOWNLOAD_FAIL;
  }
  LocalSync_CleanEnv(&err);
  return r;
}

BOOL KSyncCoreEngine::SyncServerGetDocTypeInfo(LPCTSTR docTypeOid, KEcmDocTypeInfo* pInfo)
{
  BOOL r = false;
  XDOCTYPE_ST* xdocType = NULL;

  if (SyncServerGetDocTypeData(docTypeOid, &xdocType) == R_SUCCESS)
  {
    if ((xdocType != NULL) && (xdocType->extAttribute != NULL))
    {
      pInfo->SetDocTypeInfo(NULL, docTypeOid, xdocType->name);

      int colCount = xdocType->extAttribute->extColumnsCNT;
      DestinyMsgpack_LSIF::XEXTCOLUMN_** extColumns = xdocType->extAttribute->extColumns;
      for (int i = 0; i < colCount; i++)
      {
        if (!extColumns[i]->flagVisible)
          continue;

        UINT maxLength = 0;

        if ((extColumns[i]->maxInputValue != NULL) && (lstrlen(extColumns[i]->maxInputValue) > 0))
        {
          swscanf_s(extColumns[i]->maxInputValue, _T("%d"), &maxLength);
        }

        EcmExtDocTypeColumnType type = KEcmDocTypeInfo::TypeFromString((LPCTSTR)extColumns[i]->colType);
        pInfo->Add(type, (LPCTSTR)extColumns[i]->name, (LPCTSTR)extColumns[i]->dbColName, maxLength);
      }
      r = TRUE;
    }
    if (xdocType != NULL)
      SyncServerFreeDocTypeData(xdocType);
  }
  return r;
}

int KSyncCoreEngine::SyncServerGetDocumentExt(LPCTSTR IN document_oid, KMetadataInfo* pInfo, LPTSTR* pErrorMessage)
{
  int r = R_SUCCESS;
  ERROR_ST err;
  LocalSync_InitEnv(&err);

  XSYNCDOCUMENTWITHEXTATTR_ST* pXDoc = NULL;
  if (!LocalSync_GetDocumentWithExtAttr(&err, document_oid, &pXDoc))
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_GetDocumentWithExtAttr : documentOID=%s : error=%s\n"),
      document_oid, err.fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }
  else
  {
    StringCchCopy(pInfo->mDocTypeOid, MAX_OID, pXDoc->docTypeOID);
    for (int i = 0; i < pXDoc->extColumnValuesCNT; i++)
    {
      EcmExtDocTypeColumnType type = KEcmDocTypeInfo::TypeFromString(pXDoc->extColumnValues[i]->valuetype);
      pInfo->Set(pXDoc->extColumnValues[i]->key, pXDoc->extColumnValues[i]->value, NULL, type, MetaDataCategory::ExtDocType, TRUE);
    }

    LocalSync_DestroyXSYNCDOCUMENTWITHEXTATTR_ST(pXDoc, false);
  }

  LocalSync_CleanEnv(&err);
  return r;
}

LRESULT KSyncCoreEngine::OnFileSyncProgress(LPCTSTR name, int progress)
{
  return 1;
}

void KSyncCoreEngine::SetLoginUserInfo(struct SessionValue *pSession)
{
  if (pSession != NULL)
    StringCchCopy(m_SessionKey, 60, pSession->sessionKey);
  else
    m_SessionKey[0] = '\0';
}

// update User Policy Info(language, system-exclude-mask, drive-permission, drive-letters)
BOOL KSyncCoreEngine::RefreshSyncPolicyInfo(int flag) //BOOL forceChanged)
{
  BOOL changed = !!(flag & REFRESH_FORCE);
  
#ifdef _USE_AGENT_LANGUATE
  if ((flag & REFRESH_LOCALE) && CheckLanguage())
  {
    changed = TRUE;
    gpCoreEngine->PostMessageToMain(WM_SYNC_COMMAND_MESSAGE, SYNC_CMD_LOCALE_CHANGED, 0);
  }
#endif

#ifdef _USE_AGENT_TIMEOUT_LIMIT
  RegGetDestinyAgentValues(mExplorerType, mAgentTimeoutLimit, mEcmSvr);

  mAgentTimeoutLimit = (int)(mAgentTimeoutLimit * 0.8);
  mAgentCheckedTime = GetTickCount();
#endif

  if (flag & REFRESH_EXCLUDE_MASK)
    changed = TRUE;

  BOOL updated = FALSE;

  if (flag & REFRESH_DRIVE_PERMISSION)
  {
    // update policy
    LSDD_POLICY_INFORMATIONEX DDPolicyInfo;
    ERROR_ST err;
    LocalSync_InitEnv(&err);

    memset(&DDPolicyInfo, 0, sizeof(DDPolicyInfo));
    if (mUseLocalSync &&
      LocalSync_GetDDPolicyInfo(&err, &DDPolicyInfo))
    {
      if (UpdateServerPolicyInfo(&DDPolicyInfo, FALSE))
      {
        changed = TRUE;
        updated = TRUE;
      }
    }
    else
    {
      if (flag & REFRESH_FORCE)
      {
        TCHAR msg[256] = _T("Not use sync or failure on LocalSync_GetDDPolicyInfo() : ");
        if (err.fault_string != NULL)
          CopyStringTrim(msg, 256, err.fault_string);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG|SYNC_MSG_HISTORY);
      }

      changed = (mUserFlag != 0);

      // clear all
      mUserFlag = 0;
      mSecurityDrive = ' ';
      mEcmDrive = ' ';
      mEcmVirtualDrive = ' ';
    }
    LocalSync_CleanEnv(&err);
  }

  if (changed && !updated)
    refreshLogUserInfoSetting();

  return changed;
}

BOOL KSyncCoreEngine::UpdateServerPolicyInfo(void *p, BOOL updateSetting)
{
  BOOL changed = FALSE;
  PLSDD_POLICY_INFORMATIONEX pPolicy = (PLSDD_POLICY_INFORMATIONEX)p;

  // R/W/D/N/X, X:보안DD의 파일은 일반DD로 저장 불가(2018.07)
  int userFlag = 0;
  if(pPolicy->PolicyInfo.cNormalDDExplorer == 'R')
    userFlag |= FLAG_READABLE_NORMAL_DISK;
  else if((pPolicy->PolicyInfo.cNormalDDExplorer == 'W') || (pPolicy->PolicyInfo.cNormalDDExplorer == 'X'))
    userFlag |= FLAG_READABLE_NORMAL_DISK|FLAG_WRITABLE_NORMAL_DISK;

  if(pPolicy->PolicyInfo.cSecurityDD == 'R')
    userFlag |= FLAG_READABLE_SECURITY_DISK;
  else if(pPolicy->PolicyInfo.cSecurityDD == 'W')
    userFlag |= FLAG_READABLE_SECURITY_DISK|FLAG_WRITABLE_SECURITY_DISK;

  if(pPolicy->PolicyInfo.cEcmDD == 'R')
    userFlag |= FLAG_READABLE_ECM_DISK;
  else if(pPolicy->PolicyInfo.cEcmDD == 'W')
    userFlag |= FLAG_READABLE_ECM_DISK|FLAG_WRITABLE_ECM_DISK;

  if (mUserFlag != userFlag)
  {
    mUserFlag = userFlag;
    changed = TRUE;
  }

  TCHAR msg[256];

  StringCchCopy(msg, 256, _T("Start for "));
  StringCchCat(msg, 256, mUserOid);

  StringCchCat(msg, 256, _T(", NormalDisk:"));
  if (mUserFlag & FLAG_RW_NORMAL_DISK)
  {
    if (mUserFlag & FLAG_WRITABLE_NORMAL_DISK)
      StringCchCat(msg, 256, _T("W"));
    else
      StringCchCat(msg, 256, _T("R"));
  }
  else
    StringCchCat(msg, 256, _T("X"));

  StringCchCat(msg, 256, _T(", SecurityDisk:"));
  if (mUserFlag & FLAG_RW_SECURITY_DISK)
  {
    if (mUserFlag & FLAG_WRITABLE_SECURITY_DISK)
      StringCchCat(msg, 256, _T("W"));
    else
      StringCchCat(msg, 256, _T("R"));
  }
  else
    StringCchCat(msg, 256, _T("X"));

  StringCchCat(msg, 256, _T(", ECMDisk:"));
  if (mUserFlag & FLAG_RW_ECM_DISK)
  {
    if (mUserFlag & FLAG_WRITABLE_ECM_DISK)
      StringCchCat(msg, 256, _T("W"));
    else
      StringCchCat(msg, 256, _T("R"));
  }
  else
    StringCchCat(msg, 256, _T("X"));

  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG| SYNC_MSG_DEBUG);

  if (mSecurityDrive != toupper(pPolicy->PolicyInfo.cSecurityDDVolume))
  {
    mSecurityDrive = toupper(pPolicy->PolicyInfo.cSecurityDDVolume);
    if (mSecurityDrive < ' ')
      mSecurityDrive = ' ';
    changed = TRUE;
  }

  if (mEcmDrive != toupper(pPolicy->PolicyInfo.cEcmDDVolume))
  {
    mEcmDrive = toupper(pPolicy->PolicyInfo.cEcmDDVolume);
    if (mEcmDrive < ' ')
      mEcmDrive = ' ';
    changed = TRUE;
  }

  if (mEcmVirtualDrive != toupper(pPolicy->PolicyInfo.cVirtualDDVolume))
  {
    mEcmVirtualDrive = toupper(pPolicy->PolicyInfo.cVirtualDDVolume);
    if (mEcmVirtualDrive < ' ')
      mEcmVirtualDrive = ' ';
    changed = TRUE;
  }

  StringCchCopy(gECMDriveName, 32, pPolicy->PolicyInfo.wszEcmDDVolumeName);
  StringCchCopy(gSecurityDriveName, 32, pPolicy->PolicyInfo.wszSecurityDDVolumeName);

  if (changed || updateSetting)
    refreshLogUserInfoSetting();
  return changed;
}

void KSyncCoreEngine::refreshLogUserInfoSetting()
{
#ifdef USE_BACKUP // 백업에서 보안 드라이브는 사용하지 않는다.
  gHiddenDrives[0] = mSecurityDrive;
  int len = 1;
#else
  int len = 0;
#endif

  if (mEcmDrive > ' ')
    gHiddenDrives[len++] = mEcmDrive;
  if (mEcmVirtualDrive > ' ')
    gHiddenDrives[len++] = mEcmVirtualDrive;
  gHiddenDrives[len] = '\0';
}


int KSyncCoreEngine::IsSyncRootFolder(LPCTSTR rootFolder)
{
  int r = 0;

  return r;
}

BOOL KSyncCoreEngine::GetFolderInfo(LPCTSTR IN folderOID, LPTSTR OUT parentOID, LPTSTR OUT folderName)
{
  BOOL rtn = FALSE;
  ERROR_ST err;
  XSYNCFOLDER_ST *syncFolder = NULL;

  LocalSync_InitEnv(&err);
  if (LocalSync_GetFolder(&err, folderOID, &syncFolder))
  {
    if (parentOID != NULL)
      StringCchCopy(parentOID, KMAX_PATH, syncFolder->parentOID);
    if (folderName != NULL)
      StringCchCopy(folderName, KMAX_PATH, syncFolder->name);

    LocalSync_DestroyXSYNCFOLDER_ST(syncFolder);
    rtn = TRUE;
  }
  LocalSync_CleanEnv(&err);
  return rtn;
}


BOOL KSyncCoreEngine::SyncServerDeleteFolder(LPCTSTR folder_OID, LPCTSTR folder_index, LPTSTR* pErrorMsg)
{
  ERROR_ST err;
  XSYNCFOLDER_ST *syncFolder = NULL;

  LocalSync_InitEnv(&err);
  BOOL r = LocalSync_DeleteFolder(&err, folder_OID, folder_index);
  if (!r)
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_DeleteFolder : %s : error=%s"), folder_OID, err.fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    if (pErrorMsg != NULL)
      AllocCopyStringTrim(pErrorMsg, err.fault_string);
  }
  LocalSync_CleanEnv(&err);
  return r;
}

int KSyncCoreEngine::SyncServerCheckFolderExist(LPCTSTR folder_OID, LPCTSTR folderFullPathIndex, LPTSTR pErrorMsg)
{
  int rtn = R_SUCCESS;
  ERROR_ST err;
  bool r = false;
  LocalSync_InitEnv(&err);
  if (!LocalSync_IsExistedFolder(&err, folder_OID, folderFullPathIndex, &r))
  {
    TCHAR msg[512];
    StringCchPrintf(msg, 512, _T("Failure on LocalSync_IsExistedFolder : %s : error=%s"), folder_OID, err.fault_string);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    if (pErrorMsg != NULL)
      CopyStringTrim(pErrorMsg, MAX_ERROR_LEN, err.fault_string);

    if (err.fault_code == ECM_AGENT_NOT_LOGIN) // RFC_NOT_LOGIN
      rtn = R_FAIL_NO_NETWORK;
    else
      rtn = R_FAIL_SERVER_FOLDER_NOT_EXIST;
  }
  else
  {
    rtn = r ? R_SUCCESS : R_FAIL_SERVER_FOLDER_NOT_EXIST;
  }
  LocalSync_CleanEnv(&err);
  return rtn;
}

void KSyncCoreEngine::PostMessageToMain(int msg, WPARAM wParam, LPARAM lParam)
{
  CWnd *wnd = AfxGetApp()->GetMainWnd();
  if (wnd != NULL)
    wnd->PostMessage(msg, wParam, lParam);
}

BOOL KSyncCoreEngine::SaveUpdateExtAttribute(LPCWSTR pathName, KEcmDocTypeInfo& docTypeInfo, KMetadataInfo* p)
{
  if ((pathName[1] == ':') && (pathName[0] == mEcmDrive))
  {
    ERROR_ST err;
    WCHAR oid[MAX_STGOID];
    LocalSync_InitEnv(&err);

    if (LocalSync_GetObjectOIDByLocalFullPathName(&err, pathName, oid))
    {
      LocalSync_CleanEnv(&err);

      XSYNCDOCUMENT_ST* ppXDoc = NULL;
      if (LocalSync_GetDocument(&err, oid, &ppXDoc))
      {
        LONGLONG fileSize = 0;//ppXDoc->fileSize

        BOOL r = CreateDocumentWithExtAttr(FALSE, &err, ppXDoc->folderOID, ppXDoc->name, fileSize, ppXDoc->fileOID, ppXDoc->storageFileID,
            LONGLONG localLastModifiedAt, LONGLONG localCreatedAt, ppXDoc->folderFullPath, KEcmDocTypeInfo * pInfo, KMetadataInfo * pMetadata, OUT XSYNCDOCUMENT_ST * *ppXDoc);

      }
    }
    else
    {
      TCHAR msg[512];
      StringCchPrintf(msg, 512, _T("Failure on LocalSync_GetObjectOIDByLocalFullPathName code=%d, err=%s, path=%s"), err.fault_code, err.fault_string, pathName);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_EVENT_ERROR);
    }
    LocalSync_CleanEnv(&err);
  }
  return FALSE;
}

