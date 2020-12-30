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
#define REFRESH_RENAMED 2 // 생성한 항목이 다른 이름으로 이름변경 되었는지 확인.
#define SET_SCAN_DONE 4 // 새로 추가된 항목은 스캔 완료된 것으로 설정
#define SET_SCAN_NEED 8

// flags on RemoveUnBackupedItems()
#define REMOVE_WITH_STORAGE 1

// mask on SetPhaseRecurse()
#define EXCLUDE_CHANGED_ITEM 1


class KSyncCoreBase;
class KSyncRootStorage;

/**
* @class KSyncFolderItem
* @brief 동기화 대상 항목의 폴더 항목 클래스
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
  * @brief Assign 연산자('=') 구현 함수.
  */
  KSyncFolderItem &operator=(KSyncFolderItem &item);

  /**
  * @brief 이 항목과 동일한 항목을 복제하여 리턴한다.
  */
  virtual KSyncItem *DuplicateItem();

  /**
  * @brief 이 항목의 종류(파일 또는 폴더)를 리턴한다.
  */
  virtual int IsFolder() { return 1; }

  /**
  * @brief 하위 항목의 갯수를 리턴한다. 하위의 하위는 포함하지 않는다.
  */
  inline int GetChildItemCount() { return (int) mChildList.size(); }
  /**
  * @brief 지정한 N번째 항목을 리턴한다.
  */
  inline KSyncItem* GetChildItem(int index) { return mChildList.at(index); }
  /**
  * @brief 하위 항목을 모두 해제한다.
  */
  inline void RemoveChilds() { mChildList.clear(); }

  /**
  * @brief 이 항목과 하위 항목 전체를 동기화 Enable한다.
  */
  virtual void SetEnableRecurse(BOOL e);

  /**
  * @brief 하위 항목중 동기화 Enable된 항목이 있으면 TRUE를 리턴한다.
  */
  virtual BOOL GetEnableChild();

  virtual BOOL IsExcludedSyncRecur();

  /**
  * @brief 충돌 결과값에 사용자 방향선택을 적용한다.
  * @param r : 사용자 선택 방향
  * @param flag : 플래그
  * @details A. 현재의 항목에 대한 충돌 결과값에 사용자 방향선택을 적용한다.
  * @n B. 플래그에 WITH_RECURSE가 있으면 Child 항목들에도 KSyncItem::SetConflictResultAction() 메소드를 재귀적으로 호출한다.
  */
  virtual void SetConflictResultAction(int r, int flag);

  /**
  * @brief 폴더의 서버측 fullPathIndex가 바뀌었는지 확인한다. fullPathIndex가 바뀌었으면 폴더의 전체 경로가 바뀌었음을 의미한다.
  * @return BOOL : 변경되었으면 TRUE를 리턴한다.
  * @details A. mServer의 pFullPathIndex가 NULL이고 mServerNew의 pFullPathIndex가 NULL이 아니면 새로 추가된 항목이니 FALSE를 리턴한다.
  * @n B. mServer와 mServerNew의 pFullPathIndex를 비교하여 다르면 TRUE를 리턴한다.
  */
  virtual BOOL IsFolderFullPathChanged();

  /**
  * @brief 서버측에서 특정 폴더의 하위 항목중에서 변동된 항목만 받아오는 기능 처리에서 대상 항목의 상태를 설정하는 가상함수.
  * @param s : 항목의 scan 상태(변동 없음, 삭제됨, 존재함)
  * @param flag : 플래그
  * @details A. KSyncItem::SetConflictResultAction()을 호출하여 이 항목에 대한 상태값을 설정한다.
  * @n B. 플래그에 WITH_RECURSE가 있으면 Child 항목들에도 KSyncItem::SetConflictResultAction() 메소드를 재귀적으로 호출한다.
  */
  virtual void SetScanServerFolderState(int s, int flag);
  /**
  * @brief SetScanServerFolderState()에서 설정된 상태값에 따라 서버측 상태값을 설정한다.
  * @details A. KSyncItem::SetServerStateByScanState()을 호출하여 이 항목에 대한 서버측 상태값을 설정한다.
  * @n B. Child 항목들에도 KSyncItem::SetServerStateByScanState() 메소드를 재귀적으로 호출한다.
  */
  virtual void SetServerStateByScanState();

  /**
  * @brief 상대 경로명을 설정한다.
  * @param pathname : 상대 경로명
  * @details A. KSyncItem::SetRelativePathName()을 호출하여 상대 경로명을 설정한다.
  * @n B. mBaseFolder를 클리어하고 여기에 경로명을 복사한다.
  */
  virtual void SetRelativePathName(LPCTSTR pathname);

  virtual void ClearRelativePathName(int flag);

  /**
  * @brief mBaseFolder로부터 상대 경로명을 업데이트한다.
  * @details A. KSyncItem::SetRelativePathName()을 호출하여 상대 경로명을 mBaseFolder로 설정한다.
  */
  virtual void UpdateRelativePath();

  /**
  * @brief 경로명을 지정한 path로 설정한다.
  * @details A. mBaseFolder를 클리어하고 여기에 path+현재 개체명(mLocal.Filename)을 복사한다.
  */
  virtual void SetLocalPath(LPCTSTR baseFolder, LPCTSTR rootFolder, int depth);

  /**
  * @brief 이 항목의 경로명이 바뀌었을때 호출되는 가상함수.
  * @details A. AllocTreePathName()를 호출하여 부모 폴더들로부터 경로명을 받아 mBaseFolder로 한다.
  * @n B. mBaseFolder가 변경되었으면 KSyncRootFolderItem::UpdateMetaFolderNameMapping()를 호출한다(2.0에서는 사용 안됨)
  */
  virtual void OnPathChanged();

  /**
  * @brief 이 항목의 경로명이 바뀌었을때 호출되는 가상함수.
  * @param folder : 상대 경로명
  * @param rootFolder : 동기화 루트 폴더의 경로명
  * @param depth : 동기화 루트 폴더로부터의 폴더 Depth
  * @param prev : 이전 경로
  * @param changed : 바뀐 경로
  * @param refreshNow : 서버 경로를 업데이트하고 DB도 바로 업데이트 함.
  * @details A. AllocTreePathName()를 호출하여 부모 폴더들로부터 경로명을 받아 mBaseFolder로 한다.
  * @n B. KSyncItem::OnPathChanged()를 호출한다.
  */
  virtual void OnPathChanged(LPCTSTR folder, LPCTSTR rootFolder, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow);

  /**
  * @brief 로컬측 상대 경로명을 받아간다.(deprecated)
  * @param path :  로컬측 상대 경로명을 저장할 버퍼 주소
  * @details A. mBaseFolder를 path에 저장한다.
  */
  //void GetLocalPath(LPTSTR path);

  /**
  * @brief 폴더의 이름이 바뀌거나 이동되었을때, 그 하위의 항목들도 모두 경로가 업데이트 되어야 하며, 이때 불리는 함수이다.
  * @param baseFolder : 동기화 루트 폴더의 경로명
  * @param depth : 동기화 루트 폴더로부터의 폴더 Depth
  * @param prev : 이전 경로
  * @param changed : 바뀐 경로
  * @param refreshNow : 서버 경로를 업데이트하고 DB도 바로 업데이트 함.
  * @details A. 하위 항목들에 대해 KSyncItem::OnPathChanged()를 재귀적으로 호출한다.
  * @n B. 하위 항목이 폴더이면 KSyncFolderItem::UpdateChildItemPath()를 재귀적으로 호출한다.
  * @n C. depth가 1이면(하위 항목들의 업데이트가 모두 끝났으면),
  *     - KSyncRootStorage::UpdateRenameParentFolder()를 호출하여 이름 변경 테이블에서 경로가 바뀐 내용을 적용한다.
  *     - KSyncCoreEngine::ShellNotifyIconChangeOverIcon()을 호출하여 탐색기에서 대상 항목의 아이콘이 업데이트 되도록 한다.
  */
  void UpdateChildItemPath(KSyncRootFolderItem *root, int depth, LPCTSTR prev, LPCTSTR changed, BOOL refreshNow);

  virtual int UpdateServerRenamedAs(LPCTSTR orgRelativePath, LPCTSTR changedRelativePath);

  // relative pathname, root is NULL
  /**
  * @brief 폴더 항목의 상대 경로명을 리턴한다. 동기화 루트 폴더에서는 NULL을 리턴한다.
  */
#ifdef USE_BASE_FOLDER
  inline TCHAR *GetBaseFolder() { return ((mParent == NULL) ? NULL : mBaseFolder); }
#else
  inline TCHAR *GetBaseFolder() { return ((mParent == NULL) ? NULL : mpRelativePath); }
#endif
  /**
  * @brief 폴더 항목의 상대 경로명을 리턴한다. 동기화 루트 폴더에서는 "\"을 리턴한다. 화면에 표시하기 위해 사용된다.
  */
  LPCTSTR GetBaseFolderName();

  /**
  * @brief 대상 항목을 폴더의 Child로 등록한다.
  */
  void AddChild(KSyncItem *item);

  /**
  * @brief 대상 항목을 폴더의 Child로 등록한다. after로 지정한 항목 다음에 추가한다.
  */
  void AppendChild(KSyncItem *after, KSyncItem *item);

  /**
  * @brief 특정 phase에 처리할 다음 항목을 찾아 리턴한다.
  * @details A. parentFirst가 TRUE 이면,
  *     - 이 항목의 mPhase가 지정한 phase보다 작고, type이 일치하면,
  *       + IsNeedSyncPhase()를 호출하여 이 항목에 해당 phase에 처리될 개체이면
  *         + 이 항목을 리턴하고,
  *       + 아니면 항목의 mPhase를 phase로 변경한다.
  *     .
  * @n B. 서버 상태가 동기화 권한이 없거나 동기화 오류 상태이면 NULL을 리턴한다.(Child 항목은 처리되지 않도록 하기 위함)
  * @n C. Child 항목들에 대해서 순서대로
  *     - 항목이 폴더이면
  *       + 항목의 mPhase가 PHASE_END_OF_JOB보다 적으면(항목이 열외로 설정되지 않았으면)
  *         + KSyncFolderItem::GetNextChild()를 재귀적으로 호출하고,
  *         + 리턴값이 NULL이 아니면 이를 리턴한다.
  *     - 항목이 폴더가 아니면
  *       + 항목의 mPhase가 지정한 phase보다 작고 type이 일치하면,
  *       + IsNeedSyncPhase()를 호출하여 이 항목에 해당 phase에 처리될 개체이면
  *         + 이 항목을 리턴하고,
  *       + 아니면 항목의 mPhase를 phase로 변경한다.
  *     .
  * @n D. parentFirst가 FALSE이면,
  *     - 이 항목의 mPhase가 지정한 phase보다 작고, type이 일치하면,
  *       + IsNeedSyncPhase()를 호출하여 이 항목에 해당 phase에 처리될 개체이면
  *         + 이 항목을 리턴하고,
  *       + 아니면 항목의 mPhase를 phase로 변경한다.
  *     .
  */
  KSyncItem *GetNextChild(int phase, int type, BOOL parentFirst = TRUE);

  // KSyncItem *GetNextPriorityChild(int phase, int type);

  /**
  * @brief 지정한 항목의 포인터를 찾아 리턴한다(존재하는지 확인한다).
  * @param item : 찾고자 하는 항목의 포인터
  * @return KSyncItem* : 찾은 항목의 포인터
  * @details A. 이 항목과 찾고자 하는 항목을 비교하여 같으면 이 항목을 리턴한다.
  * @n B. Child 항목들에 대해
  *     - child 항목과 찾고자 하는 항목을 비교하여 같으면 child 항목을 리턴한다.
  *     - child 항목이 폴더이면 해당 항목에 대해 KSyncFolderItem::FindItemExist()를 재귀적으로 호출하고 결과가 NULL이 아니면 그것을 리턴한다.
  */
  KSyncItem *FindItemExist(KSyncItem *item);

  /**
  * @brief 동기화 검색 단계에서 메타 폴더의 메타 파일을 검색한다. 2.0에서는 사용하지 않음.
  * @param storage : 동기화 루트 스토리지
  * @param metaFolder : 메타 폴더
  * @param p : 동기화 정책
  * @param rootFolder : 동기화 루트 폴더 경로
  * @param done : 이 폴더의 검색 완료 여부
  * @param flag : 플래그
  * @param rootP : 동기화 루트 폴더 항목
  * @details A. flag의 SCAN_FOR_ALL_OBJECT값에 따라 max_iteration값을 설정한다(기본 100).
  * @n B. mLocalScanHandle이 NULL이면(처음 검색을 시작한 경우),
  *     - 이 폴더의 전체 경로명에 "*.ifo"를 더하여 검색 파일명을 만들어 FindFirstFile()로 파일 검색 핸들을 생성하고 핸들이 NULL이면,
  *       + done = TRUE로 하고 루우프를 빠져나간다.
  *     .
  * @n C. 아니면 FindNextFile()로 다음 파일을 검색하고 결과가 실패하면,
  *     - mLocalScanHandle을 해제하고, done = TRUE로 하고 루우프를 빠져나간다.
  *     .
  * @n D. 찾은 파일 이름이 유효하고 ".root.ifo"이 아니면,
  *     - 유효한 메타 파일로 간주하고 KSyncItem을 생성하여 메타데이터를 읽어온다.
  *     - 생성된 항목을 child 목록에 추가한다.
  *     .
  * @n E. 찾은 항목의 갯수를 리턴한다.
  */
  int scanSubObjects(KSyncRootStorage* storage, LPCTSTR metaFolder, KSyncPolicy *p, LPCTSTR rootFolder, BOOL &done, int flag, KSyncFolderItem* rootP);

  /**
  * @brief 동기화 검색 단계에서 서버측 폴더에서 하위 항목을 검색한다.
  * @param p : 동기화 정책
  * @param rootP : 동기화 루트 폴더 항목
  * @details A. KSyncCoreEngine::StartScanFolder()를 호출하여 하위 항목에 대한 검색을 요청한다.
  */
  int scanSubFoldersServer(KSyncPolicy *p, KSyncRootFolderItem* rootP);

#ifndef SCAN_FOLDER_FILE_AT_ONCE
  /**
  * @brief 동기화 검색 단계에서 로컬측 폴더에서 하위 항목을 검색한다.
  * @param p : 동기화 정책
  * @param rootP : 동기화 루트 폴더 항목
  * @details A. 현재의 경로명 + "*" 로 파일 검색 마스크를 만든다.
  * @n B. FindFirstFile()함수로 파일 검색을 시작한다.
  * @n C. 검색 핸들이 NULL이 아니면,
  * @n D. 검색 결과 파일명이 유효하면
  *     - findMovedRenamedOrigin()으로 이동 및 이름 변경된 개체였는지 확인한다.
  *     - 찾은 개체가 없으면 findChildByName()으로 파일명이 동일한 개체를 확인한다.
  *     - 찾은 개체가 없으면 전체 경로명을 만들고,
  *       + 전체 경로명이 동기화 대상 영역에 포함되는지 IsPartialSyncOn()으로 확인하여 맞으면
  *         + 새 KSyncFolderItem 개체를 생성하고, child 목록에 추가한다.
  *     - 찾은 개체가 있으면 검색 결과의 마지막 수정시간 마지막 액세스 시간을 mLocalNew에 업데이트하고 로컬 상태에 ITEM_STATE_NOW_EXIST를 더한다.
  *     .
  * @n E. FindNextFile()로 다음 파일을 검색하고 성공하면 D로 이동한다.
  * @n F. SetPhaseExcludedItems() 함수를 호출하여 제외되는 항목들의 phase를 PHASE_MOVE_FOLDERS+1로 설정한다.
  */
  int scanSubFoldersLocal(KSyncPolicy *p, KSyncRootFolderItem* rootP);
#endif // SCAN_FOLDER_FILE_AT_ONCE

  /**
  * @brief 하위 항목 중에서 폴더가 아닌 문서 항목은 모두 목록에서 제거하고 삭제한다.
  * @return int : 제거된 갯수를 리턴한다.
  * @details A. 하위 전체 항목에 대해,
  * @n B. 하위 항목이 폴더이면 넘어가고,
  * @n C. 파일이면 목록에서 제거하고 해당 항목을 해제한다.
  * @n D. 제거된 갯수를 리턴한다.
  */
  int ClearSubFiles();

  /**
  * @brief 하위 항목 중에서 동기화 제외 폴더 항목의 phase를 지정한 phase로 설정한다.
  * @param bRecurr : TRUE이면 하위 폴더에 대해 재귀 호출을 한다.
  * @param phase : 설정할 phase값.
  * @details A. 하위 전체 항목에 대해,
  *     - KSyncItem::IsExcludedSync()을 호출하여 그 항목이 제외되었으면,
  *       + 하위 항목의 KSyncItem::SetPhase()로 KSyncItem::mPhase를 설정한다.
  *     - 제외되지 않았고 bRecurr가 TRUE이면,
  *       + 하위 항목의 KSyncFolderItem::SetPhaseExcludedItems()를 재귀적으로 호출한다..
  *     .
  * @n B. 하위 항목 중에서 제외된 항목이 있으면 동기화 플래그에 SYNC_ITEM_HAS_EXCLUDED를 더한다.
  */
  void SetPhaseExcludedItems(BOOL bRecurr, int phase);

  /**
  * @brief 하위 항목 중에서 동기화 제외 폴더 항목을 찾아 제거하고 삭제한다.
  * @n 동기화 검색 단계에서 포함된 제외 항목들이 더이상 처리되지 않도록 목록에서 제거되어야 할 때 호출된다.
  * @param root : 동기화 루트 폴더 항목.
  * @param bRecurr : TRUE이면 하위 폴더에 대해 재귀 호출을 한다.
  * @details A. 하위 전체 항목에 대해,
  *     - KSyncItem::IsExcludedSync()을 호출하여 그 항목이 제외되었으면,
  *       + 하위 목록에서 제거하고 항목을 삭제한다.
  *     - 제외되지 않았고 bRecurr가 TRUE이면,
  *       + 하위 항목의 KSyncFolderItem::RemoveExcludedItems()를 재귀적으로 호출한다..
  *     .
  * @n B. 하위 항목 중에서 제외된 항목이 있으면 동기화 플래그에 SYNC_ITEM_HAS_EXCLUDED를 더한다.
  */
  void RemoveExcludedItems(KSyncFolderItem* root, BOOL bRecurr);

  /**
  * @brief 변동 목록에 포함되지 않은 하위 항목은 제거한다.
  */
  void RemoveOutOfModifiedList(KVoidPtrArray& modifiedList);

  /**
  * @brief 하위 항목들에 대한 동기화 메타 데이터를 삭제한다.
  * @n 동기화 하위 폴더가 삭제되었을때 하위 항목도 모두 제거하기 위해 호출된다.
  * @param baseFolder : 동기화 루트 폴더.
  * @details A. 하위 전체 항목에 대해,
  *     - 폴더 항목이면,
  *       + 하위 항목의 KSyncFolderItem::RetireChildObjects()를 재귀적으로 호출한다..
  *     - 제외되지 않았고 bRecurr가 TRUE이면,
  *     .
  * @n B. 하위 항목 중에서 제외된 항목이 있으면 동기화 플래그에 SYNC_ITEM_HAS_EXCLUDED를 더한다.
  */
  void RetireChildObjects(LPCTSTR baseFolder);

  void RetireChildObjectsRecurr(KSyncRootStorage* s, LPCTSTR baseFolder);

  /**
  * @brief 동기화 정책에서 동기화 제외 형식에 해당하는 파일들을 동기화 작업에서 예외가 되도록 phase를 설정한다.
  * @n 로컬 및 서버측 폴더가 검색된 후에 동기화 제외 형식에 해당하는 항목들을 골라내기 위해 호출된다.
  * @param engine : 동기화 Engine.
  * @param root : 동기화 루트 폴더
  * @param restorePhase : 복구할 phase
  * @details A. 하위 전체 항목에 대해,
  *     - 항목의 이름이 KSyncCoreEngine::IsExcludeFileMask() 또는 KSyncPolicy::IsExcludeFileMask()에 해당하면 ex에 1을 넣는다.
  *     - 항목이 로컬 이름 변경되어 있으면,
  *       + 이름 변경된 이름에 대해 KSyncCoreEngine::IsExcludeFileMask() 또는 KSyncPolicy::IsExcludeFileMask()에 해당하면,
  *         + ex에 2를 더한다.
  *       + 아니고 (ex & 1)이면,(제외 형식에 해당되었으나 이름 변경으로 제외 형식이 아닌 경우)
  *         + 이름 변경된 파일 이름을 UpdateRenameLocal()로 이름을 변경하고,
  *         + phase를 restorePhase로 돌려놓는다.
  *     - 항목이 서버 이름 변경되어 있으면,
  *       + 이름 변경된 이름에 대해 KSyncCoreEngine::IsExcludeFileMask() 또는 KSyncPolicy::IsExcludeFileMask()에 해당하면,
  *         + ex에 4를 더한다.
  *     - ex가 0이 아니면,
  *       + 로그 메시지를 작성하여 저장하고
  *       + (ex & 2)이면
  *         + 로컬의 이름 변경된 이름을 UpdateRenameLocal()로 이름을 변경하고 메타 스토리지에 저장한다
  *       + (ex & 4)이면
  *         + StoreMetafile()로 메타 스토리지에 저장한다
  *       + 항목의 phase를 PHASE_END_OF_JOB로 설정한다(동기화에서 열외로 설정한다)
  *     .
  * @n B. 하위 항목 중에서 제외된 항목이 있으면 동기화 플래그에 SYNC_ITEM_HAS_EXCLUDED를 더한다.
  */
  void RemoveExcludeNamedItems(KSyncCoreBase *engine, KSyncRootFolderItem *root, int restorePhase);

  /**
  * @brief 동기화 정책에서 동기화 제외 형식에 해당하는 항목들을 동기화 작업에서 예외가 되도록 Disable 설정한다.
  * @param root : 동기화 루트 폴더 항목.
  * @return int : Disable된 항목 갯수를 리턴한다.
  * @details A. 이 개체가 루트 폴더가 아니면,
  *     - 지금 Enable상태이고, KSyncRootFolderItem::IsExcludeFileMask()로 제외 형식 마스크에 해당하면
  *       + Enable을 FALSE로 하고 카운트를 하나 증가한다.
  *     .
  * @n B. 하위 전체 항목에 대해,
  *     - 항목이 폴더이면
  *       + 하위 항목에 대해 KSyncFolderItem::DisableExcludedFileMask()를 재귀적으로 호출하고 리턴값을 카운트에 더한다.
  *     - 항목이 폴더가 아니면
  *       + 지금 Enable상태이고, KSyncRootFolderItem::IsExcludeFileMask()로 제외 형식 마스크에 해당하면
  *         + Enable을 FALSE로 하고 카운트를 하나 증가한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */
  int DisableExcludedFileMask(KSyncRootFolderItem* root);

  /**
  * @brief 서버측에 존재하지 않는 폴더는 지정한 phase로 설정한다.
  * @param phase : 설정할 phase.
  * @details A. 하위 전체 항목에 대해,
  *     - 항목이 폴더이고 KSyncItem::IsServerNotExist()로 서버에 존재하지 않으면
  *       + 항목의 phase를 지정한 phase로 설정하고,
  *       + KSyncFolderItem::SetPhaseServerNotExistFolders()를 재귀적으로 호출한다.
  *     .
  */
  void SetPhaseServerNotExistFolders(int phase);

  /**
  * @brief 서버측 위치가 이동된 폴더 항목의 phase를 바꾼다. 서버의 변경된 항목만 받아오는 부분에서 사용한다.
  * @n OnEndScanFolderItem 이후에 실행됨.
  * @n phase가 바뀐 항목은 끝 노드까지 모두 업데이트되니 recursive할 필요 없다.
  * @param phase : 이동된 폴더에 적용할 phase.
  * @param default_phase : 기본 phase.
  * @details A. 하위 전체 항목에 대해,
  *     - 항목의 phase를 기본 phase로 설정한다.
  *     - 항목이 폴더인 경우에,
  *       + 폴더의 fullPathIndex가 바뀌었고, 하위에 폴더 항목의 갯수가 0보다 큰 경우에,
  *         + phase를 적용할 phase로 설정한다.
  *       + 아니면 KSyncFolderItem::SetPhaseForMovedFoldersInServer()를 재귀적으로 호출한다.
  *     .
  */
  void SetPhaseForMovedFoldersInServer(int phase, int default_phase);

  /**
  * @brief 서버측 위치가 지정 영역의 바깥-안으로 이동된 항목의 개수를 리턴한다.
  */
  virtual int GetCountForMoveInOutOnServer(LPCTSTR partialPath, int path_len = -1);

#ifndef USE_SCAN_BY_HTTP_REQUEST
  /**
  * @brief 서버측에서 현재 폴더 하위의 문서 항목을 조회하여 KSyncItem으로 만들어 child 개체로 추가한다.(2.0부터 사용 안함)
  */
  int scanSubFilesServer(KSyncPolicy *p, KSyncFolderItem* rootP, BOOL &done);
#endif // USE_SCAN_BY_HTTP_REQUEST

  /**
  * @brief 로컬측에서 스캔된 파일 중 스킵되어야 할 파일이면 TRUE를 리턴한다.
  */
  static BOOL IsLocalScanSkipFile(KSyncFolderItem* root, LPCTSTR cFileName, BOOL isNewItem);

  /**
  * @brief 로컬측에서 현재 폴더 하위의 항목을 검색하여 KSyncFolderItem 또는 KSyncItem으로 만들어 child 개체로 추가한다.
  * @param rootP : 동기화 루트 폴더 항목
  * @param folder_count : 찾은 폴더 개수
  * @param done : TRUE이면 검색 완료
  * @return int : 찾은 파일 개수
  */
  int scanSubFilesLocal(KSyncFolderItem* rootP, int& folder_count, BOOL &done);

  /**
  * @brief 서버측 정보를 클리어하고 하위의 폴더들에 대해 재귀 호출을 한다.
  * @details A. clearServerSideInfo()를 호출하여 서버측 정보를 클리어하고
  * @n B. 하위 전체 항목에 대해,
  *     - 항목이 폴더이면
  *       + KSyncFolderItem::ClearServerSideInfoCascade()를 재귀적으로 호출한다.
  *     .
  */
  void ClearServerSideInfoCascade();

  /**
  * @brief 주어진 파일명에 대해 이동 및 이름 변경된 원래 개체를 찾는다.
  * @param name : 파일명
  * @return KSyncItem* : 원 개체의 포인터
  * @details A. GetActualPathName()를 호출하여 이 폴더의 경로를 받아온다.
  * @n B. 위에서 받은 경로에 지정한 파일명을 붙인다.
  * @n C. KSyncRootFolderItem::GetMovedRenamedOriginal()으로 원래 개체를 받아온다.
  * @n D. 위에서 받은 개체가 NULL이 아니면 KSyncItem::CheckLocalRenamedAs()를 호출하여 이름 변경을 확인한다.
  */
  KSyncItem *findMovedRenamedOrigin(LPCTSTR name);

  /**
  * @brief 대상 항목을 주어진 경로의 폴더의 child 개체로 부모관계를 설정한다.
  * @param item : 대상 항목
  * @param relativePathName : 이동할 상대 경로명
  * @param pathHasName : 경로명에 자신의 폴더명이 포함되어 있음.
  * @return BOOL : 성공하면 TRUE를 리턴한다.
  * @details A. 대상 항목의 현재의 부모 개체의 포인터를 임시 저장한다.
  * @n B. relativePathName 가 유효하면
  *     - FindChildItemCascade()를 호출하여 relativePathName에 해당하는 폴더 개체를 찾아온다.
  *     .
  * @n C. 아니면 루트 폴더 항목을 이동할 폴더로 한다.
  * @n D. 이동할 목적지 개체가 유효하면,
  *     - 이전 부모 개체에서 RemoveItem()를 호출하여 대상 항목을 분리하고,
  *     - 새 부모 개체에 AddChild()를 호출하여 child 개체로 추가한다.
  *     - 대상 항목이 폴더이면 항목의 KSyncFolderItem::OnPathChanged()를 호출하여 경로가 바뀐것을 업데이트한다.
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
  * @brief 폴더 내의 하위 파일들을 동기화 비교하여 처리 방법을 결정한다.
  * @param baseFolder : 동기화 루트 폴더 경로명
  * @param p : 동기화 정책
  * @param count : 처리된 갯수
  * @param phase : 동기화 phase
  * @return BOOL : 비교가 완료되었으면 TRUE를 리턴한다.(하위 항목의 갯수가 많은 경우 정한 갯수만큼씩 처리한다)
  * @details A. 하위 항목 처리 스텝(mChildItemIndex)이 0이고, 이 항목이 동기화 루트 폴더가 아니면,
  *     - 서버 상태가 지금 존재하면(ITEM_STATE_NOW_EXIST)
  *       + 서버측 권한에서 하위 항목 조회 권한이 없으면,
  *         + 서버측 상태에 ITEM_STATE_NO_PERMISSION를 더한다.
  *         + 결과를 FILE_SYNC_NO_FOLDER_PERMISSION로 설정한다.
  *         + mChildItemIndex를 1 증가하고 TRUE를 리턴한다.
  *     .
  * @n B. 처리 스텝(mChildItemIndex)이 0이면,
  *     - compareSyncFileItem()를 호출하여 이 개체의 로컬측 및 서버측 정보 비교를 행한다.
  *     - removeEndOfJobObject()를 호출하여 동기화 제외되는 항목은 child 목록에서 제거한다.
  *     .
  * @n C. 처리 스텝(mChildItemIndex)이 child 갯수보다 크거나 같으면 TRUE를 리턴한다.
  * @n D. 전체 child 항목에 대해
  *     - 하위 항목이 폴더가 아니면,
  *       + 하위 항목의 mPhase가 지정한 phase보다 작으면 ,
  *         + KSyncItem::compareSyncFileItem()를 호출하여 비교 처리를 하고
  *         + KSyncItem::SetPhase()를 호출하여 하위 항목의 mPhase를 phase로 설정한다.
  *         + count를 1 증가한다.
  *     - mChildItemIndex를 1 증가한다.
  *     - count가 정한 갯수보다 작으면 다음 하위 항목을 처리한다.
  *     - 아니면 done = FALSE로 하고 빠져나간다.
  *     .
  * @n E. GetTotalSize()로 하위 항목들의 크기를 mLocal.FileSize에 저장한다.
  */
  int compareFiles(/*LPCTSTR baseFolder, KSyncPolicy *p*/KSyncRootFolderItem* root, int &count, int phase);

  /**
  * @brief child 항목에서 항목의 동기화 비교 결과가 0이면 대상 항목을 제거한다.
  * @param phase : 동기화 phase
  * @return int : child 항목의 개수를 리턴한다.
  * @details A. 본 항목의 phase를 지정한 phase로 설정한다.
  * @n B. 하위 폴더들에 대해 KSyncFolderItem::RemoveUnchanged()를 재귀적으로 호출한다.
  * @n C. 하위 파일들에서 항목의 결과값이 0이면 RemoveItem() 메소드로 해당 항목을 제거하고 삭제한다.
  * @n D. 아니면 phase를 지정한 phase로 설정한다.
  * @n E. 남은 child항목 개수를 리턴한다.
  */
  int RemoveUnchanged(int phase);

  /**
  * @brief child 항목에서 항목의 충돌 결과값이 0이면 대상 항목을 제거한다.
  * @return int : child 항목의 개수를 리턴한다.
  * @details A. 본 항목의 phase를 PHASE_CLEAN_UNCONFLICT로 설정한다.
  * @n B. 하위 폴더들에 대해 KSyncFolderItem::RemoveUnconflictItem()를 재귀적으로 호출한다.
  * @n C. 하위 항목들에서 충돌 결과값이 0이면 RemoveItem() 메소드로 해당 항목을 제거하고 삭제한다.
  * @n D. 아니면 phase를 지정한 PHASE_CLEAN_UNCONFLICT로 설정한다.
  * @n E. 남은 child항목 개수를 리턴한다.
  */
  int RemoveUnconflictItem();

  /**
  * @brief child 항목에서 항목의 phase가 PHASE_END_OF_JOB이면 대상 항목을 제거한다.
  * @param rootFolder : 동기화 루트 폴더 경로명
  * @details A. 하위 전체 항목에 대해,
  *     - 항목의 mPhase가 PHASE_END_OF_JOB이면
  *       + KSyncItem::IsExcludedSync()을 호출하여 항목이 제외 파일이 아니면()
  *         + StoreMetafile()을 호출하여 메타 데이터를 삭제한다.
  */
  void removeEndOfJobObject();

  /**
  * @brief 항목의 mPhase를 지정한 값으로 설정하는 가상 함수.
  * @param p : 지정한 phase
  * @details A. KSyncItem::SetPhase()를 호출하여 mPhase를 설정한다.
  * @n B. Child 항목 처리 스텝(mChildItemIndex)를 0으로 설정한다.
  */
  virtual void SetPhase(int p);

  /**
  * @brief 하위 항목을 포함하여 항목의 mPhase를 지정한 값으로 설정한다.
  * @param p : 지정한 phase
  * @param mask : EXCLUDE_CHANGED_ITEM : 처리 결과가 있는 항목은 제외한다.
  * @details A. SetPhase()를 호출하여 mPhase를 설정한다.
  * @n B. 하위 항목들에 대해,
  *     - 하위 항목이 폴더이면 KSyncFolderItem::SetPhaseRecurse()를 재귀적으로 호출한다.
  *     - 아니면 하위 항목에 대해 SetPhase()를 호출한다.
  */
  void SetPhaseRecurse(int p, int mask);

  int SetPhaseRetryRecurse(int p, int mask);

  /**
  * @brief 이 항목과 상위 항목의 mPhase를 지정한 값으로 설정한다.
  * @param p : 지정한 phase
  * @details A. SetPhase()를 호출하여 mPhase를 설정한다.
  * @n B. 부모 항목이 존재하면 부모 항목에 대해, KSyncFolderItem::SetPhaseRevRecurse()를 재귀적으로 호출한다.
  */
  void SetPhaseRevRecurse(int p);

  /**
  * @brief 하위 항목을 포함하여 강제로 항목의 mPhase를 지정한 값으로 설정한다. KSyncItem::SetPhase()에서는 phase값이 감소하지 않는다.
  * @n 이 함수에서는 조건없이 강제로 지정한 phase 값으로 설정한다.
  * @param p : 지정한 phase
  * @details A. SetPhaseForce()를 호출하여 mPhase를 설정한다.
  * @n B. 하위 항목들에 대해,
  *     - 하위 항목이 폴더이면 KSyncFolderItem::SetPhaseForceRecurse()를 재귀적으로 호출한다.
  *     - 아니면 하위 항목에 대해 SetPhaseForce()를 호출한다.
  */
  void SetPhaseForceRecurse(int p);
  
  /**
  * @brief 하위 항목을 포함하여 동기화 결과값과 phase값을 지정한 값으로 설정한다.
  * @param result : 지정한 결과값.
  * @param phase : 지정한 phase값
  * @param flag : 플래그(SET_WITH_ITSELF, EXCLUDE_OUTGOING, SET_SERVER_DELETED, SET_LOCAL_DELETED)
  * @details A. 플래그에 SET_WITH_ITSELF가 있으면,
  *     - 이 항목의 동기화 결과값을 result로 설정하고,
  *     - 이 항목의 phase값을 설정한다.
  *     .
  * @n B. 하위 항목들에 대해
  *     - 플래그에 EXCLUDE_OUTGOING이 없거나, IsOutGoingChild()가 FALSE이면,
  *       + 항목이 폴더이면 KSyncFolderItem::SetConflictResultPhase()를 재귀적으로 호출한다.
  *       + 아니면 이 항목의 동기화 결과값과 phase값을 지정한 값으로 설정한다.
  *       + 플래그에 SET_SERVER_DELETED가 있으면 clearServerSideInfo()를 호출하여 서버측 정보를 클리어한다.
  *       + 플래그에 SET_LOCAL_DELETED가 있으면 clearLocalSideInfo()를 호출하여 서버측 정보를 클리어한다.
  *     .
  */
  virtual void SetConflictResultRecurse(int result, int phase, int flag);

  /**
  * @brief 하위 항목중에서 백업 대상 항목이 아닌 항목은 제거한다.
  * @details A. 하위 항목의 폴더 항목에서, 대상 경로가 KSyncCoreBase::IsTargetDrive()에 해당하지 않으면,
  * @n 대상 항목과 그 하위 항목의 결과를 FILE_SYNC_NOT_EXIST_DRIVE로, phase를 PHASE_END_OF_JOB로 설정한다.
  */
  int RemoveUnBackupedItems(KSyncRootFolderItem *root, int flag);

  /**
  * @brief 하위 항목 중에서 지정한 서버 OID를 가진 항목을 찾아 리턴한다.
  * @param serverOID : 지정한 서버 OID.
  * @return KSyncItem* : 지정한 서버 OID를 가진 항목의 포인터.
  * @details A. 현재 항목의 서버 OID와 지정한 OID를 먼저 비교하여 일치하면 이 개체의 포인터를 리턴한다.
  * @n B. 하위 항목들에 대해, 항목의 서버 OID와 지정한 OID를 먼저 비교하여 일치하면 이 개체의 포인터를 리턴한다.
  * @n C. 일치하는 항목이 없으면 동기화 루트 폴더에서부터 다시 찾기 위해 KSyncRootFolderItem::FindChildItemByOIDCascade()를 호출하여 결과값을 리턴한다.
  */
  KSyncItem *findItemByOID(LPCTSTR serverOID);

  /**
  * @brief 하위 항목 중에서 지정한 이름을 가진 항목을 찾아 리턴한다.
  * @param fileName : 찾을 이름.
  * @return KSyncItem* : 지정한 이름을 가진 항목의 포인터.
  * @details A. 하위 항목들에 대해, 항목의 이름과 지정한 이름을 비교하여 일치하면 해당 개체의 포인터를 리턴한다.
  */
  KSyncItem *findChildByName(LPCTSTR fileName);

  /**
  * @brief 지정한 하위 항목을 child 목록에 머지하거나 추가한다.
  * @param item : 머지할 하위 항목.
  * @return BOOL : 머지되었으면 TRUE를 리턴한다.
  * @details A. findChildByName() 메소드로 동일한 이름을 가지고 있는 하위 항목이 존재하는지 확인하고,
  *     - 존재하면
  *       + 해당 항목에 KSyncItem::MergeItem()으로 머지한다.
  *       + item은 삭제하고 TRUE를 리턴한다.
  *     - 존재하지 않으면,
  *       + AddChild() 메소드로 Child 항목으로 추가하고 FALSE를 리턴한다.
  */
  BOOL MergeChild(KSyncItem *item);

  /**
  * @brief 하위 항목들을 대상 폴더 항목 아래로 이동한다.
  * @param dest : 이동할 대상 폴더 항목.
  * @return int : 이동한 항목 갯수.
  * @details A. 하위 항목들에 대해,
  *     - 현재 폴더의 child 목록에서 제거하고
  *     - 대상 폴더의 MergeChild()메소드를 호출하여 하위 항목으로 추가한다.
  *     .
  * @n B. child 목록을 모드 클리어한다.
  * @n C. 카운트 값을 리턴한다.
  */
  int MoveChildItems(KSyncFolderItem *dest);

  /**
  * @brief 하위 항목들에서 대상 항목을 찾아 child 목록에서 제거한다.
  * @param item : 찾을 대상 항목의 포인터.
  * @return BOOL : 찾았으면 TRUE를 리턴한다.
  * @details A. 하위 항목들에 대해,
  *     - 찾는 항목과 일치하면 child 목록에서 제거하고 TRUE를 리턴한다.
  *     - 아니고 하위 항목이 폴더이면
  *       + KSyncFolderItem::RemoveItem()을 재귀적으로 호출하고 그 리턴값이 TRUE이면 TRUE를 리턴한다.
  *     .
  * @n B. FALSE를 리턴한다.
  */
  BOOL RemoveItem(KSyncItem *item);

  /**
  * @brief 하위 항목들 전체를 해제한다.
  * @details A. 하위 항목들에 대해,
  *     - child 목록에서 제거하고
  *     - 하위 항목이 폴더이면
  *       + KSyncFolderItem::FreeChilds()을 재귀적으로 호출한다.
  *     - 하위 항목을 삭제한다.
  */
  void FreeChilds();

  /**
  * @brief 이 항목이 지금 존재하는지 확인한다.
  * @param fullname : 폴더의 전체 경로명
  * @details A. IsDirectoryExist() 함수로 폴더가 존재하는지 확인하고 결과를 리턴한다.
  */
  virtual BOOL IsItemExist(LPCTSTR fullname);

  /**
  * @brief 이 항목의 동기화 비교 작업을 수행한다.
  * @param baseFolder : 동기화 루트 폴더 경로명
  * @details A. 동기화 루트 폴더이면 R_SUCCESS을 리턴한다.
  * @n B. checkAddedRenamedSyncError()를 호출하여, 새로 추가되거나 이동된 항목을 처리할때, 동일한 이름의 파일이 존재하는지 검사하고 존재하면 충돌 상태가 되도록 설정한다.
  * @n C. 로컬 상태가 ITEM_STATE_NOW_EXIST이면 로컬 상태값에 ITEM_STATE_NEWLY_ADDED를 더한다.
  * @n D. 서버 상태가 ITEM_STATE_NOW_EXIST이면 서버 상태값에 ITEM_STATE_NEWLY_ADDED를 더한다.
  * @n E. 로컬 상태가 ITEM_STATE_WAS_EXIST이면 로컬 상태값에 ITEM_STATE_DELETED를 더한다.
  * @n F. 서버 상태가 ITEM_STATE_WAS_EXIST이면,
  *     - 서버측 상태를 업데이트하여 권한이 0이면 결과값을 FILE_SYNC_NO_FOLDER_PERMISSION로 하고,
  *     - 아니면 서버 상태값에 ITEM_STATE_DELETED를 더한다.
  *     .
  * @n G. 로컬 상태가 새로 추가됨이면,
  *     - StoreMetafile()을 호출하여 메타 데이터 저장한다.
  *     - 폴더이고 KSyncRootFolderItem::IsSyncRootFolder()을 호출하여 다른 동기화 루트폴더이면,
  *       + 결과를 FILE_RELEASE_PREV_SYNC_ROOT로 한다.
  *     - 아니고 로컬측이 존재하지 않으면
  *       + 결과를 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED로 한다.
  *     .
  * @n H. 로컬 상태가 삭제됨이면,
  *     - 로컬측 이름 변경 정보가 있으면 해제한다.
  *     .
  * @n I. 서버 상태가 삭제됨이면,
  *     - 서버측 이름 변경 정보가 있으면 해제한다.
  *     .
  * @n J. 로컬측이 이름 변경 또는 이동되었으면,
  *     - 로컬 상태값에 ITEM_STATE_MOVED를 더한다.
  *     .
  * @n K. 서버측이 이름 변경 또는 이동되었으면,
  *     - 서버 상태값에 ITEM_STATE_MOVED를 더한다.
  *     .
  * @n L. 결과값이 0이면
  *     - makeResultFromState()를 호출하여 로컬측 서버측 상태값에 따른 결과값을 만든다.
  *     - 결과값이 충돌이나 오류가 아니면,
  *       + 결과값에 FILE_NEED_RETIRED가 없으면,
  *         + 결과 경로명을 만들고 그 길이가 MAX_FOLDER_PATH_LENGTH보다 크면
  *           + 로컬측 정보를 업데이트하고
  *           + 결과값을 FILE_SYNC_OVER_PATH_LENGTH로 한다.
  *     - 결과값이 업로드이면,
  *       + 서버 상태가 ITEM_STATE_NOW_EXIST이면 결과값을 0으로 한다.
  *     - 충돌 결과값이 FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED이면, 충돌이 아닌것으로 처리하도록 충돌값을 지운다.
  *     .
  * @n M. 결과값에 대한 로그를 저장하고 SetResult로 결과값을 저장한다.
  * @n N. 결과값에서 충돌 및 오류값을 충돌 결과값으로 mConflictResult로 저장한다.
  * @n O. StoreMetafile()로 결과값, 충돌 결과값을 저장한다.
  */
  virtual int compareSyncFileItem(LPCTSTR baseFolder, int syncMethod);

  /**
  * @brief 이 항목 및 하위 항목의 상태에 따라 phase값을 변경한다.
  * @return void
  * @param added_phase : 새로 추가된 항목이면 이 phase값을 적용한다.
  * @param renamed_phase : 이름 변경/이동된 항목이면 이 phase값을 적용한다.
  * @param normal_phase : 정상 항목이면 이 phase값을 적용한다.
  * @param parentCheckNeed : Child 항목이 변경되었으면 Parent 항목도 맞추어 변경한다.
  * @details A. 베이스 함수 KSyncItem::SetUnchangedItemPhase()를 호출한다.
  * @n B. 이 개체의 mPhase가 renamed_phase이고, 서버 상태가 ITEM_STATE_LOCATION_CHANGED이면, 
  * StoreMetafile()을 호출하여 파일명과 서버측 fullPathIndex를 저장한다.
  * @n C. 하위 항목들에 대해,
  *     - KSyncItem::SetUnchangedItemPhase()를 재귀적으로 호출한다.
  *     .
  * @n D. parentCheckNeed가 TRUE이면, 
  *     - hasChangedChild()가 TRUE이면 KSyncItem::SetUnchangedItemPhase(added_phase-1, renamed_phase-1, normal_phase-1, FALSE)를 호출하여
  * 이 개체가 다시 체크되도록 한다.
  */
  virtual void SetUnchangedItemPhase(int added_phase, int renamed_phase, int normal_phase, BOOL parentCheckNeed);


  void SetCascadedItemFlag(LPCTSTR pathName, int flag);
  int CheckMoveTargetFolders(KSyncFolderItem* rootItem, int syncMethod);
  int RemoveUnreferencedFolders(KSyncRootFolderItem *root, int syncMethod);

  /**
  * @brief 동기화 충돌 처리과정에서 이 항목 및 하위 항목의 상태에 따라 phase값을 변경한다.
  * @return BOOL : Enable되어 있으면 TRUE를 리턴한다.
  * @param parent_phase : 폴더 항목에 이 phase값을 적용한다.
  * @param renamed_phase : 이름 변경된 항목에 이 phase값을 적용한다.
  * @param normal_phase : 기타 다른 항목에 이 phase값을 적용한다.
  * @details A. 하위 항목들에 대해,
  *     - KSyncItem::SetPhaseOnResolve()를 재귀적으로 호출하고, TRUE가 리턴되면 enable을 TRUE로 한다.
  *     .
  * @n B. 이 개체가 Enable 상태이면 normal_phase를 적용한다.
  * @n C. 아니고 
  *     - 하위 항목이 enable이면 parent_phase를 적용하고,
  *     - 아니면 PHASE_END_OF_JOB을 리턴한다.
  *     .
  * @n D. mEnable이나 enable이 TRUE이면 TRUE를 리턴한다.
  */
  virtual BOOL SetPhaseOnResolve(int parent_phase, int renamed_phase, int normal_phase);

  /**
  * @brief 하위 항목중에서 결과값이 0이 아닌 항목이 있으면 TRUE를 리턴한다. 재귀 호출을 하지 않는다.
  * @return BOOL : 변동된 하위 항목이 있으면 TRUE를 리턴한다.
  * @details A. 하위 항목들에 대해,
  *     - 결과값이 FILE_NOT_CHANGED(0)이 아니면 TRUE를 리턴한다.
  */
  BOOL hasChangedChild();

  /**
  * @brief 하위 항목중에서 결과값이 0인 항목의 개수를 리턴한다. 재귀 호출을 하여 전체 하위 항목을 검사한다.
  * @param type : 대상 항목의 종류(1:폴더, 0:파일, 2:모두)
  * @return int : 변동되지 않은 하위 항목의 갯수.
  * @details A. 이 개체의 결과값이 FILE_NOT_CHANGED이고, 타입이 일치하면 카운트를 1 증가한다.
  * @n B. 하위 항목들에 대해,
  *     - 폴더이면 KSyncFolderItem::GetUnchangedItemCount()를 재귀 호출하고 리턴값을 카운트에 더한다.
  *     - 폴더가 아니면, 개체의 결과값이 FILE_NOT_CHANGED이고, 타입이 일치하면 카운트를 1 증가한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */
  int GetUnchangedItemCount(int type);

  /**
  * @brief 서버측 폴더가 삭제되었으면 생성한다
  * @param baseFolder : 동기화 루트 폴더 경로명
  * @return BOOL : 성공하면 TRUE를 리턴한다.
  * @details A. 부모 폴더가 NULL이면(동기화 루트 폴더), TRUE를 리턴한다.
  * @n B. 부모 폴더에 대해 CreateServerFolderIfDeleted()를 호출하여 먼저 상위 폴더가 처리되도록 한다.
  * @n C. 서버측 pullPathIndex가 NULL이면 createServerFolder()를 호출하여 폴더를 생성하고,
  *     - 성공하면 서버 상태를 ITEM_STATE_EXISTING로 하고, StoreMetafile()을 호출하여 메타 데이터를 저장한다.
  */
  BOOL CreateServerFolderIfDeleted(LPCTSTR baseFolder);

  /**
  * @brief 충돌 처리창에서 동기화를 시작한 경우에 호출되며, 충돌 처리 준비를 한다
  * @param rootItem : 동기화 루트 폴더 항목
  * @param phase : 동기화 작업 단계
  * @return int : 성공하면 R_SUCCESS를 리턴하고, 아니면 오류 코드를 리턴한다.
  * @details A. 결과값이 FILE_NOT_CHANGED이면
  *     - RefreshServerNewState()를 호출하여 서버측 정보를 업데이트하고,
  *     - IsFolderFullPathChanged()를 호출하여 폴더의 전체 경로가 바뀌었으면,
  *       + 서버 상태에 ITEM_STATE_LOCATION_CHANGED를 더한다.
  *     - 로컬 상태가 ITEM_STATE_MOVED이거나 서버 상태가 ITEM_STATE_MOVED이면
  *       + 서버 상태에 ITEM_STATE_LOCATION_CHANGED를 더한다.
  *       + R_FAIL_NEED_FULL_SYNC_PARTIAL를 리턴한다(전체 동기화가 필요한 상태)
  *     - R_SUCCESS를 리턴한다.
  *     .
  * @n B. 충돌 결과가 오류인 경우에,
  *     - 서버 상태가 삭제됨이고,
  *       + 충돌 결과가 FILE_NEED_UPLOAD이면
  *         + child_user_action을 FILE_NEED_UPLOAD로 하고,
  *       + 아니고 충돌 결과가 FILE_NEED_UPLOAD이면
  *         + child_user_action을 FILE_NEED_LOCAL_DELETE로 한다.
  *     - 로컬 상태가 삭제됨이고,
  *       + 충돌 결과가 FILE_NEED_DOWNLOAD이면
  *         + child_user_action을 FILE_NEED_DOWNLOAD로 하고,
  *       + 아니고 충돌 결과가 FILE_NEED_UPLOAD이면
  *         + child_user_action을 FILE_NEED_SERVER_DELETE로 한다.
  *     .
  * @n C. 충돌 결과가 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED이거나
  * FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED인 경우에, 충돌 결과의 선택 방향을 child_user_action으로 한다.
  * @n D. 베이스 클래스의 KSyncItem::ReadyResolve()을 호출한다.
  * @n E. child_user_action이 0이 아니고 mEnable이 TRUE이면,
  *     - 충돌 결과가 FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED이면 flag를 SET_SERVER_DELETED로 하고,
  *     - 충돌 결과가 FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED이면 flag를 SET_LOCAL_DELETED로 한다.
  *     - SetConflictResultRecurse()를 호출하여 하위 항목에도 모두 child_user_action이 적용되도록 한다.
  */
  virtual int ReadyResolve(KSyncItem *rootItem, int phase, int syncMethod);

  /**
  * @brief 동기화 작업을 진행한다
  * @param p : 동가화 정책
  * @param baseFolder : 동기화 루트 폴더 경로명
  * @param phase : 동기화 작업 단계
  * @details A. SetPhase()로 phase값을 설정한다.
  * @n B. 상대 경로명을 준비한다.
  * @n C. phase가 PHASE_CONFLICT_FILES보다 작으면
  *     - 결과값이 충돌이나 오류이면
  *       + 이를 r에 저장하고, cancel_child_objects를 TRUE로 하고, 작업을 마무리한다.
  *     .
  * @n D. phase가 PHASE_CONFLICT_FILES이면
  *     - mEnable이 FALSE이면 로그를 저장하고, R_SUCCESS를 리턴한다.
  *     - 충돌 결과값을 d에 저장하고,
  *     - d가 중돌 또는 오류이고, d의 사용자 선택 방향이 0이면
  *       + d의 충돌 및 오류값을 r에 저장하고, cancel_child_objects를 TRUE로 하고, 작업을 마무리한다.
  *     .
  * @n E. d가 FILE_NEED_LOCAL_DELETE이고, phase가 PHASE_CONFLICT_FILES아니거나 d에 FILE_NEED_DOWNLOAD가 있으면,
  *     - IsLocalDeleteOk()가 실패이면,
  *       + d를 0으로 하고, r을 FILE_SYNC_DELETE_FOLDER_FAIL_CHILD_MODIFIED로 하고,
  *       + cancel_child_objects를 TRUE로 하고, 작업을 마무리한다.
  *     .
  * @n F. d값에서 (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE) 플래그를 지운다.
  * @n G. d값에 FILE_SAME_MOVED 플래그가 있으면,
  *     - 지금의 경로명을 임시 저장하고,
  *     - updateMetaAsRenamed()를 호출하여 경로명을 업데이트하고,
  *     - UpdateChildItemPath()를 호출하여 경로가 바뀐것을 하위 항목들에게도 업데이트한다.
  *     .
  * @n H. 아니면,
  *     - 로컬 상태에 ITEM_STATE_MOVED가 있고 서버 상태에 ITEM_STATE_MOVED가 있으면,
  *       + d에 FILE_NEED_UPLOAD가 있으면,
  *         + d에 FILE_NEED_SERVER_MOVE를 더한다.
  *       + 아니고 d에 FILE_NEED_DOWNLOAD가 있으면,
  *         + d에 FILE_NEED_LOCAL_MOVE를 더한다.
  *     - 아니고 로컬 상태에 ITEM_STATE_MOVED가 있으면,
  *       + d에 FILE_NEED_SERVER_MOVE를 더한다.
  *     - 아니고 서버 상태에 ITEM_STATE_MOVED가 있으면,
  *       + d에 FILE_NEED_LOCAL_MOVE를 더한다.
  *     .
  * @n I. d값에 (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE)가 있으면,
  *     - d값에 FILE_NEED_SERVER_MOVE가 있으면,
  *       + 서버 상태가 삭제됨(ITEM_STATE_DELETED)이면,
  *         + renameServerObjectAs()를 호출하고 결과가 성공이 아니면 r에 결과를 저장한다.
  *         + 서버 상태에서 ITEM_STATE_DELETED를 제거하고,
  *         + r과 d에서 (FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED)를 제거한다.
  *       + 아니면,
  *         + 동기화 정책에서 업로드가 가능한 상태이면
  *           + renameServerObjectAs()를 호출하여 서버측 이동을 하고 성공하면 d값에서 (FILE_NEED_UPLOAD|FILE_CONFLICT)를 제거한다.
  *         + 아니면,
  *           + isNeedMoveServerObjectAs()가 TRUE이면 r에 FILE_SYNC_NO_MOVE_PERMISSION를 넣고,
  * FALSE이면 FILE_SYNC_NO_RENAME_PERMISSION를 넣는다.
  *     - d값에 FILE_NEED_LOCAL_MOVE가 있으면,
  *       + 로컬 상태가 삭제됨(ITEM_STATE_DELETED)이면,
  *         + renameLocalObjectAs(FALSE)를 호출하고,
  *         + 로컬 상태에서 ITEM_STATE_DELETED를 제거하고, 
  *         + r과 d에서 (FILE_NEED_SERVER_MOVE|FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED)를 제거한다.
  *       + 아니면,
  *         + 동기화 정책에서 다운로드가 가능한 상태이면
  *           + renameLocalObjectAs(TRUE)를 호출하여 로컬측 이동을 행하고, 성공하면 d값에서 (FILE_NEED_DOWNLOAD|FILE_CONFLICT)를 제거한다.
  *         + 아니면 r값을 FILE_SYNC_NO_LOCAL_PERMISSION로 한다.
  *     - r값이 R_SUCCESS가 아니면 마무리하고 빠져 나간다.
  *     .
  * @n J. phase가 PHASE_MOVE_FOLDERS이면
  *     - 로컬 상태와 서버 상태에서 ITEM_STATE_MOVED를 제거한다.
  *     - d값에서 (FILE_NEED_LOCAL_MOVE|FILE_NEED_SERVER_MOVE)를 제거하고, 마무리하고 빠져 나간다.
  *     .
  * @n K. d값에 FILE_NEED_UPLOAD가 있고 부모가 NULL이 아니면,
  *     - 전체 경로를 준비하고,
  *     - 경로의 폴더가 존재하지 않으면, 
  *       + r값을 FILE_SYNC_NO_LOCAL_FOLDER_FOUND로 하고, 마무리하고 빠져 나간다.
  *     - 동기화 정책에 업로드 권한이 있으면,
  *       + CheckPermissionFolderUpload()를 호출하여 해당 폴더에 업로드할 권한이 있는지 검사하고, 권한이 있으면
  *         + createServerFolder()로 서버 폴더를 생성하고, 
  *           + 오류가 발생되면 r값을 FILE_SYNC_CREATE_SERVER_FOLDER_FAIL로 하고
  *           + 아니면 서버 상태와 로컬 상태를 ITEM_STATE_EXISTING로 하고, 서버 상태를 업데이트하고, 히스토리에 동작을 저장한다..
  *       + 권한이 없으면 r값을 FILE_SYNC_NO_CREATE_FOLDER_PERMISSION로 한다.
  *     - r값이 R_SUCCESS가 아니면 마무리하고 빠져 나간다.
  *     .
  * @n L. 아니고 d값에 FILE_NEED_DOWNLOAD가 있으면,
  *     - 동기화 정책에 다운로드 권한이 있으면,
  *       + CheckPermissionFolderDownload()를 호출하여 해당 폴더에 다운로드할 권한이 있는지 검사하고, 권한이 있으면
  *         + 서버 상태가 삭제됨이면, 
  *           + r값을 FILE_SYNC_NO_SERVER_FOLDER_FOUND로 하고
  *         + 아니면
  *           + createLocalFolder()로 로컬 폴더를 생성하고 결과를 r에 저장한다.
  *         + r이 성공이면
  *           + RefreshServerState()로 서버 정보를 업데이트하고
  *           + d값에서 (FILE_NEED_LOCAL_DELETE|FILE_NEED_SERVER_DELETE)를 제거하고,
  *           + 로컬 상태와 서버 상태를 ITEM_STATE_EXISTING로 하고
  *           + 로컬 정보를 업데이트하고,
  *           + 히스토리에 동작을 저장한다.
  *           + 아니면 서버 상태와 로컬 상태를 ITEM_STATE_EXISTING로 하고, 서버 상태를 업데이트 한다.
  *       + 권한이 없으면 r값을 FILE_SYNC_NO_LOCAL_WRITE_PERMISSION로 한다.
  *     - r값이 R_SUCCESS가 아니면 마무리하고 빠져 나간다.
  *     .
  * @n M. d값에 (FILE_NEED_RETIRED | FILE_NEED_LOCAL_DELETE | FILE_NEED_SERVER_DELETE)가 있으면, delete_meta를 TRUE로 한다.
  * @n N. d값에 FILE_NEED_LOCAL_DELETE가 있으면,
  *     - 로컬 상태가 삭제됨이거나 IsLocalFileExist()가 FALSE이면
  *       + changed를 TRUE로 한다.
  *     - 아니면,
  *       + 동기화 정책에 다운로드 권한이 있으면,
  *         + deleteLocalFolder()를 호출하여 로컬측 폴더를 삭제(휴지통에 이동)하고, 성공하면
  *           + deleteLocalChildsRecursive()를 호출하여 남은 하위 항목도 삭제하고,
  *           + changed를 TRUE로 하고,
  *           + 히스토리에 동작을 저장한다.
  *       + 없으면 r값을 FILE_SYNC_NO_DELETE_LOCAL_FOLDER_PERMISSION로 한다.
  *     .
  * @n O. d값에 FILE_NEED_SERVER_DELETE가 있으면,
  *     - 서버 상태가 삭제됨이거나 IsServerFileExist()가 FALSE이면
  *       + changed를 TRUE로 한다.
  *     - 아니면,
  *       + 동기화 정책에 업로드 권한이 있으면,
  *         + CheckPermissionFolderDelete()로 폴더 삭제 권한이 있으면
  *           + deleteServerFolder()로 서버 폴더를 삭제하고 실패하면
  *             + r값에 FILE_SYNC_DELETE_SERVER_FOLDER_FAIL를 넣고
  *           + 성공하면 
  *             + deleteLocalChildsRecursive()를 호출하여 하위 항목도 모두 삭제하고,
  *             + changed를 TRUE로 하고, 
  *             + 히스토리에 동작을 저장한다.
  *       + 권한이 없으면 r값을 FILE_SYNC_NO_DELETE_SERVER_FOLDER_PERMISSION로 한다.
  *     .
  * @n P. d값에 FILE_NEED_RETIRED가 있으면, deleteMeta를 TRUE로 하고, d값에서 FILE_NEED_RETIRED를 제거한다.
  * @n Q. r값에서 사용자 선택 방향을 제거하고 r값이 R_SUCCESS이면 changed를 TRUE로 한다.
  * @n R. r값을 결과 및 충돌 결과값으로 셋트한다.
  * @n S. r이 0이면
  *     - mChildResult도 0으로 하고,
  *     - mLocalNew의 시간 정보를 mLocal로, mServerNew의 시간을 mServer로 복사한다.
  *     - mServerNew를 클리어한다.
  *     .
  * @n T. r이 0이 아니면
  *     - 히스토리에 작업 실패 내용을 저장한다.
  *     .
  * @n U. StoreMetafile()을 호출하여 메타 데이터를 저장하거나 삭제한다.
  * @n V. deleteMeta가 TRUE이면, RetireChildObjects()를 호출하여 하위 항목도 모두 제거한다.
  * @n W. deleteMeta가 FALSE이면 ShellNotifyIconChangeOverIcon()를 호출하여 탐색기에서 아이콘을 업데이트한다.
  * @n X. cancel_child_objects가 TRUE이면 SetPhaseRecurse(PHASE_CONFLICT_FILES)를 호출하여 하위 항목들은 처리하지 않고 충돌 상태가 유지되도록 한다.
  */ 
  virtual int doSyncItem(KSyncPolicy *p, LPCTSTR baseFolder, int phase);


  virtual BOOL OnDeleteServerItem(BOOL r, LPCTSTR pError);

  /**
  * @brief 로컬측 이동 및 이름변경 정보에서 부모 폴더가 바뀌었는지 확인한다.
  * @return BOOL : 서버측 부모 폴더가 바뀌었으면(이름 변경이 아닌 이동이면) TRUE를 리턴한다.
  * @details A. 로컬측 이동 및 이름변경 정보에서 경로와 이름을 분리하여 경로를 changedLocation에 넣어둔다.
  * @n B. GetFolderInfo()로 이 폴더의 부모 폴더의 OID를 받아오고,
  * @n C. KSyncRootFolderItem::GetFolderOIDCascade()를 통해 changedLocation에 해당하는 폴더의 OID를 받아서
  * @n D. 두 폴더 OID가 다르면 TRUE를 리턴한다.
  */
  BOOL isNeedMoveServerObjectAs();

  /**
  * @brief 로컬측 이동 및 이름변경 정보에 따라 서버에서 이동 및 이름변경을 행하고 정보를 업데이트한다.
  * @param renameObject : 실제 이동 및 이름변경을 행할 것인지 여부.
  * @return int : 성공하면 R_SUCCESS 리턴하고 실패하면 오류 코드를 리턴한다.
  * @details A. renameObject가 TRUE이면 
  *     - 로컬측 이동 및 이름변경 정보에서 경로와 이름을 분리하여 각각 changedLocation, changedName에 넣어둔다.
  *     - KSyncRootFolderItem::GetFolderOIDCascade()를 통해 changedLocation에 해당하는 폴더의 OID를 받아온다.
  *     - 부모 폴더가 바뀌었으면
  *       + 새 부모 폴더 하위에 바꿀 이름과 동일한 다른 폴더가 존재하면,
  *         + 같은 이름을 가진 폴더가 이름이 바뀌지 않는 것이면 FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS 오류로 처리하여 리턴한다.
  *         + ReadyUniqueChildName()으로 임시 이름을 changedName에 준비한다.
  *         + KSyncCoreEngine::ChangeFolderName()으로 이름을 바꾸고 성공하면,
  *           + mpLocalRenamedAs을 임시 이름으로 바꾸고, 원래 이름은 SetServerTemporaryRenamed()을 호출하여 저장하여 둔다.
  *         + 실패하면 FILE_SYNC_NO_MOVE_PERMISSION를 리턴한다.
  *       + CheckPermissionFolderMove()를 호출하여 이동할 권한이 있는지 확인하고,
  *       + 권한이 있으면 KSyncCoreEngine::MoveItemTo()를 호출하여 이동을 하고 성공하면,
  *         + RefreshServerState()를 호출하여 서버측 정보를 업데이트하고, mServerNew 정보를 클리어한다.
  *       + 권한이 없으면 FILE_SYNC_NO_MOVE_PERMISSION를 리턴한다.
  *     - 폴더의 이름이 바뀌었으면
  *       + CheckPermissionFolderRename()을 호출하여 권한이 있는지 확인하고 권한이 있으면,
  *         + 부모 폴더에 바꿀 이름과 동일한 다른 폴더가 존재하면,
  *           + 같은 이름을 가진 폴더가 이름이 바뀌지 않는 것이면 FILE_SYNC_SERVER_RENAME_ALREADY_EXISTS 오류로 처리하여 리턴한다.
  *           + ReadyUniqueChildName()으로 임시 이름을 changedName에 준비한다.
  *           + KSyncCoreEngine::ChangeFolderName()으로 이름을 바꾸고 성공하면,
  *             + 이름을 임시 이름으로 바꾸고, 원래 이름은 SetServerTemporaryRenamed()을 호출하여 저장하여 둔다.
  *       + 권한이 없으면 FILE_SYNC_NO_RENAME_PERMISSION를 리턴한다.
  *     .
  * @n B. renameObject가 FALSE이면 
  *     - 성공한 것으로 설정한다.
  *     .
  * @n C. 위의 작업에서 이동이 성공하였으면 KSyncRootFolderItem::MoveSyncItem()을 호출하여 이 항목을 새 부모 폴더 하위로 이동시킨다.
  * @n D. 위의 작업이 성공하였으면,
  *     - MoveMetafileAsRename()를 호출하여 경로를 업데이트하고,
  *     - KSyncRootFolderItem::RemoveMovedRenamedOriginal()를 호출하여 이름 변경 테이블에서 이 폴더에 대한 항목을 삭제한다.
  *     - 로컬 및 서버측 이동 정보를 클리어하고,
  *     - 로컬 및 서버 상태에서 ITEM_STATE_MOVED를 제거하고,
  *     - renameObject가 TRUE이면, 히스토리에 작업 내용을 저장하고, 서버측 상태를 업데이트한다.
  *     - SetRelativePathName()으로 mBaseFolder를 업데이트하고,
  *     - StoreMetafile()로 메타 데이터를 저장한다.
  *     - 임시 이름으로 바뀐 상태이면 UpdateChildItemPath()으로 하위 항목들의 경로도 업데이트한다.
  *     .
  */
  int renameServerObjectAs(BOOL renameObject);

  /**
  * @brief 서버측 이동 및 이름변경 정보에 따라 로컬에서 이동 및 이름변경을 행하고 정보를 업데이트한다.
  * @param renameObject : 실제 이동 및 이름변경을 행할 것인지 여부.
  * @return int : 성공하면 R_SUCCESS 리턴하고 실패하면 오류 코드를 리턴한다.
  * @details A. 로컬측의 폴더 전체 경로를 준비하고,
  *     - 서버측 이동 및 이름변경 정보가 유효하면,
  *       + 이전 부모개체와 새 부모 개체가 다르면,
  *         + 새 부모 폴더의 경로를 받아서 mpServerRenamedAs에 업데이트한다.
  *       + 루트 폴더의 경로에 mpServerRenamedAs를 더하여 전체 경로명을 만든다.
  *     - 아니면 GetSyncResultName()으로 폴더의 전체 경로명을 받아온다.
  *     .
  * @n B. 이전 경로명과 새로 받은 경로명이 다르면,
  *     - renameObject가 TRUE이면,
  *       + 새 경로의 폴더가 존재하면
  *         + KSyncFolderItem::FindChildItemCascade()로 새 경로에 해당하는 폴더 개체를 찾고, 
  * 그 폴더에 동일한 이름이 존재하면 FILE_SYNC_LOCAL_RENAME_ALREADY_EXISTS를 리턴한다.
  *         + ReadyUniqueLocalName()으로 임시 이름으로 새 경로를 바꾼다.
  *       + KMoveFile() 으로 폴더를 이동하고, 임시 이름으로 바뀐것이면 원래 이름을 SetLocalTemporaryRenamed()로 저장해둔다.
  *     - 아니면 성공한 것으로 한다.
  *     - 위의 작업이 성공하였으면,
  *       + KSyncRootFolderItem::MoveSyncItem()으로 새 폴더의 하위로 이 개체를 이동한다.
  *       + MoveMetafileAsRename()으로 상대 경로명을 갱신하고 메타 데이터를 갱신한다.
  *       + SetRelativePathName()으로 mBaseFolder를 업데이트한다.
  *       + 이름 변경 정보를 클리어하고 서버 및 로컬 상태에서 ITEM_STATE_MOVED를 제거한다.
  *       + UpdateServerNewInfo()을 호출하여 서버측 정보를 갱신한다.
  *       + StoreMetafile()으로 메타 데이터를 저장한다.
  *       + 임시 이름으로 바뀌지 않았으면 UpdateChildItemPath()를 호출하여 하위 항목들의 경로도 갱신한다.
  * 임시 이름으로 바뀐 경우에는 원래 이름을 바꾸는 작업을 행한 후에 UpdateChildItemPath()를 호출하게 된다.
  *       + 작업 내용을 히스토리에 저장한다.
  *     .
  * @n C. 이전 경로명과 새로 받은 경로명이 같으면, R_SUCCESS를 리턴한다.
  * @n D. 오류 코드에 따른 충돌 오류를 준비하고 히스토리에 저장한다.
  */
  int renameLocalObjectAs(BOOL renameObject);

  /**
  * @brief 하위 항목중 서버측 이름 변경 정보가 지정한 상대 경로명과 동일하면 TRUE를 리턴한다.
  * @param relativeName : 상대 경로명.
  * @details A. 하위 항목들에 대해
  *     - 항목이 서버측 이름 변경 정보를 가지고 있고 이것이 지정한 relativeName과 일치하면 TRUE를 리턴한다.
  *     - 항목이 폴더이면 KSyncFolderItem::IsExistServerRenamed()를 재귀적으로 호출하고 리턴값이 TRUE이면 TRUE를 리턴한다.
  *     .
  */
  BOOL IsExistServerRenamed(LPCTSTR relativeName);

  /**
  * @brief 지정한 하위 항목이 이 폴더 바깥으로 이동되었으면 TRUE를 리턴한다.
  * @param c : 하위 항목 포인터.
  * @details A. 하위 항목이 로컬측 이동 및 이름변경된 상태이면,
  *     - 로컬측 결과 경로명을 구하고, 이것이 현재 폴더의 하위 영역(IsChildFolderOrFile())이 아니면 TRUE를 리턴한다.
  * @n B. 하위 항목이 서버측 이동 및 이름변경된 상태이면,
  *     - 서버측 결과 경로명을 구하고, 이것이 현재 폴더의 하위 영역(IsChildFolderOrFile())이 아니면 TRUE를 리턴한다.
  */
  BOOL IsOutGoingChild(KSyncItem *c);

  /**
  * @brief 본 개체에 대한 서버측 폴더를 생성한다.
  * @param parentItem : 부모 폴더 개체
  * @param pErrorMsg : 오류 메시지를 저장할 버퍼.
  * @return BOOL : 성공하면 TRUE를 리턴한다.
  * @details A. 부모 폴더의 서버 OID가 NULL이면 FALSE로 리턴한다.
  * @n B. KSyncCoreEngine::SyncServerCreateFolder()로 서버측에 폴더를 생성하고 성공하면,
  *     - 폴더 생성시에 받은 mServerNew에서 permission이나 fullPathIndex가 NULL이면, KSyncCoreEngine::GetServerFolderSyncInfo()로
  * 서버측 정보를 새로 받아온다.(이전 버전에서는 생성후에 서버측에서 permission이나 fullPathIndex정보를 보내주지 않았음)
  *     - mServerNew에 받은 정보를 mServer로 복사하고, 로그에 저장하고 TRUE를 리턴한다.
  */
  BOOL createServerFolder(KSyncItem *parentItem, LPTSTR* pErrorMsg);

  /**
  * @brief 본 개체에 대한 로컬측 폴더를 생성한다.
  * @details A. 로컬측 전체 경로명을 준비하고,
  * @n B. 폴더가 존재하지 않으면
  *     - CreateDirectory()로 폴더를 생성하고 성공하면
  *       + 폴더의 시간 정보를 업데이트하고, mServerNew를 mServer로 복사한다.
  *     - 실패하면 로그를 저장하고, GetLastError()가 5이면 FILE_SYNC_NO_LOCAL_PERMISSION를 리턴하고,
  * 아니면 FILE_SYNC_CREATE_LOCAL_FOLDER_FAIL를 리턴한다.
  */
  int createLocalFolder();

  /**
  * @brief 로컬측의 하위 항목들을 모두 삭제한다.
  * @param baseFolder : 동기화 루트 폴더
  * @param flag : 플래그
  * @details A. 하위 항목들에 대해
  *     - d를 R_SUCCESS로 초기화한다.
  *     - 플래그에 EXCLUDE_OUTGOING이 있으면,
  *       + IsOutGoingChild()(이 폴더 바깥으로 이동되는 항목)이면 d = 1로 한다.
  *     - d가 R_SUCCESS이면
  *       + 하위 항목이 폴더이면 KSyncFolderItem::deleteLocalChildsRecursive()를 재귀적으로 호출한다.
  *       + 아니면
  *         + 플래그에 DELETE_LOCAL_FILES가 있으면,
  *           + 하위 항목의 전체 경로를 준비하고
  *           + KSyncCoreEngine::SyncLocalDeleteFile()로 파일을 삭제한다.
  *         + d가 R_SUCCESS이면, StoreMetafile()로 메타 데이터를 삭제한다.
  *       + d가 R_SUCCESS이면, child 목록에서 이 하위 항목을 제거하고, 해제한다.
  *       + d가 R_SUCCESS가 아니면
  *         + 하위 항목의 phase를 PHASE_END_OF_JOB로 하고,
  *         + 리턴값을 R_FAIL_DELETE_CHILDS로 한다.
  *     .
  * @n B. 리턴값이 R_SUCCESS이며
  *     - 플래그에 DELETE_LOCAL_FILES이 있으면
  *       + 전체 경로를 준비하고, KSyncCoreEngine::SyncLocalDeleteFolder()로 이 폴더를 삭제한다.
  *       + StoreMetafile()로 메타 데이터를 삭제한다.
  *     .
  */
  int deleteLocalChildsRecursive(LPCTSTR baseFolder, int flag);

  /**
  * @brief 로컬측의 폴더를 삭제한다.
  * @param baseFolder : 동기화 루트 폴더
  * @return int : 성공하면 R_SUCCESS를 리턴한다.
  * @details A. 로컬측의 동기화 결과 전체 경로명을 준비하고
  * @n B. KSyncCoreEngine::SyncLocalDeleteFolder()로 이 폴더를 삭제한다.
  * @n C. 실패하면 로그 내용을 저장하고 FILE_SYNC_CHILD_DELETE_FAIL를 리턴한다.
  */
  int deleteLocalFolder(LPCTSTR baseFolder);

  /**
  * @brief 서버측의 폴더를 삭제한다.
  * @return int : 성공하면 R_SUCCESS를 리턴한다.
  * @details A. KSyncCoreEngine::SyncServerDeleteFolder()로 서버의 이 폴더를 삭제한다.
  * @n B. 실패하면 FILE_SYNC_DELETE_SERVER_FOLDER_FAIL를 리턴한다.
  */
  int deleteServerFolder();

  /**
  * @brief 서버측의 정보를 갱신한다.
  * @return BOOL : 성공하면 TRUE를 리턴한다.
  * @details A. 서버측 OID가 유효하면,
  *     - 지금의 mServer를 클리어한다.
  *     - KSyncCoreEngine::GetServerFolderSyncInfo()으로 서버측 정보를 mServer에 받아온다.
  */
  virtual BOOL RefreshServerState();

  /**
  * @brief 서버측의 새 정보를 갱신한다.
  * @return int : 성공하면 R_SUCCESS를, 아니면 ITEM_STATE_DELETED를 리턴한다.
  * @details A. 서버측 OID가 유효하면,
  *     - 지금의 mServerNew를 클리어한다.
  *     - KSyncCoreEngine::GetServerFolderSyncInfo()으로 서버측 정보를 mServerNew에 받아온다.
  */
  virtual int RefreshServerNewState();

  /**
  * @brief 동기화 충돌 처리창에서 로컬측의 이름을 변경하였을 때 복제 개체가 생성되며, 여기서 복제된 폴더의 추가작업을 처리한다.
  * 폴더가 복제되어 새로 추가되었으니 하위 항목들도 복제되어 새로 추가되어야 한다.
  * @param root : 동기화 루트 폴더 항목.
  * @param pair : 복제 개체.
  * @details A. KSyncItem::CloneLocalRename()를 호출하여 이 개체와 복제 개체의 상태와 정보를 설정한다.
  * @n B. 하위 항목들에 대해,
  *     - KSyncItem::SetLocalPath()으로 하위 항목의 경로를 업데이트 한다.
  *     - pair가 NULL이 아니면
  *       + KSyncItem::DuplicateItem()으로 하위 항목의 복제 항목을 생성하고,
  *       + KSyncItem::SetLocalPath()으로 복제된 하위 항목의 경로를 업데이트 하고,
  *       + 이를 pair의 child 개체로 등록한다
  *     - KSyncItem::CloneLocalRename()을 재귀적으로 호출하여 하위의 하위도 복제되도록 한다.
  *     .
  */
  virtual void CloneLocalRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief 동기화 충돌 처리창에서 서버측의 이름을 변경하였을 때 복제 개체가 생성되며, 여기서 복제된 폴더의 추가작업을 처리한다.
  * 폴더가 복제되어 새로 추가되었으니 하위 항목들도 복제되어 새로 추가되어야 한다.
  * @param root : 동기화 루트 폴더 항목.
  * @param pair : 복제 개체.
  * @details A. KSyncItem::CloneServerRename()를 호출하여 이 개체와 복제 개체의 상태와 정보를 설정한다.
  * @n B. 하위 항목들에 대해,
  *     - KSyncItem::SetLocalPath()으로 하위 항목의 경로를 업데이트 한다.
  *     - pair가 NULL이 아니면
  *       + KSyncItem::DuplicateItem()으로 하위 항목의 복제 항목을 생성하고,
  *       + KSyncItem::SetLocalPath()으로 복제된 하위 항목의 경로를 업데이트 하고,
  *       + 이를 pair의 child 개체로 등록한다
  *     - KSyncItem::CloneServerRename()을 재귀적으로 호출하여 하위의 하위도 복제되도록 한다.
  *     .
  */
  virtual void CloneServerRename(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief 동기화 충돌 처리창에서 새 개체로 등록하기를 하였을 때 복제 개체가 생성되며, 여기서 복제된 폴더의 추가작업을 처리한다.
  * 폴더가 복제되어 새로 추가되었으니 하위 항목들도 복제되어 새로 추가되어야 한다.
  * @param root : 동기화 루트 폴더 항목.
  * @param pair : 복제 개체.
  * @details A. KSyncItem::CloneSeparate()를 호출하여 이 개체와 복제 개체의 상태와 정보를 설정한다.
  * @n B. 하위 항목들에 대해,
  *     - KSyncItem::SetLocalPath()으로 하위 항목의 경로를 업데이트 한다.
  *     - pair가 NULL이 아니면
  *       + KSyncItem::DuplicateItem()으로 하위 항목의 복제 항목을 생성하고,
  *       + KSyncItem::SetLocalPath()으로 복제된 하위 항목의 경로를 업데이트 하고,
  *       + 이를 pair의 child 개체로 등록한다
  *     - KSyncItem::CloneSeparate()을 재귀적으로 호출하여 하위의 하위도 복제되도록 한다.
  *     .
  */
  virtual void CloneSeparate(KSyncRootFolderItem *root, KSyncItem *pair);

  /**
  * @brief 현재의 경로명을 대상 버퍼에 복사한다.
  * @param pathname : 경로명을 저장할 대상 버퍼, NULL이면 저장하지 않고 길이만 리턴한다.
  * @param length : 경로명을 저장할 대상 버퍼 길이
  * @return int : 성공하면 대상 버퍼 길이를 리턴한다.
  * @details A. 부모 개체가 NULL이면(동기화 루트 폴더)
  *     - mBaseFolder가 설정된 길이 KMAX_PATH보다 크면 FALSE를 리턴한다.
  *     - 대상 버퍼에 mBaseFolder를 복사한다.
  *     .
  * @n B. 아니면
  *     - 이 개체가 로컬측 이동 및 이름변경 정보를 가지고 있으면
  *       + 동기화 루트 폴더 경로에 로컬측 이동 및 이름변경 정보(mpLocalRenamedAs)를 더하여 경로명을 만든다.
  *     - 아니면
  *       + 부모 개체의 KSyncFolderItem::GetCurrentFolder()를 호출하여 경로명을 받아와서 여기에 이 개체의 이름을 더하여 경로명을 만든다.
  *     .
  */
  virtual int GetCurrentFolder(LPTSTR pathname, int length);

  /**
  * @brief 이 항목과 하위 항목에서 동기화 충돌 또는 오류 항목의 갯수를 리턴한다.
  * @param includeDisabled : disable된 항목도 카운트에 포함할지 여부.
  * @return int : 동기화 충돌 또는 오류 항목의 갯수
  * @details A. includeDisabled이 TRUE이거나, mPhase가 PHASE_END_OF_JOB보다 작으면
  *     - 이 항목의 충돌 결과값이 충돌 또는 오류이면 카운트를 1 증가한다.
  *     .
  * @n B. 하위 항목들에 대해
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::GetConflictCount()를 재귀호출하여 리턴된 값을 카운트에 더한다.
  *     - 아니면,
  *       + includeDisabled이 TRUE이거나, mPhase가 PHASE_END_OF_JOB보다 작으면,
  *         + 이 항목의 충돌 결과값이 충돌 또는 오류이면 카운트를 1 증가한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */
  int GetConflictCount(BOOL includeDisabled);

  void GetConflictCount(int& folderCount, int& fileCount, BOOL includeDisabled);

  /**
  * @brief 하위 항목의 전체 갯수를 리턴한다.
  * @param type : 대상 하위 항목의 형식
  * @details A. 하위 항목들에 대해
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::GetTotalCount()를 재귀호출하여 리턴된 값을 카운트에 더한다.
  *     - 항목의 형식이 일치하면 카운트를 1 증가한다.
  * @n B. 카운트값을 리턴한다.
  */
  int GetTotalCount(int type);

  /**
  * @brief 하위 항목의 전체 크기를 리턴한다.
  * @param conflictResult : 충돌 항목도 포함할지 여부
  * @return __int64 : 하위 항목의 전체 크기
  * @details A. 하위 항목들에 대해
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::GetTotalSize()를 재귀호출하여 리턴된 값을 크기값에 더한다.
  *     - 아니면 GetSyncFileSize()로 항목의 크기값을 더한다.
  * @n B. 크기값을 리턴한다.
  */
  __int64 GetTotalSize(BOOL conflictResult);

  /**
  * @brief 동기화 진행중에 작업 완료될 전체 크기를 리턴한다.
  * @param conflictResult : 충돌 항목도 포함할지 여부
  * @return __int64 : 동기화 완료된 크기
  * @details A. 하위 항목들에 대해
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::GetTotalSizeProgress()를 재귀호출하여 리턴된 값을 크기값에 더한다.
  *       + 폴더의 동기화 작업도 프로그레스에 반영되도록 EMPTY_FILE_ESTIMATE_SIZE 크기를 더한다.
  *     - 아니면
  *       + GetSyncFileSize()로 항목의 크기값을 더한다.
  *       + 빈 파일의 동기화 작업도 프로그레스에 반영되도록 EMPTY_FILE_ESTIMATE_SIZE 크기를 더한다.
  * @n B. 크기값을 리턴한다.
  */
  __int64 GetTotalSizeProgress(BOOL conflictResult);

  /**
  * @brief 동기화 진행중에 처리 완료된 항목의 갯수, 실제 크기, 완료된 크기를 리턴한다. 이는 동기화 상태창에 진행상태를 표시하기 위해 사용된다.
  * @param phase : 작업 단계
  * @param conflictResult : 충돌 항목도 포함할지 여부
  * @param count : 갯수를 저장할 버퍼
  * @param sizePure : 실제 크기를 저장할 버퍼
  * @param sizeProgress : 진행된 크기를 저장할 버퍼
  * @details A. 이 개체의 mPhase가 지정한 phase보다 작으면
  *     - 카운트를 1 증가하고, sizeProgress를 EMPTY_FILE_ESTIMATE_SIZE만큼 증가한다.
  *     .
  * @n B. 하위 항목들에 대해
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::GetCandidateCount()를 재귀호출하여 하위 폴더에 대한 값을 더한다.
  *     - 아니면
  *       + 하위 항목의 mPhase가 지정한 phase보다 작으면
  *         + GetSyncFileSize()로 크기값을 받아서 sizePure, sizeProgress에 더한다.
  *         + 빈 파일의 동기화 작업도 프로그레스에 반영되도록 sizeProgress에 EMPTY_FILE_ESTIMATE_SIZE 크기를 더한다.
  *     .
  */
  void GetCandidateCount(int phase, BOOL conflictResult, int &count, __int64 &sizePure, __int64 &sizeProgress);

  // int GetResultItemCount(int resultMask);

  /**
  * @brief 동기화 진행중에 로컬측 폴더를 삭제해도 되는지 확인한다. 하위 항목 중에 수정되거나 수정중인 파일이 있으면 삭제하지 않는다.
  * @param onLocal : 로컬측 또는 서버측 확인
  * @return TRUE : 삭제 가능하면 TRUE를 리턴한다.
  * @details A. 하위 항목들에 대해
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::IsLocalDeleteOk()를 재귀호출하여 하위 폴더에 대한 검사를 행하고 리턴값이 FALSE이면 FALSE를 리턴한다.
  *     - 아니면
  *       + 하위 항목의 로컬 상태에 (ITEM_STATE_MOVED|ITEM_STATE_UPDATED)값이 있으면, FALSE를 리턴한다.
  *       + 하위 항목의 플래그에 SYNC_ITEM_EDITING_NOW값이 있으면, 이 파일이 외부 프로그램에 의해 열려 있으므로, FALSE를 리턴한다.
  *     .
  */
  BOOL IsChildDeleteOk(BOOL onLocal);

  /**
  * @brief 동기화 루트 폴더 항목을 찾아 리턴한다.
  * @return KSyncFolderItem* : 동기화 루트 폴더 항목.
  * @details A. 이 항목의 mResolveFlag에 HIDDEN_ROOT_FOLDER값이 있으면(동기화 충돌처리창에서 이동된 개체 폴더를 표시하기
  * 위해 두개의 루트 폴더가 만들어지며, 이 둘을 포함하고 있는 숨겨진 루트 폴더),
  *     - 하위 항목중에서 
  *       + 항목이 폴더이고, IsDummyCloneFolder()가 FALSE이면 이 개체가 실제 동기화 루트 폴더이니 이를 리턴한다.
  *     .
  * @n B. mParent가 NULL이면 이것이 루트 폴더이니 이를 리턴한다.
  * @n C. mParent의 GetRootFolderItem()을 호출하고 그 결과를 리턴한다.
  */
  KSyncFolderItem *GetRootFolderItem();

  /**
  * @brief 동기화 루트 폴더의 로컬측 경로명을 리턴한다.
  * @return LPCTSTR : 동기화 루트 폴더의 로컬측 경로명.
  * @details A. GetRootFolderItem()를 호출하여 루트 폴더 항목을 찾는다.
  * @n B. 루트 폴더 항목을 찾았으면 이것의 mBaseFolder를 리턴한다.
  * @n C. 루트 폴더가 없으면 이 개체의 mBaseFolder를 리턴한다.
  */
  virtual LPCTSTR GetRootPath();

  /**
  * @brief 지정한 상대 경로명에 해당하는 항목을 찾아 리턴한다.
  * @param relativePathName : 찾을 항목의 상대 경로명.
  * @param matchType : 항목의 종류
  * @param withRenamed : 이름 변경된 정보를 포함하여 찾는다.
  * @return KSyncItem* : 찾은 개체의 포인터를 리턴한다.
  * @details A. relativePathName이 비었으면,
  *     - 이 항목의 종류와 matchType이 일치하면 이 개체를 리턴한다.
  *     - 아니면 NULL을 리턴한다.
  *     .
  * @n B. relativePathName에서 최상위의 이름과 하위의 경로명을 분리한다.
  * @n C. FindChildItem()으로 하위의 항목을 찾는다.
  * @n D. 하위 항목을 찾았고, 폴더이고 하위의 경로명이 유효하면
  *     - 하위 항목의 KSyncFolderItem::FindChildItemCascade()을 재귀 호출하고 리턴된 값을 하위 항목으로 한다.
  *     .
  * @n E. 하위 항목을 리턴한다.
  */
  KSyncItem *FindChildItemCascade(LPCTSTR relativePathName, int matchType, int flag = WITH_RENAMED);

  /**
  * @brief 지정한 경로명으로 이름 변경된 개체를 찾아 리턴한다.
  * @param relativePathName : 찾을 상대 경로명.
  * @param onLocal : TRUE이면 로컬측 이름변경에서 찾는다.
  * @param matchType : 항목의 종류
  * @return KSyncItem* : 찾은 개체의 포인터를 리턴한다.
  * @details A. 하위의 항목들 중에서 
  *     - matchType이 2이거나 하위 항목의 종류가 지정한 종류와 동일하면,
  *       + onLocal이 TRUE이고, 하위 항목의 mpLocalRenamedAs이 찾을 상대 경로명과 일치하면,
  * 이 하위 항목을 리턴한다.
  *       + onLocal이 FALSE이고, 하위 항목의 mpServerRenamedAs이 찾을 상대 경로명과 일치하면,
  * 이 하위 항목을 리턴한다.
  *     - 이 항목이 폴더이면, KSyncFolderItem::FindRenamedChildItem()를 재귀적으로 호출하고 리턴값이 NULL이 아니면 리턴값을 리턴한다.
  *     .
  * @n B. NULL을 리턴한다.
  */
  KSyncItem *FindRenamedChildItem(LPCTSTR relativePathName, BOOL onLocal, int matchType);

  /**
  * @brief 지정한 경로명에 따라 동기화 개체를 생성한다.
  * @param relativePath : 생성할 개체의 상대 경로명.
  * @param matchType : 개체의 종류
  * @root : 루트 폴더 항목
  * @flag : REFRESH_LOCALNEW : 생성된 개체의 LocalNew 정보를 업데이트한다
  * @created_count : 생성된 개체 개수를 리턴
  * @return KSyncItem* : 마지막 개체의 포인터를 리턴한다.
  * @details A. relativePathName에서 최상위의 이름과 하위의 경로명을 분리한다.
  * @n B. 하위의 경로명이 유효하거나 matchType이 0보다 크면,
  *     - KSyncFolderItem을 생성하고,
  *       + 하위의 경로명이 유효하면, KSyncFolderItem::CreateChildItemCascade()을 재귀 호출한다.
  *     .
  * @n C. 아니면
  *     - KSyncItem을 생성한다.
  */
  KSyncItem *CreateChildItemCascade(LPCTSTR relativePath, int matchType, KSyncRootFolderItem* root, int flag, int *created_count);

  /**
  * @brief 동기화 제외를 설정 또는 해제하고 메타 데이터에 저장한다.
  * @param s : 메타 데이터 스토리지
  * @param exclude : TRUE이면 동기화 제외 설정, 아니면 해제.
  * @param recursive : 0보다 크면 하위 항목도 모두 재귀적으로 처리한다.
  * @details A. SetExcludedSync()를 호출하여 동기화 제외를 설정 또는 해제한다.
  * @n B. recursive가 0보다 크면,
  *     - 하위 항목들에 대해,
  *       + 폴더이면 KSyncFolderItem::StoreExcludedSyncCascade()를 재귀 호출한다.
  *     .
  */
  void StoreExcludedSyncCascade(KSyncRootStorage *s, BOOL exclude, int recursive);

  /**
  * @brief 지정한 이름을 가진 하위 항목을 찾아 리턴한다.
  * @param name : 찾을 개체 이름
  * @param withRename : TRUE이면 이름 변경된 내용과도 비교한다.
  * @return KSyncItem* : 찾은 항목.
  * @details A. 하위 항목들에 대해,
  *     - 이름을 비교하여 일치하면 이 하위 항목을 리턴한다.
  *     - withRename이 TRUE이고, mpLocalRenamedAs이 유효하면,
  *       + mpLocalRenamedAs에서 이름을 분리하여, 이것이 name과 일치하면 이 하위 항목을 리턴한다.
  *     .
  */
  KSyncItem *FindChildItem(LPCTSTR name, BOOL withRename);

  /**
  * @brief 하위 항목에서 지정한 서버 OID를 가진 항목을 찾아 리턴한다.
  * @param serverOID : 찾을 서버 OID
  * @return KSyncItem* : 찾은 항목.
  * @details A. 하위 항목들에 대해,
  *     - 서버 OID를 비교하여 일치하면 이 하위 항목을 리턴한다.
  *     - 하위 항목이 폴더이면 KSyncFolderItem::FindChildItemByOIDCascade()를 호출하고 리턴값이 NULL이 아니면 리턴값을 리턴한다.
  *     .
  * @n B. NULL을 리턴한다.
  */
  KSyncItem *FindChildItemByOIDCascade(LPCTSTR serverOID);

  /**
  * @brief 하위 항목에서 지정한 경로에 해당하는 개체를 찾아서, 서버 OID와 개체의 포인터를 리턴한다.
  * @param pathname : 찾을 경로명
  * @param serverOID : 찾은 개체의 서버 OID를 가져올 버퍼
  * @return KSyncItem* : 찾은 항목.
  * @details A. pathname의 길이가 0이면,
  *     - 이 개체의 서버 OID를 serverOID에 복사하고,
  *     - 이 개체의 포인터를 리턴한다.
  *     .
  * @n B. pathname에서 앞쪽 첫번째 폴더 이름을 가져와서, FindChildItem()으로 그 이름에 해당하는 하위 개체를 찾는다.
  * @n C. 찾은 개체가 NULL이 아니고 폴더이면,
  *     - 다음에 찾을 경로명이 비어있지 않으면
  *       + KSyncFolderItem::GetFolderOIDCascade()를 재귀 호출하고 그 리턴값을 리턴한다.
  *     - 아니면
  *       + 찾은 개체의 서버 OID를 serverOID에 복사하고,
  *       + 찾은 개체를 리턴한다.
  *     .
  * @n D. NULL을 리턴한다.
  */
  KSyncFolderItem *GetFolderOIDCascade(LPTSTR pathname, LPTSTR OUT serverOID);

  /**
  * @brief 이 개체에 해당하는 서버측 폴더가 존재하는지 확인한다.
  * @param createIfNotExist : TRUE이고 찾는 폴더가 없으면 새로 생성한다.
  * @return BOOL : 서버측 폴더가 존재하면 TRUE를 리턴한다.
  * @details A. 이 개체의 서버 OID가 비어있으면,
  *     - 결과를 TRUE로 설정한다.
  *     - 부모 개체가 NULL이 아니면 부모 개체에 KSyncFolderItem::CheckServerFolderExist()를 호출하고 결과를 받아온다.
  *     - 결과가 TRUE이고 createIfNotExist도 TRUE이면,
  *       + KSyncCoreEngine::SyncServerCreateFolder()로 서버측에 폴더를 생성한다.
  *     - 결과를 리턴한다.
  *     .
  * @n B. 아니면 TRUE를 리턴한다.
  */
  BOOL CheckServerFolderExist(BOOL createIfNotExist);

  /**
  * @brief 동기화 작업중에 로컬측 폴더가 정상적으로 존재하는지 확인한다. 
  * 이 함수는 동기화 진행중 오류 발생하면 상위 폴더가 삭제된 경우인지 체크하여, 
  * 삭제되었으면 해당 폴더의 하위 항목이 더이상 진행되지 않도록 오류로 처리하기 위해 사용된다.
  * 아니면 무의미한 시도를 계속하게 된다.
  * @return BOOL : 로컬측 폴더가 존재하면 TRUE를 리턴한다.
  * @details A. 이 개체의 전체 경로명을 준비하고, 그 폴더가 로컬에 존재하지 않으면, 
  *     - 결과 및 충돌 결과를 FILE_SYNC_NO_LOCAL_FOLDER_FOUND로 설정한다.
  *     - 로컬 상태에 ITEM_STATE_ERROR값을 더한다.
  *     - 이 개체의 phase값을 PHASE_CONFLICT_FILES로 한다.
  *     - 부모 폴더가 존재하면 부모 폴더에 대해 KSyncFolderItem::CheckLocalFolderExistOnSyncing()을 재귀적으로 호출한다.
  *     - FALSE를 리턴한다.
  *     .
  * @n B. 아니면 TRUE를 리턴한다.
  */
  BOOL CheckLocalFolderExistOnSyncing();

  /**
  * @brief 동기화 작업중에 서버측 폴더가 정상적으로 존재하는지 확인한다. 
  * 이 함수는 동기화 진행중 오류 발생하면 상위 폴더가 삭제된 경우인지 체크하여, 
  * 삭제되었으면 해당 폴더의 하위 항목이 더이상 진행되지 않도록 오류로 처리하기 위해 사용된다.
  * 아니면 무의미한 시도를 계속하게 된다.
  * @return BOOL : 서버측 폴더가 존재하면 TRUE를 리턴한다.
  * @details A. KSyncCoreEngine::SyncServerCheckFolderExist()를 호출하여 서버측에 폴더가 정상적으로 존재하지 않으면,
  *     - 결과 및 충돌 결과를 FILE_SYNC_NO_SERVER_FOLDER_FOUND로 설정한다.
  *     - 서버 상태에 ITEM_STATE_ERROR값을 더한다.
  *     - 이 개체의 phase값을 PHASE_CONFLICT_FILES로 한다.
  *     - 부모 폴더가 존재하면 부모 폴더에 대해 KSyncFolderItem::CheckServerFolderExistOnSyncing()을 재귀적으로 호출한다.
  *     - FALSE를 리턴한다.
  *     .
  * @n B. 아니면 TRUE를 리턴한다.
  */
  BOOL CheckServerFolderExistOnSyncing();

  /**
  * @brief 하위 항목 중에서 동기화 충돌 처리창 목록에 나타날 항목의 개수를 카운트하여 리턴한다.(재귀 호출하지 않음)
  * @return int : 목록의 개수
  * @details A. 하위 항목들에서 
  *     - KSyncItem::IsResolveVisible()을 호출하여 TRUE이면 카운트를 1 중가한다.
  *     .
  * @n B. 카운트값을 리턴한다.
  */
  int GetResolveChildItemCount();

  /**
  * @brief 하위 항목 중 동기화 충돌 처리창 목록에 나타날 항목에서 index번째 개체를 리턴한다.(재귀 호출하지 않음)
  * @param index : 찾고자 하는 항목의 순서.
  * @return KSyncItem* : 하위 항목 포인터
  * @details A. 하위 항목들에서 
  *     - KSyncItem::IsResolveVisible()을 호출하여 TRUE이면,
  *       + 카운트가 index와 동일하면 이 개체를 리턴한다.
  *       + 카운트 값을 1 중가한다.
  *     .
  * @n B. NULL을 리턴한다.
  */
  KSyncItem *GetResolveChildItem(int index);

  /**
  * @brief 하위 항목 중 동기화 충돌 처리창 목록에 나타날 항목중 조건에 맞는 개체들의 리스트를 리턴한다.(재귀 호출하지 않음)
  * @param itemArray : 찾은 항목의 포인터를 담아갈 어레이.
  * @param type : 찾고자 하는 항목의 종류.
  * @param flag : 플래그
  * @details A. 하위 항목들에서 
  *     - KSyncItem::IsResolveVisible()이 TRUE이고 찾는 타입이 일치하면,
  *       + 플래그에 EX_DUMMY가 있고(더미 항목은 제외함), 이 항목이 폴더이며,
  * 이 항목이 더미 폴더이면 (KSyncFolderItem::IsDummyCloneFolder()) 제외한다.
  *       + 플래그에 IN_SINGLE_DUMMY가 있으면,
  *         + 이 항목이 더미 폴더이면, GetNormalChildItem()으로 원본 개체를 찾아 원본 개체가 없거나, 원본 개체가 IsResolveVisible() 이면 포함시키고,
  * 아니면 제외시킨다.
  */
  void GetResolveChildItems(KSyncItemArray &itemArray, int type, int flag = 0);

  /**
  * @brief 하위 항목 중 종류와 결과가 조건에 해당하는 개체들을 리스트에 담아 리턴한다.
  * @param itemArray : 찾은 항목의 포인터를 담아갈 어레이.
  * @param type : 찾고자 하는 항목의 종류.
  * @param resultMask : 결과 마스크
  * @param recursive : TRUE이면 하위 폴더에 대해 재귀호출을 한다.
  * @return int : 찾은 개체의 개수를 리턴한다.
  * @details A. 이 항목의 충돌 결과값과 resultMask를 AND한 결과가 0이 아니면,
  *     - 리스트에 이 개체를 담고 카운트를 1 증가한다.
  *     .
  * @n B. 아니면 하위 항목들에서 
  *     - 항목이 폴더이면,
  *       + recursive가 TRUE이면 하위 항목에 KSyncFolderItem::GetChildItemArray()를 호출하고 리턴값을 카운트에 더한다.
  *     - 항목이 폴더가 아니고, 종류가 지정한 type과 일치하며, 충돌 결과값과 resultMask를 AND한 결과가 0이 아니면, 
  *       + 리스트에 하위 항목을 담고 카운트를 1 증가한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */
  int GetChildItemArray(KSyncItemArray &itemArray, int type, int resultMask, BOOL recursive);

  /**
  * @brief 하위 항목의 충돌 결과값을 부모 폴더의 mChildResult에 더하여 하위 폴더에 충돌이 있는 상태임을 폴더가 알 수 있도록 한다.
  * @param depth : calling depth. start from 0
  * @param rootFolder : 동기화 루트 폴더 항목
  * @param flag : 플래그. (REMOVE_DISABLED, STORE_META, SET_CONFLICT_CHILD)
  * @return int : 동기화 결과값을 리턴한다.
  * @details A. 플래그에 SET_CONFLICT_CHILD이 있으면,
  *     - mResult가 0이 아니면, AddConflictResult()로 충돌 결과값을 (FILE_SYNC_BELOW_CONFLICT | (mResult & FILE_USER_ACTION_MASK))로 한다.
  *     .
  * @n B. 충돌 결과값(mConflictResult)에 충돌이나 오류코드가 있으면, flag에 SET_CONFLICT_CHILD를 더한다.
  * (즉 이 폴더 하위의 개체들은 모두 충돌 결과값에 FILE_SYNC_BELOW_CONFLICT를 갖는다)
  * @n C. 하위 항목들에 대해
  *     - 항목이 폴더이면, KSyncFolderItem::UpdateFolderSyncResult()를 재귀 호출한다.
  *     .
  * @n D. mChildResult를 클리어하고, 하위 항목들에 대해
  *     - 하위 항목의 충돌 결과값에 FILE_SYNC_RESOLVE_VISIBLE_FLAG를 마스크한 결과값을 mChildResult에 OR로 더한다.
  *     - 하위 항목이 폴더이면 하위 항목의 mChildResult를 이 개체의 mChildResult에 더한다.
  *     - 하위 항목이 폴더가 아니면,
  *       + flag에  SET_CONFLICT_CHILD가 있으며,
  *         + 하위 항목의 결과가 0이 아니면, 하위 항목의 충돌 결과값에 (FILE_SYNC_BELOW_CONFLICT | (item->mResult & FILE_USER_ACTION_MASK))를 더한다.
  *     - flag에 REMOVE_DISABLED가 있고, 하위 항목이 Disable 상태이면,
  *       + 하위 항목을 child 목록에서 제거하고, 하위 항목을 해제한다.
  *     .
  * @n E. flag에 STORE_META가 있으면,
  *     - StoreMetafile()을 호출하여 메타 데이터를 저장한다.
  *     .
  * @n F. 동기화 결과값을 리턴한다.
  */
  int UpdateFolderSyncResult(int depth, KSyncRootFolderItem *rootFolder, int flag);

  /**
  * @brief 하위 항목의 상대 경로명을 중간의 폴더 이름으로부터 얻어온다.
  * @param pItem : 대상 동기화 항목
  * @param relativePath : 상대 경로명이 저장될 버퍼
  * @details A. 하위 항목들에 대해
  *     - relativePath에 하위 항목의 이름을 붙인다.
  *     - 하위 항목이 pItem과 일치하면 TRUE를 리턴한다.
  *     - 하위 항목이 폴더이면,
  *       + KSyncFolderItem::GetItemRelativePath()를 호출하고 리턴값이 TRUE이면 TRUE를 리턴한다.
  *     - relativePath를 원래 입력된 만큼으로 자른다.
  *     .
  * @n B. FALSE를 리턴한다.
  */
  BOOL GetItemRelativePath(KSyncItem *pItem, LPTSTR OUT relativePath);

  /**
  * @brief 경로명의 길이를 리턴한다.
  * @param type : 1이면 로컬측 경로명, 2이면 서버측 경로명.
  * @details A. 부모 폴더가 NULL이 아니면
  *     - 부모 폴더에서 KSyncFolderItem::GetPathNameLength()으로 길이를 구한다
  *     - 자신의 이름의 길이 + 1을 길이에 더한다.
  *     .
  * @n B. 아니면(이 개체는 동기화 루트 폴더임)
  *     - type이 1이면 mBaseFolder의 길이 + 1을 길이에 더한다.
  *     - type이 2이면 GetServerRelativePath()의 길이 + 1을 길이에 더한다.
  *     .
  * @n C. 길이값을 리턴한다.
  */
  int GetPathNameLength(int type);

  /**
  * @brief 경로명을 지정한 버퍼에 저장한다.
  * @param buff : 경로명을 저장할 버퍼.
  * @param len : 경로명을 저장할 버퍼의 크기.
  * @param type : 1이면 로컬측 경로명, 2이면 서버측 경로명.
  * @details A. 부모 폴더가 NULL이 아니면
  *     - 부모 폴더에서 KSyncFolderItem::GetPathName()으로 경로명을 구한다
  *     - 자신의 이름을 더한다..
  *     .
  * @n B. 아니면(이 개체는 동기화 루트 폴더임)
  *     - type이 1이면 mBaseFolder를 버퍼에 복사한다..
  *     - type이 2이면 GetServerRelativePath()의 결과를 버퍼에 복사한다..
  */
  void GetPathName(LPTSTR buff, int len, int type);

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
  //int GetRenamedPathName(LPTSTR buff, int len, int flag);

  /**
  * @brief 상위 폴더 중에서 이름 변경된 폴더가 있는지 확인한다.
  * @return BOOL : 변경된 폴더가 있으면 TRUE를 리턴한다.
  * @param flag : 로컬 또는 서버측.
  * @details A. flag에 AS_LOCAL값이 있고, 로컬측 이름 변경 정보가 유효하면 TRUE를 리턴한다.
  * @n B. flag에 AS_LOCAL값이 없고, 서버측 이름 변경 정보가 유효하면 TRUE를 리턴한다.
  * @n C. 부모 개체가 NULL이 아니면 부모 개체의 KSyncFolderItem::IsParentRenamed()를 호출하고 리턴값을 리턴한다.
  * @n D. FALSE을 리턴한다.
  */
  BOOL IsParentRenamed(int flag);

#if 0
  /*
  * @brief 동기화 결과 상대 경로명을 버퍼에 저장한다.
  * @param name : 경로명을 저장할 버퍼.
  * @param len : 경로명을 저장할 버퍼의 크기.
  * @param flag : 로컬 또는 서버측.
  */
  virtual void GetSyncResultRName(LPTSTR name, int length, int flag);
#endif

  /**
  * @brief 동기화 결과 이동된 상대 경로명을 새 버퍼에 할당, 저장하여 리턴한다.
  * @param flag : 로컬 또는 서버측.
  * @details A. name을 NULL로 한다.
  * @n B. 이 항목이 동기화 더미 복제 폴더이면(IsDummyCloneFolder()),
  *     - flag에 AS_LOCAL값이 있고, mpDummyLocalPath가 유효하면
  *       + flag에 AS_NAME_ONLY값이 있으면 mpDummyLocalPath의 이름 부분만 name에 저장하고,
  *       + 아니면 mpDummyLocalPath를 name에 복제 저장한다.
  *     - flag에 AS_LOCAL값이 없고, mpDummyServerPath가 유효하면
  *       + flag에 AS_NAME_ONLY값이 있으면 mpDummyServerPath의 이름 부분만 name에 저장하고,
  *       + 아니면 mpDummyServerPath를 name에 복제 저장한다.
  *     .
  * @n C. name을 리턴한다.
  */
  virtual LPTSTR AllocSyncMoveResultName(int flag);

  /**
  * @brief 동기화 충돌처리 트리에서 폴더 항목이 열린 상태인지 확인한다.
  */
  inline BOOL IsOpened() { return !(mResolveFlag & FOLDER_CLOSED); }
  /**
  * @brief 동기화 충돌처리 트리에서 폴더 항목이 열린 상태로 한다.
  */
  inline void SetOpened(BOOL open) { if (open) mResolveFlag &= ~FOLDER_CLOSED; else mResolveFlag |= FOLDER_CLOSED; }

  /**
  * @brief 동기화 충돌처리 트리에서 더미 복제 폴더인지 확인한다. 
  * 더미 복제 폴더는 다른 폴더로 이동된 개체들을 따로 묶어 보여주기 위해 임시 생성된 개체이다.
  */
  inline BOOL IsDummyCloneFolder() { return (mResolveFlag & DUMMY_FOLDER); }
  /**
  * @brief 동기화 충돌처리 트리에서 더미 복제 폴더로 설정한다. 
  */
  inline void SetDummyCloneFolder() { mResolveFlag |= DUMMY_FOLDER; }

  /**
  * @brief 동기화 충돌처리 트리에서 더미 복제 폴더와 쌍이 되는 원본 개체를 찾아 리턴한다. 
  * @param name : 찾을 개체의 이름
  * @return KSyncItem* : 찾은 개체의 포인터
  * @details A. 하위항목 중에서
  *     - 항목이 폴더이고, 이름이 원본 개체와 같으며 더미 복제 폴더이면 이 항목을 리턴한다.
  *     .
  * @n B. NULL을 리턴한다.
  */
  KSyncItem* GetNormalChildItem(LPCTSTR name = NULL);

  /**
  * @brief 동기화 충돌처리 트리에서 origin에 해당하는 더미 복제 폴더를 어레이에 담아간다. 
  * @param origin : 찾을 원본 개체
  * @param dummyItems : 찾은 더미 개체를 담을 어레이.
  * @return int : 찾은 항목 개수
  * @details A. 하위항목 중에서
  *     - 항목이 폴더이고, 원본 개체가 아니며, 더미 복제 폴더이며, 이름이 원본 개체와 같으면 어레이에 추가한다.
  *     .
  * @n B. 어레이의 개수를 리턴한다.
  */
  int FindDummyPairItems(KSyncItem* origin, KSyncItemArray& dummyItems);

  /**
  * @brief 동기화 충돌처리 트리에서 지정한 순서에 해당하는 항목을 리턴한다.
  * @param index : 1부터 시작하는 순서.
  * @return KSyncItem* : 찾은 개체의 포인터
  * @details A. index값에서 1을 뺀다.
  * @n B. getTreeDisplayChildItemR()을 호출하여 리턴값을 리턴한다.
  */
  KSyncItem* GetTreeDisplayChildItem(int index);

  /**
  * @brief 동기화 충돌처리 트리에서 지정한 순서에 해당하는 항목을 리턴한다.
  * @param index : 0부터 시작하는 순서.
  * @return KSyncItem* : 찾은 개체의 포인터
  * @details A. 이 항목이 IsVisible() && IsResolveVisible()에 해당하면,
  *     - index값이 0이면 이 개체를 리턴한다.
  *     - index값에서 1을 뺀다.
  *     .
  * @n B. 이 폴더가 열린 상태(IsOpened())이면 .
  *     - 하위 항목중에서
  *       + 항목의 IsResolveVisible()이 TRUE이고, 항목이 문서이면
  *         + index가 0이면 이 하위 항목을 리턴하고,
  *         + index값에서 1을 뺀다.
  *     .
  * @n C. 부모 개체가 NULL이 아니고 이 개체가 더미 복제 개체가 아니면
  *     - 부모 개체에서 FindDummyPairItems()을 호출하여 이 개체의 더미 복제 개체를 받아온다.
  *     - 더미 복제 개체 리스트에서 각 개체들 중에서
  *       + 항목이 KSyncItem::IsVisible()이면,
  *         + index가 0이면 이 항목을 리턴하고,
  *         + index값에서 1을 뺀다.
  *       + 항목이 오픈 상태이면,
  *         + KSyncFolderItem::GetResolveChildItems()을 호출하여 이 항목의 하위 항목 리스트를 받아온다.
  *         + index가 리스트 갯수보다 적으면,
  *           + 리스트에서 index번째 항목을 리턴하고, index를 0으로 한다.
  *         + index값에서 리스트 갯수만큼 뺀다.
  *     .
  * @n D. 이 개체가 오픈된 상태이면
  *     - GetResolveChildItems()으로 더미 개체가 아닌 하위 항목을 리스트에 받아온다.
  *     - 위의 C에서 받아온 하위 더미 복제 개체 리스트의 항목에 대해
  *       + 이 항목이 오픈된 상태이면, 이 항목에서 GetResolveChildItems()를 호출하여 더미 복제가 아닌 하위 항목을 리스트에 받아온다. 
  *     - GetResolveChildItems()을 호출하여 SINGLE_DUMMY로 된 개체들을 리스트에 받아온다. 
  *     - 위의 단계에서 받아온 리스트의 항목에 대해
  *       + 항목에 대해 getTreeDisplayChildItemR()을 호출하여 결과가 NULL이 아니면 그것을 리턴한다.
  *     .
  * @n E. NULL을 리턴한다.
  */
  KSyncItem* getTreeDisplayChildItemR(int& index);

  /**
  * @brief 이 항목이 동기화 충돌처리 트리에 나타나는 항목인지 확인한다..
  * @return BOOL : 트리에 나타나는 항목이면 TRUE를 리턴한다.
  * @details A. KSyncItem::IsResolveVisible()을 호출하여 TRUE이면 TRUE를 리턴한다.
  * @n B. mChildResult에 FILE_SYNC_RESOLVE_VISIBLE_FLAG값이 있으면 TRUE를 리턴한다.
  * @n C. FALSE를 리턴한다.
  */
  virtual BOOL IsResolveVisible();

  /**
  * @brief 동기화 충돌처리 트리에 나타나기 이전에 준비 작업을 한다. 동기화 충돌처리 트리에 초기화 될 때 호출된다.
  * 하위 항목 중에서 다른 폴더로 이동된 개체를 찾아서 해당하는 더미 복제 폴더 개체를 이동시켜 준다.
  * @param clones : 복제 개체들을 이 리스트에 담는다.
  * @param flag : 플래그
  * @details A. flag & FORCE_OPEN_FOLDER이면, 오픈 상태로 설정한다.
  * @n B. InitResolveState()를 호출하여 동기화 충돌 초기화를 한다.
  * @n C. 하위 항목들에 대하여
  *     - 하위 항목이 폴더이면 KSyncFolderItem::InitializeResolveState()를 재귀 호출한다.
  *     - 아니면 KSyncItem::InitResolveState()를 호출한다.
  *     - GetItemStateMovedRenamed()로 로컬측 상태와 서버측 상태를 받아와서, lo_flag, sv_flag에 저장한다.
  *     - flag & MAKE_CLONE_FOLDER이고, 하위 항목의 결과에 동기화 충돌 또는 오류가 있으며, lo_flag 또는 sv_flag에 ITEM_STATE_MOVED가 있으면,
  *       + 항목의 현재 경로를 임시 저장하여 두고, 
  *       + 항목의 로컬측 이름 변경된 상태이면,
  *         + 이름 변경 정보에서 새 경로를 받아와서 newLocalPath에 저장하고 이를 현재 경로와 비교하여, 
  * 경로가 바뀌지 않았으면 newLocalPath를 NULL로 하고, 아니면 newLocalPath를 "\"로 한다.
  *       + 항목의 서버측 이름 변경된 상태이면,
  *         + 이름 변경 정보에서 새 경로를 받아와서 newServerPath에 저장하고 이를 현재 경로와 비교하여, 
  * 경로가 바뀌지 않았으면 newServerPath를 NULL로 하고, 아니면 newServerPath를 "\"로 한다.
  *       + 로컬측 또는 서버측 경로가 바뀌었으면,
  *         + GetMovedCloneFolder()으로 새 경로에 해당하는 복제 개체를 찾아와서 이 항목의 parent로 설정하고, 본 항목의 child 목록에서 제거한다.
  *     .
  */
  void InitializeResolveState(KSyncItemArray& clones, int flag);

  /**
  * @brief 동기화 충돌처리 트리에 더미 복제 폴더로 나뉘어졌던 항목들을 원래의 폴더 하위로 이동시켜 준다.
  * @param clones : 더미 복제 폴더로 이동되었던 개체들이 담겨져 있다.
  * @details A. clones 리스트의 항목에서
  *     - 항목의 부모가 NULL이면 본 항목의 AddChild()를 호출하여 child로 등록한다.
  *     - 아니면 항목의 부모 개체에 AddChild()를 호출하여 child로 등록한다.
  *     - 리스트에서 제거한다.
  */
  void RestoreClones(KSyncItemArray& clones);

  /**
  * @brief 하위 목록 중에서 더미 복제 폴더를 제거하고 삭제한다. 더미 복제 폴더에 포함된 하위 개체들은 원래 개체의 하위로 이동시킨다.
  * @return int : 제거된 갯수를 리턴한다.
  * @details A. 하위 항목 중에서
  *     - 항목이 폴더이면,
  *       + KSyncFolderItem::RemoveDummyCloneFolders()를 재귀 호출하고 리턴값을 카운트에 더한다.
  *       + 항목이 더미 복제 폴더이면(IsDummyCloneFolder()),
  *         + 원 폴더를 찾고, KSyncFolderItem::MoveChildItemTo()를 호출하여 하위 항목들을 원 폴더 하위로 이동한다.
  *         + 항목을 삭제하고 child 목록에서 제거하고, 카운트를 1 증가한다.
  *     .
  * @n B. 카운트값을 리턴한다.
  */
  int RemoveDummyCloneFolders();

  /**
  * @brief 충돌 및 오류 발생한 항목은 Enable로 설정한다. 변경된 UI(2016.09)에서는 사용 안함.
  * @return int : Disable 된 항목의 갯수를 리턴한다.
  * @details A. 이 항목의 충돌 결과값에 (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)이 없으면 Enable을 FALSE로 하고, 카운트를 1 증가한다.
  * @n B. 하위 항목 중에서
  *     - 항목이 폴더이면 KSyncFolderItem::CheckEnableOnResolveConflict()을 재귀 호출하고 리턴값을 카운트에 더한다.
  *     - 아니면,
  *       + 충돌 결과값에 (FILE_CONFLICT | FILE_SYNC_ERR | FILE_SYNC_RENAMED)이 없으면 Enable을 FALSE로 하고, 카운트를 1 증가한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */
  int CheckEnableOnResolveConflict();

  /**
  * @brief 이 항목 아래의 하위 항목을 지정한 부모 폴더의 아래로 이동한다.
  * @param newParent 새 부모 폴더
  * @return int : 이동된 항목의 갯수를 리턴한다.
  */
  int MoveChildItemTo(KSyncFolderItem *newParent);

  /**
  * @brief 이 항목 아래의 하위 항목을 지정한 부모 폴더의 아래로 이동한다.
  * @param clones : 복제 항목 리스트
  * @param org : 원 폴더 개체
  * @param newLocalPath : 새로운 로컬측 경로
  * @param newServerPath : 새로운 서버측 경로
  * @return KSyncFolderItem* : 이동된 폴더 항목을 리턴한다.
  * @details A. 복제 항목 리스트에서
  *     - 리스트 내의 항목에 KSyncFolderItem::IsMatchDummyClonePath()를 호출하여 TRUE이면 그 항목을 리턴한다.
  *     .
  * @n B. 원 폴더 항목이 루트 폴더이면 KSyncRootFolderItem를 생성하고, 아니면 KSyncFolderItem를 복제 개체로 생성한다.
  * @n C. 내용을 복사하고, SetDummyCloneFolder(), SetDummyClonePath()를 호출하여 복제 폴더 속성을 설정하고,
  * @n D. 복제 항목 리스트에 추가한 후 해당 개체를 리턴한다.
  */
  KSyncFolderItem* GetMovedCloneFolder(KSyncItemArray& clones, KSyncFolderItem* org, LPCTSTR newLocalPath, LPCTSTR newServerPath);

  // check if parent does not checked, but child is checked
  /**
  * @brief 동기화 충돌 처리창에서 하위 항목은 선택되고 부모 항목은 선택이 안된 상태로 동기화를 진행하면 안되기 때문에 이러한 조건이 있는지 검사한다.
  * @param p : 선택되지 않은 부모 폴더의 경로명을 받아가는 버퍼
  * @details A. 이 항목이 Disable 상태이고, IsConflictResolved()이고, 루트 폴더가 아니면,
  *     - GetEnableChild()을 호출하여 Enable된 하위 항목이 존재하면,
  *       + 버퍼 p가 NULL이 아니면, 이 항목의 mBaseFolder(루트 폴더이면 "\")를 버퍼 p에 복사하고
  *       + TRUE를 리턴한다.
  *     .
  * @n B. 하위 항목들에서
  *     - 하위 항목이 폴더이고, KSyncFolderItem::IsChildCheckedWithoutParent()를 재귀 호출하여 TRUE이면, TRUE를 리턴한다.
  *     .
  * @n C. FALSE를 리턴한다.
  */ 
  BOOL IsChildCheckedWithoutParent(LPTSTR *p);

  /**
  * @brief 지정한 경로명의 개체가 존재하는지 확인한다.
  * @param relativePathName : 찾을 개체의 상대 경로명.
  * @param onLocal : 로컬측 또는 서버측
  * @param ex : 예외로 처리할 항목, 찾은 결과가 이 개체이면 무시한다.
  * @details A. 하위 항목들에 대하여
  *     - 항목이 루트 폴더이면 KSyncFolderItem::FindChildItemCascade()로 상대 경로명에 해당하는 개체를 찾아온다.
  *     - 결과가 NULL이 아니고, ex도 아니면 결과를 리턴한다.
  *     .
  * @n B. 하위 항목들에 대하여
  *     - 항목이 루트 폴더이면 KSyncFolderItem::FindRenamedChildItem()로 상대 경로명에 해당하는 개체를 찾아온다.
  *     - 결과가 NULL이 아니면 결과를 리턴한다.
  *     .
  * @n C. NULL을 리턴한다.
  */ 
  KSyncItem* CheckIfExistOnVirtualTop(LPTSTR relativePathName, BOOL onLocal, KSyncItem* ex);

  /**
  * @brief 동기화 충돌 처리창에서 대상 항목을 서버(로컬)로 적용하려는데, 이미 동일한 파일이 존재하는지 검사한다.
  * @param root : 동기화 루트 폴더 항목
  * @param applyToServer : TRUE이면 서버로 적용하는 경우, 아니면 로컬로 적용하는 경우.
  * @param name : 충돌 발생된 항목 이름을 받아가는 버퍼.
  * @return BOOL : 동일한 이름이 있으면 TRUE를 리턴한다.
  * @details A. 하위 항목들에 대하여
  *     - 항목이 Enable이고, 루트 폴더가 아니면,
  *       + applyToServer가 TRUE이고, 항목의 로컬 파일이 존재하고, 항목의 로컬 상태에 ITEM_STATE_EXISTING가 있거나,
  * applyToServer가 FALSE이고, 항목의 서버 파일이 존재하고, 항목의 서버 상태에 ITEM_STATE_EXISTING가 있으면,
  *         + 동기화 결과 경로명을 준비하고,
  *         + KSyncFolderItem::CheckIfExistOnVirtualTop()을 호출하여 대상항목을 찾아오고,
  *         + 대상 항목이 존재하고 이것이 이 하위 항목과 다르면,
  *           + name에 동기화 결과 경로명을 복사하고, TRUE를 리턴한다.
  *     .
  * @n B. 하위 항목들에 대하여
  *     - 항목이 폴더이면 KSyncFolderItem::CheckIfCrossConflict()를 재귀호출하여 결과가 TRUE이면 TRUE를 리턴한다.
  *     .
  * @n C. FALSE를 리턴한다.
  */ 
  BOOL CheckIfCrossConflict(KSyncFolderItem*root, BOOL applyToServer, LPTSTR *name);

  /**
  * @brief 하위 항목들을 조건에 따라 정렬한다.
  * @param sortColumn : 정렬할 컬럼
  * @param sortAscent : 0이 아니면 오름 차순
  * @return int : 하위 항목의 개수를 리턴한다.
  * @details A. 하위 항목의 개수가 1보다 크면
  *     - 하위 항목들을 임시 어레이에 넣어두고, 하위 항목 리스트는 비운다.
  *     - 임시 어레이의 항목들에 대해
  *       + 하위 항목 리스트에 순서대로 CompareSyncItem()로 비교하여  결과가 0보다 작으면 앞에 삽입한다.
  *       + 아니면 맨 뒤에 삽입한다.
  *     .
  * @n B. 하위 항목들에 대하여
  *     - 항목이 폴더이면 KSyncFolderItem::SortChildList()를 재귀호출하여 하위의 하위 항목들도 정렬시킨다.
  *     - 결과값을 카운트에 더한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */ 
  int SortChildList(int sortColumn, int sortAscent);

  /**
  * @brief 이 개체를 포함하여 하위 항목중에 선택된 항목이 있으면 TRUE를 리턴한다.
  * @details A. KSyncItem::HasSelected()으로 이 개체가 선택되어 있으면 TRUE를 리턴한다.
  * @n B. 하위 항목들에 대하여
  *     - 항목이 선택되어 있으면 TRUE를 리턴한다.
  *     .
  * @n C. FALSE를 리턴한다.
  */ 
  virtual BOOL HasSelected();

  /**
  * @brief 이 개체를 포함하여 하위 항목중에 선택된 항목을 Enable로 설정한다.
  * @details A. KSyncItem::EnableSelectedItem()으로 이 개체가 선택되어 있으면 Enable로 설정한다.
  * @n B. 하위 항목들에 대하여
  *     - KSyncItem::EnableSelectedItem()으로 이 항목이 선택되어 있으면 Enable로 설정한다.
  *     .
  */ 
  virtual void EnableSelectedItem();

  /**
  * @brief 이 개체를 포함하여 하위 항목에 ResolveFlag에 지정한 값을 마스크하고 더한다.
  * @details A. KSyncItem::SetResolveFlagMask()으로 이 개체의 ResolveFlag에 지정한 값을 마스크하고 더한다.
  * @n B. 하위 항목들에 대하여
  *     - KSyncItem::IsResolveVisible()이면,
  *       + KSyncItem::SetResolveFlagMask()으로 이 개체의 ResolveFlag에 지정한 값을 마스크하고 더한다.
  *     .
  */ 
  virtual void SetResolveFlagMask(int a, int o, BOOL withChild);

  /**
  * @brief 동기화 충돌 처리 플래그값(mResolveFlag)이 지정한 조건에 해당되는 개체들의 인덱스 범위를 구한다.
  * @param a : AND로 마스크할 값.
  * @param o : 비교값.
  * @param index : 개체의 index값.
  * @param area : 개체의 인덱스 범위를 저장할 버퍼.
  * @param flag : 플래그 값.
  * @details A. KSyncItem::GetResolveFlagArea()를 호출하여 이 개체의 mResolveFlag를 검사하고 범위를 구한다.
  * @n B. 이 폴더가 열린 상태이고, IsResolveVisible()이고, (flag & WITH_CHILD_DOCUMENT)이면,
  *     - 하위 항목들에 대하여,
  *       - 하위 항목이 IsResolveVisible()이고, 파일이면,
  *         + 이 항목에 대해 KSyncItem::GetResolveFlagArea()를 호출하여 이 개체의 mResolveFlag를 검사하고 범위를 구한다.
  *     - (flag & WITH_CHILD_FOLDER)이면,(원래 폴더에 있던 개체와 더미 복제 폴더에 속한 개체가 섞인 순서로 트리에 표시되니 아래와 같은 순서로 처리한다)
  *       + 이개체가 루트 폴더이고, 더미 복제 폴더가 아니면,
  *         + FindDummyPairItems()로 더미 복제 폴더들의 어레이를 받아온다.
  *         + 더미 복제 폴더들의 어레이의 하위 항목들에 대해
  *           + 그 항목이 IsVisible()이면, KSyncItem::GetResolveFlagArea()를 호출하여 이 개체의 mResolveFlag를 검사하고 범위를 구한다.
  *       + GetResolveChildItems()으로 더미가 아닌 하위 항목들의 어레이를 itemArray에 받아온다.
  *         + 더미 복제 폴더들의 어레이의 하위 항목들에 대해
  *           + 오픈된 항목이면 GetResolveChildItems()으로 더미가 아닌 하위 항목들의 어레이를 itemArray에 받아온다.
  *       + GetResolveChildItems()으로 IN_SINGLE_DUMMY인 하위 항목들의 어레이를 itemArray에 받아온다.
  *       + 위의 스텝에서 받은 itemArray의 하위 항목들에 대해
  *         + KSyncItem::GetResolveFlagArea()를 호출하여 이 개체의 mResolveFlag를 검사하고 범위를 구한다.
  *     .
  */
  virtual int GetResolveFlagArea(int a, int o, int index, int* area, int flag);

  /**
  * @brief 동기화 충돌 처리창의 트리에 나타나는 본 항목과 하위 항목의 열 개수를 구한다.
  * @return int : 개수를 리턴한다.
  * @details A. IsVisible() 이고 IsResolveVisible()이면 카운트를 1로 한다.
  * @n B. 이 폴더 항목이 오픈된 상태이면,
  *     - 하위 항목들에 대하여
  *       + 항목이 KSyncItem::IsResolveVisible()이면 이 항목에 대해 GetChildRowCount()를 재귀 호출하여 리턴값을 카운트에 더한다.
  *     .
  * @n C. 카운트값을 리턴한다.
  */
  virtual int GetChildRowCount();

  /**
  * @brief 하위 항목중에 지정한 이름과 동일한 이름이 존재하는지 확인한다.
  * 동일한 이름이 존재하고 그것이 최종 이름이면(다른 이름으로 바뀔 예정이 아니면) 임시 이름 바꾸기로 해결되지 못하니 충돌로 처리한다.
  * @param childName : 찾을 이름
  * @param except : 이름이 같아도 예외로 할 항목
  * @param checkIfMoved : 0 : 상태 체크 안함, 1 : 로컬 상태, 2 : 서버 상태.
  * @param onLocal : TRUE이면 로컬측 이름 변경 확인, FALSE이면 서버측 이름 변경 확인.
  * @return int : 0이면 동일한 이름 없음, 1이면 동일한 이름이 존재함, 2이면 동일한 이름이 존재하고 변경되지 않을것임.
  * @details A. 하위 항목들에 대하여
  *     - 항목이 except가 아니면,
  *       + 항목의 이름이 지정한 childName과 일치하거나,  항목의 새 이름(mLocalNew.Filename)이 childName과 일치하면,
  *         + 이 항목이 임시로 이름이 바뀐 것(IsServerTemporaryRenamed())이 아니면,
  *           + checkIfMoved가 0이 아니면,
  *             + checkIfMoved가  2이고, 서버 상태가 ITEM_STATE_MOVED이거나 IsLocalTemporaryRenamed()인 것이 아니면 2을 리턴한다.
  *             + checkIfMoved가  1이고, 로컬 상태가 ITEM_STATE_MOVED이거나 IsServerTemporaryRenamed()인 것이 아니면 2을 리턴한다.
  *           + 1을 리턴한다.
  *     .
  * @n B. 0을 리턴한다.
  */
  int CheckIfChildNameExist(LPCTSTR childName, KSyncItem* except, int checkIfMoved, BOOL onLocal);

  /**
  * @brief 이동 처리시에 동일한 이름의 다른 항목이 존재하여 임시 이름으로 바꾸어 처리할 때 호출되며, 임시 이름을 만들어 리턴한다.
  * @param childName : 원래의 이름
  * @param resultName : 임시 이름을 저장할 버퍼
  * @param side : 1이면 로컬측, 2이면 서버측
  * @return BOOL : 성공하였으면 TRUE를 리턴한다.
  * @details A. resultName에 원래의 이름을 복사한다.
  * @n B. resultName에 ".AAA"를 붙인다.
  * @n C. CheckIfChildNameExist()으로 resultName에 해당하는 파일이 존재하는지 확인하고, 존재하지 않으면 TRUE를 리턴한다.
  * @n D. MakeNextSequenceName()을 호출하여 "AAA" 부분을 다음 문자열로 바꾼다.(AAA, AAB, AAC,,,ABA,,,ZZZ)
  * @n E. 다음 문자열로 바꾸었으면 C로 이동한다.
  * @n F. FALSE를 리턴한다.
  */
  BOOL ReadyUniqueChildName(LPCTSTR childName, LPTSTR resultName, int side, BOOL onLocal);

  /**
  * @brief 로컬측에 유일한 임시 이름을 만들어 할당된 메모리를 리턴한다.
  * @param pathname : 원래의 이름
  * @return LPTSTR : 새로 만든 임시 이름을 저장한 버퍼주소.
  * @details A. 결과 스트링을 저장할 버퍼를 할당하고, 여기에 pathname을 복사한다.
  * @n B. 결과 스트링에 ".AAA"를 붙인다.
  * @n C. IsDirectoryExist()으로 해당 폴더가 존재하는지 확인하고, 존재하지 않으면 결과 스트링을 리턴한다.
  * @n D. MakeNextSequenceName()을 호출하여 "AAA" 부분을 다음 문자열로 바꾼다.(AAA, AAB, AAC,,,ABA,,,ZZZ)
  * @n E. 다음 문자열로 바꾸었으면 C로 이동한다.
  * @n F. NULL을 리턴한다.
  */
  virtual LPTSTR ReadyUniqueLocalName(LPCTSTR pathname);

  /**
  * @brief 이동 처리시에 동일한 이름의 다른 항목이 존재하여 임시 이름으로 바꾸어 처리된 개체에서, 마무리 작업으로 원래 이름으로 바꾸어 놓는다.
  * @return int : 원래 이름으로 복구한 항목의 카운트를 리턴한다.
  * @details A. 이 개체가 루트 폴더 항목이 아니면,
  *     - 서버측이 임시 이름으로 바뀌어 있으면(IsServerTemporaryRenamed()),
  *       + KSyncCoreEngine::ChangeFolderName()으로 원래의 이름으로 바꾸고 성공하면,
  *         + 카운트를 1 증가하고, RefreshServerState()으로 서버 상태를 업데이트하고, 이 개체의 이름 mLocal.Filename도 바꾸고, mpServerTempRenamed를 해제한다.
  *     - 로컬측이 임시 이름으로 바뀌어 있으면(IsLocalTemporaryRenamed()),
  *       + KMoveFile()으로 원래의 이름으로 바꾸고 성공하면,
  *         + 카운트를 1 증가하고, 이 개체의 이름 mLocal.Filename도 바꾸고, mpLocalTempRenamed를 해제한다.
  *     - 카운트가 0보다 크면
  *       + mBaseFolder를 업데이트하고, MoveMetafileAsRename()을 호출하여 메타 데이터 경로명을 업데이트하고, 
  *       + UpdateChildItemPath()을 호출하여 하위 항목의 경로도 업데이트하고,
  *       + StoreMetafile()으로 메타 데이터를 저장한다.
  *     .
  * @n B. 하위 항목에 대해
  *     - KSyncItem::RestoreTemporaryRenamed()를 재귀 호출한다.
  *     .
  * @n C. 카운트 값을 리턴한다.
  */
  virtual int RestoreTemporaryRenamed();

  /**
  * @brief 로컬측의 이름 변경을 처리하는 가상함수이다.
  * @param baseFolder : 동기화 루트 폴더 경로면
  * @param str : 변경할 새 이름
  * @return int : 성공하면 R_SUCCESS를, 실패하면 오류 코드를 리턴한다.
  * @details A. 현재의 경로명을 복사해둔다.
  * @n B. KSyncItem::SetLocalRename()으로 이름 변경을 처리한다.
  * @n C. 위의 결과가 성공하면 UpdateChildItemPath()을 호출하여 하위 항목들의 경로를 업데이트한다.
  * @n D. 결과값을 리턴한다.
  */
  virtual int SetLocalRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief 서버측의 이름 변경을 처리하는 가상함수이다.
  * @param baseFolder : 동기화 루트 폴더 경로면
  * @param str : 변경할 새 이름
  * @return int : 성공하면 R_SUCCESS를, 실패하면 오류 코드를 리턴한다.
  * @details A. 현재의 경로명을 복사해둔다.
  * @n B. KSyncItem::SetLocalRename()으로 이름 변경을 처리한다.
  * @n C. 위의 결과가 성공하면 UpdateChildItemPath()을 호출하여 하위 항목들의 경로를 업데이트한다.
  * @n D. 결과값을 리턴한다.
  */
  virtual int SetServerRename(LPCTSTR baseFolder, LPCTSTR str);

  /**
  * @brief 로컬 및 서버측 더미 복제 경로명을 설정한다.
  * @param localPath : 로컬측 더미 복제 경로명
  * @param serverPath : 서버측 더미 복제 경로명
  * @details A. 현재의 mpDummyLocalPath, mpDummyServerPath를 해제한다.
  * @n B. 지정한 새로운 경로명을 각각 복사한다.
  */
  void SetDummyClonePath(LPCTSTR localPath, LPCTSTR serverPath);

  /**
  * @brief 이 개체가 로컬 및 서버측 더미 복제 경로명과 일치하는지 확인한다.
  * @param localPath : 로컬측 더미 복제 경로명
  * @param serverPath : 서버측 더미 복제 경로명
  * @return BOOL : 이 개체가 일치하면 TRUE를 리턴한다.
  * @details A. 이 개체가 더미 복제 폴더이면,
  *     - 지정한 localPath, serverPath가 NULL이면 TRUE를 리턴한다.
  *     - localPath와 로컬측 더미 복제 경로가 유효한 스트링임이 다르면 FALSE를 리턴한다.
  *     - serverPath와 서버측 더미 복제 경로가 유효한 스트링임이 다르면 FALSE를 리턴한다.
  *     - localPath와 로컬측 더미 복제 경로가 모두 유효한 스트링이며,
  *       + 두 경로가 다르면 FALSE를 리턴한다.
  *     - serverPath와 서버측 더미 복제 경로가 모두 유효한 스트링이며,
  *       + 두 경로가 다르면 FALSE를 리턴한다.
  *     - TRUE를 리턴한다.
  *     
  * @n B. FALSE를 리턴한다.
  */
  BOOL IsMatchDummyClonePath(LPCTSTR localPath, LPCTSTR serverPath);

  /**
  * @brief 이 개체와 하위 개체의 메타 데이터를 저장한다.
  * @return : 이 폴더 개체 하위에 처리할 항목이 남아 있으면 TRUE를 리턴한다.
  * @details A. mChildItemIndex가 0이면 자신의 메타 데이터를 저장한다.
  * @n B. mChildItemIndex가 0보다 크면 child 항목의 인덱스를 증가한다.
  * @n C. 하위 항목중에 파일들에 대해
  *     - StoreMetafile()을 호출하여 메타 데이터에 저장하고 카운트를 1 증가한다.
  *     - 카운트가 지정한 숫자(30)가 되면 TRUE를 리턴한다.
  *     .
  * @n D. 더 이상 처리할 하위 항목이 없으니 FALSE를 리턴한다.
  */
  BOOL ContinueStoreMetafile(KSyncRootStorage* storage);

  virtual void SetOverridePath(int callingDepth, KSyncRootStorage* s, LPCTSTR path);

  void CheckClearOverridePath(KSyncRootStorage* s);

#ifdef _DEBUG
  virtual void DumpSyncItems(int depth);
#endif

private:
#ifdef USE_BASE_FOLDER
  /** 이 폴더의 상대 경로명 */
  LPTSTR mBaseFolder;
#endif
  /** 하위 항목 리스트 */
  KSyncItemArray mChildList;
  /** 로컬측 폴더를 스캔할 때 사용되는 FindFirstFile 핸들 */
  HANDLE mLocalScanHandle;
  /** 동기화 처리 작업시에 처리 완료한 하위 항목의 인덱스 */
  int mChildItemIndex;
  /** 충돌 처리창에서 더미 복제 항목의 로컬측 경로 */
  LPTSTR mpDummyLocalPath;
  /** 충돌 처리창에서 더미 복제 항목의 서버측 경로 */
  LPTSTR mpDummyServerPath;

  friend class KSyncRootFolderItem;

};

#endif
