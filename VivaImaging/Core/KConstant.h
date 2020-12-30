#pragma once

#ifndef _DESTINY_CONSTANT_VARIABLE_MODULE_H_
#define _DESTINY_CONSTANT_VARIABLE_MODULE_H_

#define LF_CHAR '\n'
#define PARTIAL_SYNC_FOLDER_SEPARATOR ';'

#define DA_PARAMETER_SEPARATOR '\b'
#define DA_NAME_SEPARATOR '='
#define DA_VALUE_SEPARATOR '|'
#define DA_SLASH_SEPARATOR '/'
#define DA_RESULT_SEPARATOR ':'
#define DA_COMMA_SEPARATOR ','

extern const char DA_NAME_DOCUMENT_OID[];
extern const char DA_NAME_FOLDER_OID[];

#define USE_SCAN_BY_HTTP_REQUEST /** getModifiedFolderNDocument 메소드로 서버측 조회 **/
#define SCAN_FOLDER_FILE_AT_ONCE /** 로컬 파일 스캔시 폴더와 파일을 한 번에 같이 스캔 **/

#ifdef _DEBUG
// #define USE_SECURE_DISK_BACKUP /** 보안 디스크는 백업 대상 아님 **/
// #define _DEBUG_SYNC_TIME
#endif

#ifdef USE_BACKUP
#define SYNC_ALL_DRIVE
#define NO_USE_OVERLAY_ICON
#define USE_INSTANT_SYNC
//#define USE_LAZY_UPLOAD (일부 시스템에서 PdhCollectQueryData failed with status 0x800007d5 발생)
#define USE_SYNC_DURATION
//#define USE_BACKUP_EXCLUDE_RENAME

#define PURE_INSTANT_BACKUP /** 실시간 백업에서 스캔하지 않고 대상 항목만 바로 처리한다. 전체 백업이 진행중이면 멈추고 이를 먼저 처리한다 **/

#define USING_STORAGE_TRANSACTION

#define USE_BACKUP_JOB_LIST /** 작업 내용을 storage에 저장하여, 다시 시작되었을때 시작되도록 한다. */

#define STEP_BY_STEP_FULLBACKUP /** 전체백업을 다시 시작할때, 1,2,3 단계중 완료된 단계는 생략한다. */

// #define SYNCTIME_BASE_LAST_START /** 백업 시작일 기준으로 다음 백업일 계산 */


// #define _DEBUG_DUPLICATE_FOLDER_TEST // 중복 폴더 생성 테스트.

#endif

#ifdef _DEBUG
#define NOF_ITEMS_PER_META 100 /** 한 주기당 메타 데이터 로드 갯수 **/
#else
#define NOF_ITEMS_PER_META 500 /** 한 주기당 메타 데이터 로드 갯수 **/
#endif

#ifdef USE_BACKUP
#define NOF_DOCUMENT_PER_PAGE 500 /** 한 주기당 서버 문서 목록 조회 갯수 **/
#define NOF_LOCAL_FILES_PER_PAGE 50 /** 한 주기당 로컬 파일 FindFile 갯수 **/
#else
#define NOF_DOCUMENT_PER_PAGE 200 /** 한 주기당 서버 문서 목록 조회 갯수 **/
#define NOF_LOCAL_FILES_PER_PAGE 20 /** 한 주기당 로컬 파일 FindFile 갯수 **/
#endif
#define NUMBER_OF_ITEMS_COMPARE 20 /** 한 주기당 서버-로컬 비교 갯수 **/
#define NUMBER_OF_ITEMS_SCAN_PARSE 20 /** 한 주기당 서버 HTTP SCAN parse 갯수 **/
#define NUMBER_OF_ITEMS_META_STORE 30 // 한 주기당 메타 데이터 저장 갯수(Immigration process)

#ifdef _DEBUG
#define TRAFFIC_CHECK_SEC (60 * 1)  /** 전체 동기화 시작시 서버 트래픽 체크 주기 **/
#define TRAFFIC_UPLOAD_CHECK_SEC 5 /** 파일 업로드시 서버 Busy retry 주기 **/
#else
#define TRAFFIC_CHECK_SEC (60 * 15) /** 전체 동기화 시작시 서버 트래픽 체크 주기 **/
#define TRAFFIC_UPLOAD_CHECK_SEC 30 /** 파일 업로드시 서버 Busy retry 주기 **/
#endif

// #define USE_LONG_PATH_LENGTH // some bug found

// 충돌 처리 트리의 order를 Tree depth로 표시, or Dummy-폴더 항목을 인접하여 표시.
// #define CONFLICT_TREE_ORDER_BY_DEPTH

#define KLONG_PATH 500

#ifdef USE_LONG_PATH_LENGTH
#define KMAX_PATH 600
#define MAX_SYNCROOT_PATH_LENGTH (KMAX_PATH - 40)
#define MAX_FOLDER_PATH_LENGTH (KMAX_PATH - 20)
#define MAX_FILE_PATH_LENGTH (KMAX_PATH - 6)
#else
#define KMAX_PATH MAX_PATH
#define MAX_SYNCROOT_PATH_LENGTH 200
#define MAX_FOLDER_PATH_LENGTH 240
#define MAX_FILE_PATH_LENGTH 256
#define MAX_FILENAME_LENGTH 256

#define KMAX_LONGPATH 600
#endif

#define KMAX_PATH_A (KMAX_PATH * 4)

#define KMAX_METAPATH (KMAX_PATH + 30)
#define KMAX_EXTPATH (KMAX_METAPATH + 10)

#define KMAX_PATHINDEX 500 // server FullPathIndex max-length
#define MAX_USERNAME 64
#define MAX_OID 12
#define MAX_DATETIME 16
#define MAX_STGOID 20
#define MAX_ERROR_LEN 200
#define DESTINY_OID_LENGTH 11
#define DESTINY_FILE_OID_LENGTH 11

#define KMAX_FILENAME 256
#define KMAX_FULLPATH 32768 // long path name

#define ROOT_ITEM_KEY_CHAR '\\'

// key names for setting
extern const char KEY_EXE_MODULE[];
extern const char KEY_WINDOW_POSITION[];
extern const char KEY_LAST_SYNC[];
extern const char KEY_LAST_AUTOSYNC[];
extern const char KEY_LAST_SETTING[];
extern const char KEY_DEFAULT_SYSTEM_EXCLUDE_MASKS[];

extern const TCHAR DESTINY_ROOT_FOLDER[];
extern const TCHAR DESTINY_HOME_FOLDER[];
extern const TCHAR DESTINY_TEMP_FOLDER[];
extern const TCHAR DESTINY_TEMP_PREFIX[];

extern const TCHAR LOG_FOLDER_NAME[];
extern const TCHAR LOG_SUBFOLDER_NAME[];
extern const TCHAR LOG_EX_SUBFOLDER_NAME[];

extern const TCHAR APP_SETTING_FILENAME[];

extern const TCHAR HISTORY_FOLDERNAME[];
extern const TCHAR HISTORY_DB_FILENAME[];


extern const TCHAR SERVER_KEY_USE_LOCAL_SYNC[];
extern const TCHAR SERVER_OKEY_USE_LOCAL_SYNC[];
extern const TCHAR SERVER_KEY_RECENT_SYNC_COUNT[];
extern const TCHAR SERVER_KEY_EXCLUDE_MASKS[];
extern const TCHAR SERVER_KEY_USE_EXCLUDE_MASKS[];
extern const TCHAR SERVER_KEY_USE_FILE_SIZE_CHECK[];
extern const TCHAR SERVER_KEY_MAX_FILE_SIZE[];

extern TCHAR gHiddenDrives[6];


#if defined(USE_BACKUP) || defined(USE_LOCALSYNC)

extern const TCHAR MainDialogClassName[];

extern const TCHAR LOCALSYNC_LOGIN_MUTEX[];
extern const TCHAR LOCALSYNC_MODAL_MUTEX[];
extern const TCHAR LOCALSYNC_INSTANCE_MUTEX[];

extern const char META_RELATIVE_FILENAME[];
extern const char META_SYNC_RESULT[];
extern const char META_CHILD_RESULT[];
extern const char META_CONFLICT_RESULT[];
extern const char META_SERVER_MESSAGE[];

extern const char META_ITEM_TYPE[];
extern const char META_ITEM_FLAG[];

extern const char META_LOCAL_UPDATE_TIME[];
extern const char META_LOCAL_WRITER[];
extern const char META_LOCAL_SIZE[];
extern const char META_LOCAL_STATE[];

extern const char META_SERVER_UPDATE_TIME[];
extern const char META_SERVER_OID[];
extern const char META_SERVER_FILE_OID[];
extern const char META_SERVER_STORAGE_OID[];
extern const char META_SERVER_WRITER[];
extern const char META_SERVER_SIZE[];
extern const char META_SERVER_STATE[];
extern const char META_SERVER_FULLPATH[];
extern const char META_SERVER_FULLPATHINDEX[];
// extern const char META_SERVER_PERMISSIONS[];

extern const char META_LOCAL_RENAMED[];
extern const char META_SERVER_RENAMED[];

extern const char META_SERVER_NEW_UPDATE_TIME[];
extern const char META_SERVER_NEW_WRITER[];
extern const char META_SERVER_NEW_FULLPATH[];
extern const char META_SERVER_NEW_FULLPATHINDEX[];
extern const char META_SERVER_NEW_FILE_OID[];
extern const char META_SERVER_NEW_STORAGE_OID[];
extern const char META_SERVER_NEW_SIZE[];
extern const char META_LOCAL_NEW_UPDATE_TIME[];
extern const char META_LOCAL_NEW_SIZE[];

extern const char SYNC_FOLDER_OWNER_OID[];
extern const char LOGIN_USER_INFO[];


extern const TCHAR META_DB_FILENAME[];

extern const TCHAR FOLDERMAP_LIST_FILENAME[];
extern const TCHAR meta_folder_name[];
extern const TCHAR object_folder_name[];
extern const TCHAR ROOT_METAFILENAME[];
extern const TCHAR SYNC_RENAMED_MANAGE_FILENAME[];

extern const TCHAR SYNC_FOLDER_LIST_FILENAME[];
extern const TCHAR SYNC_ROOT_FOLDER_SETTING_FILENAME[];
extern const TCHAR SUBTEMP_FOLDER_NAME[];
extern const TCHAR DOWNTEMP_FOLDER_NAME[];

extern const TCHAR LOG_FILENAME[];

extern const TCHAR INFO_EXTENSION[];

extern const LPCTSTR SHELL_CMD_EXPLORER;
extern const LPCTSTR SHELL_CMD_OPEN;

extern const TCHAR *SYNC_COMPARE_RESULT_STR[];

#ifdef USE_BACKUP
extern const char KEYS_FULL_BACKUP[];
extern const char KEYS_INSTANT_BACKUP[];
extern const char KEYS_RESTORE[];

extern const TCHAR RESTORE_STATUS_S[];
extern const TCHAR RESTORE_STATUS_C[];
#endif

extern const char SYNC_POLICY_METHOD[];
extern const char SYNC_POLICY_REPEAT_TYPE[];
extern const char SYNC_POLICY_DATES_IN_WEEK[];
extern const char SYNC_POLICY_EVERY_N_DAYS[];
extern const char SYNC_POLICY_EVERY_N_HOURS[];
extern const char SYNC_POLICY_EVERY_N_MINUTES[];
extern const char SYNC_POLICY_FLAG[];
extern const char SYNC_POLICY_EXCLUDES[];
extern const char SYNC_POLICY_TIMEAT[];
extern const char SYNC_POLICY_PAUSED[];

extern const char KEYS_LAST_SCAN_TIME[];

#endif

#define MAX_SESSION_ID 0x008
#define INVALID_SESSION_ID 0xFFFF

extern const TCHAR ROOT_ITEM_KEY_STR[]; // root item relative-path-name(DB item key)

extern const char LF_STR[];
extern const TCHAR L_LF_STR[];
extern const TCHAR L_CRLF_STR[];
extern const TCHAR L_ASTERISK_STR[];
extern const TCHAR PARTIAL_SYNC_FOLDER_SEPARATOR_STR[];

extern const TCHAR SERVER_PRIVATE_ROOT_OID[];

extern TCHAR mUserOid[MAX_OID];
extern TCHAR mUserName[MAX_USERNAME];
extern TCHAR mHomeFolder[KMAX_PATH];
extern WORD mResourceLanguage;
extern WORD mExplorerType;
extern WORD mUseAutoSync;
extern WORD mEcmSvr;
extern DWORD mSessionIdx;

extern TCHAR gECMDriveName[32];
extern TCHAR gSecurityDriveName[32];

extern TCHAR gModulePathName[KMAX_PATH];

extern TCHAR mUploadServerFolderOID[MAX_OID];
extern const LPCTSTR strLastUnfinishedDocument;

#define GO_READONLY 0x001
#define GO_EDITABLE 0x002
#define GO_STARTSCAN 0x004
#define GO_CHECKUPLOAD 0x008
#define GO_USECRYPTION 0x010
#define GO_ENABLELOCALSAVE 0x020
#define GO_TEMPFOLDERONSECUREDISK 0x040
#define GO_MERGEANNOTATE 0x080
#define GO_AUTOCHANGESELECT 0x0100

#define GO_UPLOADAS_NEWNAME 0x01000
#define GO_UPLOADAS_NEWVERSION 0x02000
#define GO_UPLOAD_ONLY_MODIFIED 0x04000
#define GO_CLEARFOLDER_BEFOREUPLOAD 0x08000

#define GO_UPLOAD_OPTIONS 0x0F000

extern DWORD mGlobalOption;

typedef enum tagImagingSaveFormat {
  FORMAT_UNKNOWN,
  FORMAT_ORIGIN,
  FORMAT_JPG,
  FORMAT_JPEG,
  FORMAT_PNG,
  FORMAT_TIF,
  FORMAT_TIFF,
  TIF_MULTI_PAGE,
  TIFF_MULTI_PAGE,
  MAX_SAVE_IMAGE_FORMAT
}ImagingSaveFormat;

extern ImagingSaveFormat  gSaveFormat;
extern LPTSTR  gImageFilenameFormat;
extern TCHAR mSecureWorkingBaseFolder[];
extern TCHAR mDefaultWorkingBaseFolder[];
extern TCHAR mDefaultUploadFolderPath[];
extern TCHAR mDefaultUploadFolderOID[];

extern const TCHAR mutexNameECMAgent[]; // "Global\\Mutex_X_DestinyECMAGENT"

// mExplorerType
#define EXPLORER_PANTA 1
#define EXPLORER_WINDOW 2

#define PHASE_STARTING 1
#define PHASE_SCAN_OBJECTS 3

#define PHASE_SCAN_FOLDERS_SERVER 5 // on server
#define PHASE_SCAN_SERVER_FILES 7
#define PHASE_UPDATE_SERVER_INFO 9

#define PHASE_SCAN_FOLDERS_LOCAL 11 // on local
#define PHASE_SCAN_LOCAL_FILES 13
#define PHASE_UPDATE_LOCAL_INFO 15

#define PHASE_CLEAN_UNCONFLICT 20
#define PHASE_COMPARE_FILES 22
#define PHASE_CLEAN_UNCHANGED 24

#define PHASE_LOAD_FILES 30 // move folders

#define PHASE_CREATE_FOLDERS 32 // create newly added folders
#define PHASE_MOVE_FOLDERS 34 // move folders
// #define PHASE_DELETE_FILES 23 // backup only, remove items on server
#define PHASE_COPY_FILES 36 // copy files

#define PHASE_CONFLICT_FILES 40
#define PHASE_RETIRE 42
#define PHASE_ENDING 44
#define PHASE_ENDING2 46
#define PHASE_END_OF_JOB 48

#define FILE_NOT_CHANGED 0x000
#define FILE_NEED_DOWNLOAD 0x001
#define FILE_NEED_UPLOAD 0x002
#define FILE_NEED_LOCAL_DELETE 0x004
#define FILE_NEED_SERVER_DELETE 0x008
#define FILE_NEED_RETIRED 0x0010
#define FILE_NEED_LOCAL_MOVE 0x020
#define FILE_NEED_SERVER_MOVE 0x040

#define FILE_NEED_BOTH_MOVE (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE)
#define FILE_RELEASE_PREV_SYNC_ROOT 0x080 // previously SyncRootFolder is moved as subfolder into this Sync folder.

#define FILE_USER_ACTION_MASK 0x000F
#define FILE_CHANGE_MASK 0x00FF

#define FILE_SAME_MOVED 0x0100
#define FILE_SYNC_RENAMED 0x0200
#define FILE_NOW_SYNCHRONIZING 0x0400
#define FILE_NOW_CHANGED 0x0800
#define FILE_ACTION_MASK 0x0FFF

#define FILE_CONFLICT 0x0FF000
#define FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED 0x01000// local modified, server modified
#define FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED 0x02000 // local modified, server deleted
#define FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED 0x04000 // local deleted, server modified
#define FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED 0x08000 // local added, server added

#define FILE_SYNC_ERR 0x0FF00000
#define FILE_SYNC_UPLOAD_FAIL 0x0100000 // upload fail
#define FILE_SYNC_DOWNLOAD_FAIL 0x0200000 // download fail

#define FILE_SYNC_LOCAL_REMOVE_FAIL 0x0300000 // remove on local fail
#define FILE_SYNC_SERVER_REMOVE_FAIL 0x0400000 // remove on server fail
#define FILE_SYNC_NO_UPLOAD_PERMISSION 0x0500000
#define FILE_SYNC_NO_DOWNLOAD_PERMISSION 0x0600000
#define FILE_SYNC_ERROR_FAIL 0x0700000 // failure by server-call-error
#define FILE_SYNC_LOCAL_RENAME_FAIL 0x0800000 // failure when rename/move local file
#define FILE_SYNC_SERVER_RENAME_FAIL 0x0900000 // failure when rename/move server file
#define FILE_SYNC_CREATE_SERVER_FOLDER_FAIL 0x0A00000
#define FILE_SYNC_CREATE_LOCAL_FOLDER_FAIL 0x0B00000
#define FILE_SYNC_DELETE_LOCAL_FOLDER_FAIL 0x0C00000
#define FILE_SYNC_DELETE_SERVER_FOLDER_FAIL 0x0D00000

#define FILE_SYNC_ERROR_CHECKOUT 0x0E00000
#define FILE_SYNC_DOWNLOAD_WRITE_FAIL 0x0F00000 // fail on writing to file
#define FILE_SYNC_NO_CDISK_SPACE 0x01000000 // no dis space for c:
#define FILE_SYNC_NO_CREATE_FOLDER_PERMISSION 0x01100000
#define FILE_SYNC_NO_PERMISSION_ECM_DISK 0x01200000 // no permission to sync
#define FILE_SYNC_NO_PERMISSION_SECURITY_DISK 0x01300000
#define FILE_SYNC_NO_PERMISSION_LOCAL_DISK 0x01400000
#define FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER 0x01500000
#define FILE_SYNC_ROOTSERVER_FOLDER_NOT_EXIST 0x01600000

#define FILE_SYNC_NO_LOCAL_WRITE_PERMISSION 0x01700000
#define FILE_SYNC_OVER_PATH_LENGTH 0x01800000
#define FILE_SYNC_OVER_FILE_SIZE_LIMIT 0x01900000
#define FILE_SYNC_NO_FOLDER_PERMISSION 0x01A00000 // no folder access right on server
#define FILE_SYNC_PARENT_FOLDER_MOVED 0x01B00000 // parent folder fullPathIndex changed
#define FILE_SYNC_PARENT_FOLDER_FAIL 0x01C00000 // parent folder sync fail, so children give up sync
#define FILE_SYNC_NO_LOCAL_PERMISSION 0x1D00000 // no local-side permission
#define FILE_SYNC_NO_SERVER_PERMISSION 0x1E00000 // no server-side permission
#define FILE_SYNC_NO_MOVE_PERMISSION 0x1F00000 // no move permission on server-side
#define FILE_SYNC_NO_RENAME_PERMISSION 0x2000000 // no rename permission on server-side

#define FILE_SYNC_NO_DELETE_LOCAL_FOLDER_PERMISSION 0x02100000
#define FILE_SYNC_NO_DELETE_SERVER_FOLDER_PERMISSION 0x02200000
#define FILE_SYNC_NO_DELETE_LOCAL_FILE_PERMISSION 0x02300000
#define FILE_SYNC_NO_DELETE_SERVER_FILE_PERMISSION 0x02400000

#define FILE_SYNC_ALREADY_EXISTS_ON_LOCAL 0x02500000 // failure same file exist on local when download new file.
#define FILE_SYNC_ALREADY_EXISTS_ON_SERVER 0x02600000 // failure same file exist on server when upload new file.
#define FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS 0x02700000 // failure when rename/move local file
#define FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS 0x02800000 // failure when rename/move server file

#define FILE_SYNC_CHILD_DELETE_FAIL 0x02900000 // failure on delete child folder/files
#define FILE_SYNC_NO_LOCAL_FOLDER_FOUND 0x02A00000 // local folder not exist when create server folder for upload.
#define FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH 0x02B00000 // cannot download long-filename
#define FILE_SYNC_CANNOT_UPLOAD_LONGPATH 0x02C00000 // cannot upload long-filename
#define FILE_SYNC_DELETE_FOLDER_FAIL_CHILD_MODIFIED 0x02D00000 // cannot delete local folder which has modified or excluded items
#define FILE_SYNC_LOCAL_MODIFIED_AFTER_COMPARE 0x02E00000
#define FILE_SYNC_SERVER_MODIFIED_AFTER_COMPARE 0x02F00000

#define FILE_SYNC_NO_SERVER_FOLDER_FOUND 0x03000000 // 48, server folder not exist when create local folder for download.
#define FILE_SYNC_LOCAL_ACCESS_DENIED 0x03100000 // local folder move-rename access denied
#define FILE_SYNC_METAFILE_DISK_FULL 0x03200000 // metafile write fail by disk full
#define FILE_SYNC_DENY_DELETE_LOCAL_FILE 0x03300000 // delete local file access denied
#define FILE_SYNC_DENY_DELETE_LOCAL_FOLDER 0x03400000 // delete local folder access denied
#define FILE_SYNC_CANNOT_MAKE_METAFILE 0x03500000 // 53, failure create metafile

#define FILE_SYNC_EXCEPTION_ERROR 0x03600000
#define FILE_SYNC_MEMORY_ALLOC_FAIL 0x03700000

#define FILE_SYNC_SFS_ERROR 0x03800000 // failure on scan result

#define FILE_SYNC_EDITING_FILE_SKIP 0x03900000 // 편집중인 파일이라 복구 실패
#define FILE_SYNC_FAIL_REQUEST_DELETE 0x03A00000 // 백업 삭제처리 요청 실패
#define FILE_SYNC_NOT_EXIST_DRIVE 0x03B00000 // 존재하지 않는 드라이브

#define FILE_SYNC_UPLOAD_BUSY_REJECT 0x03C00000 // Server Traffic busy, download fail

#define MAX_SYNC_ERROR_INDEX 0x03D

#define SYNC_ERROR_INDEX(err)   ((err >> 20) & 0x03F)

// extra flag 0x0F000000
#define FILE_NOW_ORPHAN             0x010000000
#define FILE_SYNC_THREAD_WAIT   0x020000000 // only used on CoreEngine
#define FILE_ACTION_USER_SELECT 0x040000000

#define FILE_SYNC_BELOW_CONFLICT   0x020000000 // childs of conflict item

#define FILE_SYNC_RESOLVE_VISIBLE_FLAG (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED | FILE_SYNC_BELOW_CONFLICT)

/*
#define CONFLICT_UPLOAD 0
#define CONFLICT_DOWNLOAD 1
#define CONFLICT_DELETE 2
#define CONFLICT_MAX 2
*/

// return values. should matched with ksync_error_message[] on KSyncCoreEngine
#define R_SUCCESS 0	 // success
#define R_FAIL_PARENT_ALREADY_SYNCFOLDER		1 // parent folder already SYNC_FOLDER
#define R_FAIL_CHILD_ALREADY_SYNCFOLDER			2 // child folder already SYNC_FOLDER
#define R_FAIL_FOLDER_ALREADY_SYNCFOLDER		3 // folder is already SYNC_FOLDER
#define R_FAIL_NO_ACCESS_RIGHT							4 // no right for folder to write or create folder/files
#define R_FAIL_NO_NETWORK								5 // network error
#define R_FAIL_INTERNAL											6 // registry update error
#define R_FAIL_INVALID_LOCAL_PATH 7
#define R_FAIL_LOCAL_PATH_IS_SYSTEM_FOLDER 8
#define R_FAIL_NO_PERMISSION_TO_SECURITY_DISK 9
#define R_FAIL_NO_PERMISSION_TO_LOCAL_DISK 10
#define R_FAIL_INVALID_SERVER_PATH 11
#define R_FAIL_SERVER_FOLDER_ERROR 12 // Failure on receive server folder name
#define R_FAIL_NO_PERMISSION_ON_SERVER_FOLDER 13 // server_folder has no permission to create sub-folder
#define R_FAIL_SERVER_HAS_SAME_SUBFOLDER 14 // server_folder already has child folder which has same name
#define R_FAIL_SERVER_FOLDER_NOT_EXIST 15 // server_folder_oid is not exist
#define R_FAIL_UPLOAD_FAIL 16 // upload storage file failure
#define R_FAIL_DOWNLOAD_FAIL 17 // download storage file failure
#define R_FAIL_SAME_FILE_EXIST_LOCAL 18 // rename failure, same file exist
#define R_FAIL_SAME_FILE_EXIST_SERVER 19 // rename failure, same file exist
#define R_FAIL_CANNOT_RENAME_FOLDER 20 // canoot rename(maybe root folder)
#define R_FAIL_NO_PERMISSION_TO_RENAME 21 // rename failure, no permission
#define R_FAIL_RENAME_FAILURE 22
#define R_FAIL_LOCAL_MOVE_FAIL 23 // move/rename failure on local
#define R_FAIL_SERVER_MOVE_FAIL 24 // move/rename failure on server
#define R_FAIL_NO_PERMISSION_TO_ECM_DISK 25
#define R_FAIL_ALREADY_INITIALIZING 26 // already initializing sync-folder
#define R_FAIL_SO_LONG_LOCAL_PATH 27
#define R_FAIL_LOCAL_PATH_IS_REMOTE_DISK 28
#define R_FAIL_DOWNLOAD_WRITE_FAIL 29
#define R_FAIL_NO_SYNC_FOLDER 30
#define R_FAIL_DOWNLOAD_NO_CDISK_SPACE 31
#define R_FAIL_NOT_ON_ECMSYSTEM_DISK 32
#define R_FAIL_RENAME_NO_CONFLICT 33 // rename only conflict item
#define R_FAIL_CANNOT_RENAME_ROOT 34
#define R_FAIL_CANNOT_RENAME_FILE 35 // cannot rename(maybe root folder)
#define R_FAIL_ALREADY_SYNCING 36
#define R_FAIL_NO_PERMISSION_TO_LOCAL_WRITE 37
#define R_FAIL_NEED_FULL_SYNC_VERGIN 38
#define R_FAIL_NEED_FULL_SYNC_PARTIAL 39
#define R_FAIL_DELETE_CHILDS 40
#define R_FAIL_CONFLICT_ON_EACH_SIDES 41
#define R_FAIL_DELETED_FILE 42 // deleted file
#define R_FAIL_NO_ACCESS_PERMISSION 43
#define R_FAIL_PARENT_FOLDER_MOVED 44
#define R_FAIL_SEPARATE_FAILURE 45
#define R_FAIL_NO_SECURITY_DISK_SPACE 46
#define R_FAIL_LOCAL_MODIFIED_AFTER_COMPARE 47
#define R_FAIL_CONNECT_BACKUP_VERSION 48
#define R_FAIL_CREATE_BACKUP_VERSION 49
#define R_FAIL_ACCESS_BACKUP_FOLDER 50
#define R_FAIL_SYNCING_MERGED 51

#define MAX_ERROR_CODE 52
#define R_NOTHING_TODO 52

#define R_WARN_BEGIN 100
#define R_WARN_FAILURE_DIRECTORY_MONITOR 100
#define MAX_WARN_CODE 102


#define FAILURE_ON_GETDOCTYPES 14001
#define FAILURE_ON_GETDOCTYPEDATA 14002

const TCHAR *GetSyncCompareResultString(int result);

#define EMPTY_FILE_ESTIMATE_SIZE (1024 * 1024) // 1M

#define DIALOG_FRAME_COLOR 0xF9F6EC
#define DIALOG_BG_COLOR 0xFFFFFF

#define ECM_BUTTON_BORDER_COLOR 0xC2B43A
#define ECM_BUTTON_FACE_COLOR 0xFAFAEC
#define ECM_BUTTON_OVER_COLOR 0xF0EDD6
#define ECM_BUTTON_SELECT_COLOR 0xE0DBB8
#define ECM_BUTTON_DISABLED_COLOR 0xF2F1E5
#define ECM_BUTTON_DISABLED_TEXT 0x999999
#define ECM_BUTTON_DISABLED_BORDER 0xD8D6C9

#define BORDER_FRAME_COLOR 0xF0F0F0
#define BORDER_INNER_COLOR 0xB9B9B9
#define HORZ_SEPARATOR_COLOR 0xA0A0A0
#define VERT_SEPARATOR_COLOR 0xA0A0A0
#define VERT_SEPARATOR_COLOR2 0xEAEAEA

#define CONFLICT_SEPARATOR_COLOR 0xF0F0F0

#define TREE_FOLDER_BG_COLOR 0 // 0xD9F5E2 
#define TREE_ITEM_HIGHLIGHT_COLOR 0xFFF3E5

#define TREE_SELECTED_FOLDER_BG_COLOR 0xF9F8D7 // 0xFFE8CD
#define TREE_SELECTED_ITEM_BG_COLOR 0xF9F8D7 // 0xFFE8CD

// ECM special fault code
#define ECM_AGENT_SERVER_BUSY 1234
#define ECM_AGENT_NO_PERMISSION 10000
#define ECM_AGENT_NOT_LOGIN 100001
#define ECM_AGENT_RFC_WININET_ERROR 101000
#define ECM_AGENT_SAME_FOLDER_EXIST 32005
#define ECM_AGENT_FOLDER_NOT_FOUND 31002

// FileBackup internal
#define ECM_AGENT_INTERNAL_ERROR 99
#define ECM_AGENT_FOLDER_UNKNOWN 100
// #define ECM_AGENT_FOLDER_DELETED 101

typedef enum tagDirModifyEvent {
  DIR_NONE,
  DIR_ADDED,
  DIR_DELETED,
  DIR_MODIFIED,
  DIR_MOVED, // move,rename old name
  DIR_MOVED_NEW, // move,rename new name
  //DIR_DELETE_N_MOVED_NEW, // deleted and rename new name, not-use
} DIR_MODIFY_EVENT;

// return value of IsSyncingFileItem
typedef enum taSFI_TYPE {
  SFI_NO_ITEM,
  SFI_BELOW_SYNC_FOLDER,
  SFI_META_DB,
  SFI_SYNCING_FILE,
  SFI_UPLOADING_FILE,
  SFI_UNKNOWN
} SFI_TYPE;


#endif
