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

#define ITEM_STATE_NONE 0x0 /** ���� �� ���� */
#define ITEM_STATE_NOW_EXIST 0x01 /** ��� �׸��� ���� ������ */
#define ITEM_STATE_WAS_EXIST 0x02 /** ��� �׸��� �����ϰ� �־��� */
#define ITEM_STATE_UPDATED 0x04 /** ��� �׸��� ����Ǿ��� */
#define ITEM_STATE_DELETED 0x08 /** ��� �׸��� ������ */
#define ITEM_STATE_MOVED 0x10 /** ��� �׸��� �̵��� */
#define ITEM_STATE_PARENT_MOVED 0x20 /** ��� �׸��� �θ� ������ �̵��� */
#define ITEM_STATE_CONFLICT_RENAMED 0x40 /** ��� �׸��� �浹 ó������ �̸� ����� */
#define ITEM_STATE_RENAMED 0x80 /** used only ConflictTreeXCtrl temporarily */

#define ITEM_STATE_EXISTING (ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST)

// persisting states
#define ITEM_STATE_NEWLY_ADDED 0x100 /** ��� �׸��� ���� �߰��� ���� */
#define ITEM_STATE_NO_PERMISSION 0x200 /** ��� �׸��� ���� ���� ������ */
#define ITEM_STATE_ERROR 0x400 /** ��� �׸��� ����ȭ ���� ������ */
#define ITEM_STATE_REFRESH_PATH 0x0800 /** ������ ��θ� ������Ʈ�ϰ� DB���� ��θ� ������Ʈ�� �ʿ� ���� */
#define ITEM_STATE_LOCATION_CHANGED 0x1000 /** ��� �׸��� ��ġ�� ����� */
#define ITEM_STATE_RE_UPDATED 0x2000 /** ��� �׸��� ����ȭ �� ���� �ٽ� ������ */
#define ITEM_STATE_RESTORED 0x4000 /** ��� �׸��� �����Ǿ��ٰ� ������ */

#define ITEM_STATE_PERSISTING 0xFF00

#define ITEM_STATE_CLEAR_MASK (ITEM_STATE_PERSISTING | ITEM_STATE_EXISTING)

// mServerScanState
#define ITEM_NO_CHANGE 0 /** ��� �׸��� ���� ���� */
#define ITEM_EXIST 1 /** ��� �׸��� ������ */
#define ITEM_DELETED 2 /** ��� �׸��� ������ */

// for itself
#define SFP_LOOKUP 0x0001 /** ��� �׸��� ��ȸ ���� */
#define SFP_READ 0x0002 /** ��� �׸��� �б� ���� */
#define SFP_WRITE 0x0004 /** ��� �׸��� ���� ���� */
#define SFP_DELETE 0x0008 /** ��� �׸��� ���� ���� */
#define SFP_SETUP 0x0010 // old 0x000F  /** ��� �׸��� ��ü ���� */

// for childs
#define SFP_INSERT_CONTAINER 0x0020 // old 0x0010  /** ��� �׸��� ���� ���� ���� ���� */
#define SFP_INSERT_ELEMENT 0x0040 // old 0x0020 /** ��� �׸��� ���� ���� ���� ���� */

#define SFP_LOOKUP_ELEMENT 0x0080 /** ��� �׸��� ���� �׸� ��ȸ ���� */
#define SFP_READ_ELEMENT 0x0100 /** ��� �׸��� ���� �׸� �б� ���� */
#define SFP_WRITE_ELEMENT 0x0200 /** ��� �׸��� ���� �׸� ���� ���� */
#define SFP_DELETE_ELEMENT 0x0400 /** ��� �׸��� ���� �׸� ���� ���� */
#define SFP_SETUP_ELEMENT 0x0800 // old 0x0FF0  /** ��� �׸��� ���� �׸� ��ü ���� */

#define SFP_FULL_SETUP 0x0FFF  /** ��� �׸��� ��ü ���� */

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
// #define WITH_STORE_META 0x01 //��Ÿ �����͸� ������ �� ����
#define WITH_NOT_UPDATE_RELATIVE_PATH 0x04 // ��θ��� ������Ʈ �� ������.

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
* @brief ����ȭ ��� �׸��� ���� �׸� Ŭ����(base class)
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
  * @brief ��Ʈ������ ǥ���� ���� ������ mask value�� ��ȯ��.
  * @param permissions : ��Ʈ������ ǥ���� ���� ����('l,r,w,d' ��)
  * @return int
  */
  static int String2Permission(LPCTSTR permissions);

  /**
  * @brief �Էµ� ���Ѱ��� ����ȭ ��Ʈ ������ ���ѿ� �����ϴ��� �˻���.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  * @return BOOL
  */
  static BOOL CheckPermissionRootFolder(int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ����ȭ ���� ������ ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionSubFolder(int permission);

  /**
  * @brief �Էµ� ���Ѱ��� �� ������ ������ ���ε��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFolderUpload(int parent_permission, int permission);

  /**
  * @brief �Էµ� ���Ѱ��� �� ������ �������� �ٿ�ε��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFolderDownload(int parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ ������ ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFolderDelete(int parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ �̵��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param src_parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param dest_parent_permission : �̵��� ������ ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFolderMove(int src_parent_permission, int dest_parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ �̸��� �ٲ� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFolderRename(int parent_permission, int permission);

  /**
  * @brief �Էµ� ���Ѱ��� ������ ���ε��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFileUpload(int parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ �߰��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFileInsert(int parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ �ٿ�ε��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFileDownload(int parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ ������ ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFileDelete(int parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ �̵��� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param src_parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param dest_parent_permission : �̵��� ������ ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFileMove(int src_parent_permission, int dest_parent_permission, int permission);
  /**
  * @brief �Էµ� ���Ѱ��� ������ �̸��� �ٲ� ���ѿ� �����ϴ��� �˻���.
  * @return BOOL
  * @param parent_permission : �ش� ��ü�� �θ� ��ü�� ���Ѱ�.
  * @param permission : �ش� ��ü�� ���Ѱ�.
  */
  static BOOL CheckPermissionFileRename(int parent_permission, int permission);

  /**
  * @brief ������ ��� ��� �� ���ϸ��� �� �޸� ���ۿ� �Ҵ��Ͽ� ������ �� ������.
  * @param rootItem : ����ȭ ��Ʈ ���� ��ü
  * @param serverFullPath : ��� ��ü�� ������ ��ü ���
  * @param fileName : ���ϸ�
  * @return LPTSTR : ������ ��� ��θ�
  */
  static LPTSTR allocServerSideRelativePath(KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName);

  /**
  * @brief ������ ��� ��� �� ���ϸ��� �� �޸� ���ۿ� �Ҵ��Ͽ� ������ �� ������.
  * @param serverRootPath : ����ȭ ��Ʈ ���� ��ü�� ������ ��ü ���
  * @param serverFullPath : ��� ��ü�� ������ ��ü ���
  * @param fileName : ���ϸ�
  * @return LPTSTR : ������ ��� ��θ�
  */
  static LPTSTR AllocServerSideRelativePath(LPCTSTR serverRootPath, LPCTSTR serverFullPath, LPCTSTR fileName);

  /**
  * @brief �� ��θ��� ���Ͽ� ��ΰ� �ٸ��� Ȯ���Ѵ�.
  * @param oldpathname : ���� ��θ�
  * @param newpathname : �� ��θ�
  * @return BOOL : ��ΰ� �ٸ��� TRUE�� �����Ѵ�.
  */
  static BOOL isPathChanged(LPCTSTR oldpathname, LPCTSTR newpathname);

  /**
  * @brief �� ��θ��� ���Ͽ� ���ϸ��� �ٸ��� Ȯ���Ѵ�.
  * @param oldpathname : ���� ��θ�
  * @param newpathname : �� ��θ�
  * @return BOOL : ���ϸ��� �ٸ��� TRUE�� �����Ѵ�.
  */
  static BOOL isNameChanged(LPCTSTR oldpathname, LPCTSTR newpathname);

  /*
  void getServerSideFullPathName(LPTSTR fullName, int size, KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName);
  LPTSTR allocServerSidePathName(BOOL bFull, KSyncItem *rootItem, LPCTSTR serverFullPath, LPCTSTR fileName);
  */

  /**
  * @brief �浹 ó�� ���ۿ��� ��� �׸��� �浹 ó���� �� ������ �ƴ��� ���θ� ��Ÿ��.
  * @return BOOL
  */
  inline BOOL GetEnable() { return mEnable; } 
  /**
  * @brief �浹 ó�� ���ۿ��� ��� �׸��� �浹 ó���� �� ������ �ƴ��� ���θ� �����Ѵ�.
  * @return void
  * @param e : TRUE�̸� ��� �׸��� �浹 ó���� ��.
  */
  inline void SetEnable(BOOL e) { mEnable = e; } 
  /**
  * @brief �浹 ó�� ���ۿ��� ��� �׸��� �浹 ó���� �� ������ �ƴ��� ���θ� �ݴ�� ��.
  * @return void
  */
  inline void ToggleEnable() { mEnable = !mEnable; } 
  /**
  * @brief �浹 ó�� ���ۿ��� ��� �׸��� �浹 ó���� �� ������ �ƴ��� ���θ� ������. ���� �׸� ��� ������
  * @return void
  * @param e : TRUE�̸� ��� �׸��� �浹 ó���� ��.
  */
  virtual inline void SetEnableRecurse(BOOL e) { mEnable = e; }
  /**
  * @brief �浹 ó�� ���ۿ��� ��� �׸� �Ǵ� ���� �׸��� �浹 ó���� �ϵ��� �����Ǿ� ������ TRUE�� ������
  * @return void
  */
  virtual BOOL GetEnableChild() {return mEnable; }

  /**
  * @brief ����׸��� ����ȭ ��󿡼� �����ϰų� ���� �÷��׸� ������.
  * @return void
  */
  inline void SetExcludedSync(BOOL set) { if(set) mFlag |= SYNC_ITEM_EXCLUDED; else mFlag &= ~SYNC_ITEM_EXCLUDED; }
  /**
  * @brief ����׸��� ����ȭ ��󿡼� ���ܵǾ� ������ TRUE�� ������.
  * @return BOOL
  */
  inline BOOL IsExcludedSync() { return (mFlag & SYNC_ITEM_EXCLUDED); }

  virtual BOOL IsExcludedSyncRecur() { return (mFlag & SYNC_ITEM_EXCLUDED); }

  /*
  * @brief ������ ���� �׸� Rescan �ʿ��� �÷��׸� ��Ʈ�ϰų� �����Ѵ�.
  */
  inline void SetServerNeedRescan(BOOL set) { if(set) mFlag |= SYNC_ITEM_NEED_SERVER_RESCAN; else mFlag &= ~SYNC_ITEM_NEED_SERVER_RESCAN; }

  /*
  * @brief ������ ���� �׸� Rescan �ʿ��� �÷��װ� ��Ʈ�Ǿ����� Ȯ���Ѵ�.
  */
  inline BOOL IsServerNeedRescan() { return (mFlag  & SYNC_ITEM_NEED_SERVER_RESCAN); }

  /**
  * @brief ����׸��� flag�� �Ӽ����� ����.
  * @return void
  */
  inline void AddSyncFlag(int f) { mFlag |= f; }
  /**
  * @brief ����׸��� flag�� �Ӽ����� ������.
  * @return void
  */
  inline void RemoveSyncFlag(int f) { mFlag &= ~f; }
  /**
  * @brief ����׸��� flag�� Ư�� �Ӽ����� �ִ��� �˻���.
  * @return BOOL
  */
  inline BOOL HasSyncFlag(int f) { return !!(mFlag & f); }

  /**
  * @brief �ڽ� �Ǵ� �� ������ ���� ��û�� ó���Ǿ����� Ȯ����.
  */
  virtual BOOL IsParentAlreadyRegistDeleteOnServer();

  /**
  * @brief ������ ����ȭ �۾��� �κ� ����ȭ �۾��̸� TRUE�� ������.
  * @return BOOL
  * @param pathname : ��� ��� �̸�
  * @param isFolder : ����� �������� ��������.
  * @details ��� ������ �κ� ����ȭ �۾� ���� �̿��� ���� �����̸� FALSE�� ������.
  */
  virtual inline BOOL IsPartialSyncOn(LPCTSTR pathname, BOOL isFolder) { return TRUE; }

  /**
  * @brief ����ȭ �۾��� �߻��� ���� �޽����� ������(���������� ���޹��� ���� �޽���)
  * @return LPCTSTR : ������ ���� �޽���.
  */
  inline LPCTSTR GetServerConflictMessage() { return mpServerConflictMessage; }

  /**
  * @brief ����ȭ �۾� ������� ������
  * @param r : �����
  * @return void
  */
  inline void SetResult(int r) { mResult = r; }
  /**
  * @brief ����ȭ �۾� ������� �޾ƿ�
  * @return int : ����ȭ �۾� �����
  */
  inline int GetResult() { return mResult; }

  /**
  * @brief �������� �����ϴ��� ���θ� ������
  * @return BOOL : �����ϸ� TRUE�� ������
  */
  inline BOOL IsServerNotExist() { return (mServerOID[0] == '\0'); }

  /**
  * @brief �������� �����ϴ��� ���θ� ������(���� Ÿ���� ��ȿ�ϸ� �����ϴ� ������)
  * @return BOOL : �����ϸ� TRUE�� ������
  */
  inline BOOL IsLocalNotExist() { return ((mLocal.FileTime.wYear == 0) && (mLocalNew.FileTime.wYear == 0)); }

  /**
  * @brief folderFullPathIndex�� �ٲ�������� �����ϴ� �����Լ�
  * @return BOOL : �ٲ������ TRUE�� ������
  */
  virtual inline BOOL IsFolderFullPathChanged() { return FALSE; }

  /**
  * @brief ���� ���°��� �����Ѵ�.
  * @param state : ���°�
  * @return void
  */
  inline void setLocalState(int state) { mLocalState = state; }
  /**
  * @brief ���� ���°��� ������ ���� ���Ѵ�.
  * @param state : ���°�
  * @return void
  */
  inline void addLocalState(int state) { mLocalState |= state; }
  /**
  * @brief ���� ���°��� �����Ѵ�.
  * @param state : ���� ���°�
  * @return void
  */
  inline void setServerState(int state) { mServerState = state; }
  /**
  * @brief ���� ���°��� ������ ���� ���Ѵ�.
  * @param state : ���� ���°�
  * @return void
  */
  inline void addServerState(int state) { mServerState |= state; }
  /**
  * @brief ���� ���°��� ������ ���� ����ũ�ϰ� ���Ѵ�.
  * @param mask : not and value
  * @param add : or value
  * @return void
  */
  inline void maskLocalState(int mask, int add) { mLocalState = (mLocalState & ~mask) | add; }
  /**
  * @brief ���� ���°��� ������ ���� ����ũ�ϰ� ���Ѵ�.
  * @param mask : not and value
  * @param add : or value
  * @return void
  */
  inline void maskServerState(int mask, int add) { mServerState = (mServerState & ~mask) | add; }

  /**
  * @brief ���� ���°��� �����Ѵ�.
  * @return int : ���°�
  */
  inline int GetLocalState() { return mLocalState; }
  /**
  * @brief ���� ���°��� �����Ѵ�.
  * @return int : ���°�
  */
  inline int GetServerState() { return mServerState; }

  /**
  * @brief ������ ���� ũ�⸦ �����Ѵ�.
  * @param size : Byte ���� ���� ũ��
  * @return void
  */
  inline void SetLocalFileSize(__int64 size) { mLocal.FileSize = size; }
  /**
  * @brief ������ ���� ũ�⸦ �����Ѵ�.
  * @param size : Byte ���� ���� ũ��
  * @return void
  */
  inline void SetServerFileSize(__int64 size) { mServer.FileSize = size; }

  /**
  * @brief ����ȭ �۾� �ܰ谪�� �����Ѵ�.
  * @return int : ����ȭ �۾� �ܰ谪
  */
  inline int GetPhase() { return mPhase; }
  /**
  * @brief ����ȭ �۾� �ܰ谪�� �����Ѵ�.
  * @param p : ����ȭ �۾� �ܰ谪
  * @details : �ܰ谪�� PHASE_END_OF_JOB�� ��쿡�� �ٲ��� �ʴ´�.
  */
  virtual inline void SetPhase(int p) { if (mPhase < PHASE_END_OF_JOB) mPhase = p; }
  /**
  * @brief ����ȭ �۾� �ܰ谪�� ������ �����Ѵ�.
  * @param p : ����ȭ �۾� �ܰ谪
  */
  virtual inline void SetPhaseForce(int p) { mPhase = p; }

  /**
  * @brief KSyncItem object�� ������ �����ϰ� ��ü reference�� ������
  * @return KSyncItem &
  * @param item : �ҽ� ��ü
  */
  KSyncItem &operator=(KSyncItem &item);

  /**
  * @brief �ڽŰ� ������ object�� �����ϰ� �����͸� ������.
  * @return KSyncItem* : ���� ������ ��ü
  */
  virtual KSyncItem* DuplicateItem();

  /**
  * @brief ��� �������� ��� �ʱ�ȭ�Ѵ�.
  * @return void
  * @param parent : parent object
  */
  void init(KSyncItem *parent);

  /**
  * @return void
  * @brief �� ��ü�� �Ҵ��� �޸� ����.
  * @details A. ������ �޸� �����͸� �����Ѵ�.
  *     - mpRelativePath
  *     - mpMetafilename
  *     - mpLocalRenamedAs
  *     - mpServerRenamedAs
  *     - mpServerTempRenamed
  *     - mpLocalTempRenamed
  *     - mpServerConflictMessage
  *     .
  * @details B. FreeServerInfo() �޼ҵ�� mServer, mServerNew �ΰ��� �����Ѵ�.
  */
  void FreeAllocated();

  /**
  * @brief �� �׸��� ������ ����ȭ �۾� �ܰ迡�� ó���� �۾��� �ִ��� Ȯ���Ѵ�.
  * @return BOOL : �۾��� ������ ������ TRUE�� �����Ѵ�.
  * @param phase : �۾� �ܰ�
  */
  virtual BOOL IsNeedSyncPhase(int phase);

  /**
  * @brief �� �׸��� ���� ũ�⸦ �����Ѵ�.
  * @return __int64 : ���� ũ��
  * @param conflictResult : TRUE�̸� �浹 ��� ���� ũ��
  */
  __int64 GetSyncFileSize(BOOL conflictResult);

  /**
  * @brief 'yyyy-mm-dd hh:mm:ss:ms'������ ���ڿ����� mLocal.FileTime���� �о����.
  * @return bool
  * @param str : �Է� ��Ʈ��
  */
  BOOL ParseDateString(char *str);

  /**
  * @brief �����丮 ���Ϸ� ����� ���ڿ����� ��ü�� ����(name=, user=, result ��)�� �о����.
  * @return bool
  * @param str : �Է� ��Ʈ��
  */
  BOOL ParseHistoryString(char *str);

  /**
  * @brief ��ü�� ���� ������ ��� ��θ� ������. ��Ʈ ���������� NULL
  * @return LPCTSTR
  */
  LPCTSTR GetPath();

  /**
  * @brief ��ü�� ���� ������ ��� ��θ� ������. ��Ʈ ���������� '\\'
  * @return LPCTSTR
  */
  LPCTSTR GetPathString();

  /**
  * @brief ��ü�� ������ ���ϸ��� ������.
  * @return LPCTSTR
  */
  LPCTSTR GetFilename();

  /**
  * @brief ��ü�� ������ ������ �����ð� SYSTEMTIME�� �����͸� ������.
  * @return LPCTSTR
  */
  SYSTEMTIME *GetLocalFileTime();

  /**
  * @brief ��ü�� ������ ���� ũ�⸦ ������.
  * @return LPCTSTR
  */
  inline __int64 GetLocalFileSize() { return ((mLocalNew.FileSize >= 0) ? mLocalNew.FileSize : mLocal.FileSize); }
  /**
  * @brief ��ü�� ������ ���ϸ��� ������.
  * @return LPCTSTR
  */
  inline __int64 GetServerFileSize() { return ((mServerNew.FileSize >= 0) ? mServerNew.FileSize : mServer.FileSize); }

  /**
  * @brief ����ȭ ��ü�� ������ �����ϴ� �����Լ�.
  * @return int : ����ȭ ��ü�� ����(0=file, 1=folder, 2=root folder)
  */
  virtual int IsFolder() { return 0; }

  virtual BOOL IsSystemRoot() { return ((mLocal.pFilename != NULL) && (mLocal.pFilename[0] == '\\') && (mLocal.pFilename[1] == '\0')); }

  /**
  * @brief ��ü�� ��� ��� �̸�(mpRelativePath)�� ������ pathname���� ������.
  * @return void
  * @param pathname : �Էµ� ��� ��� �� �̸� ��Ʈ��
  * mpRelativePath�� ��Ÿ DB�� Ű������ ����.
  */
  virtual void SetRelativePathName(LPCTSTR pathname);

  virtual void ClearRelativePathName(int flag);

  /**
  * @brief ��ü�� �θ� ��ü�� ��ο� ��ü�� �̸��� ���ؼ� mpRelativePath ���� ������Ʈ ��.
  * @return void
  */
  virtual void UpdateRelativePath();

  /**
  * @brief ��ü�� ��ο� �̸��� �Էµ� ��� �̸����� �ٲ�, ��ü Ʈ�������� ��ġ�� �ٲ�.
  * @return void
  * @param rootFolder : ����ȭ ��Ʈ ���� ��ü
  * @param relativePathName : �Էµ� ��� ��� �� �̸� ��Ʈ��
  */
  void ChangeRelativePathName(KSyncRootFolderItem* rootFolder, LPCTSTR relativePathName, BOOL moveInTree);

  /**
  * @brief ��ü�� ������ FileOID�� ������.
  * @return LPCTSTR
  */
  LPCTSTR GetServerFileOID();
  /**
  * @brief ��ü�� ������ StorageOID�� ������.
  * @return LPCTSTR
  */
  LPCTSTR GetServerStorageOID();

  /**
  * @brief ��ü�� ������ ����θ� ������.
  * @return LPCTSTR
  * ������ ��쿡�� �ش� ������ �̸��� �����ϰ� ������ ��쿡�� ������ ��ġ�� ������.
  */
  virtual LPCTSTR GetServerRelativePath();

  /**
  * @brief ��ü�� ������ FullPathIndex�� ������.
  * @return LPCTSTR
  */
  LPCTSTR GetServerFullPathIndex();

  /**
  * @brief ��ü�� ������ ��� ��ο� ���ϸ��� ���� �Ҵ�� �޸𸮿� �����ؼ� ������.
  * @return LPCTSTR
  */
  LPTSTR AllocServerRelativePathName();


  /**
  * @brief ��ü�� ���Ե� ������ ��θ� ������ path�� �ٲ�.
  * @return void
  * @param baseFolder : �ش� ��ü�� ���Ե� ������ �����.
  * @param rootFolder : ����ȭ ��Ʈ ������ ���.
  * @param depth : ����ȭ ���� Ʈ�������� depth
  */
  virtual void SetLocalPath(LPCTSTR baseFolder, LPCTSTR rootFolder, int depth);

  /**
  * @brief ����ȭ �浹 ó��â���� ������ ��ġ�� �̵��� �׸�鿡�� �̵��� ����� �̸��� �޸� �Ҵ��Ͽ� ������.
  * @return LPTSTR
  * @param flag : option flag(AS_LOCAL : ������ �̸�, AS_NAME_ONLY : ��δ� �����ϰ� �̸���)
  */
  virtual LPTSTR AllocSyncMoveResultName(int flag) { return NULL; }


  /**
  * @brief ������ ���� ������ mLocal�� ������Ʈ ��.
  * @return BOOL
  * @param user : ������ ���� ������ OID�� �̰����� ������Ʈ ��.
  */
  BOOL RefreshLocalFileInfo(LPCTSTR user);

  /**
  * @brief ������ ������ ���� ������ mLocal�� ������Ʈ ��.
  * @return BOOL
  * @param fullname : ��� ������ ���� ��ü ��� �̸�.
  * @param user : ������ ���� ������ OID�� �̰����� ������Ʈ ��.
  */
  BOOL RefreshLocalFileInfo(LPCTSTR fullname, LPCTSTR user);

  /**
  * @brief ������ ������ ���� ������ mLocalNew�� ������Ʈ ��.
  * @return BOOL
  * @param user : ������ ���� ������ OID�� �̰����� ������Ʈ ��.
  * @param rootItem : ����ȭ ��Ʈ ���� �׸�
  */
  BOOL RefreshLocalNewFileInfo(LPCTSTR user, KSyncItem* rootItem = NULL);

  void RefreshLocalNewState(KSyncRootFolderItem* root);

  /**
  * @brief ������ ������ ���� ����ȭ ���� ������ �������� �˻���(mLocalNew�� mLocal ������ ���ؼ�)
  * @return BOOL
  */
  BOOL IsLocalModified();
  /**
  * @brief ������ ������ ����ȭ �� ���� �ٽ� ������ �������� �˻���.
  * @return BOOL
  */
  BOOL IsLocalModifiedAgain(BOOL update); // check modified after compare

  /**
  * @brief ������ ������ ���� ����ȭ ���� ������ �������� �˻���.
  * @return BOOL
  */
  BOOL IsServerModified();

  /**
  * @brief mLocalNew ������ ���� ������Ʈ�� ��, ������ ������ ���� ����ȭ ���� ������ �������� �˻���(mLocalNew�� mLocal ������ ���ؼ�)
  * @return BOOL
  */
  BOOL IsModifiedNow();

  /**
  * @brief ��� �׸��� ������Ʈ�� �ʿ����� Ȯ���Ѵ�.(�����ǰų� �̵��ǰų� ���� �߰��Ǿ��ų�)
  * @return BOOL
  */
  BOOL IsNeedUpdate(BOOL onLocal);

  /**
  * @brief ������ ������ ���� ������ mServerNew�� ������Ʈ ��.
  * @return void
  */
  void UpdateServerNewInfo();

  /**
  * @brief mLocalNew ������ mLocal�� ������ �� mLocalNew�� Ŭ���� ��.
  * @return void
  */
  void UpdateLocalNewInfo(LPCTSTR baseFolder);

  virtual int GetCountForMoveInOutOnServer(LPCTSTR partialPath, int path_len);

  /**
  * @brief ��ü�� ���� ��ġ�� �ٲ������ �Ҹ��� �Լ�. ������ ��Ÿ������ ����� ������ ����
  * @return void
  */
  virtual void OnPathChanged() { ; }

  /**
  * @brief ��ü�� ���� ��ġ�� �ٲ������ �Ҹ��� �Լ�.
  * @return void
  * @details 1. ��Ÿ������ �̵�(DB������ ����ȭ �׸��� Ű ����)
  * @n 2. ������ �̵� �� �̸� ���� ���� ������Ʈ.
  * @n 3. UpdateMetaFolderNameMapping() : ������ ��Ÿ������ ����� ������ ����
  * @n 4. UpdatePathChangeOnMetafile() : ������ ��� �� fullPathIndex�� ������Ʈ & ��Ÿ������ ���丮���� ������.
  */
  virtual void OnPathChanged(LPCTSTR folder, LPCTSTR rootFolder, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow);

  /**
  * @brief ��ü�� ���� ��ġ�� �ٲ������ �Ҹ��� �Լ�.
  * @return void
  * @details 1. ������ ��� �� fullPathIndex�� ������Ʈ
  * @n 2. ��Ÿ������ ���丮���� ������.
  */
  void UpdatePathChangeOnMetafile(LPCTSTR baseFolder, int update_mask, int flag);


  void RefreshPathChanged(LPCTSTR rootFolder);

  /**
  * @brief ����OID, fileOID, storageOID ���� ������Ʈ ��.
  * @return void
  */
  void UpdateServerOID(LPCTSTR serverOID, LPCTSTR fileOID, LPCTSTR storageOID);
  /**
  * @brief mServer�� permission, laseModifierID, fileTime�� ������Ʈ ��.
  * @return void
  */
  void UpdateServerFileInfo(int perm, LPCTSTR lastModifierOID, SYSTEMTIME &fileTime);

  /**
  * @brief mServerNew�� permission, laseModifierID, serverFullPath, fullPathIndex�� ������Ʈ ��.
  * @return void
  */
  void UpdateServerNewFileInfo(int perm, LPCTSTR lastModifierOID, LPCTSTR serverFullPath, LPTSTR fullPathIndex);

  /**
  * @brief mServerNew�� ������ ���� ������Ʈ ��.(������ ���� ������ �����ϰ� �ٽ� �Ҵ���)
  * @return BOOL
  * @param fileSize : ������ ũ��
  * @param fileTime : ������ ������ ���� ��¥
  * @param permissions : ���Ѱ� ��Ʈ�� ǥ��
  * @param lastModifierOID : ������ ������ OID
  * @param fileOID : ���� OID
  * @param storageOID : ���丮�� OID
  * @param serverFullPath : ������ fullPath
  * @param serverFullPathIndex : ������ fullPathIndex
  * @param name : �̸�
  */
  BOOL UpdateServerNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, LPCTSTR permissions, 
    LPCTSTR lastModifierOID, LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR serverFullPath, LPCTSTR serverFullPathIndex, LPCTSTR name);

  /**
  * @brief mServerNew�� ������ ���� ������Ʈ ��.(������ ���� ������ �����ϰ� �ٽ� �Ҵ���)
  * @return BOOL
  * @param fileSize : ������ ũ��
  * @param fileTime : ������ ������ ���� ��¥
  * @param permissions : ���Ѱ�
  * @param lastModifierOID : ������ ������ OID
  * @param fileOID : ���� OID
  * @param storageOID : ���丮�� OID
  * @param serverFullPath : ������ fullPath
  * @param serverFullPathIndex : ������ fullPathIndex
  * @param name : �̸�
  */
  BOOL UpdateServerNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, int permissions, 
    LPCTSTR lastModifierOID, LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR serverFullPath, LPCTSTR serverFullPathIndex, LPCTSTR name);

  /**
  * @brief mLocalNew�� ������ ���� ������Ʈ ��.(������ ���� ������ �����ϰ� �ٽ� �Ҵ���)
  * @return BOOL
  * @param fileSize : ������ ũ��
  * @param fileTime : ������ ������ ���� ��¥
  * @param accessTime : ������ ������ ���� ��¥
  * @param user : ������ ������ ������
  */
  BOOL UpdateLocalNewFileInfo(__int64 fileSize, SYSTEMTIME &fileTime, SYSTEMTIME &accessTime, LPCTSTR user);

  /**
  * @brief ��� �׸��� ���� ������ �� ��ü�� ������.
  * @param nItem : ��� �׸�
  * @details A. ������ ����(mLocalState)�� �����Ѵ�.
  * @n B. �� ���� ����(mLocalNew)�� �����Ѵ�.
  */
  void MergeLocalInformation(KSyncItem *nItem);

  /**
  * @brief ������ �̸� ���� ������ �����Ѵ�.
  * @param newRelativeName : �̸� ���� ����
  * @details A. ���� �̸� ���� ������ ��������� ������ �ְ� TRUE�� �����Ѵ�.
  * @n B. �ƴϸ� �α� �޽����� �����ϰ� FALSE�� �����Ѵ�.
  */
  BOOL SetLocalRenamedAs(LPCTSTR newRelativeName);

  /**
  * @brief �̵��� �θ� ������ OID�� ��Ʈ�Ѵ�.
  * @param newParentServerOID : �� �θ� ������ OID
  * @details A. �θ� ������ OID ������ ��������� ������ �ְ� TRUE�� �����Ѵ�.
  * @n B. �ƴϸ� �α� �޽����� �����ϰ� FALSE�� �����Ѵ�.
  */
  BOOL SetNewParentOID(LPTSTR newParentServerOID);

#if 0
  /**
  * @brief path+name���� ��� ��θ��� ���ۿ� �����Ѵ�.
  * @param relativeName : ��� ��θ��� ������ ����
  * @param path : ���� ��θ�
  * @param name : ���ϸ�
  * @details A. path+name���� ��ü ��θ��� �����.
  * @n B. ��Ʈ ������ ��θ��� ���Ͽ� �� �κ��� �����ϰ� ���� �κ��� relativeName�� �����Ѵ�.
  */
  BOOL GetRelativePathName(LPTSTR OUT relativeName, LPCTSTR IN path, LPCTSTR IN name);

  /**
  * @brief fullname���� ��� ��θ��� ���ۿ� �����Ѵ�.
  * @param relativeName : ��� ��θ��� ������ ����
  * @param fullname : ��ü ��θ�
  * @details A. ��Ʈ ������ ��θ��� ���Ͽ� �� �κ��� �����ϰ� ���� �κ��� relativeName�� �����Ѵ�.
  */
  BOOL GetRelativePathName(LPTSTR OUT relativeName, LPCTSTR IN fullname);
#endif

  /**
  * @brief ���� �Ҵ��� �޸𸮿� ��ü ��θ��� �����Ͽ� �����Ѵ�.
  * @return LPTSTR : ��ü ��θ�
  * @details A. ��� ��θ��� �����,
  * @details B. AllocAbsolutePathName()���� ���� ��θ��� ����� �����Ѵ�.
  */
  LPTSTR AllocFullPathName();
  /**
  * @brief ���� �Ҵ��� �޸𸮿� ��ü ��θ��� �����Ͽ� �����Ѵ�.
  * @return LPTSTR : ��ü ��θ�
  * @details A. �θ� ��ü�� ��θ��� �޾Ƽ� �� ��ü�� �̸��� �ٿ��� ��θ��� ����� �����Ѵ�.
  */
  LPTSTR AllocTreePathName();

  /**
  * @brief ���� �Ҵ��� �޸𸮿� ��� ��θ��� �����Ͽ� �����Ѵ�.
  * @return LPTSTR : ��� ��θ�
  * @details A. ��Ʈ ������ �ƴϸ� 
  *   - mpRelativePath�� NULL�� �ƴϸ� mpRelativePath�� �����Ѵ�.
  *   - �ƴϰ�, �θ� NULL�� �ƴϸ� �θ� ��ü�� AllocActualPathName() �޼ҵ�� ���� ��θ� �޾Ƽ� ���⿡ ���ϸ��� ���δ�.
  *   - �ƴϸ�, ���ϸ� �����Ѵ�.
  *   .
  * @n B. ��Ʈ ������ NULL�� �����Ѵ�.
  */
  LPTSTR AllocRelativePathName();

  /**
  * @brief ��� ��ü ��θ��� ��� ��θ��� ����� �����Ѵ�.
  * @param fullPathName : ��ü ��θ�
  * @return LPTSTR : ��� ��θ�
  * @details A. ��Ʈ ������ ��θ� �޾ƿ´�.
  * @n B. ��ü ��θ��� ��Ʈ ���� ��θ� �κ��� ������ ������ �κи� �����Ͽ� �����Ѵ�.
  */
  LPTSTR AllocRelativePathName(LPCTSTR fullPathName);

  /**
  * @brief ��� ��ο� ���ϸ��� ��� ��θ��� ����� �����Ѵ�.
  * @param fullPath : ��ü ���
  * @param fileName : ���ϸ�
  * @return LPTSTR : ��� ��θ�
  * @details A. ��Ʈ ������ ��θ� �޾ƿ´�.
  * @n B. ��θ��� ��Ʈ ���� ��θ� �κ��� ������ ������ �κи� �����ϰ� ���ϸ��� �ٿ� �����Ѵ�.
  */
  LPTSTR AllocRelativePathName(LPCTSTR fullPath, LPCTSTR fileName);

  /**
  * @brief ��ü ��θ��� ����� �����Ѵ�.
  * @return LPTSTR : ���� ��θ�
  * @details A. AllocRelativePathName() �޼ҵ�� ��� ��θ��� �޾ƿ´�.
  * @n B. AllocAbsolutePathName() �޼ҵ�� ���� ��θ� ����� �����Ѵ�.
  */
  LPTSTR AllocAbsolutePathName();

  /**
  * @brief ��ü ��θ��� ����� �����Ѵ�.
  * @param relativePathName : ��� ��θ�
  * @return LPTSTR : ���� ��θ�
  * @details A. ��Ʈ ������ ��θ� �޾ƿ´�.
  * @n B. ��Ʈ ������ ��ο� ��� ��θ�(relativePathName)�� ���Ͽ� �����Ѵ�.
  */
  LPTSTR AllocAbsolutePathName(LPCTSTR relativePathName);

  /**
  * @brief ��ü ��θ��� ����� �����Ѵ�.
  * deprecated : AllocMakeFullPathName
  * @param rootFolder : ��Ʈ ���� ���
  * @param relativePathName : ��� ��θ�
  * @return LPTSTR : ���� ��θ�
  * @details A. ��Ʈ ������ ��ο� ��� ��θ�(relativePathName)�� ���Ͽ� �����Ѵ�.
  */
  // LPTSTR AllocAbsolutePathName(LPCTSTR rootFolder, LPCTSTR relativePathName);

  /**
  * @brief ������ �Ǵ� ������ ���� ��θ��� ��� ���ۿ� �����ϴ� �����Լ�. ���۰� NULL�̸� �������� �ʰ� ���̰��� ����Ѵ�.
  * @param fullname: ��θ��� ������ ����.
  * @param size : ������ ������ ũ��
  * @param abs : TRUE�̸� ���� ��η�, FALSE�̸� ��� ��η� �����Ѵ�.
  * @return int : ����� ��θ��� ����
  * @details A. �θ� ��ü�� NULL�� �ƴϸ� �θ� ��ü�� ��θ��� �޾ƿ´�.
  * @n B. ������ �̸� ���� ������ ������, 
  *     - abs�� TRUE�̸� (��Ʈ ���� ��� + mpLocalRenamedAs)�� ���ۿ� �����Ѵ�.
  *     - �ƴϸ� (mpLocalRenamedAs)�� ���ۿ� �����Ѵ�.
  * @n C. �ƴϸ� ���ϸ��� ���ۿ� ���Ѵ�.
  */
  virtual int GetActualPathName(LPTSTR fullname, int size, BOOL abs);

  /**
  * @brief ������ �Ǵ� ������ ���� ��θ��� ���� �Ҵ��� ���ۿ� �����Ͽ� �����Ѵ�.
  * @param abs : TRUE�̸� ���� ��η�, FALSE�̸� ��� ��η� �����Ѵ�.
  * @return LPTSTR : ���� ��θ�
  * @details A. GetActualPathName(NULL, 0, abs) �޼ҵ带 ȣ���Ͽ� ���̸� ���� ���Ѵ�.
  * @n B. ���ۿ� �޸𸮸� �Ҵ��Ͽ� �ٽ� GetActualPathName() �� ȣ���ϰ�, ���۸� �����Ѵ�.
  */
  LPTSTR AllocActualPathName(BOOL abs);

  /**
  * @brief ��� ��θ����� ���� ��θ� ����� ��� ���ۿ� �����Ѵ�.
  * @param fullname : ���� ��θ��� ������ ����.
  * @param relativeName : ��� ��θ�
  * @details A. ��Ʈ ������ ��θ� �޾ƿ´�.
  * @n B. ��Ʈ ������ ��ο� ��� ��θ�(relativeName)�� ���ļ� ���ۿ� �����Ѵ�.
  */
  BOOL GetAbsolutePathName(LPTSTR OUT fullname, LPCTSTR IN relativeName);

  /**
  * @brief ��� ��θ� ���� �̵� �� �̸� ���� ������ �޾ƿ´�.
  * @param oldRelativeName : ������ ��� ��θ�.
  * @return LPTSTR : Directory Monitor���� ���� ������ �˻��Ͽ� ���� �̵� �� �̸� ����� ��� ��θ�.
  * @details A. ��Ʈ ���� �׸��� �޾ƿ´�.
  * @n B. KSyncRootFolderItem::AllocMovedRenamedAs()�� ȣ���ϰ� �� ����� �����Ѵ�.
  */
  virtual LPTSTR AllocMovedRenamedAs(LPCTSTR oldRelativeName);

  // BOOL GetLocalRenamedAsByOld(LPCTSTR oldPath, LPCTSTR oldName, LPTSTR renamedAs);

  /**
  * @brief �������� �̵� �� �̸� ���� ������ ������Ʈ �Ѵ�.
  * @param rootItem : ��Ʈ ���� �׸�.
  * @return BOOL : �̵� �� �̸� ���� ������ ��ȿ�ϸ� TRUE�� �����Ѵ�.
  * @details A. ������ �̵� �� �̸� ���� ����(mpLocalRenamedAs)�� �����Ѵ�.
  * @n B. ���� ���°� ���� �������̸� ��� ��θ��� �غ��ϰ�, AllocMovedRenamedAs() �޼ҵ�� �̵� �� �̸� ���� ������ �޾ƿ´�.
  */
  BOOL CheckLocalRenamedAsByOld(KSyncItem* rootItem = NULL);

  /**
  * @brief �������� �̵� �� �̸� ���� ������ ������Ʈ �Ѵ�.
  * @param newPath : ���ο� ���.
  * @param filename : ���ο� ���ϸ�.
  * @return BOOL : �̵� �� �̸� ���� ������ ��ȿ�ϸ� TRUE�� �����Ѵ�.
  * @details A. ������ ��� ��θ��� AllocRelativePathName() �޼ҵ�� �޾ƿ´�.
  * @n B. �Էµ� newPath, filename���� ��� ��θ��� �غ��Ͽ� newPathName�� �޾Ƴ��´�.
  * @n C. ������ �̵� �� �̸� ���� ����(mpLocalRenamedAs)�� �����Ѵ�.
  * @n D. ������ ���� �ΰ��� ��� ��θ��� ���Ͽ� �ٸ���,
  *     - ���� ���� �κ��� �ٲ���� �ڽ��� ��ġ�� �̸��� �ٲ��� �ʾҴٸ� �ٲ��� ���� ������ �Ѵ�.
  *     - �ٲ������ newPathName�� mpLocalRenamedAs�� �����Ѵ�.
  */
  BOOL CheckLocalRenamedAs(LPCTSTR newPath, LPCTSTR filename);

  /**
  * @brief �ڽ��� ������ ���� ���� ������ ����Ǿ����� ���� �̸����� ������ Ŭ�����Ѵ�.
  */
  BOOL CheckLocalRenamedItself();

  /**
  * @brief �������� �̵� �� �̸� ���� ������ ������Ʈ �Ѵ�.
  * @param newPath : ���ο� ���.
  * @param filename : ���ο� ���ϸ�.
  * @return BOOL : �������� �̵� �� �̸� ���� ������ ��ȿ�ϸ� TRUE�� �����Ѵ�.
  * @details A. ������ ��� ��θ��� AllocRelativePathName() �޼ҵ�� �޾ƿ´�.
  * @n B. �Էµ� newPath, filename���� ��� ��θ��� �غ��Ͽ� newPathName�� �޾Ƴ��´�.
  * @n C. ������ �̵� �� �̸� ���� ����(mpServerRenamedAs)�� �����Ѵ�.
  * @n D. ������ ���� �ΰ��� ��� ��θ��� ���Ͽ� �ٸ���,
  *     - ���� ���� �κ��� �ٲ���� �ڽ��� ��ġ�� �̸��� �ٲ��� �ʾҴٸ� �ٲ��� ���� ������ �Ѵ�.
  *     - �ٲ������ newPathName�� mpServerRenamedAs�� �����Ѵ�.
  */
  BOOL CheckServerRenamedAs(LPCTSTR newPath, LPCTSTR filename);

  virtual int UpdateServerRenamedAs(LPCTSTR orgRelativePath, LPCTSTR changedRelativePath);

  /**
  * @brief ������ �̸� ������ Ȯ���Ѵ�.
  * @return BOOL : ���ϸ��� ����Ǿ����� TRUE�� �����Ѵ�.
  */
  BOOL isLocalRenamedItself();

  /**
  * @brief ������ �̵� �� �̸� ������ ��ȿ���� Ȯ���Ѵ�.
  * @return BOOL : ������ �̵� �� �̸� ����Ǿ����� TRUE�� �����Ѵ�.
  */
  BOOL isLocalRenamedAs();

  /**
  * @brief ������ �̵� �� �̸� ������ ��ȿ���� Ȯ���Ѵ�.
  * @return BOOL : ������ �̵� �� �̸� ����Ǿ����� TRUE�� �����Ѵ�.
  */
  BOOL isServerRenamedAs();

  /**
  * @brief �������� �̵��Ǿ����� Ȯ���Ѵ�.
  * @return BOOL : ������ �̵��Ǿ����� TRUE�� �����Ѵ�.
  * @details A. mNewParent�� ��ȿ�ϰ� mParent�� �ٸ��� TRUE�� �����Ѵ�.
  */
  BOOL isServerMovedAs();

  /**
  * @brief ������ �̵� �� �̸������� ��ȿ���� Ȯ���Ѵ�.
  * @return BOOL : ������ �̵��Ǿ����� TRUE�� �����Ѵ�.
  * @details A. mpLocalRenamedAs�� ��ȿ�ϰ�,
  *     - mpLocalRenamedAs�� �ش��ϴ� ������ �������� ������ �����Ѵ�.
  * @n B. mpLocalRenamedAs�� ��ȿ�ϸ� TRUE�� �����Ѵ�.
  */
  BOOL validLocalRenamedAs();

  /**
  * @brief ��Ÿ ���ϸ��� �����Ѵ�.
  * @param filename : ��Ÿ ���ϸ�.
  */
  void SetMetafilename(TCHAR *filename);

  /**
  * @brief ��Ÿ ���ϸ��� �����Ѵ�.
  */
  inline TCHAR *GetMetafilename() { return mpMetafilename; }

  /**
  * @brief ���� �߰��� �׸����� Ȯ���Ѵ�.
  * @details A. ���� ������ ���� ��¥�� ����ְ�, ���� ������ ���� ��¥�� ��������� TRUE�� �����Ѵ�.
  */
  BOOL IsNewlyAdded();

  inline void SetServerScanState(int s) { mServerScanState = s; }
  /**
  * @brief ���� ��ĵ ���¸� �����ϴ� �����Լ�.
  * @param s : ���°�(0 : ��������, ITEM_EXIST : ������, ITEM_DELETED : ������)
  * @param flag : �÷���(WITH_ITSELF, WITH_RECURSE, WITH_CHILD, EXCLUDE_MOVED)
  * @details A. �÷��׿� WITH_ITSELF�� ������ setServerScanState() �޼ҵ带 ȣ���Ѵ�.
  */
  virtual void SetScanServerFolderState(int s, int flag);

  /**
  * @brief ���� ��ĵ ���°��� ���� ���� ���¸� �����ϴ� �����Լ�.
  * @details A. 
  */
  virtual void SetServerStateByScanState();

  /**
  * @brief ��� ������ ���� �����ð��� �ý��ۿ��� �о� ��ȯ�Ѵ�.
  * @param local_time : ������ ������ ���� �����ð�
  * @param server_time : ������ ������ ���� �����ð�(��� ����)
  */
  BOOL GetCurrentFileTime(SYSTEMTIME &local_time, SYSTEMTIME &server_time);

  /**
  * @brief �� ���������� ������Ʈ �Ѵ�.
  * @details A. ������ �� ������ ���Ͻð��� ��ȿ�ϸ� ������Ʈ �Ѵ�(mLocal.FileTime = mLocalNew.FileTime).
  * @n B. ������ �� ������ ���Ͻð��� ��ȿ�ϸ� ������Ʈ �Ѵ�(mServer.FileTime = mServerNew.FileTime).
  * @n C. ������ ������ ���� ũ�⵵ ������Ʈ �Ѵ�.
  * @n D. ������ ���� ������ ������ ������Ʈ �Ѵ�.
  */
  void RefreshOldNew();

  /**
  * @brief �� ��ü�� ��� ��ü�� ������ ���Ͽ� �� ��ü�� ũ�� 1��, ������ -1, ������ 0�� �����Ѵ�.
  * @param item : ���� ��� ��ü
  * @param column : ���� ����
  * @param orderAscend : ����
  * @details A. CompareItem(KSyncItem *item, int column)�� ȣ���Ͽ� ���Ѵ�.
  * @n B. orderAscend�� FALSE�̸� ���� �� ������� �����´�.
  */
  int CompareItem(KSyncItem *item, int column, BOOL orderAscend);

  /**
  * @brief �� ��ü�� ��� ��ü�� ������ ���Ͽ� �� ��ü�� ũ�� 1��, ������ -1, ������ 0�� �����Ѵ�.
  * @param item : ���� ��� ��ü
  * @param column : ���� ����(0 : ���ϸ�, 1 : ���� ���� �ð�, 2 : ������ ���� ũ��, 3 : ���ϸ�,
  * 4 : ������ ���� ������ �̸�, 5 : ������ �����ð�, 6 : ������ ���� ũ��)
  * @details A. �÷��� 0, �Ǵ� 3�̸�, ������ ���� ���� �̸��� ���ϰ� ����� �����Ѵ�.
  * @n B. �÷��� 1�̸�, ������ ���� �ð��� ���ϰ� ����� �����Ѵ�.
  * @n C. �÷��� 2�̸�, ������ ���� ���� ũ�⸦ ���ϰ� ����� �����Ѵ�.
  * @n D. �÷��� 4�̸�, ������ ������ ���� ����� �̸��� ���ϰ� ����� �����Ѵ�.
  * @n E. �÷��� 5�̸�, ������ ���� �ð��� ���ϰ� ����� �����Ѵ�.
  * @n F. �÷��� 6�̸�, ������ ���� ���� ũ�⸦ ���ϰ� ����� �����Ѵ�.
  * @n G. 0�� �����Ѵ�.
  */
  int CompareItem(KSyncItem *item, int column);

#ifdef USE_META_STORAGE
  /**
  * @brief ��ü�� ����ȭ ������ ������ ��Ÿ���� �̸��� �غ���.(DB ���丮�� ��Ŀ����� �ϴ� �� ����)
  * @return BOOL
  * @param baseFolder : ����ȭ ��Ʈ ������ ���� ����
  */
  BOOL MakeMetafilename(LPCTSTR baseFolder);

  /**
  * @brief ��ü�� ����ȭ ������ ������ ��Ÿ���� �غ���.(DB ���丮�� ��Ŀ����� �ϴ� �� ����)
  * @return BOOL
  * @param s : ���丮�� ��ü
  * @param metafile_flag : ��Ÿ���� ó�� �÷���(����, ����)
  */
  virtual BOOL ReadyMetafilename(KSyncRootStorage *s, int metafile_flag);

  /**
  * @brief ��ü�� ����ȭ ������ ��Ÿ���Ͽ��� �о����
  * @return BOOL
  * @param s : ���丮�� ��ü
  * @param mask : �о�� �׸� ����ũ ��
  * @details A. s�� NULL �̸� GetSafeStorage() �޼ҵ�� ���丮�� ��ü�� �غ��Ѵ�.
  * @n B. ReadyMetafilename() �޼ҵ� ȣ���Ѵ�
  * @n C. ��� ��θ��� �غ��ϰ� KSyncRootStorage::LoadMetafile()�޼ҵ带 ȣ���Ͽ� ��Ÿ�����͸� �о�´�.
  */
  BOOL LoadMetafile(KSyncRootStorage *s, DWORD mask=META_MASK_ALL);

  /**
  * @return KSyncRootStorage*
  * @brief ��ü�� ����ȭ ������ ����Ǵ� ���丮�� ��ü�� ��(����ȭ ��Ʈ ���� ��ü�� ������ ����)
  * @details A. ����ȭ ��Ʈ ���� ��ü�� �غ��Ѵ�.
  * @n B. KSyncRootFolderItem::GetStorage() �޼ҵ�� ���丮�� ��ü�� ���ͼ� �����Ѵ�.
  */
  KSyncRootStorage* GetSafeStorage();

  /**
  * @brief ��ü�� ����ȭ ������ ���丮�� ��ü�� ������
  * @return BOOL
  * @param s : ���丮�� ��ü
  * @param mask : ������ �׸� ����ũ ��
  * @param metafile_flag : ��Ÿ���� ó�� �÷���(����)
  */
  BOOL StoreMetafile(KSyncRootStorage *s, DWORD mask = META_MASK_ALL, int metafile_flag = 0);

  /**
  * @brief ��ü�� ��ο� ���� ��Ÿ������ ��ο� �̸��� ������
  * @return BOOL
  * @param prevRelativePathName : ���� ��Ÿ���� �̸�
  * @param updateAlso : BOOL�̸� ��Ÿ �����͵� �����Ͽ� ������Ʈ ��.
  */
  BOOL MoveMetafileAsRename(LPCTSTR prevRelativePathName, int flag, KSyncRootStorage *s = NULL);

#endif // USE_META_STORAGE

  /**
  * @brief ��� �׸��� ������ ������ TRUE�� ������.
  * @return BOOL : ������ ������ TRUE
  * @param onLocal : TRUE�̸� ������ ���, �ƴϸ� ������ ���
  * @details A. ������ ����̸� ����� ���� ���� ���� �����̸�, ������ ����̸� ����� ���� ���� ���� �����̸� FALSE�� �����Ѵ�.
  * @n B. ������ ����̸� 
  *     - �浹 ������ �ٿ�ε� ���Ѿ���, ���� ���� ���� ����, ���� ����̺� ���� ����, ����ȭ ��Ʈ ���� ���� ���� ����,
  *     .
  * ����ȭ ���� ����, ���� ���� ���� ����, ���� ���� ���� ���� ���̸� FALSE�� �����Ѵ�.
  * @n C. IsServerFileExist() �޼ҵ� ȣ���Ͽ� ������ ������ �������� ������ FALSE�� �����Ѵ�.
  * @n D. ������ ���� SFP_READ�� ������ FALSE�� �����Ѵ�.
  * @n E. TRUE�� ������.
  */
  BOOL CanBeReadable(BOOL onLocal);

  /**
  * @brief ��� �׸��� �̸��� �ٲܼ� ������ TRUE�� ������.
  * @return BOOL : �ٲܼ� ������ TRUE
  * @param onLocal : TRUE�̸� ������ ���, �ƴϸ� ������ ���
  * @details A. ������ ����̸� ����� ���� ���� ���� �����̸�, ������ ����̸� ����� ���� ���� ���� �����̸� FALSE�� �����Ѵ�.
  * @n B. ������ ����̸� ������ ���� SFP_WRITE�� ������ FALSE�� �����Ѵ�.
  * @n E. TRUE�� ������.
  */
  BOOL CanBeRenamed(BOOL onLocal);

  /**
  * @brief ��� �׸��� �������� ��� ��ü�� ���� OID�� ������.
  * @return void
  * @param serverOid : �������� ��� ��ü�� ���� OID
  */
  void SetServerOID(LPCTSTR serverOid);

  /**
  * @brief ����ȭ ���� ���¸� ��Ÿ �����Ϳ� �����ϰ�, Ž������ Overlay Icon�� ����� ����ȭ �������� ���·� ���̵��� ��Ʈ�ϰų� ������.
  * @return void
  * @param baseFolder : ����ȭ ��Ʈ ����
  * @param startSync : 1�̸� ����ȭ ������, 0�̸� ����ȭ ������ �ƴ�
  * @param iconUpdateRecurse : TRUE�̸� ������ ���� �� ���ϵ� Shell Icon�� ������Ʈ ��.
  * @details A. ��ü ��θ��� �غ��ϰ� �ش� ������ �����ϸ�
  *     - start sync�� 2���� ������
  *       + startSync�� 0�� �ƴϸ� ������� FILE_NOW_SYNCHRONIZING �÷��׸� �ְ�
  *       + startSync�� 0�̸� ������� FILE_NOW_SYNCHRONIZING �÷��׸� ����.
  *     .
  * @n KSyncCoreBase::ShellNotifyIconChangeOverIcon() �޼ҵ�� Ž���� �������� ������Ʈ�Ѵ�.
  */
  void SetItemSynchronizing(LPCTSTR baseFolder, int startSync, int flag);

  /**
  * @brief ���ð� ������ �׸��� ���¿� ���� �����(���ε�/�ٿ�ε�/����/�̸�����/�浹/���� ��)�� ����.
  * @return void
  * @param userAction : ����� ���� action(���ε�/�ٿ�ε�/����/����)
  * @details A. ���� ���°� ���Ѿ���(ITEM_STATE_NO_PERMISSION)�̸�,
  *     - ����� �׼��� ���ε��̸� ����� FILE_SYNC_NO_UPLOAD_PERMISSION��, �ƴϸ� FILE_SYNC_NO_DOWNLOAD_PERMISSION�� �Ѵ�.
  *     .
  * @n B. �ƴϰ�, ���� ���°� ��ġ �����(ITEM_STATE_LOCATION_CHANGED)�̸�,
  *     - ����� FILE_SYNC_PARENT_FOLDER_MOVED�� �Ѵ�.
  *     .
  * @n C. �ƴϰ�, ���� �Ǵ� ���� ���°� ����(ITEM_STATE_ERROR)�̸�,
  *     - ����� FILE_SYNC_ERROR_FAIL�� �Ѵ�.
  *     .
  * @n D. �ƴϰ�, ���� �Ǵ� ���� ���°� �̵���(ITEM_STATE_MOVED)�̸�,
  *     - ������ �̸� ������ ��ȿ�ϰ�, ������ �̸� ���浵 ��ȿ�ϸ� �ΰ��� ������ �ٸ���,
  * ������� ���� ���� �浹(FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)�� �Ѵ�.
  *     - �ƴϸ�
  *       + ������� FILE_SAME_MOVED�� �Ѵ�.
  *       + ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̰�
  *         + ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̸�, ������� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED�� ���Ѵ�.
  *         + �ƴϰ�, ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED�� ���Ѵ�.
  *         + �ƴϸ� ������� FILE_NEED_UPLOAD�� ���Ѵ�.
  *       + �ƴϸ�
  *         + ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̸�, ������� FILE_NEED_DOWNLOAD�� ���Ѵ�.
  *         + �ƴϰ�, ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� (FILE_NEED_LOCAL_DELETE | FILE_NEED_RETIRED)�� ���Ѵ�.
  *     .
  * @n E. �ƴϰ�, ���� ���°� ����(ITEM_STATE_NONE, �������� ������ ���ݵ� ����)�̸�,
  *     - ���� ���°� ���� ������(ITEM_STATE_NOW_EXIST)�̸�, ������� FILE_NEED_DOWNLOAD�� �Ѵ�.
  *     - �ƴϰ�, ���� ���°� ����(ITEM_STATE_NONE)�̰ų� ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� FILE_NEED_RETIRED�� �Ѵ�.
  *     .
  * @n F. �ƴϰ�, ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̸�,
  *     - ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̸�, ������� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED�� �Ѵ�.
  *     - �ƴϰ�, ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED�� �Ѵ�.
  *     - �ƴϸ� ������� FILE_NEED_UPLOAD�� �Ѵ�.
  *     .
  * @n G. �ƴϰ�, ���� ���°� ������(ITEM_STATE_DELETED)�̸�,
  *     - ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� FILE_NEED_RETIRED�� �Ѵ�.
  *     - �ƴϰ�, ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED) �Ǵ� �̵���(ITEM_STATE_MOVED)�̸�,
  *     .
  * ������� FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED�� �Ѵ�.
  *     - �ƴϰ�, ���� ���°� ���� ������(ITEM_STATE_NOW_EXIST)�̸�, ������� (FILE_NEED_SERVER_DELETE|FILE_NEED_RETIRED)�� �Ѵ�.
  *     - �ƴϸ� ������� FILE_NEED_RETIRED�� �Ѵ�.
  *     .
  * @n H. �ƴϰ�, ���� ���°� ���� �߰���(ITEM_STATE_NEWLY_ADDED)�̸�,
  *     - ���� ���°� ���� �߰���(ITEM_STATE_NEWLY_ADDED)�̸�, ������� FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED�� �Ѵ�.
  *     - �ƴϸ� ������� FILE_NEED_UPLOAD�� �Ѵ�.
  *     .
  * @n I. �ƴϰ�, ���� ���°� �̵���(ITEM_STATE_MOVED)�̸�,
  *     - ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED�� �Ѵ�.
  *     - �ƴϰ� ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̸�, ������� FILE_NEED_DOWNLOAD�� �Ѵ�.
  *     .
  * @n J. �ƴϸ�, 
  *     - ���� ���°� ������Ʈ��(ITEM_STATE_UPDATED)�̸�, ������� FILE_NEED_DOWNLOAD�� �Ѵ�.
  *     - �ƴϰ� ���� ���°� ������(ITEM_STATE_DELETED)�̸�, ������� (FILE_NEED_LOCAL_DELETE | FILE_NEED_RETIRED)�� �Ѵ�.
  *     - �ƴϰ� ���� ���°� ���� �߰���(ITEM_STATE_NEWLY_ADDED)�̸�, ������� FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED�� �Ѵ�.
  *     - �ƴϰ� ���� ���°� �̵���(ITEM_STATE_MOVED)�̸�, ������� FILE_NEED_LOCAL_MOVE�� �Ѵ�.
  *     - �ƴϰ� ���� ���°� ����(ITEM_STATE_NONE, �������� ������ ���ݵ� ����)�̸�, ������� FILE_NEED_UPLOAD�� �Ѵ�.
  *     .
  * @n K. userAction�� ����� ���� ������ ������ �� ���� �����Ѵ�.
  * @n L. ������� ���� �̵��̳� ���Ű� �ƴϸ�,
  *     - ���� ���°� �̵����̸�,
  *       + ���� ���°� �������̸�, ������� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED�� �Ѵ�.
  *       + ������� 0�̸� FILE_NEED_SERVER_MOVE�� ���Ѵ�.
  *     - ���� ���°� �̵����̸�,
  *       + ���� ���°� �������̸�, ������� FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED�� �Ѵ�.
  *       + ������� 0�̸� FILE_NEED_LOCAL_MOVE�� ���Ѵ�.
  *     .
  * @n M. ������� �����Ѵ�.
  */
  int makeResultFromState(int userAction, int syncMethod);

  /**
  * @brief ����ȭ ��ü ������ �����ϴ��� �˻���.
  * @return BOOL : ����ȭ ��ü�� �����ϸ� TRUE
  * @param fullname : ����ȭ ������ ���� ��θ�
  * @details IsFileExist()�޼ҵ带 ȣ���Ͽ� ������ �����ϴ��� �˻��ϰ� �� ����� �����Ѵ�.
  */
  virtual BOOL IsItemExist(LPCTSTR fullname);

  /**
  * @brief ����ȭ ��ü�� �����ϴ��� �˻���.
  * @return BOOL : ����ȭ ��ü�� �����ϸ� TRUE
  * @param onLocal : TRUE�̸� ������ ���, FALSE�̸� ������ ���
  * @details A. onLocal�� TRUE�̸� IsLocalFileExist()�޼ҵ带 ȣ���Ͽ� ���� ������ �����ϴ��� �˻��ϰ� �� ����� �����Ѵ�.
  * @n B. onLocal�� FALSE�̸� 
  *     - IsServerFileExist() �� TRUE�̰ų� �浹 ������� ���� �ű� �߰� �浹�� ��쿡�� �����ϴ� ������ �����ϰ�,
  *     - ���ٸ� �浹 ������ 0�� �ƴϰ� mServerNew�� ���� ��ȿ�ϸ�(FileTime�� ��ȿ) �����ϴ� ������ �����Ѵ�.
  *     - mServerState�� ITEM_STATE_DELETED�� ������ ���� ������ ó���Ѵ�.
  */
  BOOL IsSyncFileExist(BOOL onLocal);

  /**
  * @brief ����ȭ ��Ʈ ���� �������� Ʈ�� �������� �ش� ��ü�� depth���� ���ϴ� �����Լ�.
  * @return int : depth in tree(0 for root object)
  * @details A. mParent�� NULL�̸�(�̴� ����ȭ ��Ʈ ����) 0�� �����ϰ�,
  * @n B. NULL�� �ƴϸ� mParent�� GetDepth() �޼ҵ带 ȣ���ϰ� ���Ϲ��� ���� 1�� ���Ͽ� �����Ѵ�.
  */
  virtual int GetDepth();

  /**
  * @brief ������ ��� ��ü�� ���¿� ������ ��� ��ü�� ���¸� ���Ͽ� ó���� ����� ������.
  * @return int
  * @param baseFolder : ����ȭ ��Ʈ ������ ���� ��ġ(����ȭ ��� ��ü���� ��� ��θ��� ������ ������, 
  * �� ��Ʈ ������ ��ġ�� �������� ���� ��θ� �����Ͽ� �۾��Ѵ�)
  * @details A. GetSyncResultName()���� ������ ��� ������ ��θ��� �غ��Ѵ�.
  *     - �� ������ �������̸� �� �׸��� ����ȭ �۾��� ��ŵ�ϵ��� phase�� PHASE_END_OBJ_JOB���� �����ϰ�,
  *     - AddSyncFlag() �޼ҵ带 ȣ���Ͽ� mFlag�� SYNC_ITEM_EDITING_NOW���� �����Ѵ�.
  *     .
  * @n B. checkAddedRenamedSyncError()�� ȣ���Ͽ� ����� TRUE�̸� 0�� �����Ѵ�.
  * @n C. storage ��ü�� �غ��Ѵ�.
  * @n D. ������ ��� ���ϸ��� ������ ���� �̻��̸� r�� FILE_SYNC_OVER_PATH_LENGTH�� �ϰ� I.������ �����Ѵ�.
  * @n E. ReadyMetafilename() �޼ҵ�� ��Ÿ ������ ������ �غ��Ѵ�. �����ϸ� ����� FILE_SYNC_CANNOT_MAKE_METAFILE�� �ϰ� �����Ѵ�.
  * @n F. ��Ÿ���ϸ��� �غ�Ǿ��� ������ �������� ������ ���� �����Ѵ�.
  * @n G. compareLocalServerState()�޼ҵ带 ȣ���Ͽ� ������ ������ ���¸� ���ϰ�,
  * @n H. makeResultFromState()�޼ҵ带 ȣ���Ͽ� ����� �����Ѵ�.
  * @n I. �񱳸� �������Ѵ�.
  *     - ����� �浹�̳� ������ ������, ���ŵ�(FILE_NEED_RETIRED) ���� �ƴϸ� �ٽ� ��� ���ϸ��� �غ��ϰ�, 
  * ��� ���ϸ��� �ִ� ���̸� ������ ����� ����(FILE_SYNC_CANNOT_UPLOAD_LONGPATH �Ǵ� FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH)�� �����Ѵ�.
  *     .
  * @n J. ����� �����ϰ�, �浹 ����� �����Ѵ�.
  * @n K. ���ð� ������ �ٸ��� �̸� ������ �Ǿ����� ������� ����(FILE_NEED_UPLOAD|FILE_NEED_DOWNLOAD|FILE_SAME_MOVED)�� �浹 ����� �߰��Ѵ�.
  * @n L. ����� ������ �ٸ��� StoreMetafile() �޼ҵ带 ȣ���Ͽ� ����� ���°����� �����Ѵ�.
  */
  virtual int compareSyncFileItem(LPCTSTR baseFolder, int syncMethod);

  BOOL checkNewFolderExcluded(LPTSTR newPathName);

  /**
  * @brief �׸��� localState/serverState�� �⺻ ���¸� ���Ͽ� localState/serverState�� ����/�̵�/���� ���� �÷��� ������
  * @return void
  * @details A. ���� ���°��� ���Ѿ���(ITEM_STATE_NO_PERMISSION), �浹â���� �̸�����(ITEM_STATE_CONFLICT_RENAMED) ���� ���°��� �ӽ� �����Ѵ�.
  * @n B. ���°����� ���ſ� ������ ���� ����(ITEM_STATE_EXISTING)���� �����.
  * @n C. ���� ���°� ���� �߰��� �����̸�(mLocalState == ITEM_STATE_NOW_EXIST)
  *     - ���� ���¿� ITEM_STATE_NEWLY_ADDED �÷��׸� �߰��ϰ�, 
  *     - ������ ������ �������� ������ ITEM_STATE_DELETED �÷��׸� �߰��Ѵ�.
  *     .
  * @n C. ���� ���°� ������ �����̸�(mLocalState == ITEM_STATE_WAS_EXIST)
  *     - ���� ���¿� ITEM_STATE_DELETED �÷��׸� �߰��Ѵ�.
  *     .
  */
  void compareLocalServerState();

  /**
  * @brief ���� �߰��ǰų� �̵��� �׸��� ó���Ҷ�, ������ �̸��� ������ �����ϴ��� �˻��ϰ� �����ϸ� �浹 ���°� �ǵ��� ������.
  * @return BOOL : ����ȭ ������ �߻��ϸ� TRUE�� �����Ѵ�.
  * @param user_action : �浹 ó��â���� ����ڰ� ���ε�/�ٿ�ε� ������ ������ ���, �浹 ó���� �ƴ� ����ȭ ���� �ܰ迡���� 0��.
  * @details A. ����� ���� ������ ���ų� ���÷� ������ �����Ͽ���,
  *     - ���������� ���� �߰��Ǿ���, �������� ������, ��� ������ �����ϸ�
  * ����ȭ ���� FILE_SYNC_ALREADY_EXISTS_ON_LOCAL�� �����ϰ� TRUE�� �����Ѵ�.
  * @n B. ����� ���� ������ ���ų� ������ ������ �����Ͽ���,
  *     - ���������� ���� �߰��Ǿ���, �������� ������, ����ȭ ���� ���� ��� �̸��� ������ ��θ����� �̸����� �Ǵ� �̵��� �׸��� ������
  * ����ȭ ���� FILE_SYNC_ALREADY_EXISTS_ON_SERVER�� �����ϰ� TRUE�� �����Ѵ�.
  */
  BOOL checkAddedRenamedSyncError(int user_action);

  /**
  * @brief ����� ������ ������ ���ǿ� ���� ������ phase�� �ٲ۴�.
  * @return void
  * @param added_phase : ���� �߰��� �׸��̸� �� phase���� �����Ѵ�.
  * @param renamed_phase : �̸� ����/�̵��� �׸��̸� �� phase���� �����Ѵ�.
  * @param normal_phase : ���� �׸��̸� �� phase���� �����Ѵ�.
  * @param parentCheckNeed : Child �׸��� ����Ǿ����� Parent �׸� ���߾� �����Ѵ�.
  * @details A. ����� ������ ������(FILE_NOT_CHANGED)
  *     - ���� �Ǵ� ������ �̸� ���� �Ǵ� �̵��� ������ renamed_phase�� �����Ѵ�.
  *     - �ƴϸ� normal_phase�� �����Ѵ�.
  *     .
  */
  virtual void SetUnchangedItemPhase(int added_phase, int renamed_phase, int normal_phase, BOOL parentCheckNeed);

  /**
  * @brief ����ȭ ������� phase���� ������ ������ �����Ѵ�.
  * @param result : ������ �����.
  * @param phase : ������ phase��
  * @param flag : �÷���(SET_SELECTED_ONLY, SET_WITH_ITSELF, EXCLUDE_OUTGOING, SET_SERVER_DELETED, SET_LOCAL_DELETED)
  */
  virtual void SetConflictResultRecurse(int result, int phase, int flag);

  /**
  * @brief �� �׸�� ���� �׸���� ����ȭ �浹 ó���� ���� phase�� �ٲ۴�.
  * @return BOOL : �� �׸��� ����ȭ Enable�� �Ǿ� ������ TRUE�� �����Ѵ�.
  * @param parent_phase : �θ� �׸��̸� �� phase���� �����Ѵ�.
  * @param renamed_phase : �̸� ����/�̵��� �׸��̸� �� phase���� �����Ѵ�.
  * @param normal_phase : ���� �׸��̸� �� phase���� �����Ѵ�.
  * @details A. ����ȭ �����ϰ�,
  *     - �������̳� �������� �̸� ���� �Ǵ� �̵��� ������ PHASE_MOVE_FOLDERS�� �����Ѵ�.
  *     - �ƴϸ� normal_phase�� �����Ѵ�.
  *     .
  * @n B. ����ȭ ���ܵ� �׸��̸� PHASE_END_OF_JOB���� �����Ѵ�.
  */
  virtual BOOL SetPhaseOnResolve(int parent_phase, int renamed_phase, int normal_phase);

  /**
  * @brief ����ȭ �浹 �� ���� �߻��� �׸񿡼� ó���� ���� �غ� �Ѵ�(������ ������ �ٽ� ���ϰ� ���ѵ� ���ϰ�, ���µ� �ٽ� ������Ʈ�Ѵ�)
  * @return int : �浹 ó�� �غ� �Ϸ�Ǹ� R_SUEECSS��, ������ �浹�� �ٽ� �߻��ϸ� �ش� �ڵ带 �����Ѵ�(FILE_CONFLICT or FILE_SYNC_ERR)
  * @param rootItem : ����ȭ ��Ʈ ���� �׸� ��ü
  * @param phase : ������ ����ȭ �۾� phase
  * @details A. ����ȭ Enable�� �ƴϸ� R_SUCCESS�� �����Ѵ�.
  * @n B. ������ �̸�����/�̵� ����θ� ������Ʈ �Ѵ�.
  * @n C. PHASE_CONFLICT_FILES�̸� user_action ���뿡 ���� �α� ������ �����Ѵ�.
  * @n D. PHASE_CONFLICT_FILES�̸�,
  *     - ������ �׸��� �ٽ� ����Ǿ����� CheckServerModifiedAgain()���� Ȯ���Ѵ�.
  *     - Ȯ���� ���� 0�� �ƴϸ� mServerState�� ���� ���ϰ�, 
  *     - Ȯ���� ���� ITEM_STATE_UPDATED�̰�, ����� action�� ���ε� �Ǵ� �ٿ�ε��̸�, mServerState�� 
  *         ITEM_STATE_RE_UPDATED�� ���ϰ�, mServerNew�� ���� ���� ���� ������ merge�ϰ�, R_SUCCESS�� �����Ѵ�.
  *     - Ȯ���� ���� ITEM_STATE_DELETED(������ ������ ������)�̸�, mServerState�� ITEM_STATE_DELETED�� �ٲ۴�.
  *     - mServerState�� ITEM_STATE_RE_UPDATED�� ������ �������� ����� ������ �Ѵ�.
  *     - permission���� ����Ǿ����� ������Ʈ�Ѵ�.
  *     .
  * @n E. ����ȭ ��Ʈ ���� ��θ��� �غ��ϰ�,
  * @n F. ���� ������ �����ϰ�, mLocalState�� ������ �����ϰų� ����������
  *     - IsLocalModified()�޼ҵ带 ȣ���Ͽ� ������ ����Ǿ����� Ȯ���ϰ�
  *     - IsLocalModifiedAgain()���� ������ �ٽ� ����Ǿ���, ����� action�� ���ε� �Ǵ� �ٿ�ε��̸�, mLocalState�� 
  *         ITEM_STATE_RE_UPDATED�� ���ϰ�, mLocalNew�� ���� ���� ������ merge�ϰ�, R_SUCCESS�� �����Ѵ�.
  *     - �����̸�, ��� ���ϸ��� �غ��ϰ� �ش� ������ �������� �����̸� mFlag�� SYNC_ITEM_EDITING_NOW �÷��׸� ���ϰ�, 
  * �������� �ƴϸ� SYNC_ITEM_EDITING_NOW �÷��׸� ����.
  *     .
  * @n G. ������ �Ǵ� �������� �̵� �Ǵ� �̸������ ��쿡
  *     - ������ ��ΰ� �ٲ� ��쿡
  *       + �ٲ� ����� ����ȭ �׸�(KSyncFolderItem)�� ã�ƺ��� ������ ���� �α׸� ����� ������ �̵� ������ Ŭ�����Ѵ�.
  *     .
  * @n H. �������� ������ �����ߴ� ��쿡
  *     - ���� ������ ���� �������� ������, ���� ���°����� ITEM_STATE_NOW_EXIST|ITEM_STATE_UPDATED|ITEM_STATE_MOVED�� �����,
  *     - ���� ������ ���� �����ϸ� ���� ���°����� ITEM_STATE_NOW_EXIST�� ���Ѵ�.
  *     .
  * @n I. ���� ���¿� ITEM_STATE_RE_UPDATED�� ������ local_changed = TRUE�� �Ѵ�.
  * @n J. checkAddedRenamedSyncError()�� ȣ���Ͽ� TRUE�̸� R_SUCCESS�� �����Ѵ�.
  * @n K. compareLocalServerState()�� ȣ���Ͽ� ������ ������ ���°��� ���Ѵ�.
  * @n L. ���� ������ ����Ǿ����� mServerState�� ITEM_STATE_UPDATED�� ���Ѵ�.
  * @n M. ���� ������ ����Ǿ���, ���� ������ �����ϸ� mLocalState�� ITEM_STATE_UPDATED�� ���Ѵ�.
  * @n N. �������� ������ ������ �̵� �Ǵ� �̸������ ��쿡
  *     - ���� ������ ���� ��θ� �ٽ� �޾ƿ��� ���⿡ ������ �̸��� ���δ�.(��� ��θ�)
  *     - ���� ���� mpLocalRenamedAs�� ������ �����ϸ� ������ �̸����� ������ Ŭ�����ϰ�, ���°����� ITEM_STATE_MOVED �÷��׵� Ŭ�����Ѵ�.
  *     .
  * @n O. ���� ���°��� �����Ͽ��ų� ������ ��
  *     - ������ ������ �÷��װ� ����, fullPathIndex�� ��ȿ�ϰ�, fullPathIndex�� ����Ǿ��� ��,
  *       + mServerState�� ITEM_STATE_MOVED �÷��׸� ���ϰ�,
  *       + �� ��ü�� �����̸�, ���ο� ������ ��θ��� mpServerRenamedAs�� �����Ѵ�.
  *     - ������ �̵��� �÷��װ� ������,
  *       + ������ ��� ��θ�� ������ ��� ��θ��� ���ϰ�, ���� ��θ��� �ٸ���,
  *         + ���� ���°��� ITEM_STATE_MOVED �÷��׸� ���ϰ�, mpServerRenamedAs�� ������ ��� ��θ��� �����Ѵ�.
  *       + ���� ��θ��� ������,
  *         + ���� ���°��� ITEM_STATE_MOVED �÷��׸� ����, mpServerRenamedAs�� Ŭ�����Ѵ�.
  *     .
  * @n P. ����ڰ� ������ ������ �÷��װ� ������ ITEM_STATE_MOVED �÷��׸� ����.
  * @n Q. ���� ���� �߰��� �浹�̰�, ������ �Ǵ� �������� �̵��� ��� ���� �׸��� �����Ͽ� �߰��ϰ� ���� ���ε�/�ٿ�ε�� �����ϰ� �����Ѵ�.
  * @n R. ����ȭ ������ �߻��� ���,
  *     - ������ �Ǵ� �������� ������Ʈ �÷��װ� �ִ� ���,
  *       + ����ڰ� ���ε带 ������ ���, 
  *         + ���� ���¿� ���� �����ϸ� ITEM_STATE_UPDATED �÷��׸� ���Ѵ�.
  *         + �浹 ó�� phase�̸�(phase == PHASE_CONFLICT_FILES), ���� ���¿��� ITEM_STATE_UPDATED|ITEM_STATE_DELETED �� Ŭ�����Ѵ�.
  *       + ����ڰ� �ٿ�ε带 ������ ���, ���� ���¿� ITEM_STATE_UPDATED�� �߰��Ѵ�.
  *     - ������ �Ǵ� �������� �̵� �÷��װ� �ִ� ���,
  *       + ����ڰ� ���ε带 ������ ���, ������ ���¿��� �̵� �÷��׸� �����, ������ ���¿� �̵� �÷��׸� �ִ´�.
  *       + ����ڰ� �ٿ�ε带 ������ ���, ������ ���¿��� �̵� �÷��׸� �ְ�, ������ ���¿� �̵� �÷��׸� �����.
  *     .
  * @n S. makeResultFromState() �޼ҵ带 ȣ���Ͽ� ����� �����.
  * @n T. ����ڰ� ������ �����Ͽ�����,
  *     - ����ڰ� �ٿ�ε� �����Ͽ���, �������� �����Ǿ��ų� �������� ���� �������� ������(ITEM_STATE_EXISTING) user_action�� ���� ������ �Ѵ�.
  *     - ����ڰ� ���ε� �����Ͽ���, �������� �����Ǿ��ų� �������� ���� �������� ������(ITEM_STATE_EXISTING) user_action�� ���� ������ �Ѵ�.
  *     - d ���� user_action���� �ִ´�.
  *     - ���� ���¿� ���� �������� �ʰ�, d���� ���� ���� �÷��װ� ������ ���� ���� �÷��׸� �����Ѵ�.
  *     - ���� ���¿� ���� �������� �ʰ�, d���� ���� ���� �÷��װ� ������ ���� ���� �÷��׸� �����Ѵ�.
  *     - ����ȭ ������ �߻��Ǿ�����,
  *       + ���� ���°� �������̸�
  *         + ����ڰ� ���ε带 �����Ͽ�����, d���� ���ε�(FILE_NEED_UPLOAD)�� ��Ʈ�Ѵ�.
  *         + ����ڰ� �ٿ�ε带 �����Ͽ�����, d���� ���� ����(FILE_NEED_LOCAL_DELETE)�� ��Ʈ�Ѵ�.
  *       + ���� ���°� �������̸�
  *         + ����ڰ� �ٿ�ε带 �����Ͽ�����, d���� �ٿ�ε�(FILE_NEED_DOWNLOAD)�� ��Ʈ�Ѵ�.
  *         + ����ڰ� ���ε带 �����Ͽ�����, d���� ���� ����(FILE_NEED_SERVER_DELETE)�� ��Ʈ�Ѵ�.
  *     - d���� ���� �� ���� ���� �÷��װ� ������, FILE_NEED_RETIRED�� �����ϰ�,
  *     - d���� ���� �� ���� ���� �÷��װ� ������, FILE_NEED_RETIRED�� �����Ѵ�.
  *     .
  * @n U. phase�� PHASE_COPY_FILES �̻��̰�, d���� �ٿ��/���ε�/���� �÷��װ� ������, d������ MOVE �÷��׸� �����.
  * @n V. d���� ���, �浹 ��������� �����ϰ� R_SUCCESS�� �����Ѵ�.
  */
  virtual int ReadyResolve(KSyncItem *rootItem, int phase, int syncMethod);

  /**
  * @brief �ش� phase�� ���� ����ȭ �۾��� �����Ѵ�.
  * @return int : �����ϸ� R_SUEECSS��, ������ �浹�� �߻��ϸ� �ش� �ڵ带 �����Ѵ�(FILE_CONFLICT or FILE_SYNC_ERR)
  * @param policy : ����ȭ ��å
  * @param baseFolder : ���� ����ȭ ��Ʈ ����
  * @param phase : ����ȭ �۾� �ܰ�
  * @details A. ����ȭ �۾� �ܰ踦 phase�� �����Ѵ�.
  * @n B. ����ȭ �۾� �ܰ谡 �ε��϶�(phase == PHASE_LOAD_FILES) d�� ������� ��������, �̵� �÷��׸� �����.
  * @n C. ����ȭ �۾� �ܰ谡 �̵� �Ǵ� �����϶�(PHASE_MOVE_FOLDERS or PHASE_COPY_FILES)
  *     - d�� ������� ��������,
  *     - d�� ������ �浹�� ������ ������ �浹���� r�� �����ϰ� �۾��� �������Ѵ�.(goto end_sync_copy)
  *     - phase�� PHASE_COPY_FILES�̰�, d�� �ٿ�ε�/���ε�/������ ������ d���� �̵� �÷��׸� �����.
  *     .
  * @n D. ����ȭ �۾� �ܰ谡 �浹ó��(PHASE_CONFLICT_FILES)�϶�,
  *     - d�� �浹 ������� ��������,
  *       + d�� �浹�̰�, d�� ����ڰ� ������ �������� �ʾ�����, r�� �浹���� �ְ� �۾��� �������Ѵ�.(goto end_sync_copy)
  *     - Enable���°� �ƴϸ�(mEnable == FALSE), �α׸� �����ϰ� R_SUCCESS�� �����Ѵ�.
  *     - ������ ������ �̸��� �ٸ���
  *       + d�� ���ε��̰�, mServerState�� 0�� �ƴϸ�,
  *         + mpLocalRenamedAs�� ����ְų�, ���� ���¿� �̵� �÷��װ� ������ 
  *           + ���� ���¿� �̵� �÷��׸� �ְ�, ���� ���¿� �̵� �÷��׸� ����, �̸� ���� ������ Ŭ�����ϰ�, ������ ��� ���ϸ��� mpLocalRenamedAs�� �����Ѵ�.
  *       + d�� �ٿ�ε��̰�, mLocalState�� 0�� �ƴϸ�,
  *         + mpServerRenamedAs�� ����ְų�, ���� ���¿� �̵� �÷��װ� ������,
  *           + ���� ���¿� �̵� �÷��׸� �ְ�, ���� ���¿� �̵� �÷��׸� ����, �̸� ���� ������ Ŭ�����ϰ�, ������ ��� ���ϸ��� mpLocalRenamedAs�� �����Ѵ�.
  *     .
  * @n E. ������ ��� ��θ��� �����, 
  *     - �� ������ �ٸ� ���α׷����� �������̸� �α׿� �����ϰ� R_SUCCESS�� �����Ѵ�.
  *     - �� ��θ��� �ִ� ���̸� ������ r�� FILE_SYNC_OVER_PATH_LENGTH�� �����ϰ�, �۾��� �������Ѵ�.
  *     .
  * @n F. d���� ���� �÷���(FILE_NEED_RETIRED)�� ������
  *     - d������ ���� �÷���(FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE)�� �����,
  *     - ������ ��� ���ϸ��� ���� �ش� ������ �����ϸ� d�� FILE_NEED_LOCAL_DELETE �÷��׸� �ְ�,
  *     - ������ ������ �����ϸ� d�� FILE_NEED_SERVER_DELETE �÷��׸� �ִ´�.
  *     .
  * @n G. d������ �̵� �÷���((FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE))�� �����.
  * @n H. d���� ���� �̵�(FILE_SAME_MOVED)�÷��׸� �˻��Ͽ�,
  *     - ������ �α׿� �� ������ ����Ѵ�.
  *     - ������,
  *       + ���� ���°� �̵� �÷��׸� ������, ���� ���µ� �̵� �÷��׸� ��������,
  *         + d���� ���ε尡 ������ d�� ������ �̵� �÷���(FILE_NEED_SERVER_MOVE)�� ���ϰ�,
  *         + �ƴϰ�, d���� �ٿ�ε尡 ������ d�� ������ �̵� �÷���(FILE_NEED_LOCAL_MOVE)�� ���Ѵ�.
  *       + �ƴϸ�,
  *         + d���� ���ε�/�ٿ�ε尡 ����, d���� ����(FILE_NEED_RETIRED)�� ������,
  *           + ���� ���°� �̵��̸� d���� ������ �̵�(FILE_NEED_SERVER_MOVE)�� ���ϰ�
  *           + �ƴϰ�, ���� ���°� �̵��̸� d���� ������ �̵�(FILE_NEED_LOCAL_MOVE)�� ���Ѵ�.
  *         + �ƴϸ�,
  *           + d���� ���ε尡 ������,
  *             + ���� ���°� �̵� �÷��׸� �������� d�� ������ �̵� �÷���(FILE_NEED_SERVER_MOVE)�� ���ϰ�,
  *             + �ƴϰ�, ���� ���°� �̵� �÷��׸� �������� d�� ������ �̵� �÷���(FILE_NEED_LOCAL_MOVE)�� ���Ѵ�.
  *           + d���� �ٿ�ε尡 ������,
  *             + ���� ���°� �̵� �÷��׸� �������� d�� ������ �̵� �÷���(FILE_NEED_LOCAL_MOVE)�� ���ϰ�,
  *             + �ƴϰ�, ���� ���°� �̵� �÷��׸� �������� d�� ������ �̵� �÷���(FILE_NEED_SERVER_MOVE)�� ���Ѵ�.
  *     .
  * @n I. d���� ���ε� �Ǵ� �ٿ�ε�(FILE_SAME_MOVED)�� ������,
  *     - ������ ũ�⸦ ���ϰ�, ��� ���ϸ��� ���Ѵ�.
  *     - IsSyncExcludeFileCondition() �޼ҵ�� ����ȭ ���� ���� ���θ� üũ�Ͽ� ���� �����̸�,
  *       + ����� FILE_SYNC_OVER_FILE_SIZE_LIMIT�� �ϰ� �۾��� �������Ѵ�.
  *     .
  * @n J. d���� ���� �̵� �Ǵ� �ٿ�ε尡 ������,
  *     - ��� ���ϸ��� ���̰� ������ ������ ����� FILE_SYNC_CANNOT_DOWNLOAD_LONGPATH�� �ϰ� �۾��� �������Ѵ�.
  *     .
  * @n K. d���� ���ε� �Ǵ� �ٿ�ε尡 �ְ�, ���� ���°� �ٽ� ����(ITEM_STATE_RE_UPDATED)�� ��쿡,
  *     - ����� FILE_SYNC_SERVER_MODIFIED_AFTER_COMPARE�� �ϰ� �۾��� �������Ѵ�.
  *     .
  * @n L. d���� ���ε� �Ǵ� �ٿ�ε尡 �ְ�, ���� ���°� �ٽ� ����(ITEM_STATE_RE_UPDATED)�� ��쿡,
  *     - ����� FILE_SYNC_LOCAL_MODIFIED_AFTER_COMPARE�� �ϰ� �۾��� �������Ѵ�.
  *     .
  * @n M. d���� ���� �̵�(FILE_SAME_MOVED)�̸�,
  *     - updateMetaAsRenamed() ȣ���Ͽ� �̸� ����� ������ ��Ÿ �����Ϳ� �ݿ��ϰ�
  *     - ������� �̵� �� ���� �����浹�� �����ϰ�, meta_mask�� META_MASK_FILENAME�� ��Ʈ�Ѵ�(��Ÿ �����Ϳ��� FILENAME �κи� ������Ʈ ��)
  *     .
  * @n N. d���� ���� �̵��� ������,
  *     - d���� ������ �̵��� ������,
  *       + d���� ���� ���� ���� ���� �浹�̸� rename_need�� FALSE�� �ϰ�(���� �̸� ���� ó���� ���� ����), able�� TRUE�� �Ѵ�.
  *       + �ƴϸ�, 
  *         + able�� ���ε� ���� ����(����� ����̺� ��å�� ���� ����)���� �ϰ�,
  *         + ������ ���� ���Ѱ��� �޾ƿ���, CheckPermissionFileRename()���� ���� üũ�� ����� able�� ��Ʈ�Ѵ�.
  *       + able�� TRUE�̸�, 
  *         + ���� rename�� �ʿ��� ��Ȳ���� üũ�ϰ� renameServerObjectAs() �޼ҵ带 ȣ���Ѵ�.
  *         + ���� ����� �����̸� 
  *           + ��������� (FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)�� �����.
  *           + rename_need�� TRUE�̰� ���� ������ ���� ���°� �ƴϸ� 
  *             + ��� ������ FILE_NEED_UPLOAD�� �����.
  *           + �ƴϸ�
  *             + ���� ���¿� ITEM_STATE_UPDATED�� ���Ѵ�.
  *       + able�� FALSE�̸�, 
  *         + ������� FILE_SYNC_NO_RENAME_PERMISSION�� �Ѵ�.
  *     - d���� ������ �̵��� ������,
  *       + d���� ���� ���� ���� ���� �浹�̸� rename_need�� FALSE�� �Ͽ� renameLocalObjectAs()�� ȣ���ϰ�(���� �̸� ���� ó���� ���� ����), 
  * ��������� (FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)�� ����.
  *       + �ƴϸ�,
  *         + able�� ���ε� ���� ����(����� ����̺� ��å�� ���� ����)���� �ϰ�,
  *           + able�� TRUE�̸�,
  *             + ���� ������ �����ϸ�
  *               + renameLocalObjectAs() �޼ҵ带 ȣ���Ͽ� ���� ������ �̸� ������ ó���ϰ� ����� �����̸�,
  *     .
  * ��������� (FILE_NEED_LOCAL_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED)�� ����,
  *                 + ���� ���°� �������� �ʾҰ�, ���� ���µ� ����(ITEM_STATE_UPDATED)���� �ʾ����� ������� FILE_NEED_DOWNLOAD�� ����.
  *                 + �ƴϸ� ���� ���¿� ITEM_STATE_UPDATED�� ���Ѵ�.
  *                 + meta_mask�� META_MASK_FILENAME�� ��Ʈ�Ѵ�(��Ÿ �����Ϳ��� FILENAME �κи� ������Ʈ ��)
  *             + ���� ������ �������� ������
  *                 + ��������� FILE_NEED_LOCAL_MOVE�� ����.
  *           + able�� FALSE�̸�,
  *             + ������� FILE_SYNC_NO_RENAME_PERMISSION�� �Ѵ�.
  *     - ������� ����ȭ ������ ������ �۾��� �������Ѵ�.
  *     .
  * @n O. d���� ���ε尡 �ְ�,�������� ����/�߰��Ǿ��ų� �������� ������ ��쿡,
  *     - ����ȭ �÷��װ� ���ε� �����ϸ� CheckPermissionFileUpload(), �Ǵ� CheckPermissionFileInsert() �޼ҵ�� ���ε� �������� üũ�ϰ�,
  *     - ���ε� �����ϸ�
  *       + ������ �������� ������ ������ ������ Ŭ�����ϰ�(�� ���Ϸ� ��ϵǵ���)
  *       + uploadFileItem() �޼ҵ�� ���ε带 �����Ѵ�.
  *     - ���ε� �������� ������ ����� FILE_SYNC_NO_UPLOAD_PERMISSION�� �Ѵ�.
  *     - �α׸� �����ϰ�, d���� FILE_NEED_UPLOAD�� Ŭ�����Ѵ�.
  *     - ���ε� ������ �����ϸ� SetItemSynchronizing()�� ȣ���Ͽ� ����ȭ �������� �����ϰ�, FILE_SYNC_THREAD_WAIT���� �����Ѵ�.
  *     .
  * @n P. �ƴϸ� d���� �ٿ�ε尡 �ְ�, ���������� ����/�߰��Ǿ��ų� �������� ������ ��쿡,
  *     - ����ȭ �÷��װ� �ٿ�ε� �����ϸ� CheckPermissionFileDownload() �޼ҵ�� �ٿ�ε� �������� üũ�ϰ�,
  *     - �ٿ�ε� �����ϸ� downloadFileItem() �޼ҵ�� �ٿ�ε带 �����Ѵ�.
  *     - �ٿ�ε� ������ ������ ����� FILE_SYNC_NO_DOWNLOAD_PERMISSION�� �Ѵ�.
  *     - �α׸� �����ϰ�, d���� FILE_NEED_DOWNLOAD�� Ŭ�����Ѵ�.
  *     - �ٿ�ε� ������ �����ϸ� SetItemSynchronizing()�� ȣ���Ͽ� ����ȭ �������� �����ϰ�, FILE_SYNC_THREAD_WAIT���� �����Ѵ�.
  *     .
  * @n Q. d���� �ٿ�ε尡 �ְ�, ���� ���°��� 0�̰�, phase�� PHASE_CONFLICT_FILES�� ��쿡, ����� FILE_SYNC_DOWNLOAD_FAIL�� �Ѵ�.
  * @n R. d���� ���� ����(FILE_NEED_LOCAL_DELETE)�� ������,
  *     - ����ȭ �÷��װ� �ٿ�ε� �����ϸ�
  *       + ������ ��� ��θ��� �غ��ϰ�, KSyncCoreBase::SyncLocalDeleteFile() �޼ҵ带 ȣ���Ͽ� ������ ������ �����Ѵ�.
  *       + ������ �����ϸ� �α׸� �����ϰ�,
  *       + �����ϸ�, mpLocalRenamedAs ������ Ŭ�����ϰ�, ������ ������ Ŭ�����ϰ�, �����丮�� �۾����� �����Ѵ�.
  *     - ����ȭ �÷��װ� �ٿ�ε� ������ ������, ����� FILE_SYNC_NO_DELETE_LOCAL_FILE_PERMISSION�� �Ѵ�.
  *     .
  * @n S. d���� ���� ����(FILE_NEED_LOCAL_DELETE)�� ������,
  *     - ����ȭ �÷��װ� ���ε� �����ϸ�, CheckPermissionFileDelete() �޼ҵ� ȣ���Ͽ� ���� �������� üũ�Ѵ�.
  *       + ������ �����ϸ� 
  *         + ������ OID�� ������ ����� R_SUCCESS�� �����ϰ�,
  *         + �ƴϸ� KSyncCoreBase::SyncServerDeleteFile()�� ȣ���Ͽ� ������ ������ �����Ѵ�.
  *         + ������ �����ϸ� mpServerRenamedAs�� �����ϰ�, ���ο� ������ ����(mServerNew)�� ��� Ŭ�����Ѵ�.
  *         + ������ �����ϸ� ����� FILE_SYNC_SERVER_REMOVE_FAIL�� �Ѵ�.
  *       + �ƴϸ� ����� FILE_SYNC_NO_DELETE_SERVER_FILE_PERMISSION�� �Ѵ�.
  *     - �����Ͽ����� �����丮�� �۾� ������ �����Ѵ�.
  *     .
  * @n T. d���� ���� ���� �Ǵ� ���� ������ �ְ�, ����� �������� ���Ͽ����� d������ FILE_NEED_RETIRED�� ����.
  * @n U. ������� �����̸�(R_SUCCESS)
  *     - phase�� PHASE_COPY_FILES�̸� ���� ���¿� ���� ���¸� 0���� �Ѵ�.
  *     - phase�� PHASE_LOAD_FILES�̸�,
  *     - d�� ������� �޾ƿ���, r = d & ~(FILE_SAME_MOVED| FILE_NEED_LOCAL_MOVE| FILE_NEED_SERVER_MOVE)�� �Ѵ�.
  *     - ������ ���� �޽��� ���۸� Ŭ���� �Ѵ�.
  *     .
  * @n V. ������� ������ �ƴϸ�
  *     - d���� �浹�̳� �����̸�
  *       + ���ο� ���� ������ ������Ʈ�ϰ�, meta_mask ������ �÷��׸� ��Ʈ�Ѵ�.
  *     .
  * @n W. ����� r�� ��� �� �浹 ��������� �����Ѵ�.
  * @n X. ����� r�� ������ �ƴϸ� �����丮�� �����Ѵ�.
  * @n Y. ��Ÿ �����̸� �α׿� retire �޽����� �����ϰ�, 
  * @n Z. StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����ϰų� �����Ѵ�.
  * @n AA. ��Ÿ ������ �ƴϸ� KSyncCoreBase::ShellNotifyIconChangeOverIcon() �޼ҵ带 ȣ���Ͽ� Ž���� ������ ������Ʈ�� ��û�Ѵ�.
  */
  virtual int doSyncItem(KSyncPolicy *policy, LPCTSTR baseFolder, int phase);

  virtual BOOL OnDeleteServerItem(BOOL r, LPCTSTR pError);

  /**
  * @brief ������ ������ �޾Ƽ� mServer struct�� �����Ѵ�(size, time, permission, fullpath, modifiedUser)
  * @return int : �����ϸ� TRUE
  * @details A. KSyncCoreBase::GetServerDocumentSyncInfo() �޼ҵ�� ������ ���� ������ mServer�� ������Ʈ�Ѵ�.
  */
  virtual BOOL RefreshServerState();

  /**
  * @brief ����ȭ �����ϱ� �ٷ� ���� ������ ������ �ٽ� üũ�ؼ� �� ���Ŀ� �ٽ� ��ȭ�� ������� �˻���
  * @param serverInfo : ���ο� ���� ������ ������ ����
  * @return int : �ٽ� �����Ǿ����� ITEM_STATE_UPDATED�� �����Ѵ�.
  * @details A. �θ� ��ü�� fullPathIndex�� ����Ǿ�����, �α׸� �����ϰ�, ITEM_STATE_LOCATION_CHANGED�� �����Ѵ�.
  * @n B. mServerOID�� ��� ������(���� ������ �������� ����) 0�� �����Ѵ�.
  * @n C. �� ��ü�� �����̸�,
  *     - KSyncCoreBase::GetServerFolderSyncInfo() �޼ҵ�� ������ ������ �޾ƿ���, �����ϸ�
  *       + ������ �̸��� ���� �޾ƿ� �̸��� ���Ͽ� �ٸ���,
  *         + ������� ITEM_STATE_UPDATED�� �Ѵ�.
  *       + �ƴϸ�, ������ fullPathIndex�� ���� �޾ƿ� fullPathIndex�� ���Ͽ� �޶�������,
  *         + ������� ITEM_STATE_LOCATION_CHANGED�� �Ѵ�.
  *     - �����ϸ�(������ �����ǰų� �׼��� ���е�) �����Ǿ����� üũ�ϰ�
  *     .
  * @n D. ������ �ƴϸ�, 
  *     - KSyncCoreBase::GetServerDocumentSyncInfo() �޼ҵ�� ������ ������ �޾ƿ���, �����ϸ�
  *       + FileOID, StorageOID, Filename �� ������ ���� ���� �޾ƿ� ���� ���Ͽ� �ٸ���, 
  *         + ������� ITEM_STATE_UPDATED�� �Ѵ�.
  *     - �����ϸ�(������ �����ǰų� �׼��� ���е�) �����Ǿ����� üũ�Ѵ�.
  *     .
  * @n E. ����� ITEM_STATE_UPDATED�̸�, �α׿� ������ �����ϰ�,
  * @n F. ����� �����Ѵ� 
  */
  int CheckServerModifiedAgain(SERVER_INFO& serverInfo);

  /**
  * @brief ������ ������ �ٿ�޾Ƽ� mServerNew struct�� �����Ѵ�(size, time, permission, fullpath, modifiedUser)
  * @return int : ���� ������ ��ġ�� �ٲ������ ITEM_STATE_LOCATION_CHANGED��, 
  * ������ ������ �ٲ������ ITEM_STATE_UPDATED�� �����ϰ� ������� �ʾ����� 0�� �����Ѵ�.
  */
  virtual int RefreshServerNewState();

  /**
  * @brief �� ��ü�� Ÿ���� ������ Ÿ�԰� ��ġ�ϴ��� �˻���.
  * @return BOOL : Ÿ���� ������ TRUE
  * @param type : ����ȭ ��ü type(0=����, 1=����, 2=��Ʈ ����)
  */
  BOOL IsMatchType(int type);

  /**
  * @brief ��� item ��ü�� ������ �� ��ü�� merge�Ѵ�.(���翡�� ���ٸ� ���� ����)
  * @return void
  * @param item : ������ ��ü
  */
  virtual void MergeItem(KSyncItem *item);

  /**
  * @brief �� �׸��� ������ �����̸� TRUE ������.
  * @return BOOL
  * @param phase : ����ȭ �ܰ�(�˻�, ��, ���� �̵�/�̸�����, ���� ��/�ٿ�)
  * @details A. phase�� PHASE_COPY_FILES�̸� ������� ��������,
  * @n B. phase�� PHASE_CONFLICT_FILES�̸� �浹 ������� ��������,
  * @n B. ������ �Ǵ� ������ ���� �÷��װ� ������ TRUE�� �����Ѵ�.
  */
  virtual BOOL willBeDeleted(int phase);

  /**
  * @brief �浹 ó�� ȭ�鿡�� ��� �׸��� �浹 ó�� ����� ���� ������ ���(���ε�/�ٿ�ε�/����/����)
  * ����� �����ϸ� TRUE�� ������. 2.0������ ��� �ȵ�.
  * @return BOOL
  */
  BOOL ToggleConflictResult();

  /**
  * @brief ����׸��� ���ÿ��� �Ǵ� �������� �̸����� �Ǵ� �̵��Ǿ������� �˻��ϰ� ����� ������.
  * @return int(ITEM_STATE_MOVED, ITEM_STATE_RENAMED mask)
  * @param onLocal : ������ �Ǵ� ������
  * @details A. onLocal�� TRUE�̰� ���� ���°� �̵����̸� newpathname�� ������ ��� ��θ��� �غ��ϰ�,
  * @n B. onLocal�� FALSE�̰� ���� ���°� �̵����̸� newpathname�� ������ ��� ��θ��� �غ��Ѵ�.
  * @n C. newpathname�� ��ȿ�ϸ�
  *       - ������ ��θ�(mpRelativePath)�� newpathname�� ��ΰ� �ٸ��� �˻��ϰ� �ٸ���,
  *         + ��ο� ���ϸ��� �и��Ͽ� ����ϰ�, ��ΰ� �޶������� ITEM_STATE_MOVED��, �̸��� �޶�������
  * ITEM_STATE_RENAMED�� ���°��� ���Ͽ� �����Ѵ�.
  */
  int GetItemStateMovedRenamed(BOOL onLocal);

  /**
  * @brief ����׸��� �浹 �������� ������.
  * @return int : �浹 �� ���� �ڵ�
  */
  inline int GetConflictResult() { return mConflictResult; }
  /**
  * @brief ����׸��� �浹 �������� ������.
  * @return void
  * @param r : �浹 �� ���� �ڵ�
  */
  inline void SetConflictResult(int r) { mConflictResult = r; }

  /**
  * @brief ����׸��� �浹 �������� ����� ���� action�� ������.
  * @return void
  * @param r : ������ action ��
  * @param flag : WITH_RECURSE=���� �׸񿡵� ������
  */
  virtual void SetConflictResultAction(int r, int flag);

  /**
  * @brief ����׸��� �浹 ���������� ����� ���� action�� index�� ��ȯ�Ͽ� ������.
  * @return void
  */
  int GetConflictResultMethod();

  /**
  * @brief ����ȭ �۾��� ���������� �߻��� ���� �޽����� ������
  * @return LPCTSTR
  */
  void SetServerConflictMessage(LPCTSTR msg);

  /**
  * @brief ������ ������ ��� ������.
  * @return void
  */
  void clearServerSideInfo();
  /**
  * @brief ������ ������ ��� ������.
  * @return void
  */
  void clearLocalSideInfo();
  /**
  * @brief �̸� �ٲٱ� �Ǵ� ��ġ �̵� ������ ������.
  * @return void
  */
  void clearRenameInfo();

  /**
  * @brief ����ȭ �浹 ���� �ذ��� ���� ������ ������ �̸��� �ٲٸ� ���� ��ü�� ���� �̸��� �ٲ�
  * ��ü �ΰ��� ����� �Ǹ�, �� ��Ȳ�� ���� �� ��ü�� �̸� �ٲ� ��ü�� ���� ��ϵǵ��� �ϰ�(������ ������
  * �����ϰ� �������� �ٲ� �̸����� ���� ��ϵ� �׸����� ó��), ��� ��ü(pair)�� 
  * �������� ���� ������ ��ü�� ó��(������ ������ �����ϰ� �������� ���� ��ϵ� �׸����� ó��)
  * @param root : ����ȭ ��Ʈ ���� �׸�
  * @param pair : ���� ������ ��ü
  * @return void
  * @details A. ���� ���°� �̵����̸�,
  *     - ChangeRelativePathName() �޼ҵ带 ȣ���Ͽ� ��θ��� mpLocalRenamedAs�� �ٲ۴�.
  *     .
  * @n B. �ƴϸ� UpdateRelativePath() �޼ҵ带 ȣ���Ͽ� ��θ��� ������Ʈ�Ѵ�.
  * @n C. ������ ������ �����.
  * @n D. �̸� ���� �� �̵� ������ �����.
  * @n E. ���� ���¸� (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)�� �Ѵ�.
  * @n F. ���� ���¸� 0���� �Ѵ�.
  * @n G. �浹 ������� FILE_NEED_UPLOAD|FILE_SYNC_RENAMED�� �Ѵ�.
  * @n H. ������� FILE_NEED_UPLOAD�� �Ѵ�.
  * @n I. �浹 ���� �޽����� Ŭ�����Ѵ�
  * @n J. ����ȭ Enable�� TRUE�� �Ѵ�.
  * @n K. StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����Ѵ�.
  * @n L. pair �׸��� NULL�� �ƴϸ� pair �׸񿡼�,
  *     - ������ ��θ� lastPath�� ������ �д�.
  *     - ������ ���°� �̵����̸�, mpServerRenamedAs�� ��θ��� �ٲٰ�,
  *     - �ƴϸ� UpdateRelativePath() �޼ҵ带 ȣ���Ͽ� ��θ��� ������Ʈ�Ѵ�.
  *     - �� ���� ������ ���� ������ ����(mServer = mServerNew)�Ѵ�.
  *     - ���� ���¸� 0���� �Ѵ�.
  *     - ���� ���¸� (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)�� �Ѵ�.
  *     - �浹 ������� FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED�� �Ѵ�.
  *     - ������� FILE_NEED_DOWNLOAD�� �Ѵ�.
  *     - �浹 ���� �޽����� Ŭ�����Ѵ�
  *     - ������ ������ Ŭ�����Ѵ�.
  *     - �̸� ���� �� �̵� ������ Ŭ�����Ѵ�.
  *     - ����ȭ Enable�� TRUE�� �Ѵ�.
  *     - StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����Ѵ�.
  */
  virtual void CloneLocalRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief ����ȭ �浹 ���� �ذ��� ���� ������ ������ �̸��� �ٲٸ� ���� ��ü�� ���� �̸��� �ٲ�
  * ��ü �ΰ��� ����� �Ǹ�, �� ��Ȳ�� ���� �� ��ü�� �̸� �ٲ� ��ü�� ���� ��ϵǵ��� �ϰ�(������ ������
  * �����ϰ� �������� �ٲ� �̸����� ���� ��ϵ� �׸����� ó��), ��� ��ü(pair)�� 
  * �������� ���� ������ ��ü�� ó��(������ ������ �����ϰ� �������� ���� ��ϵ� �׸����� ó��)
  * @param root : ����ȭ ��Ʈ ���� �׸�
  * @param pair : ���� ������ ��ü
  * @details A. ���� ���°� �̵����̸�,
  *     - ChangeRelativePathName() �޼ҵ带 ȣ���Ͽ� ��θ��� mpServerRenamedAs�� �ٲ۴�.
  *     .
  * @n B. �ƴϸ� UpdateRelativePath() �޼ҵ带 ȣ���Ͽ� ��θ��� ������Ʈ�Ѵ�.
  * @n C. ������ ������ �����.
  * @n D. �̸� ���� �� �̵� ������ �����.
  * @n E. ���� ���¸� (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)�� �Ѵ�.
  * @n F. ���� ���¸� 0���� �Ѵ�.
  * @n G. �浹 ������� FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED�� �Ѵ�.
  * @n H. �浹 ������� FILE_NEED_DOWNLOAD�� �Ѵ�.
  * @n I. �浹 ���� �޽����� Ŭ�����Ѵ�
  * @n J. ����ȭ Enable�� TRUE�� �Ѵ�.
  * @n K. StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����Ѵ�.
  * @n L. pair �׸��� NULL�� �ƴϸ� pair �׸񿡼�,
  *     - ������ ��θ� lastPath�� ������ �д�.
  *     - ������ ���°� �̵����̸�, mpLocalRenamedAs�� ��θ��� �ٲٰ�,
  *     - �ƴϸ� UpdateRelativePath() �޼ҵ带 ȣ���Ͽ� ��θ��� ������Ʈ�Ѵ�.
  *     - ������ ���°� �������̸�,
  *       + ���� ���¸� ITEM_STATE_DELETED���� �Ѵ�.
  *       + ���� ���¸� 0���� �Ѵ�.
  *       + �浹 ������� FILE_NEED_RETIRED|FILE_SYNC_RENAMED�� �Ѵ�.
  *       + ������� FILE_NEED_RETIRED�� �Ѵ�.
  *     - �ƴϸ�,
  *       + ���� ���¸� (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_WAS_EXIST|ITEM_STATE_CONFLICT_RENAMED)���� �Ѵ�.
  *       + ���� ���¸� 0���� �Ѵ�.
  *       + �浹 ������� FILE_NEED_UPLOAD|FILE_SYNC_RENAMED�� �Ѵ�.
  *       + ������� FILE_NEED_UPLOAD�� �Ѵ�.
  *     - ������ ������ Ŭ�����Ѵ�.
  *     - �̸� ���� �� �̵� ������ Ŭ�����Ѵ�.
  *     - �浹 ���� �޽����� Ŭ�����Ѵ�
  *     - ����ȭ Enable�� TRUE�� �Ѵ�.
  *     - StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����Ѵ�.
  *     .
  * @return void
  */
  virtual void CloneServerRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief ����ȭ �浹 ���� �ذ��� ���� �������� �������� ������ ��ü�� �и���.
  * �� ��ü�� ���ÿ� ���� �߰��� ���Ϸ� ó���ϵ��� ������ ������ �����ϰ�,
  * ��� ��ü(pair)�� ������ ���� �߰��� ���Ϸ� ó���ϵ��� ������ ������ �����Ѵ�.
  * @return void
  * @details A. StoreMetafile(root->GetStorage(), 0, METAFILE_DELETE)���� ��Ÿ �����͸� �����Ѵ�.
  * @n B. ������ ���°� �̵����̸�, mpLocalRenamedAs�� ��θ��� �ٲٰ�, �ƴϸ� UpdateRelativePath() �޼ҵ带 ȣ���Ͽ� ��θ��� ������Ʈ�Ѵ�.
  * @n C. ������ ������ �����.
  * @n D. ���� ���¸� (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)�� �Ѵ�.
  * @n E. ���� ���¸� 0���� �Ѵ�.
  * @n F. �浹 ������� FILE_NEED_UPLOAD|FILE_SYNC_RENAMED�� �Ѵ�.
  * @n G. ������� FILE_NEED_UPLOAD�� �Ѵ�.
  * @n H. �浹 ���� �޽����� Ŭ�����Ѵ�
  * @n I. �̸� ���� �� �̵� ������ �����.
  * @n J. ����ȭ Enable�� TRUE�� �Ѵ�.
  * @n K. StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����Ѵ�.
  * @n L. pair �׸��� NULL�� �ƴϸ� pair �׸񿡼�,
  *     - ������ ��θ� lastPath�� ������ �д�.
  *     - ������ ���°� �̵����̸�, mpServerRenamedAs�� ��θ��� �ٲٰ�, �� ���� ������ ���� ������ ����(mServer = mServerNew)�Ѵ�.
  *     - �ƴϸ� UpdateRelativePath() �޼ҵ带 ȣ���Ͽ� ��θ��� ������Ʈ�Ѵ�.
  *     - ���� ���¸� 0���� �Ѵ�.
  *     - ���� ���¸� (ITEM_STATE_NEWLY_ADDED|ITEM_STATE_NOW_EXIST|ITEM_STATE_CONFLICT_RENAMED)�� �Ѵ�.
  *     - �浹 ������� FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED�� �Ѵ�.
  *     - ������� FILE_NEED_DOWNLOAD�� �Ѵ�.
  *     - �浹 ���� �޽����� Ŭ�����Ѵ�
  *     - ������ ������ Ŭ�����Ѵ�.
  *     - �̸� ���� �� �̵� ������ Ŭ�����Ѵ�.
  *     - ����ȭ Enable�� TRUE�� �Ѵ�.
  *     - StoreMetafile() �޼ҵ�� ��Ÿ �����͸� �����Ѵ�.
  *     .
  */
  virtual void CloneSeparate(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief ����ȭ �� ��ü�̸� (������ OID�� ������ ������ �̹� ����ȭ �� ����) TRUE�� �����Ѵ�.
  * @return BOOL
  */
  BOOL IsAlreadySynced();

  /**
  * @brief ������ ��� ��� �� ���ϸ��� �� �޸� ���ۿ� �Ҵ��Ͽ� ������ �� ������.
  * @return LPTSTR : ������ ��� ��θ�
  */
  LPTSTR AllocServerSidePathName();

  /**
  * @brief �������� ���� ���ε� �۾��� �����Ѵ�.
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. �������� ��ü ��θ��� �غ��Ѵ�.
  * @n B. ���� ������ �������� ������ FILE_SYNC_UPLOAD_FAIL�� �����Ѵ�.
  * @n C. ������ FileOID, StorageOID�� ��ȿ�ϸ�(������ �̹� �����ϴ� ������)
  *     - KSyncCoreEngine::CheckOutDocument()�޼ҵ带 ȣ���Ͽ� �ش� ������ CheckOut�� ��û�Ѵ�.
  *     - CheckOut�� �����ϸ� FILE_SYNC_ERROR_CHECKOUT�� �����Ѵ�.
  *     .
  * @n D. ������ FileOID, StorageOID�� Ŭ�����ϰ�
  * @n E. KSyncCoreEngine::StartUploadFile() �޼ҵ带 ȣ���Ͽ� ���ε带 �����Ѵ�.
  */
  int uploadFileItem();

  /**
  * @brief ��� ��θ��� ���Ͽ� �޸𸮻󿡼� �� ��ü�� �̵� �� �̸������� ó���Ѵ�.
  * @return BOOL : ��θ��� �ٲ������ TRUE�� �����Ѵ�.
  * @details A. ������ ��� ��θ��� �����صΰ� ����ȭ ��� ��θ��� ���ۿ� �غ��Ѵ�.
  * @n B. isPathChanged()�޼ҵ�� ������ ������ ������ ������ ���Ͽ�, ��ΰ� �ٲ������
  *     - ���ο� ���(�� ��ü�� �θ� ����)�� �غ��ϰ� KSyncRootFolderItem::MoveSyncItem()�޼ҵ带
  * ȣ���Ͽ� �� ��ü�� ���ο� ����� KSyncFolderItem�� child�� �̵��Ѵ�.
  *     .
  * @n C. ������ ��θ�� ������ ��θ��� ���Ͽ� �ٸ���,
  *     - MoveMetafileAsRenamed()�޼ҵ带 ȣ���Ͽ� ���ο� ��θ����� ��Ÿ �����Ͱ� ����ǵ��� �Ѵ�.
  */
  BOOL checkRelativeNameChanged();

  /**
  * @brief �������� ���� �ٿ�ε� �۾��� �����Ѵ�.
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. �������� ��ü ��θ��� �غ��Ѵ�.
  * @n B. KSyncCoreEngine::StartDownloadItemFile() �޼ҵ带 ȣ���Ͽ� �ش� ������ �ٿ�ε带 �����Ѵ�.
  * @n C. ���ϰ��� ���� ����ȭ �������� �����Ѵ�.
  */
  int downloadFileItem();

  /**
  * @brief ���ε尡 �Ϸ�� �� ���ε忡�� ���Ϲ��� FileOID, StorageOID �� ��Ÿ ������ ���� ������ �ݿ��Ѵ�.
  * @param filename : ���ε�� ���ϸ�
  * @param serverStorageOID : ���ε�� storageOID
  * @return BOOL : �����Ͽ����� TRUE�� �����Ѵ�.
  * @details A. ���� storageOID�� �� ��ü�� mServerInfo�� �����Ѵ�.
  * @n B. mServerNew�� FileOID�� mServerInfo�� �����Ѵ�.
  * @n C. KSyncCoreEngine::UpdateUploadDocumentItem�޼ҵ带
  * ȣ���Ͽ� �� ��ü�� FileOID, StorageOID �� ��Ÿ ������ �������� �ݿ��Ѵ�.
  */
  BOOL UpdateDocumentItemOID(LPCTSTR filename, LPCTSTR serverStorageOID);

  /**
  * @brief ���ε� �Ǵ� �ٿ�ε尡 �Ϸ�� �� ȣ��Ǵ� �Լ��̴�.
  * @param isDown : TRUE�̸� �ٿ�ε� �۾��� ���
  * @param result : ���ε�/�ٿ�ε� �����
  * @param baseFolder : ����ȭ ��Ʈ ����.
  * @param conflictMessage : ������ ���� �޽���
  * @return int : �����ϸ� R_SUCCESS��, �����ϸ� �浹 �������� �����Ѵ�.
  * @details A. result ������� ���� �浹 �������� �����Ѵ�.
  * @n B. checkRelativeNameChanged() �޼ҵ带 ȣ���Ͽ� �̸� ����� ��쿡 ��� ��θ��� ������Ʈ �ǵ��� �Ѵ�.
  * @n C. ������� �����̸�
  *     - �ٿ�ε��̸�
  *       + RefreshLocalNewFileInfo()�Լ��� ȣ���Ͽ� ������ ������ mLocalNew�� �����Ѵ�.
  *     - UpdateLocalNewInfo() �Լ��� ȣ���Ͽ� mLocalNew ������ mLocal�� �ű��.
  *     - UpdateServerNewInfo() �Լ��� ȣ���Ͽ� mServerew ������ mServer�� �ű��.
  *     - ���°��� �������� �ٲٰ� ������� �ʱ�ȭ�ϰ� �̸����� ������ Ŭ�����Ѵ�.
  *     .
  * @n C. ������� ������ �ƴϸ�, ���� �޽����� �غ��ϰ�
  * @n D. �޽��� ������ �α׿� �����ϰ�
  * @n E. StoreHistory()�޼ҵ带 ȣ���Ͽ� �����丮�� �۾� ������ �����Ѵ�.
  * @n F. StoreMetafile()�޼ҵ带 ȣ���Ͽ� ��Ÿ �����͸� �����Ѵ�.
  * @n G. SetItemSynchronizing()�޼ҵ带 ȣ���Ͽ� ����ȭ ������ ���¸� Ŭ�����Ѵ�.
  * @n H. ���ε�/�ٿ�ε� �۾��� ������ ��쿡,
  *     - ����ȭ ��� ��θ��� �غ��Ͽ� �ش� ������ ������ �θ� ������ KSyncFolderItem::CheckLocalFolderExistOnSyncing() �޼ҵ带
  * ȣ���Ͽ� ���ÿ� �θ� ������ �������� �ʴ� ��� �ٸ� ���ϵ��� ����ȭ �۾� �õ��� ���ߵ��� �Ѵ�.
  *     - �θ� ������ KSyncFolderItem::CheckServerFolderExistOnSyncing() �޼ҵ带 
  * ȣ���Ͽ� ������ �θ� ������ �������� �ʴ� ��� �ٸ� ���ϵ��� ����ȭ �۾� �õ��� ���ߵ��� �Ѵ�.
  */
  int OnLoadDone(BOOL isDown, int result, LPCTSTR baseFolder, LPCTSTR conflictMessage);

  /**
  * @brief ���ε� �Ǵ� �ٿ�ε尡 ��ҵ� �� ȣ��Ǵ� �Լ��̴�.
  * @param baseFolder : ����ȭ ��Ʈ ����
  * @details A. SetItemSynchronizing()�޼ҵ带 ȣ���Ͽ� ����ȭ ������ ���¸� Ŭ�����Ѵ�.
  */
  void OnLoadCanceled(LPCTSTR baseFolder);

  /**
  * @brief ����ȭ �� �ܰ迡�� �񱳵� ���� ���� ����ȭ(���ε�/�ٿ�ε�/����)�� ó���Ǳ� ������ �����Ǿ����� Ȯ���ϴ� �Լ��̴�.
  * @param filename : ������ ��θ�
  * @return BOOL : �ٽ� �����Ǿ����� TRUE�� �����Ѵ�.
  * @details A. GetFileModifiedTime() �Լ��� ������ ���� ���� �ð��� �޾ƿ´�.
  * @n B. ���� ����(mLocalNew �Ǵ� mLocal)�� ���� �ð��� ���� �ð��� ���Ͽ� �޶�������, 
  * ��� �� �浹 ������ FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED�� �ϰ�, TRUE�� �����Ѵ�.
  */
  BOOL CheckModifiedAfterSyncCompare(LPCTSTR filename);

  /**
  * @brief ������ ������ �̵� �� �̸������� ó���Ѵ�.
  * @param renameObject : TRUE�̸� �ش� ������ ������ �̵� �� �̸� ������ �ϰ�, �ƴϸ� �̹� �̸� ����� ������ �����ϰ� ������ ������Ʈ�Ѵ�.
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. ������ ��� ��θ�� ������ ��� ��θ��� �غ��Ѵ�.
  * @n B. �� ��θ��� ��ο� �̸��� �и��Ͽ� ��ΰ� �ٲ������ Ȯ���Ѵ�.
  * @n C. renameObject�� TRUE�̸�,
  *     - ��ΰ� �ٲ������
  *       + ���ο� ��ο� �ش��ϴ� �θ� ���� KSyncFolderItem�� ���Ѵ�.
  *       + �� �θ� ������ ������ �̸��� ������,
  *         + ������ �̸��� ������ ������ �̸��̸� ����� FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS�� �ϰ� ����������.
  *         + �ӽ� �̸��� �غ��ϰ�, KSyncCoreEngine::ChangeDocumentName()���� �ӽ� �̸����� �ٲ۴�.
  *       + ���� �θ� ������ ���Ѱ� ���, �� �θ� ������ ���Ѱ� ��θ� KSyncCoreEngine::GetServerFolderSyncInfo()�� ���� �޾ƿ´�.
  *       + CheckPermissionFileMove()�� ���� ������ �ٸ� ������ �̵��� �� �ִ� ������ �ִ��� Ȯ���Ѵ�.
  *       + ������ ������
  *         + KSyncCoreEngine::MoveItemTo()�� ���� ��� ������ �̵��Ѵ�.
  *       + ������ ������ ����� FILE_SYNC_NO_MOVE_PERMISSION�� �ϰ� ����������.
  *     - �̸��� �ٲ������,
  *       + CheckPermissionFileRename() �޼ҵ�� ���� �̸��� �ٲ� ������ �ִ��� Ȯ���ϰ�,
  *         + ������ ������,
  *           + �θ� ������ �̹� ������ �̸��� ������ �����ϸ�,
  *             + �����ϴ� ������ �������� ���ϸ��̸�(�̸� ���� �Ǵ� �̵��� ���� �ʾ�����), 
  * ����� FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS�� �ϰ� ���� ������.
  *             + �ƴϸ� �ӽ� �̸��� �غ��Ѵ�.
  *           + KSyncCoreEngine::ChangeDocumentName()���� �̸��� �ٲ۴�.
  *         + ������ ������, ����� FILE_SYNC_NO_RENAME_PERMISSION�� �ϰ� ���� ������.
  *     .
  * @n D. renameObject�� FALSE�̸�, ������ ������ �����ϰ�,
  * @n E. ���� �۾��� �����Ͽ�����,
  *     - MoveMetafileAsRename()�޼ҵ带 ȣ���Ͽ� ��Ÿ �������� ��θ��� ������Ʈ�Ѵ�.
  *     - KSyncRootFolderItem::RemoveMovedRenamedOriginal() �޼ҵ带 ȣ���Ͽ� RenameDB���� �� �׸��� �̸����� ������ Ŭ�����Ѵ�.
  *     - clearRenameInfo()�޼ҵ�� �� ��ü�� �̸� ���� ������ Ŭ�����Ѵ�.
  *     .
  * @n F. renameObject�� TRUE�̸� StoreHistory() �޼ҵ�� �۾� ������ ����Ѵ�.
  */
  int renameServerObjectAs(BOOL renameObject);

  /**
  * @brief ������ ������ �̵� �� �̸������� ó���Ѵ�.
  * @param renameObject : TRUE�̸� �ش� ������ ������ �̵� �� �̸� ������ �ϰ�, �ƴϸ� �̹� �̸� ����� ������ �����ϰ� ������ ������Ʈ�Ѵ�.
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. ������ ��� ��θ�� ������ ��� ��θ��� �غ��Ѵ�.
  * @n B. �� ��θ��� ��ο� �̸��� �и��Ͽ� ��ΰ� �ٲ������ Ȯ���Ѵ�.
  * @n C. renameObject�� TRUE�̸�,
  *     - ���ο� ��ο� �ش��ϴ� �θ� ���� KSyncFolderItem�� ���Ѵ�.
  *       + �� �θ� ������ ������ �̸��� �ְ�,
  *         + ������ �̸��̸� ����� FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS�� �ϰ� ����������.
  *         + �ƴϸ� �ӽ� �̸��� �غ��Ѵ�.
  *     - KMoveFile()�� �̵� �� �̸� ������ �ϰ�, �ӽ� �̸��̸� SetLocalTemporaryRenamed()�� ���� �̸��� �����Ͽ� �д�.
  * �̴� ����ȭ ������ �ܰ迡�� �ٽ� ���� �̸����� �ٲٴ� �۾��� �Ѵ�.
  *     .
  * @n D. renameObject�� FALSE�̸� ������ ������ �Ѵ�.
  * @n E. ���� �۾��� �����Ͽ�����,
  *     - ��ΰ� �ٲ������ KSyncRootFolderItem::MoveSyncItem()�޼ҵ�� �� ��ü�� �ش� ���� �Ʒ��� �̵��Ѵ�.
  *     - MoveMetafileAsRename()�޼ҵ带 ȣ���Ͽ� ��Ÿ �������� ��θ��� ������Ʈ�Ѵ�.
  *     - KSyncRootFolderItem::RemoveMovedRenamedOriginal() �޼ҵ带 ȣ���Ͽ� RenameDB���� �� �׸��� �̸����� ������ Ŭ�����Ѵ�.
  *     - clearRenameInfo()�޼ҵ�� �� ��ü�� �̸� ���� ������ Ŭ�����Ѵ�.
  *     .
  * @n F. �����Ͽ�����, GetLastError()�� �ý��� ���� �ڵ带 �޾Ƽ� �̿� ���� ����ȭ ���� �ڵ带 ���ϰ����� �غ��Ѵ�.
  * @n G. renameObject�� TRUE�̸� StoreHistory() �޼ҵ�� �۾� ������ ����Ѵ�.
  */
  int renameLocalObjectAs(BOOL renameObject);

  /**
  * @brief �������� �̵� �� �̸� ����� ���뿡 ���� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  * @details A. ������ ��� ��θ�� ������ ������ ��� ��θ��� �غ��Ѵ�.
  * @n B. �� ��θ��� ��ο� �̸��� �и��Ͽ� ��ΰ� �ٲ������ Ȯ���Ѵ�.
  * @n C. ��ΰ� �ٲ������ KSyncRootFolderItem::MoveSyncItem()�޼ҵ�� �� ��ü�� �ش� ���� �Ʒ��� �̵��Ѵ�.
  * @n D. SetRelativePathName()�� ��θ��� �ٲ۴�.
  * @n E. MoveMetafileAsRename()�޼ҵ带 ȣ���Ͽ� ��Ÿ �������� ��θ��� ������Ʈ�Ѵ�.
  * @n F. clearRenameInfo()�޼ҵ�� �� ��ü�� �̸� ���� ������ Ŭ�����ϰ� ������ ������ ���°����� ITEM_STATE_MOVED�� �����Ѵ�.
  */
  void updateMetaAsRenamed();

  /**
  * @brief ������ �̸��� ������ �̸����� �ٲٰ� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ���
  * @param name : ���ο� ���ϸ�
  * @details A. ������ name���� ���ϸ��� �����Ѵ�.
  * @n B. KSyncRootStorage::MoveMetafileAsRename() �޼ҵ�� ����� �̸����� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  * @n C. StoreMetafile()�� ��Ÿ �����͸� �����Ѵ�.
  */
  BOOL UpdateRenameLocal(LPCTSTR baseFolder, LPCTSTR name);

  /**
  * @brief ������ ������ �̸��� ������ �̸����� �ٲٰ� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ���
  * @param name : ���ο� ���ϸ�
  * @param renameNow : TRUE�̸� ���� rename�� ó���Ѵ�.
  * @details A. ������ ����ȭ ��� ��θ��� �غ��Ѵ�.
  * @n B. �غ��� ��θ��� ���ϸ� �κ��� ������ name���� �ٲ۴�.
  * @n C. CFile::Rename() �޼ҵ�� ���� ������ �̸��� �ٲ۴�.
  * @n D. DirectoryMonitor�� ����� �� ������ �̸����� �̺�Ʈ�� �����Ѵ�.
  * @n E. ��� ��θ��� ������Ʈ�ϰ�, ���� ������ FileTime, AccessTime�� ������Ʈ�Ѵ�.
  * @n F. ������ �̸� ���� ������ Ŭ�����Ѵ�.
  * @n G. KSyncRootStorage::MoveMetafileAsRename() �޼ҵ�� ����� �̸����� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  */
  BOOL renameLocal(LPCTSTR baseFolder, LPCTSTR name, BOOL renameNow);

  /**
  * @brief ������ ������ �̸��� ������ �̸����� �ٲٰ� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ���
  * @param name : ���ο� ���ϸ�
  * @param renameNow : TRUE�̸� ���� rename�� ó���Ѵ�.
  * @details A. renameNow�� TRUE�̸�,
  *     - �����̸� KSyncCoreEngine::ChangeFolderName()��, �����̸� KSyncCoreEngine::ChangeDocumentName()��
  * ȣ���Ͽ� ������ �̸��� �����Ѵ�.
  *     - ������ �Ϸ�Ǿ�����,
  *       + ���� �̸��� ������ �̸����� �����ϰ�,
  *       + OnPathChanged()�� ȣ���Ͽ� ��θ� ������Ʈ�ϰ�,
  *       + RefreshServerState()�� ȣ���Ͽ� ������ ������ ������Ʈ�ϰ�
  *       + CopyServerInfo()�� mServer�� mServerNew�� �����Ѵ�.
  *       + KSyncRootStorage::MoveMetafileAsRename() �޼ҵ�� ����� �̸����� ��Ÿ �����͸� ������Ʈ�Ѵ�.
  *       + ������ �̸� ���� ������ Ŭ�����Ѵ�.
  *     .
  * @n B. renameNow�� FALSE�̸�,
  *     - RefreshServerState()�� ȣ���Ͽ� ������ ������ ������Ʈ�ϰ�
  *     - mServerNew.Filename�� �� ��ü�� �̸����� ������Ʈ�ϰ�
  *     - OnPathChanged()�� ȣ���Ͽ� ��θ� ������Ʈ�Ѵ�.
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
  * @brief �������� ���ϸ��� �����Ѵ�.
  */
  LPCTSTR GetLocalFileName();
  /**
  * @brief �������� ���ϸ��� �����Ѵ�.
  */
  LPCTSTR GetServerFileName();
#endif

  /**
  * @brief �������� ������ �ӽ� ���ϸ����� ����Ǿ� ������ �����Ѵ�.
  * @param realName : ���� �̸�
  * @details A. ������ ���� �̸��� mpServerTempRenamed ������ �����Ͽ��ٰ� ����ȭ ������ �ܰ迡�� �̸��� �ٽ� �ǵ��� ���� ���� ���ȴ�.
  */
  void SetServerTemporaryRenamed(LPCTSTR realName);
  /**
  * @brief �������� ������ �ӽ� ���ϸ����� ����Ǿ� ������ �����Ѵ�.
  * @param realName : ���� �̸�
  * @details A. ������ ���� �̸��� mpLocalTempRenamed ������ �����Ͽ��ٰ� ����ȭ ������ �ܰ迡�� �̸��� �ٽ� �ǵ��� ���� ���� ���ȴ�.
  * @n 'A/a.txt', 'B/a.txt' �� �����Ҷ�, �� ������ �������� �̵��� ��쿡, 'A/a.txt'�� 'B/a.txt'�� �ű�� �� ��, ������ �̸��� ���ϱ� ����,
  * 'A/a.txt'�� 'A/a-tmp001.txt'�� �̸��� �ٲ� ��, 'B/a-tmp001.txt'�� �̵��� �ϰ�, 'B/a.txt'�� 'A/a.txt'�� �̵��� ����,
  * �������� 'B/a-tmp001.txt'�� 'B/a.txt'�� �̸��� �ٲٴ� ������� ����ȴ�.
  */
  void SetLocalTemporaryRenamed(LPCTSTR realName);

  /**
  * @brief �������� ������ �ӽ� ���ϸ����� ����Ǿ� ������ TRUE�� �����Ѵ�.
  */
  inline BOOL IsServerTemporaryRenamed() { return ((mpServerTempRenamed != NULL) && (lstrlen(mpServerTempRenamed) > 0)); }

  /**
  * @brief �������� ������ �ӽ� ���ϸ����� ����Ǿ� ������ TRUE�� �����Ѵ�.
  */
  inline BOOL IsLocalTemporaryRenamed() { return ((mpLocalTempRenamed != NULL) && (lstrlen(mpLocalTempRenamed) > 0)); }

  /**
  * @brief ���������� ������ �̸��� ��ġ�� �ʴ� �ӽ� ���ϸ��� �غ��Ѵ�.
  * @param pathname : ������ ��θ�
  * @return LPTSTR : ���� ���� �ӽ� ���ϸ�.
  * @details A. ���۸� �غ��Ͽ� ������ ��θ��� �����Ѵ�.
  * @n B. ���� ��θ� '.AAA' ���ڿ��� ���δ�.
  * @n C. �̸��� �ش��ϴ� ������ �����ϴ��� üũ�Ͽ�, ������ �� �̸��� �����Ѵ�.
  * @n D. MakeNextSequenceName() �޼ҵ带 ȣ���Ͽ�, '.AAB', '.AAC' �� ������� �̸��� �ٲ۴�.
  * @n E. C�� �̵��Ѵ�.
  */
  virtual LPTSTR ReadyUniqueLocalName(LPCTSTR pathname);

  /**
  * @brief �ӽ� ���ϸ����� ����� �׸����� Ȯ���Ѵ�.
  * @return BOOL : �ӽ� ���ϸ����� ����Ǿ����� TRUE�� �����Ѵ�.
  */
  BOOL IsTemporaryRenamed();

  /**
  * @brief ���� ������ �̵� ó���� ������ Ȯ���Ѵ�.
  * @return BOOL : ���� ������ �̵��� ����Ǹ� TRUE�� �����Ѵ�.
  */
  BOOL ParentWillMove();

  /**
  * @brief �ӽ� ���ϸ����� ����� �׸��� �� �̸����� �ٲ۴�.
  * @return int : ���������� �� �̸����� �ٲ������ 1�� �����Ѵ�.
  * @details A. �������� �ӽ� ���ϸ����� �ٲ������,
  *     - KSyncCoreEngine::ChangeDocumentName()�޼ҵ�� ��� ������ �̸��� �� �̸����� �ٲ۴�.
  *     .
  * @n B. �������� �ӽ� ���ϸ����� �ٲ������,
  *     - ������ ��θ�� �� ��θ��� �غ��Ͽ�, KMoveFile() �޼ҵ�� ��� ������ �̸��� �� �̸����� �ٲٰ� �����ϸ�
  *       + KSyncRootFolderItem::OnStoreCancelRenamed()�޼ҵ�� �� ������ �̸� ���� ����� Ŭ�����Ѵ�.
  *     .
  * @n C. ���� �۾��� �����Ͽ����� MoveMetafileAsRename()�� ����� ��θ��� �ݿ��ϰ�,
  * @n D. StoreMetafile() �޼ҵ带 ȣ���Ͽ�, ��Ÿ �����͸� �����Ѵ�.
  */
  virtual int RestoreTemporaryRenamed();

  /**
  * @brief ������ ���� �Ǵ� ������ �����ϸ� TRUE�� �����Ѵ�.
  * @details A. ������ ��� ��θ��� �غ��ϰ�,
  *     - �����̸� IsDirectoryExist()��, �����̸� IsFileExist()�� ���� ���θ� üũ�Ͽ� �����Ѵ�.
  */
  BOOL IsLocalFileExist();

  /**
  * @brief ������ ���� �Ǵ� ������ �����ϸ� TRUE�� �����Ѵ�.
  * @details A. �������� OID�� �����ϸ�,
  *     - �����̸� TRUE�� �����ϰ�,
  *     - �����̸� ����OID, ���丮��OID�� ��ȿ�ϸ� TRUE�� �����Ѵ�.
  */
  BOOL IsServerFileExist();

  /**
  * @brief ����ȭ ����� ������� ��� ��� �� �̸��� �޸� �Ҵ��Ͽ� �����Ѵ�.
  * @return LPTSTR : ����ȭ ��� ��θ� ����
  * @param flag : option flag(AS_LOCAL : ������ �̸�, AS_NAME_ONLY : ��δ� �����ϰ� �̸���, AS_FULLPATH : ��ü ���)
  */
  virtual LPTSTR AllocSyncResultName(int flag);

  LPTSTR AllocRenamedPathName(int flag);

  /**
  * @brief �̸� ����� ��θ��� ������ ���ۿ� �����Ѵ�.
  * @param buff : ��θ��� ������ ����.
  * @param len : ��θ��� ������ ������ ũ��.
  * @param flag : ���� �Ǵ� ������.
  * @details A. �θ� ������ NULL�� �ƴϸ�
  *     - �θ� �������� KSyncFolderItem::GetRenamedPathName()���� ��θ��� ���Ѵ�
  *     .
  * @n B. flag�� AS_LOCAL���� �ְ�, ������ �̸� ���� ������ ��ȿ�ϸ�
  *     - ���ۿ� mpLocalRenamedAs�� �����Ѵ�.
  *     .
  * @n C. flag�� AS_LOCAL���� ����, ������ �̸� ���� ������ ��ȿ�ϸ�
  *     - ���ۿ� mpServerRenamedAs�� �����Ѵ�.
  *     .
  * @n D. �ƴϸ�,
  *     - ���ۿ� ���� ��ü�� �̸��� ���Ѵ�.
  */
  int GetRenamedPathName(LPTSTR buff, int len, int flag);

#if 0
  /**
  * @brief ����ȭ ����� ������� ���� ��� �� �̸��� �޸� �Ҵ��Ͽ� �����Ѵ�.
  * @return LPTSTR
  * @param flag : option flag(AS_LOCAL : ������ �̸�, AS_NAME_ONLY : ��δ� �����ϰ� �̸���)
  */
  LPTSTR AllocSyncResultFullName(int flag);

  /**
  * @brief ����ȭ ����� ������� ��� ��� �� �̸��� �޾ƿ´�.
  * @n deprecated : use AllocSyncResultName()
  * @return void
  * @param name : �̸��� �޾ƿ� ����
  * @param length : �̸��� �޾ƿ� ������ ũ��(TCHAR ����)
  * @param flag : option flag(AS_LOCAL : ������ �̸�, AS_NAME_ONLY : ��δ� �����ϰ� �̸���)
  */
  virtual void GetSyncResultRName(LPTSTR name, int length, int flag);
  /**
  * @brief ����ȭ ����� ������� ���� ��� �� �̸��� �޾ƿ´�.
  * @n deprecated : use AllocSyncResultName()
  * @return void
  * @param name : �̸��� �޾ƿ� ����
  * @param length : �̸��� �޾ƿ� ������ ũ��(TCHAR ����)
  * @param flag : option flag(AS_LOCAL : ������ �̸�, AS_NAME_ONLY : ��δ� �����ϰ� �̸���)
  */
  virtual void GetSyncResultName(LPTSTR name, int length, int flag);
#endif

  /**
  * @brief ��� ���ϸ��� ����ȭ �������� ���� ���Ͽ� �ش��ϴ� �̸����� Ȯ���Ѵ�.
  * @param filename : ��� ���ϸ�
  * @return BOOL : ���� ������ �ƴϸ� TRUE�� �����Ѵ�.
  */
  BOOL IsValidFilename(LPCTSTR filename);

  // return R_SUCCESS or failure-code
  /**
  * @brief ������ ������ �̸��� ������ �̸����� �ٲٰ� �������� �������� �и��Ͽ� �浹�� �ذ��Ѵ�.
  * ����ȭ �浹 ó��â���� ������ ���� �̸��� �ٲٸ� �� �Լ��� ó���ȴ�.
  * @param baseFolder : ����ȭ ��Ʈ ����
  * @param str : ���ο� �̸�
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ尪�� �����Ѵ�.
  * @details A. ���ο� ���� �̸��� ��ȿ�� ���ϸ��̸�,
  *     - ������ �̵� ������ ������ KSyncRootFolderItem::MoveSyncItem() �޼ҵ�� �ش� ���� ������ �̵��ϰ�,
  *     - ���ο� �̸��� ������ �̸��� �̹� �����ϴ��� IsDuplicateFilename()�� Ȯ���ϰ�, �̹� �����ϸ� ���� �ڵ带 �����Ѵ�.
  *     - DuplicateItem() �޼ҵ�� �� ��ü�� ������ ��ü�� �����ϰ�, �θ� ��ü�� �ٲ������ �θ� ��ü�� ������ ��ü�� �и��Ѵ�.
  *     - renameLocal() �޼ҵ�� ������ �̸��� �� �̸����� �ٲٰ� �����ϸ�, 
  *       + �浹 ������ FILE_SYNC_RENAMED�̰ų�, ���ü���-�������� �浹�̰ų�, ������ ������ �������� ������ ������ ��ü�� ������ �ʴ´�.
  *       + ������ ��ü�� �θ� ������ �����ϰ�,
  *       + CloneLocalRename() �޼ҵ�� ������ ��ü�� ���ÿ� ���� ��ϵ� ���Ϸ�, ������ ��ü�� �������� ���� ��ϵ� ���Ϸ� �����Ѵ�.
  *     - �����ϸ� ����� R_FAIL_RENAME_FAILURE�� �Ѵ�.
  */
  virtual int SetLocalRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief ������ ������ �̸��� ������ �̸����� �ٲٰ� �������� �������� �и��Ͽ� �浹�� �ذ��Ѵ�.
  * ����ȭ �浹 ó��â���� ������ ���� �̸��� �ٲٸ� �� �Լ��� ó���ȴ�.
  * @param baseFolder : ����ȭ ��Ʈ ����
  * @param str : ���ο� �̸�
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ尪�� �����Ѵ�.
  * @details A. ���ο� ���� �̸��� ��ȿ�� ���ϸ��̸�,
  *     - ������ �̵� ������ ������ KSyncRootFolderItem::MoveSyncItem() �޼ҵ�� �ش� ���� ������ �̵��ϰ�,
  *     - ���ο� �̸��� ������ �̸��� �̹� �����ϴ��� IsDuplicateFilename()�� Ȯ���ϰ�, �̹� �����ϸ� ���� �ڵ带 �����Ѵ�.
  *     - DuplicateItem() �޼ҵ�� �� ��ü�� ������ ��ü�� �����ϰ�, �θ� ��ü�� �ٲ������ �θ� ��ü�� ������ ��ü�� �и��Ѵ�.
  *     - renameServer() �޼ҵ�� ������ �̸��� �� �̸����� �ٲٰ� �����ϸ�, 
  *       + �浹 ������ FILE_SYNC_RENAMED�̰ų�, ���û���-�������� �浹�̰ų�, ������ ������ �������� ������ ������ ��ü�� ������ �ʴ´�.
  *       + ������ ��ü�� �θ� ������ �����ϰ�,
  *       + CloneServerRename() �޼ҵ�� ������ ��ü�� ������ ���� ��ϵ� ���Ϸ�, ������ ��ü�� �������� ���� ��ϵ� ���Ϸ� �����Ѵ�.
  *     - �����ϸ� ����� R_FAIL_RENAME_FAILURE�� �Ѵ�.
  */
  virtual int SetServerRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief �������� �������� ���� �ٸ� �̸����� �ٲ������, �� ��ü�� �и��Ͽ� �浹�� �ذ��Ѵ�.
  * ����ȭ �浹 ó��â���� �� �̸����� ����ϱ⸦ �ϸ� �� �Լ��� ó���ȴ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ尪�� �����Ѵ�.
  * @details A. �������� �������� ��� ��θ��� �غ��Ѵ�.
  * @n B. ���� ��θ��� �����ϸ� R_FAIL_SEPARATE_FAILURE�� �����Ѵ�.
  * @n C. ������ ��Ÿ �����͸� �����Ѵ�.
  * @n D. ���� ��θ��� ��� ��ȿ�ϸ�,
  *     - DuplicateItem()�� ȣ���Ͽ� ���� �׸��� �����ϰ�, 
  *     - ���� �׸��� �� ��ü�� �θ� �׸� Child�� ����Ѵ�.
  *     - CloneSeparate() �޼ҵ带 ȣ���Ͽ� �� �׸��� ���ÿ� ���ο� �׸�����, ���� �׸��� �������� ���ο� �׸����� �߰��� ������ ����Ѵ�.
  *     - �����丮�� �����Ѵ�.
  *     .
  * @n E. �ƴϸ�
  *     - ������ ��ΰ� ��ȿ�ϸ�,
  *       + ������ �̵� �� �̸� ���� ������ ������ �����Ѵ�.
  *       + �� ���� ����(mServerNew)�� ���� ����(mServer)�� �����ϰ�,
  *       + ���� ���¸� ���� �߰��� �׸�, �浹 ����� FILE_NEED_DOWNLOAD|FILE_SYNC_RENAMED, ����� FILE_NEED_DOWNLOAD�� �ϰ�,
  *       + ������ ������ Ŭ�����Ѵ�.
  *       + StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����͸� �����Ѵ�.
  *     - ������ ��ΰ� ��ȿ�ϸ�,
  *       + ������ �̵� �� �̸� ���� ������ ������ �����Ѵ�.
  *       + ���� ���¸� ���� �߰��� �׸�, �浹 ����� FILE_NEED_UPLOAD|FILE_SYNC_RENAMED, ����� FILE_NEED_UPLOAD�� �ϰ�,
  *       + ������ ������ Ŭ�����Ѵ�.
  *       + StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����͸� �����Ѵ�.
  *     .
  */
  virtual int RenameSeparate(KSyncRootFolderItem* root);

  /**
  * @brief ������ �̸����� �̸��� �ٲ� �� �ִ��� �˻��Ѵ�.
  * @param str : ���ο� �̸�
  * @param onLocal : �������̸� TRUE�̴�.
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ���� �ڵ尪�� �����Ѵ�.
  * @details A. ������ ��ο� ������ ���ο� �̸��� ���Ͽ� ��θ��� �����.
  * @n B. ���� ��θ� ���� ������ �����ϸ� R_FAIL_SAME_FILE_EXIST_LOCAL�� �����Ѵ�.
  * @n C. �θ� ��ü�� NULL�̸�(�� ��ü�� ����ȭ ��Ʈ ���� �׸�) R_FAIL_CANNOT_RENAME_ROOT�� �����Ѵ�.
  * @n D. onLocal�� TRUE�̸�
  *     - ���� ������� ������ ����̺� �۹̼��� ���� �������� Ȯ���ϰ� ������ ������ R_FAIL_NO_PERMISSION_TO_RENAME�� �����Ѵ�.
  *     - ������ ���ϸ��� �غ��ϰ�,
  *       + �ش� ������ �����ϰ�, IsFileWritable()�� ȣ���Ͽ� ���Ͽ� ��� ������ ������ R_FAIL_NO_PERMISSION_TO_RENAME�� �����Ѵ�.
  *     .
  * @n E. �ƴϸ�
  *     - ���� ������� ������ ����̺� �۹̼��� ���� �������� Ȯ���ϰ� ������ ������ R_FAIL_NO_PERMISSION_TO_RENAME�� �����Ѵ�.
  *     - ���� OID�� ��ȿ���� ������
  *       + �� ��ü�� �����̸� R_FAIL_CANNOT_RENAME_FOLDER��, �����̸� R_FAIL_CANNOT_RENAME_FILE�� �����Ѵ�.
  *       + CheckPermissionFolderRename()�� ȣ���Ͽ� �θ� ������ ���� ���Ѱ� �� ��ü�� ���� ���ѿ� ���� �̸� ������ �� �� �ִ��� üũ�ϰ�,
  * �� �� ������ R_FAIL_NO_PERMISSION_TO_RENAME�� �����Ѵ�.
  *     .
  * @n F. KSyncCoreEngine::IsFileExistServer()�� ȣ���Ͽ� ���� �Ʒ��� ������ �̸��� ������ �ִ��� üũ�Ͽ�,
  * ������ R_FAIL_SAME_FILE_EXIST_SERVER�� �����Ѵ�.
  */
  int IsDuplicateFilename(LPCTSTR str, BOOL onLocal);

  /**
  * @brief ����ȭ �׸��� ���Ͽ� �� ��ü�� ũ�� 1��, ������ -1�� ������ 0�� �����Ѵ�. ����Ʈ���� ��Ʈ�� ���� ����Ѵ�.
  * @param a : ���� ù° �׸�
  * @param b : ���� ��� �׸�
  * @param sortFlag : ���� �÷� �� �÷���
  * @details A. ������ �÷��� �ռ� ������ ������ ���ϸ� ������ ũ��.
  * @n B. �÷��� 0�̸� ���� ���ϸ�, 1�̸� ���� ���� �ð�, 2�̸� ���� ���� ũ��, 3�̸� ������ �̸�, 4�̸� ������ ���� ����ڸ�,
  * 5�̸� ���� ���� �ð�, 6�̸� ���� ����ũ�� ������ ���Ͽ� ����� �����Ѵ�.
  */
  int CompareSyncItem(KSyncItem* a, KSyncItem* b, int sortFlag);

#ifdef _DEBUG_RESOLVE
  virtual inline BOOL IsResolveVisible() { return TRUE; }
#else
  /**
  * @brief ����ȭ �浹 ó��â�� ǥ�õ� �׸��̸� TRUE�� �����Ѵ�.
  * @details A. �浹 ������� FILE_CONFLICT(����ȭ �浹), FILE_SYNC_ERR(����ȭ ����), 
  * FILE_SYNC_RENAMED(�浹�� �̸������), FILE_SYNC_BELOW_CONFLICT(���� �׸��� �浹) �� �÷��װ� ������ TRUE�� �����Ѵ�.
  */
  virtual inline BOOL IsResolveVisible() { return (mConflictResult & FILE_SYNC_RESOLVE_VISIBLE_FLAG); }
#endif

  /**
  * @brief ����ȭ �浹 ó��â���� ����ϴ� �� �׸���� �÷��� ���� �����Ѵ�.
  */
  inline int GetResolveFlag() { return mResolveFlag; }
  /**
  * @brief ����ȭ �浹 ó��â���� �� �׸��� ���õǾ����� TRUE�� �����Ѵ�.
  */
  inline BOOL IsSelected() { return !!(mResolveFlag & ITEM_SELECTED); }
  /**
  * @brief ����ȭ �浹 ó��â���� �� �׸��� Visible�̸� TRUE�� �����Ѵ�.
  */
  inline BOOL IsVisible() { return !(mResolveFlag & HIDDEN_ROOT_FOLDER); }

  /**
  * @brief ����ȭ �浹 �Ǵ� ������ �߻��� �׸��̸� TRUE�� �����Ѵ�.
  */
  inline BOOL IsConflicted() { return !!(mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR)); }
  /**
  * @brief ����ȭ �浹, ����ȭ ���� �Ǵ� �浹 ó������ �̸������ �׸��̸� TRUE�� �����Ѵ�.
  */
  inline BOOL IsConflictResolved() { return !!(mConflictResult & (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)); }

  /**
  * @brief �浹 ������� ������ flag�� ���Ѵ�
  */
  BOOL AddConflictResult(int flag);

  /**
  * @brief ����ȭ �浹 ó��â���� �� �׸� �Ǵ� ���� �׸��� �ϳ��� ���õǾ� ������ TRUE�� �����Ѵ�.
  * �浹 ��� �߿��� ���õ� �׸��� �ִ��� üũ�� �� ���ȴ�.
  */
  virtual BOOL HasSelected() { return !!(mResolveFlag & ITEM_SELECTED); }

  /**
  * @brief ����ȭ �浹 ó��â���� ���õ� �׸��� '����ȭ Enable' ��Ų��.
  * '���õ� �׸� ����ȭ�ϱ�' ����� ���� ������ ���ȴ�.
  */
  virtual void EnableSelectedItem();

  /**
  * @brief ����ȭ �浹 ó��â���� �� �׸��� Enable ���¸� �ʱ�ȭ ��Ų��.
  * @details A. ������� FILE_NOT_CHANGED(0)�� �ƴ� �׸��� Enable�� �����Ѵ�.
  */
  void InitResolveState(int flag);

  /**
  * @brief ����ȭ �浹 ó�� �÷��װ�(mResolveFlag)�� ������ ���� ����ũ�ϰ� ���Ѵ�.
  * @param a : AND�� ����ũ�� ��.
  * @param o : OR�� ���� ��.
  * @param withChild : TRUE�̸� child �׸񿡵� recursive�ϰ� �����Ѵ�.
  */
  virtual void SetResolveFlagMask(int a, int o, BOOL withChild);

  /**
  * @brief ����ȭ �浹 ó�� �÷��װ�(mResolveFlag)�� ������ ���ǿ� �ش�Ǵ� ��ü���� �ε��� ������ ���Ѵ�.
  * @param a : AND�� ����ũ�� ��.
  * @param o : �񱳰�.
  * @param index : ��ü�� index��.
  * @param area : ��ü�� �ε��� ������ ������ ����.
  * @param flag : �÷��� ��.
  * @details A. �� �׸��� ����ȭ �浹 ��Ͽ� ��Ÿ���� �׸��̸�,
  *       - ((mResolveFlag & a) == o) ������ �����ϸ�
  *         + area[0]���� 0���� ������ ������ index ���� area[0]�� �ִ´�.
  *         + area[1]���� index���� ������ ������ index ���� area[1]�� �ִ´�.
  *       - (index + 1) ���� �����Ѵ�.
  * @n B. �ƴϸ� index ���� �����Ѵ�.
  */
  virtual int GetResolveFlagArea(int a, int o, int index, int* area, int flag);

  /**
  * @brief ����ȭ �浹 ó�� Ʈ������ �� �׸��� ���� Child �׸��� ������ �����ϴ� �����Լ��̴�.
  */
  virtual int GetChildRowCount() { return 1; }


  /**
  * @brief ������ ��ü�� ������ ���� ��¥�� ������.
  * @return SYSTEMTIME : ������ ������ ���� ��¥
  */
  inline SYSTEMTIME GetLocalTime() { return ((CompareSystemTime(mLocal.FileTime, mLocalNew.FileTime) > 0) ? mLocal.FileTime : mLocalNew.FileTime); }

  /**
  * @brief ������ ��ü�� ������ ���� ��¥�� ������.
  * @return SYSTEMTIME : ������ ������ ���� ��¥
  */
  inline SYSTEMTIME GetServerTime() { return ((CompareSystemTime(mServer.FileTime, mServerNew.FileTime) > 0) ? mServer.FileTime : mServerNew.FileTime); }

  /**
  * @brief ������ ��ü�� ������ ������ OID�� ������.
  * @return LPTSTR : ������ ������ OID
  */
  inline LPTSTR GetServerUser() { return ((mServerNew.UserOID[0] != '\0') ? mServerNew.UserOID : mServer.UserOID); }

  /**
  * @brief ������ ��ü�� ���Ѱ��� ������.
  * @return int
  */
  inline int GetServerPermission() { return ((mServerNew.Permissions != -1) ? mServerNew.Permissions : mServer.Permissions); }

  /**
  * @brief ������ ��ü�� ���Ѱ��� ������.
  * @return void
  * @param p : ���� �� ������ ���Ѱ�
  */
  inline void SetServerPermission(int p) { mServer.Permissions = p; }

  /**
  * @brief ��� �׸��� �θ� �׸��� �����͸� ������.
  * @return void
  * @param p : �θ� �׸��� ������
  */
  inline void SetParent(KSyncItem *p) { mParent = p; }

  /**
  * @brief ��� �׸��� �� �θ� �׸��� �����͸� ������.
  * @return void
  * @param p : �� �θ� �׸��� ������
  */
  inline void SetNewParent(KSyncItem *p) { mNewParent = p; }

  /**
  * @brief �� ��ü�� ���� ��ü�� ��θ� �����.
  */

  virtual BOOL IsOverridePath();

#ifdef USE_META_STORAGE
  virtual void SetOverridePath(int callingDepth, KSyncRootStorage* s, LPCTSTR path);
#endif

#ifdef _DEBUG
  virtual void DumpSyncItems(int depth);
#endif

  /** �θ� �׸��� ������ */
  KSyncItem *mParent; 
  /** �� �θ� �׸��� ������ */
  KSyncItem *mNewParent; // parent item which moved by server-side.

  /** ��Ÿ ���ϸ�, KSyncRootStorageFile ����� ������ ���� */
  LPTSTR mpMetafilename;
  // TCHAR mMetafilename[KMAX_LONGPATH]; old metafile name

  /** ������ ���� */
  LOCAL_INFO mLocal;
  /** �� ������ ���� */
  LOCAL_INFO mLocalNew;
  /** �÷��� */
  int mFlag;

  /** ������ OID */
  TCHAR mServerOID[MAX_OID];

  /** ������ ���� */
  SERVER_INFO mServer;
  /** �� ������ ���� */
  SERVER_INFO mServerNew;

  /** ������ ���� */
  int mLocalState;
  /** ������ ���� */
  int mServerState;

  /** ���� ���� ��ĵ ����(��������, ������, ������) */
  int mServerScanState;

  /** ����ȭ Enable ���� */
  BOOL mEnable;
  /** ����ȭ �浹 ��� */
  int mConflictResult;
  /** ���� �׸� ��� */
  int mChildResult;
  /** ����ȭ ��� */
  int mResult;

  /** ������ �̵� �� �̸� ���� ���� */
  LPTSTR mpLocalRenamedAs;
  /** ������ �̵� �� �̸� ���� ���� */
  LPTSTR mpServerRenamedAs;
  /*
  TCHAR mLocalRenamedAs[KMAX_PATH]; // local file was renamed by user
  TCHAR mServerRenamedAs[KMAX_PATH]; // server file was renamed by user
  */

  /** ��� ��θ�, DB Storage���� key�� ���ȴ� */
  LPTSTR mpRelativePath;
  //TCHAR mRelativeName[KMAX_PATH]; // local relative pathname on last sync-time

  /** ������ ����ȭ ���� �޽����� ����Ǵ� ���� */
  LPTSTR mpServerConflictMessage;

  /** ������ �ӽ� �̸� ���� ����*/
  LPTSTR mpServerTempRenamed;
  /** ������ �ӽ� �̸� ���� ����
  * @n �׸��� �̵� ó���Ҷ�, ������ �̸��� �׸��� �ִ� ��쿡 �ӽ� �̸����� �ٲپ� �̵��� ó���ϰ� ���߿� �ٽ� ���� �̸�����
  * �ٲپ� ���� ���� ���ȴ�.
  */
  LPTSTR mpLocalTempRenamed;

  //TCHAR mServerTempRenamed[KMAX_PATH];
  //TCHAR mLocalTempRenamed[KMAX_PATH];

  /** ������ �̵� ��� ���� OID */
  TCHAR mServerMoveToOID[MAX_OID]; // new folder which item should move to

  /** ����ȭ �۾� �ܰ�. �˻� �ܰ�, �� �ܰ�, �̵� �ܰ�, ���� �ܰ� �� */
  int mPhase;

  /** ����ȭ �浹 ó�� �÷���, ������ Open/Close, ���� ����, ����� ���� ���� ���� ��Ÿ����.*/
  int mResolveFlag;

  /** ���� ����� ���� ��� */
  LPTSTR mpOverridePath;
};

#include <vector>

typedef std::vector<KSyncItem*> KSyncItemArray;
typedef std::vector<KSyncItem*>::iterator KSyncItemIterator;
typedef std::vector<KSyncItem*>::reverse_iterator KSyncItemRIterator;

/**
* @brief KSyncItemArray�� ���� �׸��� ��� �����Ѵ�.
*/
extern void FreeSyncItemList(KSyncItemArray& array);
/**
* @brief KSyncItemArray�� ���� �׸��� ������ ������ŭ �����Ѵ�.
*/
extern void FreeSyncItemListTop(KSyncItemArray &array, int maxCount, BOOL top);

/**
* @brief LOCAL_INFO ��Ʈ���ĸ� �ʱ�ȭ �Ѵ�.
*/
extern void ClearLocalInfo(LOCAL_INFO &local);
/**
* @brief SERVER_INFO ��Ʈ���ĸ� �ʱ�ȭ �Ѵ�.
*/
extern void ClearServerInfo(SERVER_INFO &server);

extern void FreeLocalInfo(LOCAL_INFO &local);
/**
* @brief SERVER_INFO ��Ʈ���ĸ� �����Ѵ�.
*/
extern void FreeServerInfo(SERVER_INFO &dst);
/**
* @brief SERVER_INFO ��Ʈ���� dst�� src�� �����Ѵ�.
*/
extern void CopyServerInfo(SERVER_INFO &dst, SERVER_INFO &src);
/**
* @brief SERVER_INFO ��Ʈ���� dst�� src�� �����Ѵ�.
*/
extern void MergeServerInfo(SERVER_INFO &dst, SERVER_INFO &src);
/**
* @brief LOCAL_INFO ��Ʈ���� dst�� src�� �����Ѵ�.
*/
extern void MergeLocalInfo(LOCAL_INFO &dst, LOCAL_INFO &src);

extern LPCTSTR GetDisplayPathName(LPCTSTR folder_path);


#endif