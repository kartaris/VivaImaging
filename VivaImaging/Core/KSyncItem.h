/**
* @file KSyncItem.h
* @date 2015.02
* @brief KSyncItem class header file
*/
#pragma once

#ifndef _DESTINY_FILE_SYNC_ITEM_CLASS_H_
#define _DESTINY_FILE_SYNC_ITEM_CLASS_H_

#include "KConstant.h"
#include "../Platform/Platform.h"

#ifdef USE_SQLITE_META
#include "../SqliteLib/sqlite3.h"
#endif

#ifdef USE_META_STORAGE
#include "KSyncRootStorage.h"
#endif

class KSyncRootFolderItem;

#define MAX_LONG_SYNC_LOCAL_PATH 200

#define ITEM_STATE_NONE 0x0 /** 상태 값 없음 */
#define ITEM_STATE_NOW_EXIST 0x01 /** 대상 항목이 현재 존재함 */
#define ITEM_STATE_WAS_EXIST 0x02 /** 대상 항목이 존재하고 있었음 */
#define ITEM_STATE_UPDATED 0x04 /** 대상 항목이 변경되었음 */
#define ITEM_STATE_DELETED 0x08 /** 대상 항목이 삭제됨 */
#define ITEM_STATE_MOVED 0x10 /** 대상 항목이 이동됨 */
#define ITEM_STATE_PARENT_MOVED 0x20 /** 대상 항목의 부모 폴더가 이동됨 */
#define ITEM_STATE_CONFLICT_RENAMED 0x40 /** 대상 항목이 충돌 처리에서 이름 변경됨 */
#define ITEM_STATE_RENAMED 0x80 /** used only ConflictTreeXCtrl temporarily */

#define ITEM_STATE_EXISTING (ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST)

// persisting states
#define ITEM_STATE_NEWLY_ADDED 0x100 /** 대상 항목이 새로 추가된 것임 */
#define ITEM_STATE_NO_PERMISSION 0x200 /** 대상 항목이 권한 없음 상태임 */
#define ITEM_STATE_ERROR 0x400 /** 대상 항목이 동기화 오류 상태임 */
#define ITEM_STATE_REFRESH_PATH 0x0800 /** 서버측 경로를 업데이트하고 DB에서 경로를 업데이트할 필요 있음 */
#define ITEM_STATE_LOCATION_CHANGED 0x1000 /** 대상 항목의 위치가 변경됨 */
#define ITEM_STATE_RE_UPDATED 0x2000 /** 대상 항목이 동기화 비교 이후 다시 수정됨 */
#define ITEM_STATE_RESTORED 0x4000 /** 대상 항목이 삭제되었다가 복구됨 */

#define ITEM_STATE_PERSISTING 0xFF00

#define ITEM_STATE_CLEAR_MASK (ITEM_STATE_PERSISTING | ITEM_STATE_EXISTING)

// mServerScanState
#define ITEM_NO_CHANGE 0 /** 대상 항목이 변동 없음 */
#define ITEM_EXIST 1 /** 대상 항목이 존재함 */
#define ITEM_DELETED 2 /** 대상 항목이 삭제됨 */

// for itself
#define SFP_LOOKUP 0x0001 /** 대상 항목의 조회 권한 */
#define SFP_READ 0x0002 /** 대상 항목의 읽기 권한 */
#define SFP_WRITE 0x0004 /** 대상 항목의 쓰기 권한 */
#define SFP_DELETE 0x0008 /** 대상 항목의 삭제 권한 */
#define SFP_SETUP 0x0010 // old 0x000F  /** 대상 항목의 전체 권한 */

// for childs
#define SFP_INSERT_CONTAINER 0x0020 // old 0x0010  /** 대상 항목의 하위 폴더 생성 권한 */
#define SFP_INSERT_ELEMENT 0x0040 // old 0x0020 /** 대상 항목의 하위 문서 생성 권한 */

#define SFP_LOOKUP_ELEMENT 0x0080 /** 대상 항목의 하위 항목 조회 권한 */
#define SFP_READ_ELEMENT 0x0100 /** 대상 항목의 하위 항목 읽기 권한 */
#define SFP_WRITE_ELEMENT 0x0200 /** 대상 항목의 하위 항목 쓰기 권한 */
#define SFP_DELETE_ELEMENT 0x0400 /** 대상 항목의 하위 항목 삭제 권한 */
#define SFP_SETUP_ELEMENT 0x0800 // old 0x0FF0  /** 대상 항목의 하위 항목 전체 권한 */

#define SFP_FULL_SETUP 0x0FFF  /** 대상 항목의 전체 권한 */

// item flag(stored)
#define SYNC_ITEM_EXCLUDED 0x001
#define SYNC_ITEM_OVERRIDED 0x002
#define SYNC_ITEM_NEED_SERVER_RESCAN 0x004
#define SYNC_FLAG_STORED 0x00F
// item flag(unstored, temporarily used)
#define SYNC_FLAG_IT_SCANNED 0x010
#define SYNC_FLAG_CHILD_SCANNED 0x020

// item flag(none stored)
#define SYNC_ITEM_HAS_EXCLUDED 0x0100
#define SYNC_ITEM_EDITING_NOW 0x0200
#define SYNC_ITEM_MOVE_TARGET 0x0400
#define SYNC_ITEM_WAIT_CLEAR 0x0800
#define SYNC_ITEM_REQUEST_DELET_ONSERVER 0x1000
#define SYNC_ITEM_RESTORE_OVERRIDE 0x2000
#define SYNC_FLAG_MODIFY_SCANNED 0x4000

#define FOLDER_PERMISSIONS_FOR_SYNC   (SFP_LOOKUP_ELEMENT|SFP_READ)

// HTTP SCAN flags
#define SCAN_ONLY_CHANGED 0x01

// metafile item mask
#ifdef USE_META_STORAGE

#define META_MASK_FILENAME 0x00000001
#define META_MASK_LOCAL 0x00000002 // (time & size)
#define META_MASK_LASTUSER 0x00000004
#define META_MASK_LOCALSTATE 0x00000008
//#define META_MASK_LOCALSIZE 0x00000008

#define META_MASK_SERVER 0x00000010 // (time & size)
#define META_MASK_SERVERUSER 0x00000020
#define META_MASK_SERVEROID 0x00000040
#define META_MASK_FILEOID 0x00000080
#define META_MASK_SERVERPATH 0x00000100
#define META_MASK_SERVERPATHINDEX 0x00000200
#define META_MASK_SERVERSTATE 0x00000400
// #define META_MASK_STORAGEOID 0x00000100
// #define META_MASK_SERVERSIZE 0x00000200
//#define META_MASK_SERVERPERMISSION 0x00000100

#define META_MASK_SYNCRESULT 0x00000800
#define META_MASK_CONFLICT 0x00001000
#define META_MASK_TYPE 0x00002000
#define META_MASK_FLAG 0x00004000

#define META_MASK_RENAMED 0x00008000
//#define META_MASK_SERVER_RENAMED 0x00020000

#define META_MASK_NEW_SERVER                  0x00010000 // (time & size)
#define META_MASK_NEW_SERVERUSER          0x00020000
#define META_MASK_NEW_FILEOID                  0x00040000
#define META_MASK_NEW_SERVERPATH          0x00080000
#define META_MASK_NEW_SERVERPATHINDEX  0x00100000
//#define META_MASK_NEW_STORAGEOID          0x04000000
//#define META_MASK_NEW_SERVERSIZE            0x08000000

#define META_MASK_NEW_LOCAL             0x00200000 // (time & size)
//#define META_MASK_NEW_LOCALSIZE             0x20000000

#define META_MASK_CHILD_RESULT                 0x00400000

#define META_MASK_OVERRIDE                 0x00800000

#define META_MASK_ALL       0x00FFFFFF

#define META_MASK_FILE_VALID_MASK (META_MASK_ALL & ~(META_MASK_SERVERPATHINDEX|META_MASK_CHILD_RESULT|META_MASK_NEW_SERVERPATHINDEX))

#define META_MASK_FOLDER_VALID_MASK (META_MASK_ALL & ~META_MASK_FILEOID)

#define META_MASK_ROOT_VALID_MASK (META_MASK_FILENAME|META_MASK_TYPE|META_MASK_LOCAL|META_MASK_LASTUSER|META_MASK_SYNCRESULT|META_MASK_CHILD_RESULT|META_MASK_CONFLICT|META_MASK_SERVEROID|META_MASK_SERVERPATH|META_MASK_SERVERPATHINDEX)

#define META_MASK_OLD_SERVER_SIDE (META_MASK_SERVER|META_MASK_SERVERUSER|META_MASK_SERVERPATH|META_MASK_SERVERPATHINDEX|META_MASK_FILEOID)

#define META_MASK_NEW_SERVER_SIDE (META_MASK_NEW_SERVER|META_MASK_NEW_SERVERUSER|META_MASK_NEW_SERVERPATH|META_MASK_NEW_SERVERPATHINDEX|META_MASK_NEW_FILEOID)

#define META_MASK_NEW_LOCAL_SIDE (META_MASK_NEW_LOCAL)

#define META_MASK_SERVER_SIDE (META_MASK_SERVEROID|META_MASK_SERVERSTATE|META_MASK_OLD_SERVER_SIDE|META_MASK_NEW_SERVER_SIDE)

#define META_MASK_LOCAL_SIDE (META_MASK_LASTUSER|META_MASK_LOCAL|META_MASK_NEW_LOCAL|META_MASK_LOCALSTATE)

// LoadMetafile flag
#define META_CLEAR_STATE    ~(META_MASK_LOCALSTATE|META_MASK_SERVERSTATE)

#endif // USE_META_STORAGE

// StoreFileMonitorSequence data
#define FM_FILE_RENAME 1
#define FM_FOLDER_RENAME 2

// SortFlag
#define ITEM_SORT_ASCENT 0x0100
#define ITEM_SORT_COLUMN 0x001F

// Resolve flag
#define FOLDER_CLOSED 0x01
#define ITEM_SELECTED 0x02
#define DUMMY_FOLDER 0x08
#define HIDDEN_ROOT_FOLDER 0x10
#define CHILD_OF_CONFLICT 0x20

// flags on SetItemSynchronizing()
#define WITH_STORE_METAFILE 1
#define SHELL_ICON_UPDATE_RECURSE 2

// flags for KSyncRootStorage::MoveMetafileAsRename()
// #define WITH_STORE_META 0x01 //메타 데이터를 저장할 지 여부
#define WITH_NOT_UPDATE_RELATIVE_PATH 0x04 // 경로명을 업데이트 할 것인지.

class KSyncPolicy;

typedef struct tagLOCAL_INFO {
  LPTSTR pFilename;
  TCHAR UserOID[MAX_OID];
  __int64 FileSize;
  SYSTEMTIME FileTime;
  SYSTEMTIME AccessTime;
} LOCAL_INFO;

typedef struct tagSERVER_INFO {
  LPTSTR pFilename;
  TCHAR FileOID[MAX_OID];
  TCHAR StorageOID[MAX_STGOID];
  LPTSTR pFullPath;
  LPTSTR pFullPathIndex;
  TCHAR UserOID[MAX_OID];
  __int64 FileSize;
  SYSTEMTIME FileTime;
  int Permissions;
} SERVER_INFO;


#define METAFILE_PATH_CHANGED 0x01
#define METAFILE_DELETE 0x02
#define METAFILE_CREATE_IF_NOT_EXIST 0x04
#define METAFILE_CLEAR_MODIFIED 0x08

#include <vector>
typedef std::vector<LPVOID> KVoidPtrArray;
typedef std::vector<LPVOID>::iterator KVoidPtrIterator;

// GetSyncResultRName flag
#define AS_LOCAL 1
#define AS_NAME_ONLY 2
#define WITH_DUMMY_PATH 4
#define AS_FULLPATH 8
#define AS_NO_OVERRIDE 0x10

#define WITH_RECURSE 1
#define CHANGE_RESULT 2
#define ENABLE_ONLY_SELECTED 4
#define WITH_ITSELF 0x010
#define WITH_CHILD 0x020
#define EXCLUDE_MOVED 0x040
#define WITH_ALL_CHILD 0x020

// InitializeResolveState flag
#define FORCE_OPEN_FOLDER 0x01
#define MAKE_CLONE_FOLDER 0x02
#define FORCE_CHECK_ENABLE 0x04

#define WITH_CHILD_DOCUMENT 1
#define WITH_CHILD_FOLDER 2
#define WITH_ALL 0x0F

/**
* @class KSyncItem
* @brief 동기화 대상 항목의 문서 항목 클래스(base class)
*/
class KSyncItem
{
public:
  KSyncItem(KSyncItem *p);
  KSyncItem(KSyncItem *p, LPCTSTR filename);
  KSyncItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename);
  KSyncItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid);
  virtual ~KSyncItem(void);

  /**
  * @brief 스트링으로 표현된 권한 정보를 mask value로 변환함.
  * @param permissions : 스트링으로 표현된 권한 정보('l,r,w,d' 등)
  * @return int
  */
  static int String2Permission(LPCTSTR permissions);

  /**
  * @brief 입력된 권한값이 동기화 루트 폴더의 권한에 부합하는지 검사함.
  * @param permission : 해당 개체의 권한값.
  * @return BOOL
  */
  static BOOL CheckPermissionRootFolder(int permission);
  /**
  * @brief 입력된 권한값이 동기화 하위 폴더의 권한에 부합하는지 검사함.
  * @return BOOL
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionSubFolder(int permission);

  /**
  * @brief 입력된 권한값이 새 폴더를 서버에 업로드할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFolderUpload(int parent_permission, int permission);

  /**
  * @brief 입력된 권한값이 새 폴더를 서버에서 다운로드할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFolderDownload(int parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 폴더를 삭제할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFolderDelete(int parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 폴더를 이동할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param src_parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param dest_parent_permission : 이동할 폴더의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFolderMove(int src_parent_permission, int dest_parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 폴더의 이름을 바꿀 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFolderRename(int parent_permission, int permission);

  /**
  * @brief 입력된 권한값이 파일을 업로드할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFileUpload(int parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 파일을 추가할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFileInsert(int parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 파일을 다운로드할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFileDownload(int parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 파일을 삭제할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFileDelete(int parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 파일을 이동할 권한에 부합하는지 검사함.
  * @return BOOL
  * @param src_parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param dest_parent_permission : 이동할 폴더의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFileMove(int src_parent_permission, int dest_parent_permission, int permission);
  /**
  * @brief 입력된 권한값이 파일의 이름을 바꿀 권한에 부합하는지 검사함.
  * @return BOOL
  * @param parent_permission : 해당 개체의 부모 개체의 권한값.
  * @param permission : 해당 개체의 권한값.
  */
  static BOOL CheckPermissionFileRename(int parent_permission, int permission);

  /**
  * @brief 서버측 상대 경로 및 파일명을 새 메모리 버퍼에 할당하여 복사한 후 리턴함.
  * @param rootItem : 동기화 루트 폴더 개체
  * @param serverFullPath : 대상 개체의 서버측 전체 경로
  * @param fileName : 파일명
  * @return LPTSTR : 서버측 상대 경로명
  */
  static LPTSTR allocServerSideRelativePath(KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName);

  /**
  * @brief 서버측 상대 경로 및 파일명을 새 메모리 버퍼에 할당하여 복사한 후 리턴함.
  * @param serverRootPath : 동기화 루트 폴더 개체의 서버측 전체 경로
  * @param serverFullPath : 대상 개체의 서버측 전체 경로
  * @param fileName : 파일명
  * @return LPTSTR : 서버측 상대 경로명
  */
  static LPTSTR AllocServerSideRelativePath(LPCTSTR serverRootPath, LPCTSTR serverFullPath, LPCTSTR fileName);

  /**
  * @brief 두 경로명을 비교하여 경로가 다른지 확인한다.
  * @param oldpathname : 이전 경로명
  * @param newpathname : 새 경로명
  * @return BOOL : 경로가 다르면 TRUE를 리턴한다.
  */
  static BOOL isPathChanged(LPCTSTR oldpathname, LPCTSTR newpathname);

  /**
  * @brief 두 경로명을 비교하여 파일명이 다른지 확인한다.
  * @param oldpathname : 이전 경로명
  * @param newpathname : 새 경로명
  * @return BOOL : 파일명이 다르면 TRUE를 리턴한다.
  */
  static BOOL isNameChanged(LPCTSTR oldpathname, LPCTSTR newpathname);

  /*
  void getServerSideFullPathName(LPTSTR fullName, int size, KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName);
  LPTSTR allocServerSidePathName(BOOL bFull, KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName);
  */

  /**
  * @brief 충돌 처리 동작에서 대상 항목의 충돌 처리를 할 것인지 아닌지 여부를 나타냄.
  * @return BOOL
  */
  inline BOOL GetEnable() { return mEnable; } 
  /**
  * @brief 충돌 처리 동작에서 대상 항목의 충돌 처리를 할 것인지 아닌지 여부를 설정한다.
  * @return void
  * @param e : TRUE이면 대상 항목의 충돌 처리를 함.
  */
  inline void SetEnable(BOOL e) { mEnable = e; } 
  /**
  * @brief 충돌 처리 동작에서 대상 항목의 충돌 처리를 할 것인지 아닌지 여부를 반대로 함.
  * @return void
  */
  inline void ToggleEnable() { mEnable = !mEnable; } 
  /**
  * @brief 충돌 처리 동작에서 대상 항목의 충돌 처리를 할 것인지 아닌지 여부를 설정함. 하위 항목도 모두 설정함
  * @return void
  * @param e : TRUE이면 대상 항목의 충돌 처리를 함.
  */
  virtual inline void SetEnableRecurse(BOOL e) { mEnable = e; }
  /**
  * @brief 충돌 처리 동작에서 대상 항목 또는 하위 항목이 충돌 처리를 하도록 설정되어 있으면 TRUE를 리턴함
  * @return void
  */
  virtual BOOL GetEnableChild() {return mEnable; }

  /**
  * @brief 대상항목을 동기화 대상에서 제외하거나 제외 플래그를 제거함.
  * @return void
  */
  inline void SetExcludedSync(BOOL set) { if(set) mFlag |= SYNC_ITEM_EXCLUDED; else mFlag &= ~SYNC_ITEM_EXCLUDED; }
  /**
  * @brief 대상항목이 동기화 대상에서 제외되어 있으면 TRUE를 리턴함.
  * @return BOOL
  */
  inline BOOL IsExcludedSync() { return (mFlag & SYNC_ITEM_EXCLUDED); }

  virtual BOOL IsExcludedSyncRecur() { return (mFlag & SYNC_ITEM_EXCLUDED); }

  /*
  * @brief 서버측 하위 항목 Rescan 필요함 플래그를 셋트하거나 해제한다.
  */
  inline void SetServerNeedRescan(BOOL set) { if(set) mFlag |= SYNC_ITEM_NEED_SERVER_RESCAN; else mFlag &= ~SYNC_ITEM_NEED_SERVER_RESCAN; }

  /*
  * @brief 서버측 하위 항목 Rescan 필요함 플래그가 셋트되었는지 확인한다.
  */
  inline BOOL IsServerNeedRescan() { return (mFlag  & SYNC_ITEM_NEED_SERVER_RESCAN); }

  /**
  * @brief 대상항목의 flag에 속성값을 더함.
  * @return void
  */
  inline void AddSyncFlag(int f) { mFlag |= f; }
  /**
  * @brief 대상항목의 flag에 속성값을 제거함.
  * @return void
  */
  inline void RemoveSyncFlag(int f) { mFlag &= ~f; }
  /**
  * @brief 대상항목의 flag에 특정 속성값이 있는지 검사함.
  * @return BOOL
  */
  inline BOOL HasSyncFlag(int f) { return !!(mFlag & f); }

  /**
  * @brief 자신 또는 부 폴더가 삭제 요청이 처리되었는지 확인함.
  */
  virtual BOOL IsParentAlreadyRegistDeleteOnServer();

  /**
  * @brief 현재의 동기화 작업이 부분 동기화 작업이면 TRUE를 리턴함.
  * @return BOOL
  * @param pathname : 대상 경로 이름
  * @param isFolder : 대상이 폴더인지 파일인지.
  * @details 대상 폴더가 부분 동기화 작업 영역 이외의 폴더 영역이면 FALSE를 리턴함.
  */
  virtual inline BOOL IsPartialSyncOn(LPCTSTR pathname, BOOL isFolder) { return TRUE; }

  /**
  * @brief 동기화 작업중 발생된 오류 메시지를 리턴함(서버측에서 전달받은 오류 메시지)
  * @return LPCTSTR : 서버측 오류 메시지.
  */
  inline LPCTSTR GetServerConflictMessage() { return mpServerConflictMessage; }

  /**
  * @brief 동기화 작업 결과값을 설정함
  * @param r : 결과값
  * @return void
  */
  inline void SetResult(int r) { mResult = r; }
  /**
  * @brief 동기화 작업 결과값을 받아옴
  * @return int : 동기화 작업 결과값
  */
  inline int GetResult() { return mResult; }

  /**
  * @brief 서버측에 존재하는지 여부를 가져옴
  * @return BOOL : 존재하면 TRUE를 리턴함
  */
  inline BOOL IsServerNotExist() { return (mServerOID[0] == '\0'); }

  /**
  * @brief 로컬측에 존재하는지 여부를 가져옴(파일 타임이 유효하면 존재하는 것으로)
  * @return BOOL : 존재하면 TRUE를 리턴함
  */
  inline BOOL IsLocalNotExist() { return ((mLocal.FileTime.wYear == 0) && (mLocalNew.FileTime.wYear == 0)); }

  /**
  * @brief folderFullPathIndex가 바뀌었는지를 리턴하는 가상함수
  * @return BOOL : 바뀌었으면 TRUE를 리턴함
  */
  virtual inline BOOL IsFolderFullPathChanged() { return FALSE; }

  /**
  * @brief 로컬 상태값을 설정한다.
  * @param state : 상태값
  * @return void
  */
  inline void setLocalState(int state) { mLocalState = state; }
  /**
  * @brief 로컬 상태값에 지정한 값을 더한다.
  * @param state : 상태값
  * @return void
  */
  inline void addLocalState(int state) { mLocalState |= state; }
  /**
  * @brief 서버 상태값을 설정한다.
  * @param state : 더할 상태값
  * @return void
  */
  inline void setServerState(int state) { mServerState = state; }
  /**
  * @brief 서버 상태값에 지정한 값을 더한다.
  * @param state : 더할 상태값
  * @return void
  */
  inline void addServerState(int state) { mServerState |= state; }
  /**
  * @brief 로컬 상태값에 지정한 값을 마스크하고 더한다.
  * @param mask : not and value
  * @param add : or value
  * @return void
  */
  inline void maskLocalState(int mask, int add) { mLocalState = (mLocalState & ~mask) | add; }
  /**
  * @brief 서버 상태값에 지정한 값을 마스크하고 더한다.
  * @param mask : not and value
  * @param add : or value
  * @return void
  */
  inline void maskServerState(int mask, int add) { mServerState = (mServerState & ~mask) | add; }

  /**
  * @brief 로컬 상태값을 리턴한다.
  * @return int : 상태값
  */
  inline int GetLocalState() { return mLocalState; }
  /**
  * @brief 서버 상태값을 리턴한다.
  * @return int : 상태값
  */
  inline int GetServerState() { return mServerState; }

  /**
  * @brief 로컬측 파일 크기를 설정한다.
  * @param size : Byte 단위 파일 크기
  * @return void
  */
  inline void SetLocalFileSize(__int64 size) { mLocal.FileSize = size; }
  /**
  * @brief 서버측 파일 크기를 설정한다.
  * @param size : Byte 단위 파일 크기
  * @return void
  */
  inline void SetServerFileSize(__int64 size) { mServer.FileSize = size; }

  /**
  * @brief 동기화 작업 단계값을 리턴한다.
  * @return int : 동기화 작업 단계값
  */
  inline int GetPhase() { return mPhase; }
  /**
  * @brief 동기화 작업 단계값을 설정한다.
  * @param p : 동기화 작업 단계값
  * @details : 단계값이 PHASE_END_OF_JOB인 경우에는 바꾸지 않는다.
  */
  virtual inline void SetPhase(int p) { if (mPhase < PHASE_END_OF_JOB) mPhase = p; }
  /**
  * @brief 동기화 작업 단계값을 무조건 설정한다.
  * @param p : 동기화 작업 단계값
  */
  virtual inline void SetPhaseForce(int p) { mPhase = p; }

  /**
  * @brief KSyncItem object의 내용을 복사하고 개체 reference를 리턴함
  * @return KSyncItem &
  * @param item : 소스 개체
  */
  KSyncItem &operator=(KSyncItem &item);

  /**
  * @brief 자신과 동일한 object를 생성하고 포인터를 리턴함.
  * @return KSyncItem* : 새로 생성된 개체
  */
  virtual KSyncItem* DuplicateItem();

  /**
  * @brief 멤버 변수값을 모두 초기화한다.
  * @return void
  * @param parent : parent object
  */
  void init(KSyncItem *parent);

  /**
  * @return void
  * @brief 이 개체가 할당한 메모리 해제.
  * @details A. 다음의 메모리 포인터를 해제한다.
  *     - mpRelativePath
  *     - mpMetafilename
  *     - mpLocalRenamedAs
  *     - mpServerRenamedAs
  *     - mpServerTempRenamed
  *     - mpLocalTempRenamed
  *     - mpServerConflictMessage
  *     .
  * @details B. FreeServerInfo() 메소드로 mServer, mServerNew 두개를 해제한다.
  */
  void FreeAllocated();

  /**
  * @brief 이 항목이 지정한 동기화 작업 단계에서 처리할 작업이 있는지 확인한다.
  * @return BOOL : 작업할 내용이 있으면 TRUE를 리턴한다.
  * @param phase : 작업 단계
  */
  virtual BOOL IsNeedSyncPhase(int phase);

  /**
  * @brief 이 항목의 파일 크기를 리턴한다.
  * @return __int64 : 파일 크기
  * @param conflictResult : TRUE이면 충돌 결과 파일 크기
  */
  __int64 GetSyncFileSize(BOOL conflictResult);

  /**
  * @brief 'yyyy-mm-dd hh:mm:ss:ms'형식의 문자열에서 mLocal.FileTime으로 읽어들임.
  * @return bool
  * @param str : 입력 스트링
  */
  BOOL ParseDateString(char *str);

  /**
  * @brief 히스토리 파일로 저장된 문자열에서 개체의 정보(name=, user=, result 등)를 읽어들임.
  * @return bool
  * @param str : 입력 스트링
  */
  BOOL ParseHistoryString(char *str);

  /**
  * @brief 개체가 속한 폴더의 상대 경로를 리턴함. 루트 폴더에서는 NULL
  * @return LPCTSTR
  */
  LPCTSTR GetPath();

  /**
  * @brief 개체가 속한 폴더의 상대 경로를 리턴함. 루트 폴더에서는 '\\'
  * @return LPCTSTR
  */
  LPCTSTR GetPathString();

  /**
  * @brief 개체의 로컬측 파일명을 리턴함.
  * @return LPCTSTR
  */
  LPCTSTR GetFilename();

  /**
  * @brief 개체의 로컬측 마지막 수정시간 SYSTEMTIME의 포인터를 리턴함.
  * @return LPCTSTR
  */
  SYSTEMTIME *GetLocalFileTime();

  /**
  * @brief 개체의 로컬측 파일 크기를 리턴함.
  * @return LPCTSTR
  */
  inline __int64 GetLocalFileSize() { return ((mLocalNew.FileSize >= 0) ? mLocalNew.FileSize : mLocal.FileSize); }
  /**
  * @brief 개체의 서버측 파일명을 리턴함.
  * @return LPCTSTR
  */
  inline __int64 GetServerFileSize() { return ((mServerNew.FileSize >= 0) ? mServerNew.FileSize : mServer.FileSize); }

  /**
  * @brief 동기화 개체의 종류를 리턴하는 가상함수.
  * @return int : 동기화 개체의 종류(0=file, 1=folder, 2=root folder)
  */
  virtual int IsFolder() { return 0; }

  virtual BOOL IsSystemRoot() { return ((mLocal.pFilename != NULL) && (mLocal.pFilename[0] == '\\') && (mLocal.pFilename[1] == '\0')); }

  /**
  * @brief 개체의 상대 경로 이름(mpRelativePath)을 지정한 pathname으로 변경함.
  * @return void
  * @param pathname : 입력된 상대 결로 및 이름 스트링
  * mpRelativePath는 메타 DB의 키값으로 사용됨.
  */
  virtual void SetRelativePathName(LPCTSTR pathname);

  virtual void ClearRelativePathName(int flag);

  /**
  * @brief 개체의 부모 개체의 경로와 개체의 이름을 더해서 mpRelativePath 값을 업데이트 함.
  * @return void
  */
  virtual void UpdateRelativePath();

  /**
  * @brief 개체의 경로와 이름을 입력된 경로 이름으로 바꿈, 개체 트리에서의 위치도 바꿈.
  * @return void
  * @param rootFolder : 동기화 루트 폴더 개체
  * @param relativePathName : 입력된 상대 경로 및 이름 스트링
  */
  void ChangeRelativePathName(KSyncRootFolderItem* rootFolder, LPCTSTR relativePathName, BOOL moveInTree);

  /**
  * @brief 개체의 서버측 FileOID를 리턴함.
  * @return LPCTSTR
  */
  LPCTSTR GetServerFileOID();
  /**
  * @brief 개체의 서버측 StorageOID를 리턴함.
  * @return LPCTSTR
  */
  LPCTSTR GetServerStorageOID();

  /**
  * @brief 개체의 서버측 상대경로를 리턴함.
  * @return LPCTSTR
  * 폴더의 경우에는 해당 폴더의 이름을 포함하고 파일의 경우에는 파일이 위치한 폴더명.
  */
  virtual LPCTSTR GetServerRelativePath();

  /**
  * @brief 개체의 서버측 FullPathIndex를 리턴함.
  * @return LPCTSTR
  */
  LPCTSTR GetServerFullPathIndex();

  /**
  * @brief 개체의 서버측 상대 경로와 파일명을 새로 할당된 메모리에 복사해서 리턴함.
  * @return LPCTSTR
  */
  LPTSTR AllocServerRelativePathName();


  /**
  * @brief 개체가 포함된 폴더의 경로를 지정한 path로 바꿈.
  * @return void
  * @param baseFolder : 해당 개체가 포함된 폴더의 상대경로.
  * @param rootFolder : 동기화 루트 폴더의 경로.
  * @param depth : 동기화 폴더 트리에서의 depth
  */
  virtual void SetLocalPath(LPCTSTR baseFolder, LPCTSTR rootFolder, int depth);

  /**
  * @brief 동기화 충돌 처리창에서 폴더의 위치가 이동된 항목들에서 이동된 경로의 이름을 메모리 할당하여 리턴함.
  * @return LPTSTR
  * @param flag : option flag(AS_LOCAL : 로컬측 이름, AS_NAME_ONLY : 경로는 제외하고 이름만)
  */
  virtual LPTSTR AllocSyncMoveResultName(int flag) { return NULL; }


  /**
  * @brief 로컬측 파일 정보를 mLocal에 업데이트 함.
  * @return BOOL
  * @param user : 로컬측 최종 수정자 OID를 이것으로 업데이트 함.
  */
  BOOL RefreshLocalFileInfo(LPCTSTR user);

  /**
  * @brief 현재의 로컬측 파일 정보를 mLocal에 업데이트 함.
  * @return BOOL
  * @param fullname : 대상 로컬측 파일 전체 경로 이름.
  * @param user : 로컬측 최종 수정자 OID를 이것으로 업데이트 함.
  */
  BOOL RefreshLocalFileInfo(LPCTSTR fullname, LPCTSTR user);

  /**
  * @brief 현재의 로컬측 파일 정보를 mLocalNew에 업데이트 함.
  * @return BOOL
  * @param user : 로컬측 최종 수정자 OID를 이것으로 업데이트 함.
  * @param rootItem : 동기화 루트 폴더 항목
  */
  BOOL RefreshLocalNewFileInfo(LPCTSTR user, KSyncItem* rootItem = NULL);

  void RefreshLocalNewState(KSyncRootFolderItem* root);

  /**
  * @brief 로컬측 파일이 이전 동기화 이후 수정된 상태인지 검사함(mLocalNew와 mLocal 정보를 비교해서)
  * @return BOOL
  */
  BOOL IsLocalModified();
  /**
  * @brief 로컬측 파일이 동기화 비교 이후 다시 수정된 상태인지 검사함.
  * @return BOOL
  */
  BOOL IsLocalModifiedAgain(BOOL update); // check modified after compare

  /**
  * @brief 서버측 파일이 이전 동기화 이후 수정된 상태인지 검사함.
  * @return BOOL
  */
  BOOL IsServerModified();

  /**
  * @brief mLocalNew 정보를 새로 업데이트한 후, 로컬측 파일이 이전 동기화 이후 수정된 상태인지 검사함(mLocalNew와 mLocal 정보를 비교해서)
  * @return BOOL
  */
  BOOL IsModifiedNow();

  /**
  * @brief 대상 항목이 업데이트가 필요한지 확인한다.(수정되거나 이동되거나 새로 추가되었거나)
  * @return BOOL
  */
  BOOL IsNeedUpdate(BOOL onLocal);

  /**
  * @brief 현재의 서버측 파일 정보를 mServerNew에 업데이트 함.
  * @return void
  */
  void UpdateServerNewInfo();

  /**
  * @brief mLocalNew 정보를 mLocal에 머지한 후 mLocalNew를 클리어 함.
  * @return void
  */
  void UpdateLocalNewInfo(LPCTSTR baseFolder);

  virtual int GetCountForMoveInOutOnServer(LPCTSTR partialPath, int path_len);

  /**
  * @brief 개체의 폴더 위치가 바뀌었을때 불리는 함수. 기존의 메타파일을 사용할 때에만 사용됨
  * @return void
  */
  virtual void OnPathChanged() { ; }

  /**
  * @brief 개체의 폴더 위치가 바뀌었을때 불리는 함수.
  * @return void
  * @details 1. 메타파일의 이동(DB에서는 동기화 항목의 키 변경)
  * @n 2. 로컬측 이동 및 이름 변경 정보 업데이트.
  * @n 3. UpdateMetaFolderNameMapping() : 기존의 메타파일을 사용할 때에만 사용됨
  * @n 4. UpdatePathChangeOnMetafile() : 서버측 경로 및 fullPathIndex값 업데이트 & 메타정보를 스토리지에 저장함.
  */
  virtual void OnPathChanged(LPCTSTR folder, LPCTSTR rootFolder, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow);

  /**
  * @brief 개체의 폴더 위치가 바뀌었을때 불리는 함수.
  * @return void
  * @details 1. 서버측 경로 및 fullPathIndex값 업데이트
  * @n 2. 메타정보를 스토리지에 저장함.
  */
  void UpdatePathChangeOnMetafile(LPCTSTR baseFolder, int update_mask, int flag);


  void RefreshPathChanged(LPCTSTR rootFolder);

  /**
  * @brief 서버OID, fileOID, storageOID 값을 업데이트 함.
  * @return void
  */
  void UpdateServerOID(LPCTSTR serverOID, LPCTSTR fileOID, LPCTSTR storageOID);
  /**
  * @brief mServer에 permission, laseModifierID, fileTime을 업데이트 함.
  * @return void
  */
  void UpdateServerFileInfo(int perm, LPCTSTR lastModifierOID, SYSTEMTIME &fileTime);

  /**
  * @brief mServerNew에 permission, laseModifierID, serverFullPath, fullPathIndex를 업데이트 함.
  * @return void
  */
  void UpdateServerNewFileInfo(int perm, LPCTSTR lastModifierOID, LPCTSTR serverFullPath, LPTSTR fullPathIndex);

  /**
  * @brief mServerNew에 각각의 값을 업데이트 함.(기존의 값이 있으면 해제하고 다시 할당함)
  * @return BOOL
  * @param fileSize : 파일의 크기
  * @param fileTime : 파일의 마지막 수정 날짜
  * @param permissions : 권한값 스트링 표현
  * @param lastModifierOID : 마지막 수정자 OID
  * @param fileOID : 파일 OID
  * @param storageOID : 스토리지 OID
  * @param serverFullPath : 서버측 fullPath
  * @param serverFullPathIndex : 서버측 fullPathIndex
  * @param name : 이름
  */
  BOOL UpdateServerNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, LPCTSTR permissions, 
    LPCTSTR lastModifierOID, LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR serverFullPath, LPCTSTR serverFullPathIndex, LPCTSTR name);

  /**
  * @brief mServerNew에 각각의 값을 업데이트 함.(기존의 값이 있으면 해제하고 다시 할당함)
  * @return BOOL
  * @param fileSize : 파일의 크기
  * @param fileTime : 파일의 마지막 수정 날짜
  * @param permissions : 권한값
  * @param lastModifierOID : 마지막 수정자 OID
  * @param fileOID : 파일 OID
  * @param storageOID : 스토리지 OID
  * @param serverFullPath : 서버측 fullPath
  * @param serverFullPathIndex : 서버측 fullPathIndex
  * @param name : 이름
  */
  BOOL UpdateServerNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, int permissions, 
    LPCTSTR lastModifierOID, LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR serverFullPath, LPCTSTR serverFullPathIndex, LPCTSTR name);

  /**
  * @brief mLocalNew에 각각의 값을 업데이트 함.(기존의 값이 있으면 해제하고 다시 할당함)
  * @return BOOL
  * @param fileSize : 파일의 크기
  * @param fileTime : 파일의 마지막 수정 날짜
  * @param accessTime : 파일의 마지막 접근 날짜
  * @param user : 파일의 마지막 수정자
  */
  BOOL UpdateLocalNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, SYSTEMTIME &accessTime, LPCTSTR user);

  /**
  * @brief 대상 항목의 로컬 정보를 이 개체에 머지함.
  * @param nItem : 대상 항목
  * @details A. 로컬측 상태(mLocalState)를 머지한다.
  * @n B. 새 로컬 정보(mLocalNew)를 복사한다.
  */
  void MergeLocalInformation(KSyncItem *nItem);

  /**
  * @brief 로컬측 이름 변경 정보를 설정한다.
  * @param newRelativeName : 이름 변경 정보
  * @details A. 로컬 이름 변경 정보가 비어있으면 복사해 넣고 TRUE를 리턴한다.
  * @n B. 아니면 로그 메시지를 저장하고 FALSE를 리턴한다.
  */
  BOOL SetLocalRenamedAs(LPCTSTR newRelativeName);

  /**
  * @brief 이동된 부모 폴더의 OID를 셋트한다.
  * @param newParentServerOID : 새 부모 폴더의 OID
  * @details A. 부모 폴더의 OID 정보가 비어있으면 복사해 넣고 TRUE를 리턴한다.
  * @n B. 아니면 로그 메시지를 저장하고 FALSE를 리턴한다.
  */
  BOOL SetNewParentOID(LPTSTR newParentServerOID);

#if 0
  /**
  * @brief path+name에서 상대 경로명을 버퍼에 저장한다.
  * @param relativeName : 상대 경로명을 저장할 버퍼
  * @param path : 폴더 경로명
  * @param name : 파일명
  * @details A. path+name으로 전체 경로명을 만든다.
  * @n B. 루트 폴더의 경로명을 구하여 이 부분을 제외하고 남은 부분을 relativeName에 복사한다.
  */
  BOOL GetRelativePathName(LPTSTR OUT relativeName, LPCTSTR IN path, LPCTSTR IN name);

  /**
  * @brief fullname에서 상대 경로명을 버퍼에 저장한다.
  * @param relativeName : 상대 경로명을 저장할 버퍼
  * @param fullname : 전체 경로명
  * @details A. 루트 폴더의 경로명을 구하여 이 부분을 제외하고 남은 부분을 relativeName에 복사한다.
  */
  BOOL GetRelativePathName(LPTSTR OUT relativeName, LPCTSTR IN fullname);
#endif

  /**
  * @brief 새로 할당한 메모리에 전체 경로명을 복사하여 리턴한다.
  * @return LPTSTR : 전체 경로명
  * @details A. 상대 경로명을 만들고,
  * @details B. AllocAbsolutePathName()으로 절대 경로명을 만들어 리턴한다.
  */
  LPTSTR AllocFullPathName();
  /**
  * @brief 새로 할당한 메모리에 전체 경로명을 복사하여 리턴한다.
  * @return LPTSTR : 전체 경로명
  * @details A. 부모 개체의 경로명을 받아서 이 개체의 이름을 붙여서 경로명을 만들어 리턴한다.
  */
  LPTSTR AllocTreePathName();

  /**
  * @brief 새로 할당한 메모리에 상대 경로명을 복사하여 리턴한다.
  * @return LPTSTR : 상대 경로명
  * @details A. 루트 폴더가 아니면 
  *   - mpRelativePath가 NULL이 아니면 mpRelativePath를 복사한다.
  *   - 아니고, 부모가 NULL이 아니면 부모 개체의 AllocActualPathName() 메소드로 폴더 경로를 받아서 여기에 파일명을 붙인다.
  *   - 아니면, 파일명만 복사한다.
  *   .
  * @n B. 루트 폴더는 NULL을 리턴한다.
  */
  LPTSTR AllocRelativePathName();

  /**
  * @brief 대상 전체 경로명에서 상대 경로명을 만들어 리턴한다.
  * @param fullPathName : 전체 경로명
  * @return LPTSTR : 상대 경로명
  * @details A. 루트 폴더의 경로를 받아온다.
  * @n B. 전체 경로명에서 루트 폴더 경로명 부분을 제외한 나머지 부분만 복사하여 리턴한다.
  */
  LPTSTR AllocRelativePathName(LPCTSTR fullPathName);

  /**
  * @brief 대상 경로와 파일명에서 상대 경로명을 만들어 리턴한다.
  * @param fullPath : 전체 경로
  * @param fileName : 파일명
  * @return LPTSTR : 상대 경로명
  * @details A. 루트 폴더의 경로를 받아온다.
  * @n B. 경로명에서 루트 폴더 경로명 부분을 제외한 나머지 부분만 복사하고 파일명을 붙여 리턴한다.
  */
  LPTSTR AllocRelativePathName(LPCTSTR fullPath, LPCTSTR fileName);

  /**
  * @brief 전체 경로명을 만들어 리턴한다.
  * @return LPTSTR : 절대 경로명
  * @details A. AllocRelativePathName() 메소드로 상대 경로명을 받아온다.
  * @n B. AllocAbsolutePathName() 메소드로 절대 경로를 만들어 리턴한다.
  */
  LPTSTR AllocAbsolutePathName();

  /**
  * @brief 전체 경로명을 만들어 리턴한다.
  * @param relativePathName : 상대 경로명
  * @return LPTSTR : 절대 경로명
  * @details A. 루트 폴더의 경로를 받아온다.
  * @n B. 루트 폴더의 경로에 상대 경로명(relativePathName)을 더하여 리턴한다.
  */
  LPTSTR AllocAbsolutePathName(LPCTSTR relativePathName);

  /**
  * @brief 전체 경로명을 만들어 리턴한다.
  * deprecated : AllocMakeFullPathName
  * @param rootFolder : 루트 폴더 경로
  * @param relativePathName : 상대 경로명
  * @return LPTSTR : 절대 경로명
  * @details A. 루트 폴더의 경로에 상대 경로명(relativePathName)을 더하여 리턴한다.
  */
  // LPTSTR AllocAbsolutePathName(LPCTSTR rootFolder, LPCTSTR relativePathName);

  /**
  * @brief 로컬측 또는 서버측 실제 경로명을 대상 버퍼에 복사하는 가상함수. 버퍼가 NULL이면 복사하지 않고 길이값만 계산한다.
  * @param fullname: 경로명을 저장할 버퍼.
  * @param size : 저장할 버퍼의 크기
  * @param abs : TRUE이면 절대 경로로, FALSE이면 상대 경로로 저장한다.
  * @return int : 저장된 경로명의 깅이
  * @details A. 부모 개체가 NULL이 아니면 부모 개체의 경로명을 받아온다.
  * @n B. 로컬측 이름 변경 정보가 있으면, 
  *     - abs가 TRUE이면 (루트 폴더 경로 + mpLocalRenamedAs)를 버퍼에 복사한다.
  *     - 아니면 (mpLocalRenamedAs)를 버퍼에 복사한다.
  * @n C. 아니면 파일명을 버퍼에 더한다.
  */
  virtual int GetActualPathName(LPTSTR fullname, int size, BOOL abs);

  /**
  * @brief 로컬측 또는 서버측 실제 경로명을 새로 할당한 버퍼에 복사하여 리턴한다.
  * @param abs : TRUE이면 절대 경로로, FALSE이면 상대 경로로 저장한다.
  * @return LPTSTR : 절대 경로명
  * @details A. GetActualPathName(NULL, 0, abs) 메소드를 호출하여 길이를 먼저 구한다.
  * @n B. 버퍼용 메모리를 할당하여 다시 GetActualPathName() 를 호출하고, 버퍼를 리턴한다.
  */
  LPTSTR AllocActualPathName(BOOL abs);

  /**
  * @brief 상대 경로명으로 절대 경로를 만들어 대상 버퍼에 저장한다.
  * @param fullname : 절대 경로명을 저장할 버퍼.
  * @param relativeName : 상대 경로명
  * @details A. 루트 폴더의 경로를 받아온다.
  * @n B. 루트 폴더의 경로와 상대 경로명(relativeName)을 합쳐서 버퍼에 복사한다.
  */
  BOOL GetAbsolutePathName(LPTSTR OUT fullname, LPCTSTR IN relativeName);

  /**
  * @brief 상대 경로명에 대한 이동 및 이름 변경 정보를 받아온다.
  * @param oldRelativeName : 원래의 상대 경로명.
  * @return LPTSTR : Directory Monitor에서 만든 정보를 검색하여 만든 이동 및 이름 변경된 상대 경로명.
  * @details A. 루트 폴더 항목을 받아온다.
  * @n B. KSyncRootFolderItem::AllocMovedRenamedAs()를 호출하고 그 결과를 리턴한다.
  */
  virtual LPTSTR AllocMovedRenamedAs(LPCTSTR oldRelativeName);

  // BOOL GetLocalRenamedAsByOld(LPCTSTR oldPath, LPCTSTR oldName, LPTSTR renamedAs);

  /**
  * @brief 로컬측의 이동 및 이름 변경 정보를 업데이트 한다.
  * @param rootItem : 루트 폴더 항목.
  * @return BOOL : 이동 및 이름 변경 정보가 유효하면 TRUE를 리턴한다.
  * @details A. 현재의 이동 및 이름 변경 정보(mpLocalRenamedAs)를 해제한다.
  * @n B. 로컬 상태가 현재 존재함이면 상대 경로명을 준비하고, AllocMovedRenamedAs() 메소드로 이동 및 이름 변경 정보를 받아온다.
  */
  BOOL CheckLocalRenamedAsByOld(KSyncItem* rootItem = NULL);

  /**
  * @brief 로컬측의 이동 및 이름 변경 정보를 업데이트 한다.
  * @param newPath : 새로운 경로.
  * @param filename : 새로운 파일명.
  * @return BOOL : 이동 및 이름 변경 정보가 유효하면 TRUE를 리턴한다.
  * @details A. 현재의 상대 경로명을 AllocRelativePathName() 메소드로 받아온다.
  * @n B. 입력된 newPath, filename에서 상대 경로명을 준비하여 newPathName에 받아놓는다.
  * @n C. 현재의 이동 및 이름 변경 정보(mpLocalRenamedAs)를 해제한다.
  * @n D. 위에서 받은 두개의 상대 경로명을 비교하여 다르면,
  *     - 상위 폴더 부분이 바뀌었고 자신의 위치나 이름을 바뀌지 않았다면 바뀌지 않은 것으로 한다.
  *     - 바뀌었으면 newPathName을 mpLocalRenamedAs로 설정한다.
  */
  BOOL CheckLocalRenamedAs(LPCTSTR newPath, LPCTSTR filename);

  /**
  * @brief 자신은 변동이 없고 상위 폴더가 변경되었으면 로컬 이름변경 정보를 클리어한다.
  */
  BOOL CheckLocalRenamedItself();

  /**
  * @brief 서버측의 이동 및 이름 변경 정보를 업데이트 한다.
  * @param newPath : 새로운 경로.
  * @param filename : 새로운 파일명.
  * @return BOOL : 서버측의 이동 및 이름 변경 정보가 유효하면 TRUE를 리턴한다.
  * @details A. 현재의 상대 경로명을 AllocRelativePathName() 메소드로 받아온다.
  * @n B. 입력된 newPath, filename에서 상대 경로명을 준비하여 newPathName에 받아놓는다.
  * @n C. 현재의 이동 및 이름 변경 정보(mpServerRenamedAs)를 해제한다.
  * @n D. 위에서 받은 두개의 상대 경로명을 비교하여 다르면,
  *     - 상위 폴더 부분이 바뀌었고 자신의 위치나 이름을 바뀌지 않았다면 바뀌지 않은 것으로 한다.
  *     - 바뀌었으면 newPathName을 mpServerRenamedAs로 설정한다.
  */
  BOOL CheckServerRenamedAs(LPCTSTR newPath, LPCTSTR filename);

  virtual int UpdateServerRenamedAs(LPCTSTR orgRelativePath, LPCTSTR changedRelativePath);

  /**
  * @brief 로컬측 이름 변경을 확인한다.
  * @return BOOL : 파일명이 변경되었으면 TRUE를 리턴한다.
  */
  BOOL isLocalRenamedItself();

  /**
  * @brief 로컬측 이동 및 이름 변경이 유효한지 확인한다.
  * @return BOOL : 로컬측 이동 및 이름 변경되었으면 TRUE를 리턴한다.
  */
  BOOL isLocalRenamedAs();

  /**
  * @brief 서버측 이동 및 이름 변경이 유효한지 확인한다.
  * @return BOOL : 서버측 이동 및 이름 변경되었으면 TRUE를 리턴한다.
  */
  BOOL isServerRenamedAs();

  /**
  * @brief 서버측이 이동되었는지 확인한다.
  * @return BOOL : 서버측 이동되었으면 TRUE를 리턴한다.
  * @details A. mNewParent가 유효하고 mParent와 다르면 TRUE를 리턴한다.
  */
  BOOL isServerMovedAs();

  /**
  * @brief 로컬측 이동 및 이름변경이 유효한지 확인한다.
  * @return BOOL : 서버측 이동되었으면 TRUE를 리턴한다.
  * @details A. mpLocalRenamedAs가 유효하고,
  *     - mpLocalRenamedAs에 해당하는 파일이 존재하지 않으면 해제한다.
  * @n B. mpLocalRenamedAs가 유효하면 TRUE를 리턴한다.
  */
  BOOL validLocalRenamedAs();

  /**
  * @brief 메타 파일명을 지정한다.
  * @param filename : 메타 파일명.
  */
  void SetMetafilename(TCHAR *filename);

  /**
  * @brief 메타 파일명을 리턴한다.
  */
  inline TCHAR *GetMetafilename() { return mpMetafilename; }

  /**
  * @brief 새로 추가된 항목인지 확인한다.
  * @details A. 로컬 정보의 파일 날짜가 비어있고, 서버 정보의 파일 날짜가 비어있으면 TRUE를 리턴한다.
  */
  BOOL IsNewlyAdded();

  inline void SetServerScanState(int s) { mServerScanState = s; }
  /**
  * @brief 서버 스캔 상태를 설정하는 가상함수.
  * @param s : 상태값(0 : 변동없음, ITEM_EXIST : 존재함, ITEM_DELETED : 삭제됨)
  * @param flag : 플래그(WITH_ITSELF, WITH_RECURSE, WITH_CHILD, EXCLUDE_MOVED)
  * @details A. 플래그에 WITH_ITSELF가 있으면 setServerScanState() 메소드를 호출한다.
  */
  virtual void SetScanServerFolderState(int s, int flag);

  /**
  * @brief 서버 스캔 상태값에 따라 서버 상태를 설정하는 가상함수.
  * @details A. 
  */
  virtual void SetServerStateByScanState();

  /**
  * @brief 대상 파일의 최종 수정시간을 시스템에서 읽어 반환한다.
  * @param local_time : 로컬측 파일의 최종 수정시간
  * @param server_time : 서버측 파일의 최종 수정시간(사용 안함)
  */
  BOOL GetCurrentFileTime(SYSTEMTIME &local_time, SYSTEMTIME &server_time);

  /**
  * @brief 새 정보값으로 업데이트 한다.
  * @details A. 로컬측 새 정보의 파일시간이 유효하면 업데이트 한다(mLocal.FileTime = mLocalNew.FileTime).
  * @n B. 서버측 새 정보의 파일시간이 유효하면 업데이트 한다(mServer.FileTime = mServerNew.FileTime).
  * @n C. 로컬측 서버측 파일 크기도 업데이트 한다.
  * @n D. 서버측 최종 수정자 정보도 업데이트 한다.
  */
  void RefreshOldNew();

  /**
  * @brief 이 개체와 대상 개체의 내용을 비교하여 이 개체가 크면 1을, 작으면 -1, 같으면 0을 리턴한다.
  * @param item : 비교할 대상 개체
  * @param column : 비교할 내용
  * @param orderAscend : 차순
  * @details A. CompareItem(KSyncItem *item, int column)을 호출하여 비교한다.
  * @n B. orderAscend가 FALSE이면 위의 비교 결과값을 뒤집는다.
  */
  int CompareItem(KSyncItem *item, int column, BOOL orderAscend);

  /**
  * @brief 이 개체와 대상 개체의 내용을 비교하여 이 개체가 크면 1을, 작으면 -1, 같으면 0을 리턴한다.
  * @param item : 비교할 대상 개체
  * @param column : 비교할 내용(0 : 파일명, 1 : 로컬 수정 시간, 2 : 로컬측 파일 크기, 3 : 파일명,
  * 4 : 서버측 최종 수정자 이름, 5 : 서버측 수정시간, 6 : 서버측 파일 크기)
  * @details A. 컬럼이 0, 또는 3이면, 양쪽의 로컬 파일 이름을 비교하고 결과를 리턴한다.
  * @n B. 컬럼이 1이면, 양쪽의 로컬 시간을 비교하고 결과를 리턴한다.
  * @n C. 컬럼이 2이면, 양쪽의 로컬 파일 크기를 비교하고 결과를 리턴한다.
  * @n D. 컬럼이 4이면, 양쪽의 서버측 최종 사용자 이름을 비교하고 결과를 리턴한다.
  * @n E. 컬럼이 5이면, 양쪽의 서버 시간을 비교하고 결과를 리턴한다.
  * @n F. 컬럼이 6이면, 양쪽의 서버 파일 크기를 비교하고 결과를 리턴한다.
  * @n G. 0을 리턴한다.
  */
  int CompareItem(KSyncItem *item, int column);

#ifdef USE_META_STORAGE
  /**
  * @brief 개체의 동기화 정보를 저장할 메타파일 이름을 준비함.(DB 스토리지 방식에서는 하는 일 없음)
  * @return BOOL
  * @param baseFolder : 동기화 루트 폴더의 로컬 폴더
  */
  BOOL MakeMetafilename(LPCTSTR baseFolder);

  /**
  * @brief 개체의 동기화 정보를 저장할 메타파일 준비함.(DB 스토리지 방식에서는 하는 일 없음)
  * @return BOOL
  * @param s : 스토리지 개체
  * @param metafile_flag : 메타파일 처리 플래그(삭제, 생성)
  */
  virtual BOOL ReadyMetafilename(KSyncRootStorage *s, int metafile_flag);

  /**
  * @brief 개체의 동기화 정보를 메타파일에서 읽어들임
  * @return BOOL
  * @param s : 스토리지 개체
  * @param mask : 읽어올 항목 마스크 값
  * @details A. s가 NULL 이면 GetSafeStorage() 메소드로 스토리지 개체을 준비한다.
  * @n B. ReadyMetafilename() 메소드 호출한다
  * @n C. 상대 경로명을 준비하고 KSyncRootStorage::LoadMetafile()메소드를 호출하여 메타데이터를 읽어온다.
  */
  BOOL LoadMetafile(KSyncRootStorage *s, DWORD mask=META_MASK_ALL);

  /**
  * @return KSyncRootStorage*
  * @brief 개체의 동기화 정보가 저장되는 스토리지 개체를 얻어돔(동기화 루트 폴더 개체가 가지고 있음)
  * @details A. 동기화 루트 폴더 개체를 준비한다.
  * @n B. KSyncRootFolderItem::GetStorage() 메소드로 스토리지 개체을 얻어와서 리턴한다.
  */
  KSyncRootStorage* GetSafeStorage();

  /**
  * @brief 개체의 동기화 정보를 스토리지 개체에 저장함
  * @return BOOL
  * @param s : 스토리지 개체
  * @param mask : 저장할 항목 마스크 값
  * @param metafile_flag : 메타파일 처리 플래그(삭제)
  */
  BOOL StoreMetafile(KSyncRootStorage *s, DWORD mask = META_MASK_ALL, int metafile_flag = 0);

  /**
  * @brief 개체의 경로에 따라 메타파일의 경로와 이름을 변경함
  * @return BOOL
  * @param prevRelativePathName : 기존 메타파일 이름
  * @param updateAlso : BOOL이면 메타 데이터도 저장하여 업데이트 함.
  */
  BOOL MoveMetafileAsRename(LPCTSTR prevRelativePathName, int flag, KSyncRootStorage *s = NULL);

#endif // USE_META_STORAGE

  /**
  * @brief 대상 항목을 읽을수 있으면 TRUE를 리턴함.
  * @return BOOL : 읽을수 있으면 TRUE
  * @param onLocal : TRUE이면 로컬측 대상, 아니면 서버측 대상
  * @details A. 로컬측 대상이면 결과가 로컬 삭제 서버 수정이면, 서버측 대상이면 결과가 로컬 수정 서버 삭제이면 FALSE를 리턴한다.
  * @n B. 서버측 대상이면 
  *     - 충돌 오류가 다운로드 권한없음, 서버 폴더 생성 실패, 서버 드라이브 권한 없음, 동기화 루트 폴더 접근 권한 없음,
  *     .
  * 동기화 폴더 없음, 폴더 접근 권한 없음, 서버 접근 권한 없음 등이면 FALSE를 리턴한다.
  * @n C. IsServerFileExist() 메소드 호출하여 서버측 파일이 존재하지 않으면 FALSE를 리턴한다.
  * @n D. 서버측 권한 SFP_READ가 없으면 FALSE를 리턴한다.
  * @n E. TRUE를 리턴함.
  */
  BOOL CanBeReadable(BOOL onLocal);

  /**
  * @brief 대상 항목의 이름을 바꿀수 있으면 TRUE를 리턴함.
  * @return BOOL : 바꿀수 있으면 TRUE
  * @param onLocal : TRUE이면 로컬측 대상, 아니면 서버측 대상
  * @details A. 로컬측 대상이면 결과가 로컬 삭제 서버 수정이면, 서버측 대상이면 결과가 로컬 수정 서버 삭제이면 FALSE를 리턴한다.
  * @n B. 서버측 대상이면 서버측 권한 SFP_WRITE가 없으면 FALSE를 리턴한다.
  * @n E. TRUE를 리턴함.
  */
  BOOL CanBeRenamed(BOOL onLocal);

  /**
  * @brief 대상 항목의 서버에서 대상 개체에 대한 OID를 설정함.
  * @return void
  * @param serverOid : 서버에서 대상 개체에 대한 OID
  */
  void SetServerOID(LPCTSTR serverOid);

  /**
  * @brief 동기화 진행 상태를 메타 데이터에 저장하고, 탐색기의 Overlay Icon의 모양을 동기화 진행중인 상태로 보이도록 셋트하거나 해제함.
  * @return void
  * @param baseFolder : 동기화 루트 폴더
  * @param startSync : 1이면 동기화 진행중, 0이면 동기화 진행중 아님
  * @param iconUpdateRecurse : TRUE이면 하위의 폴더 및 파일도 Shell Icon을 업데이트 함.
  * @details A. 전체 경로명을 준비하고 해당 파일이 존재하면
  *     - start sync가 2보다 작으면
  *       + startSync가 0이 아니면 결과값에 FILE_NOW_SYNCHRONIZING 플래그를 넣고
  *       + startSync가 0이면 결과값에 FILE_NOW_SYNCHRONIZING 플래그를 뺀다.
  *     .
  * @n KSyncCoreBase::ShellNotifyIconChangeOverIcon() 메소드로 탐색기 아이콘을 업데이트한다.
  */
  void SetItemSynchronizing(LPCTSTR baseFolder, int startSync, int flag);

  /**
  * @brief 로컬과 서버측 항목의 상태에 따라 결과값(업로드/다운로드/삭제/이름변경/충돌/오류 등)을 정함.
  * @return void
  * @param userAction : 사용자 지정 action(업로드/다운로드/삭제/없음)
  * @details A. 서버 상태가 권한없음(ITEM_STATE_NO_PERMISSION)이면,
  *     - 사용자 액션이 업로드이면 결과를 FILE_SYNC_NO_UPLOAD_PERMISSION로, 아니면 FILE_SYNC_NO_DOWNLOAD_PERMISSION로 한다.
  *     .
  * @n B. 아니고, 서버 상태가 위치 변경됨(ITEM_STATE_LOCATION_CHANGED)이면,
  *     - 결과를 FILE_SYNC_PARENT_FOLDER_MOVED로 한다.
  *     .
  * @n C. 아니고, 로컬 또는 서버 상태가 오류(ITEM_STATE_ERROR)이면,
  *     - 결과를 FILE_SYNC_ERROR_FAIL로 한다.
  *     .
  * @n D. 아니고, 로컬 또는 서버 상태가 이동됨(ITEM_STATE_MOVED)이면,
  *     - 로컬측 이름 변경이 유효하고, 서버측 이름 변경도 유효하며 두개의 내용이 다르면,
  * 결과값을 양쪽 수정 충돌(FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)로 한다.
  *     - 아니면
  *       + 결과값을 FILE_SAME_MOVED로 한다.
  *       + 로컬 상태가 업데이트됨(ITEM_STATE_UPDATED)이고
  *         + 서버 상태가 업데이트됨(ITEM_STATE_UPDATED)이면, 결과값에 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED를 더한다.
  *         + 아니고, 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값에 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED를 더한다.
  *         + 아니면 결과값에 FILE_NEED_UPLOAD를 더한다.
  *       + 아니면
  *         + 서버 상태가 업데이트됨(ITEM_STATE_UPDATED)이면, 결과값에 FILE_NEED_DOWNLOAD를 더한다.
  *         + 아니고, 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값에 (FILE_NEED_LOCAL_DELETE | FILE_NEED_RETIRED)를 더한다.
  *     .
  * @n E. 아니고, 로컬 상태가 없음(ITEM_STATE_NONE, 이전에도 없었고 지금도 없음)이면,
  *     - 서버 상태가 지금 존재함(ITEM_STATE_NOW_EXIST)이면, 결과값을 FILE_NEED_DOWNLOAD로 한다.
  *     - 아니고, 서버 상태가 없음(ITEM_STATE_NONE)이거나 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값을 FILE_NEED_RETIRED로 한다.
  *     .
  * @n F. 아니고, 로컬 상태가 업데이트됨(ITEM_STATE_UPDATED)이면,
  *     - 서버 상태가 업데이트됨(ITEM_STATE_UPDATED)이면, 결과값을 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED로 한다.
  *     - 아니고, 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값을 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED로 한다.
  *     - 아니면 결과값을 FILE_NEED_UPLOAD로 한다.
  *     .
  * @n G. 아니고, 로컬 상태가 삭제됨(ITEM_STATE_DELETED)이면,
  *     - 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값을 FILE_NEED_RETIRED로 한다.
  *     - 아니고, 서버 상태가 업데이트됨(ITEM_STATE_UPDATED) 또는 이동됨(ITEM_STATE_MOVED)이면,
  *     .
  * 결과값을 FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED로 한다.
  *     - 아니고, 서버 상태가 지금 존재함(ITEM_STATE_NOW_EXIST)이면, 결과값을 (FILE_NEED_SERVER_DELETE|FILE_NEED_RETIRED)로 한다.
  *     - 아니면 결과값을 FILE_NEED_RETIRED로 한다.
  *     .
  * @n H. 아니고, 로컬 상태가 새로 추가됨(ITEM_STATE_NEWLY_ADDED)이면,
  *     - 서버 상태가 새로 추가됨(ITEM_STATE_NEWLY_ADDED)이면, 결과값을 FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED로 한다.
  *     - 아니면 결과값을 FILE_NEED_UPLOAD로 한다.
  *     .
  * @n I. 아니고, 로컬 상태가 이동됨(ITEM_STATE_MOVED)이면,
  *     - 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값을 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED로 한다.
  *     - 아니고 서버 상태가 업데이트됨(ITEM_STATE_UPDATED)이면, 결과값을 FILE_NEED_DOWNLOAD로 한다.
  *     .
  * @n J. 아니면, 
  *     - 서버 상태가 업데이트됨(ITEM_STATE_UPDATED)이면, 결과값을 FILE_NEED_DOWNLOAD로 한다.
  *     - 아니고 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면, 결과값을 (FILE_NEED_LOCAL_DELETE | FILE_NEED_RETIRED)로 한다.
  *     - 아니고 서버 상태가 새로 추가됨(ITEM_STATE_NEWLY_ADDED)이면, 결과값을 FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED로 한다.
  *     - 아니고 서버 상태가 이동됨(ITEM_STATE_MOVED)이면, 결과값을 FILE_NEED_LOCAL_MOVE로 한다.
  *     - 아니고 서버 상태가 없음(ITEM_STATE_NONE, 이전에도 없었고 지금도 없음)이면, 결과값을 FILE_NEED_UPLOAD로 한다.
  *     .
  * @n K. userAction에 사용자 선택 방향이 있으면 이 값을 리턴한다.
  * @n L. 결과값이 양쪽 이동이나 제거가 아니면,
  *     - 로컬 상태가 이동됨이면,
  *       + 서버 상태가 삭제됨이면, 결과값을 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED로 한다.
  *       + 결과값이 0이면 FILE_NEED_SERVER_MOVE를 더한다.
  *     - 서버 상태가 이동됨이면,
  *       + 로컬 상태가 삭제됨이면, 결과값을 FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED로 한다.
  *       + 결과값이 0이면 FILE_NEED_LOCAL_MOVE를 더한다.
  *     .
  * @n M. 결과값을 리턴한다.
  */
  int makeResultFromState(int userAction, int syncMethod);

  /**
  * @brief 동기화 개체 파일이 존재하는지 검사함.
  * @return BOOL : 동기화 개체가 존재하면 TRUE
  * @param fullname : 동기화 파일의 로컬 경로명
  * @details IsFileExist()메소드를 호출하여 파일이 존재하는지 검사하고 그 결과를 리턴한다.
  */
  virtual BOOL IsItemExist(LPCTSTR fullname);

  /**
  * @brief 동기화 개체가 존재하는지 검사함.
  * @return BOOL : 동기화 개체가 존재하면 TRUE
  * @param onLocal : TRUE이면 로컬측 대상, FALSE이면 서버측 대상
  * @details A. onLocal이 TRUE이면 IsLocalFileExist()메소드를 호출하여 로컬 파일이 존재하는지 검사하고 그 결과를 리턴한다.
  * @n B. onLocal이 FALSE이면 
  *     - IsServerFileExist() 가 TRUE이거나 충돌 결과값이 양쪽 신규 추가 충돌인 경우에도 존재하는 것으로 간주하고,
  *     - 없다면 충돌 오류가 0이 아니고 mServerNew의 값이 유효하면(FileTime이 유효) 존재하는 것으로 간주한다.
  *     - mServerState에 ITEM_STATE_DELETED가 있으면 없는 것으로 처리한다.
  */
  BOOL IsSyncFileExist(BOOL onLocal);

  /**
  * @brief 동기화 루트 폴더 기준으로 트리 구조에서 해당 개체의 depth값을 구하는 가상함수.
  * @return int : depth in tree(0 for root object)
  * @details A. mParent가 NULL이면(이는 동기화 루트 폴더) 0을 리턴하고,
  * @n B. NULL이 아니면 mParent의 GetDepth() 메소드를 호출하고 리턴받은 값에 1을 더하여 리턴한다.
  */
  virtual int GetDepth();

  /**
  * @brief 로컬측 대상 개체의 상태와 서버측 대상 개체의 상태를 비교하여 처리할 방법을 리턴함.
  * @return int
  * @param baseFolder : 동기화 루트 폴더의 로컬 위치(동기화 대상 개체들은 상대 경로만을 가지고 있으며, 
  * 이 루트 폴더의 위치를 기준으로 절대 경로를 생성하여 작업한다)
  * @details A. GetSyncResultName()으로 로컬측 결과 파일의 경로명을 준비한다.
  *     - 이 파일이 편집중이면 이 항목의 동기화 작업을 스킵하도록 phase를 PHASE_END_OBJ_JOB으로 설정하고,
  *     - AddSyncFlag() 메소드를 호출하여 mFlag에 SYNC_ITEM_EDITING_NOW값을 설정한다.
  *     .
  * @n B. checkAddedRenamedSyncError()를 호출하여 결과가 TRUE이면 0을 리턴한다.
  * @n C. storage 개체를 준비한다.
  * @n D. 로컬측 결과 파일명이 지정한 길이 이상이면 r을 FILE_SYNC_OVER_PATH_LENGTH로 하고 I.항으로 점프한다.
  * @n E. ReadyMetafilename() 메소드로 메타 데이터 저장을 준비한다. 실패하면 결과를 FILE_SYNC_CANNOT_MAKE_METAFILE로 하고 리턴한다.
  * @n F. 메타파일명이 준비되었고 파일이 존재하지 않으면 먼저 저장한다.
  * @n G. compareLocalServerState()메소드를 호출하여 로컬측 서버측 상태를 비교하고,
  * @n H. makeResultFromState()메소드를 호출하여 결과를 결정한다.
  * @n I. 비교를 마무리한다.
  *     - 결과에 충돌이나 오류가 없으며, 제거될(FILE_NEED_RETIRED) 것이 아니면 다시 결과 파일명을 준비하고, 
  * 결과 파일명이 최대 길이를 넘으면 결과를 오류(FILE_SYNC_CANNOT_UPLOAD_LONGPATH 또는 FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH)로 설정한다.
  *     .
  * @n J. 결과를 저장하고, 충돌 결과도 저장한다.
  * @n K. 로컬과 서버가 다르게 이름 변경이 되었으면 결과값의 방향(FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD|FILE_SAME_MOVED)을 충돌 결과에 추가한다.
  * @n L. 결과가 이전과 다르면 StoreMetafile() 메소드를 호출하여 결과와 상태값등을 저장한다.
  */
  virtual int compareSyncFileItem(LPCTSTR baseFolder, int syncMethod);

  BOOL checkNewFolderExcluded(LPTSTR newPathName);

  /**
  * @brief 항목의 localState/serverState의 기본 상태를 비교하여 localState/serverState에 삭제/이동/갱신 등의 플래그 설정함
  * @return void
  * @details A. 서버 상태값의 권한없음(ITEM_STATE_NO_PERMISSION), 충돌창에서 이름변경(ITEM_STATE_CONFLICT_RENAMED) 등의 상태값을 임시 저장한다.
  * @n B. 상태값에서 과거와 현재의 존재 상태(ITEM_STATE_EXISTING)값만 남긴다.
  * @n C. 로컬 상태가 새로 추가된 상태이면(mLocalState == ITEM_STATE_NOW_EXIST)
  *     - 로컬 상태에 ITEM_STATE_NEWLY_ADDED 플래그를 추가하고, 
  *     - 로컬측 파일이 존재하지 않으면 ITEM_STATE_DELETED 플래그를 추가한다.
  *     .
  * @n C. 로컬 상태가 삭제된 상태이면(mLocalState == ITEM_STATE_WAS_EXIST)
  *     - 로컬 상태에 ITEM_STATE_DELETED 플래그를 추가한다.
  *     .
  */
  void compareLocalServerState();

  /**
  * @brief 새로 추가되거나 이동된 항목을 처리할때, 동일한 이름의 파일이 존재하는지 검사하고 존재하면 충돌 상태가 되도록 설정함.
  * @return BOOL : 동기화 오류가 발생하면 TRUE를 리턴한다.
  * @param user_action : 충돌 처리창에서 사용자가 업로드/다운로드 방향을 선택한 경우, 충돌 처리가 아닌 동기화 진행 단계에서는 0임.
  * @details A. 사용자 선택 방향이 없거나 로컬로 적용을 선택하였고,
  *     - 서버측에는 새로 추가되었고, 로컬측은 없으며, 대상 파일이 존재하면
  * 동기화 오류 FILE_SYNC_ALREADY_EXISTS_ON_LOCAL로 설정하고 TRUE를 리턴한다.
  * @n B. 사용자 선택 방향이 없거나 서버로 적용을 선택하였고,
  *     - 로컬측에는 새로 추가되었고, 서버측은 없으며, 동기화 폴더 내에 대상 이름과 동일한 경로명으로 이름변경 또는 이동된 항목이 있으면
  * 동기화 오류 FILE_SYNC_ALREADY_EXISTS_ON_SERVER로 설정하고 TRUE를 리턴한다.
  */
  BOOL checkAddedRenamedSyncError(int user_action);

  /**
  * @brief 결과에 변함이 없으면 조건에 따라 설정한 phase로 바꾼다.
  * @return void
  * @param added_phase : 새로 추가된 항목이면 이 phase값을 적용한다.
  * @param renamed_phase : 이름 변경/이동된 항목이면 이 phase값을 적용한다.
  * @param normal_phase : 정상 항목이면 이 phase값을 적용한다.
  * @param parentCheckNeed : Child 항목이 변경되었으면 Parent 항목도 맞추어 변경한다.
  * @details A. 결과가 변함이 없으며(FILE_NOT_CHANGED)
  *     - 로컬 또는 서버측 이름 변경 또는 이동이 있으면 renamed_phase로 설정한다.
  *     - 아니면 normal_phase로 설정한다.
  *     .
  */
  virtual void SetUnchangedItemPhase(int added_phase, int renamed_phase, int normal_phase, BOOL parentCheckNeed);

  /**
  * @brief 동기화 결과값과 phase값을 지정한 값으로 설정한다.
  * @param result : 지정한 결과값.
  * @param phase : 지정한 phase값
  * @param flag : 플래그(SET_SELECTED_ONLY, SET_WITH_ITSELF, EXCLUDE_OUTGOING, SET_SERVER_DELETED, SET_LOCAL_DELETED)
  */
  virtual void SetConflictResultRecurse(int result, int phase, int flag);

  /**
  * @brief 이 항목과 하위 항목들을 동기화 충돌 처리를 위한 phase로 바꾼다.
  * @return BOOL : 이 항목이 동기화 Enable이 되어 있으면 TRUE를 리턴한다.
  * @param parent_phase : 부모 항목이면 이 phase값을 적용한다.
  * @param renamed_phase : 이름 변경/이동된 항목이면 이 phase값을 적용한다.
  * @param normal_phase : 정상 항목이면 이 phase값을 적용한다.
  * @details A. 동기화 가능하고,
  *     - 로컬측이나 서버측이 이름 변경 또는 이동이 있으면 PHASE_MOVE_FOLDERS로 설정한다.
  *     - 아니면 normal_phase를 적용한다.
  *     .
  * @n B. 동기화 제외된 항목이면 PHASE_END_OF_JOB으로 설정한다.
  */
  virtual BOOL SetPhaseOnResolve(int parent_phase, int renamed_phase, int normal_phase);

  /**
  * @brief 동기화 충돌 및 오류 발생한 항목에서 처리를 위한 준비를 한다(양측의 파일을 다시 비교하고 권한도 비교하고, 상태도 다시 업데이트한다)
  * @return int : 충돌 처리 준비가 완료되면 R_SUEECSS를, 오류나 충돌이 다시 발생하면 해당 코드를 리턴한다(FILE_CONFLICT or FILE_SYNC_ERR)
  * @param rootItem : 동기화 루트 폴더 항목 개체
  * @param phase : 현재의 동기화 작업 phase
  * @details A. 동기화 Enable이 아니면 R_SUCCESS로 리턴한다.
  * @n B. 로컬측 이름변경/이동 상대경로를 업데이트 한다.
  * @n C. PHASE_CONFLICT_FILES이면 user_action 내용에 따른 로그 내용을 저장한다.
  * @n D. PHASE_CONFLICT_FILES이면,
  *     - 서버측 항목이 다시 변경되었는지 CheckServerModifiedAgain()으로 확인한다.
  *     - 확인한 값이 0이 아니면 mServerState에 값을 더하고, 
  *     - 확인한 값이 ITEM_STATE_UPDATED이고, 사용자 action이 업로드 또는 다운로드이면, mServerState에 
  *         ITEM_STATE_RE_UPDATED를 더하고, mServerNew에 새로 받은 서버 정보를 merge하고, R_SUCCESS로 리턴한다.
  *     - 확인한 값이 ITEM_STATE_DELETED(서버측 파일이 삭제됨)이면, mServerState를 ITEM_STATE_DELETED로 바꾼다.
  *     - mServerState에 ITEM_STATE_RE_UPDATED가 있으면 서버측은 변경된 것으로 한다.
  *     - permission값이 변경되었으면 업데이트한다.
  *     .
  * @n E. 동기화 루트 폴더 경로명을 준비하고,
  * @n F. 로컬 파일이 존재하고, mLocalState에 파일이 존재하거나 존재했으면
  *     - IsLocalModified()메소드를 호출하여 로컬이 변경되었는지 확인하고
  *     - IsLocalModifiedAgain()으로 로컬이 다시 변경되었고, 사용자 action이 업로드 또는 다운로드이면, mLocalState에 
  *         ITEM_STATE_RE_UPDATED를 더하고, mLocalNew에 새로 받은 정보를 merge하고, R_SUCCESS로 리턴한다.
  *     - 파일이면, 결과 파일명을 준비하고 해당 파일이 편집중인 상태이면 mFlag에 SYNC_ITEM_EDITING_NOW 플래그를 더하고, 
  * 편집중이 아니면 SYNC_ITEM_EDITING_NOW 플래그를 뺀다.
  *     .
  * @n G. 로컬측 또는 서버측이 이동 또는 이름변경된 경우에
  *     - 로컬측 경로가 바뀐 경우에
  *       + 바뀐 경로의 동기화 항목(KSyncFolderItem)을 찾아보고 없으면 오류 로그를 남기고 로컬측 이동 정보를 클리어한다.
  *     .
  * @n H. 로컬측이 기존에 존재했던 경우에
  *     - 로컬 파일이 지금 존재하지 않으면, 로컬 상태값에서 ITEM_STATE_NOW_EXIST|ITEM_STATE_UPDATED|ITEM_STATE_MOVED를 지우고,
  *     - 로컬 파일이 지금 존재하면 로컬 상태값에서 ITEM_STATE_NOW_EXIST를 더한다.
  *     .
  * @n I. 로컬 상태에 ITEM_STATE_RE_UPDATED가 있으면 local_changed = TRUE로 한다.
  * @n J. checkAddedRenamedSyncError()를 호출하여 TRUE이면 R_SUCCESS를 리턴한다.
  * @n K. compareLocalServerState()를 호출하여 로컬측 서버측 상태값을 비교한다.
  * @n L. 서버 파일이 변경되었으면 mServerState에 ITEM_STATE_UPDATED를 더한다.
  * @n M. 로컬 파일이 변경되었고, 지금 파일이 존재하면 mLocalState에 ITEM_STATE_UPDATED를 더한다.
  * @n N. 로컬측과 서버측 양쪽이 이동 또는 이름변경된 경우에
  *     - 상위 폴더의 실제 경로를 다시 받아오고 여기에 로컬측 이름을 붙인다.(상대 경로명)
  *     - 위의 값과 mpLocalRenamedAs의 내용이 동일하면 로컬측 이름변경 정보를 클리어하고, 상태값에서 ITEM_STATE_MOVED 플래그도 클리어한다.
  *     .
  * @n O. 서버 상태값이 존재하였거나 존재할 때
  *     - 서버가 삭제된 플래그가 없고, fullPathIndex가 유효하고, fullPathIndex가 변경되었을 때,
  *       + mServerState에 ITEM_STATE_MOVED 플래그를 더하고,
  *       + 이 개체가 폴더이면, 새로운 서버측 경로명을 mpServerRenamedAs에 복사한다.
  *     - 서버가 이동된 플래그가 있으면,
  *       + 로컬측 상대 경로명과 서버측 상대 경로명을 구하고, 양쪽 경로명이 다르면,
  *         + 서버 상태값에 ITEM_STATE_MOVED 플래그를 더하고, mpServerRenamedAs에 서버측 상대 경로명을 복사한다.
  *       + 양쪽 경로명이 같으면,
  *         + 서버 상태값에 ITEM_STATE_MOVED 플래그를 빼고, mpServerRenamedAs를 클리어한다.
  *     .
  * @n P. 사용자가 삭제를 선택한 플래그가 있으면 ITEM_STATE_MOVED 플래그를 뺀다.
  * @n Q. 양쪽 새로 추가된 충돌이고, 로컬측 또는 서버측이 이동된 경우 복제 항목을 생성하여 추가하고 각각 업로드/다운로드로 설정하고 리턴한다.
  * @n R. 동기화 오류가 발생한 경우,
  *     - 로컬측 또는 서버측에 업데이트 플래그가 있는 경우,
  *       + 사용자가 업로드를 선택한 경우, 
  *         + 로컬 상태에 현재 존재하면 ITEM_STATE_UPDATED 플래그를 더한다.
  *         + 충돌 처리 phase이면(phase == PHASE_CONFLICT_FILES), 서버 상태에서 ITEM_STATE_UPDATED|ITEM_STATE_DELETED 를 클리어한다.
  *       + 사용자가 다운로드를 선택한 경우, 서버 상태에 ITEM_STATE_UPDATED를 추가한다.
  *     - 로컬측 또는 서버측에 이동 플래그가 있는 경우,
  *       + 사용자가 업로드를 선택한 경우, 서버측 상태에서 이동 플래그를 지우고, 로컬측 상태에 이동 플래그를 넣는다.
  *       + 사용자가 다운로드를 선택한 경우, 서버측 상태에서 이동 플래그를 넣고, 로컬측 상태에 이동 플래그를 지운다.
  *     .
  * @n S. makeResultFromState() 메소드를 호출하여 결과를 만든다.
  * @n T. 사용자가 방향을 선택하였으면,
  *     - 사용자가 다운로드 선택하였고, 서버측이 삭제되었거나 서버측이 지금 존재하지 않으면(ITEM_STATE_EXISTING) user_action을 양쪽 삭제로 한다.
  *     - 사용자가 업로드 선택하였고, 로컬측이 삭제되었거나 로컬측이 지금 존재하지 않으면(ITEM_STATE_EXISTING) user_action을 양쪽 삭제로 한다.
  *     - d 값에 user_action값을 넣는다.
  *     - 로컬 상태에 지금 존재하지 않고, d값에 로컬 삭제 플래그가 있으면 로컬 삭제 플래그를 제거한다.
  *     - 서버 상태에 지금 존재하지 않고, d값에 서버 삭제 플래그가 있으면 서버 삭제 플래그를 제거한다.
  *     - 동기화 오류가 발생되었으면,
  *       + 서버 상태가 삭제됨이면
  *         + 사용자가 업로드를 선택하였으면, d값을 업로드(FILE_NEED_UPLOAD)로 셋트한다.
  *         + 사용자가 다운로드를 선택하였으면, d값을 로컬 삭제(FILE_NEED_LOCAL_DELETE)로 셋트한다.
  *       + 로컬 상태가 삭제됨이면
  *         + 사용자가 다운로드를 선택하였으면, d값을 다운로드(FILE_NEED_DOWNLOAD)로 셋트한다.
  *         + 사용자가 업로드를 선택하였으면, d값을 서버 삭제(FILE_NEED_SERVER_DELETE)로 셋트한다.
  *     - d값에 로컬 및 서버 삭제 플래그가 없으면, FILE_NEED_RETIRED를 해제하고,
  *     - d값에 로컬 및 서버 삭제 플래그가 있으면, FILE_NEED_RETIRED를 설정한다.
  *     .
  * @n U. phase가 PHASE_COPY_FILES 이상이고, d값에 다운드/업로드/삭제 플래그가 없으면, d값에서 MOVE 플래그를 지운다.
  * @n V. d값을 결과, 충돌 결과값으로 설정하고 R_SUCCESS를 리턴한다.
  */
  virtual int ReadyResolve(KSyncItem *rootItem, int phase, int syncMethod);

  /**
  * @brief 해당 phase에 대한 동기화 작업을 진행한다.
  * @return int : 성공하면 R_SUEECSS를, 오류나 충돌이 발생하면 해당 코드를 리턴한다(FILE_CONFLICT or FILE_SYNC_ERR)
  * @param policy : 동기화 정책
  * @param baseFolder : 로컬 동기화 루트 폴더
  * @param phase : 동기화 작업 단계
  * @details A. 동기화 작업 단계를 phase로 설정한다.
  * @n B. 동기화 작업 단계가 로드일때(phase == PHASE_LOAD_FILES) d에 결과값을 가져오고, 이동 플래그만 남긴다.
  * @n C. 동기화 작업 단계가 이동 또는 복사일때(PHASE_MOVE_FOLDERS or PHASE_COPY_FILES)
  *     - d에 결과값을 가져오고,
  *     - d에 오류나 충돌이 있으면 오류나 충돌값을 r에 복사하고 작업을 마무리한다.(goto end_sync_copy)
  *     - phase가 PHASE_COPY_FILES이고, d에 다운로드/업로드/삭제가 없으면 d에서 이동 플래그를 지운다.
  *     .
  * @n D. 동기화 작업 단계가 충돌처리(PHASE_CONFLICT_FILES)일때,
  *     - d에 충돌 결과값을 가져오고,
  *       + d가 충돌이고, d에 사용자가 방향을 선택하지 않았으면, r에 충돌값을 넣고 작업을 마무리한다.(goto end_sync_copy)
  *     - Enable상태가 아니면(mEnable == FALSE), 로그를 저장하고 R_SUCCESS로 리턴한다.
  *     - 로컬측 서버측 이름이 다르면
  *       + d가 업로드이고, mServerState가 0이 아니면,
  *         + mpLocalRenamedAs가 비어있거나, 로컬 상태에 이동 플래그가 없으면 
  *           + 로컬 상태에 이동 플래그를 넣고, 서버 상태에 이동 플래그를 빼고, 이름 변경 정보를 클리어하고, 로컬측 결과 파일명을 mpLocalRenamedAs로 복사한다.
  *       + d가 다운로드이고, mLocalState가 0이 아니면,
  *         + mpServerRenamedAs가 비어있거나, 서버 상태에 이동 플래그가 없으면,
  *           + 서버 상태에 이동 플래그를 넣고, 로컬 상태에 이동 플래그를 빼고, 이름 변경 정보를 클리어하고, 서버측 결과 파일명을 mpLocalRenamedAs로 복사한다.
  *     .
  * @n E. 로컬측 결과 경로명을 만들고, 
  *     - 이 파일이 다른 프로그램에서 편집중이면 로그에 저장하고 R_SUCCESS로 리턴한다.
  *     - 이 경로명이 최대 길이를 넘으면 r을 FILE_SYNC_OVER_PATH_LENGTH로 설정하고, 작업을 마무리한다.
  *     .
  * @n F. d값이 제거 플래그(FILE_NEED_RETIRED)를 가지면
  *     - d값에서 삭제 플래그(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE)를 지우고,
  *     - 로컬측 결과 파일명을 만들어서 해당 파일이 존재하면 d에 FILE_NEED_LOCAL_DELETE 플래그를 넣고,
  *     - 서버측 파일이 존재하면 d에 FILE_NEED_SERVER_DELETE 플래그를 넣는다.
  *     .
  * @n G. d값에서 이동 플래그((FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE))를 지운다.
  * @n H. d값에 양쪽 이동(FILE_SAME_MOVED)플래그를 검사하여,
  *     - 있으면 로그에 이 내용을 기록한다.
  *     - 없으면,
  *       + 로컬 상태가 이동 플래그를 가졌고, 서버 상태도 이동 플래그를 가졌으면,
  *         + d값에 업로드가 있으면 d에 서버측 이동 플래그(FILE_NEED_SERVER_MOVE)를 더하고,
  *         + 아니고, d값에 다운로드가 있으면 d에 로컬측 이동 플래그(FILE_NEED_LOCAL_MOVE)를 더한다.
  *       + 아니면,
  *         + d값에 업로드/다운로드가 없고, d값에 제거(FILE_NEED_RETIRED)가 없으면,
  *           + 로컬 상태가 이동이면 d값에 서버측 이동(FILE_NEED_SERVER_MOVE)을 더하고
  *           + 아니고, 서버 상태가 이동이면 d값에 로컬측 이동(FILE_NEED_LOCAL_MOVE)을 더한다.
  *         + 아니면,
  *           + d값에 업로드가 있으면,
  *             + 로컬 상태가 이동 플래그를 가졌으면 d에 서버측 이동 플래그(FILE_NEED_SERVER_MOVE)를 더하고,
  *             + 아니고, 서버 상태가 이동 플래그를 가졌으면 d에 로컬측 이동 플래그(FILE_NEED_LOCAL_MOVE)를 더한다.
  *           + d값에 다운로드가 있으면,
  *             + 서버 상태가 이동 플래그를 가졌으면 d에 로컬측 이동 플래그(FILE_NEED_LOCAL_MOVE)를 더하고,
  *             + 아니고, 로컬 상태가 이동 플래그를 가졌으면 d에 서버측 이동 플래그(FILE_NEED_SERVER_MOVE)를 더한다.
  *     .
  * @n I. d값에 업로드 또는 다운로드(FILE_SAME_MOVED)가 있으면,
  *     - 파일의 크기를 구하고, 결과 파일명을 구한다.
  *     - IsSyncExcludeFileCondition() 메소드로 동기화 제외 파일 여부를 체크하여 제외 파일이면,
  *       + 결과를 FILE_SYNC_OVER_FILE_SIZE_LIMIT로 하고 작업을 마무리한다.
  *     .
  * @n J. d값에 로컬 이동 또는 다운로드가 있으면,
  *     - 결과 파일명의 길이가 제한을 넘으면 결과를 FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH로 하고 작업을 마무리한다.
  *     .
  * @n K. d값에 업로드 또는 다운로드가 있고, 서버 상태가 다시 변경(ITEM_STATE_RE_UPDATED)인 경우에,
  *     - 결과를 FILE_SYNC_SERVER_MODIFIED_AFTER_COMPARE로 하고 작업을 마무리한다.
  *     .
  * @n L. d값에 업로드 또는 다운로드가 있고, 로컬 상태가 다시 변경(ITEM_STATE_RE_UPDATED)인 경우에,
  *     - 결과를 FILE_SYNC_LOCAL_MODIFIED_AFTER_COMPARE로 하고 작업을 마무리한다.
  *     .
  * @n M. d값에 양쪽 이동(FILE_SAME_MOVED)이면,
  *     - updateMetaAsRenamed() 호출하여 이름 변경된 내용을 메타 데이터에 반영하고
  *     - 결과에서 이동 및 양쪽 수정충돌을 제거하고, meta_mask에 META_MASK_FILENAME을 셋트한다(메타 데이터에서 FILENAME 부분만 업데이트 함)
  *     .
  * @n N. d값에 한쪽 이동이 있으면,
  *     - d값에 서버측 이동이 있으면,
  *       + d값에 로컬 수정 서버 삭제 충돌이면 rename_need를 FALSE로 하고(실제 이름 변경 처리는 하지 않음), able을 TRUE로 한다.
  *       + 아니면, 
  *         + able은 업로드 가능 조건(사용자 드라이브 정책에 따른 조건)으로 하고,
  *         + 서버측 파일 권한값을 받아오고, CheckPermissionFileRename()으로 권한 체크한 결과를 able에 셋트한다.
  *       + able이 TRUE이면, 
  *         + 실제 rename이 필요한 상황인지 체크하고 renameServerObjectAs() 메소드를 호출한다.
  *         + 위의 결과가 성공이면 
  *           + 결과값에서 (FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)를 지운다.
  *           + rename_need가 TRUE이고 로컬 파일이 수정 상태가 아니면 
  *             + 결과 값에서 FILE_NEED_UPLOAD를 지운다.
  *           + 아니면
  *             + 로컬 상태에 ITEM_STATE_UPDATED를 더한다.
  *       + able이 FALSE이면, 
  *         + 결과값을 FILE_SYNC_NO_RENAME_PERMISSION로 한다.
  *     - d값에 로컬측 이동이 있으면,
  *       + d값에 로컬 삭제 서버 수정 충돌이면 rename_need를 FALSE로 하여 renameLocalObjectAs()를 호출하고(실제 이름 변경 처리는 하지 않음), 
  * 결과값에서 (FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)를 뺀다.
  *       + 아니면,
  *         + able은 업로드 가능 조건(사용자 드라이브 정책에 따른 조건)으로 하고,
  *           + able이 TRUE이면,
  *             + 로컬 파일이 존재하면
  *               + renameLocalObjectAs() 메소드를 호출하여 로컬 파일의 이름 변경을 처리하고 결과가 성공이면,
  *     .
  * 결과값에서 (FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)를 빼고,
  *                 + 서버 상태가 수정되지 않았고, 로컬 상태도 수정(ITEM_STATE_UPDATED)되지 않았으면 결과에서 FILE_NEED_DOWNLOAD를 뺀다.
  *                 + 아니면 서버 상태에 ITEM_STATE_UPDATED를 더한다.
  *                 + meta_mask에 META_MASK_FILENAME을 셋트한다(메타 데이터에서 FILENAME 부분만 업데이트 함)
  *             + 로컬 파일이 존재하지 않으면
  *                 + 결과값에서 FILE_NEED_LOCAL_MOVE를 뺀다.
  *           + able이 FALSE이면,
  *             + 결과값을 FILE_SYNC_NO_RENAME_PERMISSION로 한다.
  *     - 결과값에 동기화 오류가 있으면 작업을 마무리한다.
  *     .
  * @n O. d값에 업로드가 있고,로컬측이 수정/추가되었거나 서버측이 삭제된 경우에,
  *     - 동기화 플래그가 업로드 가능하면 CheckPermissionFileUpload(), 또는 CheckPermissionFileInsert() 메소드로 업로드 가능한지 체크하고,
  *     - 업로드 가능하면
  *       + 서버가 존재하지 않으면 서버측 정보를 클리어하고(새 파일로 등록되도록)
  *       + uploadFileItem() 메소드로 업로드를 시작한다.
  *     - 업로드 가능하지 않으면 결과를 FILE_SYNC_NO_UPLOAD_PERMISSION로 한다.
  *     - 로그를 저장하고, d에서 FILE_NEED_UPLOAD를 클리어한다.
  *     - 업로드 시작이 성공하면 SetItemSynchronizing()을 호출하여 동기화 상태임을 설정하고, FILE_SYNC_THREAD_WAIT으로 리턴한다.
  *     .
  * @n P. 아니면 d값에 다운로드가 있고, 서버측이이 수정/추가되었거나 로컬측이 삭제된 경우에,
  *     - 동기화 플래그가 다운로드 가능하면 CheckPermissionFileDownload() 메소드로 다운로드 가능한지 체크하고,
  *     - 다운로드 가능하면 downloadFileItem() 메소드로 다운로드를 시작한다.
  *     - 다운로드 가능지 않으면 결과를 FILE_SYNC_NO_DOWNLOAD_PERMISSION로 한다.
  *     - 로그를 저장하고, d에서 FILE_NEED_DOWNLOAD를 클리어한다.
  *     - 다운로드 시작이 성공하면 SetItemSynchronizing()을 호출하여 동기화 상태임을 설정하고, FILE_SYNC_THREAD_WAIT으로 리턴한다.
  *     .
  * @n Q. d값에 다운로드가 있고, 서버 상태값이 0이고, phase가 PHASE_CONFLICT_FILES인 경우에, 결과를 FILE_SYNC_DOWNLOAD_FAIL로 한다.
  * @n R. d값에 로컬 삭제(FILE_NEED_LOCAL_DELETE)가 있으면,
  *     - 동기화 플래그가 다운로드 가능하면
  *       + 로컬측 결과 경로명을 준비하고, KSyncCoreBase::SyncLocalDeleteFile() 메소드를 호출하여 로컬측 파일을 삭제한다.
  *       + 삭제가 실패하면 로그를 저장하고,
  *       + 성공하면, mpLocalRenamedAs 정보를 클리어하고, 로컬측 정보를 클리어하고, 히스토리에 작업내용 저장한다.
  *     - 동기화 플래그가 다운로드 가능지 않으면, 결과를 FILE_SYNC_NO_DELETE_LOCAL_FILE_PERMISSION로 한다.
  *     .
  * @n S. d값에 서버 삭제(FILE_NEED_LOCAL_DELETE)가 있으면,
  *     - 동기화 플래그가 업로드 가능하면, CheckPermissionFileDelete() 메소드 호출하여 삭제 가능한지 체크한다.
  *       + 삭제가 가능하면 
  *         + 서버측 OID가 없으면 결과를 R_SUCCESS로 설정하고,
  *         + 아니면 KSyncCoreBase::SyncServerDeleteFile()를 호출하여 서버측 파일을 삭제한다.
  *         + 삭제에 성공하면 mpServerRenamedAs를 해제하고, 새로운 서버측 정보(mServerNew)를 모두 클리어한다.
  *         + 삭제에 실패하면 결과를 FILE_SYNC_SERVER_REMOVE_FAIL로 한다.
  *       + 아니면 결과를 FILE_SYNC_NO_DELETE_SERVER_FILE_PERMISSION로 한다.
  *     - 성공하였으면 히스토리에 작업 내용을 저장한다.
  *     .
  * @n T. d값에 로컬 삭제 또는 서버 삭제가 있고, 결과가 성공하지 못하였으면 d값에서 FILE_NEED_RETIRED를 뺀다.
  * @n U. 결과값이 성공이면(R_SUCCESS)
  *     - phase가 PHASE_COPY_FILES이면 로컬 상태와 서버 상태를 0으로 한다.
  *     - phase가 PHASE_LOAD_FILES이면,
  *     - d에 결과값을 받아오고, r = d & ~(FILE_SAME_MOVED| FILE_NEED_LOCAL_MOVE| FILE_NEED_SERVER_MOVE)로 한다.
  *     - 서버측 오류 메시지 버퍼를 클리어 한다.
  *     .
  * @n V. 결과값이 성공이 아니면
  *     - d값이 충돌이나 오류이면
  *       + 새로운 서버 정보를 업데이트하고, meta_mask 저장할 플래그를 셋트한다.
  *     .
  * @n W. 결과값 r을 결과 및 충돌 결과값으로 설정한다.
  * @n X. 결과값 r이 성공이 아니면 히스토리에 저장한다.
  * @n Y. 메타 삭제이면 로그에 retire 메시지를 저장하고, 
  * @n Z. StoreMetafile() 메소드로 메타 데이터를 저장하거나 삭제한다.
  * @n AA. 메타 삭제가 아니면 KSyncCoreBase::ShellNotifyIconChangeOverIcon() 메소드를 호출하여 탐색기 아이콘 업데이트를 요청한다.
  */
  virtual int doSyncItem(KSyncPolicy *policy, LPCTSTR baseFolder, int phase);

  virtual BOOL OnDeleteServerItem(BOOL r, LPCTSTR pError);

  /**
  * @brief 서버측 정보를 받아서 mServer struct에 저장한다(size, time, permission, fullpath, modifiedUser)
  * @return int : 성공하면 TRUE
  * @details A. KSyncCoreBase::GetServerDocumentSyncInfo() 메소드로 서버측 문서 정보를 mServer에 업데이트한다.
  */
  virtual BOOL RefreshServerState();

  /**
  * @brief 동기화 진행하기 바로 전에 서버측 정보를 다시 체크해서 비교 이후에 다시 변화가 생겼는지 검사함
  * @param serverInfo : 새로운 서버 정보를 저장할 버퍼
  * @return int : 다시 수정되었으면 ITEM_STATE_UPDATED를 리턴한다.
  * @details A. 부모 객체의 fullPathIndex가 변경되었으면, 로그를 저장하고, ITEM_STATE_LOCATION_CHANGED를 리턴한다.
  * @n B. mServerOID가 비어 있으면(아직 서버에 존재하지 않음) 0을 리턴한다.
  * @n C. 이 객체가 폴더이면,
  *     - KSyncCoreBase::GetServerFolderSyncInfo() 메소드로 서버측 정보를 받아오고, 성공하면
  *       + 기존의 이름과 새로 받아온 이름을 비교하여 다르면,
  *         + 결과값을 ITEM_STATE_UPDATED로 한다.
  *       + 아니면, 기존의 fullPathIndex와 새로 받아온 fullPathIndex를 비교하여 달라졌으면,
  *         + 결과값을 ITEM_STATE_LOCATION_CHANGED로 한다.
  *     - 실패하면(폴더가 삭제되거나 액세스 실패됨) 삭제되었는지 체크하고
  *     .
  * @n D. 폴더가 아니면, 
  *     - KSyncCoreBase::GetServerDocumentSyncInfo() 메소드로 서버측 정보를 받아오고, 성공하면
  *       + FileOID, StorageOID, Filename 의 기존의 값과 새로 받아온 값을 비교하여 다르면, 
  *         + 결과값을 ITEM_STATE_UPDATED로 한다.
  *     - 실패하면(폴더가 삭제되거나 액세스 실패됨) 삭제되었는지 체크한다.
  *     .
  * @n E. 결과가 ITEM_STATE_UPDATED이면, 로그에 내용을 전달하고,
  * @n F. 결과를 리턴한다 
  */
  int CheckServerModifiedAgain(SERVER_INFO& serverInfo);

  /**
  * @brief 서버측 정보를 다운받아서 mServerNew struct에 저장한다(size, time, permission, fullpath, modifiedUser)
  * @return int : 상위 폴더의 위치가 바뀌었으면 ITEM_STATE_LOCATION_CHANGED를, 
  * 서버측 정보가 바뀌었으면 ITEM_STATE_UPDATED를 리턴하고 변경되지 않았으면 0을 리턴한다.
  */
  virtual int RefreshServerNewState();

  /**
  * @brief 이 개체의 타입이 지정한 타입과 일치하는지 검사함.
  * @return BOOL : 타입이 맞으면 TRUE
  * @param type : 동기화 개체 type(0=파일, 1=폴더, 2=루트 폴더)
  */
  BOOL IsMatchType(int type);

  /**
  * @brief 대상 item 개체의 내용을 이 개체에 merge한다.(현재에는 별다른 동작 없음)
  * @return void
  * @param item : 머지할 개체
  */
  virtual void MergeItem(KSyncItem *item);

  /**
  * @brief 이 항목이 삭제될 예정이면 TRUE 리턴함.
  * @return BOOL
  * @param phase : 동기화 단계(검색, 비교, 폴더 이동/이름변경, 파일 업/다운)
  * @details A. phase가 PHASE_COPY_FILES이면 결과값을 가져오고,
  * @n B. phase가 PHASE_CONFLICT_FILES이면 충돌 결과값을 가져오고,
  * @n B. 로컬측 또는 서버측 삭제 플래그가 있으면 TRUE를 리턴한다.
  */
  virtual BOOL willBeDeleted(int phase);

  /**
  * @brief 충돌 처리 화면에서 대상 항목의 충돌 처리 방식을 다음 것으로 토글(업로드/다운로드/삭제/없음)
  * 토글이 성공하면 TRUE를 리턴함. 2.0에서는 사용 안됨.
  * @return BOOL
  */
  BOOL ToggleConflictResult();

  /**
  * @brief 대상항목이 로컬에서 또는 서버에서 이름변경 또는 이동되었는지를 검사하고 결과를 리턴함.
  * @return int(ITEM_STATE_MOVED, ITEM_STATE_RENAMED mask)
  * @param onLocal : 로컬측 또는 서버측
  * @details A. onLocal이 TRUE이고 로컬 상태가 이동됨이면 newpathname에 로컬측 결과 경로명을 준비하고,
  * @n B. onLocal이 FALSE이고 서버 상태가 이동됨이면 newpathname에 서버측 결과 경로명을 준비한다.
  * @n C. newpathname이 유효하면
  *       - 기존의 경로명(mpRelativePath)과 newpathname의 경로가 다른지 검사하고 다르면,
  *         + 경로와 파일명을 분리하여 비고하고, 경로가 달라졌으면 ITEM_STATE_MOVED를, 이름이 달라졌으면
  * ITEM_STATE_RENAMED를 상태값에 더하여 리턴한다.
  */
  int GetItemStateMovedRenamed(BOOL onLocal);

  /**
  * @brief 대상항목의 충돌 오류값을 리턴함.
  * @return int : 충돌 및 오류 코드
  */
  inline int GetConflictResult() { return mConflictResult; }
  /**
  * @brief 대상항목의 충돌 오류값을 설정함.
  * @return void
  * @param r : 충돌 및 오류 코드
  */
  inline void SetConflictResult(int r) { mConflictResult = r; }

  /**
  * @brief 대상항목의 충돌 오류값에 사용자 선택 action을 설정함.
  * @return void
  * @param r : 적용할 action 값
  * @param flag : WITH_RECURSE=하위 항목에도 적용함
  */
  virtual void SetConflictResultAction(int r, int flag);

  /**
  * @brief 대상항목의 충돌 오류값에서 사용자 선택 action을 index로 변환하여 리턴함.
  * @return void
  */
  int GetConflictResultMethod();

  /**
  * @brief 동기화 작업중 서버측에서 발생된 오류 메시지를 설정함
  * @return LPCTSTR
  */
  void SetServerConflictMessage(LPCTSTR msg);

  /**
  * @brief 서버측 정보를 모두 제거함.
  * @return void
  */
  void clearServerSideInfo();
  /**
  * @brief 로컬측 정보를 모두 제거함.
  * @return void
  */
  void clearLocalSideInfo();
  /**
  * @brief 이름 바꾸기 또는 위치 이동 정보를 제거함.
  * @return void
  */
  void clearRenameInfo();

  /**
  * @brief 동기화 충돌 상태 해결을 위해 로컬측 파일의 이름을 바꾸면 기존 개체와 새로 이름을 바꾼
  * 개체 두개가 생기게 되며, 이 상황에 따라 이 개체는 이름 바뀐 개체로 새로 등록되도록 하고(서버측 정보는
  * 삭제하고 로컬측은 바뀐 이름으로 새로 등록된 항목으로 처리), 상대 개체(pair)는 
  * 서버측에 새로 생성된 개체로 처리(로컬측 정보는 삭제하고 서버측은 새로 등록된 항목으로 처리)
  * @param root : 동기화 루트 폴더 항목
  * @param pair : 새로 생성된 개체
  * @return void
  * @details A. 로컬 상태가 이동됨이면,
  *     - ChangeRelativePathName() 메소드를 호출하여 경로명을 mpLocalRenamedAs로 바꾼다.
  *     .
  * @n B. 아니면 UpdateRelativePath() 메소드를 호출하여 경로명을 업데이트한다.
  * @n C. 서버측 정보를 지운다.
  * @n D. 이름 변경 및 이동 정보를 지운다.
  * @n E. 로컬 상태를 (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)로 한다.
  * @n F. 서버 상태를 0으로 한다.
  * @n G. 충돌 결과값을 FILE_NEED_UPLOAD|FILE_SYNC_RENAMED로 한다.
  * @n H. 결과값을 FILE_NEED_UPLOAD로 한다.
  * @n I. 충돌 오류 메시지를 클리어한다
  * @n J. 동기화 Enable을 TRUE로 한다.
  * @n K. StoreMetafile() 메소드로 메타 데이터를 저장한다.
  * @n L. pair 항목이 NULL이 아니면 pair 항목에서,
  *     - 현재의 경로를 lastPath에 저장해 둔다.
  *     - 서버측 상태가 이동됨이면, mpServerRenamedAs로 경로명을 바꾸고,
  *     - 아니면 UpdateRelativePath() 메소드를 호출하여 경로명을 업데이트한다.
  *     - 새 서버 정보를 서버 정보로 복사(mServer = mServerNew)한다.
  *     - 로컬 상태를 0으로 한다.
  *     - 서버 상태를 (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)로 한다.
  *     - 충돌 결과값을 FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED로 한다.
  *     - 결과값을 FILE_NEED_DOWNLOAD로 한다.
  *     - 충돌 오류 메시지를 클리어한다
  *     - 로컬측 정보를 클리어한다.
  *     - 이름 변경 및 이동 정보를 클리어한다.
  *     - 동기화 Enable을 TRUE로 한다.
  *     - StoreMetafile() 메소드로 메타 데이터를 저장한다.
  */
  virtual void CloneLocalRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief 동기화 충돌 상태 해결을 위해 서버측 파일의 이름을 바꾸면 기존 개체와 새로 이름을 바꾼
  * 개체 두개가 생기게 되며, 이 상황에 따라 이 개체는 이름 바뀐 개체로 새로 등록되도록 하고(로컬측 정보는
  * 삭제하고 서버측은 바뀐 이름으로 새로 등록된 항목으로 처리), 상대 개체(pair)는 
  * 로컬측에 새로 생성된 개체로 처리(서버측 정보는 삭제하고 로컬측은 새로 등록된 항목으로 처리)
  * @param root : 동기화 루트 폴더 항목
  * @param pair : 새로 생성된 개체
  * @details A. 서버 상태가 이동됨이면,
  *     - ChangeRelativePathName() 메소드를 호출하여 경로명을 mpServerRenamedAs로 바꾼다.
  *     .
  * @n B. 아니면 UpdateRelativePath() 메소드를 호출하여 경로명을 업데이트한다.
  * @n C. 로컬측 정보를 지운다.
  * @n D. 이름 변경 및 이동 정보를 지운다.
  * @n E. 서버 상태를 (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)로 한다.
  * @n F. 로컬 상태를 0으로 한다.
  * @n G. 충돌 결과값을 FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED로 한다.
  * @n H. 충돌 결과값을 FILE_NEED_DOWNLOAD로 한다.
  * @n I. 충돌 오류 메시지를 클리어한다
  * @n J. 동기화 Enable을 TRUE로 한다.
  * @n K. StoreMetafile() 메소드로 메타 데이터를 저장한다.
  * @n L. pair 항목이 NULL이 아니면 pair 항목에서,
  *     - 현재의 경로를 lastPath에 저장해 둔다.
  *     - 로컬측 상태가 이동됨이면, mpLocalRenamedAs로 경로명을 바꾸고,
  *     - 아니면 UpdateRelativePath() 메소드를 호출하여 경로명을 업데이트한다.
  *     - 로컬측 상태가 삭제됨이면,
  *       + 로컬 상태를 ITEM_STATE_DELETED으로 한다.
  *       + 서버 상태를 0으로 한다.
  *       + 충돌 결과값을 FILE_NEED_RETIRED|FILE_SYNC_RENAMED로 한다.
  *       + 결과값을 FILE_NEED_RETIRED로 한다.
  *     - 아니면,
  *       + 로컬 상태를 (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_WAS_EXIST|ITEM_STATE_CONFLICT_RENAMED)으로 한다.
  *       + 서버 상태를 0으로 한다.
  *       + 충돌 결과값을 FILE_NEED_UPLOAD|FILE_SYNC_RENAMED로 한다.
  *       + 결과값을 FILE_NEED_UPLOAD로 한다.
  *     - 서버측 정보를 클리어한다.
  *     - 이름 변경 및 이동 정보를 클리어한다.
  *     - 충돌 오류 메시지를 클리어한다
  *     - 동기화 Enable을 TRUE로 한다.
  *     - StoreMetafile() 메소드로 메타 데이터를 저장한다.
  *     .
  * @return void
  */
  virtual void CloneServerRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief 동기화 충돌 상태 해결을 위해 로컬측과 서버측을 별도의 개체로 분리함.
  * 이 개체는 로컬에 새로 추가된 파일로 처리하도록 서버측 정보를 삭제하고,
  * 상대 개체(pair)는 서버에 새로 추가된 파일로 처리하도록 로컬측 정보를 삭제한다.
  * @return void
  * @details A. StoreMetafile(root->GetStorage(), 0, METAFILE_DELETE)으로 메타 데이터를 삭제한다.
  * @n B. 로컬측 상태가 이동됨이면, mpLocalRenamedAs로 경로명을 바꾸고, 아니면 UpdateRelativePath() 메소드를 호출하여 경로명을 업데이트한다.
  * @n C. 서버측 정보를 지운다.
  * @n D. 로컬 상태를 (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)로 한다.
  * @n E. 서버 상태를 0으로 한다.
  * @n F. 충돌 결과값을 FILE_NEED_UPLOAD|FILE_SYNC_RENAMED로 한다.
  * @n G. 결과값을 FILE_NEED_UPLOAD로 한다.
  * @n H. 충돌 오류 메시지를 클리어한다
  * @n I. 이름 변경 및 이동 정보를 지운다.
  * @n J. 동기화 Enable을 TRUE로 한다.
  * @n K. StoreMetafile() 메소드로 메타 데이터를 저장한다.
  * @n L. pair 항목이 NULL이 아니면 pair 항목에서,
  *     - 현재의 경로를 lastPath에 저장해 둔다.
  *     - 서버측 상태가 이동됨이면, mpServerRenamedAs로 경로명을 바꾸고, 새 서버 정보를 서버 정보로 복사(mServer = mServerNew)한다.
  *     - 아니면 UpdateRelativePath() 메소드를 호출하여 경로명을 업데이트한다.
  *     - 로컬 상태를 0으로 한다.
  *     - 서버 상태를 (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)로 한다.
  *     - 충돌 결과값을 FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED로 한다.
  *     - 결과값을 FILE_NEED_DOWNLOAD로 한다.
  *     - 충돌 오류 메시지를 클리어한다
  *     - 로컬측 정보를 클리어한다.
  *     - 이름 변경 및 이동 정보를 클리어한다.
  *     - 동기화 Enable을 TRUE로 한다.
  *     - StoreMetafile() 메소드로 메타 데이터를 저장한다.
  *     .
  */
  virtual void CloneSeparate(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief 동기화 된 개체이면 (서버측 OID를 가지고 있으면 이미 동기화 된 것임) TRUE를 리턴한다.
  * @return BOOL
  */
  BOOL IsAlreadySynced();

  /**
  * @brief 서버측 상대 경로 및 파일명을 새 메모리 버퍼에 할당하여 복사한 후 리턴함.
  * @return LPTSTR : 서버측 상대 경로명
  */
  LPTSTR AllocServerSidePathName();

  /**
  * @brief 서버측에 파일 업로드 작업을 시작한다.
  * @return int : 성공하면 R_SUCCESS를, 아니면 에러 코드를 리턴한다.
  * @details A. 로컬측의 전체 경로명을 준비한다.
  * @n B. 위의 파일이 존재하지 않으면 FILE_SYNC_UPLOAD_FAIL을 리턴한다.
  * @n C. 서버측 FileOID, StorageOID가 유효하면(서버에 이미 존재하는 파일임)
  *     - KSyncCoreEngine::CheckOutDocument()메소드를 호출하여 해당 파일의 CheckOut을 요청한다.
  *     - CheckOut이 실패하면 FILE_SYNC_ERROR_CHECKOUT를 리턴한다.
  *     .
  * @n D. 서버측 FileOID, StorageOID를 클리어하고
  * @n E. KSyncCoreEngine::StartUploadFile() 메소드를 호출하여 업로드를 시작한다.
  */
  int uploadFileItem();

  /**
  * @brief 상대 경로명을 비교하여 메모리상에서 이 개체의 이동 및 이름변경을 처리한다.
  * @return BOOL : 경로명이 바뀌었으면 TRUE를 리턴한다.
  * @details A. 현재의 상대 경로명을 복사해두고 동기화 결과 경로명을 버퍼에 준비한다.
  * @n B. isPathChanged()메소드로 기존의 폴더와 현재의 폴더를 비교하여, 경로가 바뀌었으면
  *     - 새로운 경로(이 개체의 부모 폴더)를 준비하고 KSyncRootFolderItem::MoveSyncItem()메소드를
  * 호출하여 이 개체를 새로운 경로의 KSyncFolderItem의 child로 이동한다.
  *     .
  * @n C. 기존의 경로명과 현재의 경로명을 비교하여 다르면,
  *     - MoveMetafileAsRenamed()메소드를 호출하여 새로운 경로명으로 메타 데이터가 저장되도록 한다.
  */
  BOOL checkRelativeNameChanged();

  /**
  * @brief 서버측의 파일 다운로드 작업을 시작한다.
  * @return int : 성공하면 R_SUCCESS를, 아니면 에러 코드를 리턴한다.
  * @details A. 서버측의 전체 경로명을 준비한다.
  * @n B. KSyncCoreEngine::StartDownloadItemFile() 메소드를 호출하여 해당 파일의 다운로드를 시작한다.
  * @n C. 리턴값에 따라 동기화 오류값을 리턴한다.
  */
  int downloadFileItem();

  /**
  * @brief 업로드가 완료된 후 업로드에서 리턴받은 FileOID, StorageOID 및 기타 정보를 서버 문서에 반영한다.
  * @param filename : 업로드된 파일명
  * @param serverStorageOID : 업로드된 storageOID
  * @return BOOL : 성공하였으면 TRUE를 리턴한다.
  * @details A. 받은 storageOID를 이 개체의 mServerInfo에 복사한다.
  * @n B. mServerNew의 FileOID를 mServerInfo에 복사한다.
  * @n C. KSyncCoreEngine::UpdateUploadDocumentItem메소드를
  * 호출하여 이 개체를 FileOID, StorageOID 및 기타 정보를 서버측에 반영한다.
  */
  BOOL UpdateDocumentItemOID(LPCTSTR filename, LPCTSTR serverStorageOID);

  /**
  * @brief 업로드 또는 다운로드가 완료된 후 호출되는 함수이다.
  * @param isDown : TRUE이면 다운로드 작업인 경우
  * @param result : 업로드/다운로드 결과값
  * @param baseFolder : 동기화 루트 폴더.
  * @param conflictMessage : 서버측 오류 메시지
  * @return int : 성공하면 R_SUCCESS를, 실패하면 충돌 오류값을 리턴한다.
  * @details A. result 결과값에 따라 충돌 오류값을 설정한다.
  * @n B. checkRelativeNameChanged() 메소드를 호출하여 이름 변경된 경우에 상대 경로명이 업데이트 되도록 한다.
  * @n C. 결과값이 성공이면
  *     - 다운로드이면
  *       + RefreshLocalNewFileInfo()함수를 호출하여 로컬측 정보를 mLocalNew에 갱신한다.
  *     - UpdateLocalNewInfo() 함수를 호출하여 mLocalNew 정보를 mLocal로 옮긴다.
  *     - UpdateServerNewInfo() 함수를 호출하여 mServerew 정보를 mServer로 옮긴다.
  *     - 상태값을 정상으로 바꾸고 결과값을 초기화하고 이름변경 정보도 클리어한다.
  *     .
  * @n C. 결과값이 성공이 아니면, 오류 메시지를 준비하고
  * @n D. 메시지 내용을 로그에 저장하고
  * @n E. StoreHistory()메소드를 호출하여 히스토리에 작업 내용을 저장한다.
  * @n F. StoreMetafile()메소드를 호출하여 메타 데이터를 저장한다.
  * @n G. SetItemSynchronizing()메소드를 호출하여 동기화 진행중 상태를 클리어한다.
  * @n H. 업로드/다운로드 작업이 실패한 경우에,
  *     - 동기화 결과 경로명을 준비하여 해당 파일이 없으면 부모 폴더의 KSyncFolderItem::CheckLocalFolderExistOnSyncing() 메소드를
  * 호출하여 로컬에 부모 폴더가 존재하지 않는 경우 다른 파일들의 동기화 작업 시도를 멈추도록 한다.
  *     - 부모 폴더의 KSyncFolderItem::CheckServerFolderExistOnSyncing() 메소드를 
  * 호출하여 서버에 부모 폴더가 존재하지 않는 경우 다른 파일들의 동기화 작업 시도를 멈추도록 한다.
  */
  int OnLoadDone(BOOL isDown, int result, LPCTSTR baseFolder, LPCTSTR conflictMessage);

  /**
  * @brief 업로드 또는 다운로드가 취소된 후 호출되는 함수이다.
  * @param baseFolder : 동기화 루트 폴더
  * @details A. SetItemSynchronizing()메소드를 호출하여 동기화 진행중 상태를 클리어한다.
  */
  void OnLoadCanceled(LPCTSTR baseFolder);

  /**
  * @brief 동기화 비교 단계에서 비교된 이후 파일 동기화(업로드/다운로드/삭제)가 처리되기 이전에 수정되었는지 확인하는 함수이다.
  * @param filename : 파일의 경로명
  * @return BOOL : 다시 수정되었으면 TRUE를 리턴한다.
  * @details A. GetFileModifiedTime() 함수로 파일의 최종 수정 시간을 받아온다.
  * @n B. 로컬 정보(mLocalNew 또는 mLocal)의 파일 시간과 위의 시간을 비교하여 달라졌으면, 
  * 결과 및 충돌 오류를 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED로 하고, TRUE를 리턴한다.
  */
  BOOL CheckModifiedAfterSyncCompare(LPCTSTR filename);

  /**
  * @brief 서버측 파일의 이동 및 이름변경을 처리한다.
  * @param renameObject : TRUE이면 해당 파일을 실제로 이동 및 이름 변경을 하고, 아니면 이미 이름 변경된 것으로 간주하고 정보를 업데이트한다.
  * @return int : 성공하면 R_SUCCESS를, 아니면 오류 코드를 리턴한다.
  * @details A. 로컬측 상대 경로명과 서버측 상대 경로명을 준비한다.
  * @n B. 두 경로명에서 경로와 이름을 분리하여 경로가 바뀌었는지 확인한다.
  * @n C. renameObject가 TRUE이면,
  *     - 경로가 바뀌었으면
  *       + 새로운 경로에 해당하는 부모 폴더 KSyncFolderItem을 구한다.
  *       + 새 부모 폴더에 동일한 이름이 있으면,
  *         + 동일한 이름의 파일이 최종적 이름이면 결과를 FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS로 하고 빠져나간다.
  *         + 임시 이름을 준비하고, KSyncCoreEngine::ChangeDocumentName()으로 임시 이름으로 바꾼다.
  *       + 현재 부모 폴더의 권한과 경로, 새 부모 폴더의 권한과 경로를 KSyncCoreEngine::GetServerFolderSyncInfo()를 통해 받아온다.
  *       + CheckPermissionFileMove()를 통해 파일을 다른 폴더로 이동할 수 있는 권한이 있는지 확인한다.
  *       + 권한이 있으면
  *         + KSyncCoreEngine::MoveItemTo()를 통해 대상 파일을 이동한다.
  *       + 권한이 없으면 결과를 FILE_SYNC_NO_MOVE_PERMISSION로 하고 빠져나간다.
  *     - 이름이 바뀌었으면,
  *       + CheckPermissionFileRename() 메소드로 파일 이름을 바꿀 권한이 있는지 확인하고,
  *         + 권한이 있으면,
  *           + 부모 폴더에 이미 동일한 이름의 파일이 존재하면,
  *             + 존재하는 파일이 최종적인 파일명이면(이름 변경 또는 이동이 되지 않았으면), 
  * 결과를 FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS로 하고 빠져 나간다.
  *             + 아니면 임시 이름을 준비한다.
  *           + KSyncCoreEngine::ChangeDocumentName()으로 이름을 바꾼다.
  *         + 권한이 없으면, 결과를 FILE_SYNC_NO_RENAME_PERMISSION로 하고 빠져 나간다.
  *     .
  * @n D. renameObject가 FALSE이면, 성공한 것으로 간주하고,
  * @n E. 위의 작업이 성공하였으면,
  *     - MoveMetafileAsRename()메소드를 호출하여 메타 데이터의 경로명을 업데이트한다.
  *     - KSyncRootFolderItem::RemoveMovedRenamedOriginal() 메소드를 호출하여 RenameDB에서 이 항목의 이름변경 정보를 클리어한다.
  *     - clearRenameInfo()메소드로 이 개체의 이름 변경 정보를 클리어한다.
  *     .
  * @n F. renameObject가 TRUE이면 StoreHistory() 메소드로 작업 내용을 기록한다.
  */
  int renameServerObjectAs(BOOL renameObject);

  /**
  * @brief 서버측 파일의 이동 및 이름변경을 처리한다.
  * @param renameObject : TRUE이면 해당 파일을 실제로 이동 및 이름 변경을 하고, 아니면 이미 이름 변경된 것으로 간주하고 정보를 업데이트한다.
  * @return int : 성공하면 R_SUCCESS를, 아니면 오류 코드를 리턴한다.
  * @details A. 로컬측 상대 경로명과 서버측 상대 경로명을 준비한다.
  * @n B. 두 경로명에서 경로와 이름을 분리하여 경로가 바뀌었는지 확인한다.
  * @n C. renameObject가 TRUE이면,
  *     - 새로운 경로에 해당하는 부모 폴더 KSyncFolderItem을 구한다.
  *       + 새 부모 폴더에 동일한 이름이 있고,
  *         + 최종적 이름이면 결과를 FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS로 하고 빠져나간다.
  *         + 아니면 임시 이름을 준비한다.
  *     - KMoveFile()로 이동 및 이름 변경을 하고, 임시 이름이면 SetLocalTemporaryRenamed()로 원래 이름을 저장하여 둔다.
  * 이는 동기화 마지막 단계에서 다시 원래 이름으로 바꾸는 작업을 한다.
  *     .
  * @n D. renameObject가 FALSE이면 성공한 것으로 한다.
  * @n E. 위의 작업이 성공하였으면,
  *     - 경로가 바뀌었으면 KSyncRootFolderItem::MoveSyncItem()메소드로 이 개체를 해당 폴더 아래로 이동한다.
  *     - MoveMetafileAsRename()메소드를 호출하여 메타 데이터의 경로명을 업데이트한다.
  *     - KSyncRootFolderItem::RemoveMovedRenamedOriginal() 메소드를 호출하여 RenameDB에서 이 항목의 이름변경 정보를 클리어한다.
  *     - clearRenameInfo()메소드로 이 개체의 이름 변경 정보를 클리어한다.
  *     .
  * @n F. 실패하였으면, GetLastError()로 시스템 오류 코드를 받아서 이에 대한 동기화 오류 코드를 리턴값으로 준비한다.
  * @n G. renameObject가 TRUE이면 StoreHistory() 메소드로 작업 내용을 기록한다.
  */
  int renameLocalObjectAs(BOOL renameObject);

  /**
  * @brief 로컬측의 이동 및 이름 변경된 내용에 따라 메타 데이터를 업데이트한다.
  * @details A. 기존의 상대 경로명과 현재의 로컬측 상대 경로명을 준비한다.
  * @n B. 두 경로명에서 경로와 이름을 분리하여 경로가 바뀌었는지 확인한다.
  * @n C. 경로가 바뀌었으면 KSyncRootFolderItem::MoveSyncItem()메소드로 이 개체를 해당 폴더 아래로 이동한다.
  * @n D. SetRelativePathName()로 경로명을 바꾼다.
  * @n E. MoveMetafileAsRename()메소드를 호출하여 메타 데이터의 경로명을 업데이트한다.
  * @n F. clearRenameInfo()메소드로 이 개체의 이름 변경 정보를 클리어하고 로컬측 서버측 상태값에서 ITEM_STATE_MOVED를 제거한다.
  */
  void updateMetaAsRenamed();

  /**
  * @brief 파일의 이름을 지정한 이름으로 바꾸고 메타 데이터를 업데이트한다.
  * @param baseFolder : 동기화 루트 폴더 경로
  * @param name : 새로운 파일명
  * @details A. 지정한 name으로 파일명을 설정한다.
  * @n B. KSyncRootStorage::MoveMetafileAsRename() 메소드로 변경된 이름으로 메타 데이터를 업데이트한다.
  * @n C. StoreMetafile()로 메타 데이터를 저장한다.
  */
  BOOL UpdateRenameLocal(LPCTSTR baseFolder, LPCTSTR name);

  /**
  * @brief 로컬측 파일의 이름을 지정한 이름으로 바꾸고 메타 데이터를 업데이트한다.
  * @param baseFolder : 동기화 루트 폴더 경로
  * @param name : 새로운 파일명
  * @param renameNow : TRUE이면 지금 rename을 처리한다.
  * @details A. 로컬측 동기화 결과 경로명을 준비한다.
  * @n B. 준비한 경로명의 파일명 부분을 지정한 name으로 바꾼다.
  * @n C. CFile::Rename() 메소드로 로컬 파일의 이름을 바꾼다.
  * @n D. DirectoryMonitor에 저장된 이 파일의 이름변경 이벤트를 삭제한다.
  * @n E. 상대 경로명을 업데이트하고, 로컬 정보의 FileTime, AccessTime을 업데이트한다.
  * @n F. 로컬측 이름 변경 정보를 클리어한다.
  * @n G. KSyncRootStorage::MoveMetafileAsRename() 메소드로 변경된 이름으로 메타 데이터를 업데이트한다.
  */
  BOOL renameLocal(LPCTSTR baseFolder, LPCTSTR name, BOOL renameNow);

  /**
  * @brief 서버측 파일의 이름을 지정한 이름으로 바꾸고 메타 데이터를 업데이트한다.
  * @param baseFolder : 동기화 루트 폴더 경로
  * @param name : 새로운 파일명
  * @param renameNow : TRUE이면 지금 rename을 처리한다.
  * @details A. renameNow가 TRUE이면,
  *     - 폴더이면 KSyncCoreEngine::ChangeFolderName()을, 파일이면 KSyncCoreEngine::ChangeDocumentName()을
  * 호출하여 서버측 이름을 변경한다.
  *     - 변경이 완료되었으면,
  *       + 파일 이름을 지정한 이름으로 변경하고,
  *       + OnPathChanged()를 호출하여 경로를 업데이트하고,
  *       + RefreshServerState()를 호출하여 서버측 정보를 업데이트하고
  *       + CopyServerInfo()로 mServer를 mServerNew로 복사한다.
  *       + KSyncRootStorage::MoveMetafileAsRename() 메소드로 변경된 이름으로 메타 데이터를 업데이트한다.
  *       + 서버측 이름 변경 정보를 클리어한다.
  *     .
  * @n B. renameNow가 FALSE이면,
  *     - RefreshServerState()를 호출하여 서버측 정보를 업데이트하고
  *     - mServerNew.Filename을 이 개체의 이름으로 업데이트하고
  *     - OnPathChanged()를 호출하여 경로를 업데이트한다.
  *     .
  */
  BOOL renameServer(LPCTSTR baseFolder, LPCTSTR name, BOOL renameNow);

#ifdef RENAME_ON_SYNC_TIME
  TCHAR mLocalRename[KMAX_PATH];
  TCHAR mServerRename[KMAX_PATH];

  BOOL IsRenamed();
  LPCTSTR GetLocalFileName() { return ((lstrlen(mLocalRename) > 0) ? mLocalRename : mFilename); }
  LPCTSTR GetServerFileName() { return ((lstrlen(mServerRename) > 0) ? mServerRename : mFilename); }
#else

  /**
  * @brief 로컬측의 파일명을 리턴한다.
  */
  LPCTSTR GetLocalFileName();
  /**
  * @brief 서버측의 파일명을 리턴한다.
  */
  LPCTSTR GetServerFileName();
#endif

  /**
  * @brief 서버측의 파일이 임시 파일명으로 변경되어 있음을 설정한다.
  * @param realName : 원본 이름
  * @details A. 지정한 원본 이름을 mpServerTempRenamed 변수에 저장하였다가 동기화 마지막 단계에서 이름을 다시 되돌려 놓기 위해 사용된다.
  */
  void SetServerTemporaryRenamed(LPCTSTR realName);
  /**
  * @brief 로컬측의 파일이 임시 파일명으로 변경되어 있음을 설정한다.
  * @param realName : 원본 이름
  * @details A. 지정한 원본 이름을 mpLocalTempRenamed 변수에 저장하였다가 동기화 마지막 단계에서 이름을 다시 되돌려 놓기 위해 사용된다.
  * @n 'A/a.txt', 'B/a.txt' 가 존재할때, 두 파일이 양쪽으로 이동된 경우에, 'A/a.txt'를 'B/a.txt'로 옮기려 할 때, 동일한 이름을 피하기 위해,
  * 'A/a.txt'를 'A/a-tmp001.txt'로 이름을 바꾼 후, 'B/a-tmp001.txt'로 이동을 하고, 'B/a.txt'를 'A/a.txt'로 이동한 다음,
  * 마지막에 'B/a-tmp001.txt'를 'B/a.txt'로 이름을 바꾸는 방법으로 진행된다.
  */
  void SetLocalTemporaryRenamed(LPCTSTR realName);

  /**
  * @brief 서버측의 파일이 임시 파일명으로 변경되어 있으면 TRUE를 리턴한다.
  */
  inline BOOL IsServerTemporaryRenamed() { return ((mpServerTempRenamed != NULL) && (lstrlen(mpServerTempRenamed) > 0)); }

  /**
  * @brief 로컬측의 파일이 임시 파일명으로 변경되어 있으면 TRUE를 리턴한다.
  */
  inline BOOL IsLocalTemporaryRenamed() { return ((mpLocalTempRenamed != NULL) && (lstrlen(mpLocalTempRenamed) > 0)); }

  /**
  * @brief 로컬측에서 동일한 이름이 겹치지 않는 임시 파일명을 준비한다.
  * @param pathname : 현재의 경로명
  * @return LPTSTR : 새로 만든 임시 파일명.
  * @details A. 버퍼를 준비하여 현재의 경로명을 복사한다.
  * @n B. 위의 경로명에 '.AAA' 문자열을 붙인다.
  * @n C. 이름에 해당하는 파일이 존재하는지 체크하여, 없으면 이 이름을 리턴한다.
  * @n D. MakeNextSequenceName() 메소드를 호출하여, '.AAB', '.AAC' 등 순서대로 이름을 바꾼다.
  * @n E. C로 이동한다.
  */
  virtual LPTSTR ReadyUniqueLocalName(LPCTSTR pathname);

  /**
  * @brief 임시 파일명으로 변경된 항목인지 확인한다.
  * @return BOOL : 임시 파일명으로 변경되었으면 TRUE을 리턴한다.
  */
  BOOL IsTemporaryRenamed();

  /**
  * @brief 상위 폴더가 이동 처리될 것인지 확인한다.
  * @return BOOL : 상위 폴더의 이동이 예상되면 TRUE을 리턴한다.
  */
  BOOL ParentWillMove();

  /**
  * @brief 임시 파일명으로 변경된 항목을 원 이름으로 바꾼다.
  * @return int : 성공적으로 원 이름으로 바뀌었으면 1을 리턴한다.
  * @details A. 서버측이 임시 파일명으로 바뀌었으면,
  *     - KSyncCoreEngine::ChangeDocumentName()메소드로 대상 파일의 이름을 원 이름으로 바꾼다.
  *     .
  * @n B. 로컬측이 임시 파일명으로 바뀌었으면,
  *     - 현재의 경로명과 원 경로명을 준비하여, KMoveFile() 메소드로 대상 파일의 이름을 원 이름으로 바꾸고 성공하면
  *       + KSyncRootFolderItem::OnStoreCancelRenamed()메소드로 이 파일의 이름 변경 기록을 클리어한다.
  *     .
  * @n C. 위의 작업이 성공하였으면 MoveMetafileAsRename()로 변경된 경로명을 반영하고,
  * @n D. StoreMetafile() 메소드를 호출하여, 메타 데이터를 저장한다.
  */
  virtual int RestoreTemporaryRenamed();

  /**
  * @brief 로컬측 파일 또는 폴더가 존재하면 TRUE를 리턴한다.
  * @details A. 로컬측 결과 경로명을 준비하고,
  *     - 폴더이면 IsDirectoryExist()로, 파일이면 IsFileExist()로 존재 여부를 체크하여 리턴한다.
  */
  BOOL IsLocalFileExist();

  /**
  * @brief 서버측 파일 또는 폴더가 존재하면 TRUE를 리턴한다.
  * @details A. 서버측의 OID가 존재하면,
  *     - 폴더이면 TRUE를 리턴하고,
  *     - 파일이면 파일OID, 스토리지OID가 유효하면 TRUE를 리턴한다.
  */
  BOOL IsServerFileExist();

  /**
  * @brief 동기화 결과로 만들어질 상대 경로 및 이름을 메모리 할당하여 리턴한다.
  * @return LPTSTR : 동기화 결과 경로명 버퍼
  * @param flag : option flag(AS_LOCAL : 로컬측 이름, AS_NAME_ONLY : 경로는 제외하고 이름만, AS_FULLPATH : 전체 경로)
  */
  virtual LPTSTR AllocSyncResultName(int flag);

  LPTSTR AllocRenamedPathName(int flag);

  /**
  * @brief 이름 변경된 경로명을 지정한 버퍼에 저장한다.
  * @param buff : 경로명을 저장할 버퍼.
  * @param len : 경로명을 저장할 버퍼의 크기.
  * @param flag : 로컬 또는 서버측.
  * @details A. 부모 폴더가 NULL이 아니면
  *     - 부모 폴더에서 KSyncFolderItem::GetRenamedPathName()으로 경로명을 구한다
  *     .
  * @n B. flag에 AS_LOCAL값이 있고, 로컬측 이름 변경 정보가 유효하면
  *     - 버퍼에 mpLocalRenamedAs를 복사한다.
  *     .
  * @n C. flag에 AS_LOCAL값이 없고, 서버측 이름 변경 정보가 유효하면
  *     - 버퍼에 mpServerRenamedAs를 복사한다.
  *     .
  * @n D. 아니면,
  *     - 버퍼에 현재 개체의 이름을 더한다.
  */
  int GetRenamedPathName(LPTSTR buff, int len, int flag);

#if 0
  /**
  * @brief 동기화 결과로 만들어질 절대 경로 및 이름을 메모리 할당하여 리턴한다.
  * @return LPTSTR
  * @param flag : option flag(AS_LOCAL : 로컬측 이름, AS_NAME_ONLY : 경로는 제외하고 이름만)
  */
  LPTSTR AllocSyncResultFullName(int flag);

  /**
  * @brief 동기화 결과로 만들어질 상대 경로 및 이름을 받아온다.
  * @n deprecated : use AllocSyncResultName()
  * @return void
  * @param name : 이름을 받아올 버퍼
  * @param length : 이름을 받아올 버퍼의 크기(TCHAR 단위)
  * @param flag : option flag(AS_LOCAL : 로컬측 이름, AS_NAME_ONLY : 경로는 제외하고 이름만)
  */
  virtual void GetSyncResultRName(LPTSTR name, int length, int flag);
  /**
  * @brief 동기화 결과로 만들어질 절대 경로 및 이름을 받아온다.
  * @n deprecated : use AllocSyncResultName()
  * @return void
  * @param name : 이름을 받아올 버퍼
  * @param length : 이름을 받아올 버퍼의 크기(TCHAR 단위)
  * @param flag : option flag(AS_LOCAL : 로컬측 이름, AS_NAME_ONLY : 경로는 제외하고 이름만)
  */
  virtual void GetSyncResultName(LPTSTR name, int length, int flag);
#endif

  /**
  * @brief 대상 파일명이 동기화 폴더에서 제외 파일에 해당하는 이름인지 확인한다.
  * @param filename : 대상 파일명
  * @return BOOL : 제외 파일이 아니면 TRUE를 리턴한다.
  */
  BOOL IsValidFilename(LPCTSTR filename);

  // return R_SUCCESS or failure-code
  /**
  * @brief 로컬측 파일의 이름을 지정한 이름으로 바꾸고 로컬측과 서버측을 분리하여 충돌을 해결한다.
  * 동기화 충돌 처리창에서 로컬측 파일 이름을 바꾸면 이 함수로 처리된다.
  * @param baseFolder : 동기화 루트 폴더
  * @param str : 새로운 이름
  * @return int : 성공하면 R_SUCCESS를, 아니면 오류 코드값을 리턴한다.
  * @details A. 새로운 파일 이름이 유효한 파일명이면,
  *     - 로컬측 이동 정보가 있으면 KSyncRootFolderItem::MoveSyncItem() 메소드로 해당 폴더 하위로 이동하고,
  *     - 새로운 이름과 동일한 이름이 이미 존재하는지 IsDuplicateFilename()로 확인하고, 이미 존재하면 오류 코드를 리턴한다.
  *     - DuplicateItem() 메소드로 이 개체와 동일한 개체를 복제하고, 부모 개체가 바뀌었으면 부모 개체와 복제한 개체를 분리한다.
  *     - renameLocal() 메소드로 로컬측 이름을 새 이름으로 바꾸고 성공하면, 
  *       + 충돌 오류가 FILE_SYNC_RENAMED이거나, 로컬수정-서버삭제 충돌이거나, 서버측 파일이 존재하지 않으면 복제된 개체를 만들지 않는다.
  *       + 복제된 개체에 부모 폴더를 연결하고,
  *       + CloneLocalRename() 메소드로 지금의 개체는 로컬에 새로 등록된 파일로, 복제된 개체는 서버측에 새로 등록된 파일로 설정한다.
  *     - 실패하면 결과를 R_FAIL_RENAME_FAILURE로 한다.
  */
  virtual int SetLocalRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief 서버측 파일의 이름을 지정한 이름으로 바꾸고 로컬측과 서버측을 분리하여 충돌을 해결한다.
  * 동기화 충돌 처리창에서 서버측 파일 이름을 바꾸면 이 함수로 처리된다.
  * @param baseFolder : 동기화 루트 폴더
  * @param str : 새로운 이름
  * @return int : 성공하면 R_SUCCESS를, 아니면 오류 코드값을 리턴한다.
  * @details A. 새로운 파일 이름이 유효한 파일명이면,
  *     - 서버측 이동 정보가 있으면 KSyncRootFolderItem::MoveSyncItem() 메소드로 해당 폴더 하위로 이동하고,
  *     - 새로운 이름과 동일한 이름이 이미 존재하는지 IsDuplicateFilename()로 확인하고, 이미 존재하면 오류 코드를 리턴한다.
  *     - DuplicateItem() 메소드로 이 개체와 동일한 개체를 복제하고, 부모 개체가 바뀌었으면 부모 개체와 복제한 개체를 분리한다.
  *     - renameServer() 메소드로 서버측 이름을 새 이름으로 바꾸고 성공하면, 
  *       + 충돌 오류가 FILE_SYNC_RENAMED이거나, 로컬삭제-서버수정 충돌이거나, 로컬측 파일이 존재하지 않으면 복제된 개체를 만들지 않는다.
  *       + 복제된 개체에 부모 폴더를 연결하고,
  *       + CloneServerRename() 메소드로 지금의 개체는 서버에 새로 등록된 파일로, 복제된 개체는 로컬측에 새로 등록된 파일로 설정한다.
  *     - 실패하면 결과를 R_FAIL_RENAME_FAILURE로 한다.
  */
  virtual int SetServerRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief 서버측과 로컬측이 서로 다른 이름으로 바뀌었을때, 두 개체로 분리하여 충돌을 해결한다.
  * 동기화 충돌 처리창에서 새 이름으로 등록하기를 하면 이 함수로 처리된다.
  * @param root : 동기화 루트 폴더 항목
  * @return int : 성공하면 R_SUCCESS를, 아니면 오류 코드값을 리턴한다.
  * @details A. 로컬측과 서버측의 결과 경로명을 준비한다.
  * @n B. 양쪽 경로명이 동일하면 R_FAIL_SEPARATE_FAILURE로 리턴한다.
  * @n C. 현재의 메타 데이터를 삭제한다.
  * @n D. 양쪽 경로명이 모두 유효하면,
  *     - DuplicateItem()을 호출하여 복제 항목을 생성하고, 
  *     - 복제 항목을 본 개체의 부모 항목에 Child로 등록한다.
  *     - CloneSeparate() 메소드를 호출하여 본 항목은 로컬에 새로운 항목으로, 복제 항목은 서버측에 새로운 항목으로 추가된 것으로 등록한다.
  *     - 히스토리에 저장한다.
  *     .
  * @n E. 아니면
  *     - 서버측 경로가 유효하면,
  *       + 서버측 이동 및 이름 변경 정보가 있으면 적용한다.
  *       + 새 서버 정보(mServerNew)를 서버 정보(mServer)에 복사하고,
  *       + 서버 상태를 새로 추가된 항목, 충돌 결과를 FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED, 결과를 FILE_NEED_DOWNLOAD로 하고,
  *       + 로컬측 정보는 클리어한다.
  *       + StoreMetafile()을 호출하여 메타 데이터를 저장한다.
  *     - 로컬측 경로가 유효하면,
  *       + 로컬측 이동 및 이름 변경 정보가 있으면 적용한다.
  *       + 로컬 상태를 새로 추가된 항목, 충돌 결과를 FILE_NEED_UPLOAD|FILE_SYNC_RENAMED, 결과를 FILE_NEED_UPLOAD로 하고,
  *       + 서버측 정보는 클리어한다.
  *       + StoreMetafile()을 호출하여 메타 데이터를 저장한다.
  *     .
  */
  virtual int RenameSeparate(KSyncRootFolderItem* root);

  /**
  * @brief 지정한 이름으로 이름을 바꿀 수 있는지 검사한다.
  * @param str : 새로운 이름
  * @param onLocal : 로컬측이면 TRUE이다.
  * @return int : 성공하면 R_SUCCESS를, 아니면 오류 코드값을 리턴한다.
  * @details A. 현재의 경로에 지정한 새로운 이름을 합하여 경로명을 만든다.
  * @n B. 위의 경로명에 대해 파일이 존재하면 R_FAIL_SAME_FILE_EXIST_LOCAL를 리턴한다.
  * @n C. 부모 개체가 NULL이면(이 개체가 동기화 루트 폴더 항목) R_FAIL_CANNOT_RENAME_ROOT를 리턴한다.
  * @n D. onLocal이 TRUE이면
  *     - 현재 사용자의 로컬쪽 드라이브 퍼미션이 쓰기 가능인지 확인하고 권한이 없으면 R_FAIL_NO_PERMISSION_TO_RENAME를 리턴한다.
  *     - 현재의 파일명을 준비하고,
  *       + 해당 파일이 존재하고, IsFileWritable()를 호출하여 파일에 덮어쓸 권한이 없으면 R_FAIL_NO_PERMISSION_TO_RENAME를 리턴한다.
  *     .
  * @n E. 아니면
  *     - 현재 사용자의 서버측 드라이브 퍼미션이 쓰기 가능인지 확인하고 권한이 없으면 R_FAIL_NO_PERMISSION_TO_RENAME를 리턴한다.
  *     - 서버 OID가 유효하지 않으면
  *       + 이 개체가 폴더이면 R_FAIL_CANNOT_RENAME_FOLDER를, 파일이면 R_FAIL_CANNOT_RENAME_FILE를 리턴한다.
  *       + CheckPermissionFolderRename()를 호출하여 부모 폴더의 서버 권한과 이 개체의 서버 권한에 대해 이름 변경을 할 수 있는지 체크하고,
  * 할 수 없으면 R_FAIL_NO_PERMISSION_TO_RENAME를 리턴한다.
  *     .
  * @n F. KSyncCoreEngine::IsFileExistServer()를 호출하여 폴더 아래에 동일한 이름의 파일이 있는지 체크하여,
  * 있으면 R_FAIL_SAME_FILE_EXIST_SERVER를 리턴한다.
  */
  int IsDuplicateFilename(LPCTSTR str, BOOL onLocal);

  /**
  * @brief 동기화 항목을 비교하여 이 개체가 크면 1을, 작으면 -1을 같으면 0을 리턴한다. 리스트에서 소트할 때에 사용한다.
  * @param a : 비교할 첫째 항목
  * @param b : 비교할 대상 항목
  * @param sortFlag : 비교할 컬럼 및 플래그
  * @details A. 지정한 컬럼에 앞서 폴더와 파일을 비교하면 폴더가 크다.
  * @n B. 컬럼이 0이면 로컬 파일명, 1이면 로컬 파일 시간, 2이면 로컬 파일 크기, 3이면 서버측 이름, 4이면 서버측 최종 사용자명,
  * 5이면 서버 파일 시간, 6이면 서버 파일크기 등으로 비교하여 결과를 리턴한다.
  */
  int CompareSyncItem(KSyncItem* a, KSyncItem* b, int sortFlag);

#ifdef _DEBUG_RESOLVE
  virtual inline BOOL IsResolveVisible() { return TRUE; }
#else
  /**
  * @brief 동기화 충돌 처리창에 표시될 항목이면 TRUE를 리턴한다.
  * @details A. 충돌 결과값에 FILE_CONFLICT(동기화 충돌), FILE_SYNC_ERR(동기화 오류), 
  * FILE_SYNC_RENAMED(충돌시 이름변경됨), FILE_SYNC_BELOW_CONFLICT(하위 항목이 충돌) 의 플래그가 있으면 TRUE를 리턴한다.
  */
  virtual inline BOOL IsResolveVisible() { return (mConflictResult & FILE_SYNC_RESOLVE_VISIBLE_FLAG); }
#endif

  /**
  * @brief 동기화 충돌 처리창에서 사용하는 각 항목들의 플래그 값을 리턴한다.
  */
  inline int GetResolveFlag() { return mResolveFlag; }
  /**
  * @brief 동기화 충돌 처리창에서 이 항목이 선택되었으면 TRUE를 리턴한다.
  */
  inline BOOL IsSelected() { return !!(mResolveFlag & ITEM_SELECTED); }
  /**
  * @brief 동기화 충돌 처리창에서 이 항목이 Visible이면 TRUE를 리턴한다.
  */
  inline BOOL IsVisible() { return !(mResolveFlag & HIDDEN_ROOT_FOLDER); }

  /**
  * @brief 동기화 충돌 또는 오류가 발생된 항목이면 TRUE를 리턴한다.
  */
  inline BOOL IsConflicted() { return !!(mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR)); }
  /**
  * @brief 동기화 충돌, 동기화 오류 또는 충돌 처리에서 이름변경된 항목이면 TRUE를 리턴한다.
  */
  inline BOOL IsConflictResolved() { return !!(mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)); }

  /**
  * @brief 충돌 결과값에 지정한 flag를 더한다
  */
  BOOL AddConflictResult(int flag);

  /**
  * @brief 동기화 충돌 처리창에서 이 항목 또는 하위 항목이 하나라도 선택되어 있으면 TRUE를 리턴한다.
  * 충돌 목록 중에서 선택된 항목이 있는지 체크할 때 사용된다.
  */
  virtual BOOL HasSelected() { return !!(mResolveFlag & ITEM_SELECTED); }

  /**
  * @brief 동기화 충돌 처리창에서 선택된 항목을 '동기화 Enable' 시킨다.
  * '선택된 항목만 동기화하기' 기능이 켜져 있을때 사용된다.
  */
  virtual void EnableSelectedItem();

  /**
  * @brief 동기화 충돌 처리창에서 각 항목의 Enable 상태를 초기화 시킨다.
  * @details A. 결과값이 FILE_NOT_CHANGED(0)이 아닌 항목은 Enable로 설정한다.
  */
  void InitResolveState(int flag);

  /**
  * @brief 동기화 충돌 처리 플래그값(mResolveFlag)에 지정한 값은 마스크하고 더한다.
  * @param a : AND로 마스크할 값.
  * @param o : OR로 더할 값.
  * @param withChild : TRUE이면 child 항목에도 recursive하게 적용한다.
  */
  virtual void SetResolveFlagMask(int a, int o, BOOL withChild);

  /**
  * @brief 동기화 충돌 처리 플래그값(mResolveFlag)이 지정한 조건에 해당되는 개체들의 인덱스 범위를 구한다.
  * @param a : AND로 마스크할 값.
  * @param o : 비교값.
  * @param index : 개체의 index값.
  * @param area : 개체의 인덱스 범위를 저장할 버퍼.
  * @param flag : 플래그 값.
  * @details A. 이 항목이 동기화 충돌 목록에 나타나는 항목이면,
  *       - ((mResolveFlag & a) == o) 조건이 만족하면
  *         + area[0]값이 0보다 작으면 지금의 index 값을 area[0]에 넣는다.
  *         + area[1]값이 index보다 작으면 지금의 index 값을 area[1]에 넣는다.
  *       - (index + 1) 값을 리턴한다.
  * @n B. 아니면 index 값을 리턴한다.
  */
  virtual int GetResolveFlagArea(int a, int o, int index, int* area, int flag);

  /**
  * @brief 동기화 충돌 처리 트리에서 본 항목의 하위 Child 항목의 갯수를 리턴하는 가상함수이다.
  */
  virtual int GetChildRowCount() { return 1; }


  /**
  * @brief 로컬측 개체의 마지막 수정 날짜를 리턴함.
  * @return SYSTEMTIME : 로컬측 마지막 수정 날짜
  */
  inline SYSTEMTIME GetLocalTime() { return ((CompareSystemTime(mLocal.FileTime, mLocalNew.FileTime) > 0) ? mLocal.FileTime : mLocalNew.FileTime); }

  /**
  * @brief 서버측 개체의 마지막 수정 날짜를 리턴함.
  * @return SYSTEMTIME : 서버측 마지막 수정 날짜
  */
  inline SYSTEMTIME GetServerTime() { return ((CompareSystemTime(mServer.FileTime, mServerNew.FileTime) > 0) ? mServer.FileTime : mServerNew.FileTime); }

  /**
  * @brief 서버측 개체의 마지막 수정자 OID를 리턴함.
  * @return LPTSTR : 마지막 수정자 OID
  */
  inline LPTSTR GetServerUser() { return ((mServerNew.UserOID[0] != '\0') ? mServerNew.UserOID : mServer.UserOID); }

  /**
  * @brief 서버측 개체의 권한값을 리턴함.
  * @return int
  */
  inline int GetServerPermission() { return ((mServerNew.Permissions != -1) ? mServerNew.Permissions : mServer.Permissions); }

  /**
  * @brief 서버측 개체의 권한값을 설정함.
  * @return void
  * @param p : 파일 및 폴더의 권한값
  */
  inline void SetServerPermission(int p) { mServer.Permissions = p; }

  /**
  * @brief 대상 항목의 부모 항목의 포인터를 설정함.
  * @return void
  * @param p : 부모 항목의 포인터
  */
  inline void SetParent(KSyncItem *p) { mParent = p; }

  /**
  * @brief 대상 항목의 새 부모 항목의 포인터를 설정함.
  * @return void
  * @param p : 새 부모 항목의 포인터
  */
  inline void SetNewParent(KSyncItem *p) { mNewParent = p; }

  /**
  * @brief 이 개체와 하위 개체의 경로를 덮어쓴다.
  */

  virtual BOOL IsOverridePath();

#ifdef USE_META_STORAGE
  virtual void SetOverridePath(int callingDepth, KSyncRootStorage* s, LPCTSTR path);
#endif

#ifdef _DEBUG
  virtual void DumpSyncItems(int depth);
#endif

  /** 부모 항목의 포인터 */
  KSyncItem *mParent; 
  /** 새 부모 항목의 포인터 */
  KSyncItem *mNewParent; // parent item which moved by server-side.

  /** 메타 파일명, KSyncRootStorageFile 사용할 때에만 사용됨 */
  LPTSTR mpMetafilename;
  // TCHAR mMetafilename[KMAX_LONGPATH]; old metafile name

  /** 로컬측 정보 */
  LOCAL_INFO mLocal;
  /** 새 로컬측 정보 */
  LOCAL_INFO mLocalNew;
  /** 플래그 */
  int mFlag;

  /** 서버측 OID */
  TCHAR mServerOID[MAX_OID];

  /** 서버측 정보 */
  SERVER_INFO mServer;
  /** 새 서버측 정보 */
  SERVER_INFO mServerNew;

  /** 로컬측 상태 */
  int mLocalState;
  /** 서버측 상태 */
  int mServerState;

  /** 서버 폴더 스캔 상태(변동없음, 삭제됨, 존재함) */
  int mServerScanState;

  /** 동기화 Enable 상태 */
  BOOL mEnable;
  /** 동기화 충돌 결과 */
  int mConflictResult;
  /** 하위 항목 결과 */
  int mChildResult;
  /** 동기화 결과 */
  int mResult;

  /** 로컬측 이동 및 이름 변경 정보 */
  LPTSTR mpLocalRenamedAs;
  /** 서버측 이동 및 이름 변경 정보 */
  LPTSTR mpServerRenamedAs;
  /*
  TCHAR mLocalRenamedAs[KMAX_PATH]; // local file was renamed by user
  TCHAR mServerRenamedAs[KMAX_PATH]; // server file was renamed by user
  */

  /** 상대 경로명, DB Storage에서 key로 사용된다 */
  LPTSTR mpRelativePath;
  //TCHAR mRelativeName[KMAX_PATH]; // local relative pathname on last sync-time

  /** 서버의 동기화 오류 메시지가 저장되는 버퍼 */
  LPTSTR mpServerConflictMessage;

  /** 서버측 임시 이름 변경 버퍼*/
  LPTSTR mpServerTempRenamed;
  /** 로컬측 임시 이름 변경 버퍼
  * @n 항목을 이동 처리할때, 동일한 이름의 항목이 있는 경우에 임시 이름으로 바꾸어 이동을 처리하고 나중에 다시 원래 이름으로
  * 바꾸어 놓을 때에 사용된다.
  */
  LPTSTR mpLocalTempRenamed;

  //TCHAR mServerTempRenamed[KMAX_PATH];
  //TCHAR mLocalTempRenamed[KMAX_PATH];

  /** 서버측 이동 대상 폴더 OID */
  TCHAR mServerMoveToOID[MAX_OID]; // new folder which item should move to

  /** 동기화 작업 단계. 검색 단계, 비교 단계, 이동 단계, 복사 단계 등 */
  int mPhase;

  /** 동기화 충돌 처리 플래그, 폴더의 Open/Close, 더미 폴더, 사용자 선택 여부 등을 나타낸다.*/
  int mResolveFlag;

  /** 강제 변경된 로컬 경로 */
  LPTSTR mpOverridePath;
};

#include <vector>

typedef std::vector<KSyncItem*> KSyncItemArray;
typedef std::vector<KSyncItem*>::iterator KSyncItemIterator;
typedef std::vector<KSyncItem*>::reverse_iterator KSyncItemRIterator;

/**
* @brief KSyncItemArray의 하위 항목을 모두 해제한다.
*/
extern void FreeSyncItemList(KSyncItemArray& array);
/**
* @brief KSyncItemArray의 하위 항목중 지정한 갯수만큼 해제한다.
*/
extern void FreeSyncItemListTop(KSyncItemArray &array, int maxCount, BOOL top);

/**
* @brief LOCAL_INFO 스트럭쳐를 초기화 한다.
*/
extern void ClearLocalInfo(LOCAL_INFO &local);
/**
* @brief SERVER_INFO 스트럭쳐를 초기화 한다.
*/
extern void ClearServerInfo(SERVER_INFO &server);

extern void FreeLocalInfo(LOCAL_INFO &local);
/**
* @brief SERVER_INFO 스트럭쳐를 해제한다.
*/
extern void FreeServerInfo(SERVER_INFO &dst);
/**
* @brief SERVER_INFO 스트럭쳐 dst에 src를 복사한다.
*/
extern void CopyServerInfo(SERVER_INFO &dst, SERVER_INFO &src);
/**
* @brief SERVER_INFO 스트럭쳐 dst에 src를 머지한다.
*/
extern void MergeServerInfo(SERVER_INFO &dst, SERVER_INFO &src);
/**
* @brief LOCAL_INFO 스트럭쳐 dst에 src를 머지한다.
*/
extern void MergeLocalInfo(LOCAL_INFO &dst, LOCAL_INFO &src);

extern LPCTSTR GetDisplayPathName(LPCTSTR folder_path);


#endif