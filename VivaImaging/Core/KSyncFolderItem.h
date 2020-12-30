/**
* @file KSyncFolderItem.h
* @date 2015.02
* @brief KSyncFolderItem class header file
*/
#pragma once

#ifndef _DESTINY_FILE_SYNC_FOLDER_ITEM_CLASS_H_
#define _DESTINY_FILE_SYNC_FOLDER_ITEM_CLASS_H_

#include "KSyncItem.h"

class KSyncPolicy;

// scanSubObjects flag
#define SCAN_FOR_ALL_OBJECT 0x0100
#define SCAN_CLEAR_LOCAL_STATE 0x0200
#define SCAN_CLEAR_SERVER_STATE 0x0400
#define SCAN_CHANGED_FILE_ONLY 0x0800
#define SCAN_CONFLICT_ONLY 0x1000

// UpdateChildItemPath flag
#define UPDATE_WITH_FULLPATHINDEX 0x01

// flags on deleteLocalChildsRecursive
#define DELETE_LOCAL_FILES 1
#define EXCLUDE_OUTGOING 8 /** do not remove which will be moved out */

// flags on SetConflictResultRecurse
#define SET_WITH_ITSELF 1
#define SET_SERVER_DELETED 2
#define SET_LOCAL_DELETED 4
#define SET_WITH_RECURSIVE 8
#define STORE_META 0x080
#define SET_SELECTED_ONLY 0x0100

  // flags on UpdateFolderSyncResult
#define REMOVE_DISABLED 1
// #define STORE_META 2
#define SET_CONFLICT_CHILD 4
#define CLEAR_NOW_SYNCHRONIZING 8

// flag of GetResolveChildItems()
#define EX_DUMMY 1
#define IN_SINGLE_DUMMY 2

// flags on FindChildItemCascade()
#define WITH_RENAMED 1
#define ONLY_EXISTING 2
// flags on CreateChildItemCascade
#define REFRESH_LOCALNEW 1
#define REFRESH_RENAMED 2 // ������ �׸��� �ٸ� �̸����� �̸����� �Ǿ����� Ȯ��.
#define SET_SCAN_DONE 4 // ���� �߰��� �׸��� ��ĵ �Ϸ�� ������ ����
#define SET_SCAN_NEED 8

// flags on RemoveUnBackupedItems()
#define REMOVE_WITH_STORAGE 1

// mask on SetPhaseRecurse()
#define EXCLUDE_CHANGED_ITEM 1


class KSyncCoreBase;
class KSyncRootStorage;

/**
* @class KSyncFolderItem
* @brief ����ȭ ��� �׸��� ���� �׸� Ŭ����
*/
class KSyncFolderItem :
  public KSyncItem
{
public:
  KSyncFolderItem(KSyncItem *p);
  KSyncFolderItem(KSyncItem *p, LPCTSTR fullpathname);
  KSyncFolderItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename);
  KSyncFolderItem(KSyncItem *p, LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid);
  virtual ~KSyncFolderItem(void);

  /**
  * @brief Assign ������('=') ���� �Լ�.
  */
  KSyncFolderItem &operator=(KSyncFolderItem &item);

  /**
  * @brief �� �׸�� ������ �׸��� �����Ͽ� �����Ѵ�.
  */
  virtual KSyncItem *DuplicateItem();

  /**
  * @brief �� �׸��� ����(���� �Ǵ� ����)�� �����Ѵ�.
  */
  virtual int IsFolder() { return 1; }

  /**
  * @brief ���� �׸��� ������ �����Ѵ�. ������ ������ �������� �ʴ´�.
  */
  inline int GetChildItemCount() { return (int) mChildList.size(); }
  /**
  * @brief ������ N��° �׸��� �����Ѵ�.
  */
  inline KSyncItem* GetChildItem(int index) { return mChildList.at(index); }
  /**
  * @brief ���� �׸��� ��� �����Ѵ�.
  */
  inline void RemoveChilds() { mChildList.clear(); }

  /**
  * @brief �� �׸�� ���� �׸� ��ü�� ����ȭ Enable�Ѵ�.
  */
  virtual void SetEnableRecurse(BOOL e);

  /**
  * @brief ���� �׸��� ����ȭ Enable�� �׸��� ������ TRUE�� �����Ѵ�.
  */
  virtual BOOL GetEnableChild();

  virtual BOOL IsExcludedSyncRecur();

  /**
  * @brief �浹 ������� ����� ���⼱���� �����Ѵ�.
  * @param r : ����� ���� ����
  * @param flag : �÷���
  * @details A. ������ �׸� ���� �浹 ������� ����� ���⼱���� �����Ѵ�.
  * @n B. �÷��׿� WITH_RECURSE�� ������ Child �׸�鿡�� KSyncItem::SetConflictResultAction() �޼ҵ带 ��������� ȣ���Ѵ�.
  */
  virtual void SetConflictResultAction(int r, int flag);

  /**
  * @brief ������ ������ fullPathIndex�� �ٲ������ Ȯ���Ѵ�. fullPathIndex�� �ٲ������ ������ ��ü ��ΰ� �ٲ������ �ǹ��Ѵ�.
  * @return BOOL : ����Ǿ����� TRUE�� �����Ѵ�.
  * @details A. mServer�� pFullPathIndex�� NULL�̰� mServerNew�� pFullPathIndex�� NULL�� �ƴϸ� ���� �߰��� �׸��̴� FALSE�� �����Ѵ�.
  * @n B. mServer�� mServerNew�� pFullPathIndex�� ���Ͽ� �ٸ��� TRUE�� �����Ѵ�.
  */
  virtual BOOL IsFolderFullPathChanged();

  /**
  * @brief ���������� Ư�� ������ ���� �׸��߿��� ������ �׸� �޾ƿ��� ��� ó������ ��� �׸��� ���¸� �����ϴ� �����Լ�.
  * @param s : �׸��� scan ����(���� ����, ������, ������)
  * @param flag : �÷���
  * @details A. KSyncItem::SetConflictResultAction()�� ȣ���Ͽ� �� �׸� ���� ���°��� �����Ѵ�.
  * @n B. �÷��׿� WITH_RECURSE�� ������ Child �׸�鿡�� KSyncItem::SetConflictResultAction() �޼ҵ带 ��������� ȣ���Ѵ�.
  */
  virtual void SetScanServerFolderState(int s, int flag);
  /**
  * @brief SetScanServerFolderState()���� ������ ���°��� ���� ������ ���°��� �����Ѵ�.
  * @details A. KSyncItem::SetServerStateByScanState()�� ȣ���Ͽ� �� �׸� ���� ������ ���°��� �����Ѵ�.
  * @n B. Child �׸�鿡�� KSyncItem::SetServerStateByScanState() �޼ҵ带 ��������� ȣ���Ѵ�.
  */
  virtual void SetServerStateByScanState();

  /**
  * @brief ��� ��θ��� �����Ѵ�.
  * @param pathname : ��� ��θ�
  * @details A. KSyncItem::SetRelativePathName()�� ȣ���Ͽ� ��� ��θ��� �����Ѵ�.
  * @n B. mBaseFolder�� Ŭ�����ϰ� ���⿡ ��θ��� �����Ѵ�.
  */
  virtual void SetRelativePathName(LPCTSTR pathname);

  virtual void ClearRelativePathName(int flag);

  /**
  * @brief mBaseFolder�κ��� ��� ��θ��� ������Ʈ�Ѵ�.
  * @details A. KSyncItem::SetRelativePathName()�� ȣ���Ͽ� ��� ��θ��� mBaseFolder�� �����Ѵ�.
  */
  virtual void UpdateRelativePath();

  /**
  * @brief ��θ��� ������ path�� �����Ѵ�.
  * @details A. mBaseFolder�� Ŭ�����ϰ� ���⿡ path+���� ��ü��(mLocal.Filename)�� �����Ѵ�.
  */
  virtual void SetLocalPath(LPCTSTR baseFolder, LPCTSTR rootFolder, int depth);

  /**
  * @brief �� �׸��� ��θ��� �ٲ������ ȣ��Ǵ� �����Լ�.
  * @details A. AllocTreePathName()�� ȣ���Ͽ� �θ� ������κ��� ��θ��� �޾� mBaseFolder�� �Ѵ�.
  * @n B. mBaseFolder�� ����Ǿ����� KSyncRootFolderItem::UpdateMetaFolderNameMapping()�� ȣ���Ѵ�(2.0������ ��� �ȵ�)
  */
  virtual void OnPathChanged();

  /**
  * @brief �� �׸��� ��θ��� �ٲ������ ȣ��Ǵ� �����Լ�.
  * @param folder : ��� ��θ�
  * @param rootFolder : ����ȭ ��Ʈ ������ ��θ�
  * @param depth : ����ȭ ��Ʈ �����κ����� ���� Depth
  * @param prev : ���� ���
  * @param changed : �ٲ� ���
  * @param refreshNow : ���� ��θ� ������Ʈ�ϰ� DB�� �ٷ� ������Ʈ ��.
  * @details A. AllocTreePathName()�� ȣ���Ͽ� �θ� ������κ��� ��θ��� �޾� mBaseFolder�� �Ѵ�.
  * @n B. KSyncItem::OnPathChanged()�� ȣ���Ѵ�.
  */
  virtual void OnPathChanged(LPCTSTR folder, LPCTSTR rootFolder, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow);

  /**
  * @brief ������ ��� ��θ��� �޾ư���.(deprecated)
  * @param path :  ������ ��� ��θ��� ������ ���� �ּ�
  * @details A. mBaseFolder�� path�� �����Ѵ�.
  */
  //void GetLocalPath(LPTSTR path);

  /**
  * @brief ������ �̸��� �ٲ�ų� �̵��Ǿ�����, �� ������ �׸�鵵 ��� ��ΰ� ������Ʈ �Ǿ�� �ϸ�, �̶� �Ҹ��� �Լ��̴�.
  * @param baseFolder : ����ȭ ��Ʈ ������ ��θ�
  * @param depth : ����ȭ ��Ʈ �����κ����� ���� Depth
  * @param prev : ���� ���
  * @param changed : �ٲ� ���
  * @param refreshNow : ���� ��θ� ������Ʈ�ϰ� DB�� �ٷ� ������Ʈ ��.
  * @details A. ���� �׸�鿡 ���� KSyncItem::OnPathChanged()�� ��������� ȣ���Ѵ�.
  * @n B. ���� �׸��� �����̸� KSyncFolderItem::UpdateChildItemPath()�� ��������� ȣ���Ѵ�.
  * @n C. depth�� 1�̸�(���� �׸���� ������Ʈ�� ��� ��������),
  *     - KSyncRootStorage::UpdateRenameParentFolder()�� ȣ���Ͽ� �̸� ���� ���̺��� ��ΰ� �ٲ� ������ �����Ѵ�.
  *     - KSyncCoreEngine::ShellNotifyIconChangeOverIcon()�� ȣ���Ͽ� Ž���⿡�� ��� �׸��� �������� ������Ʈ �ǵ��� �Ѵ�.
  */
  void UpdateChildItemPath(KSyncRootFolderItem *root, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow);

  virtual int UpdateServerRenamedAs(LPCTSTR orgRelativePath, LPCTSTR changedRelativePath);

  // relative pathname, root is NULL
  /**
  * @brief ���� �׸��� ��� ��θ��� �����Ѵ�. ����ȭ ��Ʈ ���������� NULL�� �����Ѵ�.
  */
#ifdef USE_BASE_FOLDER
  inline TCHAR *GetBaseFolder() { return ((mParent == NULL) ? NULL : mBaseFolder); }
#else
  inline TCHAR *GetBaseFolder() { return ((mParent == NULL) ? NULL : mpRelativePath); }
#endif
  /**
  * @brief ���� �׸��� ��� ��θ��� �����Ѵ�. ����ȭ ��Ʈ ���������� "\"�� �����Ѵ�. ȭ�鿡 ǥ���ϱ� ���� ���ȴ�.
  */
  LPCTSTR GetBaseFolderName();

  /**
  * @brief ��� �׸��� ������ Child�� ����Ѵ�.
  */
  void AddChild(KSyncItem *item);

  /**
  * @brief ��� �׸��� ������ Child�� ����Ѵ�. after�� ������ �׸� ������ �߰��Ѵ�.
  */
  void AppendChild(KSyncItem *after, KSyncItem *item);

  /**
  * @brief Ư�� phase�� ó���� ���� �׸��� ã�� �����Ѵ�.
  * @details A. parentFirst�� TRUE �̸�,
  *     - �� �׸��� mPhase�� ������ phase���� �۰�, type�� ��ġ�ϸ�,
  *       + IsNeedSyncPhase()�� ȣ���Ͽ� �� �׸� �ش� phase�� ó���� ��ü�̸�
  *         + �� �׸��� �����ϰ�,
  *       + �ƴϸ� �׸��� mPhase�� phase�� �����Ѵ�.
  *     .
  * @n B. ���� ���°� ����ȭ ������ ���ų� ����ȭ ���� �����̸� NULL�� �����Ѵ�.(Child �׸��� ó������ �ʵ��� �ϱ� ����)
  * @n C. Child �׸�鿡 ���ؼ� �������
  *     - �׸��� �����̸�
  *       + �׸��� mPhase�� PHASE_END_OF_JOB���� ������(�׸��� ���ܷ� �������� �ʾ�����)
  *         + KSyncFolderItem::GetNextChild()�� ��������� ȣ���ϰ�,
  *         + ���ϰ��� NULL�� �ƴϸ� �̸� �����Ѵ�.
  *     - �׸��� ������ �ƴϸ�
  *       + �׸��� mPhase�� ������ phase���� �۰� type�� ��ġ�ϸ�,
  *       + IsNeedSyncPhase()�� ȣ���Ͽ� �� �׸� �ش� phase�� ó���� ��ü�̸�
  *         + �� �׸��� �����ϰ�,
  *       + �ƴϸ� �׸��� mPhase�� phase�� �����Ѵ�.
  *     .
  * @n D. parentFirst�� FALSE�̸�,
  *     - �� �׸��� mPhase�� ������ phase���� �۰�, type�� ��ġ�ϸ�,
  *       + IsNeedSyncPhase()�� ȣ���Ͽ� �� �׸� �ش� phase�� ó���� ��ü�̸�
  *         + �� �׸��� �����ϰ�,
  *       + �ƴϸ� �׸��� mPhase�� phase�� �����Ѵ�.
  *     .
  */
  KSyncItem *GetNextChild(int phase, int type, BOOL parentFirst = TRUE);

  // KSyncItem *GetNextPriorityChild(int phase, int type);

  /**
  * @brief ������ �׸��� �����͸� ã�� �����Ѵ�(�����ϴ��� Ȯ���Ѵ�).
  * @param item : ã���� �ϴ� �׸��� ������
  * @return KSyncItem* : ã�� �׸��� ������
  * @details A. �� �׸�� ã���� �ϴ� �׸��� ���Ͽ� ������ �� �׸��� �����Ѵ�.
  * @n B. Child �׸�鿡 ����
  *     - child �׸�� ã���� �ϴ� �׸��� ���Ͽ� ������ child �׸��� �����Ѵ�.
  *     - child �׸��� �����̸� �ش� �׸� ���� KSyncFolderItem::FindItemExist()�� ��������� ȣ���ϰ� ����� NULL�� �ƴϸ� �װ��� �����Ѵ�.
  */
  KSyncItem *FindItemExist(KSyncItem *item);

  /**
  * @brief ����ȭ �˻� �ܰ迡�� ��Ÿ ������ ��Ÿ ������ �˻��Ѵ�. 2.0������ ������� ����.
  * @param storage : ����ȭ ��Ʈ ���丮��
  * @param metaFolder : ��Ÿ ����
  * @param p : ����ȭ ��å
  * @param rootFolder : ����ȭ ��Ʈ ���� ���
  * @param done : �� ������ �˻� �Ϸ� ����
  * @param flag : �÷���
  * @param rootP : ����ȭ ��Ʈ ���� �׸�
  * @details A. flag�� SCAN_FOR_ALL_OBJECT���� ���� max_iteration���� �����Ѵ�(�⺻ 100).
  * @n B. mLocalScanHandle�� NULL�̸�(ó�� �˻��� ������ ���),
  *     - �� ������ ��ü ��θ� "*.ifo"�� ���Ͽ� �˻� ���ϸ��� ����� FindFirstFile()�� ���� �˻� �ڵ��� �����ϰ� �ڵ��� NULL�̸�,
  *       + done = TRUE�� �ϰ� ������� ����������.
  *     .
  * @n C. �ƴϸ� FindNextFile()�� ���� ������ �˻��ϰ� ����� �����ϸ�,
  *     - mLocalScanHandle�� �����ϰ�, done = TRUE�� �ϰ� ������� ����������.
  *     .
  * @n D. ã�� ���� �̸��� ��ȿ�ϰ� ".root.ifo"�� �ƴϸ�,
  *     - ��ȿ�� ��Ÿ ���Ϸ� �����ϰ� KSyncItem�� �����Ͽ� ��Ÿ�����͸� �о�´�.
  *     - ������ �׸��� child ��Ͽ� �߰��Ѵ�.
  *     .
  * @n E. ã�� �׸��� ������ �����Ѵ�.
  */
  int scanSubObjects(KSyncRootStorage* storage, LPCTSTR metaFolder, KSyncPolicy *p, LPCTSTR rootFolder, BOOL &done, int flag, KSyncFolderItem* rootP);

  /**
  * @brief ����ȭ �˻� �ܰ迡�� ������ �������� ���� �׸��� �˻��Ѵ�.
  * @param p : ����ȭ ��å
  * @param rootP : ����ȭ ��Ʈ ���� �׸�
  * @details A. KSyncCoreEngine::StartScanFolder()�� ȣ���Ͽ� ���� �׸� ���� �˻��� ��û�Ѵ�.
  */
  int scanSubFoldersServer(KSyncPolicy *p, KSyncRootFolderItem* rootP);

#ifndef SCAN_FOLDER_FILE_AT_ONCE
  /**
  * @brief ����ȭ �˻� �ܰ迡�� ������ �������� ���� �׸��� �˻��Ѵ�.
  * @param p : ����ȭ ��å
  * @param rootP : ����ȭ ��Ʈ ���� �׸�
  * @details A. ������ ��θ� + "*" �� ���� �˻� ����ũ�� �����.
  * @n B. FindFirstFile()�Լ��� ���� �˻��� �����Ѵ�.
  * @n C. �˻� �ڵ��� NULL�� �ƴϸ�,
  * @n D. �˻� ��� ���ϸ��� ��ȿ�ϸ�
  *     - findMovedRenamedOrigin()���� �̵� �� �̸� ����� ��ü������ Ȯ���Ѵ�.
  *     - ã�� ��ü�� ������ findChildByName()���� ���ϸ��� ������ ��ü�� Ȯ���Ѵ�.
  *     - ã�� ��ü�� ������ ��ü ��θ��� �����,
  *       + ��ü ��θ��� ����ȭ ��� ������ ���ԵǴ��� IsPartialSyncOn()���� Ȯ���Ͽ� ������
  *         + �� KSyncFolderItem ��ü�� �����ϰ�, child ��Ͽ� �߰��Ѵ�.
  *     - ã�� ��ü�� ������ �˻� ����� ������ �����ð� ������ �׼��� �ð��� mLocalNew�� ������Ʈ�ϰ� ���� ���¿� ITEM_STATE_NOW_EXIST�� ���Ѵ�.
  *     .
  * @n E. FindNextFile()�� ���� ������ �˻��ϰ� �����ϸ� D�� �̵��Ѵ�.
  * @n F. SetPhaseExcludedItems() �Լ��� ȣ���Ͽ� ���ܵǴ� �׸���� phase�� PHASE_MOVE_FOLDERS+1�� �����Ѵ�.
  */
  int scanSubFoldersLocal(KSyncPolicy *p, KSyncRootFolderItem* rootP);
#endif // SCAN_FOLDER_FILE_AT_ONCE

  /**
  * @brief ���� �׸� �߿��� ������ �ƴ� ���� �׸��� ��� ��Ͽ��� �����ϰ� �����Ѵ�.
  * @return int : ���ŵ� ������ �����Ѵ�.
  * @details A. ���� ��ü �׸� ����,
  * @n B. ���� �׸��� �����̸� �Ѿ��,
  * @n C. �����̸� ��Ͽ��� �����ϰ� �ش� �׸��� �����Ѵ�.
  * @n D. ���ŵ� ������ �����Ѵ�.
  */
  int ClearSubFiles();

  /**
  * @brief ���� �׸� �߿��� ����ȭ ���� ���� �׸��� phase�� ������ phase�� �����Ѵ�.
  * @param bRecurr : TRUE�̸� ���� ������ ���� ��� ȣ���� �Ѵ�.
  * @param phase : ������ phase��.
  * @details A. ���� ��ü �׸� ����,
  *     - KSyncItem::IsExcludedSync()�� ȣ���Ͽ� �� �׸��� ���ܵǾ�����,
  *       + ���� �׸��� KSyncItem::SetPhase()�� KSyncItem::mPhase�� �����Ѵ�.
  *     - ���ܵ��� �ʾҰ� bRecurr�� TRUE�̸�,
  *       + ���� �׸��� KSyncFolderItem::SetPhaseExcludedItems()�� ��������� ȣ���Ѵ�..
  *     .
  * @n B. ���� �׸� �߿��� ���ܵ� �׸��� ������ ����ȭ �÷��׿� SYNC_ITEM_HAS_EXCLUDED�� ���Ѵ�.
  */
  void SetPhaseExcludedItems(BOOL bRecurr, int phase);

  /**
  * @brief ���� �׸� �߿��� ����ȭ ���� ���� �׸��� ã�� �����ϰ� �����Ѵ�.
  * @n ����ȭ �˻� �ܰ迡�� ���Ե� ���� �׸���� ���̻� ó������ �ʵ��� ��Ͽ��� ���ŵǾ�� �� �� ȣ��ȴ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�.
  * @param bRecurr : TRUE�̸� ���� ������ ���� ��� ȣ���� �Ѵ�.
  * @details A. ���� ��ü �׸� ����,
  *     - KSyncItem::IsExcludedSync()�� ȣ���Ͽ� �� �׸��� ���ܵǾ�����,
  *       + ���� ��Ͽ��� �����ϰ� �׸��� �����Ѵ�.
  *     - ���ܵ��� �ʾҰ� bRecurr�� TRUE�̸�,
  *       + ���� �׸��� KSyncFolderItem::RemoveExcludedItems()�� ��������� ȣ���Ѵ�..
  *     .
  * @n B. ���� �׸� �߿��� ���ܵ� �׸��� ������ ����ȭ �÷��׿� SYNC_ITEM_HAS_EXCLUDED�� ���Ѵ�.
  */
  void RemoveExcludedItems(KSyncFolderItem* root, BOOL bRecurr);

  /**
  * @brief ���� ��Ͽ� ���Ե��� ���� ���� �׸��� �����Ѵ�.
  */
  void RemoveOutOfModifiedList(KVoidPtrArray& modifiedList);

  /**
  * @brief ���� �׸�鿡 ���� ����ȭ ��Ÿ �����͸� �����Ѵ�.
  * @n ����ȭ ���� ������ �����Ǿ����� ���� �׸� ��� �����ϱ� ���� ȣ��ȴ�.
  * @param baseFolder : ����ȭ ��Ʈ ����.
  * @details A. ���� ��ü �׸� ����,
  *     - ���� �׸��̸�,
  *       + ���� �׸��� KSyncFolderItem::RetireChildObjects()�� ��������� ȣ���Ѵ�..
  *     - ���ܵ��� �ʾҰ� bRecurr�� TRUE�̸�,
  *     .
  * @n B. ���� �׸� �߿��� ���ܵ� �׸��� ������ ����ȭ �÷��׿� SYNC_ITEM_HAS_EXCLUDED�� ���Ѵ�.
  */
  void RetireChildObjects(LPCTSTR baseFolder);

  void RetireChildObjectsRecurr(KSyncRootStorage* s, LPCTSTR baseFolder);

  /**
  * @brief ����ȭ ��å���� ����ȭ ���� ���Ŀ� �ش��ϴ� ���ϵ��� ����ȭ �۾����� ���ܰ� �ǵ��� phase�� �����Ѵ�.
  * @n ���� �� ������ ������ �˻��� �Ŀ� ����ȭ ���� ���Ŀ� �ش��ϴ� �׸���� ��󳻱� ���� ȣ��ȴ�.
  * @param engine : ����ȭ Engine.
  * @param root : ����ȭ ��Ʈ ����
  * @param restorePhase : ������ phase
  * @details A. ���� ��ü �׸� ����,
  *     - �׸��� �̸��� KSyncCoreEngine::IsExcludeFileMask() �Ǵ� KSyncPolicy::IsExcludeFileMask()�� �ش��ϸ� ex�� 1�� �ִ´�.
  *     - �׸��� ���� �̸� ����Ǿ� ������,
  *       + �̸� ����� �̸��� ���� KSyncCoreEngine::IsExcludeFileMask() �Ǵ� KSyncPolicy::IsExcludeFileMask()�� �ش��ϸ�,
  *         + ex�� 2�� ���Ѵ�.
  *       + �ƴϰ� (ex & 1)�̸�,(���� ���Ŀ� �ش�Ǿ����� �̸� �������� ���� ������ �ƴ� ���)
  *         + �̸� ����� ���� �̸��� UpdateRenameLocal()�� �̸��� �����ϰ�,
  *         + phase�� restorePhase�� �������´�.
  *     - �׸��� ���� �̸� ����Ǿ� ������,
  *       + �̸� ����� �̸��� ���� KSyncCoreEngine::IsExcludeFileMask() �Ǵ� KSyncPolicy::IsExcludeFileMask()�� �ش��ϸ�,
  *         + ex�� 4�� ���Ѵ�.
  *     - ex�� 0�� �ƴϸ�,
  *       + �α� �޽����� �ۼ��Ͽ� �����ϰ�
  *       + (ex & 2)�̸�
  *         + ������ �̸� ����� �̸��� UpdateRenameLocal()�� �̸��� �����ϰ� ��Ÿ ���丮���� �����Ѵ�
  *       + (ex & 4)�̸�
  *         + StoreMetafile()�� ��Ÿ ���丮���� �����Ѵ�
  *       + �׸��� phase�� PHASE_END_OF_JOB�� �����Ѵ�(����ȭ���� ���ܷ� �����Ѵ�)
  *     .
  * @n B. ���� �׸� �߿��� ���ܵ� �׸��� ������ ����ȭ �÷��׿� SYNC_ITEM_HAS_EXCLUDED�� ���Ѵ�.
  */
  void RemoveExcludeNamedItems(KSyncCoreBase *engine, KSyncRootFolderItem *root, int restorePhase);

  /**
  * @brief ����ȭ ��å���� ����ȭ ���� ���Ŀ� �ش��ϴ� �׸���� ����ȭ �۾����� ���ܰ� �ǵ��� Disable �����Ѵ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�.
  * @return int : Disable�� �׸� ������ �����Ѵ�.
  * @details A. �� ��ü�� ��Ʈ ������ �ƴϸ�,
  *     - ���� Enable�����̰�, KSyncRootFolderItem::IsExcludeFileMask()�� ���� ���� ����ũ�� �ش��ϸ�
  *       + Enable�� FALSE�� �ϰ� ī��Ʈ�� �ϳ� �����Ѵ�.
  *     .
  * @n B. ���� ��ü �׸� ����,
  *     - �׸��� �����̸�
  *       + ���� �׸� ���� KSyncFolderItem::DisableExcludedFileMask()�� ��������� ȣ���ϰ� ���ϰ��� ī��Ʈ�� ���Ѵ�.
  *     - �׸��� ������ �ƴϸ�
  *       + ���� Enable�����̰�, KSyncRootFolderItem::IsExcludeFileMask()�� ���� ���� ����ũ�� �ش��ϸ�
  *         + Enable�� FALSE�� �ϰ� ī��Ʈ�� �ϳ� �����Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */
  int DisableExcludedFileMask(KSyncRootFolderItem* root);

  /**
  * @brief �������� �������� �ʴ� ������ ������ phase�� �����Ѵ�.
  * @param phase : ������ phase.
  * @details A. ���� ��ü �׸� ����,
  *     - �׸��� �����̰� KSyncItem::IsServerNotExist()�� ������ �������� ������
  *       + �׸��� phase�� ������ phase�� �����ϰ�,
  *       + KSyncFolderItem::SetPhaseServerNotExistFolders()�� ��������� ȣ���Ѵ�.
  *     .
  */
  void SetPhaseServerNotExistFolders(int phase);

  /**
  * @brief ������ ��ġ�� �̵��� ���� �׸��� phase�� �ٲ۴�. ������ ����� �׸� �޾ƿ��� �κп��� ����Ѵ�.
  * @n OnEndScanFolderItem ���Ŀ� �����.
  * @n phase�� �ٲ� �׸��� �� ������ ��� ������Ʈ�Ǵ� recursive�� �ʿ� ����.
  * @param phase : �̵��� ������ ������ phase.
  * @param default_phase : �⺻ phase.
  * @details A. ���� ��ü �׸� ����,
  *     - �׸��� phase�� �⺻ phase�� �����Ѵ�.
  *     - �׸��� ������ ��쿡,
  *       + ������ fullPathIndex�� �ٲ����, ������ ���� �׸��� ������ 0���� ū ��쿡,
  *         + phase�� ������ phase�� �����Ѵ�.
  *       + �ƴϸ� KSyncFolderItem::SetPhaseForMovedFoldersInServer()�� ��������� ȣ���Ѵ�.
  *     .
  */
  void SetPhaseForMovedFoldersInServer(int phase, int default_phase);

  /**
  * @brief ������ ��ġ�� ���� ������ �ٱ�-������ �̵��� �׸��� ������ �����Ѵ�.
  */
  virtual int GetCountForMoveInOutOnServer(LPCTSTR partialPath, int path_len = -1);

#ifndef USE_SCAN_BY_HTTP_REQUEST
  /**
  * @brief ���������� ���� ���� ������ ���� �׸��� ��ȸ�Ͽ� KSyncItem���� ����� child ��ü�� �߰��Ѵ�.(2.0���� ��� ����)
  */
  int scanSubFilesServer(KSyncPolicy *p, KSyncFolderItem* rootP, BOOL &done);
#endif // USE_SCAN_BY_HTTP_REQUEST

  /**
  * @brief ���������� ��ĵ�� ���� �� ��ŵ�Ǿ�� �� �����̸� TRUE�� �����Ѵ�.
  */
  static BOOL IsLocalScanSkipFile(KSyncFolderItem* root, LPCTSTR cFileName, BOOL isNewItem);

  /**
  * @brief ���������� ���� ���� ������ �׸��� �˻��Ͽ� KSyncFolderItem �Ǵ� KSyncItem���� ����� child ��ü�� �߰��Ѵ�.
  * @param rootP : ����ȭ ��Ʈ ���� �׸�
  * @param folder_count : ã�� ���� ����
  * @param done : TRUE�̸� �˻� �Ϸ�
  * @return int : ã�� ���� ����
  */
  int scanSubFilesLocal(KSyncFolderItem* rootP, int& folder_count, BOOL &done);

  /**
  * @brief ������ ������ Ŭ�����ϰ� ������ �����鿡 ���� ��� ȣ���� �Ѵ�.
  * @details A. clearServerSideInfo()�� ȣ���Ͽ� ������ ������ Ŭ�����ϰ�
  * @n B. ���� ��ü �׸� ����,
  *     - �׸��� �����̸�
  *       + KSyncFolderItem::ClearServerSideInfoCascade()�� ��������� ȣ���Ѵ�.
  *     .
  */
  void ClearServerSideInfoCascade();

  /**
  * @brief �־��� ���ϸ� ���� �̵� �� �̸� ����� ���� ��ü�� ã�´�.
  * @param name : ���ϸ�
  * @return KSyncItem* : �� ��ü�� ������
  * @details A. GetActualPathName()�� ȣ���Ͽ� �� ������ ��θ� �޾ƿ´�.
  * @n B. ������ ���� ��ο� ������ ���ϸ��� ���δ�.
  * @n C. KSyncRootFolderItem::GetMovedRenamedOriginal()���� ���� ��ü�� �޾ƿ´�.
  * @n D. ������ ���� ��ü�� NULL�� �ƴϸ� KSyncItem::CheckLocalRenamedAs()�� ȣ���Ͽ� �̸� ������ Ȯ���Ѵ�.
  */
  KSyncItem *findMovedRenamedOrigin(LPCTSTR name);

  /**
  * @brief ��� �׸��� �־��� ����� ������ child ��ü�� �θ���踦 �����Ѵ�.
  * @param item : ��� �׸�
  * @param relativePathName : �̵��� ��� ��θ�
  * @param pathHasName : ��θ� �ڽ��� �������� ���ԵǾ� ����.
  * @return BOOL : �����ϸ� TRUE�� �����Ѵ�.
  * @details A. ��� �׸��� ������ �θ� ��ü�� �����͸� �ӽ� �����Ѵ�.
  * @n B. relativePathName �� ��ȿ�ϸ�
  *     - FindChildItemCascade()�� ȣ���Ͽ� relativePathName�� �ش��ϴ� ���� ��ü�� ã�ƿ´�.
  *     .
  * @n C. �ƴϸ� ��Ʈ ���� �׸��� �̵��� ������ �Ѵ�.
  * @n D. �̵��� ������ ��ü�� ��ȿ�ϸ�,
  *     - ���� �θ� ��ü���� RemoveItem()�� ȣ���Ͽ� ��� �׸��� �и��ϰ�,
  *     - �� �θ� ��ü�� AddChild()�� ȣ���Ͽ� child ��ü�� �߰��Ѵ�.
  *     - ��� �׸��� �����̸� �׸��� KSyncFolderItem::OnPathChanged()�� ȣ���Ͽ� ��ΰ� �ٲ���� ������Ʈ�Ѵ�.
  *     .
  */
  BOOL MoveSyncItem(KSyncItem *item, LPTSTR relativePathName, BOOL pathHasName);

  /*
  static BOOL CreateFileSyncMetaData(LPCTSTR filename, LPCTSTR base_path,
    LPCTSTR object_path, LPCTSTR server_id, SYSTEMTIME& datetime, __int64 size, BOOL isFile);
  int initializeFiles(LPCTSTR baseFolder, KSyncPolicy *p, int phase, BOOL withChild);
  int mergeOldNewInfo();
  */

  /**
  * @brief ���� ���� ���� ���ϵ��� ����ȭ ���Ͽ� ó�� ����� �����Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ��θ�
  * @param p : ����ȭ ��å
  * @param count : ó���� ����
  * @param phase : ����ȭ phase
  * @return BOOL : �񱳰� �Ϸ�Ǿ����� TRUE�� �����Ѵ�.(���� �׸��� ������ ���� ��� ���� ������ŭ�� ó���Ѵ�)
  * @details A. ���� �׸� ó�� ����(mChildItemIndex)�� 0�̰�, �� �׸��� ����ȭ ��Ʈ ������ �ƴϸ�,
  *     - ���� ���°� ���� �����ϸ�(ITEM_STATE_NOW_EXIST)
  *       + ������ ���ѿ��� ���� �׸� ��ȸ ������ ������,
  *         + ������ ���¿� ITEM_STATE_NO_PERMISSION�� ���Ѵ�.
  *         + ����� FILE_SYNC_NO_FOLDER_PERMISSION�� �����Ѵ�.
  *         + mChildItemIndex�� 1 �����ϰ� TRUE�� �����Ѵ�.
  *     .
  * @n B. ó�� ����(mChildItemIndex)�� 0�̸�,
  *     - compareSyncFileItem()�� ȣ���Ͽ� �� ��ü�� ������ �� ������ ���� �񱳸� ���Ѵ�.
  *     - removeEndOfJobObject()�� ȣ���Ͽ� ����ȭ ���ܵǴ� �׸��� child ��Ͽ��� �����Ѵ�.
  *     .
  * @n C. ó�� ����(mChildItemIndex)�� child �������� ũ�ų� ������ TRUE�� �����Ѵ�.
  * @n D. ��ü child �׸� ����
  *     - ���� �׸��� ������ �ƴϸ�,
  *       + ���� �׸��� mPhase�� ������ phase���� ������ ,
  *         + KSyncItem::compareSyncFileItem()�� ȣ���Ͽ� �� ó���� �ϰ�
  *         + KSyncItem::SetPhase()�� ȣ���Ͽ� ���� �׸��� mPhase�� phase�� �����Ѵ�.
  *         + count�� 1 �����Ѵ�.
  *     - mChildItemIndex�� 1 �����Ѵ�.
  *     - count�� ���� �������� ������ ���� ���� �׸��� ó���Ѵ�.
  *     - �ƴϸ� done = FALSE�� �ϰ� ����������.
  *     .
  * @n E. GetTotalSize()�� ���� �׸���� ũ�⸦ mLocal.FileSize�� �����Ѵ�.
  */
  int compareFiles(/*LPCTSTR baseFolder, KSyncPolicy *p*/KSyncRootFolderItem* root, int &count, int phase);

  /**
  * @brief child �׸񿡼� �׸��� ����ȭ �� ����� 0�̸� ��� �׸��� �����Ѵ�.
  * @param phase : ����ȭ phase
  * @return int : child �׸��� ������ �����Ѵ�.
  * @details A. �� �׸��� phase�� ������ phase�� �����Ѵ�.
  * @n B. ���� �����鿡 ���� KSyncFolderItem::RemoveUnchanged()�� ��������� ȣ���Ѵ�.
  * @n C. ���� ���ϵ鿡�� �׸��� ������� 0�̸� RemoveItem() �޼ҵ�� �ش� �׸��� �����ϰ� �����Ѵ�.
  * @n D. �ƴϸ� phase�� ������ phase�� �����Ѵ�.
  * @n E. ���� child�׸� ������ �����Ѵ�.
  */
  int RemoveUnchanged(int phase);

  /**
  * @brief child �׸񿡼� �׸��� �浹 ������� 0�̸� ��� �׸��� �����Ѵ�.
  * @return int : child �׸��� ������ �����Ѵ�.
  * @details A. �� �׸��� phase�� PHASE_CLEAN_UNCONFLICT�� �����Ѵ�.
  * @n B. ���� �����鿡 ���� KSyncFolderItem::RemoveUnconflictItem()�� ��������� ȣ���Ѵ�.
  * @n C. ���� �׸�鿡�� �浹 ������� 0�̸� RemoveItem() �޼ҵ�� �ش� �׸��� �����ϰ� �����Ѵ�.
  * @n D. �ƴϸ� phase�� ������ PHASE_CLEAN_UNCONFLICT�� �����Ѵ�.
  * @n E. ���� child�׸� ������ �����Ѵ�.
  */
  int RemoveUnconflictItem();

  /**
  * @brief child �׸񿡼� �׸��� phase�� PHASE_END_OF_JOB�̸� ��� �׸��� �����Ѵ�.
  * @param rootFolder : ����ȭ ��Ʈ ���� ��θ�
  * @details A. ���� ��ü �׸� ����,
  *     - �׸��� mPhase�� PHASE_END_OF_JOB�̸�
  *       + KSyncItem::IsExcludedSync()�� ȣ���Ͽ� �׸��� ���� ������ �ƴϸ�()
  *         + StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����͸� �����Ѵ�.
  */
  void removeEndOfJobObject();

  /**
  * @brief �׸��� mPhase�� ������ ������ �����ϴ� ���� �Լ�.
  * @param p : ������ phase
  * @details A. KSyncItem::SetPhase()�� ȣ���Ͽ� mPhase�� �����Ѵ�.
  * @n B. Child �׸� ó�� ����(mChildItemIndex)�� 0���� �����Ѵ�.
  */
  virtual void SetPhase(int p);

  /**
  * @brief ���� �׸��� �����Ͽ� �׸��� mPhase�� ������ ������ �����Ѵ�.
  * @param p : ������ phase
  * @param mask : EXCLUDE_CHANGED_ITEM : ó�� ����� �ִ� �׸��� �����Ѵ�.
  * @details A. SetPhase()�� ȣ���Ͽ� mPhase�� �����Ѵ�.
  * @n B. ���� �׸�鿡 ����,
  *     - ���� �׸��� �����̸� KSyncFolderItem::SetPhaseRecurse()�� ��������� ȣ���Ѵ�.
  *     - �ƴϸ� ���� �׸� ���� SetPhase()�� ȣ���Ѵ�.
  */
  void SetPhaseRecurse(int p, int mask);

  int SetPhaseRetryRecurse(int p, int mask);

  /**
  * @brief �� �׸�� ���� �׸��� mPhase�� ������ ������ �����Ѵ�.
  * @param p : ������ phase
  * @details A. SetPhase()�� ȣ���Ͽ� mPhase�� �����Ѵ�.
  * @n B. �θ� �׸��� �����ϸ� �θ� �׸� ����, KSyncFolderItem::SetPhaseRevRecurse()�� ��������� ȣ���Ѵ�.
  */
  void SetPhaseRevRecurse(int p);

  /**
  * @brief ���� �׸��� �����Ͽ� ������ �׸��� mPhase�� ������ ������ �����Ѵ�. KSyncItem::SetPhase()������ phase���� �������� �ʴ´�.
  * @n �� �Լ������� ���Ǿ��� ������ ������ phase ������ �����Ѵ�.
  * @param p : ������ phase
  * @details A. SetPhaseForce()�� ȣ���Ͽ� mPhase�� �����Ѵ�.
  * @n B. ���� �׸�鿡 ����,
  *     - ���� �׸��� �����̸� KSyncFolderItem::SetPhaseForceRecurse()�� ��������� ȣ���Ѵ�.
  *     - �ƴϸ� ���� �׸� ���� SetPhaseForce()�� ȣ���Ѵ�.
  */
  void SetPhaseForceRecurse(int p);
  
  /**
  * @brief ���� �׸��� �����Ͽ� ����ȭ ������� phase���� ������ ������ �����Ѵ�.
  * @param result : ������ �����.
  * @param phase : ������ phase��
  * @param flag : �÷���(SET_WITH_ITSELF, EXCLUDE_OUTGOING, SET_SERVER_DELETED, SET_LOCAL_DELETED)
  * @details A. �÷��׿� SET_WITH_ITSELF�� ������,
  *     - �� �׸��� ����ȭ ������� result�� �����ϰ�,
  *     - �� �׸��� phase���� �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡 ����
  *     - �÷��׿� EXCLUDE_OUTGOING�� ���ų�, IsOutGoingChild()�� FALSE�̸�,
  *       + �׸��� �����̸� KSyncFolderItem::SetConflictResultPhase()�� ��������� ȣ���Ѵ�.
  *       + �ƴϸ� �� �׸��� ����ȭ ������� phase���� ������ ������ �����Ѵ�.
  *       + �÷��׿� SET_SERVER_DELETED�� ������ clearServerSideInfo()�� ȣ���Ͽ� ������ ������ Ŭ�����Ѵ�.
  *       + �÷��׿� SET_LOCAL_DELETED�� ������ clearLocalSideInfo()�� ȣ���Ͽ� ������ ������ Ŭ�����Ѵ�.
  *     .
  */
  virtual void SetConflictResultRecurse(int result, int phase, int flag);

  /**
  * @brief ���� �׸��߿��� ��� ��� �׸��� �ƴ� �׸��� �����Ѵ�.
  * @details A. ���� �׸��� ���� �׸񿡼�, ��� ��ΰ� KSyncCoreBase::IsTargetDrive()�� �ش����� ������,
  * @n ��� �׸�� �� ���� �׸��� ����� FILE_SYNC_NOT_EXIST_DRIVE��, phase�� PHASE_END_OF_JOB�� �����Ѵ�.
  */
  int RemoveUnBackupedItems(KSyncRootFolderItem *root, int flag);

  /**
  * @brief ���� �׸� �߿��� ������ ���� OID�� ���� �׸��� ã�� �����Ѵ�.
  * @param serverOID : ������ ���� OID.
  * @return KSyncItem* : ������ ���� OID�� ���� �׸��� ������.
  * @details A. ���� �׸��� ���� OID�� ������ OID�� ���� ���Ͽ� ��ġ�ϸ� �� ��ü�� �����͸� �����Ѵ�.
  * @n B. ���� �׸�鿡 ����, �׸��� ���� OID�� ������ OID�� ���� ���Ͽ� ��ġ�ϸ� �� ��ü�� �����͸� �����Ѵ�.
  * @n C. ��ġ�ϴ� �׸��� ������ ����ȭ ��Ʈ ������������ �ٽ� ã�� ���� KSyncRootFolderItem::FindChildItemByOIDCascade()�� ȣ���Ͽ� ������� �����Ѵ�.
  */
  KSyncItem *findItemByOID(LPCTSTR serverOID);

  /**
  * @brief ���� �׸� �߿��� ������ �̸��� ���� �׸��� ã�� �����Ѵ�.
  * @param fileName : ã�� �̸�.
  * @return KSyncItem* : ������ �̸��� ���� �׸��� ������.
  * @details A. ���� �׸�鿡 ����, �׸��� �̸��� ������ �̸��� ���Ͽ� ��ġ�ϸ� �ش� ��ü�� �����͸� �����Ѵ�.
  */
  KSyncItem *findChildByName(LPCTSTR fileName);

  /**
  * @brief ������ ���� �׸��� child ��Ͽ� �����ϰų� �߰��Ѵ�.
  * @param item : ������ ���� �׸�.
  * @return BOOL : �����Ǿ����� TRUE�� �����Ѵ�.
  * @details A. findChildByName() �޼ҵ�� ������ �̸��� ������ �ִ� ���� �׸��� �����ϴ��� Ȯ���ϰ�,
  *     - �����ϸ�
  *       + �ش� �׸� KSyncItem::MergeItem()���� �����Ѵ�.
  *       + item�� �����ϰ� TRUE�� �����Ѵ�.
  *     - �������� ������,
  *       + AddChild() �޼ҵ�� Child �׸����� �߰��ϰ� FALSE�� �����Ѵ�.
  */
  BOOL MergeChild(KSyncItem *item);

  /**
  * @brief ���� �׸���� ��� ���� �׸� �Ʒ��� �̵��Ѵ�.
  * @param dest : �̵��� ��� ���� �׸�.
  * @return int : �̵��� �׸� ����.
  * @details A. ���� �׸�鿡 ����,
  *     - ���� ������ child ��Ͽ��� �����ϰ�
  *     - ��� ������ MergeChild()�޼ҵ带 ȣ���Ͽ� ���� �׸����� �߰��Ѵ�.
  *     .
  * @n B. child ����� ��� Ŭ�����Ѵ�.
  * @n C. ī��Ʈ ���� �����Ѵ�.
  */
  int MoveChildItems(KSyncFolderItem *dest);

  /**
  * @brief ���� �׸�鿡�� ��� �׸��� ã�� child ��Ͽ��� �����Ѵ�.
  * @param item : ã�� ��� �׸��� ������.
  * @return BOOL : ã������ TRUE�� �����Ѵ�.
  * @details A. ���� �׸�鿡 ����,
  *     - ã�� �׸�� ��ġ�ϸ� child ��Ͽ��� �����ϰ� TRUE�� �����Ѵ�.
  *     - �ƴϰ� ���� �׸��� �����̸�
  *       + KSyncFolderItem::RemoveItem()�� ��������� ȣ���ϰ� �� ���ϰ��� TRUE�̸� TRUE�� �����Ѵ�.
  *     .
  * @n B. FALSE�� �����Ѵ�.
  */
  BOOL RemoveItem(KSyncItem *item);

  /**
  * @brief ���� �׸�� ��ü�� �����Ѵ�.
  * @details A. ���� �׸�鿡 ����,
  *     - child ��Ͽ��� �����ϰ�
  *     - ���� �׸��� �����̸�
  *       + KSyncFolderItem::FreeChilds()�� ��������� ȣ���Ѵ�.
  *     - ���� �׸��� �����Ѵ�.
  */
  void FreeChilds();

  /**
  * @brief �� �׸��� ���� �����ϴ��� Ȯ���Ѵ�.
  * @param fullname : ������ ��ü ��θ�
  * @details A. IsDirectoryExist() �Լ��� ������ �����ϴ��� Ȯ���ϰ� ����� �����Ѵ�.
  */
  virtual BOOL IsItemExist(LPCTSTR fullname);

  /**
  * @brief �� �׸��� ����ȭ �� �۾��� �����Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ��θ�
  * @details A. ����ȭ ��Ʈ �����̸� R_SUCCESS�� �����Ѵ�.
  * @n B. checkAddedRenamedSyncError()�� ȣ���Ͽ�, ���� �߰��ǰų� �̵��� �׸��� ó���Ҷ�, ������ �̸��� ������ �����ϴ��� �˻��ϰ� �����ϸ� �浹 ���°� �ǵ��� �����Ѵ�.
  * @n C. ���� ���°� ITEM_STATE_NOW_EXIST�̸� ���� ���°��� ITEM_STATE_NEWLY_ADDED�� ���Ѵ�.
  * @n D. ���� ���°� ITEM_STATE_NOW_EXIST�̸� ���� ���°��� ITEM_STATE_NEWLY_ADDED�� ���Ѵ�.
  * @n E. ���� ���°� ITEM_STATE_WAS_EXIST�̸� ���� ���°��� ITEM_STATE_DELETED�� ���Ѵ�.
  * @n F. ���� ���°� ITEM_STATE_WAS_EXIST�̸�,
  *     - ������ ���¸� ������Ʈ�Ͽ� ������ 0�̸� ������� FILE_SYNC_NO_FOLDER_PERMISSION�� �ϰ�,
  *     - �ƴϸ� ���� ���°��� ITEM_STATE_DELETED�� ���Ѵ�.
  *     .
  * @n G. ���� ���°� ���� �߰����̸�,
  *     - StoreMetafile()�� ȣ���Ͽ� ��Ÿ ������ �����Ѵ�.
  *     - �����̰� KSyncRootFolderItem::IsSyncRootFolder()�� ȣ���Ͽ� �ٸ� ����ȭ ��Ʈ�����̸�,
  *       + ����� FILE_RELEASE_PREV_SYNC_ROOT�� �Ѵ�.
  *     - �ƴϰ� �������� �������� ������
  *       + ����� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED�� �Ѵ�.
  *     .
  * @n H. ���� ���°� �������̸�,
  *     - ������ �̸� ���� ������ ������ �����Ѵ�.
  *     .
  * @n I. ���� ���°� �������̸�,
  *     - ������ �̸� ���� ������ ������ �����Ѵ�.
  *     .
  * @n J. �������� �̸� ���� �Ǵ� �̵��Ǿ�����,
  *     - ���� ���°��� ITEM_STATE_MOVED�� ���Ѵ�.
  *     .
  * @n K. �������� �̸� ���� �Ǵ� �̵��Ǿ�����,
  *     - ���� ���°��� ITEM_STATE_MOVED�� ���Ѵ�.
  *     .
  * @n L. ������� 0�̸�
  *     - makeResultFromState()�� ȣ���Ͽ� ������ ������ ���°��� ���� ������� �����.
  *     - ������� �浹�̳� ������ �ƴϸ�,
  *       + ������� FILE_NEED_RETIRED�� ������,
  *         + ��� ��θ��� ����� �� ���̰� MAX_FOLDER_PATH_LENGTH���� ũ��
  *           + ������ ������ ������Ʈ�ϰ�
  *           + ������� FILE_SYNC_OVER_PATH_LENGTH�� �Ѵ�.
  *     - ������� ���ε��̸�,
  *       + ���� ���°� ITEM_STATE_NOW_EXIST�̸� ������� 0���� �Ѵ�.
  *     - �浹 ������� FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED�̸�, �浹�� �ƴѰ����� ó���ϵ��� �浹���� �����.
  *     .
  * @n M. ������� ���� �α׸� �����ϰ� SetResult�� ������� �����Ѵ�.
  * @n N. ��������� �浹 �� �������� �浹 ��������� mConflictResult�� �����Ѵ�.
  * @n O. StoreMetafile()�� �����, �浹 ������� �����Ѵ�.
  */
  virtual int compareSyncFileItem(LPCTSTR baseFolder, int syncMethod);

  /**
  * @brief �� �׸� �� ���� �׸��� ���¿� ���� phase���� �����Ѵ�.
  * @return void
  * @param added_phase : ���� �߰��� �׸��̸� �� phase���� �����Ѵ�.
  * @param renamed_phase : �̸� ����/�̵��� �׸��̸� �� phase���� �����Ѵ�.
  * @param normal_phase : ���� �׸��̸� �� phase���� �����Ѵ�.
  * @param parentCheckNeed : Child �׸��� ����Ǿ����� Parent �׸� ���߾� �����Ѵ�.
  * @details A. ���̽� �Լ� KSyncItem::SetUnchangedItemPhase()�� ȣ���Ѵ�.
  * @n B. �� ��ü�� mPhase�� renamed_phase�̰�, ���� ���°� ITEM_STATE_LOCATION_CHANGED�̸�, 
  * StoreMetafile()�� ȣ���Ͽ� ���ϸ�� ������ fullPathIndex�� �����Ѵ�.
  * @n C. ���� �׸�鿡 ����,
  *     - KSyncItem::SetUnchangedItemPhase()�� ��������� ȣ���Ѵ�.
  *     .
  * @n D. parentCheckNeed�� TRUE�̸�, 
  *     - hasChangedChild()�� TRUE�̸� KSyncItem::SetUnchangedItemPhase(added_phase-1, renamed_phase-1, normal_phase-1, FALSE)�� ȣ���Ͽ�
  * �� ��ü�� �ٽ� üũ�ǵ��� �Ѵ�.
  */
  virtual void SetUnchangedItemPhase(int added_phase, int renamed_phase, int normal_phase, BOOL parentCheckNeed);


  void SetCascadedItemFlag(LPCTSTR pathName, int flag);
  int CheckMoveTargetFolders(KSyncFolderItem* rootItem, int syncMethod);
  int RemoveUnreferencedFolders(KSyncRootFolderItem *root, int syncMethod);

  /**
  * @brief ����ȭ �浹 ó���������� �� �׸� �� ���� �׸��� ���¿� ���� phase���� �����Ѵ�.
  * @return BOOL : Enable�Ǿ� ������ TRUE�� �����Ѵ�.
  * @param parent_phase : ���� �׸� �� phase���� �����Ѵ�.
  * @param renamed_phase : �̸� ����� �׸� �� phase���� �����Ѵ�.
  * @param normal_phase : ��Ÿ �ٸ� �׸� �� phase���� �����Ѵ�.
  * @details A. ���� �׸�鿡 ����,
  *     - KSyncItem::SetPhaseOnResolve()�� ��������� ȣ���ϰ�, TRUE�� ���ϵǸ� enable�� TRUE�� �Ѵ�.
  *     .
  * @n B. �� ��ü�� Enable �����̸� normal_phase�� �����Ѵ�.
  * @n C. �ƴϰ� 
  *     - ���� �׸��� enable�̸� parent_phase�� �����ϰ�,
  *     - �ƴϸ� PHASE_END_OF_JOB�� �����Ѵ�.
  *     .
  * @n D. mEnable�̳� enable�� TRUE�̸� TRUE�� �����Ѵ�.
  */
  virtual BOOL SetPhaseOnResolve(int parent_phase, int renamed_phase, int normal_phase);

  /**
  * @brief ���� �׸��߿��� ������� 0�� �ƴ� �׸��� ������ TRUE�� �����Ѵ�. ��� ȣ���� ���� �ʴ´�.
  * @return BOOL : ������ ���� �׸��� ������ TRUE�� �����Ѵ�.
  * @details A. ���� �׸�鿡 ����,
  *     - ������� FILE_NOT_CHANGED(0)�� �ƴϸ� TRUE�� �����Ѵ�.
  */
  BOOL hasChangedChild();

  /**
  * @brief ���� �׸��߿��� ������� 0�� �׸��� ������ �����Ѵ�. ��� ȣ���� �Ͽ� ��ü ���� �׸��� �˻��Ѵ�.
  * @param type : ��� �׸��� ����(1:����, 0:����, 2:���)
  * @return int : �������� ���� ���� �׸��� ����.
  * @details A. �� ��ü�� ������� FILE_NOT_CHANGED�̰�, Ÿ���� ��ġ�ϸ� ī��Ʈ�� 1 �����Ѵ�.
  * @n B. ���� �׸�鿡 ����,
  *     - �����̸� KSyncFolderItem::GetUnchangedItemCount()�� ��� ȣ���ϰ� ���ϰ��� ī��Ʈ�� ���Ѵ�.
  *     - ������ �ƴϸ�, ��ü�� ������� FILE_NOT_CHANGED�̰�, Ÿ���� ��ġ�ϸ� ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */
  int GetUnchangedItemCount(int type);

  /**
  * @brief ������ ������ �����Ǿ����� �����Ѵ�
  * @param baseFolder : ����ȭ ��Ʈ ���� ��θ�
  * @return BOOL : �����ϸ� TRUE�� �����Ѵ�.
  * @details A. �θ� ������ NULL�̸�(����ȭ ��Ʈ ����), TRUE�� �����Ѵ�.
  * @n B. �θ� ������ ���� CreateServerFolderIfDeleted()�� ȣ���Ͽ� ���� ���� ������ ó���ǵ��� �Ѵ�.
  * @n C. ������ pullPathIndex�� NULL�̸� createServerFolder()�� ȣ���Ͽ� ������ �����ϰ�,
  *     - �����ϸ� ���� ���¸� ITEM_STATE_EXISTING�� �ϰ�, StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����͸� �����Ѵ�.
  */
  BOOL CreateServerFolderIfDeleted(LPCTSTR baseFolder);

  /**
  * @brief �浹 ó��â���� ����ȭ�� ������ ��쿡 ȣ��Ǹ�, �浹 ó�� �غ� �Ѵ�
  * @param rootItem : ����ȭ ��Ʈ ���� �׸�
  * @param phase : ����ȭ �۾� �ܰ�
  * @return int : �����ϸ� R_SUCCESS�� �����ϰ�, �ƴϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. ������� FILE_NOT_CHANGED�̸�
  *     - RefreshServerNewState()�� ȣ���Ͽ� ������ ������ ������Ʈ�ϰ�,
  *     - IsFolderFullPathChanged()�� ȣ���Ͽ� ������ ��ü ��ΰ� �ٲ������,
  *       + ���� ���¿� ITEM_STATE_LOCATION_CHANGED�� ���Ѵ�.
  *     - ���� ���°� ITEM_STATE_MOVED�̰ų� ���� ���°� ITEM_STATE_MOVED�̸�
  *       + ���� ���¿� ITEM_STATE_LOCATION_CHANGED�� ���Ѵ�.
  *       + R_FAIL_NEED_FULL_SYNC_PARTIAL�� �����Ѵ�(��ü ����ȭ�� �ʿ��� ����)
  *     - R_SUCCESS�� �����Ѵ�.
  *     .
  * @n B. �浹 ����� ������ ��쿡,
  *     - ���� ���°� �������̰�,
  *       + �浹 ����� FILE_NEED_UPLOAD�̸�
  *         + child_user_action�� FILE_NEED_UPLOAD�� �ϰ�,
  *       + �ƴϰ� �浹 ����� FILE_NEED_UPLOAD�̸�
  *         + child_user_action�� FILE_NEED_LOCAL_DELETE�� �Ѵ�.
  *     - ���� ���°� �������̰�,
  *       + �浹 ����� FILE_NEED_DOWNLOAD�̸�
  *         + child_user_action�� FILE_NEED_DOWNLOAD�� �ϰ�,
  *       + �ƴϰ� �浹 ����� FILE_NEED_UPLOAD�̸�
  *         + child_user_action�� FILE_NEED_SERVER_DELETE�� �Ѵ�.
  *     .
  * @n C. �浹 ����� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED�̰ų�
  * FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED�� ��쿡, �浹 ����� ���� ������ child_user_action���� �Ѵ�.
  * @n D. ���̽� Ŭ������ KSyncItem::ReadyResolve()�� ȣ���Ѵ�.
  * @n E. child_user_action�� 0�� �ƴϰ� mEnable�� TRUE�̸�,
  *     - �浹 ����� FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED�̸� flag�� SET_SERVER_DELETED�� �ϰ�,
  *     - �浹 ����� FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED�̸� flag�� SET_LOCAL_DELETED�� �Ѵ�.
  *     - SetConflictResultRecurse()�� ȣ���Ͽ� ���� �׸񿡵� ��� child_user_action�� ����ǵ��� �Ѵ�.
  */
  virtual int ReadyResolve(KSyncItem *rootItem, int phase, int syncMethod);

  /**
  * @brief ����ȭ �۾��� �����Ѵ�
  * @param p : ����ȭ ��å
  * @param baseFolder : ����ȭ ��Ʈ ���� ��θ�
  * @param phase : ����ȭ �۾� �ܰ�
  * @details A. SetPhase()�� phase���� �����Ѵ�.
  * @n B. ��� ��θ��� �غ��Ѵ�.
  * @n C. phase�� PHASE_CONFLICT_FILES���� ������
  *     - ������� �浹�̳� �����̸�
  *       + �̸� r�� �����ϰ�, cancel_child_objects�� TRUE�� �ϰ�, �۾��� �������Ѵ�.
  *     .
  * @n D. phase�� PHASE_CONFLICT_FILES�̸�
  *     - mEnable�� FALSE�̸� �α׸� �����ϰ�, R_SUCCESS�� �����Ѵ�.
  *     - �浹 ������� d�� �����ϰ�,
  *     - d�� �ߵ� �Ǵ� �����̰�, d�� ����� ���� ������ 0�̸�
  *       + d�� �浹 �� �������� r�� �����ϰ�, cancel_child_objects�� TRUE�� �ϰ�, �۾��� �������Ѵ�.
  *     .
  * @n E. d�� FILE_NEED_LOCAL_DELETE�̰�, phase�� PHASE_CONFLICT_FILES�ƴϰų� d�� FILE_NEED_DOWNLOAD�� ������,
  *     - IsLocalDeleteOk()�� �����̸�,
  *       + d�� 0���� �ϰ�, r�� FILE_SYNC_DELETE_FOLDER_FAIL_CHILD_MODIFIED�� �ϰ�,
  *       + cancel_child_objects�� TRUE�� �ϰ�, �۾��� �������Ѵ�.
  *     .
  * @n F. d������ (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE) �÷��׸� �����.
  * @n G. d���� FILE_SAME_MOVED �÷��װ� ������,
  *     - ������ ��θ��� �ӽ� �����ϰ�,
  *     - updateMetaAsRenamed()�� ȣ���Ͽ� ��θ��� ������Ʈ�ϰ�,
  *     - UpdateChildItemPath()�� ȣ���Ͽ� ��ΰ� �ٲ���� ���� �׸�鿡�Ե� ������Ʈ�Ѵ�.
  *     .
  * @n H. �ƴϸ�,
  *     - ���� ���¿� ITEM_STATE_MOVED�� �ְ� ���� ���¿� ITEM_STATE_MOVED�� ������,
  *       + d�� FILE_NEED_UPLOAD�� ������,
  *         + d�� FILE_NEED_SERVER_MOVE�� ���Ѵ�.
  *       + �ƴϰ� d�� FILE_NEED_DOWNLOAD�� ������,
  *         + d�� FILE_NEED_LOCAL_MOVE�� ���Ѵ�.
  *     - �ƴϰ� ���� ���¿� ITEM_STATE_MOVED�� ������,
  *       + d�� FILE_NEED_SERVER_MOVE�� ���Ѵ�.
  *     - �ƴϰ� ���� ���¿� ITEM_STATE_MOVED�� ������,
  *       + d�� FILE_NEED_LOCAL_MOVE�� ���Ѵ�.
  *     .
  * @n I. d���� (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE)�� ������,
  *     - d���� FILE_NEED_SERVER_MOVE�� ������,
  *       + ���� ���°� ������(ITEM_STATE_DELETED)�̸�,
  *         + renameServerObjectAs()�� ȣ���ϰ� ����� ������ �ƴϸ� r�� ����� �����Ѵ�.
  *         + ���� ���¿��� ITEM_STATE_DELETED�� �����ϰ�,
  *         + r�� d���� (FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)�� �����Ѵ�.
  *       + �ƴϸ�,
  *         + ����ȭ ��å���� ���ε尡 ������ �����̸�
  *           + renameServerObjectAs()�� ȣ���Ͽ� ������ �̵��� �ϰ� �����ϸ� d������ (FILE_NEED_UPLOAD|FILE_CONFLICT)�� �����Ѵ�.
  *         + �ƴϸ�,
  *           + isNeedMoveServerObjectAs()�� TRUE�̸� r�� FILE_SYNC_NO_MOVE_PERMISSION�� �ְ�,
  * FALSE�̸� FILE_SYNC_NO_RENAME_PERMISSION�� �ִ´�.
  *     - d���� FILE_NEED_LOCAL_MOVE�� ������,
  *       + ���� ���°� ������(ITEM_STATE_DELETED)�̸�,
  *         + renameLocalObjectAs(FALSE)�� ȣ���ϰ�,
  *         + ���� ���¿��� ITEM_STATE_DELETED�� �����ϰ�, 
  *         + r�� d���� (FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)�� �����Ѵ�.
  *       + �ƴϸ�,
  *         + ����ȭ ��å���� �ٿ�ε尡 ������ �����̸�
  *           + renameLocalObjectAs(TRUE)�� ȣ���Ͽ� ������ �̵��� ���ϰ�, �����ϸ� d������ (FILE_NEED_DOWNLOAD|FILE_CONFLICT)�� �����Ѵ�.
  *         + �ƴϸ� r���� FILE_SYNC_NO_LOCAL_PERMISSION�� �Ѵ�.
  *     - r���� R_SUCCESS�� �ƴϸ� �������ϰ� ���� ������.
  *     .
  * @n J. phase�� PHASE_MOVE_FOLDERS�̸�
  *     - ���� ���¿� ���� ���¿��� ITEM_STATE_MOVED�� �����Ѵ�.
  *     - d������ (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE)�� �����ϰ�, �������ϰ� ���� ������.
  *     .
  * @n K. d���� FILE_NEED_UPLOAD�� �ְ� �θ� NULL�� �ƴϸ�,
  *     - ��ü ��θ� �غ��ϰ�,
  *     - ����� ������ �������� ������, 
  *       + r���� FILE_SYNC_NO_LOCAL_FOLDER_FOUND�� �ϰ�, �������ϰ� ���� ������.
  *     - ����ȭ ��å�� ���ε� ������ ������,
  *       + CheckPermissionFolderUpload()�� ȣ���Ͽ� �ش� ������ ���ε��� ������ �ִ��� �˻��ϰ�, ������ ������
  *         + createServerFolder()�� ���� ������ �����ϰ�, 
  *           + ������ �߻��Ǹ� r���� FILE_SYNC_CREATE_SERVER_FOLDER_FAIL�� �ϰ�
  *           + �ƴϸ� ���� ���¿� ���� ���¸� ITEM_STATE_EXISTING�� �ϰ�, ���� ���¸� ������Ʈ�ϰ�, �����丮�� ������ �����Ѵ�..
  *       + ������ ������ r���� FILE_SYNC_NO_CREATE_FOLDER_PERMISSION�� �Ѵ�.
  *     - r���� R_SUCCESS�� �ƴϸ� �������ϰ� ���� ������.
  *     .
  * @n L. �ƴϰ� d���� FILE_NEED_DOWNLOAD�� ������,
  *     - ����ȭ ��å�� �ٿ�ε� ������ ������,
  *       + CheckPermissionFolderDownload()�� ȣ���Ͽ� �ش� ������ �ٿ�ε��� ������ �ִ��� �˻��ϰ�, ������ ������
  *         + ���� ���°� �������̸�, 
  *           + r���� FILE_SYNC_NO_SERVER_FOLDER_FOUND�� �ϰ�
  *         + �ƴϸ�
  *           + createLocalFolder()�� ���� ������ �����ϰ� ����� r�� �����Ѵ�.
  *         + r�� �����̸�
  *           + RefreshServerState()�� ���� ������ ������Ʈ�ϰ�
  *           + d������ (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE)�� �����ϰ�,
  *           + ���� ���¿� ���� ���¸� ITEM_STATE_EXISTING�� �ϰ�
  *           + ���� ������ ������Ʈ�ϰ�,
  *           + �����丮�� ������ �����Ѵ�.
  *           + �ƴϸ� ���� ���¿� ���� ���¸� ITEM_STATE_EXISTING�� �ϰ�, ���� ���¸� ������Ʈ �Ѵ�.
  *       + ������ ������ r���� FILE_SYNC_NO_LOCAL_WRITE_PERMISSION�� �Ѵ�.
  *     - r���� R_SUCCESS�� �ƴϸ� �������ϰ� ���� ������.
  *     .
  * @n M. d���� (FILE_NEED_RETIRED | FILE_NEED_LOCAL_DELETE | FILE_NEED_SERVER_DELETE)�� ������, delete_meta�� TRUE�� �Ѵ�.
  * @n N. d���� FILE_NEED_LOCAL_DELETE�� ������,
  *     - ���� ���°� �������̰ų� IsLocalFileExist()�� FALSE�̸�
  *       + changed�� TRUE�� �Ѵ�.
  *     - �ƴϸ�,
  *       + ����ȭ ��å�� �ٿ�ε� ������ ������,
  *         + deleteLocalFolder()�� ȣ���Ͽ� ������ ������ ����(�����뿡 �̵�)�ϰ�, �����ϸ�
  *           + deleteLocalChildsRecursive()�� ȣ���Ͽ� ���� ���� �׸� �����ϰ�,
  *           + changed�� TRUE�� �ϰ�,
  *           + �����丮�� ������ �����Ѵ�.
  *       + ������ r���� FILE_SYNC_NO_DELETE_LOCAL_FOLDER_PERMISSION�� �Ѵ�.
  *     .
  * @n O. d���� FILE_NEED_SERVER_DELETE�� ������,
  *     - ���� ���°� �������̰ų� IsServerFileExist()�� FALSE�̸�
  *       + changed�� TRUE�� �Ѵ�.
  *     - �ƴϸ�,
  *       + ����ȭ ��å�� ���ε� ������ ������,
  *         + CheckPermissionFolderDelete()�� ���� ���� ������ ������
  *           + deleteServerFolder()�� ���� ������ �����ϰ� �����ϸ�
  *             + r���� FILE_SYNC_DELETE_SERVER_FOLDER_FAIL�� �ְ�
  *           + �����ϸ� 
  *             + deleteLocalChildsRecursive()�� ȣ���Ͽ� ���� �׸� ��� �����ϰ�,
  *             + changed�� TRUE�� �ϰ�, 
  *             + �����丮�� ������ �����Ѵ�.
  *       + ������ ������ r���� FILE_SYNC_NO_DELETE_SERVER_FOLDER_PERMISSION�� �Ѵ�.
  *     .
  * @n P. d���� FILE_NEED_RETIRED�� ������, deleteMeta�� TRUE�� �ϰ�, d������ FILE_NEED_RETIRED�� �����Ѵ�.
  * @n Q. r������ ����� ���� ������ �����ϰ� r���� R_SUCCESS�̸� changed�� TRUE�� �Ѵ�.
  * @n R. r���� ��� �� �浹 ��������� ��Ʈ�Ѵ�.
  * @n S. r�� 0�̸�
  *     - mChildResult�� 0���� �ϰ�,
  *     - mLocalNew�� �ð� ������ mLocal��, mServerNew�� �ð��� mServer�� �����Ѵ�.
  *     - mServerNew�� Ŭ�����Ѵ�.
  *     .
  * @n T. r�� 0�� �ƴϸ�
  *     - �����丮�� �۾� ���� ������ �����Ѵ�.
  *     .
  * @n U. StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����͸� �����ϰų� �����Ѵ�.
  * @n V. deleteMeta�� TRUE�̸�, RetireChildObjects()�� ȣ���Ͽ� ���� �׸� ��� �����Ѵ�.
  * @n W. deleteMeta�� FALSE�̸� ShellNotifyIconChangeOverIcon()�� ȣ���Ͽ� Ž���⿡�� �������� ������Ʈ�Ѵ�.
  * @n X. cancel_child_objects�� TRUE�̸� SetPhaseRecurse(PHASE_CONFLICT_FILES)�� ȣ���Ͽ� ���� �׸���� ó������ �ʰ� �浹 ���°� �����ǵ��� �Ѵ�.
  */ 
  virtual int doSyncItem(KSyncPolicy *p, LPCTSTR baseFolder, int phase);


  virtual BOOL OnDeleteServerItem(BOOL r, LPCTSTR pError);

  /**
  * @brief ������ �̵� �� �̸����� �������� �θ� ������ �ٲ������ Ȯ���Ѵ�.
  * @return BOOL : ������ �θ� ������ �ٲ������(�̸� ������ �ƴ� �̵��̸�) TRUE�� �����Ѵ�.
  * @details A. ������ �̵� �� �̸����� �������� ��ο� �̸��� �и��Ͽ� ��θ� changedLocation�� �־�д�.
  * @n B. GetFolderInfo()�� �� ������ �θ� ������ OID�� �޾ƿ���,
  * @n C. KSyncRootFolderItem::GetFolderOIDCascade()�� ���� changedLocation�� �ش��ϴ� ������ OID�� �޾Ƽ�
  * @n D. �� ���� OID�� �ٸ��� TRUE�� �����Ѵ�.
  */
  BOOL isNeedMoveServerObjectAs();

  /**
  * @brief ������ �̵� �� �̸����� ������ ���� �������� �̵� �� �̸������� ���ϰ� ������ ������Ʈ�Ѵ�.
  * @param renameObject : ���� �̵� �� �̸������� ���� ������ ����.
  * @return int : �����ϸ� R_SUCCESS �����ϰ� �����ϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. renameObject�� TRUE�̸� 
  *     - ������ �̵� �� �̸����� �������� ��ο� �̸��� �и��Ͽ� ���� changedLocation, changedName�� �־�д�.
  *     - KSyncRootFolderItem::GetFolderOIDCascade()�� ���� changedLocation�� �ش��ϴ� ������ OID�� �޾ƿ´�.
  *     - �θ� ������ �ٲ������
  *       + �� �θ� ���� ������ �ٲ� �̸��� ������ �ٸ� ������ �����ϸ�,
  *         + ���� �̸��� ���� ������ �̸��� �ٲ��� �ʴ� ���̸� FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS ������ ó���Ͽ� �����Ѵ�.
  *         + ReadyUniqueChildName()���� �ӽ� �̸��� changedName�� �غ��Ѵ�.
  *         + KSyncCoreEngine::ChangeFolderName()���� �̸��� �ٲٰ� �����ϸ�,
  *           + mpLocalRenamedAs�� �ӽ� �̸����� �ٲٰ�, ���� �̸��� SetServerTemporaryRenamed()�� ȣ���Ͽ� �����Ͽ� �д�.
  *         + �����ϸ� FILE_SYNC_NO_MOVE_PERMISSION�� �����Ѵ�.
  *       + CheckPermissionFolderMove()�� ȣ���Ͽ� �̵��� ������ �ִ��� Ȯ���ϰ�,
  *       + ������ ������ KSyncCoreEngine::MoveItemTo()�� ȣ���Ͽ� �̵��� �ϰ� �����ϸ�,
  *         + RefreshServerState()�� ȣ���Ͽ� ������ ������ ������Ʈ�ϰ�, mServerNew ������ Ŭ�����Ѵ�.
  *       + ������ ������ FILE_SYNC_NO_MOVE_PERMISSION�� �����Ѵ�.
  *     - ������ �̸��� �ٲ������
  *       + CheckPermissionFolderRename()�� ȣ���Ͽ� ������ �ִ��� Ȯ���ϰ� ������ ������,
  *         + �θ� ������ �ٲ� �̸��� ������ �ٸ� ������ �����ϸ�,
  *           + ���� �̸��� ���� ������ �̸��� �ٲ��� �ʴ� ���̸� FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS ������ ó���Ͽ� �����Ѵ�.
  *           + ReadyUniqueChildName()���� �ӽ� �̸��� changedName�� �غ��Ѵ�.
  *           + KSyncCoreEngine::ChangeFolderName()���� �̸��� �ٲٰ� �����ϸ�,
  *             + �̸��� �ӽ� �̸����� �ٲٰ�, ���� �̸��� SetServerTemporaryRenamed()�� ȣ���Ͽ� �����Ͽ� �д�.
  *       + ������ ������ FILE_SYNC_NO_RENAME_PERMISSION�� �����Ѵ�.
  *     .
  * @n B. renameObject�� FALSE�̸� 
  *     - ������ ������ �����Ѵ�.
  *     .
  * @n C. ���� �۾����� �̵��� �����Ͽ����� KSyncRootFolderItem::MoveSyncItem()�� ȣ���Ͽ� �� �׸��� �� �θ� ���� ������ �̵���Ų��.
  * @n D. ���� �۾��� �����Ͽ�����,
  *     - MoveMetafileAsRename()�� ȣ���Ͽ� ��θ� ������Ʈ�ϰ�,
  *     - KSyncRootFolderItem::RemoveMovedRenamedOriginal()�� ȣ���Ͽ� �̸� ���� ���̺��� �� ������ ���� �׸��� �����Ѵ�.
  *     - ���� �� ������ �̵� ������ Ŭ�����ϰ�,
  *     - ���� �� ���� ���¿��� ITEM_STATE_MOVED�� �����ϰ�,
  *     - renameObject�� TRUE�̸�, �����丮�� �۾� ������ �����ϰ�, ������ ���¸� ������Ʈ�Ѵ�.
  *     - SetRelativePathName()���� mBaseFolder�� ������Ʈ�ϰ�,
  *     - StoreMetafile()�� ��Ÿ �����͸� �����Ѵ�.
  *     - �ӽ� �̸����� �ٲ� �����̸� UpdateChildItemPath()���� ���� �׸���� ��ε� ������Ʈ�Ѵ�.
  *     .
  */
  int renameServerObjectAs(BOOL renameObject);

  /**
  * @brief ������ �̵� �� �̸����� ������ ���� ���ÿ��� �̵� �� �̸������� ���ϰ� ������ ������Ʈ�Ѵ�.
  * @param renameObject : ���� �̵� �� �̸������� ���� ������ ����.
  * @return int : �����ϸ� R_SUCCESS �����ϰ� �����ϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. �������� ���� ��ü ��θ� �غ��ϰ�,
  *     - ������ �̵� �� �̸����� ������ ��ȿ�ϸ�,
  *       + ���� �θ�ü�� �� �θ� ��ü�� �ٸ���,
  *         + �� �θ� ������ ��θ� �޾Ƽ� mpServerRenamedAs�� ������Ʈ�Ѵ�.
  *       + ��Ʈ ������ ��ο� mpServerRenamedAs�� ���Ͽ� ��ü ��θ��� �����.
  *     - �ƴϸ� GetSyncResultName()���� ������ ��ü ��θ��� �޾ƿ´�.
  *     .
  * @n B. ���� ��θ�� ���� ���� ��θ��� �ٸ���,
  *     - renameObject�� TRUE�̸�,
  *       + �� ����� ������ �����ϸ�
  *         + KSyncFolderItem::FindChildItemCascade()�� �� ��ο� �ش��ϴ� ���� ��ü�� ã��, 
  * �� ������ ������ �̸��� �����ϸ� FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS�� �����Ѵ�.
  *         + ReadyUniqueLocalName()���� �ӽ� �̸����� �� ��θ� �ٲ۴�.
  *       + KMoveFile() ���� ������ �̵��ϰ�, �ӽ� �̸����� �ٲ���̸� ���� �̸��� SetLocalTemporaryRenamed()�� �����صд�.
  *     - �ƴϸ� ������ ������ �Ѵ�.
  *     - ���� �۾��� �����Ͽ�����,
  *       + KSyncRootFolderItem::MoveSyncItem()���� �� ������ ������ �� ��ü�� �̵��Ѵ�.
  *       + MoveMetafileAsRename()���� ��� ��θ��� �����ϰ� ��Ÿ �����͸� �����Ѵ�.
  *       + SetRelativePathName()���� mBaseFolder�� ������Ʈ�Ѵ�.
  *       + �̸� ���� ������ Ŭ�����ϰ� ���� �� ���� ���¿��� ITEM_STATE_MOVED�� �����Ѵ�.
  *       + UpdateServerNewInfo()�� ȣ���Ͽ� ������ ������ �����Ѵ�.
  *       + StoreMetafile()���� ��Ÿ �����͸� �����Ѵ�.
  *       + �ӽ� �̸����� �ٲ��� �ʾ����� UpdateChildItemPath()�� ȣ���Ͽ� ���� �׸���� ��ε� �����Ѵ�.
  * �ӽ� �̸����� �ٲ� ��쿡�� ���� �̸��� �ٲٴ� �۾��� ���� �Ŀ� UpdateChildItemPath()�� ȣ���ϰ� �ȴ�.
  *       + �۾� ������ �����丮�� �����Ѵ�.
  *     .
  * @n C. ���� ��θ�� ���� ���� ��θ��� ������, R_SUCCESS�� �����Ѵ�.
  * @n D. ���� �ڵ忡 ���� �浹 ������ �غ��ϰ� �����丮�� �����Ѵ�.
  */
  int renameLocalObjectAs(BOOL renameObject);

  /**
  * @brief ���� �׸��� ������ �̸� ���� ������ ������ ��� ��θ�� �����ϸ� TRUE�� �����Ѵ�.
  * @param relativeName : ��� ��θ�.
  * @details A. ���� �׸�鿡 ����
  *     - �׸��� ������ �̸� ���� ������ ������ �ְ� �̰��� ������ relativeName�� ��ġ�ϸ� TRUE�� �����Ѵ�.
  *     - �׸��� �����̸� KSyncFolderItem::IsExistServerRenamed()�� ��������� ȣ���ϰ� ���ϰ��� TRUE�̸� TRUE�� �����Ѵ�.
  *     .
  */
  BOOL IsExistServerRenamed(LPCTSTR relativeName);

  /**
  * @brief ������ ���� �׸��� �� ���� �ٱ����� �̵��Ǿ����� TRUE�� �����Ѵ�.
  * @param c : ���� �׸� ������.
  * @details A. ���� �׸��� ������ �̵� �� �̸������ �����̸�,
  *     - ������ ��� ��θ��� ���ϰ�, �̰��� ���� ������ ���� ����(IsChildFolderOrFile())�� �ƴϸ� TRUE�� �����Ѵ�.
  * @n B. ���� �׸��� ������ �̵� �� �̸������ �����̸�,
  *     - ������ ��� ��θ��� ���ϰ�, �̰��� ���� ������ ���� ����(IsChildFolderOrFile())�� �ƴϸ� TRUE�� �����Ѵ�.
  */
  BOOL IsOutGoingChild(KSyncItem *c);

  /**
  * @brief �� ��ü�� ���� ������ ������ �����Ѵ�.
  * @param parentItem : �θ� ���� ��ü
  * @param pErrorMsg : ���� �޽����� ������ ����.
  * @return BOOL : �����ϸ� TRUE�� �����Ѵ�.
  * @details A. �θ� ������ ���� OID�� NULL�̸� FALSE�� �����Ѵ�.
  * @n B. KSyncCoreEngine::SyncServerCreateFolder()�� �������� ������ �����ϰ� �����ϸ�,
  *     - ���� �����ÿ� ���� mServerNew���� permission�̳� fullPathIndex�� NULL�̸�, KSyncCoreEngine::GetServerFolderSyncInfo()��
  * ������ ������ ���� �޾ƿ´�.(���� ���������� �����Ŀ� ���������� permission�̳� fullPathIndex������ �������� �ʾ���)
  *     - mServerNew�� ���� ������ mServer�� �����ϰ�, �α׿� �����ϰ� TRUE�� �����Ѵ�.
  */
  BOOL createServerFolder(KSyncItem *parentItem, LPTSTR* pErrorMsg);

  /**
  * @brief �� ��ü�� ���� ������ ������ �����Ѵ�.
  * @details A. ������ ��ü ��θ��� �غ��ϰ�,
  * @n B. ������ �������� ������
  *     - CreateDirectory()�� ������ �����ϰ� �����ϸ�
  *       + ������ �ð� ������ ������Ʈ�ϰ�, mServerNew�� mServer�� �����Ѵ�.
  *     - �����ϸ� �α׸� �����ϰ�, GetLastError()�� 5�̸� FILE_SYNC_NO_LOCAL_PERMISSION�� �����ϰ�,
  * �ƴϸ� FILE_SYNC_CREATE_LOCAL_FOLDER_FAIL�� �����Ѵ�.
  */
  int createLocalFolder();

  /**
  * @brief �������� ���� �׸���� ��� �����Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ����
  * @param flag : �÷���
  * @details A. ���� �׸�鿡 ����
  *     - d�� R_SUCCESS�� �ʱ�ȭ�Ѵ�.
  *     - �÷��׿� EXCLUDE_OUTGOING�� ������,
  *       + IsOutGoingChild()(�� ���� �ٱ����� �̵��Ǵ� �׸�)�̸� d = 1�� �Ѵ�.
  *     - d�� R_SUCCESS�̸�
  *       + ���� �׸��� �����̸� KSyncFolderItem::deleteLocalChildsRecursive()�� ��������� ȣ���Ѵ�.
  *       + �ƴϸ�
  *         + �÷��׿� DELETE_LOCAL_FILES�� ������,
  *           + ���� �׸��� ��ü ��θ� �غ��ϰ�
  *           + KSyncCoreEngine::SyncLocalDeleteFile()�� ������ �����Ѵ�.
  *         + d�� R_SUCCESS�̸�, StoreMetafile()�� ��Ÿ �����͸� �����Ѵ�.
  *       + d�� R_SUCCESS�̸�, child ��Ͽ��� �� ���� �׸��� �����ϰ�, �����Ѵ�.
  *       + d�� R_SUCCESS�� �ƴϸ�
  *         + ���� �׸��� phase�� PHASE_END_OF_JOB�� �ϰ�,
  *         + ���ϰ��� R_FAIL_DELETE_CHILDS�� �Ѵ�.
  *     .
  * @n B. ���ϰ��� R_SUCCESS�̸�
  *     - �÷��׿� DELETE_LOCAL_FILES�� ������
  *       + ��ü ��θ� �غ��ϰ�, KSyncCoreEngine::SyncLocalDeleteFolder()�� �� ������ �����Ѵ�.
  *       + StoreMetafile()�� ��Ÿ �����͸� �����Ѵ�.
  *     .
  */
  int deleteLocalChildsRecursive(LPCTSTR baseFolder, int flag);

  /**
  * @brief �������� ������ �����Ѵ�.
  * @param baseFolder : ����ȭ ��Ʈ ����
  * @return int : �����ϸ� R_SUCCESS�� �����Ѵ�.
  * @details A. �������� ����ȭ ��� ��ü ��θ��� �غ��ϰ�
  * @n B. KSyncCoreEngine::SyncLocalDeleteFolder()�� �� ������ �����Ѵ�.
  * @n C. �����ϸ� �α� ������ �����ϰ� FILE_SYNC_CHILD_DELETE_FAIL�� �����Ѵ�.
  */
  int deleteLocalFolder(LPCTSTR baseFolder);

  /**
  * @brief �������� ������ �����Ѵ�.
  * @return int : �����ϸ� R_SUCCESS�� �����Ѵ�.
  * @details A. KSyncCoreEngine::SyncServerDeleteFolder()�� ������ �� ������ �����Ѵ�.
  * @n B. �����ϸ� FILE_SYNC_DELETE_SERVER_FOLDER_FAIL�� �����Ѵ�.
  */
  int deleteServerFolder();

  /**
  * @brief �������� ������ �����Ѵ�.
  * @return BOOL : �����ϸ� TRUE�� �����Ѵ�.
  * @details A. ������ OID�� ��ȿ�ϸ�,
  *     - ������ mServer�� Ŭ�����Ѵ�.
  *     - KSyncCoreEngine::GetServerFolderSyncInfo()���� ������ ������ mServer�� �޾ƿ´�.
  */
  virtual BOOL RefreshServerState();

  /**
  * @brief �������� �� ������ �����Ѵ�.
  * @return int : �����ϸ� R_SUCCESS��, �ƴϸ� ITEM_STATE_DELETED�� �����Ѵ�.
  * @details A. ������ OID�� ��ȿ�ϸ�,
  *     - ������ mServerNew�� Ŭ�����Ѵ�.
  *     - KSyncCoreEngine::GetServerFolderSyncInfo()���� ������ ������ mServerNew�� �޾ƿ´�.
  */
  virtual int RefreshServerNewState();

  /**
  * @brief ����ȭ �浹 ó��â���� �������� �̸��� �����Ͽ��� �� ���� ��ü�� �����Ǹ�, ���⼭ ������ ������ �߰��۾��� ó���Ѵ�.
  * ������ �����Ǿ� ���� �߰��Ǿ����� ���� �׸�鵵 �����Ǿ� ���� �߰��Ǿ�� �Ѵ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�.
  * @param pair : ���� ��ü.
  * @details A. KSyncItem::CloneLocalRename()�� ȣ���Ͽ� �� ��ü�� ���� ��ü�� ���¿� ������ �����Ѵ�.
  * @n B. ���� �׸�鿡 ����,
  *     - KSyncItem::SetLocalPath()���� ���� �׸��� ��θ� ������Ʈ �Ѵ�.
  *     - pair�� NULL�� �ƴϸ�
  *       + KSyncItem::DuplicateItem()���� ���� �׸��� ���� �׸��� �����ϰ�,
  *       + KSyncItem::SetLocalPath()���� ������ ���� �׸��� ��θ� ������Ʈ �ϰ�,
  *       + �̸� pair�� child ��ü�� ����Ѵ�
  *     - KSyncItem::CloneLocalRename()�� ��������� ȣ���Ͽ� ������ ������ �����ǵ��� �Ѵ�.
  *     .
  */
  virtual void CloneLocalRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief ����ȭ �浹 ó��â���� �������� �̸��� �����Ͽ��� �� ���� ��ü�� �����Ǹ�, ���⼭ ������ ������ �߰��۾��� ó���Ѵ�.
  * ������ �����Ǿ� ���� �߰��Ǿ����� ���� �׸�鵵 �����Ǿ� ���� �߰��Ǿ�� �Ѵ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�.
  * @param pair : ���� ��ü.
  * @details A. KSyncItem::CloneServerRename()�� ȣ���Ͽ� �� ��ü�� ���� ��ü�� ���¿� ������ �����Ѵ�.
  * @n B. ���� �׸�鿡 ����,
  *     - KSyncItem::SetLocalPath()���� ���� �׸��� ��θ� ������Ʈ �Ѵ�.
  *     - pair�� NULL�� �ƴϸ�
  *       + KSyncItem::DuplicateItem()���� ���� �׸��� ���� �׸��� �����ϰ�,
  *       + KSyncItem::SetLocalPath()���� ������ ���� �׸��� ��θ� ������Ʈ �ϰ�,
  *       + �̸� pair�� child ��ü�� ����Ѵ�
  *     - KSyncItem::CloneServerRename()�� ��������� ȣ���Ͽ� ������ ������ �����ǵ��� �Ѵ�.
  *     .
  */
  virtual void CloneServerRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief ����ȭ �浹 ó��â���� �� ��ü�� ����ϱ⸦ �Ͽ��� �� ���� ��ü�� �����Ǹ�, ���⼭ ������ ������ �߰��۾��� ó���Ѵ�.
  * ������ �����Ǿ� ���� �߰��Ǿ����� ���� �׸�鵵 �����Ǿ� ���� �߰��Ǿ�� �Ѵ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�.
  * @param pair : ���� ��ü.
  * @details A. KSyncItem::CloneSeparate()�� ȣ���Ͽ� �� ��ü�� ���� ��ü�� ���¿� ������ �����Ѵ�.
  * @n B. ���� �׸�鿡 ����,
  *     - KSyncItem::SetLocalPath()���� ���� �׸��� ��θ� ������Ʈ �Ѵ�.
  *     - pair�� NULL�� �ƴϸ�
  *       + KSyncItem::DuplicateItem()���� ���� �׸��� ���� �׸��� �����ϰ�,
  *       + KSyncItem::SetLocalPath()���� ������ ���� �׸��� ��θ� ������Ʈ �ϰ�,
  *       + �̸� pair�� child ��ü�� ����Ѵ�
  *     - KSyncItem::CloneSeparate()�� ��������� ȣ���Ͽ� ������ ������ �����ǵ��� �Ѵ�.
  *     .
  */
  virtual void CloneSeparate(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief ������ ��θ��� ��� ���ۿ� �����Ѵ�.
  * @param pathname : ��θ��� ������ ��� ����, NULL�̸� �������� �ʰ� ���̸� �����Ѵ�.
  * @param length : ��θ��� ������ ��� ���� ����
  * @return int : �����ϸ� ��� ���� ���̸� �����Ѵ�.
  * @details A. �θ� ��ü�� NULL�̸�(����ȭ ��Ʈ ����)
  *     - mBaseFolder�� ������ ���� KMAX_PATH���� ũ�� FALSE�� �����Ѵ�.
  *     - ��� ���ۿ� mBaseFolder�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ�
  *     - �� ��ü�� ������ �̵� �� �̸����� ������ ������ ������
  *       + ����ȭ ��Ʈ ���� ��ο� ������ �̵� �� �̸����� ����(mpLocalRenamedAs)�� ���Ͽ� ��θ��� �����.
  *     - �ƴϸ�
  *       + �θ� ��ü�� KSyncFolderItem::GetCurrentFolder()�� ȣ���Ͽ� ��θ��� �޾ƿͼ� ���⿡ �� ��ü�� �̸��� ���Ͽ� ��θ��� �����.
  *     .
  */
  virtual int GetCurrentFolder(LPTSTR pathname, int length);

  /**
  * @brief �� �׸�� ���� �׸񿡼� ����ȭ �浹 �Ǵ� ���� �׸��� ������ �����Ѵ�.
  * @param includeDisabled : disable�� �׸� ī��Ʈ�� �������� ����.
  * @return int : ����ȭ �浹 �Ǵ� ���� �׸��� ����
  * @details A. includeDisabled�� TRUE�̰ų�, mPhase�� PHASE_END_OF_JOB���� ������
  *     - �� �׸��� �浹 ������� �浹 �Ǵ� �����̸� ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡 ����
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::GetConflictCount()�� ���ȣ���Ͽ� ���ϵ� ���� ī��Ʈ�� ���Ѵ�.
  *     - �ƴϸ�,
  *       + includeDisabled�� TRUE�̰ų�, mPhase�� PHASE_END_OF_JOB���� ������,
  *         + �� �׸��� �浹 ������� �浹 �Ǵ� �����̸� ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */
  int GetConflictCount(BOOL includeDisabled);

  void GetConflictCount(int& folderCount, int& fileCount, BOOL includeDisabled);

  /**
  * @brief ���� �׸��� ��ü ������ �����Ѵ�.
  * @param type : ��� ���� �׸��� ����
  * @details A. ���� �׸�鿡 ����
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::GetTotalCount()�� ���ȣ���Ͽ� ���ϵ� ���� ī��Ʈ�� ���Ѵ�.
  *     - �׸��� ������ ��ġ�ϸ� ī��Ʈ�� 1 �����Ѵ�.
  * @n B. ī��Ʈ���� �����Ѵ�.
  */
  int GetTotalCount(int type);

  /**
  * @brief ���� �׸��� ��ü ũ�⸦ �����Ѵ�.
  * @param conflictResult : �浹 �׸� �������� ����
  * @return __int64 : ���� �׸��� ��ü ũ��
  * @details A. ���� �׸�鿡 ����
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::GetTotalSize()�� ���ȣ���Ͽ� ���ϵ� ���� ũ�Ⱚ�� ���Ѵ�.
  *     - �ƴϸ� GetSyncFileSize()�� �׸��� ũ�Ⱚ�� ���Ѵ�.
  * @n B. ũ�Ⱚ�� �����Ѵ�.
  */
  __int64 GetTotalSize(BOOL conflictResult);

  /**
  * @brief ����ȭ �����߿� �۾� �Ϸ�� ��ü ũ�⸦ �����Ѵ�.
  * @param conflictResult : �浹 �׸� �������� ����
  * @return __int64 : ����ȭ �Ϸ�� ũ��
  * @details A. ���� �׸�鿡 ����
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::GetTotalSizeProgress()�� ���ȣ���Ͽ� ���ϵ� ���� ũ�Ⱚ�� ���Ѵ�.
  *       + ������ ����ȭ �۾��� ���α׷����� �ݿ��ǵ��� EMPTY_FILE_ESTIMATE_SIZE ũ�⸦ ���Ѵ�.
  *     - �ƴϸ�
  *       + GetSyncFileSize()�� �׸��� ũ�Ⱚ�� ���Ѵ�.
  *       + �� ������ ����ȭ �۾��� ���α׷����� �ݿ��ǵ��� EMPTY_FILE_ESTIMATE_SIZE ũ�⸦ ���Ѵ�.
  * @n B. ũ�Ⱚ�� �����Ѵ�.
  */
  __int64 GetTotalSizeProgress(BOOL conflictResult);

  /**
  * @brief ����ȭ �����߿� ó�� �Ϸ�� �׸��� ����, ���� ũ��, �Ϸ�� ũ�⸦ �����Ѵ�. �̴� ����ȭ ����â�� ������¸� ǥ���ϱ� ���� ���ȴ�.
  * @param phase : �۾� �ܰ�
  * @param conflictResult : �浹 �׸� �������� ����
  * @param count : ������ ������ ����
  * @param sizePure : ���� ũ�⸦ ������ ����
  * @param sizeProgress : ����� ũ�⸦ ������ ����
  * @details A. �� ��ü�� mPhase�� ������ phase���� ������
  *     - ī��Ʈ�� 1 �����ϰ�, sizeProgress�� EMPTY_FILE_ESTIMATE_SIZE��ŭ �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡 ����
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::GetCandidateCount()�� ���ȣ���Ͽ� ���� ������ ���� ���� ���Ѵ�.
  *     - �ƴϸ�
  *       + ���� �׸��� mPhase�� ������ phase���� ������
  *         + GetSyncFileSize()�� ũ�Ⱚ�� �޾Ƽ� sizePure, sizeProgress�� ���Ѵ�.
  *         + �� ������ ����ȭ �۾��� ���α׷����� �ݿ��ǵ��� sizeProgress�� EMPTY_FILE_ESTIMATE_SIZE ũ�⸦ ���Ѵ�.
  *     .
  */
  void GetCandidateCount(int phase, BOOL conflictResult, int &count, __int64 &sizePure, __int64 &sizeProgress);

  // int GetResultItemCount(int resultMask);

  /**
  * @brief ����ȭ �����߿� ������ ������ �����ص� �Ǵ��� Ȯ���Ѵ�. ���� �׸� �߿� �����ǰų� �������� ������ ������ �������� �ʴ´�.
  * @param onLocal : ������ �Ǵ� ������ Ȯ��
  * @return TRUE : ���� �����ϸ� TRUE�� �����Ѵ�.
  * @details A. ���� �׸�鿡 ����
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::IsLocalDeleteOk()�� ���ȣ���Ͽ� ���� ������ ���� �˻縦 ���ϰ� ���ϰ��� FALSE�̸� FALSE�� �����Ѵ�.
  *     - �ƴϸ�
  *       + ���� �׸��� ���� ���¿� (ITEM_STATE_MOVED|ITEM_STATE_UPDATED)���� ������, FALSE�� �����Ѵ�.
  *       + ���� �׸��� �÷��׿� SYNC_ITEM_EDITING_NOW���� ������, �� ������ �ܺ� ���α׷��� ���� ���� �����Ƿ�, FALSE�� �����Ѵ�.
  *     .
  */
  BOOL IsChildDeleteOk(BOOL onLocal);

  /**
  * @brief ����ȭ ��Ʈ ���� �׸��� ã�� �����Ѵ�.
  * @return KSyncFolderItem* : ����ȭ ��Ʈ ���� �׸�.
  * @details A. �� �׸��� mResolveFlag�� HIDDEN_ROOT_FOLDER���� ������(����ȭ �浹ó��â���� �̵��� ��ü ������ ǥ���ϱ�
  * ���� �ΰ��� ��Ʈ ������ ���������, �� ���� �����ϰ� �ִ� ������ ��Ʈ ����),
  *     - ���� �׸��߿��� 
  *       + �׸��� �����̰�, IsDummyCloneFolder()�� FALSE�̸� �� ��ü�� ���� ����ȭ ��Ʈ �����̴� �̸� �����Ѵ�.
  *     .
  * @n B. mParent�� NULL�̸� �̰��� ��Ʈ �����̴� �̸� �����Ѵ�.
  * @n C. mParent�� GetRootFolderItem()�� ȣ���ϰ� �� ����� �����Ѵ�.
  */
  KSyncFolderItem *GetRootFolderItem();

  /**
  * @brief ����ȭ ��Ʈ ������ ������ ��θ��� �����Ѵ�.
  * @return LPCTSTR : ����ȭ ��Ʈ ������ ������ ��θ�.
  * @details A. GetRootFolderItem()�� ȣ���Ͽ� ��Ʈ ���� �׸��� ã�´�.
  * @n B. ��Ʈ ���� �׸��� ã������ �̰��� mBaseFolder�� �����Ѵ�.
  * @n C. ��Ʈ ������ ������ �� ��ü�� mBaseFolder�� �����Ѵ�.
  */
  virtual LPCTSTR GetRootPath();

  /**
  * @brief ������ ��� ��θ� �ش��ϴ� �׸��� ã�� �����Ѵ�.
  * @param relativePathName : ã�� �׸��� ��� ��θ�.
  * @param matchType : �׸��� ����
  * @param withRenamed : �̸� ����� ������ �����Ͽ� ã�´�.
  * @return KSyncItem* : ã�� ��ü�� �����͸� �����Ѵ�.
  * @details A. relativePathName�� �������,
  *     - �� �׸��� ������ matchType�� ��ġ�ϸ� �� ��ü�� �����Ѵ�.
  *     - �ƴϸ� NULL�� �����Ѵ�.
  *     .
  * @n B. relativePathName���� �ֻ����� �̸��� ������ ��θ��� �и��Ѵ�.
  * @n C. FindChildItem()���� ������ �׸��� ã�´�.
  * @n D. ���� �׸��� ã�Ұ�, �����̰� ������ ��θ��� ��ȿ�ϸ�
  *     - ���� �׸��� KSyncFolderItem::FindChildItemCascade()�� ��� ȣ���ϰ� ���ϵ� ���� ���� �׸����� �Ѵ�.
  *     .
  * @n E. ���� �׸��� �����Ѵ�.
  */
  KSyncItem *FindChildItemCascade(LPCTSTR relativePathName, int matchType, int flag = WITH_RENAMED);

  /**
  * @brief ������ ��θ����� �̸� ����� ��ü�� ã�� �����Ѵ�.
  * @param relativePathName : ã�� ��� ��θ�.
  * @param onLocal : TRUE�̸� ������ �̸����濡�� ã�´�.
  * @param matchType : �׸��� ����
  * @return KSyncItem* : ã�� ��ü�� �����͸� �����Ѵ�.
  * @details A. ������ �׸�� �߿��� 
  *     - matchType�� 2�̰ų� ���� �׸��� ������ ������ ������ �����ϸ�,
  *       + onLocal�� TRUE�̰�, ���� �׸��� mpLocalRenamedAs�� ã�� ��� ��θ�� ��ġ�ϸ�,
  * �� ���� �׸��� �����Ѵ�.
  *       + onLocal�� FALSE�̰�, ���� �׸��� mpServerRenamedAs�� ã�� ��� ��θ�� ��ġ�ϸ�,
  * �� ���� �׸��� �����Ѵ�.
  *     - �� �׸��� �����̸�, KSyncFolderItem::FindRenamedChildItem()�� ��������� ȣ���ϰ� ���ϰ��� NULL�� �ƴϸ� ���ϰ��� �����Ѵ�.
  *     .
  * @n B. NULL�� �����Ѵ�.
  */
  KSyncItem *FindRenamedChildItem(LPCTSTR relativePathName, BOOL onLocal, int matchType);

  /**
  * @brief ������ ��θ� ���� ����ȭ ��ü�� �����Ѵ�.
  * @param relativePath : ������ ��ü�� ��� ��θ�.
  * @param matchType : ��ü�� ����
  * @root : ��Ʈ ���� �׸�
  * @flag : REFRESH_LOCALNEW : ������ ��ü�� LocalNew ������ ������Ʈ�Ѵ�
  * @created_count : ������ ��ü ������ ����
  * @return KSyncItem* : ������ ��ü�� �����͸� �����Ѵ�.
  * @details A. relativePathName���� �ֻ����� �̸��� ������ ��θ��� �и��Ѵ�.
  * @n B. ������ ��θ��� ��ȿ�ϰų� matchType�� 0���� ũ��,
  *     - KSyncFolderItem�� �����ϰ�,
  *       + ������ ��θ��� ��ȿ�ϸ�, KSyncFolderItem::CreateChildItemCascade()�� ��� ȣ���Ѵ�.
  *     .
  * @n C. �ƴϸ�
  *     - KSyncItem�� �����Ѵ�.
  */
  KSyncItem *CreateChildItemCascade(LPCTSTR relativePath, int matchType, KSyncRootFolderItem* root, int flag, int *created_count);

  /**
  * @brief ����ȭ ���ܸ� ���� �Ǵ� �����ϰ� ��Ÿ �����Ϳ� �����Ѵ�.
  * @param s : ��Ÿ ������ ���丮��
  * @param exclude : TRUE�̸� ����ȭ ���� ����, �ƴϸ� ����.
  * @param recursive : 0���� ũ�� ���� �׸� ��� ��������� ó���Ѵ�.
  * @details A. SetExcludedSync()�� ȣ���Ͽ� ����ȭ ���ܸ� ���� �Ǵ� �����Ѵ�.
  * @n B. recursive�� 0���� ũ��,
  *     - ���� �׸�鿡 ����,
  *       + �����̸� KSyncFolderItem::StoreExcludedSyncCascade()�� ��� ȣ���Ѵ�.
  *     .
  */
  void StoreExcludedSyncCascade(KSyncRootStorage *s, BOOL exclude, int recursive);

  /**
  * @brief ������ �̸��� ���� ���� �׸��� ã�� �����Ѵ�.
  * @param name : ã�� ��ü �̸�
  * @param withRename : TRUE�̸� �̸� ����� ������� ���Ѵ�.
  * @return KSyncItem* : ã�� �׸�.
  * @details A. ���� �׸�鿡 ����,
  *     - �̸��� ���Ͽ� ��ġ�ϸ� �� ���� �׸��� �����Ѵ�.
  *     - withRename�� TRUE�̰�, mpLocalRenamedAs�� ��ȿ�ϸ�,
  *       + mpLocalRenamedAs���� �̸��� �и��Ͽ�, �̰��� name�� ��ġ�ϸ� �� ���� �׸��� �����Ѵ�.
  *     .
  */
  KSyncItem *FindChildItem(LPCTSTR name, BOOL withRename);

  /**
  * @brief ���� �׸񿡼� ������ ���� OID�� ���� �׸��� ã�� �����Ѵ�.
  * @param serverOID : ã�� ���� OID
  * @return KSyncItem* : ã�� �׸�.
  * @details A. ���� �׸�鿡 ����,
  *     - ���� OID�� ���Ͽ� ��ġ�ϸ� �� ���� �׸��� �����Ѵ�.
  *     - ���� �׸��� �����̸� KSyncFolderItem::FindChildItemByOIDCascade()�� ȣ���ϰ� ���ϰ��� NULL�� �ƴϸ� ���ϰ��� �����Ѵ�.
  *     .
  * @n B. NULL�� �����Ѵ�.
  */
  KSyncItem *FindChildItemByOIDCascade(LPCTSTR serverOID);

  /**
  * @brief ���� �׸񿡼� ������ ��ο� �ش��ϴ� ��ü�� ã�Ƽ�, ���� OID�� ��ü�� �����͸� �����Ѵ�.
  * @param pathname : ã�� ��θ�
  * @param serverOID : ã�� ��ü�� ���� OID�� ������ ����
  * @return KSyncItem* : ã�� �׸�.
  * @details A. pathname�� ���̰� 0�̸�,
  *     - �� ��ü�� ���� OID�� serverOID�� �����ϰ�,
  *     - �� ��ü�� �����͸� �����Ѵ�.
  *     .
  * @n B. pathname���� ���� ù��° ���� �̸��� �����ͼ�, FindChildItem()���� �� �̸��� �ش��ϴ� ���� ��ü�� ã�´�.
  * @n C. ã�� ��ü�� NULL�� �ƴϰ� �����̸�,
  *     - ������ ã�� ��θ��� ������� ������
  *       + KSyncFolderItem::GetFolderOIDCascade()�� ��� ȣ���ϰ� �� ���ϰ��� �����Ѵ�.
  *     - �ƴϸ�
  *       + ã�� ��ü�� ���� OID�� serverOID�� �����ϰ�,
  *       + ã�� ��ü�� �����Ѵ�.
  *     .
  * @n D. NULL�� �����Ѵ�.
  */
  KSyncFolderItem *GetFolderOIDCascade(LPTSTR pathname, LPTSTR OUT serverOID);

  /**
  * @brief �� ��ü�� �ش��ϴ� ������ ������ �����ϴ��� Ȯ���Ѵ�.
  * @param createIfNotExist : TRUE�̰� ã�� ������ ������ ���� �����Ѵ�.
  * @return BOOL : ������ ������ �����ϸ� TRUE�� �����Ѵ�.
  * @details A. �� ��ü�� ���� OID�� ���������,
  *     - ����� TRUE�� �����Ѵ�.
  *     - �θ� ��ü�� NULL�� �ƴϸ� �θ� ��ü�� KSyncFolderItem::CheckServerFolderExist()�� ȣ���ϰ� ����� �޾ƿ´�.
  *     - ����� TRUE�̰� createIfNotExist�� TRUE�̸�,
  *       + KSyncCoreEngine::SyncServerCreateFolder()�� �������� ������ �����Ѵ�.
  *     - ����� �����Ѵ�.
  *     .
  * @n B. �ƴϸ� TRUE�� �����Ѵ�.
  */
  BOOL CheckServerFolderExist(BOOL createIfNotExist);

  /**
  * @brief ����ȭ �۾��߿� ������ ������ ���������� �����ϴ��� Ȯ���Ѵ�. 
  * �� �Լ��� ����ȭ ������ ���� �߻��ϸ� ���� ������ ������ ������� üũ�Ͽ�, 
  * �����Ǿ����� �ش� ������ ���� �׸��� ���̻� ������� �ʵ��� ������ ó���ϱ� ���� ���ȴ�.
  * �ƴϸ� ���ǹ��� �õ��� ����ϰ� �ȴ�.
  * @return BOOL : ������ ������ �����ϸ� TRUE�� �����Ѵ�.
  * @details A. �� ��ü�� ��ü ��θ��� �غ��ϰ�, �� ������ ���ÿ� �������� ������, 
  *     - ��� �� �浹 ����� FILE_SYNC_NO_LOCAL_FOLDER_FOUND�� �����Ѵ�.
  *     - ���� ���¿� ITEM_STATE_ERROR���� ���Ѵ�.
  *     - �� ��ü�� phase���� PHASE_CONFLICT_FILES�� �Ѵ�.
  *     - �θ� ������ �����ϸ� �θ� ������ ���� KSyncFolderItem::CheckLocalFolderExistOnSyncing()�� ��������� ȣ���Ѵ�.
  *     - FALSE�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ� TRUE�� �����Ѵ�.
  */
  BOOL CheckLocalFolderExistOnSyncing();

  /**
  * @brief ����ȭ �۾��߿� ������ ������ ���������� �����ϴ��� Ȯ���Ѵ�. 
  * �� �Լ��� ����ȭ ������ ���� �߻��ϸ� ���� ������ ������ ������� üũ�Ͽ�, 
  * �����Ǿ����� �ش� ������ ���� �׸��� ���̻� ������� �ʵ��� ������ ó���ϱ� ���� ���ȴ�.
  * �ƴϸ� ���ǹ��� �õ��� ����ϰ� �ȴ�.
  * @return BOOL : ������ ������ �����ϸ� TRUE�� �����Ѵ�.
  * @details A. KSyncCoreEngine::SyncServerCheckFolderExist()�� ȣ���Ͽ� �������� ������ ���������� �������� ������,
  *     - ��� �� �浹 ����� FILE_SYNC_NO_SERVER_FOLDER_FOUND�� �����Ѵ�.
  *     - ���� ���¿� ITEM_STATE_ERROR���� ���Ѵ�.
  *     - �� ��ü�� phase���� PHASE_CONFLICT_FILES�� �Ѵ�.
  *     - �θ� ������ �����ϸ� �θ� ������ ���� KSyncFolderItem::CheckServerFolderExistOnSyncing()�� ��������� ȣ���Ѵ�.
  *     - FALSE�� �����Ѵ�.
  *     .
  * @n B. �ƴϸ� TRUE�� �����Ѵ�.
  */
  BOOL CheckServerFolderExistOnSyncing();

  /**
  * @brief ���� �׸� �߿��� ����ȭ �浹 ó��â ��Ͽ� ��Ÿ�� �׸��� ������ ī��Ʈ�Ͽ� �����Ѵ�.(��� ȣ������ ����)
  * @return int : ����� ����
  * @details A. ���� �׸�鿡�� 
  *     - KSyncItem::IsResolveVisible()�� ȣ���Ͽ� TRUE�̸� ī��Ʈ�� 1 �߰��Ѵ�.
  *     .
  * @n B. ī��Ʈ���� �����Ѵ�.
  */
  int GetResolveChildItemCount();

  /**
  * @brief ���� �׸� �� ����ȭ �浹 ó��â ��Ͽ� ��Ÿ�� �׸񿡼� index��° ��ü�� �����Ѵ�.(��� ȣ������ ����)
  * @param index : ã���� �ϴ� �׸��� ����.
  * @return KSyncItem* : ���� �׸� ������
  * @details A. ���� �׸�鿡�� 
  *     - KSyncItem::IsResolveVisible()�� ȣ���Ͽ� TRUE�̸�,
  *       + ī��Ʈ�� index�� �����ϸ� �� ��ü�� �����Ѵ�.
  *       + ī��Ʈ ���� 1 �߰��Ѵ�.
  *     .
  * @n B. NULL�� �����Ѵ�.
  */
  KSyncItem *GetResolveChildItem(int index);

  /**
  * @brief ���� �׸� �� ����ȭ �浹 ó��â ��Ͽ� ��Ÿ�� �׸��� ���ǿ� �´� ��ü���� ����Ʈ�� �����Ѵ�.(��� ȣ������ ����)
  * @param itemArray : ã�� �׸��� �����͸� ��ư� ���.
  * @param type : ã���� �ϴ� �׸��� ����.
  * @param flag : �÷���
  * @details A. ���� �׸�鿡�� 
  *     - KSyncItem::IsResolveVisible()�� TRUE�̰� ã�� Ÿ���� ��ġ�ϸ�,
  *       + �÷��׿� EX_DUMMY�� �ְ�(���� �׸��� ������), �� �׸��� �����̸�,
  * �� �׸��� ���� �����̸� (KSyncFolderItem::IsDummyCloneFolder()) �����Ѵ�.
  *       + �÷��׿� IN_SINGLE_DUMMY�� ������,
  *         + �� �׸��� ���� �����̸�, GetNormalChildItem()���� ���� ��ü�� ã�� ���� ��ü�� ���ų�, ���� ��ü�� IsResolveVisible() �̸� ���Խ�Ű��,
  * �ƴϸ� ���ܽ�Ų��.
  */
  void GetResolveChildItems(KSyncItemArray &itemArray, int type, int flag = 0);

  /**
  * @brief ���� �׸� �� ������ ����� ���ǿ� �ش��ϴ� ��ü���� ����Ʈ�� ��� �����Ѵ�.
  * @param itemArray : ã�� �׸��� �����͸� ��ư� ���.
  * @param type : ã���� �ϴ� �׸��� ����.
  * @param resultMask : ��� ����ũ
  * @param recursive : TRUE�̸� ���� ������ ���� ���ȣ���� �Ѵ�.
  * @return int : ã�� ��ü�� ������ �����Ѵ�.
  * @details A. �� �׸��� �浹 ������� resultMask�� AND�� ����� 0�� �ƴϸ�,
  *     - ����Ʈ�� �� ��ü�� ��� ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n B. �ƴϸ� ���� �׸�鿡�� 
  *     - �׸��� �����̸�,
  *       + recursive�� TRUE�̸� ���� �׸� KSyncFolderItem::GetChildItemArray()�� ȣ���ϰ� ���ϰ��� ī��Ʈ�� ���Ѵ�.
  *     - �׸��� ������ �ƴϰ�, ������ ������ type�� ��ġ�ϸ�, �浹 ������� resultMask�� AND�� ����� 0�� �ƴϸ�, 
  *       + ����Ʈ�� ���� �׸��� ��� ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */
  int GetChildItemArray(KSyncItemArray &itemArray, int type, int resultMask, BOOL recursive);

  /**
  * @brief ���� �׸��� �浹 ������� �θ� ������ mChildResult�� ���Ͽ� ���� ������ �浹�� �ִ� �������� ������ �� �� �ֵ��� �Ѵ�.
  * @param depth : calling depth. start from 0
  * @param rootFolder : ����ȭ ��Ʈ ���� �׸�
  * @param flag : �÷���. (REMOVE_DISABLED, STORE_META, SET_CONFLICT_CHILD)
  * @return int : ����ȭ ������� �����Ѵ�.
  * @details A. �÷��׿� SET_CONFLICT_CHILD�� ������,
  *     - mResult�� 0�� �ƴϸ�, AddConflictResult()�� �浹 ������� (FILE_SYNC_BELOW_CONFLICT | (mResult & FILE_USER_ACTION_MASK))�� �Ѵ�.
  *     .
  * @n B. �浹 �����(mConflictResult)�� �浹�̳� �����ڵ尡 ������, flag�� SET_CONFLICT_CHILD�� ���Ѵ�.
  * (�� �� ���� ������ ��ü���� ��� �浹 ������� FILE_SYNC_BELOW_CONFLICT�� ���´�)
  * @n C. ���� �׸�鿡 ����
  *     - �׸��� �����̸�, KSyncFolderItem::UpdateFolderSyncResult()�� ��� ȣ���Ѵ�.
  *     .
  * @n D. mChildResult�� Ŭ�����ϰ�, ���� �׸�鿡 ����
  *     - ���� �׸��� �浹 ������� FILE_SYNC_RESOLVE_VISIBLE_FLAG�� ����ũ�� ������� mChildResult�� OR�� ���Ѵ�.
  *     - ���� �׸��� �����̸� ���� �׸��� mChildResult�� �� ��ü�� mChildResult�� ���Ѵ�.
  *     - ���� �׸��� ������ �ƴϸ�,
  *       + flag��  SET_CONFLICT_CHILD�� ������,
  *         + ���� �׸��� ����� 0�� �ƴϸ�, ���� �׸��� �浹 ������� (FILE_SYNC_BELOW_CONFLICT | (item->mResult & FILE_USER_ACTION_MASK))�� ���Ѵ�.
  *     - flag�� REMOVE_DISABLED�� �ְ�, ���� �׸��� Disable �����̸�,
  *       + ���� �׸��� child ��Ͽ��� �����ϰ�, ���� �׸��� �����Ѵ�.
  *     .
  * @n E. flag�� STORE_META�� ������,
  *     - StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����͸� �����Ѵ�.
  *     .
  * @n F. ����ȭ ������� �����Ѵ�.
  */
  int UpdateFolderSyncResult(int depth, KSyncRootFolderItem *rootFolder, int flag);

  /**
  * @brief ���� �׸��� ��� ��θ��� �߰��� ���� �̸����κ��� ���´�.
  * @param pItem : ��� ����ȭ �׸�
  * @param relativePath : ��� ��θ��� ����� ����
  * @details A. ���� �׸�鿡 ����
  *     - relativePath�� ���� �׸��� �̸��� ���δ�.
  *     - ���� �׸��� pItem�� ��ġ�ϸ� TRUE�� �����Ѵ�.
  *     - ���� �׸��� �����̸�,
  *       + KSyncFolderItem::GetItemRelativePath()�� ȣ���ϰ� ���ϰ��� TRUE�̸� TRUE�� �����Ѵ�.
  *     - relativePath�� ���� �Էµ� ��ŭ���� �ڸ���.
  *     .
  * @n B. FALSE�� �����Ѵ�.
  */
  BOOL GetItemRelativePath(KSyncItem *pItem, LPTSTR OUT relativePath);

  /**
  * @brief ��θ��� ���̸� �����Ѵ�.
  * @param type : 1�̸� ������ ��θ�, 2�̸� ������ ��θ�.
  * @details A. �θ� ������ NULL�� �ƴϸ�
  *     - �θ� �������� KSyncFolderItem::GetPathNameLength()���� ���̸� ���Ѵ�
  *     - �ڽ��� �̸��� ���� + 1�� ���̿� ���Ѵ�.
  *     .
  * @n B. �ƴϸ�(�� ��ü�� ����ȭ ��Ʈ ������)
  *     - type�� 1�̸� mBaseFolder�� ���� + 1�� ���̿� ���Ѵ�.
  *     - type�� 2�̸� GetServerRelativePath()�� ���� + 1�� ���̿� ���Ѵ�.
  *     .
  * @n C. ���̰��� �����Ѵ�.
  */
  int GetPathNameLength(int type);

  /**
  * @brief ��θ��� ������ ���ۿ� �����Ѵ�.
  * @param buff : ��θ��� ������ ����.
  * @param len : ��θ��� ������ ������ ũ��.
  * @param type : 1�̸� ������ ��θ�, 2�̸� ������ ��θ�.
  * @details A. �θ� ������ NULL�� �ƴϸ�
  *     - �θ� �������� KSyncFolderItem::GetPathName()���� ��θ��� ���Ѵ�
  *     - �ڽ��� �̸��� ���Ѵ�..
  *     .
  * @n B. �ƴϸ�(�� ��ü�� ����ȭ ��Ʈ ������)
  *     - type�� 1�̸� mBaseFolder�� ���ۿ� �����Ѵ�..
  *     - type�� 2�̸� GetServerRelativePath()�� ����� ���ۿ� �����Ѵ�..
  */
  void GetPathName(LPTSTR buff, int len, int type);

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
  //int GetRenamedPathName(LPTSTR buff, int len, int flag);

  /**
  * @brief ���� ���� �߿��� �̸� ����� ������ �ִ��� Ȯ���Ѵ�.
  * @return BOOL : ����� ������ ������ TRUE�� �����Ѵ�.
  * @param flag : ���� �Ǵ� ������.
  * @details A. flag�� AS_LOCAL���� �ְ�, ������ �̸� ���� ������ ��ȿ�ϸ� TRUE�� �����Ѵ�.
  * @n B. flag�� AS_LOCAL���� ����, ������ �̸� ���� ������ ��ȿ�ϸ� TRUE�� �����Ѵ�.
  * @n C. �θ� ��ü�� NULL�� �ƴϸ� �θ� ��ü�� KSyncFolderItem::IsParentRenamed()�� ȣ���ϰ� ���ϰ��� �����Ѵ�.
  * @n D. FALSE�� �����Ѵ�.
  */
  BOOL IsParentRenamed(int flag);

#if 0
  /*
  * @brief ����ȭ ��� ��� ��θ��� ���ۿ� �����Ѵ�.
  * @param name : ��θ��� ������ ����.
  * @param len : ��θ��� ������ ������ ũ��.
  * @param flag : ���� �Ǵ� ������.
  */
  virtual void GetSyncResultRName(LPTSTR name, int length, int flag);
#endif

  /**
  * @brief ����ȭ ��� �̵��� ��� ��θ��� �� ���ۿ� �Ҵ�, �����Ͽ� �����Ѵ�.
  * @param flag : ���� �Ǵ� ������.
  * @details A. name�� NULL�� �Ѵ�.
  * @n B. �� �׸��� ����ȭ ���� ���� �����̸�(IsDummyCloneFolder()),
  *     - flag�� AS_LOCAL���� �ְ�, mpDummyLocalPath�� ��ȿ�ϸ�
  *       + flag�� AS_NAME_ONLY���� ������ mpDummyLocalPath�� �̸� �κи� name�� �����ϰ�,
  *       + �ƴϸ� mpDummyLocalPath�� name�� ���� �����Ѵ�.
  *     - flag�� AS_LOCAL���� ����, mpDummyServerPath�� ��ȿ�ϸ�
  *       + flag�� AS_NAME_ONLY���� ������ mpDummyServerPath�� �̸� �κи� name�� �����ϰ�,
  *       + �ƴϸ� mpDummyServerPath�� name�� ���� �����Ѵ�.
  *     .
  * @n C. name�� �����Ѵ�.
  */
  virtual LPTSTR AllocSyncMoveResultName(int flag);

  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ���� �׸��� ���� �������� Ȯ���Ѵ�.
  */
  inline BOOL IsOpened() { return !(mResolveFlag & FOLDER_CLOSED); }
  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ���� �׸��� ���� ���·� �Ѵ�.
  */
  inline void SetOpened(BOOL open) { if (open) mResolveFlag &= ~FOLDER_CLOSED; else mResolveFlag |= FOLDER_CLOSED; }

  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ���� ���� �������� Ȯ���Ѵ�. 
  * ���� ���� ������ �ٸ� ������ �̵��� ��ü���� ���� ���� �����ֱ� ���� �ӽ� ������ ��ü�̴�.
  */
  inline BOOL IsDummyCloneFolder() { return (mResolveFlag & DUMMY_FOLDER); }
  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ���� ���� ������ �����Ѵ�. 
  */
  inline void SetDummyCloneFolder() { mResolveFlag |= DUMMY_FOLDER; }

  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ���� ���� ������ ���� �Ǵ� ���� ��ü�� ã�� �����Ѵ�. 
  * @param name : ã�� ��ü�� �̸�
  * @return KSyncItem* : ã�� ��ü�� ������
  * @details A. �����׸� �߿���
  *     - �׸��� �����̰�, �̸��� ���� ��ü�� ������ ���� ���� �����̸� �� �׸��� �����Ѵ�.
  *     .
  * @n B. NULL�� �����Ѵ�.
  */
  KSyncItem* GetNormalChildItem(LPCTSTR name = NULL);

  /**
  * @brief ����ȭ �浹ó�� Ʈ������ origin�� �ش��ϴ� ���� ���� ������ ��̿� ��ư���. 
  * @param origin : ã�� ���� ��ü
  * @param dummyItems : ã�� ���� ��ü�� ���� ���.
  * @return int : ã�� �׸� ����
  * @details A. �����׸� �߿���
  *     - �׸��� �����̰�, ���� ��ü�� �ƴϸ�, ���� ���� �����̸�, �̸��� ���� ��ü�� ������ ��̿� �߰��Ѵ�.
  *     .
  * @n B. ����� ������ �����Ѵ�.
  */
  int FindDummyPairItems(KSyncItem* origin, KSyncItemArray& dummyItems);

  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ������ ������ �ش��ϴ� �׸��� �����Ѵ�.
  * @param index : 1���� �����ϴ� ����.
  * @return KSyncItem* : ã�� ��ü�� ������
  * @details A. index������ 1�� ����.
  * @n B. getTreeDisplayChildItemR()�� ȣ���Ͽ� ���ϰ��� �����Ѵ�.
  */
  KSyncItem* GetTreeDisplayChildItem(int index);

  /**
  * @brief ����ȭ �浹ó�� Ʈ������ ������ ������ �ش��ϴ� �׸��� �����Ѵ�.
  * @param index : 0���� �����ϴ� ����.
  * @return KSyncItem* : ã�� ��ü�� ������
  * @details A. �� �׸��� IsVisible() && IsResolveVisible()�� �ش��ϸ�,
  *     - index���� 0�̸� �� ��ü�� �����Ѵ�.
  *     - index������ 1�� ����.
  *     .
  * @n B. �� ������ ���� ����(IsOpened())�̸� .
  *     - ���� �׸��߿���
  *       + �׸��� IsResolveVisible()�� TRUE�̰�, �׸��� �����̸�
  *         + index�� 0�̸� �� ���� �׸��� �����ϰ�,
  *         + index������ 1�� ����.
  *     .
  * @n C. �θ� ��ü�� NULL�� �ƴϰ� �� ��ü�� ���� ���� ��ü�� �ƴϸ�
  *     - �θ� ��ü���� FindDummyPairItems()�� ȣ���Ͽ� �� ��ü�� ���� ���� ��ü�� �޾ƿ´�.
  *     - ���� ���� ��ü ����Ʈ���� �� ��ü�� �߿���
  *       + �׸��� KSyncItem::IsVisible()�̸�,
  *         + index�� 0�̸� �� �׸��� �����ϰ�,
  *         + index������ 1�� ����.
  *       + �׸��� ���� �����̸�,
  *         + KSyncFolderItem::GetResolveChildItems()�� ȣ���Ͽ� �� �׸��� ���� �׸� ����Ʈ�� �޾ƿ´�.
  *         + index�� ����Ʈ �������� ������,
  *           + ����Ʈ���� index��° �׸��� �����ϰ�, index�� 0���� �Ѵ�.
  *         + index������ ����Ʈ ������ŭ ����.
  *     .
  * @n D. �� ��ü�� ���µ� �����̸�
  *     - GetResolveChildItems()���� ���� ��ü�� �ƴ� ���� �׸��� ����Ʈ�� �޾ƿ´�.
  *     - ���� C���� �޾ƿ� ���� ���� ���� ��ü ����Ʈ�� �׸� ����
  *       + �� �׸��� ���µ� �����̸�, �� �׸񿡼� GetResolveChildItems()�� ȣ���Ͽ� ���� ������ �ƴ� ���� �׸��� ����Ʈ�� �޾ƿ´�. 
  *     - GetResolveChildItems()�� ȣ���Ͽ� SINGLE_DUMMY�� �� ��ü���� ����Ʈ�� �޾ƿ´�. 
  *     - ���� �ܰ迡�� �޾ƿ� ����Ʈ�� �׸� ����
  *       + �׸� ���� getTreeDisplayChildItemR()�� ȣ���Ͽ� ����� NULL�� �ƴϸ� �װ��� �����Ѵ�.
  *     .
  * @n E. NULL�� �����Ѵ�.
  */
  KSyncItem* getTreeDisplayChildItemR(int& index);

  /**
  * @brief �� �׸��� ����ȭ �浹ó�� Ʈ���� ��Ÿ���� �׸����� Ȯ���Ѵ�..
  * @return BOOL : Ʈ���� ��Ÿ���� �׸��̸� TRUE�� �����Ѵ�.
  * @details A. KSyncItem::IsResolveVisible()�� ȣ���Ͽ� TRUE�̸� TRUE�� �����Ѵ�.
  * @n B. mChildResult�� FILE_SYNC_RESOLVE_VISIBLE_FLAG���� ������ TRUE�� �����Ѵ�.
  * @n C. FALSE�� �����Ѵ�.
  */
  virtual BOOL IsResolveVisible();

  /**
  * @brief ����ȭ �浹ó�� Ʈ���� ��Ÿ���� ������ �غ� �۾��� �Ѵ�. ����ȭ �浹ó�� Ʈ���� �ʱ�ȭ �� �� ȣ��ȴ�.
  * ���� �׸� �߿��� �ٸ� ������ �̵��� ��ü�� ã�Ƽ� �ش��ϴ� ���� ���� ���� ��ü�� �̵����� �ش�.
  * @param clones : ���� ��ü���� �� ����Ʈ�� ��´�.
  * @param flag : �÷���
  * @details A. flag & FORCE_OPEN_FOLDER�̸�, ���� ���·� �����Ѵ�.
  * @n B. InitResolveState()�� ȣ���Ͽ� ����ȭ �浹 �ʱ�ȭ�� �Ѵ�.
  * @n C. ���� �׸�鿡 ���Ͽ�
  *     - ���� �׸��� �����̸� KSyncFolderItem::InitializeResolveState()�� ��� ȣ���Ѵ�.
  *     - �ƴϸ� KSyncItem::InitResolveState()�� ȣ���Ѵ�.
  *     - GetItemStateMovedRenamed()�� ������ ���¿� ������ ���¸� �޾ƿͼ�, lo_flag, sv_flag�� �����Ѵ�.
  *     - flag & MAKE_CLONE_FOLDER�̰�, ���� �׸��� ����� ����ȭ �浹 �Ǵ� ������ ������, lo_flag �Ǵ� sv_flag�� ITEM_STATE_MOVED�� ������,
  *       + �׸��� ���� ��θ� �ӽ� �����Ͽ� �ΰ�, 
  *       + �׸��� ������ �̸� ����� �����̸�,
  *         + �̸� ���� �������� �� ��θ� �޾ƿͼ� newLocalPath�� �����ϰ� �̸� ���� ��ο� ���Ͽ�, 
  * ��ΰ� �ٲ��� �ʾ����� newLocalPath�� NULL�� �ϰ�, �ƴϸ� newLocalPath�� "\"�� �Ѵ�.
  *       + �׸��� ������ �̸� ����� �����̸�,
  *         + �̸� ���� �������� �� ��θ� �޾ƿͼ� newServerPath�� �����ϰ� �̸� ���� ��ο� ���Ͽ�, 
  * ��ΰ� �ٲ��� �ʾ����� newServerPath�� NULL�� �ϰ�, �ƴϸ� newServerPath�� "\"�� �Ѵ�.
  *       + ������ �Ǵ� ������ ��ΰ� �ٲ������,
  *         + GetMovedCloneFolder()���� �� ��ο� �ش��ϴ� ���� ��ü�� ã�ƿͼ� �� �׸��� parent�� �����ϰ�, �� �׸��� child ��Ͽ��� �����Ѵ�.
  *     .
  */
  void InitializeResolveState(KSyncItemArray& clones, int flag);

  /**
  * @brief ����ȭ �浹ó�� Ʈ���� ���� ���� ������ ���������� �׸���� ������ ���� ������ �̵����� �ش�.
  * @param clones : ���� ���� ������ �̵��Ǿ��� ��ü���� ����� �ִ�.
  * @details A. clones ����Ʈ�� �׸񿡼�
  *     - �׸��� �θ� NULL�̸� �� �׸��� AddChild()�� ȣ���Ͽ� child�� ����Ѵ�.
  *     - �ƴϸ� �׸��� �θ� ��ü�� AddChild()�� ȣ���Ͽ� child�� ����Ѵ�.
  *     - ����Ʈ���� �����Ѵ�.
  */
  void RestoreClones(KSyncItemArray& clones);

  /**
  * @brief ���� ��� �߿��� ���� ���� ������ �����ϰ� �����Ѵ�. ���� ���� ������ ���Ե� ���� ��ü���� ���� ��ü�� ������ �̵���Ų��.
  * @return int : ���ŵ� ������ �����Ѵ�.
  * @details A. ���� �׸� �߿���
  *     - �׸��� �����̸�,
  *       + KSyncFolderItem::RemoveDummyCloneFolders()�� ��� ȣ���ϰ� ���ϰ��� ī��Ʈ�� ���Ѵ�.
  *       + �׸��� ���� ���� �����̸�(IsDummyCloneFolder()),
  *         + �� ������ ã��, KSyncFolderItem::MoveChildItemTo()�� ȣ���Ͽ� ���� �׸���� �� ���� ������ �̵��Ѵ�.
  *         + �׸��� �����ϰ� child ��Ͽ��� �����ϰ�, ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n B. ī��Ʈ���� �����Ѵ�.
  */
  int RemoveDummyCloneFolders();

  /**
  * @brief �浹 �� ���� �߻��� �׸��� Enable�� �����Ѵ�. ����� UI(2016.09)������ ��� ����.
  * @return int : Disable �� �׸��� ������ �����Ѵ�.
  * @details A. �� �׸��� �浹 ������� (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)�� ������ Enable�� FALSE�� �ϰ�, ī��Ʈ�� 1 �����Ѵ�.
  * @n B. ���� �׸� �߿���
  *     - �׸��� �����̸� KSyncFolderItem::CheckEnableOnResolveConflict()�� ��� ȣ���ϰ� ���ϰ��� ī��Ʈ�� ���Ѵ�.
  *     - �ƴϸ�,
  *       + �浹 ������� (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)�� ������ Enable�� FALSE�� �ϰ�, ī��Ʈ�� 1 �����Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */
  int CheckEnableOnResolveConflict();

  /**
  * @brief �� �׸� �Ʒ��� ���� �׸��� ������ �θ� ������ �Ʒ��� �̵��Ѵ�.
  * @param newParent �� �θ� ����
  * @return int : �̵��� �׸��� ������ �����Ѵ�.
  */
  int MoveChildItemTo(KSyncFolderItem *newParent);

  /**
  * @brief �� �׸� �Ʒ��� ���� �׸��� ������ �θ� ������ �Ʒ��� �̵��Ѵ�.
  * @param clones : ���� �׸� ����Ʈ
  * @param org : �� ���� ��ü
  * @param newLocalPath : ���ο� ������ ���
  * @param newServerPath : ���ο� ������ ���
  * @return KSyncFolderItem* : �̵��� ���� �׸��� �����Ѵ�.
  * @details A. ���� �׸� ����Ʈ����
  *     - ����Ʈ ���� �׸� KSyncFolderItem::IsMatchDummyClonePath()�� ȣ���Ͽ� TRUE�̸� �� �׸��� �����Ѵ�.
  *     .
  * @n B. �� ���� �׸��� ��Ʈ �����̸� KSyncRootFolderItem�� �����ϰ�, �ƴϸ� KSyncFolderItem�� ���� ��ü�� �����Ѵ�.
  * @n C. ������ �����ϰ�, SetDummyCloneFolder(), SetDummyClonePath()�� ȣ���Ͽ� ���� ���� �Ӽ��� �����ϰ�,
  * @n D. ���� �׸� ����Ʈ�� �߰��� �� �ش� ��ü�� �����Ѵ�.
  */
  KSyncFolderItem* GetMovedCloneFolder(KSyncItemArray& clones, KSyncFolderItem* org, LPCTSTR newLocalPath, LPCTSTR newServerPath);

  // check if parent does not checked, but child is checked
  /**
  * @brief ����ȭ �浹 ó��â���� ���� �׸��� ���õǰ� �θ� �׸��� ������ �ȵ� ���·� ����ȭ�� �����ϸ� �ȵǱ� ������ �̷��� ������ �ִ��� �˻��Ѵ�.
  * @param p : ���õ��� ���� �θ� ������ ��θ��� �޾ư��� ����
  * @details A. �� �׸��� Disable �����̰�, IsConflictResolved()�̰�, ��Ʈ ������ �ƴϸ�,
  *     - GetEnableChild()�� ȣ���Ͽ� Enable�� ���� �׸��� �����ϸ�,
  *       + ���� p�� NULL�� �ƴϸ�, �� �׸��� mBaseFolder(��Ʈ �����̸� "\")�� ���� p�� �����ϰ�
  *       + TRUE�� �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡��
  *     - ���� �׸��� �����̰�, KSyncFolderItem::IsChildCheckedWithoutParent()�� ��� ȣ���Ͽ� TRUE�̸�, TRUE�� �����Ѵ�.
  *     .
  * @n C. FALSE�� �����Ѵ�.
  */ 
  BOOL IsChildCheckedWithoutParent(LPTSTR *p);

  /**
  * @brief ������ ��θ��� ��ü�� �����ϴ��� Ȯ���Ѵ�.
  * @param relativePathName : ã�� ��ü�� ��� ��θ�.
  * @param onLocal : ������ �Ǵ� ������
  * @param ex : ���ܷ� ó���� �׸�, ã�� ����� �� ��ü�̸� �����Ѵ�.
  * @details A. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� ��Ʈ �����̸� KSyncFolderItem::FindChildItemCascade()�� ��� ��θ� �ش��ϴ� ��ü�� ã�ƿ´�.
  *     - ����� NULL�� �ƴϰ�, ex�� �ƴϸ� ����� �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� ��Ʈ �����̸� KSyncFolderItem::FindRenamedChildItem()�� ��� ��θ� �ش��ϴ� ��ü�� ã�ƿ´�.
  *     - ����� NULL�� �ƴϸ� ����� �����Ѵ�.
  *     .
  * @n C. NULL�� �����Ѵ�.
  */ 
  KSyncItem* CheckIfExistOnVirtualTop(LPTSTR relativePathName, BOOL onLocal, KSyncItem* ex);

  /**
  * @brief ����ȭ �浹 ó��â���� ��� �׸��� ����(����)�� �����Ϸ��µ�, �̹� ������ ������ �����ϴ��� �˻��Ѵ�.
  * @param root : ����ȭ ��Ʈ ���� �׸�
  * @param applyToServer : TRUE�̸� ������ �����ϴ� ���, �ƴϸ� ���÷� �����ϴ� ���.
  * @param name : �浹 �߻��� �׸� �̸��� �޾ư��� ����.
  * @return BOOL : ������ �̸��� ������ TRUE�� �����Ѵ�.
  * @details A. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� Enable�̰�, ��Ʈ ������ �ƴϸ�,
  *       + applyToServer�� TRUE�̰�, �׸��� ���� ������ �����ϰ�, �׸��� ���� ���¿� ITEM_STATE_EXISTING�� �ְų�,
  * applyToServer�� FALSE�̰�, �׸��� ���� ������ �����ϰ�, �׸��� ���� ���¿� ITEM_STATE_EXISTING�� ������,
  *         + ����ȭ ��� ��θ��� �غ��ϰ�,
  *         + KSyncFolderItem::CheckIfExistOnVirtualTop()�� ȣ���Ͽ� ����׸��� ã�ƿ���,
  *         + ��� �׸��� �����ϰ� �̰��� �� ���� �׸�� �ٸ���,
  *           + name�� ����ȭ ��� ��θ��� �����ϰ�, TRUE�� �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� �����̸� KSyncFolderItem::CheckIfCrossConflict()�� ���ȣ���Ͽ� ����� TRUE�̸� TRUE�� �����Ѵ�.
  *     .
  * @n C. FALSE�� �����Ѵ�.
  */ 
  BOOL CheckIfCrossConflict(KSyncFolderItem*root, BOOL applyToServer, LPTSTR *name);

  /**
  * @brief ���� �׸���� ���ǿ� ���� �����Ѵ�.
  * @param sortColumn : ������ �÷�
  * @param sortAscent : 0�� �ƴϸ� ���� ����
  * @return int : ���� �׸��� ������ �����Ѵ�.
  * @details A. ���� �׸��� ������ 1���� ũ��
  *     - ���� �׸���� �ӽ� ��̿� �־�ΰ�, ���� �׸� ����Ʈ�� ����.
  *     - �ӽ� ����� �׸�鿡 ����
  *       + ���� �׸� ����Ʈ�� ������� CompareSyncItem()�� ���Ͽ�  ����� 0���� ������ �տ� �����Ѵ�.
  *       + �ƴϸ� �� �ڿ� �����Ѵ�.
  *     .
  * @n B. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� �����̸� KSyncFolderItem::SortChildList()�� ���ȣ���Ͽ� ������ ���� �׸�鵵 ���Ľ�Ų��.
  *     - ������� ī��Ʈ�� ���Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */ 
  int SortChildList(int sortColumn, int sortAscent);

  /**
  * @brief �� ��ü�� �����Ͽ� ���� �׸��߿� ���õ� �׸��� ������ TRUE�� �����Ѵ�.
  * @details A. KSyncItem::HasSelected()���� �� ��ü�� ���õǾ� ������ TRUE�� �����Ѵ�.
  * @n B. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� ���õǾ� ������ TRUE�� �����Ѵ�.
  *     .
  * @n C. FALSE�� �����Ѵ�.
  */ 
  virtual BOOL HasSelected();

  /**
  * @brief �� ��ü�� �����Ͽ� ���� �׸��߿� ���õ� �׸��� Enable�� �����Ѵ�.
  * @details A. KSyncItem::EnableSelectedItem()���� �� ��ü�� ���õǾ� ������ Enable�� �����Ѵ�.
  * @n B. ���� �׸�鿡 ���Ͽ�
  *     - KSyncItem::EnableSelectedItem()���� �� �׸��� ���õǾ� ������ Enable�� �����Ѵ�.
  *     .
  */ 
  virtual void EnableSelectedItem();

  /**
  * @brief �� ��ü�� �����Ͽ� ���� �׸� ResolveFlag�� ������ ���� ����ũ�ϰ� ���Ѵ�.
  * @details A. KSyncItem::SetResolveFlagMask()���� �� ��ü�� ResolveFlag�� ������ ���� ����ũ�ϰ� ���Ѵ�.
  * @n B. ���� �׸�鿡 ���Ͽ�
  *     - KSyncItem::IsResolveVisible()�̸�,
  *       + KSyncItem::SetResolveFlagMask()���� �� ��ü�� ResolveFlag�� ������ ���� ����ũ�ϰ� ���Ѵ�.
  *     .
  */ 
  virtual void SetResolveFlagMask(int a, int o, BOOL withChild);

  /**
  * @brief ����ȭ �浹 ó�� �÷��װ�(mResolveFlag)�� ������ ���ǿ� �ش�Ǵ� ��ü���� �ε��� ������ ���Ѵ�.
  * @param a : AND�� ����ũ�� ��.
  * @param o : �񱳰�.
  * @param index : ��ü�� index��.
  * @param area : ��ü�� �ε��� ������ ������ ����.
  * @param flag : �÷��� ��.
  * @details A. KSyncItem::GetResolveFlagArea()�� ȣ���Ͽ� �� ��ü�� mResolveFlag�� �˻��ϰ� ������ ���Ѵ�.
  * @n B. �� ������ ���� �����̰�, IsResolveVisible()�̰�, (flag & WITH_CHILD_DOCUMENT)�̸�,
  *     - ���� �׸�鿡 ���Ͽ�,
  *       - ���� �׸��� IsResolveVisible()�̰�, �����̸�,
  *         + �� �׸� ���� KSyncItem::GetResolveFlagArea()�� ȣ���Ͽ� �� ��ü�� mResolveFlag�� �˻��ϰ� ������ ���Ѵ�.
  *     - (flag & WITH_CHILD_FOLDER)�̸�,(���� ������ �ִ� ��ü�� ���� ���� ������ ���� ��ü�� ���� ������ Ʈ���� ǥ�õǴ� �Ʒ��� ���� ������ ó���Ѵ�)
  *       + �̰�ü�� ��Ʈ �����̰�, ���� ���� ������ �ƴϸ�,
  *         + FindDummyPairItems()�� ���� ���� �������� ��̸� �޾ƿ´�.
  *         + ���� ���� �������� ����� ���� �׸�鿡 ����
  *           + �� �׸��� IsVisible()�̸�, KSyncItem::GetResolveFlagArea()�� ȣ���Ͽ� �� ��ü�� mResolveFlag�� �˻��ϰ� ������ ���Ѵ�.
  *       + GetResolveChildItems()���� ���̰� �ƴ� ���� �׸���� ��̸� itemArray�� �޾ƿ´�.
  *         + ���� ���� �������� ����� ���� �׸�鿡 ����
  *           + ���µ� �׸��̸� GetResolveChildItems()���� ���̰� �ƴ� ���� �׸���� ��̸� itemArray�� �޾ƿ´�.
  *       + GetResolveChildItems()���� IN_SINGLE_DUMMY�� ���� �׸���� ��̸� itemArray�� �޾ƿ´�.
  *       + ���� ���ܿ��� ���� itemArray�� ���� �׸�鿡 ����
  *         + KSyncItem::GetResolveFlagArea()�� ȣ���Ͽ� �� ��ü�� mResolveFlag�� �˻��ϰ� ������ ���Ѵ�.
  *     .
  */
  virtual int GetResolveFlagArea(int a, int o, int index, int* area, int flag);

  /**
  * @brief ����ȭ �浹 ó��â�� Ʈ���� ��Ÿ���� �� �׸�� ���� �׸��� �� ������ ���Ѵ�.
  * @return int : ������ �����Ѵ�.
  * @details A. IsVisible() �̰� IsResolveVisible()�̸� ī��Ʈ�� 1�� �Ѵ�.
  * @n B. �� ���� �׸��� ���µ� �����̸�,
  *     - ���� �׸�鿡 ���Ͽ�
  *       + �׸��� KSyncItem::IsResolveVisible()�̸� �� �׸� ���� GetChildRowCount()�� ��� ȣ���Ͽ� ���ϰ��� ī��Ʈ�� ���Ѵ�.
  *     .
  * @n C. ī��Ʈ���� �����Ѵ�.
  */
  virtual int GetChildRowCount();

  /**
  * @brief ���� �׸��߿� ������ �̸��� ������ �̸��� �����ϴ��� Ȯ���Ѵ�.
  * ������ �̸��� �����ϰ� �װ��� ���� �̸��̸�(�ٸ� �̸����� �ٲ� ������ �ƴϸ�) �ӽ� �̸� �ٲٱ�� �ذ���� ���ϴ� �浹�� ó���Ѵ�.
  * @param childName : ã�� �̸�
  * @param except : �̸��� ���Ƶ� ���ܷ� �� �׸�
  * @param checkIfMoved : 0 : ���� üũ ����, 1 : ���� ����, 2 : ���� ����.
  * @param onLocal : TRUE�̸� ������ �̸� ���� Ȯ��, FALSE�̸� ������ �̸� ���� Ȯ��.
  * @return int : 0�̸� ������ �̸� ����, 1�̸� ������ �̸��� ������, 2�̸� ������ �̸��� �����ϰ� ������� ��������.
  * @details A. ���� �׸�鿡 ���Ͽ�
  *     - �׸��� except�� �ƴϸ�,
  *       + �׸��� �̸��� ������ childName�� ��ġ�ϰų�,  �׸��� �� �̸�(mLocalNew.Filename)�� childName�� ��ġ�ϸ�,
  *         + �� �׸��� �ӽ÷� �̸��� �ٲ� ��(IsServerTemporaryRenamed())�� �ƴϸ�,
  *           + checkIfMoved�� 0�� �ƴϸ�,
  *             + checkIfMoved��  2�̰�, ���� ���°� ITEM_STATE_MOVED�̰ų� IsLocalTemporaryRenamed()�� ���� �ƴϸ� 2�� �����Ѵ�.
  *             + checkIfMoved��  1�̰�, ���� ���°� ITEM_STATE_MOVED�̰ų� IsServerTemporaryRenamed()�� ���� �ƴϸ� 2�� �����Ѵ�.
  *           + 1�� �����Ѵ�.
  *     .
  * @n B. 0�� �����Ѵ�.
  */
  int CheckIfChildNameExist(LPCTSTR childName, KSyncItem* except, int checkIfMoved, BOOL onLocal);

  /**
  * @brief �̵� ó���ÿ� ������ �̸��� �ٸ� �׸��� �����Ͽ� �ӽ� �̸����� �ٲپ� ó���� �� ȣ��Ǹ�, �ӽ� �̸��� ����� �����Ѵ�.
  * @param childName : ������ �̸�
  * @param resultName : �ӽ� �̸��� ������ ����
  * @param side : 1�̸� ������, 2�̸� ������
  * @return BOOL : �����Ͽ����� TRUE�� �����Ѵ�.
  * @details A. resultName�� ������ �̸��� �����Ѵ�.
  * @n B. resultName�� ".AAA"�� ���δ�.
  * @n C. CheckIfChildNameExist()���� resultName�� �ش��ϴ� ������ �����ϴ��� Ȯ���ϰ�, �������� ������ TRUE�� �����Ѵ�.
  * @n D. MakeNextSequenceName()�� ȣ���Ͽ� "AAA" �κ��� ���� ���ڿ��� �ٲ۴�.(AAA, AAB, AAC,,,ABA,,,ZZZ)
  * @n E. ���� ���ڿ��� �ٲپ����� C�� �̵��Ѵ�.
  * @n F. FALSE�� �����Ѵ�.
  */
  BOOL ReadyUniqueChildName(LPCTSTR childName, LPTSTR resultName, int side, BOOL onLocal);

  /**
  * @brief �������� ������ �ӽ� �̸��� ����� �Ҵ�� �޸𸮸� �����Ѵ�.
  * @param pathname : ������ �̸�
  * @return LPTSTR : ���� ���� �ӽ� �̸��� ������ �����ּ�.
  * @details A. ��� ��Ʈ���� ������ ���۸� �Ҵ��ϰ�, ���⿡ pathname�� �����Ѵ�.
  * @n B. ��� ��Ʈ���� ".AAA"�� ���δ�.
  * @n C. IsDirectoryExist()���� �ش� ������ �����ϴ��� Ȯ���ϰ�, �������� ������ ��� ��Ʈ���� �����Ѵ�.
  * @n D. MakeNextSequenceName()�� ȣ���Ͽ� "AAA" �κ��� ���� ���ڿ��� �ٲ۴�.(AAA, AAB, AAC,,,ABA,,,ZZZ)
  * @n E. ���� ���ڿ��� �ٲپ����� C�� �̵��Ѵ�.
  * @n F. NULL�� �����Ѵ�.
  */
  virtual LPTSTR ReadyUniqueLocalName(LPCTSTR pathname);

  /**
  * @brief �̵� ó���ÿ� ������ �̸��� �ٸ� �׸��� �����Ͽ� �ӽ� �̸����� �ٲپ� ó���� ��ü����, ������ �۾����� ���� �̸����� �ٲپ� ���´�.
  * @return int : ���� �̸����� ������ �׸��� ī��Ʈ�� �����Ѵ�.
  * @details A. �� ��ü�� ��Ʈ ���� �׸��� �ƴϸ�,
  *     - �������� �ӽ� �̸����� �ٲ�� ������(IsServerTemporaryRenamed()),
  *       + KSyncCoreEngine::ChangeFolderName()���� ������ �̸����� �ٲٰ� �����ϸ�,
  *         + ī��Ʈ�� 1 �����ϰ�, RefreshServerState()���� ���� ���¸� ������Ʈ�ϰ�, �� ��ü�� �̸� mLocal.Filename�� �ٲٰ�, mpServerTempRenamed�� �����Ѵ�.
  *     - �������� �ӽ� �̸����� �ٲ�� ������(IsLocalTemporaryRenamed()),
  *       + KMoveFile()���� ������ �̸����� �ٲٰ� �����ϸ�,
  *         + ī��Ʈ�� 1 �����ϰ�, �� ��ü�� �̸� mLocal.Filename�� �ٲٰ�, mpLocalTempRenamed�� �����Ѵ�.
  *     - ī��Ʈ�� 0���� ũ��
  *       + mBaseFolder�� ������Ʈ�ϰ�, MoveMetafileAsRename()�� ȣ���Ͽ� ��Ÿ ������ ��θ��� ������Ʈ�ϰ�, 
  *       + UpdateChildItemPath()�� ȣ���Ͽ� ���� �׸��� ��ε� ������Ʈ�ϰ�,
  *       + StoreMetafile()���� ��Ÿ �����͸� �����Ѵ�.
  *     .
  * @n B. ���� �׸� ����
  *     - KSyncItem::RestoreTemporaryRenamed()�� ��� ȣ���Ѵ�.
  *     .
  * @n C. ī��Ʈ ���� �����Ѵ�.
  */
  virtual int RestoreTemporaryRenamed();

  /**
  * @brief �������� �̸� ������ ó���ϴ� �����Լ��̴�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ��θ�
  * @param str : ������ �� �̸�
  * @return int : �����ϸ� R_SUCCESS��, �����ϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. ������ ��θ��� �����صд�.
  * @n B. KSyncItem::SetLocalRename()���� �̸� ������ ó���Ѵ�.
  * @n C. ���� ����� �����ϸ� UpdateChildItemPath()�� ȣ���Ͽ� ���� �׸���� ��θ� ������Ʈ�Ѵ�.
  * @n D. ������� �����Ѵ�.
  */
  virtual int SetLocalRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief �������� �̸� ������ ó���ϴ� �����Լ��̴�.
  * @param baseFolder : ����ȭ ��Ʈ ���� ��θ�
  * @param str : ������ �� �̸�
  * @return int : �����ϸ� R_SUCCESS��, �����ϸ� ���� �ڵ带 �����Ѵ�.
  * @details A. ������ ��θ��� �����صд�.
  * @n B. KSyncItem::SetLocalRename()���� �̸� ������ ó���Ѵ�.
  * @n C. ���� ����� �����ϸ� UpdateChildItemPath()�� ȣ���Ͽ� ���� �׸���� ��θ� ������Ʈ�Ѵ�.
  * @n D. ������� �����Ѵ�.
  */
  virtual int SetServerRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief ���� �� ������ ���� ���� ��θ��� �����Ѵ�.
  * @param localPath : ������ ���� ���� ��θ�
  * @param serverPath : ������ ���� ���� ��θ�
  * @details A. ������ mpDummyLocalPath, mpDummyServerPath�� �����Ѵ�.
  * @n B. ������ ���ο� ��θ��� ���� �����Ѵ�.
  */
  void SetDummyClonePath(LPCTSTR localPath, LPCTSTR serverPath);

  /**
  * @brief �� ��ü�� ���� �� ������ ���� ���� ��θ�� ��ġ�ϴ��� Ȯ���Ѵ�.
  * @param localPath : ������ ���� ���� ��θ�
  * @param serverPath : ������ ���� ���� ��θ�
  * @return BOOL : �� ��ü�� ��ġ�ϸ� TRUE�� �����Ѵ�.
  * @details A. �� ��ü�� ���� ���� �����̸�,
  *     - ������ localPath, serverPath�� NULL�̸� TRUE�� �����Ѵ�.
  *     - localPath�� ������ ���� ���� ��ΰ� ��ȿ�� ��Ʈ������ �ٸ��� FALSE�� �����Ѵ�.
  *     - serverPath�� ������ ���� ���� ��ΰ� ��ȿ�� ��Ʈ������ �ٸ��� FALSE�� �����Ѵ�.
  *     - localPath�� ������ ���� ���� ��ΰ� ��� ��ȿ�� ��Ʈ���̸�,
  *       + �� ��ΰ� �ٸ��� FALSE�� �����Ѵ�.
  *     - serverPath�� ������ ���� ���� ��ΰ� ��� ��ȿ�� ��Ʈ���̸�,
  *       + �� ��ΰ� �ٸ��� FALSE�� �����Ѵ�.
  *     - TRUE�� �����Ѵ�.
  *     
  * @n B. FALSE�� �����Ѵ�.
  */
  BOOL IsMatchDummyClonePath(LPCTSTR localPath, LPCTSTR serverPath);

  /**
  * @brief �� ��ü�� ���� ��ü�� ��Ÿ �����͸� �����Ѵ�.
  * @return : �� ���� ��ü ������ ó���� �׸��� ���� ������ TRUE�� �����Ѵ�.
  * @details A. mChildItemIndex�� 0�̸� �ڽ��� ��Ÿ �����͸� �����Ѵ�.
  * @n B. mChildItemIndex�� 0���� ũ�� child �׸��� �ε����� �����Ѵ�.
  * @n C. ���� �׸��߿� ���ϵ鿡 ����
  *     - StoreMetafile()�� ȣ���Ͽ� ��Ÿ �����Ϳ� �����ϰ� ī��Ʈ�� 1 �����Ѵ�.
  *     - ī��Ʈ�� ������ ����(30)�� �Ǹ� TRUE�� �����Ѵ�.
  *     .
  * @n D. �� �̻� ó���� ���� �׸��� ������ FALSE�� �����Ѵ�.
  */
  BOOL ContinueStoreMetafile(KSyncRootStorage* storage);

  virtual void SetOverridePath(int callingDepth, KSyncRootStorage* s, LPCTSTR path);

  void CheckClearOverridePath(KSyncRootStorage* s);

#ifdef _DEBUG
  virtual void DumpSyncItems(int depth);
#endif

private:
#ifdef USE_BASE_FOLDER
  /** �� ������ ��� ��θ� */
  LPTSTR mBaseFolder;
#endif
  /** ���� �׸� ����Ʈ */
  KSyncItemArray mChildList;
  /** ������ ������ ��ĵ�� �� ���Ǵ� FindFirstFile �ڵ� */
  HANDLE mLocalScanHandle;
  /** ����ȭ ó�� �۾��ÿ� ó�� �Ϸ��� ���� �׸��� �ε��� */
  int mChildItemIndex;
  /** �浹 ó��â���� ���� ���� �׸��� ������ ��� */
  LPTSTR mpDummyLocalPath;
  /** �浹 ó��â���� ���� ���� �׸��� ������ ��� */
  LPTSTR mpDummyServerPath;

  friend class KSyncRootFolderItem;

};

#endif
