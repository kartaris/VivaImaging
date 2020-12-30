#include "StdAfx.h"
#include "KConstant.h"
#include <tchar.h>

const char KEY_EXE_MODULE[] = "exec_module";
const char KEY_WINDOW_POSITION[] = "window_position";
const char KEY_LAST_SYNC[] = "last_sync";
const char KEY_LAST_AUTOSYNC[] = "last_autosync";
const char KEY_LAST_SETTING[] = "last_setting";
const char KEY_DEFAULT_SYSTEM_EXCLUDE_MASKS[] = "def_ex_mask";

const TCHAR DESTINY_ROOT_FOLDER[] = _T("c:\\Cyberdigm");
const TCHAR DESTINY_TEMP_PREFIX[] = _T("DTF");
const TCHAR LOG_FOLDER_NAME[] = _T("Log");

const TCHAR APP_SETTING_FILENAME[] = _T("app.txt");

const TCHAR HISTORY_FOLDERNAME[] = _T("history");
const TCHAR HISTORY_DB_FILENAME[] = _T("history.ddb");

TCHAR gHiddenDrives[6] = _T("");


const TCHAR SERVER_KEY_USE_LOCAL_SYNC[] = _T("setup.UseLocalSync");
const TCHAR SERVER_OKEY_USE_LOCAL_SYNC[] = _T("localsync.UseLocalSync");

const TCHAR SERVER_KEY_RECENT_SYNC_COUNT[] = _T("localsync.RecentSyncFileCount");
const TCHAR SERVER_KEY_EXCLUDE_MASKS[] = _T("file.UnableFileExtensions");
const TCHAR SERVER_KEY_USE_EXCLUDE_MASKS[] = _T("file.UseFileExtensionCheck");
const TCHAR SERVER_KEY_USE_FILE_SIZE_CHECK[] = _T("file.UseFileSizeCheck");
const TCHAR SERVER_KEY_MAX_FILE_SIZE[] = _T("file.FileMaxSize");

#if defined(USE_BACKUP) || defined(USE_LOCALSYNC)
const TCHAR DESTINY_HOME_FOLDER[] = _T("c:\\Cyberdigm\\DestinyLocalSync");
const TCHAR DESTINY_TEMP_FOLDER[] = _T("c:\\Cyberdigm\\DestinyLocalSync\\Temp");
const TCHAR LOG_SUBFOLDER_NAME[] = _T("FileBackup");
const TCHAR LOG_EX_SUBFOLDER_NAME[] = _T("Service");
const TCHAR MainDialogClassName[] = _T("DestinyLocalSyncExecDlg");

const TCHAR LOCALSYNC_PROCESS_MUTEX[] = _T("Global\\Mutex_X_DestinyLocalSyncProcessAliveFlag");
const TCHAR LOCALSYNC_LOGIN_MUTEX[] = _T("Global\\Mutex_X_DestinyLocalSyncLoginFlag");
const TCHAR LOCALSYNC_MODAL_MUTEX[] = _T("Global\\Mutex_X_DestinyLocalSyncModalFlag");
const TCHAR LOCALSYNC_INSTANCE_MUTEX[] = _T("Global\\Mutex_X_DestinyLocalSyncInstanceFlag");

const char META_RELATIVE_FILENAME[] = "relative_filename";
const char META_SYNC_RESULT[] = "sync_result";
const char META_CHILD_RESULT[] = "child_result";
const char META_CONFLICT_RESULT[] = "conflict_result";
const char META_SERVER_MESSAGE[] = "server_message";
const char META_ITEM_TYPE[] = "item_type";
const char META_ITEM_FLAG[] = "item_flag";

const char META_LOCAL_UPDATE_TIME[] = "local_update_time";
const char META_LOCAL_WRITER[] = "local_writer";
const char META_LOCAL_SIZE[] = "local_size";
const char META_LOCAL_STATE[] = "local_state";

const char META_SERVER_UPDATE_TIME[] = "server_update_time";
const char META_SERVER_OID[] = "server_oid";
const char META_SERVER_FILE_OID[] = "server_file_oid";
const char META_SERVER_STORAGE_OID[] = "server_storage_oid";
const char META_SERVER_WRITER[] = "server_writer";
const char META_SERVER_SIZE[] = "server_size";
const char META_SERVER_STATE[] = "server_state";
const char META_SERVER_FULLPATH[] = "server_fullpath";
const char META_SERVER_FULLPATHINDEX[] = "server_fullpathindex";
// const char META_SERVER_PERMISSIONS[] = "server_permissions";

const char META_LOCAL_RENAMED[] = "local_renamed";
const char META_SERVER_RENAMED[] = "server_renamed";

const char META_SERVER_NEW_UPDATE_TIME[] = "snew_update_time";
const char META_SERVER_NEW_WRITER[] = "snew_writer";
const char META_SERVER_NEW_FULLPATH[] = "snew_fullpath";
const char META_SERVER_NEW_FULLPATHINDEX[] = "snew_fullpathindex";
const char META_SERVER_NEW_FILE_OID[] = "snew_file_oid";
const char META_SERVER_NEW_STORAGE_OID[] = "snew_storage_oid";
const char META_SERVER_NEW_SIZE[] = "snew_size";

const char META_LOCAL_NEW_UPDATE_TIME[] = "lonew_update_time";
const char META_LOCAL_NEW_SIZE[] = "lonew_size";

const char SYNC_FOLDER_OWNER_OID[] = "sync_folder_owner";

const char LOGIN_USER_INFO[] = "logininfo";

#ifdef USE_BACKUP
const TCHAR META_DB_FILENAME[] = _T("filebackup.ddb");
#else
const TCHAR META_DB_FILENAME[] = _T(".localsync.ddb");
#endif

const TCHAR FOLDERMAP_LIST_FILENAME[] = _T("fmaplist.txt");

const TCHAR meta_folder_name[] = _T(".filesync");
const TCHAR object_folder_name[] = _T(".objects");
const TCHAR ROOT_METAFILENAME[] = _T(".root.ifo");
const TCHAR SYNC_RENAMED_MANAGE_FILENAME[] = _T("renamed.txt");

const TCHAR SYNC_FOLDER_LIST_FILENAME[] = _T("fs_folders.txt");
const TCHAR SYNC_ROOT_FOLDER_SETTING_FILENAME[] = _T("setting.txt");
const TCHAR SUBTEMP_FOLDER_NAME[] = _T("tmp");
const TCHAR DOWNTEMP_FOLDER_NAME[] = _T(".synctmp");

const TCHAR LOG_FILENAME[] = _T("sync_log.txt");

const TCHAR INFO_EXTENSION[] = _T(".ifo");

#ifdef USE_BACKUP
const char DA_NAME_DOCUMENT_OID[] = "documentoid";
const char DA_NAME_FOLDER_OID[] = "folderoid";
#endif

const LPCTSTR SHELL_CMD_EXPLORER = _T("explore");
const LPCTSTR SHELL_CMD_OPEN = _T("open");


const TCHAR *SYNC_COMPARE_RESULT_STR[] = {
  _T("Not changed"),
  _T("Need download"),
  _T("Need upload"),
  _T("Need delete on local"),
  _T("Need delete on server"),
  _T("Retired"),
  _T("Need move/rename on local"),
  _T("Need move/rename on server"),
  _T("Need move/rename as same"),
  _T("Previous SyncRootFolder is moved as subfolder into this Sync folder"),
  _T("Conflict:local modified, server modified"),
  _T("Conflict:local modified, server deleted"),
  _T("Conflict:local deleted, server modified"),
  _T("Conflict:local added, server added"),
  _T("Conflict:sync error")
};

#ifdef USE_BACKUP
const char KEYS_FULL_BACKUP[] = "FullBackup";
const char KEYS_INSTANT_BACKUP[] = "InstantBackup";
const char KEYS_RESTORE[] = "Restore";

const TCHAR RESTORE_STATUS_S[] = _T("S");
const TCHAR RESTORE_STATUS_C[] = _T("C");
#endif


const char SYNC_POLICY_METHOD[] = "policy_method";
const char SYNC_POLICY_REPEAT_TYPE[] = "policy_r_type";
const char SYNC_POLICY_DATES_IN_WEEK[] = "policy_dinweek";
const char SYNC_POLICY_EVERY_N_DAYS[] = "policy_endays";
const char SYNC_POLICY_EVERY_N_HOURS[] = "policy_enhours";
const char SYNC_POLICY_EVERY_N_MINUTES[] = "policy_enminutes";
const char SYNC_POLICY_FLAG[] = "policy_flag";
const char SYNC_POLICY_EXCLUDES[] = "policy_exs";
const char SYNC_POLICY_TIMEAT[] = "policy_timeat";
const char SYNC_POLICY_PAUSED[] = "policy_paused";

const char KEYS_LAST_SCAN_TIME[] = "last_scan";


const TCHAR *GetSyncCompareResultString(int result)
{
  int index = 0;
  if (result & FILE_CONFLICT)
  {
    if (result & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)
      index = 10;
    else if (result & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)
      index = 11;
    else if (result & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)
      index = 12;
    else if (result & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED)
      index = 13;
  }
  else if (result & FILE_SYNC_ERR)
  {
    index = 14;
  }
  else if (result & FILE_ACTION_MASK)
  {
    result = (result & FILE_ACTION_MASK);
    if (result & FILE_NEED_DOWNLOAD)
      index = 1;
    else if (result & FILE_NEED_UPLOAD)
      index = 2;
    else if (result & FILE_NEED_LOCAL_DELETE)
      index = 3;
    else if (result & FILE_NEED_SERVER_DELETE)
      index = 4;
    else if (result & FILE_NEED_RETIRED)
      index = 5;
    else if (result & FILE_NEED_LOCAL_MOVE)
      index = 6;
    else if (result & FILE_NEED_SERVER_MOVE)
      index = 7;
    else if (result & FILE_SAME_MOVED)
      index = 8;
    else if (result & FILE_RELEASE_PREV_SYNC_ROOT)
      index = 9;
  }

  return SYNC_COMPARE_RESULT_STR[index];
}

#elif defined(USE_IMAGING)
const TCHAR DESTINY_HOME_FOLDER[] = _T("C:\\Cyberdigm\\DestinyImaging");
const TCHAR DESTINY_TEMP_FOLDER[] = _T("C:\\Cyberdigm\\DestinyImaging\\Temp");
const TCHAR LOG_SUBFOLDER_NAME[] = _T("DestinyImaging");
const TCHAR LOG_EX_SUBFOLDER_NAME[] = _T("Service");
const LPCTSTR strLastUnfinishedDocument = _T("LastUnfinishedDocument");

#endif

const TCHAR ROOT_ITEM_KEY_STR[] = _T("\\"); // root item relative-path-name(DB item key)

const char LF_STR[] = "\n";
const TCHAR L_LF_STR[] = _T("\n");
const TCHAR L_CRLF_STR[] = _T("\r\n");
const TCHAR L_ASTERISK_STR[] = _T("*");
const TCHAR PARTIAL_SYNC_FOLDER_SEPARATOR_STR[] = _T(";");

const TCHAR SERVER_PRIVATE_ROOT_OID[] = _T("P_ROOT");

TCHAR mUserOid[MAX_OID] = _T("1234567890a");
TCHAR mUserName[MAX_USERNAME] = _T("LocalUser");
TCHAR mHomeFolder[KMAX_PATH] = { 0 };
WORD mResourceLanguage = 0;
WORD mExplorerType = 1; // 1 : PANTA mode, 2 : Window Explorer mode
WORD mUseAutoSync = 1;
WORD mEcmSvr = 0; // 1 : ECM, 0 : Claudium
DWORD mSessionIdx = INVALID_SESSION_ID;

TCHAR gECMDriveName[32] = { 0 };
TCHAR gSecurityDriveName[32] = { 0 };
TCHAR gModulePathName[KMAX_PATH];

TCHAR mUploadServerFolderOID[MAX_OID] = _T("");

// global variables
//TCHAR gDefaultImageFileExt[7] = _T("PNG\0\0\0");

ImagingSaveFormat  gSaveFormat = TIF_MULTI_PAGE; // XML_WITH_JPEG;
LPTSTR gImageFilenameFormat = NULL;

TCHAR mSecureWorkingBaseFolder[MAX_PATH] = _T("");

TCHAR mDefaultWorkingBaseFolder[MAX_PATH] = _T("");
TCHAR mDefaultUploadFolderPath[MAX_PATH] = _T("");
TCHAR mDefaultUploadFolderOID[16] = _T("");

DWORD mGlobalOption = 0;

const TCHAR mutexNameECMAgent[] = _T("Global\\Mutex_X_DestinyECMAGENT");

