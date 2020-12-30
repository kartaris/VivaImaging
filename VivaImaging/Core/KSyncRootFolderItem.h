/**
* @file KSyncRootFolderItem.h
* @date 2015.02
* @brief KSyncRootFolderItem class header file
*/
#pragma once
#ifndef _DESTINY_FILE_SYNC_SYNC_ROOTFOLDER_ITEM_CLASS_H_
#define _DESTINY_FILE_SYNC_SYNC_ROOTFOLDER_ITEM_CLASS_H_

#include "KSyncFolderItem.h"
#include "KSyncPolicy.h"

#ifdef USE_META_STORAGE
#include "KSyncRootStorage.h"
#endif

#ifdef USE_SYNC_GUI
#include "../HttpDownloader/KHttpHeader.h"
#endif

#include <strsafe.h>

#define NOTHING_TODO 0
#define NEED_CONTINUE 1
#define NEED_UI_FOR_CONFLICT 2
#define NEED_UI_FOR_FULLSYNC 3
#define NEED_UI_FOR_SYNC_ERROR 4
#define NEED_YIELD 5

#define CURRENT_DB_VERSION 2
#define DB_VERSION_2 2

class KSyncCoreEngine;

typedef struct tagSYNC_PROGRESS_STRUCT
{
  TCHAR mFilename[KMAX_PATH];
  int method;
  int phase;
  int total;
  int file_done;
  int folder_done;

  __int64 total_size;
  int  percent;
  SYSTEMTIME remained_time;
} SYNC_PROGRESS_STRUCT;

typedef struct tagSYNCING_FILE_ITEM
{
  TCHAR relativeName[MAX_PATH];
  DWORD registedTick;
} SYNCING_FILE_ITEM, *LP_SYNCING_FILE_ITEM;

/**
* @class KSyncRootFolderItem
* @brief ����ȭ ��Ʈ ���� �׸� Ŭ����
*/
class KSyncRootFolderItem : public KSyncFolderItem
{
public:
  KSyncRootFolderItem(LPCTSTR pathname);
  KSyncRootFolderItem(LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid);
  KSyncRootFolderItem(KSyncRootFolderItem &item);

  virtual ~KSyncRootFolderItem(void);

  /**
  * @brief KSyncRootFolderItem object�� ������ �����ϰ� ��ü reference�� ������
  * @return KSyncRootFolderItem&
  * @param item : �ҽ� ��ü
  */
  KSyncRootFolderItem &operator=(KSyncRootFolderItem& item);

private:

  /**
  * @brief �� ��ü�� ���� ��� �������� �ʱ�ȭ�Ѵ�.
  */
  void InitFolderItem();

public:
  /**
  * @brief ����ȭ ��ü�� ������ �����ϴ� �����Լ�.
  * @return int : ����ȭ ��ü�� ����(0=file, 1=folder, 2=root folder)
  */
  virtual int IsFolder() { return 2; }

  /**
  * @brief �������� ��� ��θ��� �����ϴ� �����Լ�. ��Ʈ �����̴� NULL�� �����Ѵ�.
  * @return LPCTSTR : ��� ��θ�
  */
  virtual LPCTSTR GetServerRelativePath() { return NULL; }

#ifdef USE_SYNC_ENGINE
  /**
  * @brief ����ȭ ��Ÿ �����͸� �а� �����ϴ� ���丮�� Ŭ������ �����Ѵ�.
  * 1.x������ KSyncRootStorageFile Ŭ������ ���� ��Ÿ ���Ϸ� �����ϰ�,
  * 2.x������ KSyncRootStorageDB Ŭ������ ���� Sqlite3 DB�� �̿��� �����Ѵ�.
  * @param readonly : �б� �������� ����
  * @return KSyncRootStorage* : ���丮�� Ŭ����
  * @details A. mStorage�� NULL�� �ƴϰ� mStorage�� ReadOnly �Ӽ��� ������ readonly �Ӽ��� �ٸ��� ������ mStorage�� Ŭ�����Ѵ�.
  * @n B. mStorage�� NULL�̸�,
  *     - KSyncRootStorageFile::IsMatchRootFolder()���� �� ����ȭ ������ 1.x�� ���� ���丮���̸�,
  * KSyncRootStorageFile ��ü�� �����Ͽ� mStorage�� �Ѵ�.
  *     - �ƴϰ� KSyncRootStorageDB::IsMatchRootFolder()���� �� ����ȭ ������ 2.x�� DB ���丮���̸�,
  * KSyncRootStorageDB ��ü�� �����Ͽ� mStorage�� �Ѵ�.
  *     .
  * @n C. mStorage�� �����Ѵ�.
  */
  KSyncRootStorage* GetStorage(BOOL readonly = FALSE);
#else
  KSyncRootStorage* GetStorage(BOOL readonly = TRUE);
#endif

  /**
  * @brief ����ȭ ��Ÿ �����͸� �а� �����ϴ� ���丮���� �ݴ´�.
  * @details A. mStorage�� NULL�� �ƴϸ� �����ϰ� �����Ѵ�.
  */
  void CloseStorage();

  /**
  * @brief ���丮������ Policy ������ �ε��Ѵ�.
  */
  BOOL LoadSyncPolicy(KSyncRootStorage* s);

  /**
  * @brief ������ ��� ��θ� ������ ����ȭ ��Ÿ �����͸� �о� ����ȭ �׸���� �ε��Ѵ�.
  * @param relativePath : ��� ��θ�
  * @param child_flag : �÷���
  * @details A. ��� ��θ��� subPath�� �����Ѵ�.
  * @details B. subPath���� ���� ù��° ��θ� �ڸ���.(relativePath�� "a/b/c/" �̶�� ù������ "a", �ι�°���� "a/b", ����°���� "a/b/c")
  * @details C. KSyncRootStorage::LoadChildMetaData()�� ȣ���Ͽ� �ش� ��� �Ʒ��� �׸��� �ε��Ѵ�.
  * @details D. ������ �������� B�ܰ踦 �ݺ��Ѵ�.
  */
  int LoadChildItemCascade(LPCTSTR relativePath, int child_flag);

  /**
  * @brief ������ ��θ��� �������� �ε�Ǿ� �ִ��� Ȯ���ϰ� ������ storage���� �����ϰ� �ε��Ѵ�.
  * @param pathName : ��� �׸� ��θ�
  */
  KSyncItem* ReadyChildItemCascade(KSyncFolderItem* p, LPTSTR pathName);

  /**
  * @brief ������ OID�� �ش��ϴ� ����ȭ �׸��� �����ϰ� �ε��Ѵ�.
  * @param itemOID : ��� �׸� OID
  */
  KSyncItem* LoadItemFromStorage(LPCTSTR itemOID);

  /**
  * @brief ������ ��� ��θ� ������ ����ȭ ��Ÿ �����͸� �о� ����ȭ �׸���� �ε��Ѵ�.
  * @param relativePath : ��� ��θ�
  * @param child_flag : �÷���
  * @details A. KSyncRootStorage::LoadChildMetaData()�� ȣ���Ͽ� �ش� ��� �Ʒ��� �׸��� �ε��ϰ� �� ���ϰ��� �����Ѵ�.
  */
  int LoadChildItem(LPCTSTR relativePath, int child_flag);

  /**
  * @brief ����ȭ ���� ���� ������ �����Ѵ�.
  * @param progress : ���� ���� ������ ������ ����
  * @details A. ����ȭ ��ü ũ��, ��Ʈ ���� �̸�, ��ü ����, �Ϸ�� ���� ����, �Ϸ�� ���� ������ �����Ѵ�.
  * @n B. mPhase�� PHASE_COMPARE_FILES���� ������ ������� 0���� �Ѵ�.
  * @n C. �ƴϸ�, ������� (�Ϸ�� ���� ũ�� + �ٿ�ε� �������� ������ �Ϸ� ũ��) / ��ü ���� ũ��� ����ϰ�, 10000�� ���Ͽ� integer�� �����Ѵ�.
  * @n D. ������� 0���� ũ��,
  *     - ������ �ð��� �޾ƿ���
  *     - GetRemainedTime()���� ���� �����ð��� ����Ͽ� �´�.
  *     .
  */
  void GetSyncFileProgress(SYNC_PROGRESS_STRUCT &progress);

  /**
  * @brief ����ȭ ���� �ð�, ���� �ð�, �������κ��� ���� ���� �ð��� ����Ѵ�.
  * @param start : ���� �ð�
  * @param end : ���� �ð�
  * @param percent : ������(0���� 1 ������ ��)
  * @param remain : ���� ���� ���� �ð��� ������ ����
  * @return BOOL : 
  * @details A. TimeDifference()�� end - start�� ���Ѵ�.
  * @n B. ellapsed time�� �� ������ ����Ͽ� ellapsed_sec���� ����Ѵ�.
  * @n C. percent���� 0���� ũ��, ���� �ð�(�� ����)�� (ellapsed_sec / percent) - ellapsed_sec���� ����Ѵ�.
  * @n D. ���� �� ���� �ð����� 0���� ũ�� �� ���� SYSTEMTIME���� ��ȯ�Ͽ� remain�� �����ϰ� TRUE�� �����Ѵ�.
  * @n E. FALSE�� �����Ѵ�.
  */
  static BOOL GetRemainedTime(SYSTEMTIME &start, SYSTEMTIME &end, float percent, SYSTEMTIME& OUT remain);

#ifdef USE_SCAN_BY_HTTP_REQUEST
  /**
  * @brief ����ȭ ���� �۾��� ������ �׸� �˻��ϴ� �������� �˻� �Ϸ�� ������ ���� ������ ������Ʈ�Ѵ�.
  * @param folders : �˻��� ���� ����
  * @param files : �˻��� ���� ����
  * @details A. ���� ���� mSyncFolderCount, mSyncFileCount�� �����Ѵ�.
  */
  void OnScanFolderProgress(int folders, int files);
#endif

  /**
  * @brief ����ȭ ���� �۾��� ������ ���ε�/�ٿ�ε� ����� �Ϸ�� ũ�Ⱚ�� ������Ʈ�Ѵ�.
  * @param name : ���ε�/�ٿ�ε� �������� ���ϸ�
  * @param progress : �Ϸ���(0���� 10000������ ��)
  * @details A. ���� ���� mSyncFolderCount, mSyncFileCount�� �����Ѵ�.
  */
  void OnFileSyncProgress(LPCTSTR name, int progress);

  int MergeSameSyncJob(LPCTSTR pathname, int method, LPCTSTR syncOnlyPath);

  /**
  * @brief ������ ����ȭ ���� ��θ�� ����ȭ �۾� �÷��װ� �� ����ȭ ���� ��ü�� ��ġ�ϴ��� ���Ѵ�.
  * @param pathname : ����ȭ ���� ��θ�
  * @param method : ����ȭ �۾� Method.
  * @param syncOnlyPath : �κ� �۾� ���.
  * @details A. GetPath()�� �� ��ü�� ����ȭ ���� ��θ� �޾Ƽ� �̸� pathname�� ���ϰ�,
  * mSyncMethod�� flag�� ������ TRUE�� �����Ѵ�.
  */
  int IsSameSyncJob(LPCTSTR pathname, int method, LPCTSTR syncOnlyPath);

  /**
  * @brief ������ ����ȭ ���� ��θ�� ����ȭ �۾� �÷��װ� �� ����ȭ ���� ��ü�� ��ġ�ϴ��� ���Ѵ�.
  * @param folder : ����ȭ ���� ���� ��θ�
  * @param name : ����ȭ ���� �̸�
  * @param method : ����ȭ �۾� Method.
  * @details A. folder�� name�� ���ļ� pathname���� �����Ѵ�.
  * @n B. GetPath()�� �� ��ü�� ����ȭ ���� ��θ� �޾Ƽ� �̸� pathname�� ���ϰ�,
  * mSyncMethod�� flag�� ������ TRUE�� �����Ѵ�.
  */
  int IsSameSyncJob(LPCTSTR folder, LPCTSTR name, int method, LPCTSTR syncOnlyPath);

  BOOL MergeInstantBackupItem(LPCTSTR syncOnlyPath);

  /**
  * @brief ��Ÿ �����͸� ���Ϸ� �����ϱ� ���� ���ϸ��� �غ��Ѵ�.
  * @param s : ���丮�� ��ü
  * @param createIfNotExist : ������ ������ ���� �����.
  * @details A. ���丮�� Ŭ���� ������ ����(KSyncRootStorage::SRS_FILE)�̸�
  *     - mpMetafilename�� ��ȿ�ϸ� �����Ѵ�.
  *     - mpMetafilename�� ����ȭ ��Ʈ ���� ��ο� ".filesync/.objects/.root.ifo"�� �ٿ� �����Ѵ�.
  */
  virtual BOOL ReadyMetafilename(KSyncRootStorage *s, BOOL createIfNotExist);

  /**
  * @brief ������ ��θ��� ������ ���ۿ� �����Ѵ�.
  * @param fullname : ��θ��� ������ ����
  * @param size : ������ ũ��
  * @param abs : TRUE�̸� ��ü ��θ� �����Ѵ�.
  * @return int : ��θ� ���̸� �����Ѵ�.
  * @details A. abs�� TRUE�̸�,
  *     - fullname�� NULL�� �ƴϸ� mBaseFolder�� �����Ѵ�.
  *     - mBaseFolder ���ڿ� ���� + 1�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ�,
  *     - fullname�� NULL�� �ƴϸ� �� ��Ʈ������ �����,
  *     - 0�� �����Ѵ�.
  */
  virtual int GetActualPathName(LPTSTR fullname, int size, BOOL abs);

  /**
  * @brief ������ ��θ��� �����Ѵ�.
  */
  LPCTSTR GetServerFullPath();

#if 0
  /**
  * @brief ����ȭ ��� �̸��� ������ ���ۿ� �����Ѵ�.
  * @n deprecated
  * @param name : �̸��� ������ ����
  * @param length : ������ ũ��
  * @param flag : AS_LOCAL�̸� ������ �̸���, �ƴϸ� ������ �̸��� �����Ѵ�.
  * @details A. ������ �̸��̸�,
  *     - GetServerFullPath()�� ������ ��θ��� �޾ƿͼ�
  *     - ������ '<'�� ã�� �� ���� ���� �κк��� name�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ�, KSyncItem::GetSyncResultName()�� ȣ���Ѵ�.
  */
  virtual void GetSyncResultName(LPTSTR name, int length, int flag);
#endif

  /**
  * @brief ����ȭ ��å Ŭ������ �����Ѵ�.
  */ 
  inline KSyncPolicy &GetPolicy() { return mPolicy; }

  /**
  * @brief ����ȭ ��å Ŭ������ ������ �������� �����.
  */ 
  inline void SetPolicy(KSyncPolicy &policy) { mPolicy = policy; }

  /**
  * @brief ����ȭ ����� �����Ѵ�.
  */ 
  inline int GetSyncMethod() { return mSyncMethod; }

  /**
  * @brief ����ȭ ����� �����Ѵ�.
  */ 
  inline void SetSyncMethod(int m) { mSyncMethod = m; }

  /**
  * @brief ������ ��� �۾� ���� ������ �����Ѵ�
  */ 
  int GetUnfinishedBackupState(OUT LPTSTR* pLastBackupPath); //, OUT LPTSTR* pExtraInfo);

  /**
  * @brief ������ ��� �۾� ���� ������ �����Ѵ�
  */ 
  BOOL StoreUnfinishedBackupState(LPTSTR state);

  /**
  * @brief ������ ��� �۾� ���� ������ Ŭ�����Ѵ�
  */ 
  void ClearUnfinishedBackupState(int method);

#ifdef USE_BACKUP_JOB_LIST

  LPTSTR JobToString();

  static int ParseUnfinishedBackupState(LPTSTR state, LPTSTR *syncOnly, LPTSTR *key);

#endif


  /**
  * @brief ����ȭ �۾��� �����Ѵ�
  * @param sync_flag : ����ȭ �÷���
  * @return int : ��� �����Ϸ��� NEED_CONTINUE��, ������ �߻��Ǹ� NEED_UI_FOR_SYNC_ERROR ���� �����Ѵ�.
  * @details A. ����ȭ ���� �������� �ʱ�ȭ�Ѵ�.
  * @n B. (mSyncMethod & SYNC_METHOD_SYNCING)�̸� ReadyMetafilename()���� ��Ÿ ������ �غ��Ѵ�.
  * @n C. ������ ������ �����Ѵ�.
  * @n D. (mSyncMethod & SYNC_METHOD_SYNCING)�̸�,
  *     - ����ȭ ���������� ǥ���ϵ��� mResult�� FILE_NOW_SYNCHRONIZING�� ���Ѵ�.
  *     - ���丮�� Ŭ������ �غ��ϰ�, ���� ���丮�� ������ ũ�⿡ 16K�� ���� ũ�Ⱑ ��ũ�� ��� �������� Ȯ���ϰ�,
  * �����ϸ� ���� �ڵ� FILE_SYNC_METAFILE_DISK_FULL�� ����ȭ�� �ߴ��ϰ�, NEED_UI_FOR_SYNC_ERROR�� �����Ѵ�.
  *     - StoreMetafile()�� ����ȭ ������� ��Ÿ ���丮���� �����ϰ� �����ϸ�, �α׿� ������ �����ϰ�,
  * ���� �ڵ尡 ERR_DISK_FULL�̸� ����ȭ ������ FILE_SYNC_METAFILE_DISK_FULL�� �ϰ�, 
  * �ƴϸ�  ����ȭ ������ FILE_SYNC_CANNOT_MAKE_METAFILE�� �����ϰ�, NEED_UI_FOR_SYNC_ERROR�� �����Ѵ�.
  *     - ����ȭ ���۵��� �����丮�� ����Ѵ�.
  *     .
  * @n E. ������ ������ ���¸� (ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST)�� �ϰ�,
  * ����ȭ �۾��� ����ϵ��� NEED_CONTINUE�� �����Ѵ�.
  */ 
  int StartSync(int sync_flag);


  /**
  * @brief �ٽ� �õ��� �ʿ��ϸ� ����ȭ �۾��� �ٽ� �����Ѵ�.
  * �ٽý����ϸ� TRUE�� �����Ѵ�.
  */
  BOOL RetryInstantSync();

  /**
  * @brief ����ȭ �۾��� ����Ѵ�.
  * @return int : ��� �����Ϸ��� NEED_CONTINUE��, ������ �߻��Ǹ� NEED_UI_FOR_SYNC_ERROR ���� �����Ѵ�.
  * @details A. �浹 ������� ����ȭ ������ ������ NEED_UI_FOR_SYNC_ERROR�� �����Ѵ�.
  * @n B. mPhase�� PHASE_SCAN_OBJECTS�� ���ų� ������,
  *     - KSyncRootStorage::LoadChildMetaData()�� ȣ���Ͽ� ��Ÿ �����͸� �ε��Ѵ�.
  *     - �ε�� ���� ���� ���� ������ mSyncFolderCount, mSyncFileCount�� �޾ƿ´�.
  *     - ��Ÿ ������ �ε� ������� R_CONTINUE�̸� NEED_CONTINUE�� �����Ѵ�.
  *     - mSyncMethod�� ����ȭ �浹 ó��(SYNC_METHOD_RESOLVE_CONFLICT)�̸�,
  *       + RemoveUnconflictItem()�� ȣ���Ͽ� �浹�� �ƴ� �׸��� �����ϰ�,
  *       + RemoveExcludedItems()���� ���� ���ϵ��� �����ϰ�,
  *       + UpdateFolderSyncResult()���� ����ȭ ����� ������Ʈ�ϰ�,
  * ����ȭ �浹 ó��â�� ���� ���� NEED_UI_FOR_CONFLICT�� �����Ѵ�.
  *     - mPhase�� (PHASE_SCAN_FOLDERS_SERVER - 1)�� �ٲ۴�.
  *     .
  * @n C. mPhase�� PHASE_SCAN_FOLDERS_SERVER�� ���ų� ������(�������� ������ ����� �޾ƿ´�),
  *     - processScanFolderData()�� ȣ���Ͽ� �������� ������ ����� �޾ƿ��� �۾��� ó���ϰ� ���ϰ��� TRUE�̸� NEED_CONTINUE�� �����Ѵ�.
  *     - GetNextChild(PHASE_SCAN_FOLDERS_SERVER)�� ȣ���Ͽ� �������� ������ ����� �޾ƿ� �ʿ䰡 �ִ� ������ ã��, NULL�� �ƴϸ�
  *       + IsSyncNeedFolder()���� ó���� �������� Ȯ���ϰ�,
  *         + KSyncFolderItem::scanSubFoldersServer()�� ȣ���Ͽ� �������� ������ ����� ��û�ϴ� �۾��� �����Ѵ�.
  * �� ������� R_SUCCESS�� �ƴϸ� �� ���� �����Ѵ�.
  *         + KSyncFolderItem::RemoveExcludeNamedItems()���� ����ȭ ���� ���Ŀ� �ش��ϴ� �׸��� �����Ѵ�.
  *       + �� ������ phase�� PHASE_SCAN_FOLDERS_SERVER�� �ϰ� NEED_CONTINUE�� �����Ѵ�.
  *     - ó���� ������ ���̻� ������ SetPhaseServerNotExistFolders()�� ȣ���Ͽ�
  * �������� �������� �ʴ� �׸��� phase�� PHASE_SCAN_SERVER_FILES�� �����Ѵ�.
  *     - ó���� ���� �� ���� ������ Ŭ�����ϰ� mPhase�� (PHASE_SCAN_FOLDERS_SERVER + 1)�� �ٲ۴�.
  *     .
  * @n D. mPhase�� PHASE_SCAN_FOLDERS_LOCAL�� ���ų� ������(������ ������ �˻��Ѵ�),
  *     - GetNextChild(PHASE_SCAN_FOLDERS_LOCAL)�� �׸� �߿��� �� phase�� PHASE_SCAN_FOLDERS_LOCAL���� ���� ���� �׸��� ã�ƿ´�.
  *     - �ش��ϴ� ������ ������,
  *       + IsSyncNeedFolder()���� ����ȭ �۾��� �ʿ��� �����̸�,
  *         + KSyncFolderItem::scanSubFoldersLocal()�� ȣ���Ͽ� �� ������ ������ �ִ� �������� �˻��Ѵ�.
  *         + KSyncFolderItem::RemoveExcludeNamedItems()���� ����ȭ ���� ���Ŀ� �ش��ϴ� �׸��� �����Ѵ�.
  *       + �� ������ phase�� PHASE_SCAN_FOLDERS_LOCAL�� �����ϰ� NEED_CONTINUE�� �����Ѵ�.
  *     - ���̻� ������ ������
  *       + DeployFolderMovement()�� ȣ���Ͽ� �̸� ���� ������ �����ϰ�, ��ü �׸� ������ ������Ʈ�Ѵ�.
  *     - ����ȭ �Ϸ� ������ �ʱ�ȭ�ϰ�, mPhase�� (PHASE_SCAN_FOLDERS_LOCAL+1)�� �Ѵ�.
  *     .
  * @n E. mPhase�� PHASE_SCAN_LOCAL_FILES�� ���ų� ������(������ ���ϸ� �˻��Ѵ�),
  *     - GetNextChild(PHASE_SCAN_LOCAL_FILES)�� �׸� �߿��� �� phase�� PHASE_SCAN_LOCAL_FILES���� ���� ���� �׸��� ã�ƿ´�.
  *     - �ش��ϴ� ������ ������,
  *       + IsSyncNeedFolder()���� ����ȭ �۾��� �ʿ��� �����̸�,
  *         + KSyncFolderItem::scanSubFilesLocal()�� ȣ���Ͽ� �� ������ ������ �ִ� ���ϵ��� �˻��Ѵ�.
  *       + �ƴϸ�,
  *         + KSyncFolderItem::ClearSubFiles()�� ȣ���Ͽ� ������ ���ϵ��� �����Ѵ�.
  *       + �� ������ ���� ���� �˻��� �Ϸ�Ǿ�����,
  *         + KSyncFolderItem::RemoveExcludeNamedItems()���� ����ȭ ���� ���Ŀ� �ش��ϴ� �׸��� �����Ѵ�.
  *         + �� ������ phase�� PHASE_SCAN_LOCAL_FILES�� �����Ѵ�
  *       + NEED_CONTINUE�� �����Ѵ�.
  *     - ���̻� ������ ������
  *       + ����ȭ ��å�� ����ȭ �÷��׿� SYNC_SERVER_LOCATION_CHANGED�� ������
  * ClearServerSideInfoCascade()�� ȣ���Ͽ� ������ ������ ��� �����.
  *     - ����ȭ �Ϸ� ������ �ʱ�ȭ�ϰ�, mPhase�� (PHASE_SCAN_LOCAL_FILES+1)�� �Ѵ�.
  *     .
  * @n F. mPhase�� PHASE_COMPARE_FILES�� ���ų� ������(���� ���¸� ���Ѵ�),
  *     - GetNextChild(PHASE_COMPARE_FILES)�� �׸� �߿��� �� phase�� PHASE_COMPARE_FILES���� ���� ���� �׸��� ã�ƿ´�.
  *     - �ش��ϴ� ������ ������,
  *       + IsSyncNeedFolder()���� ����ȭ �۾��� �ʿ��� �����̸�,
  *         + KSyncFolderItem::compareFiles()�� ȣ���Ͽ� �� ������ ������ �ִ� ���ϵ��� ���Ѵ�.
  *       + �ƴϸ�,
  *         + ��ŵ�� ������ �α׿� ����Ѵ�.
  *       + �� ������ �� �۾��� �Ϸ�Ǿ����� �� ������ phase�� PHASE_COMPARE_FILES�� �����Ѵ�
  *       + �Ϸ�� �׸��� ������ ������Ʈ�ϰ�, NEED_CONTINUE�� �����Ѵ�.
  *     - ���̻� ������ ������
  *       + SetUnchangedItemPhase()�� ������ ���� �׸���� phase�� PHASE_ENDING���� �����Ѵ�.
  *       + ����ȭ �Ϸ� ������ �ʱ�ȭ�ϰ�, mPhase�� (PHASE_LOAD_FILES)�� �Ѵ�.(��Ʈ ������ ���� ����, ���� �̵� phase�� ó������ �ʵ��� ��)
  *     - ����ȭ ���ε� �ٿ�ε� �Ϸ� �����ð� ����� ����, ���� �ð��� mSyncStartTime�� �޾� ���´�.
  *     .
  * @n G. mPhase�� PHASE_CREATE_FOLDERS�� ���ų� ������(���� ������ �����Ѵ�),
  *     - GetNextChild(PHASE_CREATE_FOLDERS)�� �׸� �߿��� �� phase�� PHASE_CREATE_FOLDERS���� ���� ���� �׸��� ã�ƿ´�.
  *     - �ش��ϴ� ������ ������,
  *       + ReadyResolve()�� ȣ���Ͽ� ���¸� �ٽ��ѹ� ������Ʈ�ϰ� ���Ѵ�.
  *       + syncFileItem()�� ȣ���Ͽ� ���� ������ ó���Ѵ�.
  *     - ������,
  *       + mPhase�� (PHASE_MOVE_FOLDERS)�� �Ѵ�
  *     .
  * @n H. mPhase�� PHASE_MOVE_FOLDERS�� ���ų� ������(�׸��� �̵� �� �̸� ������ ó���Ѵ�),
  *     - GetNextChild(PHASE_MOVE_FOLDERS)�� �׸� �߿��� �� phase�� PHASE_MOVE_FOLDERS���� ���� ������ ������ �׸��� ã�ƿ´�.
  *     - �ش��ϴ� �׸��� ������,
  *       + ReadyResolve()�� ȣ���Ͽ� ���¸� �ٽ��ѹ� ������Ʈ�ϰ� ���Ѵ�.
  *       + syncFileItem()�� ȣ���Ͽ� �̵��� ó���Ѵ�.
  *     - ������,
  *       + RemoveExcludedItems()�� ȣ���Ͽ� ���� �������� �����Ѵ�.
  *       + RestoreTemporaryRenamed()�� ȣ���Ͽ� �ӽ� �̸� ����� �׸���� �� �̸����� �����Ѵ�.
  *       + mPhase�� (PHASE_COPY_FILES)�� �Ѵ�
  *     .
  * @n I. ����ȭ ������ ������ ��ġ�� ���� ����̺� ������ �ƴϸ�,
  *     - CheckDiskFreeSpace()���� ���� ������ 100K�̻����� Ȯ���Ѵ�. �����ϸ�,
  *       + ����ȭ ������ FILE_SYNC_NO_CDISK_SPACE�� �ϰ�, NEED_UI_FOR_SYNC_ERROR�� �����Ѵ�.
  *     .
  * @n J. mPhase�� PHASE_COPY_FILES�� ���ų� ������(���ε�/�ٿ�ε带 ó���Ѵ�),
  *     - GetNextChild(PHASE_COPY_FILES)�� �׸� �߿��� �� phase�� PHASE_MOVE_FOLDERS���� ���� ���� �׸��� ã�ƿ´�.
  *     - �׸��� ������ ���� �׸��� ã�ƿ´�.
  *     - �ش��ϴ� �׸��� ������,
  *       + ReadyResolve()�� ȣ���Ͽ� ���¸� �ٽ��ѹ� ������Ʈ�ϰ� ���Ѵ�.
  *       + syncFileItem()�� ȣ���Ͽ� ���ε� �Ǵ� �ٿ�ε带 ó���Ѵ�.
  *     - �ش��ϴ� �׸��� ������,
  *       + UpdateFolderSyncResult()�� ����� ������Ʈ�ϰ�,
  *       + mSyncMethod�� ����ȭ �浹 ó��(SYNC_METHOD_RESOLVE_CONFLICT)�̸�,
  * ����ȭ �浹 ó��â�� ���� ���� NEED_UI_FOR_CONFLICT�� �����Ѵ�.
  *       + GetServerFolderSyncInfo���� ��Ʈ ������ ������ �޾ƿͼ� StoreRootFolderServerPath()���� ������Ʈ�Ѵ�.
  *     - mSyncMethod�� ���� ����ȭ�� ��쿡
  *       + �浹 �� ���� �߻��� ������ 0���� ũ�� NEED_UI_FOR_CONFLICT�� �����Ѵ�.
  *     - mPhase�� (PHASE_ENDING)�� �ϰ�, NEED_CONTINUE�� �����Ѵ�.
  *     .
  * @n K. mPhase�� PHASE_CONFLICT_FILES�� ���ų� ������(����ȭ �浹�� ó���Ѵ�),
  *     - GetNextChild(PHASE_CONFLICT_FILES)�� �׸� �߿��� �� phase�� PHASE_CONFLICT_FILES���� ���� ������ ���� �׸��� ã�ƿ´�.
  *     - �ش��ϴ� �׸��� ������,
  *       + ReadyResolve()�� ȣ���Ͽ� ���¸� �ٽ��ѹ� ������Ʈ�ϰ� ���ϰ�, 
  * �� ���ϰ��� R_FAIL_NEED_FULL_SYNC_PARTIAL�̸�, NEED_UI_FOR_FULLSYNC�� �����Ѵ�.
  *       + syncFileItem()�� ȣ���ϰ� �� ���ϰ��� �����Ѵ�.
  *     - UpdateFolderSyncResult()�� ����� ������Ʈ�ϰ�,
  *     - �浹 ���(mConflictResult)�� ���� �׸� ���(mChildResult)�� �浹 �� ������ �ְų�, GetConflictCount()�� 0���� ũ��, 
  * NEED_UI_FOR_CONFLICT�� �����Ѵ�.
  *     - mPhase�� (PHASE_ENDING)�� �ϰ�, NEED_CONTINUE�� �����Ѵ�.
  *     .
  * @n L. mPhase�� PHASE_ENDING�� ���ų� ������,
  *     - SetUnchangedItemPhase()���� ������ ���� �׸���� PHASE_CONFLICT_FILES�� �Ѵ�.
  *     - UpdateFolderSyncResult()�� ����� ������Ʈ�ϰ�, NOTHING_TODO�� �����Ѵ�.
  */ 
  int ContinueSync();

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)

  /**
  * @brief �������� ������ ������ ����� �޾ƿ��� �۾��� �Ϸ�Ǹ� �� ����� �� �Լ��� ���޵ȴ�.
  * �� �Լ������� �����͸� ���� ���ۿ� �����Ͽ� �ΰ� �����Ѵ�.
  * @param p : �������� ������ ������ ����� ��û�� ����.
  * @param result : �����.
  * @param sfs_err : HTTP ������
  * @param data : ���������� ���� ������ ����
  * @param length : ���������� ���� ������ ����.
  * @param flag : �÷���
  * @param errorMessage : ���������� ���� ���� �޽���
  * @return int : �����ϸ� R_SUCCESS�� �����Ѵ�.
  * @details A. data�� NULL�̰ų� result�� R_SUCCESS�� �ƴϸ�,
  *     - OnCompleteServerItemScan(result)�� ȣ���ϰ�,
  *     - SetServerFolderScanResult()���� ����� �����ϰ�, ���� �޽����� SetServerConflictMessage()���� �����ϰ�, 0�� �����Ѵ�.
  *     .
  * @n B. m_ScanFolderData�� �ʿ��� �޸𸮸� �Ҵ��ϰ� data�� �����ϰ�, ó���� ���õ� �������� �ʱ�ȭ�Ѵ�.
  */
  int OnEndScanFolderItem(KSyncFolderItem *p, int result, KDownloaderError sfs_err, LPSTR data, long length, int flag, LPTSTR errorMessage);

  /**
  * @brief �������� ������ ������ ����� �޾ƿ��� �۾��� �Ϸ�� �� �� �����͸� ó���ϱ� ���� �� �Լ��� ȣ��ȴ�.
  * �ٿ�ε���� �����͸� �Ľ��Ͽ� ������ ������ �����ϴ� �۾��� �Ѵ�.
  * @return BOOL : �۾��� ������ �� ���� ������ TRUE�� �����Ѵ�.
  * @details A. m_ScanFolderData�� ��ȿ�ϸ�,
  *     - data�� ó���� ������ ����Ʈ�� ��ġ�ϵ��� �ϰ�,
  *     - m_ScanFolderIndex�� 0�̸�(�ٿ���� �������� ó�� �κ��� ó���ϴ� ���̸�)
  *       + mSyncFileCount, mSyncFolderCount�� 0���� �ʱ�ȭ�ϰ�,
  *       + data ���ۿ��� SCAN_ITEM_DELEMETER("\b")�� ã�Ƽ�, �̸� ��ĵ�� ���� �ð������� �о�ͼ� SetPreviousScanTimeStamp()���� ������ �д�.
  *       + �� ���̸�ŭ m_ScanFolderIndex�� �����Ѵ�.
  *       + (m_ScanFolderFlag & SCAN_ONLY_CHANGED)�̸�(��ȸ ��û�� �� ���� �ð� ���� ������ ��ü�� ��û������)
  *         + ��û�� ������ KSyncFolderItem::SetScanServerFolderState()�� �Ͽ� ��ü ���� �׸� ���� ��ĵ ���°��� ������������ �Ѵ�.
  *       + �ƴϸ�(���� ������ ��ü�� ��û������)
  *         + ��û�� ������ �����ϴ� ���·�, ������ �׸���� ������ ���·� �Ѵ�.(��ü ����� ��ȸ�� ��쿡, ��Ͽ� ���Ե��� ���� �׸��� ������ ���̹Ƿ�
  * ������ �׸��� ��� �������� �⺻ ���·� �Ѵ�)
  *     - (m_ScanFolderIndex < m_ScanFolderDataLength)�϶����� �ݺ��Ѵ�.
  *       + data���� SCAN_ITEM_DELEMETER("\b")�� ã�Ƽ� ��� ���̸� ���Ѵ�.
  *       + CreateFromStream()�� ȣ���Ͽ� ���� �� ���� ��ü�� ������ �о� ó���ϵ��� �Ѵ�.
  *       + ��ϸ�ŭ data �����͸� �����ϰ�, m_ScanFolderIndex�� �����Ѵ�.
  *       + ������ ���� �����Ͽ�����
  *         + KSyncFolderItem::SetServerStateByScanState()�� ȣ���Ͽ� ��ĵ ���°�(��������, ������, ������)�� ��ü�� ���� ����(mServerState)�� �ݿ��Ѵ�.
  *         + ����� ���븸 �޴� ����̰�, ���� �߰��� �׸��� ó���� �ʿ� �����̰�, ��Ʈ ���� ��ȸ�� ���,
  *           + SetPhaseForMovedFoldersInServer()���� ���������� �̵��� �������� phase�� (PHASE_SCAN_FOLDERS_SERVER-1)�� �����Ѵ�.
  * �̴� �ش� ������ ���� ���� ���� ��ü�� �ٽ� ��û�ؼ� �޾ƿ;� �ϱ� �����̴�.
  *         + �ƴϸ�, ���� �׸���� phase�� PHASE_SCAN_FOLDERS_SERVER�� �����Ѵ�.
  *         + ���� �޸𸮸� �����ϰ�, �����鵵 �ʱ�ȭ�Ѵ�.
  *       + ó���� ��ü�� ������ NUMBER_OF_ITEMS_SCAN_PARSE�� ���ų� ũ�� ������� �������´�.
  *     - ó���� ���۰� ���������� TRUE�� �����Ѵ�.
  *     .
  * @n B. FALSE�� �����Ѵ�.
  */
  BOOL processScanFolderData();

  /**
  * @brief ���������� ���� ��Ʈ���� �������� ����� �Ľ��Ͽ� �ش� �׸� �����Ѵ�. �׸��� ������ ���� �����Ѵ�.
  * @param pRoot : ����� ��û�� ����.
  * @param lastParent : ������ ó���� �׸��� �θ� ����(�ش� �׸��� ���� ã������ ĳ�õ� ��).
  * @param d : ���������� ���� ������ ����
  * @param data_len : ����� ������ ����.
  * @param rtn : ���ϰ�
  * @return KSyncItem* : ó���� �׸��� ������
  * @details A. SCAN_VALUE_DELEMETER('|')�� ã�� ��� ���̸� ���ϰ�,
  * @n B. ���̰� 11�̸�(������ ������ ���� OID ����)
  *     - WCHAR�� ��ȯ�� ���ڿ��� ����� oid�� �Ѵ�.
  *     - pRoot->findItemByOID()���� �׸��� ã�ƿ´�.
  *     - �׸��� ������
  *       + d���� ���� ��� �о int�� ��ȯ�Ͽ� childItemModified�� �����Ѵ�.
  *       + �׸��� SyncFlag�� SYNC_FLAG_IT_SCANNED�� ���Ѵ�.
  *       + childItemModified�� 0�� �ƴϸ�
  *         + SetScanServerFolderState()���� ���� �׸��� ��ĵ ���°��� ���������� �Ѵ�.
  *       + �� �׸��� ��ĵ ���°��� ���������� �Ѵ�.
  *     - �ƴϸ�, ���� �α׸� ����Ѵ�.
  *     - �׸��� �����Ѵ�.
  *     .
  * @n C. ���̰� 11�� �ƴϸ� object_type���� �д´�(D or F)
  * @n D. ���� ����� �о� oid�� �о�´�.
  * @n E. ���� ����� ������ NULL�� �����Ѵ�.
  * @n F. ���� ����� �о� name���� �о�´�.
  * @n G. ���� ����� �о� int�� �ٲپ� permission���� �����Ѵ�.
  * @n H. ���� ����� �о� modifiedAt �����ð����� �����Ѵ�.
  * @n I. ���� ����� �о� parent_oid�� �����Ѵ�.
  * @n J. parent_oid�� �ش��ϴ� �θ� ���� p�� ã�ƿ´�.
  * @n K. p�� NULL�̸�
  *     - oid�� pRoot�� ServerOID�� �����ϸ�
  *       + CheckPermissionRootFolder()���� permission�� ��ȿ���� Ȯ���ϰ� ������ �����ϸ�,
  *         + FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER ������ �����Ѵ�.
  *     - �ƴϸ� ���� �α׸� �����ϰ� NULL�� �����Ѵ�.
  *     .
  * @n L. object_type�� 'F'�̸�(�� �׸��� ������)
  *     - ���� ����� �о� creatorOID�� �����Ѵ�.
  *     - ���� ����� �о� fullPathIndex�� �����Ѵ�.
  *     - ���� ����� �о� childItemModified�� �����Ѵ�.
  *     - p�� NULL�� �ƴϸ�
  *       + p���� oid�� �ش��ϴ� ���� �׸��� ã�� item���� �����Ѵ�.
  *       + item�� NULL�̸�, p���� name���� ���� �׸��� ã�� item���� �����Ѵ�.
  *       + item�� NULL�� �ƴϸ�,
  *         + SyncFlag�� SYNC_FLAG_IT_SCANNED�� ���ϰ�,
  *         + ���� OID���� oid�� �����ϰ�,
  *         + CheckServerRenamedAs()���� �̸� ���� �� �̵� ������ �����ϰ�,
  *         + ������ �׸� ��û�� ���̰�, childItemModified�� 0�� �ƴϸ�
  *           + SetScanServerFolderState()���� ���� �׸�(1�ܰ踸)���� ���� ���·� �����Ѵ�.
  *       + item�� NULL�̸�,
  *         + ��θ� �����,
  *         + �� ��ΰ� ����ȭ ó�� �����̸�(�κ� ����ȭ�� ���ܵǴ� ������ �ƴ��� Ȯ���Ѵ�)
  *           + �� ������ ���� KSyncFolderItem ��ü�� ���� �����Ͽ� p�� ���� �׸����� ����ϰ� �̸� item���� �Ѵ�.
  *       + item�� NULL�� �ƴϸ�,
  *         + UpdateServerNewFileInfo()���� ������ ������ ������Ʈ�ϰ�, mServerNew.Filename�� name�� �����ϰ�,
  *         + ��ĵ ���°��� ���������� �Ѵ�.
  *     - ����� �޸𸮸� �����Ѵ�.
  *     .
  * @n M. object_type�� 'D'�̸�(�� �׸��� ������)
  *     - ���� ����� �о� lastModifier�� �����Ѵ�.
  *     - ���� ����� �о� file_oid�� �����Ѵ�.
  *     - ���� ����� �о� storage_oid�� �����Ѵ�.
  *     - ���� ����� �о� fileSize�� �����Ѵ�.
  *     - p�� NULL�� �ƴϸ�
  *       + p���� oid�� �ش��ϴ� ���� �׸��� ã�� item���� �����Ѵ�.
  *       + item�� NULL�̸� findChildByName���� name�� ��ġ�ϴ� �׸��� ã�ƿ´�.
  *       + item�� NULL�� �ƴϸ�,
  *         + ����OID ������ ������Ʈ�ϰ�,
  *         + item�� phase�� PHASE_END_OF_JOB���� ������ CheckServerRenamedAs()���� �̵� ������ �غ��Ѵ�.
  *       + �ƴϸ�,
  *         + ��θ��� �����,
  *         + �� ��ΰ� ����ȭ ó�� �����̸�(�κ� ����ȭ�� ���ܵǴ� ������ �ƴ��� Ȯ���Ѵ�)
  *           + KSyncItem�� �����ϰ� ������ �����ϰ� �̸� item���� �ϰ�, p�� child�� ����Ѵ�.
  *       + item�� NULL�� �ƴϸ�, UpdateServerNewFileInfo()���� ������ ������ ������Ʈ�ϰ�,
  *       + ��ĵ ���°��� ���������� �Ѵ�.
  *     - ����� �޸𸮸� �����Ѵ�.
  *     .
  * @n N. item�� �����Ѵ�.
  */
  KSyncItem* CreateFromStream(KSyncFolderItem *pRoot, KSyncFolderItem *lastParent, LPSTR d, long data_len, int& rtn);

  /**
  * @brief ���������� ���� ��Ʈ���� �������� �ٿ�ε尡 ����Ǿ����� ȣ��ȴ�.
  * @param result : ��û�� ���� �����.
  * @return int : NEED_CONTINUE�� �����Ѵ�.
  * @details A. ������� �����̸� phase�� ���� �ܰ��� (PHASE_SCAN_FOLDERS_LOCAL-1)�� �Ѵ�.
  * @n B. �ƴϸ� OnCancelSync()�� ȣ���Ѵ�.
  */
  int OnCompleteServerItemScan(int result);
#endif

#ifdef USE_BACKUP
  void OnRegistBackupEvent(int result);
#endif
  /**
  * @brief ��Ÿ �������� ���׷��̵� �۾��� �����Ѵ�. 1.x�� ��Ÿ���� ������ 2.x�� DB ������ �����Ѵ�.
  * ���ý�ũ�� ���۵ǰ� ����ȭ �����߿� 1.x������ �� ���� ������ �� �۾��� �ڵ����� ���۵ȴ�.
  * @details A. mPhase�� PHASE_SCAN_OBJECTS���� �۰ų� ������,
  *     - ���丮�� Ŭ������ �غ��Ͽ� KSyncRootStorage::LoadChildMetaData()�� ȣ���Ͽ� ��Ÿ �����͸� �ε��Ѵ�.
  *     .
  * @n B. �ƴϸ�
  *     - ���丮�� Ŭ�������� ����ȭ ��Ʈ ������ ������ �ε��ϰ�,
  *     - KSyncRootStorageDB::CreateMetaDataFolder()���� ��Ÿ ������ ������ �����ϰ� �����ϸ�,
  *       + KSyncRootStorageDB ��ü�� �����ϰ�, ���� ��ü�� ContinueStoreMetafile(), StoreMetafile()�� ȣ���Ͽ�
  * ��Ÿ �����͸� DB�� �����Ѵ�.
  *       + ������ ��Ÿ ����(".filesync")�� �����Ѵ�.
  */
  int ContinueImmigration();

  // 0 : out-of folder, 1 : need-folder or child, 2 : parent of need-folder

  /**
  * @brief ��� ����ȭ ������ ����ȭ ó���� �Ͽ��� �ϴ� �������� üũ�Ѵ�.
  * �κ� ����ȭ�� ������ ��, �ش� ���� �̿��� �������� ó���� ���� �ʵ��� �ϴµ� �̶� �� �Լ��� üũ�Ѵ�.
  * @param folderItem : ��� ����
  * @return int : ��� ������ ����ȭ ó���� �Ǿ�� �ϸ� 1�� �����Ѵ�.
  * @details A. �κ� ����ȭ ����(mSyncOnlyPath)�� ��ȿ�ϸ�,
  *     - mSyncOnlyPath�� ���뿡�� ';'�� ���е� ��θ��� �ϳ��� ���Ͽ�,
  *     - isSyncNeedFolderEx()�� ��� ������ �κ� ����ȭ�� ���� �������� Ȯ���Ͽ�, 1�̸� 1�� �����Ѵ�.
  *     - �ƴϸ� 0�� �����Ѵ�.
  *     .
  * @n B. �κ� ����ȭ ���� ������ ������ ��ü ������ �� ó���Ǵ� ���̴� 1�� �����Ѵ�.
  */
  int IsSyncNeedFolder(KSyncFolderItem *folderItem, int phase);


  BOOL UpdateParentOfSyncOnlyPath();

  /**
  * @brief ���� ����� �˻��ؼ� ������ ���� �׸��� ��ĵ�� �ʿ䰡 �ִ��� Ȯ���ϰ�, ������ ���� ������ �����Ѵ�.
  */
  BOOL CheckNeedScanLocalFolder(KSyncFolderItem* folderItem);

private:
  /**
  * @brief ��� ����ȭ ������ ����ȭ ó���� �Ͽ��� �ϴ� �������� üũ�Ѵ�.
  * @param syncOnlyPath : �κ� ����ȭ ����
  * @param folderItem : ��� ����
  * @details A. �κ� ����ȭ ����(syncOnlyPath)�� ��ȿ�ϸ�,
  *     - ��� ������ ��� ��θ��� �غ��Ͽ�,
  *       + NULL�̸� ��Ʈ �����̴� 2�� �����Ѵ�.
  *     - ��θ��� �κ� ����ȭ ������ ���� �����̸� 2�� �����Ѵ�.
  *     - ��θ��� �κ� ����ȭ ������ ���ų� ���� �����̸� 1�� �����Ѵ�.
  *     - 0�� �����Ѵ�.
  *     .
  * @n B. 1�� �����Ѵ�.
  */
  int isSyncNeedFolderEx(LPCTSTR syncOnlyPath, KSyncFolderItem *folderItem);

  /**
  * @brief ModifiedItemDat�� ���� �׸��� �غ��ϰ� ���� ���¸� ������Ʈ �Ѵ�.
  * @param pathName : ��θ�
  * @param modifyType : DIR_MODIFY_EVENT
  * @details A. modifyType�� DIR_DELETED�� ��쿡
  * @n - ��θ� �ش��ϴ� �׸��� DB�� �ִ��� Ȯ���ϰ�, ������ �̸� ������ Ȯ���Ͽ� ���� ��θ����� �ٽ� �ѹ� �׸��� DB�� �ִ��� Ȯ���Ѵ�.
  * @n - �׸��� �����ϸ� Ʈ���� �����ϰ�, ���ÿ� �������� �������� �����Ѵ�.
  * @n B. modifyType�� DIR_ADDED,DIR_MODIFIED,DIR_MOVED_NEW�̸�
  * @n - �̸� ������ �Ǿ� ������ ���� ��θ��� ���ϰ�,
  * @n - ��θ� �ش��ϴ� �׸��� DB�� �ִ��� Ȯ���ϰ�, ������ ���� �߰��� �׸����� �ν��Ͽ� Ʈ���� �����Ѵ�.
  * @n - ��θ� ���� Ʈ���� �����Ѵ�.
  * @n - ��� �׸��� ���ÿ� ���������� �����Ѵ�.
  */
  int readyModifiedItem(LPTSTR pathName, DIR_MODIFY_EVENT modifyType);

public:

  /**
  * @brief �ǽð� ������� ���� ������ ��ĵ���� �ʰ� ModifiedTable�� ��ϵ� �׸��� Ʈ���� �����Ѵ�.
  * @n ����� ��� ��ο� ���� �׸��� �������� ������ �����Ѵ�.
  * @details A. mModifiedItemList ��Ͽ��� DIR_MOVED�� �׸��� ��ŵ�ϰ�, readyModifiedItem()�� ȣ���Ѵ�.
  */
  int ReadyModifiedItemList();

  /**
  * @brief �κ� ����ȭ ������ �ϳ��̰� ��ȿ�ϸ� �װ͸� (phase-1)�� �ϰ� �ٸ� ������ phase�� �����Ѵ�.
  */
  BOOL CheckScanFolderServerPartialSync(int phase);

  /**
  * @brief ��� ������ ����ȭ ó���� �Ͽ��� �ϴ� �������� üũ�Ѵ�.
  * @param pathname : ��� ����
  * @param isFolder : ��� pathname�� ������.
  * @return BOOL : ��� ������ ����ȭ ó���� �Ǿ�� �ϸ� TRUE�� �����Ѵ�.
  * @details A. �κ� ����ȭ ����(mSyncOnlyPath)�� ��ȿ�ϸ�,
  *     - mSyncOnlyPath�� ���뿡�� ';'�� ���е� ��θ��� �ϳ��� ���Ͽ�,
  *     - IsPartialSyncOnEx()�� ��� ������ �κ� ����ȭ�� ���� �������� Ȯ���Ͽ�, 1�̸� TRUE�� �����Ѵ�.
  *     - �ƴϸ� FALSE�� �����Ѵ�.
  *     .
  * @n B. �κ� ����ȭ ���� ������ ������ ��ü ������ �� ó���Ǵ� ���̴� TRUE�� �����Ѵ�.
  */ 
  virtual BOOL IsPartialSyncOn(LPCTSTR pathname, BOOL isFolder);

  /**
  * @brief ��� ����ȭ ������ ����ȭ ó���� �Ͽ��� �ϴ� �������� üũ�Ѵ�.
  * @param syncOnlyPath : �κ� ����ȭ ����
  * @param pathname : ��� ����
  * @param isFolder : ��� pathname�� ������.
  * @return BOOL : ��� ������ ����ȭ ó���� �Ǿ�� �ϸ� TRUE�� �����Ѵ�.
  * @details A. �κ� ����ȭ ����(syncOnlyPath)�� ��ȿ�ϸ�,
  *     - ��� ������ ��� ��θ��� �غ��Ͽ�,
  *     - ��θ��� �κ� ����ȭ ������ ���� �����̸� isFolder�� �����Ѵ�.
  *     - ��θ��� �κ� ����ȭ ������ ���ų� ���� �����̸� TRUE�� �����Ѵ�.
  *     - FALSE�� �����Ѵ�.
  *     .
  * @n B. TRUE�� �����Ѵ�.
  */
  BOOL IsPartialSyncOnEx(LPCTSTR syncOnlyPath, LPCTSTR pathname, BOOL isFolder);

  /**
  * mSyncOnlyPath�� �߰��� �̸� ����Ǿ����� ������Ʈ�Ѵ�.
  */
  BOOL refreshPartialSyncRenamed(KSyncRootStorage* storage);

  /**
  * @brief �� ����ȭ ��Ʈ ������ ����ȭ ó���� �� �� �ִ��� üũ�Ѵ�.
  * @return int : �� �� ������ R_SUCCESS�� �ƴϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. ���� ������ ���������Ͽ� �����ϸ�,
  *     - ������ ���Ѱ��� �����ͼ�, CheckPermissionRootFolder()���� ������ �ִ��� Ȯ���ϰ�,
  *     - ������ ������ FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER�� �����Ѵ�.
  *     - R_SUCCESS�� �����Ѵ�.
  *     .
  * @n B. �����ϸ� FILE_SYNC_ROOTSERVER_FOLDER_NOT_EXIST�� �����Ѵ�.
  */
  int CheckRootSyncable();

  /**
  * @brief �浹 ó��â���� ����ȭ ��ư�� ���� �ٽ� ����ȭ�� ������ ��, �� �Լ��� ȣ��ȴ�.
  * ����ȭ �浹 ó�� �۾��� �غ��Ѵ�.
  * @param flag : �÷���
  * @details A. �÷��׿� DONE_AS_COMPLETE���� ������,
  *     - UpdateFolderSyncResult()�� ����ȭ ����� ������Ʈ�Ѵ�.
  *     - NOTHING_TODO�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ�,
  *     - ����ȭ ���� ���� �������� �ʱ�ȭ�ϰ�,
  *     - ���� �׸���� phase�� (PHASE_CONFLICT_FILES - 1)�� �����ϰ�,
  *     - ��Ʈ �׸��� phase�� PHASE_CONFLICT_FILES�� �ϰ�,
  *     - ������ �ð��� mSyncStartTime�� �޾Ƴ���,
  *     - ����ȭ ������� 0���� �ʱ�ȭ�ϰ�,
  *     - CheckRootSyncable()���� ����ȭ ��Ʈ ������ ������ üũ�ϰ� ������ ������ NEED_UI_FOR_SYNC_ERROR�� �����Ѵ�.
  *     - NEED_CONTINUE�� �����Ѵ�.
  */
  int ContinueConflict(int flag);

  /**
  * @brief ��Ÿ ���丮������ �� �׸��� ����ȭ �浹 ������� �ε��Ѵ�.
  * ����ȭ �浹 ó�� �۾��� �غ��Ѵ�.
  * @return int : ���� �ڵ� �Ǵ� NEED_CONTINUE�� �����Ѵ�.
  * @details A. SetItemSynchronizing()���� ��Ʈ ������ ����ȭ �������� ���·� �����Ѵ�.
  * @n B. LoadServerPrevious()���� ��Ÿ �����͸� �ε��Ѵ�.
  * @n C. GetRootLastSyncTime()���� ������ ����ȭ �ð��� mLastSyncCompleteTime�� �о�´�.
  * @n D. ����ȭ ���� �������� �ʱ�ȭ�Ѵ�.
  * @n E. CheckRootSyncable()�� ȣ���Ͽ� ������ ���Ѱ� ���¸� üũ�Ѵ�.
  */ 
  int LoadLastConflict();

  /**
  * @brief ����ȭ ����� �ҿ� �ð��� �α׿� ����Ѵ�.
  * @param str : �±� �޽���
  * @details A. ���� �ð��� mSyncBeginTime�� ���� ���ϰ�, �̸� GetEllapsedTimeString()���� ���ڿ��� �ٲ۴�.
  * @n B. �ڿ� ������ ��Ʈ���� " : "�� ���� �ٿ��� �α׿� �����Ѵ�.
  */ 
  void StoreEllapsedTimeLog(LPCTSTR str);

  /**
  * @brief ����ȭ �۾��� �����Ѵ�.
  * @param done : TRUE�̸� ����ȭ �Ϸ� ���·� �����Ѵ�.
  * @details A. (mSyncMethod & SYNC_METHOD_SETTING)�̸�,
  *     - StoreRootFolderServerPath()���� ������ ������ �����Ѵ�.
  *     - StoreMetafile()���� ��Ÿ �����͸� �����Ѵ�.
  *     - ShellNotifyIconChangeOverIcon()���� Ž���� �������� ������Ʈ�Ѵ�.
  *     .
  * @n B. �ƴϸ�,
  *     - SetItemSynchronizing()���� ����ȭ �������� ���¸� �����Ѵ�.
  *     - done�̸� mChildResult�� 0���� Ŭ�����Ѵ�.
  *     - StoreMetafile()���� ��Ÿ �����͸� �����Ѵ�.
  *     - done�̸� ClearFileRenamed()���� �̸� ���� ������ ��� Ŭ�����Ѵ�.
  *     - ������ �ð����� ������ ����ȭ �ð����� �����Ѵ�.
  *     - ����ȭ ���� �̺�Ʈ�� �����丮�� �����Ѵ�.
  */ 
  void CloseSync(int flag);

  /**
  * @brief ����ȭ �۾��� ����Ѵ�.
  * @param sync_error : ����ȭ ���� �ڵ尪.
  * @details A. �浹 ������� sync_error�� �Ѵ�.
  * @n B. StoreMetafile()���� ��Ÿ �����͸� �����Ѵ�.
  * @n C. ShellNotifyIconChangeOverIcon()���� Ž������ �������� ������Ʈ�Ѵ�.
  * @n D. UpdateRootLastSyncTime()���� ������ ����ȭ �ð��� ������Ʈ�Ѵ�.
  * @n E. ��Ʈ ������ ������ �ٽ� �ε��Ѵ�.
  */
  void OnCancelSync(int sync_error);

  /**
  * @brief ����ȭ �۾� ���� ������ ����Ѵ�.
  */
  int StoreBackupRestoreResult(int flag);

  /**
  * @brief ������ ó���� �ڵ� ����ȭ �ð��� ����Ͽ� ���۵� �ð��� �����Ѵ�.
  * @param time : ���� ��� �ð�.
  * @return BOOL : ������ ó���� �ڵ� ����ȭ �۾��� ��ȿ�ϸ� TRUE�� �����Ѵ�.
  * @details A. ����ȭ ��å�� ����ȭ�� �Ͻ� ������ �����̸� FALSE�� �����Ѵ�.
  * @n B. ������ ����ȭ �ð��� ��å ���� �ð��� last_sync_time, setting_time�� �о�´�.
  * @n C. last_sync_time�� ��ȿ�ϸ�, setting_time�� last_sync_time�� �����Ѵ�.
  * @n D. �ƴϰ�, setting_time�� last_sync_time���� ũ��, setting_time�� last_sync_time�� �����Ѵ�.
  * @n E. ����ȭ ��å�� CalcNextSyncTime()�� ȣ���Ͽ� ���� ����ȭ �ð��� �޾ƿ��� �����ϸ� TRUE�� �����Ѵ�.
  * @n F. FALSE�� �����Ѵ�.
  */
  BOOL CalcNextSyncTime(SYSTEMTIME &time);

  void UpdateLastSyncTime();

  // RenameManagement storage
  // void GetRenameManageFilename(LPTSTR manage_file);

  /**
  * @brief DirectoryMonitor�� �����Ͽ� ����� ������ ���� �� ������ �̵� ������ �� �׸� ��ü�� �����Ѵ�.
  * @details A. KSyncRootStorage::DeployFolderMove()�� ȣ���Ѵ�.
  */
  int DeployFolderMovement();

  /**
  * @brief DeployFolderMovement()���� ���丮���� ��ϵ� ���� �� ������ �̵� ������ �� �Լ��� ���� callback���� ���޵ȴ�.
  * �ش� ��ü�� ã�� �̵� ������ �����Ѵ�.
  * @param oldName : ������ ��θ�
  * @param newName : ����� ��θ�
  * @details A. ������ ��θ�� ����� ��θ� �ش��ϴ� ��ü�� FindChildItemCascade()���� ã�ƿ´�.
  * @n B. �� ��ü�� NULL�� �ƴϰ� �������� ������,
  * @n C. �� ��ü�� �����ϴ� �׸��̸� �������� �ʰ�, FALSE�� �����Ѵ�.
  * @n D. ���� ��ü�� �����ϰ�, �� ��ü�� ReadyMetafilename()���� ���� �����Ǹ�,
  *     - newItem�� oldItem�� �����Ѵ�.
  *     - newItem�� �����Ѵ�.
  *     - TRUE�� �����Ѵ�.
  *     .
  * @n E. FALSE�� �����Ѵ�.
  */
  BOOL OnFolderMovedInfo(LPTSTR oldName, LPTSTR newName);

  /**
  * @brief ������ �� ��θ� ���� Rename Storage���� ���� ��θ��� ã�ƿ´�.
  * @param newRelativeName : ���ο� ��θ�
  * @param originalRelativeName : ���� ��θ�
  * @return int : �̸����濡 �ش��ϸ� 1, ���� ������ �̸������̸� 2�� �����Ѵ�.
  * @details A. KSyncRootStorage::FindOriginalNameT()�� ȣ���Ͽ� ���� ��θ��� ã�ƿ´�.
  */
  int GetMovedRenamedOriginalName(LPCTSTR newRelativeName, LPTSTR originalRelativeName);


  int AllocMovedRenamedOriginalName(LPCTSTR newRelativeName, LPTSTR* originalRelativeName);

  /**
  * @brief ������ �� ��θ� ���� Rename Storage���� ���� ��θ��� ã��, �ش� �׸��� ã�� �����Ѵ�.
  * @param newRelativeName : ���ο� ��θ�
  * @return KSyncItem* : �̸����濡 �ش��ϸ� �׸��� ������.
  * @details A. GetMovedRenamedOriginalName()���� ���� ��θ��� ã�ƿ´�.
  * @n B. �����ϸ� ���� ��θ� �ش��ϴ� �׸��� FindChildItemCascade()���� ã�� �����Ѵ�.
  */
  KSyncItem *GetMovedRenamedOriginal(LPCTSTR newRelativeName);

  /**
  * @brief ������ �� ��θ� ���� Rename Storage���� �ش� �׸��� �����Ѵ�. ó���� �Ϸ�Ǿ����� ȣ��ȴ�.
  * @param newRelativeName : ���ο� ��θ�
  * @return BOOL : �����ϸ� TRUE�� �����Ѵ�.
  * @details A. KSyncRootStorage::RemoveRenameItem()�� ȣ���Ͽ� �̸����� �׸��� �����Ѵ�.
  */
  BOOL RemoveMovedRenamedOriginal(LPCTSTR newRelativeName);

  /**
  * @brief ������ ���� ��θ� ���� Rename Storage���� �ش� �׸��� ã�� ���ο� ��θ��� �����Ѵ�.
  * @param oldRelativeName : ���� ��θ�
  * @return LPTSTR : ���ο� ��θ�, �޾Ƽ� ����ϰ� �޸𸮸� �����Ͽ��� �Ѵ�.
  * @details A. KSyncRootStorage::AllocMovedRenamedAs()�� ȣ���Ͽ� ���ο� ��θ��� �����Ѵ�.
  */
  virtual LPTSTR AllocMovedRenamedAs(LPCTSTR oldRelativeName);

  //BOOL GetMovedRenamedAs(LPCTSTR oldRelativeName, LPTSTR newRelativeName);

  /**
  * @brief ������ ���� ��θ� ���� Rename Storage���� �ش� ��η� �̵��Ǿ� ��������, �ش� ��ο��� �̵��Ǿ� ���� �׸��� �ִ��� Ȯ���Ѵ�.
  * @param relativePath : ��� ��θ�
  * @return BOOL : �ش��ϴ� �׸��� ������ TRUE�� �����Ѵ�.
  * @details A. KSyncRootStorage::GetRenamedInOutCount()�� ȣ���Ͽ� ������ 0���� ũ�� TRUE�� �����Ѵ�.
  */
  BOOL CheckRenamedInOut(LPCTSTR relativePath);

  /**
  * @brief �κ� ����ȭ �Ϸ��� Rename Storage���� �ش� ������ �̵��� �׸��� ��� �����Ѵ�.
  * @param subFolderSync : �κ� ����ȭ ��θ�
  * @details A. KSyncRootStorage::ClearFileRenamed()�� ȣ���Ѵ�.
  */
  void ClearFileRenamed(LPCTSTR subFolderSync);

  BOOL ClearRenameFlag(LPCTSTR orgName, LPCTSTR newName);

  /**
  * @brief ������ �̸��� �ٲ������ �� ������ �׸���� �̸��� ������Ʈ �Ѵ�.
  * @param prevRelative : ���� ��θ�
  * @param changedRelative : �� ��θ�
  * @details A. KSyncRootStorage::UpdateRenameParentFolder()�� ȣ���Ѵ�.
  */
  void UpdateRenameParentFolder(LPCTSTR prevRelative, LPCTSTR changedRelative);

  /**
  * @brief ������ �̸��� �ٲ������ ��Ÿ ������ ��θ� Mapping�� ������Ʈ �Ѵ�. 1.x������ ���ȴ�.
  * @param lastRelativePath : ���� ��θ�
  * @param newRelativePath : �� ��θ�
  * @param includeSub : TRUE�̸� ���� �׸� ������Ʈ�Ѵ�.
  * @details A. KSyncRootStorage::UpdateMetaFolderNameMapping()�� ȣ���Ѵ�.
  */
  BOOL UpdateMetaFolderNameMapping(LPCTSTR lastRelativePath, LPCTSTR newRelativePath, BOOL includeSub = FALSE);

  /**
  * @brief �� ������ ������ fullPathIndex�� �ٲ������ Ȯ���ϴ� �����Լ��̴�.
  */
  virtual inline BOOL IsFolderFullPathChanged() { return FALSE; }

  /**
  * @brief ����ȭ ���� ��ü ũ�⸦ �����Ѵ�.
  */
  inline __int64 GetSyncTotalSize() { return mSyncTotalSize; }
  /**
  * @brief ����ȭ �Ϸ�� ���� ũ�⸦ �����Ѵ�.
  */
  inline __int64 GetSyncDoneSize() { return mSyncDoneSize; }

  /**
  * @brief ��� �׸� ���� ����ȭ �۾��� �����Ѵ�.
  * @param item : ��� �׸�
  * @param phase : ����ȭ �۾� �ܰ�.
  * @return int : ����Ϸ��� NEED_CONTINUE�� �����Ѵ�.
  * @details A. item�� ��Ʈ �����׸��̰ų� ���ܵ� �׸��̸� phase�� ������Ʈ�ϰ� NEED_CONTINUE�� �����Ѵ�.
  * @n B. mSyncItemFileSize�� GetSyncFileSize()���� ���´�.
  * @n C. phase�� PHASE_CONFLICT_FILES�� �ƴϰų� �׸��� Enable�����̸�
  *     - �׸��� doSyncItem()�� ȣ���Ͽ� �۾��� �����ϰ�, �� ���ϰ��� FILE_SYNC_THREAD_WAIT�̸� r�� �����Ѵ�.
  *     - ���ϰ��� ������ �浹�� �߻��Ͽ����� phase�� PHASE_ENDING�� �Ͽ� �ٸ� �۾��� ���� �ʵ��� �Ѵ�.
  *     .
  * @n D. �ƴϸ�, �׸��� disable�̸� phase�� PHASE_END_OF_JOB�� �Ѵ�.
  * @n E. phase�� PHASE_COPY_FILES���� ũ�ų� ������, OnSyncFileItemDone()�� ȣ���Ѵ�.
  */
  int syncFileItem(KSyncItem *item, int phase);

  /**
  * @brief ��� �׸� ���� ����ȭ �۾��� �Ϸ�Ǿ����� ȣ��ȴ�.
  * @param item : ��� �׸�
  * @param r : �����
  * @param sync_size : ����ȭ ���� ũ��
  * @details A. item�� �� �׸��� �ƴϸ�, mSyncDoneSize, mSyncDoneSizeProgress�� sync_size�� ���Ѵ�.
  * @n B. item�� �����̸� mSyncFileCount�� �����Ѵ�.
  * @n C. mSyncProcessedSize�� 0���� �Ѵ�.
  * @n D. ������� ����ȭ �浹�̳� ������ ����, �׸��� �����̸�, RemoveItem���� ��Ͽ��� �����ϰ� �׸��� �����Ѵ�.
  */
  void OnSyncFileItemDone(KSyncItem *item, int r, __int64 sync_size, int phase);

  /**
  * @brief ������ ������ ����ȭ ��Ʈ �������� Ȯ���Ѵ�.
  * @param pathName : ��θ�
  * @param onlyCurrentUser : TRUE�̸� ������ ������� ����ȭ ������ �˻��Ѵ�.
  * @return int : -1 for other user's, 0 for none, 1 for normal sync-root, 2 for initializing sync-root
  * @details A. KSyncRootStorage::IsMatchRootFolder()�� ȣ���ϰ� �� ���ϰ��� �����Ѵ�.
  */
  static int IsSyncRootFolder(LPCTSTR pathName, BOOL onlyCurrentUser = FALSE);

  // static int ReadySyncMetaData(LPCTSTR path, LPCTSTR base_path, LPCTSTR object_path, KSyncPolicy *p);

  /**
  * @brief ������ ���ϸ��� ����ȭ ���� ���Ŀ� �ش��ϴ��� �˻��Ѵ�.
  * @param filename : ���ϸ�
  * @return BOOL : ���� ���Ŀ� �ش��ϸ� TRUE�� �����Ѵ�.
  * @details A. KSyncCoreEngine::IsExcludeFileMask()�� ȣ���Ͽ� ������ ���� ���Ŀ� �ش��ϴ��� �˻��Ѵ�.
  * @n B. KSyncPolicy::IsExcludeFileMask()�� �� ����ȭ ������ ��å�� ������ ���� ���Ŀ� �ش��ϴ��� �˻��Ѵ�.
  */
  BOOL IsExcludeFileMask(LPCTSTR filename);

  /**
  * @brief ��� �׸��� ����ȭ ���� ���Ŀ� �ش��ϴ��� �˻��Ѵ�.
  * @param item : ��� �׸�
  * @return BOOL : ���� ���Ŀ� �ش��ϸ� TRUE�� �����Ѵ�.
  * @details A. KSyncCoreEngine::IsExcludeFileMask()�� ȣ���Ͽ� ������ ���� ���Ŀ� �ش��ϴ��� �˻��Ѵ�.
  * @n B. KSyncPolicy::IsExcludeFileMask()�� ȣ���Ͽ� �� ����ȭ ������ ��å�� ������ ���� ���Ŀ� �ش��ϴ��� �˻��Ѵ�.
  * @n C. ��� ��θ��� �غ��Ͽ�, KSyncPolicy::IsExcludeFileMask()�� ȣ���Ͽ� �˻��Ѵ�.
  */
  BOOL IsExcludeFileMask(KSyncItem* item);

  /**
  * @brief ���� �׸���� ��� �˻��Ͽ� ����ȭ ���� ���Ŀ� �ش��ϴ� �׸���� disable��Ų��.
  * @return BOOL : ���� ���Ŀ� �ش��ϴ� �׸��� ������ TRUE�� �����Ѵ�.
  * @details A. KSyncFolderItem::DisableExcludedFileMask()�� ȣ���Ͽ� ����ȭ ���� ���Ŀ� �ش��ϴ� ���� �׸���� disable��Ų��.
  */
  BOOL CheckExcludeNameMask();

  /**
  * @brief ����ȭ ��å ������ ���丮������ �о���ų� �����Ѵ�.
  * @param bStore : TRUE�̸� ���丮������ �о�´�.
  * @details A. �� ����ȭ ������ KSyncRootStorageFile::IsMatchRootFolder()�̸�,
  *     - KSyncRootStorageFile Ŭ������ �غ��Ͽ� StoreSyncPolicy(), �Ǵ� LoadSyncPolicy()�� ȣ���Ѵ�.
  *     .
  * @n B. �ƴϸ�
  *     - KSyncRootStorageDB Ŭ������ �غ��Ͽ� StoreSyncPolicy(), �Ǵ� LoadSyncPolicy()�� ȣ���Ѵ�.
  */
  BOOL SerializePolicy(BOOL bStore);

  /**
  * @brief ������ �ð��� ������ ����ȭ �Ϸ� �ð� �������� Ȯ���Ѵ�.
  * @param lastModifiedTime : ������ �ð�
  * @return BOOL : ������ �ð��� ������ ����ȭ �Ϸ� �ð� �����̸� TRUE�� �����Ѵ�.
  * @details A. ������ ����ȭ �Ϸ� �ð�(mLastSyncCompleteTime)�� ��ȿ�ϰ�, ������ �ð��� �̺��� ũ�� TRUE�� �����Ѵ�.
  */
  BOOL IsChangedAfterLastSync(SYSTEMTIME &lastModifiedTime);

  /**
  * @brief ����ȭ �Ϸ� �ð��� ������Ʈ�Ѵ�.
  * @param success : ����ȭ �Ϸ����� �浹�̳� ���� �׸��� �ִ��� ����.
  * @details A. success�� TRUE�̸� mSyncDoneTime�� AppendSettingKeyValue()���� KEY_LAST_SYNCŰ�� �����Ѵ�.
  * @n B. success�� TRUE�̰� �κ� ����ȭ�� �ƴϸ�,
  *     - KSyncRootStorage::StoreSettingValue()�� KEYB_LAST_SYNC���� �����Ѵ�.
  *     - KSyncRootStorage::StoreSettingValue()�� KEYB_LAST_SCAN_TIME���� mPreviousScanTime���� �����Ѵ�.
  *     - mPreviousScanTime���� mLastScanTime�� �����Ѵ�.
  *     .
  * @n B. �ڵ� ����ȭ�̸�,
  *     - KSyncRootStorage::StoreSettingValue()�� KEYB_LAST_AUTOSYNC���� �����Ѵ�.
  *     .
  */
  void UpdateRootLastSyncTime(int flag);

  /**
  * @brief �� ����ȭ ������ ������ ����ȭ �Ϸ� �ð��� �о�´�.
  * @param auto_time : ������ �ڵ� ����ȭ �ð�
  * @param sync_time : ������ ����ȭ �ð�
  * @param setting_time : ������ ����ȭ ��å ���� �ð�
  * @return BOOL : �������� �о����� TRUE�� �����Ѵ�.
  * @details A. auto_time ���۰� NULL�� �ƴϸ� KSyncRootStorage::LoadSettingValue()���� KEYB_LAST_AUTOSYNC Ű���� �о�´�.
  * @n B. sync_time ���۰� NULL�� �ƴϸ� KSyncRootStorage::LoadSettingValue()���� KEYB_LAST_SYNC Ű���� �о�´�.
  * @n C. setting_time ���۰� NULL�� �ƴϸ� KSyncRootStorage::LoadSettingValue()���� KEYB_LAST_SETTING Ű���� �о�´�.
  */
  BOOL GetRootLastSyncTime(SYSTEMTIME *auto_time, SYSTEMTIME *sync_time, SYSTEMTIME *setting_time);

  /**
  * @brief ����ȭ ���ε�/�ٿ�ε� �۾����� �׸����� ������ ������ ����Ѵ�.
  * DirectoryMonitor���� ����ȭ �۾����� ������ �����ϵ��� �ϱ� ���� ����Ѵ�.
  * @param fileName : ��� ������ ��ü ��θ�
  * @return BOOL : ��ϵǾ����� TRUE�� �����Ѵ�.
  * @details A. ��ü ��θ��� �� ����ȭ ������ ������ ���ϸ�,
  *     - clearRegisteredOverTime()���� Ÿ�ӿ����� �׸��� �����ϰ�
  *     - SYNCING_FILE_ITEM struct�� �޸� �Ҵ��Ͽ� ���� ��θ�� ������ TickCount�� �����ϰ�,
  *     - mSyncingItems��̿� �߰��ϰ� TRUE�� �����Ѵ�.
  *     .
  * @n B. FALSE�� �����Ѵ�.
  */
  BOOL AddSyncingFileItem(LPCTSTR fileName);

  /**
  * @brief DirectoryMonitor���� ��� ������ ������ ������ Ȯ���Ѵ�.
  * @param fileName : ��� ������ ��ü ��θ�
  * @return int : �����ص� �Ǹ� 1 �Ǵ� 2�� �����Ѵ�.
  * @details A. ��ü ��θ��� �� ����ȭ ������ ������ ���ϸ�,
  *     - ������ ��� ��θ��� �غ��ϰ�,
  *     - �� ������ DB �����̳� ".filesync" ��Ÿ �����̸� 1�� �����Ѵ�.
  *     - mSyncingItems ��̿��� �� ������ �ִ��� Ȯ���ϰ� ������ 2�� �����Ѵ�.
  *     .
  * @n B. 0�� �����Ѵ�.
  */
  int IsMonitoringSkipFileItem(LPCTSTR fileName);

  /**
  * @brief ����ȭ �������� �������� Ȯ���Ѵ�.
  * @param fileName : ��� ������ ��ü ��θ�
  * @return int : ����ȭ �������� �����̸� 2�� �����Ѵ�.
  */ 
  SFI_TYPE IsSyncingFileItem(LPCTSTR fileName);

  /**
  * @brief ����ȭ �������� �������� Ȯ���Ѵ�.
  * @param fileName : ��� ������ ��� ��θ�
  * @return int : ����ȭ �������� �����̸� 2�� �����Ѵ�.
  */ 
  SFI_TYPE IsSyncingFileItemRelative(LPCTSTR fileName);

  /**
  * @brief mSyncingItems�� ��ϵ� �׸񿡼� Ÿ�Ӿƿ��� �׸��� �����Ѵ�.
  * ��ϵ��� 2�� �̻� ���� �׸��� �����Ѵ�.
  */
  void clearRegisteredOverTime();

  /**
  * @brief mSyncingItems�� ��ϵ� �׸񿡼� ��ü �׸��� �����Ѵ�.
  */
  void clearAllSyncingItem();

  /**
  * @brief �ڵ� ����ȭ �Ͻ� ����/ �ٽ� ������ �����Ѵ�.
  * @param paused : TRUE�̸� �Ͻ� ������ �ϰ� FALSE�̸� �ٽ� �������� �Ѵ�.
  * @details A. mPolicy.SetPaused()�� �����ϰ�,
  * @n B. KSyncRootStorage::StoreSyncPaused()�� �������� �����Ѵ�.
  */
  void SavePausedState(BOOL paused);

  /**
  * @brief ����ȭ ��Ʈ ������ ������ ���丮���� �����Ѵ�. SETTING_TABLE�� ����ȴ�.
  * @param server_folder_oid : ������ ���� OID
  * @param fullPath : ������ ������ ��ü ��θ�
  * @param fullPathIndex : ������ ������ fullPathIndex
  * @param server_location_changed : ������ ���� ��ġ�� �ٲ������ TRUE
  * @return int : �����ϸ� R_SUCCESS�� �����Ѵ�.
  * @details A. server_location_changed�̸� KEYB_SERVER_NEW_OIDŰ�� server_folder_oid�� �����Ѵ�.
  * @n B. �ƴϸ�,
  *     - KEYB_SERVER_OID Ű�� server_folder_oid�� �����Ѵ�.
  *     - KEYB_SERVER_NEW_OID Ű�� NULL�� �����Ѵ�.
  *     .
  * @n C. KEYB_SERVER_FULLPATH Ű�� fullPath�� �����Ѵ�.
  * @n D. KEYB_SERVER_FULLPATHINDEX Ű�� fullPathIndex�� �����Ѵ�.
  * @n E. R_SUCCESS�� �����Ѵ�.
  */
  int StoreRootFolderServerPath(LPTSTR server_folder_oid, LPTSTR fullPath, LPTSTR fullPathIndex, BOOL server_location_changed);

  /**
  * @brief ����ȭ ��Ʈ ������ ������ ���丮������ �о�´�.
  * @details A. KEYB_SERVER_FULLPATHŰ���� ������ ��θ� �о�´�.
  * @n B. KEYB_SERVER_FULLPATHINDEXŰ���� ������ fullPathIndex�� �о�´�.
  * @n C. KEYB_LAST_SCAN_TIMEŰ���� mLastScanTime�� �о�´�.
  */
  void LoadServerPrevious();

  /**
  * @brief ����ȭ ��Ʈ ������ ��ġ�� �ٲ������ �����Ѵ�.
  * @param set : TRUE�̸� �ٲ������ �����ϰ�, �ƴϸ� �����Ѵ�.
  */
  inline void SetServerLocationChanged(BOOL set) { mPolicy.ChangeSyncFlag(SYNC_SERVER_LOCATION_CHANGED, set); }

  int MergeSyncOnlyPath(LPCTSTR relativePath);

  /**
  * @brief �κ� ����ȭ �۾����� ����ȭ�� ������ ��� ��θ��� �����Ѵ�.
  * @param relativePath : �κ� ����ȭ�� ������ ��� ��θ�.
  * @return int : �����ϸ� R_SUCCESS�� �����Ѵ�.
  * @details A. relativePath�� ��ȿ�ϸ�
  *     - mSyncOnlyPath�� relativePath�� �����Ѵ�.
  *     - GetMovedRenamedOriginalName()���� relativePath�� �ش��ϴ� �̵��� ���� ������ �ִ��� Ȯ���ϰ�,
  *       + �ش� ������ ����ȭ �Ϸ� ���°� �ƴϸ�, R_FAIL_NEED_FULL_SYNC_VERGIN�� �����Ѵ�.
  *     - �ƴϸ� relativePath�� ����ȭ �Ϸ� ���°� �ƴϸ�, R_FAIL_NEED_FULL_SYNC_VERGIN�� �����Ѵ�.
  *     - CheckRenamedInOut()�� ���� ���������� ������, �Ǵ� �ۿ��� �� ������ �̵��� �׸��� �ִ��� �˻��Ͽ�,
  * ������ R_FAIL_NEED_FULL_SYNC_PARTIAL�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ� mSyncOnlyPath�� Ŭ�����Ѵ�.
  */
  int SetSyncOnlyPath(LPCTSTR relativePath);

  void SetSyncExtraInfo(LPCTSTR info);

  /**
  * @brief ��� ��θ� �ش��ϴ� �׸��� �����Ͽ� ��Ÿ �����͸� �ε��ϰ� �׸��� �����Ѵ�.
  * @param relativePathName : ��� �׸��� ��θ�
  * @param isFolder : �������� ��������.
  * @return KSyncItem* : ������ �׸�
  * @details A. �����̸� KSyncFolderItem��, �ƴϸ� KSyncItem ��ü�� �����Ѵ�.
  * @n B. ��Ÿ �����͸� �ε��Ѵ�. ��Ÿ�����Ͱ� ������,
  * @n C. GetMovedRenamedOriginalName()���� �̸� ����� original ��ü �̸��� �����ϸ�,
  *     - �ռ� ������ ��ü�� �����ϰ� �� �̸����� ��ü�� �ٽ� �����ϰ�, ��Ÿ �����͸� �ε��Ѵ�.
  *     .
  * @n D. ������ ��ü�� �����Ѵ�.
  */
  KSyncItem* CreateReadyChildMetafile(LPCTSTR relativePathName, BOOL isFolder);

  /**
  * @brief ����ȭ ���� ������ ������ �̸��� ����Ǿ����� ȣ��ȴ�.
  * @param oldFileName : ���� ���� ��θ�
  * @param newFileName : �� ���� ��θ�
  * @return int : ���� ���ϸ��� ����ȭ ���� �ٱ��̸� 0�� ����, ���� ���ϸ��� ����ȭ ��Ʈ ���� ���� �׸��̸� 1�� ����,
  * ��Ÿ ���� �Ǵ� ��Ÿ DB �����̸� 2, ����ȭ ��Ʈ �����̸� 3�� �����Ѵ�.
  * @details A. oldFileName�� mBaseFolder�� ���� �����̸�,
  *     - mBaseFolder�� ������ 3�� �����Ѵ�.
  *     - ��Ÿ ���� �Ǵ� ��Ÿ DB �����̸� 2�� �����Ѵ�.
  *     - 1�� �����Ѵ�.
  *     .
  * @n B. 0�� �����Ѵ�.
  */ 
  int OnFileRenamed(LPCTSTR oldFileName, LPCTSTR newFileName);

  /**
  * @brief ����ȭ ���� ������ ������ �̸��� ���� ������ ���丮���� RENAME TABLE�� �����Ѵ�.
  * @param oldFileName : ���� ���� ��θ�
  * @param newFileName : �� ���� ��θ�
  * @param forceAppend : TRUE�̸� ������ �߰��Ѵ�.
  * @return BOOL : TRUE�� �����Ѵ�.
  * @details A. �� ��θ��� �������� �������� Ȯ���Ѵ�.
  * @n B. forceAppend�� FALSE�̸�,
  *     - CreateReadyChildMetafile()���� ��� �׸��� ã�ƿͼ�,
  *     - �׸��� ���ų� ���� ����ȭ �ȵ� �׸��̸� append�� FALSE�� �Ѵ�.
  *     .
  * @n C. �׸��� NULL�� �ƴϸ�
  *     - �׸��� ���� ��θ��� ���ϰ�, �̰��� �̹� �̸� ����Ǿ����� Ȯ���Ͽ�,
  *     - �׸��� ���� ��θ�� oldFileName�� �ٸ���,
  *       + KSyncRootStorage::SetRenameT()���� �׸��� ���� ��θ�� newFileName�� �����Ѵ�.
  *       + �� �׸��� �����̸�, KSyncRootStorage::SetRenameT()���� �̸� ����� �̸��� newFileName����,
  * ��θ� ���濡 ���� ������Ʈ�� ������Ʈ(by KSyncRootStorage::KRS_CHANGE_PATH)�� �Ѵ�.
  *       + or_name�� NULL�� �����Ѵ�.
  *     .
  * @n D. or_name�� NULL�� �ƴϸ�,
  *     - KSyncRootStorage::SetRenameT()���� �׸��� ���� ��θ�� �� ��θ��� �߰��Ѵ�.
  *     - �׸��� �����̸�,
  *       + KSyncRootStorage::SetRenameT()���� ��θ� ���濡 ���� ������Ʈ�� ������Ʈ(by KSyncRootStorage::KRS_CHANGE_PATH)�� �Ѵ�.
  *     .
  */ 
  BOOL OnStoreRenamed(LPCTSTR oldFileName, LPCTSTR newFileName, BOOL forceAppend);

  /**
  * @brief �̸� ����� �׸��� �����Ǿ����� ȣ��ȴ�.
  * @param newFileName : ������ ��θ�
  * @return int : �ش� �׸��� �� ����ȭ ��Ʈ ���� ���� �׸��̸� 1�� �����Ѵ�.
  * @details A. IsChildFolderOrFile()���� ��θ��� �� ���� �����̸� 1�� �����Ѵ�.
  */
  int OnFileRenameRemoved(LPCTSTR newFileName);

  /**
  * @brief �̸� ����� �׸��� �����Ǿ� �� ����� �̸� ���� ���̺��� ������ �� ȣ��ȴ�.
  * @param newFileName : �� ���� ��θ�
  * @param oldFileName : ���� ���� ��θ�
  * @return BOOL : ���������� �����Ǹ� TRUE�� �����Ѵ�.
  * @details A. KSyncRootStorage::RemoveRenameItem()���� �̸� ���� ������ �����Ѵ�.
  */
  BOOL OnRemoveRenamed(LPCTSTR newFileName, LPCTSTR oldFileName);

  /**
  * @brief ����ȭ �۾� ������ ���� �̸� ����� ���, �̰��� mRenamedStack�� �����Ͽ��ٰ�, �������� �̸� ���� ���丮������ �����Ѵ�.
  * @param oldFileName : ���� ���� ��θ�
  * @param newFileName : �� ���� ��θ�
  * @return BOOL : ���������� �����Ǹ� TRUE�� �����Ѵ�.
  * @details A. ���� ��θ�� �� ��θ��� DA_VALUE_SEPARATOR �� ���ļ� ��Ʈ���� ����� �̸� mRenamedStack�� �����Ѵ�.
  */
  BOOL OnStoreCancelRenamed(LPCTSTR oldFileName, LPCTSTR newFileName);

  /**
  * @brief mRenamedStack�� ����� �̸� ���� ������, �̸� ���� ���丮������ �����Ѵ�.
  * @param clearData : TRUE�̸� �̸� ���� ���丮������ �����Ѵ�.
  * @details A. mRenamedStack�� �� �׸� ����,
  *     - clearData�� TRUE�̸�,
  *       + oldname, newname���� �и��Ͽ� KSyncRootStorage::RemoveRenameItem()���� �̸� ���� ���丮������ �����Ѵ�.
  *     - �׸��� �����Ѵ�.
  *     .
  * @n B. mRenamedStack�� Ŭ�����Ѵ�.
  */
  void clearCancelRenamedItem(BOOL clearData);

  // BOOL GetValidLocalRenamePath(LPTSTR newRelativePath);

#ifdef USE_BASE_FOLDER
  virtual LPCTSTR GetRootPath() { return mBaseFolder; }
#else
  virtual LPCTSTR GetRootPath() { return mpRelativePath; }
#endif

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
  /** ������ ������ ����ȭ �۾��ÿ� ���� �׸� ��ȸ�� ��û�� �ð�. ������ ���� �׸� ��ȸ�� ��û�� �� �� �ð����� �̿��Ѵ�. */
  __int64 mPreviousScanTime;

  /** ������ ���� �׸� ��ȸ�� ��û�� �ð�. ����ȭ�� �Ϸ�Ǹ� �� ���� mPreviousScanTime���� ����ȴ� */
  __int64 mLastScanTime;

  /** getModifiedFolderNDocument API�� ��û�Ͽ� �ٿ�ε� ���� �����͸� �����ϴ� ���� */
  LPSTR m_ScanFolderData;

  /** getModifiedFolderNDocument API�� ��û�Ͽ� �ٿ�ε� ���� ������ ������ ũ�� */
  int m_ScanFolderDataLength;

  /** getModifiedFolderNDocument API�� ��û�� �۾��� �÷��� */
  int m_ScanFolderFlag;

  /** getModifiedFolderNDocument API�� ��û�Ͽ� �ٿ�ε� ���� �����Ϳ��� ó���� ���� ������ */
  int m_ScanFolderIndex;

  /** getModifiedFolderNDocument API�� ��û�� ���� �׸� */
  KSyncFolderItem* m_ScanFolderItem;

  /** getModifiedFolderNDocument API�� ��û�� ������ �Ľ��Ҷ� ���� �׸��� �θ� ���� ĳ��. */
  KSyncFolderItem* m_ScanParseLastParent;

  /**
  * @brief ������ ������ ����ȭ �۾��ÿ� ���� �׸� ��ȸ�� ��û�� �ð��� �����Ѵ�.
  */
  inline void SetPreviousScanTimeStamp(__int64 scanTime) { mPreviousScanTime = scanTime; }

  /**
  * @brief ������ ������ ����ȭ �۾��ÿ� ���� �׸� ��ȸ�� ��û�� �ð��� �޾ư���.
  */
  inline __int64 GetPreviousScanTimeStamp() { return mLastScanTime; }

  /** getModifiedFolderNDocument API�� ��û�� �����. */
  KDownloaderError mServerFolderScanResult;

  /**
  * @brief getModifiedFolderNDocument API�� ��û�� ������� �����Ѵ�.
  */
  inline void SetServerFolderScanResult(KDownloaderError r) { mServerFolderScanResult = r; }
  /**
  * @brief getModifiedFolderNDocument API�� ��û�� ������� �޾ư���.
  */
  inline KDownloaderError GetServerFolderScanResult() { return mServerFolderScanResult; }

#endif // USE_SCAN_BY_HTTP_REQUEST && USE_SYNC_GUI

#if defined(USE_SYNC_GUI) && defined(PURE_INSTANT_BACKUP)
  // �ǽð� ����� ��� �׸� ����Ʈ
  KVoidPtrArray mModifiedItemList;
  // �ǽð� ����� ��� �׸� ����Ʈ db key
  LPTSTR mModifiedKey;

  BOOL AddModifiedItemList(KVoidPtrArray& modifiedList);

  void SetModifiedListKey(LPCTSTR modifiedKey);

#endif // USE_SYNC_GUI

  inline int GetDBVersion() { return mDbVersion; }

private:

  /** �� ����ȭ ��Ʈ ������ ����ȭ ��å */
  KSyncPolicy mPolicy;

#ifdef USE_META_STORAGE

  /** �� ����ȭ ��Ʈ ������ ���丮�� ��ü */
  KSyncRootStorage* mStorage;

  /** Old storage class used on Data immigration */
  KSyncRootStorage* moldStorage;
#endif

  /** ����ȭ �۾� ����. none, pending, synching, sync-done, sync-paused */
  int mSynchState;
  /** ����ȭ �Ͻ� ���� ����. pause/resume */
  int mSyncPaused;
  /** ����ȭ ���. manual or automatic */
  int mSyncMethod;

  int mDbVersion;

public:
  /** ����ȭ ��ü ũ�� */
  __int64 mSyncTotalSize;
  /** ����ȭ �Ϸ�� ũ�� */
  __int64 mSyncDoneSize;

  /** ����ȭ �Ϸ����� ����ϱ� ���� �Ϸ�� ũ�� */
  __int64 mSyncDoneSizeProgress;
  /** ����ȭ �Ϸ����� ����ϱ� ���� ��ü ũ�� */
  __int64 mSyncTotalSizeProgress;

  /** ����ȭ ��ü �׸� ���� */
  int mSyncTotalCount;
  /** ����ȭ ó���� ���� �׸� ���� */
  int mSyncFolderCount;
  /** ����ȭ ó���� ���� �׸� ���� */
  int mSyncFileCount;

  /** ����ȭ ��� ���� �׸� ���� */
  int mTargetFolderCount;
  /** ����ȭ ��� ���� �׸� ���� */
  int mTargetFileCount;


  /** ����ȭ �۾��� ���ε�/�ٿ�ε� �����ϴ� ������ ��ü ũ�� */
  __int64 mSyncItemFileSize;
  /** ����ȭ �۾��� ���ε�/�ٿ�ε� �����ϴ� ������ �Ϸ�� ũ�� */
  __int64 mSyncProcessedSize;

  /** ����ȭ �۾� ���� �ð� */
  SYSTEMTIME mSyncBeginTime; // start
  /** ����ȭ ���ε�/�ٿ�ε� ���� �ð�. ����ȭ ����â�� ������� �� �ð����� ����Ѵ� */
  SYSTEMTIME mSyncStartTime;
  /** ����ȭ �۾� �Ϸ� �ð� */
  SYSTEMTIME mSyncDoneTime;

  // working buffer
  /** �κ� ����ȭ ���� ��θ� */
  LPTSTR mSyncOnlyPath;
  /** ����ȭ �߰� ����, ���� �۾��� ���� ��¥ ���� */
  LPTSTR mSyncExtraInfo;
  /** ������ ����ȭ �Ϸ� �ð� */
  SYSTEMTIME mLastSyncCompleteTime;

  /** ����ȭ �۾����� ���� ��� */
  KVoidPtrArray mSyncingItems;

  /** ����ȭ �۾��� �̸� ���� �׸� ��� */
  KVoidPtrArray mRenamedStack;

#ifdef USE_BACKUP
  /** ���� �������� ���� ó���� �׸� ����Ʈ */
  LPSTR mDOS_FolderList;
  int mDOS_FolderListSize;
  LPSTR mDOS_FileList;
  int mDOS_FileListSize;

  /* �ǽð� ��� Retry ȸ�� */
  int mInstantBackupRetry;

  TCHAR mRestoreFolderOID[MAX_OID];

  /**
  * @brief ������ ������ ���� OID�� �����Ѵ�.
  */
  void SetRestoreFolderOID(LPCTSTR folderOID) { StringCchCopy(mRestoreFolderOID, MAX_OID, folderOID); }

  /**
  * @brief ��� �������� ���� ó����(~#*.*) �׸����� ����Ѵ�.
  * @n details ������ mDOS_FolderList��, ������ mDOS_FileList�� ������ �׸��� OID�� �߰��Ѵ�.
  */
  BOOL RegistDeleteOnServer(KSyncItem* item);

  /**
  * @brief ��� �������� ���� ó���� �׸��� �������� ������û�Ѵ�.
  * @n details gpCoreEngine->RequestRemoveFiles()�� ȣ���Ͽ� ������ ��û�Ѵ�.
  */
  int RequestDeleteOnServerItemList();

  /**
  * @brief ���������� ���� ó���� �ϰ� ����� ���Ϲ޾����� ȣ��ȴ�.
  * @n details ���� ��û�� ��Ͽ��� ó�� ��� �Ϸ�� �׸��� �����Ͽ� ������ �̿Ϸ�� �׸� ���ܵд�.
  */
  void OnDeleteOnServerItem(int isFolder, LPSTR data);

  /**
  * @brief ���������� ���� ó���� �ϰ� ����� ���Ϲ޾����� ȣ��ȴ�.
  * @n details ���� ��û�� ��Ͽ��� ó�� ��� �Ϸ�� �׸��� �����Ͽ� ������ �̿Ϸ�� �׸� ���ܵд�.
  */
  void OnRequestDeleteOnServerItemList(LPSTR data, long length);

  /**
  * @brief �������� ���� ó�� ��û�� �����Ͽ����� ȣ��ȴ�.
  * @n details ���� ��û�� �׸���� ����ó���Ѵ�.
  */
  void SetDeleteOnServerItemResult(LPSTR p);

  /**
  * @brief ���� ��Ͽ��� ������ �׸��� �Ϸ�� �����Ѵ�.
  */
  BOOL ClearDeleteOnServerItemAsDone(LPSTR list, LPSTR itemOID);

  /**
  * @brief �������� ���� ó�� ��û�� �����Ͽ����� ȣ��ȴ�.
  * @n details ���� ��û�� �׸���� ����ó���Ѵ�.
  */
  void SetDeleteOnServerItemsFail(int r);

#endif // USE_BACKUP

#if 0
  KVoidPtrArray mModifyCancelStack;
#endif

};

#include <vector>

typedef std::vector<KSyncRootFolderItem*> KSyncRootItemArray;
typedef std::vector<KSyncRootFolderItem*>::iterator KSyncRootItemIter;

  /**
  * @brief ����ȭ ��Ʈ ���� �׸��� ����Ʈ�� �����Ѵ�.
  */
extern void FreeSyncRootItemList(KSyncRootItemArray &array);

#endif