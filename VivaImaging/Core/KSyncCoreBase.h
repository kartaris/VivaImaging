/**
* @file KSyncCoreBase.h
* @date 2015.02
* @brief KSyncCoreBase class header file
*/
#pragma once

#ifndef _LOCALSYNC_SYNCCOREBASE_CLASS_H_
#define _LOCALSYNC_SYNCCOREBASE_CLASS_H_

#include "KConstant.h"
#include "KSyncItem.h"
#include "KSyncFolderItem.h"
#include "KSyncRootFolderItem.h"
#include "KMetadataInfo.h"

#ifdef _DEBUG
// #define _DEBUG_TEST
#endif

#define CHECK_UNREADED_MESSAGE_JOB 0x0001
#define REQUEST_BACKUP_POLICY_JOB 0x0002
#define PRESERVED_RESTORE_JOB 0x0004
#define LAST_UNFINISHED_JOB 0x0008
#define CHECK_AUTOSYNC_JOB 0x0010
#define CLEAR_JOB_MASK 0x0FF

// ECM-Agent will send this message when user left-mouse click tray-icon(wParam == 1),
// or select menu manage-sync-folder(wParam == 2), recent-sync-item(wParam == 3).
#define WM_TRAYICON_MESSAGE (WM_USER + 281)

#define WM_SYNC_COMMAND_MESSAGE (WM_USER + 285)

#define WM_PROCESS_SYNC (WM_USER + 287)

#define WMA_SYNC_BROADCAST (WM_USER + 290)

#define THREADED_CALL_RESULT (WM_USER + 293)

#define WM_SYNC_NOTIFY (WM_USER + 295)

#define SYNC_BUSY_START 1
#define SYNC_BUSY_END 2

typedef struct tagTHREAD_CALL_DATA
{
  HWND hwnd;
  TCHAR folder_oid[MAX_OID];
  int is_document;
  TCHAR local_path[MAX_PATH];
} THREAD_CALL_DATA;


#define LOGOUT_BY_TIMEOUT 0 // LSLS_SESSION_TIMEOUT
#define LOGOUT_BY_USER 1
#define LOGOUT_BY_LOCAL_LOGIN 2// LSLS_LOCAL_LOGIN

/** @enum SyncCommandType
* @brief Sync Command Enum(from LocalSyncShellExt)
*/
enum SyncCommandType {
  /** Sync command none */
  SYNC_CMD_NONE=0,
  /** Set sync folder from local folder */
  SYNC_CMD_SET_LOCAL_FOLDER=1,
  /** Clear sync folder from local folder */
  SYNC_CMD_CLEAR_FOLDER=2,
  /** Start sync folder */
  SYNC_CMD_SYNC_START=3,
  /** Stop sync folder */
  SYNC_CMD_SYNC_STOP=4,
  /** Pause sync folder */
  SYNC_CMD_SYNC_PAUSE=5,
  /** Resume sync folder */
  SYNC_CMD_SYNC_RESUME	=6,
  /** Open sync history dialog */
  SYNC_CMD_VIEW_HISTORY=7,
  /** Open sync folder management */
  SYNC_CMD_VIEW_MANAGE=8,
  /** Exclude sync folder */
  SYNC_CMD_EXCLUDE_FOLDER=9,
  /** Include sync folder */
  SYNC_CMD_INCLUDE_FOLDER=10,
  /** Set sync folder from server folder */
  SYNC_CMD_SET_SERVER_FOLDER=11,
  /** Resolve sync conflicts */
  SYNC_CMD_RESOLVE_CONFLICT=12,
  /** Resolve sync conflicts */
  SYNC_CMD_RESET_SYNC_FOLDER=13,

#ifdef USE_BACKUP
  /** Start full Backup */
  SYNC_CMD_FULL_BACKUP_START,
  /** Start Restore */
  SYNC_CMD_RESTORE_START,
  /** Start Restore for previous backup version */
  SYNC_CMD_RESTORE_PREV_VERSION,
#endif

  /** Notification message when sync policy changed */
  SYNC_CMD_POLICY_CHANGED=20,
  /** Notification message when need to check auto sync schedule */
  SYNC_CMD_CHECK_AUTOSYNC=21,
  /** Notification message when setting done */
  SYNC_CMD_SETTING_DONE=22,
  /** Notification message when resolve conflict completed */
  SYNC_CMD_RESOLVE_DONE=23,
  /** Notification message when user logout */
  SYNC_CMD_LOGOUT=24,
  /** Notification message when user login */
  SYNC_CMD_LOGIN=25,
  /** Notification message when sync folder broken */
  SYNC_CMD_FOLDER_BROKEN=26,
  /** Notification message when sync folder moved */
  SYNC_CMD_FOLDER_MOVED=27,
  /** Notification message when sync done */
  SYNC_NOTI_SYNC_DONE=28,
  /** Notification message when sync canceled */
  SYNC_NOTI_SYNC_CANCEL=29,
  /** Notification message when sync folder need updated */
  SYNC_CMD_NEED_FOLDER_UPDATE=30,
  /** Notification message when drive is ready to use */
  SYNC_CMD_DEVICE_READY=31,
  /** Notification message when language changed */
  SYNC_CMD_LOCALE_CHANGED,
  /** ECM, Security Disk ready */
  SYNC_CMD_DRIVE_READY,

  /** check respite on next day */
  SYNC_CMD_RESPITE_READY,
  /** Backup Polcy is downloaded */
  SYNC_CMD_POLICY_READY,
  /** Show respite-term message */
  SYNC_CMD_SHOW_RESPITE_TERM,
  /** Select Backup Version */
  SYNC_CMD_SELECT_BACKUP_VERSION,
  /** done unreaded push message */
  SYNC_CMD_PUSH_MESSAGE_READY,
  /** start restore in server */
  SYNC_CMD_RESTORE_READY,
  /** cancel restore in server */
  SYNC_CMD_RESTORE_CANCEL,
  /** now ready restore in server */
  SYNC_CMD_RESTORE_DONE,
  /** update backup policy */
  SYNC_CMD_UPDATE_BACKUP_POLICY,
  /** show push message */
  SYNC_CMD_SHOW_PUSH_MESSAGE,
  /** Start Instant sync */
  SYNC_CMD_READY_INSTANT_SYNC,
  /** send Backup/Restore event to server */
  SYNC_NOTI_SYNC_EVENT,
  /** after receive traffic level value, start expected full backup or wait some minutes */
  SYNC_CMD_ON_TRAFFIC_LEVEL,
  /** 백업 버전 연결(폴더 이름 변경) 실패 */
  SYNC_CMD_BACKUP_CONNECT_FAIL,
  /** 백업 폴더 접근 실패 */
  SYNC_CMD_FAIL_ACCESS_BACKUP_FOLDER,
  /** 백업 버전 생성(폴더 생성) 실패 */
  SYNC_CMD_FAIL_CREATE_BACKUP_VERSION,

  /** 백업 로그인 단계 실패하면 10분후 다시 시도한다. */
  SYNC_CMD_RETRY_BACKUP,

  /** 백업 버전 연결 알림 */
  SYNC_CMD_USE_BACKUP_FOLDER,

  /** Message to wakeup. LocalSyncService will send. */
  SYNC_CMD_WAKEUP=80,
  /** Message to quit localsync. LocalSyncService will send. */
  SYNC_CMD_QUIT,
  /** Message to clear all sync folders. LocalSyncService will send. */
  SYNC_CMD_CLEAR_ALL_SYNC,

  /** Message to kill sync timer.*/
  SYNC_CMD_KILL_TIMER=90,
  /** Message to start sync timer.*/
  SYNC_CMD_START_TIMER
};

enum BackupEventType {
  BE_NONE = 0,
  /** 새로운 버전 생성하고 시작한 이벤트 */
  BE_START_VERSION = 1,
  /** 기존 버전 재사용 시작한 이벤트 */
  BE_RESTART_VERSION,
  /** 실시간 백업 이벤트 */
  BE_INSTANT_BACKUP,
  /** 전체 백업 이벤트 */
  BE_FULL_BACKUP_START,
  BE_FULL_BACKUP_END,
  /** 복원 이벤트 */
  BE_RESTORE_START,
  BE_RESTORE_END,
  /** End of 이벤트 */
  BE_MAX
};

enum PushMessageType {
  /** 공지 메시지 */
  PM_NOTIFY_MESSAGE = 1,
  /** 백업 정책 업데이트 메시지 */
  PM_CHANGE_POLICY,
  /** 복원 준비 알림 */
  PM_READY_RESTORE,
  PM_CANCEL_RESTORE,
  PM_START_RESTORE,

  /** 백업 지점 연결 알림 */
  PM_NOTIFY_USE_BACKUP_FOLDER,

  /** 내부 사용 */
  PM_NOTIFY_UNREADED_MESSAGE
};

enum RequestPolicyType {
  BY_LOGIN,
  BY_PUSH
};

typedef struct tagBackupRestoreResultST {
  int method;
  int rootResult;
  int done_flag;

  int syncFolderCount;
  int failFolderCount;
  int syncFileCount;
  int failFileCount;
  TCHAR versionFolderOID[MAX_OID];
  // TCHAR restoreFolderOID[MAX_OID];
} BackupRestoreResultST;

/** 동기화 대기 상태 broadcast 메시지 */
#define BM_SYNC_IDLE 0
/** 동기화 시작 broadcast 메시지 */
#define BM_SYNC_START 100
/** 동기화 준비 broadcast 메시지 */
#define BM_SYNC_READY 110

/** 동기화 일시 멈춤 broadcast 메시지 */
#define BM_SYNC_FREEZE 113
/** 동기화 다시 시작 broadcast 메시지 */
#define BM_SYNC_UNFREEZE 114

/** 동기화 계속 진행 broadcast 메시지 */
#define BM_SYNC_CONTINUE 120
/** 동기화 사용자 일시 정지 broadcast 메시지 */
#define BM_SYNC_PAUSED 130

/** 동기화 완료 broadcast 메시지 */
#define BM_SYNC_DONE 140
/** 동기화 미완료 종료 broadcast 메시지 */
#define BM_SYNC_DONE_UC 141

/** no synchronization job. */
#define BM_SYNC_WAIT 150

#define ARRANGE_ICON_STATE(s) ((s < BM_SYNC_DONE) ? 0 : ((s == BM_SYNC_DONE) ? 1 : 2))

#define IS_MINI_SYNC_STATE(s) (s >= BM_SYNC_WAIT)

/** Sync Folder Status : Manual Sync Folder*/
#define SFS_MANUAL_SYNC   0x0010
/** Sync Folder Status : Auto Sync Folder*/
#define SFS_AUTO_SYNC       0x0020
/** Sync Folder Status : Enable set sync*/
#define SFS_SETSYNCABLE     0x0040
/** Sync Folder Status : Server folder*/
#define SFS_SERVER_FOLDER  0x0080
/** Sync Folder Status : Disable shell menu*/
#define SFS_DISABLE 0x0100
/** Sync Folder Status : Sync folder but not yet synced*/
#define SFS_SYNC_VERGIN   0x0200
/** Sync Folder Status : Broken Sync Folder*/
#define SFS_SYNC_BROKEN 0x0400

/** Sync Folder Status : Sync excluded folder*/
#define SFS_SYNC_EXCLUDED_FOLDER 0x001
/** Sync Folder Status : Sync paused folder*/
#define SFS_SYNC_PAUSED_FOLDER 0x002
/** Sync Folder Status : Sync root folder*/
#define SFS_SYNC_ROOT_FOLDER  0x004
/** Sync Folder Status : Sync conflict folder*/
#define SFS_SYNC_CONFLICT_FOLDER 0x008

/** SyncItem result state : none */
#define LOCAL_SYNC_NONE -1
/** SyncItem result state : unknown */
#define LOCAL_SYNC_UNKNOWN 0
/** SyncItem result state : sync complete */
#define LOCAL_SYNC_COMPLETE 1
/** SyncItem result state : conflict */
#define LOCAL_SYNC_CONFLICT 2
/** SyncItem result state : need download */
#define LOCAL_SYNC_NEED_DOWNLOAD 3
/** SyncItem result state : need upload */
#define LOCAL_SYNC_NEED_UPLOAD 4
/** SyncItem result state : now syncronizing */
#define LOCAL_SYNC_SYNCING 5
/** SyncItem result state : broken sync */
#define LOCAL_SYNC_BROKEN 6

/**
* mStatus values
*/
#define SYNC_STATUS_INIT -6 // 초기 상태
#define SYNC_STATUS_OFFLINE -5 // 로그인 이전 상태
#define SYNC_STATUS_CONNECTED -4 // Agent connected
#define SYNC_STATUS_REQUEST_POLICY -3 // 정책 요청 상태
#define SYNC_STATUS_LOGGED -2 // 정책을 받은 상태(유예 기간 및 IP 제한 상태)
#define SYNC_STATUS_CREATE_BACKUP -1 // 정책을 받고 동기화(백업 지점) 준비하는 상태
#define SYNC_STATUS_IDLE 0 // 동기화 준비하고 대기 상태
#define SYNC_STATUS_BUSY 1
#define SYNC_STATUS_STOP 2 // internal stop
#define SYNC_STATUS_PAUSE 3 // pause
#define SYNC_STATUS_FREEZE 5 // freezed by modal dialog(ConflictDialog)

/** SetSyncFolder() flag : set sync */
#define SYNC_SET 0x01
/** SetSyncFolder() flag : clear sync */
#define SYNC_CLEAR 0x02
/** SetSyncFolder() flag : upload sync */
#define SYNC_UPLOAD 0x04
/** SetSyncFolder() flag : check-only */
#define SYNC_CHECKONLY 0x10
/** SetSyncFolder() flag : reset broken sync folder */
#define SYNC_RESET 0x20

/** Type of folder : sync-root folder */
#define SYNC_ROOT_FOLDER 0x01
/** Type of folder : sync-child folder */
#define SYNC_CHILD_FOLDER 0x02
/** Type of folder : sync-parent folder */
#define SYNC_PARENT_FOLDER 0x04
/** Type of folder : sync unavailable folder */
#define SYNC_UNAVAILABLE 0x10
/** Type of folder : sync brkoen folder(not registered on fs_folders, but locasync.ddb is exist) */
#define SYNC_BROKEN_FOLDER 0x20
#define SYNC_BROKEN_CHILD_FOLDER 0x40
#define SYNC_BROKEN_PARENT_FOLDER 0x80

/**
* mUserFlag definition
*/
#define FLAG_READABLE_NORMAL_DISK 0x001
#define FLAG_WRITABLE_NORMAL_DISK 0x002
#define FLAG_READABLE_SECURITY_DISK 0x0004
#define FLAG_WRITABLE_SECURITY_DISK 0x0008
#define FLAG_READABLE_ECM_DISK 0x0010
#define FLAG_WRITABLE_ECM_DISK 0x0020

#define FLAG_RW_NORMAL_DISK 0x003
#define FLAG_RW_SECURITY_DISK 0x000C
#define FLAG_RW_ECM_DISK 0x0030

/** 
* StopSyncFolder flag definition
*/
#define DONE_BY_LOGOUT 1
#define CANCEL_BY_FORCE 2
#define NO_NOTI_MESSAGE 4
#define DONE_AS_COMPLETE 8
#define DONE_START 0x010
#define DONE_BY_ERROR 0x20
#define DONE_BY_SILENTLY 4

/**
* RefreshSyncPolicyInfo flag definition
*/
#define REFRESH_FORCE 0x001
#define REFRESH_LOCALE 0x010
#define REFRESH_EXCLUDE_MASK 0x020
#define REFRESH_DRIVE_PERMISSION 0x040
#define REFRESH_ALL 0x0F0

/**
* CheckEngineReady flag definition
*/
#define ENGINE_LOGIN 0x01
#define ENGINE_DRIVE_READY 0x02

class KSyncPolicy;

/** @struct tagSyncHistoryItem
* @brief 동기화 기록 아이템 struct
*/
typedef struct tagSyncHistoryItem {
  /** DB row id */
  long id;
  /** 히스토리 이벤트값 */
  int state;
  /** 폴더=1, 파일=0 */
  int IsFolder;
  /** 상대 경로명 */
  LPTSTR pathName;
  /** 추가 정보 */
  LPTSTR extraInfo;
  /** 사용자 OID */
  TCHAR UserOID[12];
  /** 사용자 표시 이름 */
  TCHAR UserName[MAX_USERNAME];
  /** 히스토리 이벤트 제목, 소트할때 사용됨 */
  TCHAR stateName[64];
  /** 시간 정보 */
  SYSTEMTIME syncTime;
} SYNC_HISTORY_ITEM;


// GetSyncRootFolderItems flags
#define SYNC_ROOT_FOR_ANYONE 1
#define SYNC_ROOT_REFRESH_INFO 2

/**
* @class KSyncCoreBase
* @brief 동기화 엔진의 interface class. 이 엔진은 로컬싱크Exec 모듈뿐만 아니라
* Shell Extension인 OverIconDLL, ShellExt에서도 사용되는 모듈이다
*/
class KSyncCoreBase
{
public:
  KSyncCoreBase(void);
  virtual ~KSyncCoreBase(void);

  /**
  * @return BOOL : 언어 로케일 값이 변경되었으면 TRUE를 리턴한다.
  * @brief 화면 표시에 사용하는 언어는 ECM Agent의 언어 설정을 따르며, 이 값은 레지스트리에 저장되어 있다.
  * 언어 로케일값을 읽어 mResourceLanguage 변수에 저장해 둔다.
  */
  BOOL CheckLanguage();

  /**
  * @return BOOL : 폴더가 준비되어 있으면 TRUE를 리턴한다.
  * @brief 로컬싱크의 프로그램 폴더(C:/Cyberdigm/DestinyLocalSync) 경로명을 버퍼에 복사하고,
  * 해당 폴더가 존재하지 않으면 생성한다.
  * @param folderPath : 출력 버퍼 포인터
  */
  static BOOL GetRootFolder(LPTSTR folderPath);

  /**
  * @return BOOL
  * @brief mHomeFolder 변수가 비어있으면 여기에 현재 사용자의 홈 폴더 경로명(C:/Cyberdigm/DestinyLocalSync/’사용자OID’)을
  * 저장하고 폴더가 존재하지 않으면 새로 생성한다
  * @param createIfNotExist : 폴더가 없으면 생성한다.
  * @param force : 강제로 업데이트 한다.
  */
  static BOOL GetHomeFolder(BOOL createIfNotExist, BOOL force = FALSE);

  /**
  * @return BOOL : ECM Agent가 준비상태이면 TRUE를 리턴한다.
  * @brief ECM Agent가 유효한 상태인지 검사한다. 
  * @details ECM Agent는  "Global\\Mutex_X_DestinyECMAGENT" 라는 이름의 Mutex를 생성하고,
  * 여기에서는 이 mutex를 새로 생성해서 결과가 ERROR_ALREADY_EXISTS이면
  * Agent가 살아있는 상태로 판단하고, 아니면 없는 것으로 판단한다
  */
  static BOOL IsECMAgentReady();

  BOOL GetSecurityDisk(LPTSTR pathname);

  /**
  * @return BOOL : 보안 디스크 영역이 준비된 상태이면 TRUE를 리턴한다.
  * @brief 보안 디스크 영역이 준비된 상태인지 확인한다.
  */
  BOOL IsSecurityDiskReady();

  /**
  * @return BOOL : 대상 경로가 보안 디스크 영역이면 TRUE를 리턴한다.
  * @brief 대상 경로가 보안 디스크 영역인지 검사한다.
  */
  BOOL IsSecurityDisk(LPCTSTR pathname);

  /**
  * @return int : R_SUCCESS or error code
  * @brief 대상 폴더가 동기화 설정이 가능한지 검사하여 오류가 있으면 오류 코드를, 설정 가능하면 R_SUCCESS를 리턴한다
  * @param set : flag(SET, CLEAR,,,)
  * @param  local_path : 대상 폴더
  * @param root_path : 찾은 동기화 루트 폴더
  * @details A. local_path가 유효하고 디렉토리가 존재하는지 검사한다.
  * @n B. local_path 경로명 길이가 최대 길이(200자)를 넘는지 검사한다. (하위에 ‘.localsync’폴더와 메타 파일, 설정 파일 등을 만들어야 하기 때문에)
  * @n C. ECM 사용자 권한(mUserFlag)에서 ECM 서버 접근 권한이 있는지 검사한다.
  * @n D. 로컬측 경로가 ECM 드라이브 또는 ECM 가상 드라이브 영역이면 오류로 리턴한다.
  * @n E.	 로컬측 경로가 보안 드라이브 영역이면 보안 디스크 접근 권한이 있는지 검사한다.
  * @n F.	 로컬 드라이브의 접근 권한이 있는지 검사한다.
  * @n G. 로컬측 쓰기 권한이 없거나 서버측 쓰기 권한이 없으면 오류로 리턴한다.
  * @n H. 로컬측 폴더의 디렉토리 권한이 ReadOnly이면 오류로 리턴한다.
  * @n I.	 로컬측 폴더가 시스템 디렉토리이면 오류로 리턴한다.
  * @n J.	 로컬측 드라이브가 고정 디스크(FixedDisk, 이동식 디스크는 안됨)가 아니면 오류로 리턴한다.
  * @n K. 로컬측 폴더가 이미 동기화 폴더로 설정되어 있는지, 또는 이미 설정된 동기화 폴더의 하위 폴더인지, 또는 하위 폴더가 동기화 폴더로 설정되어 있는지 검사한다
  * @n L.	 모두 해당되지 않으면 R_SUCCESS로 리턴한다. 
  */
  int CheckSyncableFolder(int set, LPCTSTR local_path, LPTSTR root_path);

  /**
  * @return LPTSTR : 동기화 루트 폴더 목록
  * @brief 현재의 동기화 루트 폴더 목록을 Comma-separated-string으로 만들어 리턴한다.
  * @details A. 셋팅 파일에 저장된 동기화 루트 폴더 목록을 GetCurrentSyncFoldersT() 메소드로 받아온다.
  * @n B. 하나씩 잘라내서 IsValidSyncRootFolderItem()메소드로 유효한 동기화 루트 폴더임을 체크하고, 유효한 경우에만 버퍼에 복사한다.
  * @n C. A에서 받아온 버퍼를 해제하고 새로 준비한 버퍼를 리턴한다.
  * caller should free returned pointer with delete[]
  */
  static LPTSTR GetSyncRootFolderPaths();

  /**
  * @return BOOL : 특정 사용자의 동기화 루트 폴더이면 TRUE를 리턴
  * @brief 대상 폴더가 특정 사용자의 동기화 루트 폴더인지 확인한다
  * @param folder_path : 대상 폴더
  * @param owner : 사용자 OID
  * @details A. 대상 폴더가 동기화 루트 폴더인지 확인한다.
  * @n B. 동기화 루트 폴더의 정보를 getServerRootPathOnSetting() 메소드로 받아와서 사용자를 비교하여 동일하면 TRUE를 리턴한다.
  * @n C. 아니면 FALSE를 리턴한다.
  */
  static BOOL IsValidSyncRootFolderItem(LPCTSTR folder_path, LPCTSTR owner);

  /**
  * @brief 로그인 처리를 해야하면 TRUE를 리턴한다.
  */
  virtual BOOL CheckEngineReady(int s) { return FALSE; }

  /**
  * @brief 로그인 또는 로그아웃 되었을 때  호출되는 가상함수
  *@return BOOL : 동기화 타이머가 필요하면 TRUE를 리턴한다.
  */
  virtual BOOL OnLogged(BOOL bOn) { return FALSE; }

  /**
  * @brief 서버측 정책을 다시 요청할 필요가 있는지 검사하는 가상함수
  * @n 지난번 요청이후 날이 바뀌면 다시 요청할 필요가 있는 것으로 판단한다.
  *@return BOOL : 요청이 성공하면 TRUE를 리턴한다.
  */
  virtual BOOL IsNeedUpdateServerPolicy() { return FALSE; }


  int mUseLocalSync;
  /** 엔진 상태(IDLE, READY, OFFLINE,,) */
  int mStatus;
  /** 동기화 작업에서 동기화 가능한 최대 파일 크기, 이보다 큰 파일은 무시됨 */
  int mMaxSyncFileSizeMB;
  /** 사용자의 로컬/보안/서버 디스크 접근권한 */
  int mUserFlag;
  /** 보안 디스크 문자 이름 */
  char mSecurityDrive;
  /** ECM 디스크 문자 이름 */
  char mEcmDrive;
  /** ECM 가상 디스크 문자 이름 */
  char mEcmVirtualDrive;

  /**
  * @brief 일반 디스크 쓰기 권한 체크
  */
  inline BOOL IsNormalDiskWritable() { return !!(mUserFlag & FLAG_WRITABLE_NORMAL_DISK); }
  /**
  * @brief 보안 디스크 쓰기 권한 체크
  */
  inline BOOL IsSecurityDiskWritable() { return !!(mUserFlag & FLAG_WRITABLE_SECURITY_DISK); }
  /**
  * @brief ECM 디스크 쓰기 권한 체크
  */
  inline BOOL IsECMDiskWritable() { return !!(mUserFlag & FLAG_WRITABLE_ECM_DISK); }

  inline BOOL IsECMDiskReadable() { return !!(mUserFlag & FLAG_READABLE_ECM_DISK); }

  /**
  * @brief 개체가 삭제될 때, 외부 데이터를 Clear하는 가상함수
  */
  virtual void ClearExternal() { ; }

  /**
  * @brief 개체가 생성되고 초기화 될때 호출되는 가상함수
  */
  virtual BOOL Ready(BOOL asMain);

  /**
  * @brief 동기화(백업) 허용된 상태인지 확인함
  */
  virtual BOOL IsSyncAllowed();

  /**
  * @brief 로그인을 확인하거나 자동 로그인을 처리하기 위한 가상함수
  */
  virtual BOOL CheckAutoLogin(BOOL autoLogin) { return FALSE; }

  /**
  * @brief 서버의 설정값을 받아와 초기화하는 가상함수
  */
  virtual void InitializeServerSetting(int set, int logoutMethod) { ; }
  /**
  * @brief 사용자 로그인 정보를 저장해두기 위한 가상함수
  */
  virtual void SetLoginUserInfo(struct SessionValue *pSession) { ; }

  /**
  * @brief 서버의 동기화 정책을 업데이트하는 가상함수
  */
  virtual BOOL RefreshSyncPolicyInfo(int flag) { return FALSE; }
  /**
  * @brief 서버의 동기화 정책을 저장해두는 가상함수
  */
  virtual BOOL UpdateServerPolicyInfo(void *pPolicy, BOOL updateSetting) { return FALSE; }


  /**
  * @brief 동기화 폴더가 이미 준비중인지 확인하는 가상함수
  */
  virtual int CheckIfPendingExist(LPCTSTR folder, int flag, LPCTSTR syncOnlyPath) { return 0; }

  virtual BOOL HasSchedule() { return FALSE; }

  /**
  * @brief 동기화 폴더가 이미 준비중인지 확인하는 가상함수
  */
  virtual KSyncRootFolderItem* GetRootFolderItem(LPCTSTR folder, int flag, LPCTSTR syncOnlyPath) { return NULL; }

  /**
  * @brief 동기화 작업을 진행하는 가상함수
  */
  virtual int ContinueSync() { return R_SUCCESS; }

  virtual BOOL IsLoaderNeedTimer() { return TRUE; }

  virtual void dumpLoaders() { ; }

  /**
  * @brief 동기화 작업 전체를 중단하는 가상함수
  */
  virtual int StopSyncFolder(int flag) { return R_SUCCESS; }

  /**
  * @brief 현재 진행중인 동기화 작업을 중단하는 가상함수
  */
  virtual int StopCurrentSyncFolder(int flag) { return R_SUCCESS; }

  virtual int EndCurrentSyncFolder() { return R_SUCCESS; }

  /**
  * @brief 동기화 작업을 일시 멈추는 가상함수
  */
  virtual int PauseSyncFolder() { return R_SUCCESS; }

  /**
  * @brief 일시 멈춘 동기화 작업을 다시 진행하는 가상함수
  */
  virtual int ResumeSyncFolder() { return R_SUCCESS; }

  /**
  * @brief 동기화 작업중인지 검사하는 가상함수
  */
  virtual BOOL IsCurrentSyncingFolder(LPCTSTR path) { return FALSE; }


  /**
  * @brief 현재 진행중인 동기화 작업의 작업을 일시 중지하거나 계속하는 가상함수
  */
  virtual void Freeze(BOOL freeze) { ;}
  /**
  * @brief 현재 진행중인 동기화 작업이 일시 중지되었는지 검사하는 가상함수
  */
  virtual BOOL IsFreezed() { return (mStatus == SYNC_STATUS_FREEZE); }


  /**
  * @brief 디스크 용량을 확보 및 체크하는 가상함수
  */
  virtual int CheckReadyDiskSpace(LPCTSTR filename, __int64 needed_size) { return R_SUCCESS; } // need 16K minimum

  /**
  * @brief 파일을 서버에서 다운로드하는 가상함수
  */
  virtual int StartDownloadItemFile(LPCTSTR filename, LPCTSTR documentOID, LPCTSTR fileOID, LPCTSTR storageOID, void* item) { return 0;}
  /**
  * @brief 파일을 서버로 업로드하는 가상함수
  */
  virtual int StartUploadItemFile(LPCTSTR IN filename, LPTSTR IN OUT storageOID, void* item) { return 0;}

  /**
  * @brief 대상 문서 파일을 Checkout하고 서버로 업로드하는 가상함수.
  * @param storageOID : 동기화 대상 파일의 storageOID, 처음 업로드되는 문서에서는 일련번호로 대치된다.
  */
  virtual int StartUploadItemFile(LPCTSTR IN filename, LPCTSTR IN serverOID, LPCTSTR IN fileOID, LPTSTR IN OUT storageOID,
    LPCTSTR IN parentFolderOID, LPCTSTR IN parentFullPathIndex, void* item) { return 0;}

#ifdef USE_SCAN_BY_HTTP_REQUEST
  /**
  * @brief getModifiedNDocument() API를 이용하여 특정 폴더 하위의 수정된 파일 목록을 다운로드하는 가상함수
  */
  virtual int StartScanFolder(KSyncRootFolderItem *root, KSyncItem *item) { return 0; }
#endif


  /**
  * @brief 서버측 폴더/파일의 정보를 다운로드하는 가상함수
  */
  virtual int GetServerFolderSyncInfo(KSyncRootFolderItem *root, LPCTSTR folder_oid, SERVER_INFO &info) { return 0; }

  /**
  * @brief 동기화 상태를 받아가는 가상함수
  */
  virtual inline int GetSyncStatus() { return mStatus; }

  /**
  * @brief 동기화 상태를 설정하는 가상함수
  */
  virtual inline void SetSyncStatus(int s) { mStatus = s; }

  /**
  * @brief 서버측에 대상 폴더 안에 특정 폴더가 존재하는지 검사하는 가상함수
  */
  virtual BOOL CheckServerSubFolderExist(LPCTSTR folder_oid, LPCTSTR subfolder, LPTSTR outFolderOID) { return FALSE; }

  /**
  * @brief 서버측에서 대상 문서의 정보를 받아오는 가상함수
  */
  virtual int GetServerDocumentSyncInfo(LPCTSTR IN document_oid, SERVER_INFO &info) { return R_SUCCESS; }
  /*
  virtual BOOL GetServerDocumentSyncInfo(LPCTSTR IN document_oid, LPTSTR OUT name, __int64 OUT *fileSize, SYSTEMTIME OUT *time, 
    int OUT *permissions, LPTSTR OUT fullpath, LPTSTR OUT lastUser, LPTSTR OUT fileOID, LPTSTR OUT storageFileOID) { return FALSE; }
  */

  /**
  * @brief 서버측의 대상 폴더를 선택하는 가상함수
  */
  virtual BOOL SelectSyncServerFolderLocation(HWND hwnd, LPCTSTR localFolder, LPCTSTR server_folder_oid) { return FALSE; }
  /**
  * @brief 서버측의 폴더를 생성하는 가상함수
  */
  virtual BOOL SyncServerCreateFolder(KSyncRootFolderItem *root, LPCTSTR IN server_folder_oid, LPCTSTR IN server_folder_index,
    LPCTSTR IN name, LPTSTR OUT out_child_folder_oid, SERVER_INFO& OUT out_child_folder_info, LPTSTR* OUT pErrorMsg) { return FALSE; }
  /**
  * @brief 서버측의 폴더 이름을 받아오는 가상함수
  */
  virtual BOOL SyncServerGetFolderName(LPCTSTR server_folder_oid, LPTSTR folder_name, BOOL fullPath, LPTSTR folder_index) { return FALSE; }

  virtual int SyncServerUploadFile(LPCTSTR folderOid, LPCTSTR folderIndex, LPTSTR docOID,
    LPTSTR serverFileOID, LPTSTR serverStorageOID, LPCTSTR name, LPCTSTR localFileName, 
    KEcmDocTypeInfo* pInfo, KMetadataInfo* pMetadata, LPTSTR* pErrorMessage) {
    return -1;
  }

  virtual int SyncServerDownloadFile(LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR localFileName, LPTSTR* pErrorMessage) { return -1; }

  virtual BOOL SyncServerGetDocTypeInfo(LPCTSTR docTypeOid, KEcmDocTypeInfo* pInfo) { return FALSE; }

  virtual int SyncServerGetDocumentExt(LPCTSTR IN document_oid, KMetadataInfo* pInfo, LPTSTR* pErrorMessage) { return -1; }

  /**
  * @brief 로컬측의 파일을 삭제하는 가상함수
  */
  virtual int SyncLocalDeleteFile(LPCTSTR filename) { return 0; }
  /**
  * @brief 로컬측의 폴더를 삭제하는 가상함수
  */
  virtual int SyncLocalDeleteFolder(LPCTSTR foldername) { return 0; }
  /**
  * @brief 서버측의 파일을 삭제하는 가상함수
  */
  virtual BOOL SyncServerDeleteFile(LPCTSTR documentOID, LPCTSTR fileOID, LPCTSTR folderOID, LPCTSTR folderFullPathIndex, LPTSTR* OUT pErrorMsg) { return FALSE; }
  /**
  * @brief 서버측의 폴더을 삭제하는 가상함수
  */
  virtual BOOL SyncServerDeleteFolder(LPCTSTR folder_OID, LPCTSTR folder_index, LPTSTR* pErrorMsg) { return FALSE; }
  /**
  * @brief 서버측의 폴더가 존재하는지 검사하는 가상함수
  */
  virtual int SyncServerCheckFolderExist(LPCTSTR folder_OID, LPCTSTR folderFullPathIndex, LPTSTR pErrorMsg) { return 1; }

  /**
  * @brief 서버측에 파일을 업로드하는 가상함수
  */
  virtual int UploadItemFile(LPCTSTR filename, LPTSTR fileID, LPTSTR pErrorMsg) { return 0; }

  /**
  * @brief 서버측에 업로드된 파일을 등록하여 문서를 생성하는 가상함수
  */
  virtual int UpdateUploadDocumentItem(KSyncItem &item, LPCTSTR IN folderOID, LPCTSTR IN folderFullPathIndex) { return 0; }

  /**
  * @brief 서버측에 폴더나 파일을 다른 폴더로 이동하는 가상함수
  */
  virtual int MoveItemTo(LPCTSTR itemOID, LPCTSTR targetFolderOID, LPCTSTR targetFullPathIndex, BOOL isFolder, LPTSTR* OUT err_msg) { return FALSE; }
  /**
  * @brief 서버측에서 문서의 상위 폴더를 얻는 가상함수
  */
  virtual BOOL GetParentOfDocumentOID(LPCTSTR IN documentOID, LPTSTR OUT parentOID) { return FALSE; }
  /**
  * @brief 서버측에서 문서의 이름을 얻는 가상함수
  */
  virtual BOOL GetNameOfDocumentOID(LPCTSTR IN documentOID, LPTSTR OUT documentName) { return FALSE; }
  /**
  * @brief 서버측에서 폴더의 부모 폴더 OID와 이름을 얻는 가상함수
  */
  virtual BOOL GetFolderInfo(LPCTSTR IN folderOID, LPTSTR OUT parentOID, LPTSTR OUT folderName) { return FALSE; }

  /**
  * @brief 서버측에서 문서의 이름을 바꾸는 가상함수
  */
  virtual BOOL ChangeDocumentName(LPCTSTR IN documentOID, LPCTSTR IN fileName, LPCTSTR IN documentFileOID, LPCTSTR IN fullPathIndex, LPTSTR* OUT err_msg) { return FALSE; }
  /**
  * @brief 서버측에서 폴더의 이름을 바꾸는 가상함수
  */
  virtual BOOL ChangeFolderName(LPCTSTR IN folderOID, LPCTSTR IN fullPathIndex, LPCTSTR IN lastName, LPCTSTR IN folderName, LPTSTR* OUT err_msg) { return FALSE; }

  /**
  * @brief 서버에서 파일을 임시 파일로 다운로드하는 가상함수
  */
  virtual BOOL DownloadTempFile(LPCTSTR filename, LPTSTR documentOID,
    LPTSTR storageOID) { return FALSE; }
  /**
  * @brief 서버에서 파일을 CheckOut하는 가상함수
  */
  virtual int CheckOutDocument_deprecated(LPCTSTR IN documentOID, LPCTSTR IN fileOID, LPCTSTR IN folderOID, LPCTSTR IN fullPathIndex, LPTSTR* OUT error_msg) { return 0; }
  /**
  * @brief 서버에서 파일을 Unlock하는 가상함수
  */
  virtual BOOL UnlockDocument(LPCTSTR fileOID, LPTSTR err_msg) { return FALSE; }

  /**
  * @brief 서버에서 대상 폴더나 파일을 읽기전용으로 여는 가상함수
  */
  virtual BOOL OpenServerObject(LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR fileName, BOOL isFolder, LPTSTR errMessage) { return FALSE; }
  /**
  * @brief 서버에서 대상 파일의 속성 보기창을 여는 가상함수
  */
  virtual BOOL ShowObjectProperties(HWND hwnd, LPTSTR documentOID, BOOL isDocument) { return FALSE; }

  /**
  * @brief 서버에서 동일한 이름의 문서가 있는지 검사하는 가상함수
  */
  virtual BOOL IsFileExistServer(LPCTSTR folderOID, LPCTSTR documentOID, LPCTSTR filename) { return FALSE; }
  /**
  * @brief 서버에서 동일한 이름의 폴더가 있는지 검사하는 가상함수
  */
  virtual BOOL IsFolderExistServer(LPCTSTR folderOID, LPCTSTR subfolderOID, LPCTSTR foldername) { return FALSE; }

  /**
  * @brief 서버에서 키에 해당하는 설정값을 받아오는 가상함수
  * @n returned pointer should be freed by using delete[]
  */
  virtual LPTSTR GetServerConfig(LPCTSTR key) { return NULL; }

  /**
  * @brief 임시 파일들을 삭제하는 가상함수
  */
  virtual void ClearTempFiles() { ; }

  /**
  * @brief 임시 파일 이름을 준비하는 가상함수
  */
  virtual BOOL ReadyTempPathName(LPTSTR outFullname, int outBufferSize, LPCTSTR rootFolder, LPCTSTR originalPath, LPCTSTR originalFilename) { return FALSE; }


  /**
  * @brief 파일 다운로드시 진행 상태값을 받는 가상함수
  */
  virtual LRESULT OnFileSyncProgress(LPCTSTR name, int progress) { return 0; }

#ifdef _USING_FILE_MONITORING
  /**
  * @brief 로컬측 파일의 이름이 바뀌었을 때 불리는 가상함수
  */
  virtual void OnFileRenamed(LPCTSTR strOldFileName, LPCTSTR strNewFileName) { ;}
  /**
  * @brief 로컬측 파일의 이름 변경이 취소되었을 때 불리는 가상함수
  */
  virtual void OnFileRenameRemoved(LPCTSTR strFileName, LPCTSTR oldFileName) { ;}

  /**
  * @brief 로컬측 파일이 추가되었을 때 불리는 가상함수
  */
  virtual void OnFileAdded(LPCTSTR strFileName) { ;}
  /**
  * @brief 로컬측 파일이 삭제되었을 때 불리는 가상함수
  */
  virtual void OnFileRemoved(LPCTSTR strFileName) { ;}
  /**
  * @brief 로컬측 파일이 수정되었을 때 불리는 가상함수
  */
  virtual void OnFileModified(LPCTSTR strFileName) { ;}
  /**
  * @brief 로컬측 파일이 이름 바뀌었을 때 불리는 가상함수
  */
  virtual void OnFileNameChanged(LPCTSTR strFileName) { ;}

  /**
  * @brief 동기화 상태 모니터를 등록하는 가상함수
  */
  virtual void RegisterMonitorStatus(LPVOID wnd) { ;}
  /**
  * @brief 동기화 상태 모니터를 등록 해제하는 가상함수
  */
  virtual void UnregisterMonitorStatus(LPVOID wnd) { ;}
#endif // _USING_FILE_MONITORING

  /**
  * @brief 사용자 OID에 대한 사용자 표시 이름을 리턴하는 가상함수
  */
  virtual BOOL GetUserName(LPTSTR IN userId, LPTSTR OUT name) { StringCchCopy(name, MAX_USERNAME, userId); return TRUE; }

  /**
  * @brief 동기화 진행상태를 모니터링하는 윈도우에 진행 상태 이벤트 메시지를 보낸다.
  */
  virtual void BroadcastToMonitor(WPARAM wparam, LPARAM lparam) { ; }

  /**
  * @brief 메시지를 메인 윈도우에 보내는 가상함수
  */
  virtual void PostMessageToMain(int msg, WPARAM wParam, LPARAM lParam) { ; }

  /**
  * @brief 메인 윈도우에 해당 메시지를 보낸다.
  */
  virtual void SendMessageToMain(int msg, WPARAM wParam, LPARAM lParam) { ; }

  /**
  * @brief 모달 윈도우를 등록하 가상함수
  */
  virtual void RegisterModalWnd(void *p) { ; }
  /**
  * @brief 메시지를 모달 윈도우에 보내는 가상함수
  */
  virtual void PostMessageToModal(int msg, WPARAM wParam, LPARAM lParam) { ; }

  /**
  * @brief 사용자의 로그인 시간을 받아가는 가상함수
  */
  virtual inline void GetLoginTime(SYSTEMTIME *pTime) { ; }

  virtual LPCTSTR GetServerSystemRootFolder() { return NULL; }

  /**
  * @brief 모달 윈도우가 시작되거나 끝날때 호출되는 가상함수
  */
  virtual void OnModalStarted(BOOL started) { ; }

  virtual BOOL IsOnModal() { return FALSE; }



  /**
  * @return int : 폴더 상태값을 리턴한다.(SYNC_ENABLE_DOWNLOAD, SYNC_ENABLE_UPLOAD)
  * @brief 현재의 사용자 권한이 대상 폴더에 대해 동기화 폴더로 설정 가능한 상태인지 검사한다.
  * @details A.	ECM 서버 읽기/쓰기 권한이 있는지 검사하여 없으면 0을 리턴한다.
  * @n B.	ECM 서버 쓰기 권한이 있으면 업로드 가능.
  * @n C.	대상 폴더가 보안 드라이브 영역일 때, 사용자의 보안 드라이브 접근 권한이 없으면 0으로 하고, 보안 드라이브 쓰기 권한이 있으면 다운로드 가능.
  * @n D.	대상 폴더가 일반 드라이브 영역이면 로컬 드라이브 쓰기 권한이 있으면 다운로드 가능.
  */
  int GetSyncAvailabilityByDrivePermission(LPCTSTR baseFolder);

  /**
  * @return BOOL : 사용자 OID 목록을 리턴한다.
  * @brief 현재 시스템에 등록된 동기화 사용자의 목록을 리턴한다.
  * @details A.	동기화 홈 폴더(C:/Cyberdigm/DestinyLocalSync)의 하위 폴더에서 FildFirstFile을 이용하여 폴더를 검색한다.
  * @n B.	사용자 OID를 찾아 “C:/Cyberdigm/DestinyLocalSync/’사용자OID’/fs_folders.txt” 파일이 존재하는지 검사하여 존재하면 해당 사용자 OID는 유효한 것으로 판단하여 리스트에 추가한다.
  * @n C.	준비된 리스트 버퍼를 리턴한다.
  * @n caller should free returned memory by delete[]
  */
  LPTSTR GetCurrentSyncUsers();

  /**
  * @return void
  * @brief 시스템에 설정된 모든 동기화 폴더의 설정을 해제한다.
  * @details A.	현재 시스템에 등록된 동기화 사용자의 목록을 구한다.
  * @n B.	각각 동기화 사용자에 대해 DeleteUserSyncFolder() 메소드를 호출한다.
  */
  void ClearAllSyncFolder();

  /**
  * @return void
  * @brief 대상 사용자의 동기화 루트 폴더의 설정을 모두 지우고 메타 데이터도 모두 삭제한다.
  * @details A.	대상 사용자의 동기화 루트 폴더 리스트를 구한다.
  * @n B.	각각의 동기화 루트 폴더에 대해 DeleteMetaDataFolder()메소드를 호출하여 메타 데이터를 삭제한다.
  * @n C.	ShellNotifyIconChangeOverIcon() 메소드를 호출하여 탐색기에서 아이콘이 업데이트 되도록 한다.
  * @n D.	RemoveSyncFolderList() 메소드를 호출하여 사용자의 동기화 폴더 정보 파일(fs_folders.txt)를 삭제한다.
  */
  void DeleteUserSyncFolder(LPTSTR user);

  /**
  * @return BOOL : 정상 폴더이면 TRUE를 리턴한다.
  * @brief 동기화 설정이 깨진 폴더의 정보를 삭제한다.
  * @details A.	전체 사용자의 동기화 폴더 목록을 받아온다.
  * @n B.	대상 폴더가 위의 동기화 폴더 목록에 포함되지 않았으면 DeleteMetaDataFolder() 메소드로 클리어한다.
  * @n C.	대상 폴더 경로가 드라이브의 루트가 아니면 경로의 뒤쪽 이름을 잘라내고 다시 이 함수를 호출한다.
  */
  BOOL ClearBrokenSyncRootFolder(LPTSTR szPath);

#ifdef PERFORMANCE_ANALYSYS
  virtual void performanceCheck_AddTransferTime(DWORD startTick, DWORD endTick) { ; }
#endif

#ifdef USE_LAZY_UPLOAD
  virtual void SetTrafficMonitorValue(int value) { ; }
  virtual int GetTrafficMonitorValue() { return 0; }
#endif

  virtual inline void SetServerBusy(BOOL busy) { ; }
  virtual inline BOOL IsServerBusy() { return FALSE; }

};

extern KSyncCoreBase* gpCoreEngine;

#ifdef USE_SYNC_STORAGE
extern void FreeSyncHistoryItem(SYNC_HISTORY_ITEM *item, BOOL freeItself);
extern void FreeSyncHistoryItemList(KSyncHistoryItemArray& itemArray);
#endif

#ifdef USE_BACKUP
extern void FreeModifiedItemList(KVoidPtrArray& modifiedList);

#define ONLY_SAME_DRIVE 1 /** MergeModifiedItemList에서 사용하는 플래그 */

/**
* @brief 실시간 백업에서 사용하는 백업 대상 목록 ModifiedItemData를 머지한다.
* @param destList : 머지된 결과 리스트
* @param srcList : 소스 리스트
* @param flag : 플래그. ONLY_SAME_DRIVE : 드라이브가 다른 항목은 머지하지 않는다.
* @details A.	modifyType이 DIR_DELETED, DIR_MOVED인 항목은 경로명 확인하지 않고 머지한다.
* @n B.	modifyType이 DIR_ADDED,DIR_MODIFIED,DIR_MOVED_NEW인 항목은 경로명이 존재하면 머지한다.
* @n C.	플래그에 ONLY_SAME_DRIVE가 있으면 결과 리스트에 동일한 드라이브 경로명인 항목만 머지한다.
*/
extern int MergeModifiedItemList(KVoidPtrArray& destList, KVoidPtrArray& srcList, int flag);
#endif

/**
* @brief 파일명 변경에서 이전 이름과 새 이름이 MS Office 파일 저장시에 나타나는 형식인지 확인한다.
* @param tempFileName : 임시 파일명(확장자 없거나 *.tmp)
* @param newFileName : MS Office file(doc,xls,ppt)
* @return BOOL : 두 파일명의 형식이 맞으면 TRUE를 리턴한다.
*/
extern BOOL isMsOfficeTempStore(LPCTSTR tempFileName, LPCTSTR newFileName);

#endif
