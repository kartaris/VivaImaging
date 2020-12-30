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
* @brief 동기화 루트 폴더 항목 클래스
*/
class KSyncRootFolderItem : public KSyncFolderItem
{
public:
  KSyncRootFolderItem(LPCTSTR pathname);
  KSyncRootFolderItem(LPCTSTR pathname, LPCTSTR filename, LPCTSTR server_oid, LPCTSTR server_file_oid, LPCTSTR server_storage_oid);
  KSyncRootFolderItem(KSyncRootFolderItem &item);

  virtual ~KSyncRootFolderItem(void);

  /**
  * @brief KSyncRootFolderItem object의 내용을 복사하고 개체 reference를 리턴함
  * @return KSyncRootFolderItem&
  * @param item : 소스 개체
  */
  KSyncRootFolderItem &operator=(KSyncRootFolderItem& item);

private:

  /**
  * @brief 이 개체의 내부 멤버 변수들을 초기화한다.
  */
  void InitFolderItem();

public:
  /**
  * @brief 동기화 개체의 종류를 리턴하는 가상함수.
  * @return int : 동기화 개체의 종류(0=file, 1=folder, 2=root folder)
  */
  virtual int IsFolder() { return 2; }

  /**
  * @brief 서버측의 상대 경로명을 리턴하는 가상함수. 루트 폴더이니 NULL을 리턴한다.
  * @return LPCTSTR : 상대 경로명
  */
  virtual LPCTSTR GetServerRelativePath() { return NULL; }

#ifdef USE_SYNC_ENGINE
  /**
  * @brief 동기화 메타 데이터를 읽고 저장하는 스토리지 클래스를 리턴한다.
  * 1.x에서는 KSyncRootStorageFile 클래스를 통해 메타 파일로 저장하고,
  * 2.x에서는 KSyncRootStorageDB 클래스를 통해 Sqlite3 DB를 이용해 저장한다.
  * @param readonly : 읽기 전용으로 열기
  * @return KSyncRootStorage* : 스토리지 클래스
  * @details A. mStorage가 NULL이 아니고 mStorage의 ReadOnly 속성이 지정한 readonly 속성과 다르면 지금의 mStorage를 클리어한다.
  * @n B. mStorage가 NULL이면,
  *     - KSyncRootStorageFile::IsMatchRootFolder()으로 이 동기화 폴더가 1.x의 파일 스토리지이면,
  * KSyncRootStorageFile 개체를 생성하여 mStorage로 한다.
  *     - 아니고 KSyncRootStorageDB::IsMatchRootFolder()으로 이 동기화 폴더가 2.x의 DB 스토리지이면,
  * KSyncRootStorageDB 개체를 생성하여 mStorage로 한다.
  *     .
  * @n C. mStorage를 리턴한다.
  */
  KSyncRootStorage* GetStorage(BOOL readonly = FALSE);
#else
  KSyncRootStorage* GetStorage(BOOL readonly = TRUE);
#endif

  /**
  * @brief 동기화 메타 데이터를 읽고 저장하는 스토리지를 닫는다.
  * @details A. mStorage가 NULL이 아니면 삭제하고 해제한다.
  */
  void CloseStorage();

  /**
  * @brief 스토리지에서 Policy 정보를 로드한다.
  */
  BOOL LoadSyncPolicy(KSyncRootStorage* s);

  /**
  * @brief 지정한 상대 경로명 하위의 동기화 메타 데이터를 읽어 동기화 항목들을 로드한다.
  * @param relativePath : 상대 경로명
  * @param child_flag : 플래그
  * @details A. 상대 경로명을 subPath에 복사한다.
  * @details B. subPath에서 앞쪽 첫번째 경로만 자른다.(relativePath가 "a/b/c/" 이라면 첫번에는 "a", 두번째에는 "a/b", 세번째에는 "a/b/c")
  * @details C. KSyncRootStorage::LoadChildMetaData()를 호출하여 해당 경로 아래의 항목을 로드한다.
  * @details D. 마지막 폴더까지 B단계를 반복한다.
  */
  int LoadChildItemCascade(LPCTSTR relativePath, int child_flag);

  /**
  * @brief 지정한 경로명의 폴더들이 로드되어 있는지 확인하고 없으면 storage에서 생성하고 로드한다.
  * @param pathName : 대상 항목 경로명
  */
  KSyncItem* ReadyChildItemCascade(KSyncFolderItem* p, LPTSTR pathName);

  /**
  * @brief 지정한 OID에 해당하는 동기화 항목을 생성하고 로드한다.
  * @param itemOID : 대상 항목 OID
  */
  KSyncItem* LoadItemFromStorage(LPCTSTR itemOID);

  /**
  * @brief 지정한 상대 경로명 하위의 동기화 메타 데이터를 읽어 동기화 항목들을 로드한다.
  * @param relativePath : 상대 경로명
  * @param child_flag : 플래그
  * @details A. KSyncRootStorage::LoadChildMetaData()를 호출하여 해당 경로 아래의 항목을 로드하고 그 리턴값을 리턴한다.
  */
  int LoadChildItem(LPCTSTR relativePath, int child_flag);

  /**
  * @brief 동기화 진행 상태 정보를 리턴한다.
  * @param progress : 진행 상태 정보를 저장할 버퍼
  * @details A. 동기화 전체 크기, 루트 폴더 이름, 전체 개수, 완료된 폴더 개수, 완료된 파일 개수를 복사한다.
  * @n B. mPhase가 PHASE_COMPARE_FILES보다 작으면 진행률은 0으로 한다.
  * @n C. 아니면, 진행률은 (완료된 진행 크기 + 다운로드 진행중인 파일의 완료 크기) / 전체 진행 크기로 계산하고, 10000을 곱하여 integer로 복사한다.
  * @n D. 진행률이 0보다 크면,
  *     - 현재의 시간을 받아오고
  *     - GetRemainedTime()으로 남은 예정시간을 계산하여 온다.
  *     .
  */
  void GetSyncFileProgress(SYNC_PROGRESS_STRUCT &progress);

  /**
  * @brief 동기화 시작 시간, 현재 시간, 진행율로부터 남은 예정 시간을 계산한다.
  * @param start : 시작 시간
  * @param end : 지금 시간
  * @param percent : 진행율(0에서 1 사이의 값)
  * @param remain : 계산된 남은 예정 시간을 저장할 버퍼
  * @return BOOL : 
  * @details A. TimeDifference()로 end - start를 구한다.
  * @n B. ellapsed time을 초 단위로 계산하여 ellapsed_sec값을 계산한다.
  * @n C. percent값이 0보다 크면, 남은 시간(초 단위)을 (ellapsed_sec / percent) - ellapsed_sec으로 계산한다.
  * @n D. 남은 초 단위 시간값이 0보다 크면 이 값을 SYSTEMTIME으로 변환하여 remain에 복사하고 TRUE를 리턴한다.
  * @n E. FALSE를 리턴한다.
  */
  static BOOL GetRemainedTime(SYSTEMTIME &start, SYSTEMTIME &end, float percent, SYSTEMTIME& OUT remain);

#ifdef USE_SCAN_BY_HTTP_REQUEST
  /**
  * @brief 동기화 진행 작업중 서버측 항목 검색하는 과정에서 검색 완료된 폴더와 파일 개수를 업데이트한다.
  * @param folders : 검색된 폴더 개수
  * @param files : 검색된 파일 개수
  * @details A. 값을 각각 mSyncFolderCount, mSyncFileCount에 저장한다.
  */
  void OnScanFolderProgress(int folders, int files);
#endif

  /**
  * @brief 동기화 진행 작업중 파일의 업로드/다운로드 진행시 완료된 크기값을 업데이트한다.
  * @param name : 업로드/다운로드 진행중인 파일명
  * @param progress : 완료율(0에서 10000사이의 값)
  * @details A. 값을 각각 mSyncFolderCount, mSyncFileCount에 저장한다.
  */
  void OnFileSyncProgress(LPCTSTR name, int progress);

  int MergeSameSyncJob(LPCTSTR pathname, int method, LPCTSTR syncOnlyPath);

  /**
  * @brief 지정한 동기화 폴더 경로명과 동기화 작업 플래그가 이 동기화 폴더 개체와 일치하는지 비교한다.
  * @param pathname : 동기화 폴더 경로명
  * @param method : 동기화 작업 Method.
  * @param syncOnlyPath : 부분 작업 경로.
  * @details A. GetPath()로 이 개체의 동기화 폴더 경로를 받아서 이를 pathname과 비교하고,
  * mSyncMethod와 flag가 맞으면 TRUE를 리턴한다.
  */
  int IsSameSyncJob(LPCTSTR pathname, int method, LPCTSTR syncOnlyPath);

  /**
  * @brief 지정한 동기화 폴더 경로명과 동기화 작업 플래그가 이 동기화 폴더 개체와 일치하는지 비교한다.
  * @param folder : 동기화 폴더 상위 경로명
  * @param name : 동기화 폴더 이름
  * @param method : 동기화 작업 Method.
  * @details A. folder와 name을 합쳐서 pathname으로 저장한다.
  * @n B. GetPath()로 이 개체의 동기화 폴더 경로를 받아서 이를 pathname과 비교하고,
  * mSyncMethod와 flag가 맞으면 TRUE를 리턴한다.
  */
  int IsSameSyncJob(LPCTSTR folder, LPCTSTR name, int method, LPCTSTR syncOnlyPath);

  BOOL MergeInstantBackupItem(LPCTSTR syncOnlyPath);

  /**
  * @brief 메타 데이터를 파일로 저장하기 위한 파일명을 준비한다.
  * @param s : 스토리지 개체
  * @param createIfNotExist : 파일이 없으면 새로 만든다.
  * @details A. 스토리지 클래스 형식이 파일(KSyncRootStorage::SRS_FILE)이면
  *     - mpMetafilename이 유효하면 리턴한다.
  *     - mpMetafilename에 동기화 루트 폴더 경로와 ".filesync/.objects/.root.ifo"를 붙여 복사한다.
  */
  virtual BOOL ReadyMetafilename(KSyncRootStorage *s, BOOL createIfNotExist);

  /**
  * @brief 로컬측 경로명을 지정한 버퍼에 복사한다.
  * @param fullname : 경로명을 저장할 버퍼
  * @param size : 버퍼의 크기
  * @param abs : TRUE이면 전체 경로를 복사한다.
  * @return int : 경로명 길이를 리턴한다.
  * @details A. abs가 TRUE이면,
  *     - fullname이 NULL이 아니면 mBaseFolder를 복사한다.
  *     - mBaseFolder 문자열 길이 + 1을 리턴한다.
  *     .
  * @n B. 아니면,
  *     - fullname이 NULL이 아니면 빈 스트링으로 만들고,
  *     - 0을 리턴한다.
  */
  virtual int GetActualPathName(LPTSTR fullname, int size, BOOL abs);

  /**
  * @brief 서버측 경로명을 리턴한다.
  */
  LPCTSTR GetServerFullPath();

#if 0
  /**
  * @brief 동기화 결과 이름을 지정한 버퍼에 복사한다.
  * @n deprecated
  * @param name : 이름을 저장할 버퍼
  * @param length : 버퍼의 크기
  * @param flag : AS_LOCAL이면 로컬측 이름을, 아니면 서버측 이름을 복사한다.
  * @details A. 서버측 이름이면,
  *     - GetServerFullPath()로 서버측 경로명을 받아와서
  *     - 마지막 '<'를 찾아 그 다음 글자 부분부터 name에 복사한다.
  *     .
  * @n B. 아니면, KSyncItem::GetSyncResultName()을 호출한다.
  */
  virtual void GetSyncResultName(LPTSTR name, int length, int flag);
#endif

  /**
  * @brief 동기화 정책 클래스를 리턴한다.
  */ 
  inline KSyncPolicy &GetPolicy() { return mPolicy; }

  /**
  * @brief 동기화 정책 클래스를 지정한 내용으로 덮어쓴다.
  */ 
  inline void SetPolicy(KSyncPolicy &policy) { mPolicy = policy; }

  /**
  * @brief 동기화 방식을 리턴한다.
  */ 
  inline int GetSyncMethod() { return mSyncMethod; }

  /**
  * @brief 동기화 방식을 설정한다.
  */ 
  inline void SetSyncMethod(int m) { mSyncMethod = m; }

  /**
  * @brief 마지막 백업 작업 상태 정보를 리턴한다
  */ 
  int GetUnfinishedBackupState(OUT LPTSTR* pLastBackupPath); //, OUT LPTSTR* pExtraInfo);

  /**
  * @brief 마지막 백업 작업 상태 정보를 저장한다
  */ 
  BOOL StoreUnfinishedBackupState(LPTSTR state);

  /**
  * @brief 마지막 백업 작업 상태 정보를 클리어한다
  */ 
  void ClearUnfinishedBackupState(int method);

#ifdef USE_BACKUP_JOB_LIST

  LPTSTR JobToString();

  static int ParseUnfinishedBackupState(LPTSTR state, LPTSTR *syncOnly, LPTSTR *key);

#endif


  /**
  * @brief 동기화 작업을 시작한다
  * @param sync_flag : 동기화 플래그
  * @return int : 계속 진행하려면 NEED_CONTINUE를, 오류가 발생되면 NEED_UI_FOR_SYNC_ERROR 등을 리턴한다.
  * @details A. 동기화 관련 변수들을 초기화한다.
  * @n B. (mSyncMethod & SYNC_METHOD_SYNCING)이면 ReadyMetafilename()으로 메타 파일을 준비한다.
  * @n C. 로컬측 정보를 갱신한다.
  * @n D. (mSyncMethod & SYNC_METHOD_SYNCING)이면,
  *     - 동기화 진행중임을 표시하도록 mResult에 FILE_NOW_SYNCHRONIZING을 더한다.
  *     - 스토리지 클래스를 준비하고, 현재 스토리지 파일의 크기에 16K를 더한 크기가 디스크에 사용 가능한지 확인하고,
  * 부족하면 오류 코드 FILE_SYNC_METAFILE_DISK_FULL로 동기화를 중단하고, NEED_UI_FOR_SYNC_ERROR를 리턴한다.
  *     - StoreMetafile()로 동기화 결과값을 메타 스토리지에 저장하고 실패하면, 로그에 내용을 저장하고,
  * 실패 코드가 ERR_DISK_FULL이면 동기화 오류를 FILE_SYNC_METAFILE_DISK_FULL로 하고, 
  * 아니면  동기화 오류를 FILE_SYNC_CANNOT_MAKE_METAFILE로 설정하고, NEED_UI_FOR_SYNC_ERROR를 리턴한다.
  *     - 동기화 시작됨을 히스토리에 기록한다.
  *     .
  * @n E. 로컬측 서버측 상태를 (ITEM_STATE_NOW_EXIST | ITEM_STATE_WAS_EXIST)로 하고,
  * 동기화 작업을 계속하도록 NEED_CONTINUE를 리턴한다.
  */ 
  int StartSync(int sync_flag);


  /**
  * @brief 다시 시도가 필요하면 동기화 작업을 다시 시작한다.
  * 다시시작하면 TRUE를 리턴한다.
  */
  BOOL RetryInstantSync();

  /**
  * @brief 동기화 작업을 계속한다.
  * @return int : 계속 진행하려면 NEED_CONTINUE를, 오류가 발생되면 NEED_UI_FOR_SYNC_ERROR 등을 리턴한다.
  * @details A. 충돌 결과값에 동기화 오류가 있으면 NEED_UI_FOR_SYNC_ERROR를 리턴한다.
  * @n B. mPhase가 PHASE_SCAN_OBJECTS와 같거나 적으면,
  *     - KSyncRootStorage::LoadChildMetaData()을 호출하여 메타 데이터를 로드한다.
  *     - 로드된 폴더 갯수 파일 갯수를 mSyncFolderCount, mSyncFileCount에 받아온다.
  *     - 메타 데이터 로드 결과값이 R_CONTINUE이면 NEED_CONTINUE를 리턴한다.
  *     - mSyncMethod가 동기화 충돌 처리(SYNC_METHOD_RESOLVE_CONFLICT)이면,
  *       + RemoveUnconflictItem()를 호출하여 충돌이 아닌 항목을 제거하고,
  *       + RemoveExcludedItems()으로 제외 파일들을 제외하고,
  *       + UpdateFolderSyncResult()으로 동기화 결과를 업데이트하고,
  * 동기화 충돌 처리창을 띄우기 위해 NEED_UI_FOR_CONFLICT를 리턴한다.
  *     - mPhase를 (PHASE_SCAN_FOLDERS_SERVER - 1)로 바꾼다.
  *     .
  * @n C. mPhase가 PHASE_SCAN_FOLDERS_SERVER와 같거나 적으면(서버에서 변동된 목록을 받아온다),
  *     - processScanFolderData()를 호출하여 서버측의 변동된 목록을 받아오는 작업을 처리하고 리턴값이 TRUE이면 NEED_CONTINUE를 리턴한다.
  *     - GetNextChild(PHASE_SCAN_FOLDERS_SERVER)를 호출하여 서버측의 변동된 목록을 받아올 필요가 있는 폴더를 찾고, NULL이 아니면
  *       + IsSyncNeedFolder()으로 처리할 폴더인지 확인하고,
  *         + KSyncFolderItem::scanSubFoldersServer()를 호출하여 서버측의 변동된 목록을 요청하는 작업을 시작한다.
  * 이 결과값이 R_SUCCESS가 아니면 그 값을 리턴한다.
  *         + KSyncFolderItem::RemoveExcludeNamedItems()으로 동기화 제외 형식에 해당하는 항목을 제거한다.
  *       + 이 폴더의 phase를 PHASE_SCAN_FOLDERS_SERVER로 하고 NEED_CONTINUE를 리턴한다.
  *     - 처리할 폴더가 더이상 없으면 SetPhaseServerNotExistFolders()을 호출하여
  * 서버측에 존재하지 않는 항목의 phase를 PHASE_SCAN_SERVER_FILES로 설정한다.
  *     - 처리된 폴더 및 파일 개수를 클리어하고 mPhase를 (PHASE_SCAN_FOLDERS_SERVER + 1)로 바꾼다.
  *     .
  * @n D. mPhase가 PHASE_SCAN_FOLDERS_LOCAL와 같거나 적으면(로컬측 폴더를 검색한다),
  *     - GetNextChild(PHASE_SCAN_FOLDERS_LOCAL)로 항목 중에서 그 phase가 PHASE_SCAN_FOLDERS_LOCAL보다 작은 폴더 항목을 찾아온다.
  *     - 해당하는 폴더가 있으면,
  *       + IsSyncNeedFolder()으로 동기화 작업이 필요한 폴더이면,
  *         + KSyncFolderItem::scanSubFoldersLocal()을 호출하여 이 폴더의 하위에 있는 폴더들을 검색한다.
  *         + KSyncFolderItem::RemoveExcludeNamedItems()으로 동기화 제외 형식에 해당하는 항목을 제거한다.
  *       + 이 폴더의 phase를 PHASE_SCAN_FOLDERS_LOCAL로 설정하고 NEED_CONTINUE를 리턴한다.
  *     - 더이상 폴더가 없으면
  *       + DeployFolderMovement()을 호출하여 이름 변경 정보를 설정하고, 전체 항목 갯수를 업데이트한다.
  *     - 동기화 완료 갯수를 초기화하고, mPhase를 (PHASE_SCAN_FOLDERS_LOCAL+1)로 한다.
  *     .
  * @n E. mPhase가 PHASE_SCAN_LOCAL_FILES와 같거나 적으면(로컬측 파일를 검색한다),
  *     - GetNextChild(PHASE_SCAN_LOCAL_FILES)로 항목 중에서 그 phase가 PHASE_SCAN_LOCAL_FILES보다 작은 폴더 항목을 찾아온다.
  *     - 해당하는 폴더가 있으면,
  *       + IsSyncNeedFolder()으로 동기화 작업이 필요한 폴더이면,
  *         + KSyncFolderItem::scanSubFilesLocal()을 호출하여 이 폴더의 하위에 있는 파일들을 검색한다.
  *       + 아니면,
  *         + KSyncFolderItem::ClearSubFiles()을 호출하여 하위의 파일들을 제거한다.
  *       + 이 폴더에 대한 파일 검색이 완료되었으면,
  *         + KSyncFolderItem::RemoveExcludeNamedItems()으로 동기화 제외 형식에 해당하는 항목을 제거한다.
  *         + 이 폴더의 phase를 PHASE_SCAN_LOCAL_FILES로 설정한다
  *       + NEED_CONTINUE를 리턴한다.
  *     - 더이상 폴더가 없으면
  *       + 동기화 정책의 동기화 플래그에 SYNC_SERVER_LOCATION_CHANGED이 있으면
  * ClearServerSideInfoCascade()를 호출하여 서버측 정보를 모두 지운다.
  *     - 동기화 완료 갯수를 초기화하고, mPhase를 (PHASE_SCAN_LOCAL_FILES+1)로 한다.
  *     .
  * @n F. mPhase가 PHASE_COMPARE_FILES와 같거나 적으면(양쪽 상태를 비교한다),
  *     - GetNextChild(PHASE_COMPARE_FILES)로 항목 중에서 그 phase가 PHASE_COMPARE_FILES보다 작은 폴더 항목을 찾아온다.
  *     - 해당하는 폴더가 있으면,
  *       + IsSyncNeedFolder()으로 동기화 작업이 필요한 폴더이면,
  *         + KSyncFolderItem::compareFiles()을 호출하여 이 폴더의 하위에 있는 파일들을 비교한다.
  *       + 아니면,
  *         + 스킵된 내용을 로그에 기록한다.
  *       + 이 폴더의 비교 작업이 완료되었으면 이 폴더의 phase를 PHASE_COMPARE_FILES로 설정한다
  *       + 완료된 항목의 개수를 업데이트하고, NEED_CONTINUE를 리턴한다.
  *     - 더이상 폴더가 없으면
  *       + SetUnchangedItemPhase()로 변동이 없은 항목들은 phase를 PHASE_ENDING으로 설정한다.
  *       + 동기화 완료 갯수를 초기화하고, mPhase를 (PHASE_LOAD_FILES)로 한다.(루트 폴더는 폴더 생성, 폴더 이동 phase를 처리하지 않도록 함)
  *     - 동기화 업로드 다운로드 완료 예정시간 계산을 위해, 현재 시간을 mSyncStartTime에 받아 놓는다.
  *     .
  * @n G. mPhase가 PHASE_CREATE_FOLDERS와 같거나 적으면(하위 폴더를 생성한다),
  *     - GetNextChild(PHASE_CREATE_FOLDERS)로 항목 중에서 그 phase가 PHASE_CREATE_FOLDERS보다 작은 폴더 항목을 찾아온다.
  *     - 해당하는 폴더가 있으면,
  *       + ReadyResolve()를 호출하여 상태를 다시한번 업데이트하고 비교한다.
  *       + syncFileItem()을 호출하여 폴더 생성을 처리한다.
  *     - 없으면,
  *       + mPhase를 (PHASE_MOVE_FOLDERS)로 한다
  *     .
  * @n H. mPhase가 PHASE_MOVE_FOLDERS와 같거나 적으면(항목의 이동 및 이름 변경을 처리한다),
  *     - GetNextChild(PHASE_MOVE_FOLDERS)로 항목 중에서 그 phase가 PHASE_MOVE_FOLDERS보다 작은 폴더나 파일을 항목을 찾아온다.
  *     - 해당하는 항목이 있으면,
  *       + ReadyResolve()를 호출하여 상태를 다시한번 업데이트하고 비교한다.
  *       + syncFileItem()을 호출하여 이동을 처리한다.
  *     - 없으면,
  *       + RemoveExcludedItems()을 호출하여 제외 폴더들을 제거한다.
  *       + RestoreTemporaryRenamed()을 호출하여 임시 이름 변경된 항목들을 원 이름으로 변경한다.
  *       + mPhase를 (PHASE_COPY_FILES)로 한다
  *     .
  * @n I. 동기화 폴더의 로컬측 위치가 보안 드라이브 영역이 아니면,
  *     - CheckDiskFreeSpace()으로 여유 공간이 100K이상인지 확인한다. 부족하면,
  *       + 동기화 오류를 FILE_SYNC_NO_CDISK_SPACE로 하고, NEED_UI_FOR_SYNC_ERROR를 리턴한다.
  *     .
  * @n J. mPhase가 PHASE_COPY_FILES와 같거나 적으면(업로드/다운로드를 처리한다),
  *     - GetNextChild(PHASE_COPY_FILES)로 항목 중에서 그 phase가 PHASE_MOVE_FOLDERS보다 작은 폴더 항목을 찾아온다.
  *     - 항목이 없으면 파일 항목을 찾아온다.
  *     - 해당하는 항목이 있으면,
  *       + ReadyResolve()를 호출하여 상태를 다시한번 업데이트하고 비교한다.
  *       + syncFileItem()을 호출하여 업로드 또는 다운로드를 처리한다.
  *     - 해당하는 항목이 없으면,
  *       + UpdateFolderSyncResult()로 결과를 업데이트하고,
  *       + mSyncMethod가 동기화 충돌 처리(SYNC_METHOD_RESOLVE_CONFLICT)이면,
  * 동기화 충돌 처리창을 띄우기 위해 NEED_UI_FOR_CONFLICT를 리턴한다.
  *       + GetServerFolderSyncInfo으로 루트 폴더의 정보를 받아와서 StoreRootFolderServerPath()으로 업데이트한다.
  *     - mSyncMethod가 수동 동기화인 경우에
  *       + 충돌 및 오류 발생한 개수가 0보다 크면 NEED_UI_FOR_CONFLICT를 리턴한다.
  *     - mPhase를 (PHASE_ENDING)로 하고, NEED_CONTINUE를 리턴한다.
  *     .
  * @n K. mPhase가 PHASE_CONFLICT_FILES와 같거나 적으면(동기화 충돌을 처리한다),
  *     - GetNextChild(PHASE_CONFLICT_FILES)로 항목 중에서 그 phase가 PHASE_CONFLICT_FILES보다 작은 폴더나 파일 항목을 찾아온다.
  *     - 해당하는 항목이 있으면,
  *       + ReadyResolve()를 호출하여 상태를 다시한번 업데이트하고 비교하고, 
  * 그 리턴값이 R_FAIL_NEED_FULL_SYNC_PARTIAL이면, NEED_UI_FOR_FULLSYNC를 리턴한다.
  *       + syncFileItem()을 호출하고 그 리턴값을 리턴한다.
  *     - UpdateFolderSyncResult()로 결과를 업데이트하고,
  *     - 충돌 결과(mConflictResult)나 하위 항목 결과(mChildResult)에 충돌 및 오류가 있거나, GetConflictCount()가 0보다 크면, 
  * NEED_UI_FOR_CONFLICT를 리턴한다.
  *     - mPhase를 (PHASE_ENDING)로 하고, NEED_CONTINUE를 리턴한다.
  *     .
  * @n L. mPhase가 PHASE_ENDING와 같거나 적으면,
  *     - SetUnchangedItemPhase()으로 변동이 없는 항목들은 PHASE_CONFLICT_FILES로 한다.
  *     - UpdateFolderSyncResult()로 결과를 업데이트하고, NOTHING_TODO를 리턴한다.
  */ 
  int ContinueSync();

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)

  /**
  * @brief 서버측의 하위의 변동된 목록을 받아오는 작업이 완료되면 그 결과가 이 함수로 전달된다.
  * 이 함수에서는 데이터를 내부 버퍼에 복사하여 두고 리턴한다.
  * @param p : 서버측의 하위의 변동된 목록을 요청한 폴더.
  * @param result : 결과값.
  * @param sfs_err : HTTP 오류값
  * @param data : 서버측에서 받은 데이터 버퍼
  * @param length : 서버측에서 받은 데이터 길이.
  * @param flag : 플래그
  * @param errorMessage : 서버측에서 보낸 오류 메시지
  * @return int : 성공하면 R_SUCCESS를 리턴한다.
  * @details A. data가 NULL이거나 result가 R_SUCCESS가 아니면,
  *     - OnCompleteServerItemScan(result)을 호출하고,
  *     - SetServerFolderScanResult()으로 결과를 저장하고, 오류 메시지를 SetServerConflictMessage()으로 저장하고, 0을 리턴한다.
  *     .
  * @n B. m_ScanFolderData에 필요한 메모리를 할당하고 data를 복사하고, 처리에 관련된 변수들을 초기화한다.
  */
  int OnEndScanFolderItem(KSyncFolderItem *p, int result, KDownloaderError sfs_err, LPSTR data, long length, int flag, LPTSTR errorMessage);

  /**
  * @brief 서버측의 하위의 변동된 목록을 받아오는 작업이 완료된 후 그 데이터를 처리하기 위해 이 함수가 호출된다.
  * 다운로드받은 데이터를 파싱하여 서버측 정보를 갱신하는 작업을 한다.
  * @return BOOL : 작업할 내용이 더 남아 있으면 TRUE를 리턴한다.
  * @details A. m_ScanFolderData가 유효하면,
  *     - data를 처리할 데이터 포인트에 위치하도록 하고,
  *     - m_ScanFolderIndex가 0이면(다운받은 데이터의 처음 부분을 처리하는 것이면)
  *       + mSyncFileCount, mSyncFolderCount를 0으로 초기화하고,
  *       + data 버퍼에서 SCAN_ITEM_DELEMETER("\b")를 찾아서, 이를 스캔한 서버 시간값으로 읽어와서 SetPreviousScanTimeStamp()으로 저장해 둔다.
  *       + 이 길이만큼 m_ScanFolderIndex를 증가한다.
  *       + (m_ScanFolderFlag & SCAN_ONLY_CHANGED)이면(조회 요청할 때 지정 시간 이후 변동된 개체만 요청했으면)
  *         + 요청한 폴더에 KSyncFolderItem::SetScanServerFolderState()를 하여 전체 하위 항목에 대해 스캔 상태값이 변동없음으로 한다.
  *       + 아니면(폴더 하위의 전체를 요청했으면)
  *         + 요청한 폴더는 존재하는 상태로, 하위의 항목들은 삭제됨 상태로 한다.(전체 목록을 조회한 경우에, 목록에 포함되지 않은 항목은 삭제된 것이므로
  * 하위의 항목이 모두 삭제됨을 기본 상태로 한다)
  *     - (m_ScanFolderIndex < m_ScanFolderDataLength)일때까지 반복한다.
  *       + data에서 SCAN_ITEM_DELEMETER("\b")를 찾아서 블록 길이를 구한다.
  *       + CreateFromStream()을 호출하여 폴더 및 파일 개체의 정보를 읽어 처리하도록 한다.
  *       + 블록만큼 data 포인터를 증가하고, m_ScanFolderIndex도 증가한다.
  *       + 버퍼의 끝에 도달하였으면
  *         + KSyncFolderItem::SetServerStateByScanState()을 호출하여 스캔 상태값(변동없음, 삭제됨, 존재함)을 개체의 서버 상태(mServerState)에 반영한다.
  *         + 변경된 내용만 받는 경우이고, 새로 추가된 항목은 처리할 필요 없음이고, 루트 폴더 조회한 경우,
  *           + SetPhaseForMovedFoldersInServer()으로 서버측에서 이동된 폴더들의 phase를 (PHASE_SCAN_FOLDERS_SERVER-1)로 설정한다.
  * 이는 해당 폴더에 대해 하위 정보 전체를 다시 요청해서 받아와야 하기 때문이다.
  *         + 아니면, 하위 항목들의 phase를 PHASE_SCAN_FOLDERS_SERVER로 설정한다.
  *         + 버퍼 메모리를 해제하고, 변수들도 초기화한다.
  *       + 처리한 개체의 개수가 NUMBER_OF_ITEMS_SCAN_PARSE와 같거나 크면 루우프를 빠져나온다.
  *     - 처리할 버퍼가 남아있으면 TRUE를 리턴한다.
  *     .
  * @n B. FALSE를 리턴한다.
  */
  BOOL processScanFolderData();

  /**
  * @brief 서버측에서 받은 스트리밍 데이터의 블록을 파싱하여 해당 항목에 저장한다. 항목이 없으면 새로 생성한다.
  * @param pRoot : 목록을 요청한 폴더.
  * @param lastParent : 이전에 처리한 항목의 부모 폴더(해당 항목을 빨리 찾기위해 캐시된 값).
  * @param d : 서버측에서 받은 데이터 버퍼
  * @param data_len : 블록의 데이터 길이.
  * @param rtn : 리턴값
  * @return KSyncItem* : 처리한 항목의 포인터
  * @details A. SCAN_VALUE_DELEMETER('|')를 찾아 블록 길이를 구하고,
  * @n B. 길이가 11이면(폴더나 파일의 서버 OID 길이)
  *     - WCHAR로 변환된 문자열을 만들어 oid로 한다.
  *     - pRoot->findItemByOID()으로 항목을 찾아온다.
  *     - 항목이 있으면
  *       + d에서 다음 블록 읽어서 int로 변환하여 childItemModified에 저장한다.
  *       + 항목의 SyncFlag에 SYNC_FLAG_IT_SCANNED을 더한다.
  *       + childItemModified가 0이 아니면
  *         + SetScanServerFolderState()으로 하위 항목의 스캔 상태값을 삭제됨으로 한다.
  *       + 이 항목의 스캔 상태값은 존재함으로 한다.
  *     - 아니면, 오류 로그를 기록한다.
  *     - 항목을 리턴한다.
  *     .
  * @n C. 길이가 11이 아니면 object_type으로 읽는다(D or F)
  * @n D. 다음 블록을 읽어 oid로 읽어온다.
  * @n E. 다음 블록이 없으면 NULL을 리턴한다.
  * @n F. 다음 블록을 읽어 name으로 읽어온다.
  * @n G. 다음 블록을 읽어 int로 바꾸어 permission으로 저장한다.
  * @n H. 다음 블록을 읽어 modifiedAt 수정시간으로 저장한다.
  * @n I. 다음 블록을 읽어 parent_oid로 저장한다.
  * @n J. parent_oid에 해당하는 부모 폴더 p를 찾아온다.
  * @n K. p가 NULL이면
  *     - oid가 pRoot의 ServerOID와 동일하면
  *       + CheckPermissionRootFolder()으로 permission이 유효한지 확인하고 권한이 부족하면,
  *         + FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER 오류를 리턴한다.
  *     - 아니면 오류 로그를 저장하고 NULL을 리턴한다.
  *     .
  * @n L. object_type이 'F'이면(이 항목은 폴더임)
  *     - 다음 블록을 읽어 creatorOID로 저장한다.
  *     - 다음 블록을 읽어 fullPathIndex로 저장한다.
  *     - 다음 블록을 읽어 childItemModified로 저장한다.
  *     - p가 NULL이 아니면
  *       + p에서 oid에 해당하는 하위 항목을 찾아 item으로 저장한다.
  *       + item이 NULL이면, p에서 name으로 하위 항목을 찾아 item으로 저장한다.
  *       + item이 NULL이 아니면,
  *         + SyncFlag에 SYNC_FLAG_IT_SCANNED를 더하고,
  *         + 서버 OID값을 oid로 설정하고,
  *         + CheckServerRenamedAs()으로 이름 변경 및 이동 정보를 설정하고,
  *         + 변동된 항목만 요청한 것이고, childItemModified가 0이 아니면
  *           + SetScanServerFolderState()으로 하위 항목(1단계만)들을 삭제 상태로 설정한다.
  *       + item이 NULL이면,
  *         + 경로를 만들고,
  *         + 이 경로가 동기화 처리 영역이면(부분 동기화시 제외되는 영역이 아닌지 확인한다)
  *           + 이 폴더에 대해 KSyncFolderItem 개체를 새로 생성하여 p에 하위 항목으로 등록하고 이를 item으로 한다.
  *       + item이 NULL이 아니면,
  *         + UpdateServerNewFileInfo()으로 서버측 정보를 업데이트하고, mServerNew.Filename에 name을 복사하고,
  *         + 스캔 상태값을 존재함으로 한다.
  *     - 사용한 메모리를 해제한다.
  *     .
  * @n M. object_type이 'D'이면(이 항목은 파일임)
  *     - 다음 블록을 읽어 lastModifier로 저장한다.
  *     - 다음 블록을 읽어 file_oid로 저장한다.
  *     - 다음 블록을 읽어 storage_oid로 저장한다.
  *     - 다음 블록을 읽어 fileSize로 저장한다.
  *     - p가 NULL이 아니면
  *       + p에서 oid에 해당하는 하위 항목을 찾아 item으로 저장한다.
  *       + item이 NULL이면 findChildByName으로 name에 일치하는 항목을 찾아온다.
  *       + item이 NULL이 아니면,
  *         + 서버OID 정보를 업데이트하고,
  *         + item의 phase가 PHASE_END_OF_JOB보다 작으면 CheckServerRenamedAs()으로 이동 정보를 준비한다.
  *       + 아니면,
  *         + 경로명을 만들고,
  *         + 이 경로가 동기화 처리 영역이면(부분 동기화시 제외되는 영역이 아닌지 확인한다)
  *           + KSyncItem을 생성하고 정보를 설정하고 이를 item으로 하고, p에 child로 등록한다.
  *       + item이 NULL이 아니면, UpdateServerNewFileInfo()으로 서버측 정보를 업데이트하고,
  *       + 스캔 상태값을 존재함으로 한다.
  *     - 사용한 메모리를 해제한다.
  *     .
  * @n N. item을 리턴한다.
  */
  KSyncItem* CreateFromStream(KSyncFolderItem *pRoot, KSyncFolderItem *lastParent, LPSTR d, long data_len, int& rtn);

  /**
  * @brief 서버측에서 받은 스트리밍 데이터의 다운로드가 종료되었을때 호출된다.
  * @param result : 요청에 대한 결과값.
  * @return int : NEED_CONTINUE를 리턴한다.
  * @details A. 결과값이 성공이면 phase를 다음 단계인 (PHASE_SCAN_FOLDERS_LOCAL-1)로 한다.
  * @n B. 아니면 OnCancelSync()를 호출한다.
  */
  int OnCompleteServerItemScan(int result);
#endif

#ifdef USE_BACKUP
  void OnRegistBackupEvent(int result);
#endif
  /**
  * @brief 메타 데이터의 업그레이드 작업을 진행한다. 1.x의 메타파일 구조를 2.x용 DB 구조로 변경한다.
  * 로컬싱크가 시작되고 동기화 폴더중에 1.x구조로 된 것이 있으면 이 작업이 자동으로 시작된다.
  * @details A. mPhase가 PHASE_SCAN_OBJECTS보다 작거나 같으면,
  *     - 스토리지 클래스를 준비하여 KSyncRootStorage::LoadChildMetaData()을 호출하여 메타 데이터를 로드한다.
  *     .
  * @n B. 아니면
  *     - 스토리지 클래스에서 동기화 루트 폴더의 정보를 로드하고,
  *     - KSyncRootStorageDB::CreateMetaDataFolder()으로 메타 데이터 폴더를 생성하고 성공하면,
  *       + KSyncRootStorageDB 개체를 생성하고, 하위 개체에 ContinueStoreMetafile(), StoreMetafile()를 호출하여
  * 메타 데이터를 DB로 저장한다.
  *       + 기존의 메타 폴더(".filesync")를 삭제한다.
  */
  int ContinueImmigration();

  // 0 : out-of folder, 1 : need-folder or child, 2 : parent of need-folder

  /**
  * @brief 대상 동기화 폴더가 동기화 처리를 하여야 하는 폴더인지 체크한다.
  * 부분 동기화를 진행할 때, 해당 폴더 이외의 폴더들은 처리가 되지 않도록 하는데 이때 이 함수로 체크한다.
  * @param folderItem : 대상 폴더
  * @return int : 대상 폴더가 동기화 처리가 되어야 하면 1을 리턴한다.
  * @details A. 부분 동기화 폴더(mSyncOnlyPath)가 유효하면,
  *     - mSyncOnlyPath의 내용에서 ';'로 구분된 경로명을 하나씩 구하여,
  *     - isSyncNeedFolderEx()로 대상 폴더가 부분 동기화의 하위 폴더인지 확인하여, 1이면 1을 리턴한다.
  *     - 아니면 0을 리턴한다.
  *     .
  * @n B. 부분 동기화 폴더 정보가 없으면 전체 폴더가 다 처리되는 것이니 1을 리턴한다.
  */
  int IsSyncNeedFolder(KSyncFolderItem *folderItem, int phase);


  BOOL UpdateParentOfSyncOnlyPath();

  /**
  * @brief 변경 목록을 검사해서 폴더의 하위 항목을 스캔할 필요가 있는지 확인하고, 없으면 하위 파일을 조사한다.
  */
  BOOL CheckNeedScanLocalFolder(KSyncFolderItem* folderItem);

private:
  /**
  * @brief 대상 동기화 폴더가 동기화 처리를 하여야 하는 폴더인지 체크한다.
  * @param syncOnlyPath : 부분 동기화 폴더
  * @param folderItem : 대상 폴더
  * @details A. 부분 동기화 폴더(syncOnlyPath)가 유효하면,
  *     - 대상 폴더의 상대 경로명을 준비하여,
  *       + NULL이면 루트 폴더이니 2를 리턴한다.
  *     - 경로명이 부분 동기화 폴더의 상위 폴더이면 2을 리턴한다.
  *     - 경로명이 부분 동기화 폴더와 같거나 하위 폴더이면 1을 리턴한다.
  *     - 0을 리턴한다.
  *     .
  * @n B. 1을 리턴한다.
  */
  int isSyncNeedFolderEx(LPCTSTR syncOnlyPath, KSyncFolderItem *folderItem);

  /**
  * @brief ModifiedItemDat에 대한 항목을 준비하고 로컬 상태를 업데이트 한다.
  * @param pathName : 경로명
  * @param modifyType : DIR_MODIFY_EVENT
  * @details A. modifyType이 DIR_DELETED인 경우에
  * @n - 경로명에 해당하는 항목이 DB에 있는지 확인하고, 없으면 이름 변경을 확인하여 원본 경로명으로 다시 한번 항목이 DB에 있는지 확인한다.
  * @n - 항목이 존재하면 트리를 구성하고, 로컬에 존재하지 않음으로 설정한다.
  * @n B. modifyType이 DIR_ADDED,DIR_MODIFIED,DIR_MOVED_NEW이면
  * @n - 이름 변경이 되어 있으면 원본 경로명을 구하고,
  * @n - 경로명에 해당하는 항목이 DB에 있는지 확인하고, 없으면 새로 추가된 항목으로 인식하여 트리를 구성한다.
  * @n - 경로명에 대해 트리를 구성한다.
  * @n - 대상 항목이 로컬에 존재함으로 설정한다.
  */
  int readyModifiedItem(LPTSTR pathName, DIR_MODIFY_EVENT modifyType);

public:

  /**
  * @brief 실시간 백업에서 로컬 폴더를 스캔하지 않고 ModifiedTable에 등록된 항목을 트리로 생성한다.
  * @n 변경된 목록 경로에 대한 항목이 존재하지 않으면 생성한다.
  * @details A. mModifiedItemList 목록에서 DIR_MOVED인 항목은 스킵하고, readyModifiedItem()을 호출한다.
  */
  int ReadyModifiedItemList();

  /**
  * @brief 부분 동기화 폴더가 하나이고 유효하면 그것만 (phase-1)로 하고 다른 폴더는 phase로 설정한다.
  */
  BOOL CheckScanFolderServerPartialSync(int phase);

  /**
  * @brief 대상 폴더가 동기화 처리를 하여야 하는 폴더인지 체크한다.
  * @param pathname : 대상 폴더
  * @param isFolder : 대상 pathname이 폴더임.
  * @return BOOL : 대상 폴더가 동기화 처리가 되어야 하면 TRUE를 리턴한다.
  * @details A. 부분 동기화 폴더(mSyncOnlyPath)가 유효하면,
  *     - mSyncOnlyPath의 내용에서 ';'로 구분된 경로명을 하나씩 구하여,
  *     - IsPartialSyncOnEx()로 대상 폴더가 부분 동기화의 하위 폴더인지 확인하여, 1이면 TRUE를 리턴한다.
  *     - 아니면 FALSE를 리턴한다.
  *     .
  * @n B. 부분 동기화 폴더 정보가 없으면 전체 폴더가 다 처리되는 것이니 TRUE를 리턴한다.
  */ 
  virtual BOOL IsPartialSyncOn(LPCTSTR pathname, BOOL isFolder);

  /**
  * @brief 대상 동기화 폴더가 동기화 처리를 하여야 하는 폴더인지 체크한다.
  * @param syncOnlyPath : 부분 동기화 폴더
  * @param pathname : 대상 폴더
  * @param isFolder : 대상 pathname이 폴더임.
  * @return BOOL : 대상 폴더가 동기화 처리가 되어야 하면 TRUE를 리턴한다.
  * @details A. 부분 동기화 폴더(syncOnlyPath)가 유효하면,
  *     - 대상 폴더의 상대 경로명을 준비하여,
  *     - 경로명이 부분 동기화 폴더의 상위 폴더이면 isFolder를 리턴한다.
  *     - 경로명이 부분 동기화 폴더와 같거나 하위 폴더이면 TRUE를 리턴한다.
  *     - FALSE를 리턴한다.
  *     .
  * @n B. TRUE를 리턴한다.
  */
  BOOL IsPartialSyncOnEx(LPCTSTR syncOnlyPath, LPCTSTR pathname, BOOL isFolder);

  /**
  * mSyncOnlyPath가 중간에 이름 변경되었는지 업데이트한다.
  */
  BOOL refreshPartialSyncRenamed(KSyncRootStorage* storage);

  /**
  * @brief 이 동기화 루트 폴더가 동기화 처리를 할 수 있는지 체크한다.
  * @return int : 할 수 있으면 R_SUCCESS를 아니면 오류 코드를 리턴한다.
  * @details A. 서버 정보를 리프레시하여 성공하면,
  *     - 서버측 권한값을 가져와서, CheckPermissionRootFolder()으로 권한이 있는지 확인하고,
  *     - 권한이 없으면 FILE_SYNC_NO_PERMISSION_ROOTSERVER_FOLDER를 리턴한다.
  *     - R_SUCCESS를 리턴한다.
  *     .
  * @n B. 실패하면 FILE_SYNC_ROOTSERVER_FOLDER_NOT_EXIST를 리턴한다.
  */
  int CheckRootSyncable();

  /**
  * @brief 충돌 처리창에서 동기화 버튼을 눌러 다시 동기화를 시작할 때, 이 함수가 호출된다.
  * 동기화 충돌 처리 작업을 준비한다.
  * @param flag : 플래그
  * @details A. 플래그에 DONE_AS_COMPLETE값이 없으면,
  *     - UpdateFolderSyncResult()로 동기화 결과를 업데이트한다.
  *     - NOTHING_TODO를 리턴한다.
  *     .
  * @n B. 아니면,
  *     - 동기화 진행 관련 변수들을 초기화하고,
  *     - 하위 항목들의 phase를 (PHASE_CONFLICT_FILES - 1)로 설정하고,
  *     - 루트 항목의 phase를 PHASE_CONFLICT_FILES로 하고,
  *     - 지금의 시간을 mSyncStartTime로 받아놓고,
  *     - 동기화 결과값을 0으로 초기화하고,
  *     - CheckRootSyncable()으로 동기화 루트 폴더의 권한을 체크하고 오류가 있으면 NEED_UI_FOR_SYNC_ERROR를 리턴한다.
  *     - NEED_CONTINUE를 리턴한다.
  */
  int ContinueConflict(int flag);

  /**
  * @brief 메타 스토리지에서 각 항목의 동기화 충돌 결과값을 로드한다.
  * 동기화 충돌 처리 작업을 준비한다.
  * @return int : 오류 코드 또는 NEED_CONTINUE를 리턴한다.
  * @details A. SetItemSynchronizing()으로 루트 폴더가 동기화 진행중인 상태로 설정한다.
  * @n B. LoadServerPrevious()으로 메타 데이터를 로드한다.
  * @n C. GetRootLastSyncTime()으로 마지막 동기화 시간을 mLastSyncCompleteTime에 읽어온다.
  * @n D. 동기화 관련 변수들을 초기화한다.
  * @n E. CheckRootSyncable()을 호출하여 서버측 권한과 상태를 체크한다.
  */ 
  int LoadLastConflict();

  /**
  * @brief 동기화 진행시 소요 시간을 로그에 기록한다.
  * @param str : 태그 메시지
  * @details A. 현재 시간과 mSyncBeginTime의 차를 구하고, 이를 GetEllapsedTimeString()으로 문자열로 바꾼다.
  * @n B. 뒤에 지정한 스트링을 " : "와 같이 붙여서 로그에 저장한다.
  */ 
  void StoreEllapsedTimeLog(LPCTSTR str);

  /**
  * @brief 동기화 작업을 종료한다.
  * @param done : TRUE이면 동기화 완료 상태로 종료한다.
  * @details A. (mSyncMethod & SYNC_METHOD_SETTING)이면,
  *     - StoreRootFolderServerPath()으로 서버측 정보를 저장한다.
  *     - StoreMetafile()으로 메타 데이터를 저장한다.
  *     - ShellNotifyIconChangeOverIcon()으로 탐색기 아이콘을 업데이트한다.
  *     .
  * @n B. 아니면,
  *     - SetItemSynchronizing()으로 동기화 진행중인 상태를 해제한다.
  *     - done이면 mChildResult를 0으로 클리어한다.
  *     - StoreMetafile()으로 메타 데이터를 저장한다.
  *     - done이면 ClearFileRenamed()으로 이름 변경 정보를 모두 클리어한다.
  *     - 지금의 시간값을 마지막 동기화 시간으로 설정한다.
  *     - 동기화 종료 이벤트를 히스토리에 저장한다.
  */ 
  void CloseSync(int flag);

  /**
  * @brief 동기화 작업을 취소한다.
  * @param sync_error : 동기화 오류 코드값.
  * @details A. 충돌 결과값을 sync_error로 한다.
  * @n B. StoreMetafile()으로 메타 데이터를 저장한다.
  * @n C. ShellNotifyIconChangeOverIcon()으로 탐색기의 아이콘을 업데이트한다.
  * @n D. UpdateRootLastSyncTime()으로 마지막 동기화 시간을 업데이트한다.
  * @n E. 루트 폴더의 정보를 다시 로드한다.
  */
  void OnCancelSync(int sync_error);

  /**
  * @brief 동기화 작업 종료 내용을 기록한다.
  */
  int StoreBackupRestoreResult(int flag);

  /**
  * @brief 다음에 처리할 자동 동기화 시간을 계산하여 시작될 시간을 리턴한다.
  * @param time : 계산된 결과 시간.
  * @return BOOL : 다음에 처리할 자동 동기화 작업이 유효하면 TRUE를 리턴한다.
  * @details A. 동기화 정책에 동기화가 일시 정지된 상태이면 FALSE를 리턴한다.
  * @n B. 마지막 동기화 시간과 정책 설정 시간을 last_sync_time, setting_time에 읽어온다.
  * @n C. last_sync_time이 무효하면, setting_time을 last_sync_time에 복사한다.
  * @n D. 아니고, setting_time이 last_sync_time보다 크면, setting_time을 last_sync_time에 복사한다.
  * @n E. 동기화 정책의 CalcNextSyncTime()을 호출하여 다음 동기화 시간을 받아오고 성공하면 TRUE를 리턴한다.
  * @n F. FALSE를 리턴한다.
  */
  BOOL CalcNextSyncTime(SYSTEMTIME &time);

  void UpdateLastSyncTime();

  // RenameManagement storage
  // void GetRenameManageFilename(LPTSTR manage_file);

  /**
  * @brief DirectoryMonitor가 수집하여 기록한 로컬측 폴더 및 파일의 이동 정보를 각 항목 개체에 설정한다.
  * @details A. KSyncRootStorage::DeployFolderMove()를 호출한다.
  */
  int DeployFolderMovement();

  /**
  * @brief DeployFolderMovement()에서 스토리지에 기록된 폴더 및 파일의 이동 정보가 이 함수를 통해 callback으로 전달된다.
  * 해당 개체를 찾아 이동 정보로 설정한다.
  * @param oldName : 원래의 경로명
  * @param newName : 변경된 경로명
  * @details A. 원래의 경로명과 변경된 경로명에 해당하는 개체를 FindChildItemCascade()으로 찾아온다.
  * @n B. 두 개체가 NULL이 아니고 동일하지 않으면,
  * @n C. 두 개체가 존재하는 항목이면 머지하지 않고, FALSE를 리턴한다.
  * @n D. 이전 개체가 존재하고, 새 개체가 ReadyMetafilename()으로 새로 생성되면,
  *     - newItem을 oldItem에 머지한다.
  *     - newItem을 삭제한다.
  *     - TRUE를 리턴한다.
  *     .
  * @n E. FALSE를 리턴한다.
  */
  BOOL OnFolderMovedInfo(LPTSTR oldName, LPTSTR newName);

  /**
  * @brief 지정한 새 경로명에 대해 Rename Storage에서 원본 경로명을 찾아온다.
  * @param newRelativeName : 새로운 경로명
  * @param originalRelativeName : 원본 경로명
  * @return int : 이름변경에 해당하면 1, 상위 폴더가 이름변경이면 2를 리턴한다.
  * @details A. KSyncRootStorage::FindOriginalNameT()을 호출하여 원본 경로명을 찾아온다.
  */
  int GetMovedRenamedOriginalName(LPCTSTR newRelativeName, LPTSTR originalRelativeName);


  int AllocMovedRenamedOriginalName(LPCTSTR newRelativeName, LPTSTR* originalRelativeName);

  /**
  * @brief 지정한 새 경로명에 대해 Rename Storage에서 원본 경로명을 찾고, 해당 항목을 찾아 리턴한다.
  * @param newRelativeName : 새로운 경로명
  * @return KSyncItem* : 이름변경에 해당하면 항목의 포인터.
  * @details A. GetMovedRenamedOriginalName()으로 원본 경로명을 찾아온다.
  * @n B. 성공하면 원본 경로명에 해당하는 항목을 FindChildItemCascade()으로 찾아 리턴한다.
  */
  KSyncItem *GetMovedRenamedOriginal(LPCTSTR newRelativeName);

  /**
  * @brief 지정한 새 경로명에 대해 Rename Storage에서 해당 항목을 삭제한다. 처리가 완료되었을때 호출된다.
  * @param newRelativeName : 새로운 경로명
  * @return BOOL : 성공하면 TRUE를 리턴한다.
  * @details A. KSyncRootStorage::RemoveRenameItem()을 호출하여 이름변경 항목을 삭제한다.
  */
  BOOL RemoveMovedRenamedOriginal(LPCTSTR newRelativeName);

  /**
  * @brief 지정한 원본 경로명에 대해 Rename Storage에서 해당 항목을 찾아 새로운 경로명을 리턴한다.
  * @param oldRelativeName : 원본 경로명
  * @return LPTSTR : 새로운 경로명, 받아서 사용하고 메모리를 해제하여야 한다.
  * @details A. KSyncRootStorage::AllocMovedRenamedAs()을 호출하여 새로운 경로명을 리턴한다.
  */
  virtual LPTSTR AllocMovedRenamedAs(LPCTSTR oldRelativeName);

  //BOOL GetMovedRenamedAs(LPCTSTR oldRelativeName, LPTSTR newRelativeName);

  /**
  * @brief 지정한 원본 경로명에 대해 Rename Storage에서 해당 경로로 이동되어 들어오가나, 해당 경로에서 이동되어 나간 항목이 있는지 확인한다.
  * @param relativePath : 대상 경로명
  * @return BOOL : 해당하는 항목이 있으면 TRUE를 리턴한다.
  * @details A. KSyncRootStorage::GetRenamedInOutCount()을 호출하여 개수가 0보다 크면 TRUE를 리턴한다.
  */
  BOOL CheckRenamedInOut(LPCTSTR relativePath);

  /**
  * @brief 부분 동기화 완료후 Rename Storage에서 해당 폴더로 이동된 항목을 모두 삭제한다.
  * @param subFolderSync : 부분 동기화 경로명
  * @details A. KSyncRootStorage::ClearFileRenamed()을 호출한다.
  */
  void ClearFileRenamed(LPCTSTR subFolderSync);

  BOOL ClearRenameFlag(LPCTSTR orgName, LPCTSTR newName);

  /**
  * @brief 폴더의 이름이 바뀌었으니 이 하위의 항목들의 이름도 업데이트 한다.
  * @param prevRelative : 이전 경로명
  * @param changedRelative : 새 경로명
  * @details A. KSyncRootStorage::UpdateRenameParentFolder()을 호출한다.
  */
  void UpdateRenameParentFolder(LPCTSTR prevRelative, LPCTSTR changedRelative);

  /**
  * @brief 폴더의 이름이 바뀌었으니 메타 폴더의 경로명 Mapping도 업데이트 한다. 1.x에서만 사용된다.
  * @param lastRelativePath : 이전 경로명
  * @param newRelativePath : 새 경로명
  * @param includeSub : TRUE이면 하위 항목도 업데이트한다.
  * @details A. KSyncRootStorage::UpdateMetaFolderNameMapping()을 호출한다.
  */
  BOOL UpdateMetaFolderNameMapping(LPCTSTR lastRelativePath, LPCTSTR newRelativePath, BOOL includeSub = FALSE);

  /**
  * @brief 이 폴더의 서버측 fullPathIndex가 바뀌었는지 확인하는 가상함수이다.
  */
  virtual inline BOOL IsFolderFullPathChanged() { return FALSE; }

  /**
  * @brief 동기화 파일 전체 크기를 리턴한다.
  */
  inline __int64 GetSyncTotalSize() { return mSyncTotalSize; }
  /**
  * @brief 동기화 완료된 파일 크기를 리턴한다.
  */
  inline __int64 GetSyncDoneSize() { return mSyncDoneSize; }

  /**
  * @brief 대상 항목에 대한 동기화 작업을 실행한다.
  * @param item : 대상 항목
  * @param phase : 동기화 작업 단계.
  * @return int : 계속하려면 NEED_CONTINUE를 리턴한다.
  * @details A. item이 루트 폴더항목이거나 제외된 항목이면 phase를 업데이트하고 NEED_CONTINUE를 리턴한다.
  * @n B. mSyncItemFileSize를 GetSyncFileSize()으로 얻어온다.
  * @n C. phase가 PHASE_CONFLICT_FILES이 아니거나 항목이 Enable상태이면
  *     - 항목의 doSyncItem()을 호출하여 작업을 진행하고, 그 리턴값이 FILE_SYNC_THREAD_WAIT이면 r을 리턴한다.
  *     - 리턴값에 오류나 충돌이 발생하였으면 phase를 PHASE_ENDING로 하여 다른 작업을 하지 않도록 한다.
  *     .
  * @n D. 아니면, 항목이 disable이면 phase를 PHASE_END_OF_JOB로 한다.
  * @n E. phase가 PHASE_COPY_FILES보다 크거나 같으면, OnSyncFileItemDone()을 호출한다.
  */
  int syncFileItem(KSyncItem *item, int phase);

  /**
  * @brief 대상 항목에 대한 동기화 작업이 완료되었을때 호출된다.
  * @param item : 대상 항목
  * @param r : 결과값
  * @param sync_size : 동기화 파일 크기
  * @details A. item이 이 항목이 아니면, mSyncDoneSize, mSyncDoneSizeProgress에 sync_size를 더한다.
  * @n B. item이 파일이면 mSyncFileCount도 증가한다.
  * @n C. mSyncProcessedSize를 0으로 한다.
  * @n D. 결과값이 동기화 충돌이나 오류가 없고, 항목이 파일이면, RemoveItem으로 목록에서 제거하고 항목을 삭제한다.
  */
  void OnSyncFileItemDone(KSyncItem *item, int r, __int64 sync_size, int phase);

  /**
  * @brief 지정한 폴더가 동기화 루트 폴더인지 확인한다.
  * @param pathName : 경로명
  * @param onlyCurrentUser : TRUE이면 현재의 사용자의 동기화 폴더만 검사한다.
  * @return int : -1 for other user's, 0 for none, 1 for normal sync-root, 2 for initializing sync-root
  * @details A. KSyncRootStorage::IsMatchRootFolder()을 호출하고 그 리턴값을 리턴한다.
  */
  static int IsSyncRootFolder(LPCTSTR pathName, BOOL onlyCurrentUser = FALSE);

  // static int ReadySyncMetaData(LPCTSTR path, LPCTSTR base_path, LPCTSTR object_path, KSyncPolicy *p);

  /**
  * @brief 지정한 파일명이 동기화 제외 형식에 해당하는지 검사한다.
  * @param filename : 파일명
  * @return BOOL : 제외 형식에 해당하면 TRUE를 리턴한다.
  * @details A. KSyncCoreEngine::IsExcludeFileMask()을 호출하여 서버측 제외 형식에 해당하는지 검사한다.
  * @n B. KSyncPolicy::IsExcludeFileMask()을 이 동기화 폴더의 정책에 설정된 제외 형식에 해당하는지 검사한다.
  */
  BOOL IsExcludeFileMask(LPCTSTR filename);

  /**
  * @brief 대상 항목이 동기화 제외 형식에 해당하는지 검사한다.
  * @param item : 대상 항목
  * @return BOOL : 제외 형식에 해당하면 TRUE를 리턴한다.
  * @details A. KSyncCoreEngine::IsExcludeFileMask()을 호출하여 서버측 제외 형식에 해당하는지 검사한다.
  * @n B. KSyncPolicy::IsExcludeFileMask()을 호출하여 이 동기화 폴더의 정책에 설정된 제외 형식에 해당하는지 검사한다.
  * @n C. 상대 경로명을 준비하여, KSyncPolicy::IsExcludeFileMask()을 호출하여 검사한다.
  */
  BOOL IsExcludeFileMask(KSyncItem* item);

  /**
  * @brief 하위 항목들을 모두 검사하여 동기화 제외 형식에 해당하는 항목들을 disable시킨다.
  * @return BOOL : 제외 형식에 해당하는 항목이 있으면 TRUE를 리턴한다.
  * @details A. KSyncFolderItem::DisableExcludedFileMask()을 호출하여 동기화 제외 형식에 해당하는 하위 항목들을 disable시킨다.
  */
  BOOL CheckExcludeNameMask();

  /**
  * @brief 동기화 정책 정보를 스토리지에서 읽어오거나 저장한다.
  * @param bStore : TRUE이면 스토리지에서 읽어온다.
  * @details A. 이 동기화 폴더가 KSyncRootStorageFile::IsMatchRootFolder()이면,
  *     - KSyncRootStorageFile 클래스를 준비하여 StoreSyncPolicy(), 또는 LoadSyncPolicy()를 호출한다.
  *     .
  * @n B. 아니면
  *     - KSyncRootStorageDB 클래스를 준비하여 StoreSyncPolicy(), 또는 LoadSyncPolicy()를 호출한다.
  */
  BOOL SerializePolicy(BOOL bStore);

  /**
  * @brief 지정한 시간이 마지막 동기화 완료 시간 이후인지 확인한다.
  * @param lastModifiedTime : 지정한 시간
  * @return BOOL : 지정한 시간이 마지막 동기화 완료 시간 이후이면 TRUE를 리턴한다.
  * @details A. 마지막 동기화 완료 시간(mLastSyncCompleteTime)이 유효하고, 지정한 시간이 이보다 크면 TRUE를 리턴한다.
  */
  BOOL IsChangedAfterLastSync(SYSTEMTIME &lastModifiedTime);

  /**
  * @brief 동기화 완료 시간을 업데이트한다.
  * @param success : 동기화 완료인지 충돌이나 오류 항목이 있는지 여부.
  * @details A. success가 TRUE이면 mSyncDoneTime을 AppendSettingKeyValue()으로 KEY_LAST_SYNC키로 저장한다.
  * @n B. success가 TRUE이고 부분 동기화가 아니면,
  *     - KSyncRootStorage::StoreSettingValue()로 KEYB_LAST_SYNC으로 저장한다.
  *     - KSyncRootStorage::StoreSettingValue()로 KEYB_LAST_SCAN_TIME으로 mPreviousScanTime값을 저장한다.
  *     - mPreviousScanTime값을 mLastScanTime에 복사한다.
  *     .
  * @n B. 자동 동기화이면,
  *     - KSyncRootStorage::StoreSettingValue()로 KEYB_LAST_AUTOSYNC으로 저장한다.
  *     .
  */
  void UpdateRootLastSyncTime(int flag);

  /**
  * @brief 이 동기화 폴더의 마지막 동기화 완료 시간을 읽어온다.
  * @param auto_time : 마지막 자동 동기화 시간
  * @param sync_time : 마지막 동기화 시간
  * @param setting_time : 마지막 동기화 정책 설정 시간
  * @return BOOL : 설정값을 읽었으면 TRUE를 리턴한다.
  * @details A. auto_time 버퍼가 NULL이 아니면 KSyncRootStorage::LoadSettingValue()으로 KEYB_LAST_AUTOSYNC 키값을 읽어온다.
  * @n B. sync_time 버퍼가 NULL이 아니면 KSyncRootStorage::LoadSettingValue()으로 KEYB_LAST_SYNC 키값을 읽어온다.
  * @n C. setting_time 버퍼가 NULL이 아니면 KSyncRootStorage::LoadSettingValue()으로 KEYB_LAST_SETTING 키값을 읽어온다.
  */
  BOOL GetRootLastSyncTime(SYSTEMTIME *auto_time, SYSTEMTIME *sync_time, SYSTEMTIME *setting_time);

  /**
  * @brief 동기화 업로드/다운로드 작업중인 항목으로 지정한 파일을 등록한다.
  * DirectoryMonitor에서 동기화 작업중인 파일은 무시하도록 하기 위해 등록한다.
  * @param fileName : 대상 파일의 전체 경로명
  * @return BOOL : 등록되었으면 TRUE를 리턴한다.
  * @details A. 전체 경로명이 현 동기화 폴더의 하위에 속하면,
  *     - clearRegisteredOverTime()으로 타임오버된 항목을 삭제하고
  *     - SYNCING_FILE_ITEM struct를 메모리 할당하여 파일 경로명과 현재의 TickCount를 저장하고,
  *     - mSyncingItems어레이에 추가하고 TRUE를 리턴한다.
  *     .
  * @n B. FALSE를 리턴한다.
  */
  BOOL AddSyncingFileItem(LPCTSTR fileName);

  /**
  * @brief DirectoryMonitor에서 대상 파일을 무시할 것인지 확인한다.
  * @param fileName : 대상 파일의 전체 경로명
  * @return int : 무시해도 되면 1 또는 2를 리턴한다.
  * @details A. 전체 경로명이 현 동기화 폴더의 하위에 속하면,
  *     - 파일의 상대 경로명을 준비하고,
  *     - 이 파일이 DB 파일이나 ".filesync" 메타 폴더이면 1을 리턴한다.
  *     - mSyncingItems 어레이에서 이 파일이 있는지 확인하고 있으면 2를 리턴한다.
  *     .
  * @n B. 0을 리턴한다.
  */
  int IsMonitoringSkipFileItem(LPCTSTR fileName);

  /**
  * @brief 동기화 진행중인 파일인지 확인한다.
  * @param fileName : 대상 파일의 전체 경로명
  * @return int : 동기화 진행중인 파일이면 2를 리턴한다.
  */ 
  SFI_TYPE IsSyncingFileItem(LPCTSTR fileName);

  /**
  * @brief 동기화 진행중인 파일인지 확인한다.
  * @param fileName : 대상 파일의 상대 경로명
  * @return int : 동기화 진행중인 파일이면 2를 리턴한다.
  */ 
  SFI_TYPE IsSyncingFileItemRelative(LPCTSTR fileName);

  /**
  * @brief mSyncingItems에 등록된 항목에서 타임아웃된 항목은 삭제한다.
  * 등록된지 2초 이상 지난 항목은 삭제한다.
  */
  void clearRegisteredOverTime();

  /**
  * @brief mSyncingItems에 등록된 항목에서 전체 항목을 삭제한다.
  */
  void clearAllSyncingItem();

  /**
  * @brief 자동 동기화 일시 멈춤/ 다시 시작을 설정한다.
  * @param paused : TRUE이면 일시 멈춤을 하고 FALSE이면 다시 사직으로 한다.
  * @details A. mPolicy.SetPaused()로 설정하고,
  * @n B. KSyncRootStorage::StoreSyncPaused()로 설정값을 저장한다.
  */
  void SavePausedState(BOOL paused);

  /**
  * @brief 동기화 루트 폴더의 정보를 스토리지에 저장한다. SETTING_TABLE에 저장된다.
  * @param server_folder_oid : 서버측 폴더 OID
  * @param fullPath : 서버측 폴더의 전체 경로명
  * @param fullPathIndex : 서버측 폴더의 fullPathIndex
  * @param server_location_changed : 서버측 폴더 위치가 바뀌었으면 TRUE
  * @return int : 성공하면 R_SUCCESS을 리턴한다.
  * @details A. server_location_changed이면 KEYB_SERVER_NEW_OID키에 server_folder_oid를 저장한다.
  * @n B. 아니면,
  *     - KEYB_SERVER_OID 키에 server_folder_oid를 저장한다.
  *     - KEYB_SERVER_NEW_OID 키에 NULL을 저장한다.
  *     .
  * @n C. KEYB_SERVER_FULLPATH 키에 fullPath를 저장한다.
  * @n D. KEYB_SERVER_FULLPATHINDEX 키에 fullPathIndex를 저장한다.
  * @n E. R_SUCCESS을 리턴한다.
  */
  int StoreRootFolderServerPath(LPTSTR server_folder_oid, LPTSTR fullPath, LPTSTR fullPathIndex, BOOL server_location_changed);

  /**
  * @brief 동기화 루트 폴더의 정보를 스토리지에서 읽어온다.
  * @details A. KEYB_SERVER_FULLPATH키에서 서버측 경로를 읽어온다.
  * @n B. KEYB_SERVER_FULLPATHINDEX키에서 서버측 fullPathIndex를 읽어온다.
  * @n C. KEYB_LAST_SCAN_TIME키에서 mLastScanTime를 읽어온다.
  */
  void LoadServerPrevious();

  /**
  * @brief 동기화 루트 폴더의 위치가 바뀌었음을 설정한다.
  * @param set : TRUE이면 바뀌었음을 설정하고, 아니면 해제한다.
  */
  inline void SetServerLocationChanged(BOOL set) { mPolicy.ChangeSyncFlag(SYNC_SERVER_LOCATION_CHANGED, set); }

  int MergeSyncOnlyPath(LPCTSTR relativePath);

  /**
  * @brief 부분 동기화 작업으로 동기화를 진행할 상대 경로명을 설정한다.
  * @param relativePath : 부분 동기화를 진행할 상대 경로명.
  * @return int : 성공하면 R_SUCCESS을 리턴한다.
  * @details A. relativePath가 유효하면
  *     - mSyncOnlyPath에 relativePath를 복사한다.
  *     - GetMovedRenamedOriginalName()으로 relativePath에 해당하는 이동된 원본 폴더가 있는지 확인하고,
  *       + 해당 폴더가 동기화 완료 상태가 아니면, R_FAIL_NEED_FULL_SYNC_VERGIN을 리턴한다.
  *     - 아니면 relativePath가 동기화 완료 상태가 아니면, R_FAIL_NEED_FULL_SYNC_VERGIN을 리턴한다.
  *     - CheckRenamedInOut()을 통해 이폴더에서 밖으로, 또는 밖에서 이 폴더로 이동된 항목이 있는지 검사하여,
  * 있으면 R_FAIL_NEED_FULL_SYNC_PARTIAL를 리턴한다.
  *     .
  * @n B. 아니면 mSyncOnlyPath를 클리어한다.
  */
  int SetSyncOnlyPath(LPCTSTR relativePath);

  void SetSyncExtraInfo(LPCTSTR info);

  /**
  * @brief 대상 경로명에 해당하는 항목을 생성하여 메타 데이터를 로드하고 항목을 리턴한다.
  * @param relativePathName : 대상 항목의 경로명
  * @param isFolder : 폴더인지 파일인지.
  * @return KSyncItem* : 생성된 항목
  * @details A. 폴더이면 KSyncFolderItem을, 아니면 KSyncItem 개체를 생성한다.
  * @n B. 메타 데이터를 로드한다. 메타데이터가 없으면,
  * @n C. GetMovedRenamedOriginalName()으로 이름 변경된 original 개체 이름이 존재하면,
  *     - 앞서 생성한 개체를 삭제하고 이 이름으로 개체를 다시 생성하고, 메타 데이터를 로드한다.
  *     .
  * @n D. 생성된 개체를 리턴한다.
  */
  KSyncItem* CreateReadyChildMetafile(LPCTSTR relativePathName, BOOL isFolder);

  /**
  * @brief 동기화 폴더 하위의 파일의 이름이 변경되었을때 호출된다.
  * @param oldFileName : 이전 파일 경로명
  * @param newFileName : 새 파일 경로명
  * @return int : 이전 파일명이 동기화 폴더 바깥이면 0을 리턴, 이전 파일명이 동기화 루트 폴더 하위 항목이면 1을 리턴,
  * 메타 폴더 또는 메타 DB 파일이면 2, 동기화 루트 폴더이면 3을 리턴한다.
  * @details A. oldFileName이 mBaseFolder의 하위 폴더이면,
  *     - mBaseFolder와 같으면 3을 리턴한다.
  *     - 메타 폴더 또는 메타 DB 파일이면 2를 리턴한다.
  *     - 1을 리턴한다.
  *     .
  * @n B. 0을 리턴한다.
  */ 
  int OnFileRenamed(LPCTSTR oldFileName, LPCTSTR newFileName);

  /**
  * @brief 동기화 폴더 하위의 파일의 이름이 변경 사항을 스토리지의 RENAME TABLE에 저장한다.
  * @param oldFileName : 이전 파일 경로명
  * @param newFileName : 새 파일 경로명
  * @param forceAppend : TRUE이면 무조건 추가한다.
  * @return BOOL : TRUE를 리턴한다.
  * @details A. 새 경로명이 파일인지 폴더인지 확인한다.
  * @n B. forceAppend가 FALSE이면,
  *     - CreateReadyChildMetafile()으로 대상 항목을 찾아와서,
  *     - 항목이 없거나 아직 동기화 안된 항목이면 append를 FALSE로 한다.
  *     .
  * @n C. 항목이 NULL이 아니면
  *     - 항목의 원본 경로명을 구하고, 이것이 이미 이름 변경되었는지 확인하여,
  *     - 항목의 원본 경로명과 oldFileName이 다르면,
  *       + KSyncRootStorage::SetRenameT()으로 항목의 원본 경로명과 newFileName을 저장한다.
  *       + 이 항목이 폴더이면, KSyncRootStorage::SetRenameT()으로 이름 변경된 이름과 newFileName으로,
  * 경로명 변경에 따른 업데이트를 업데이트(by KSyncRootStorage::KRS_CHANGE_PATH)를 한다.
  *       + or_name을 NULL로 설정한다.
  *     .
  * @n D. or_name이 NULL이 아니면,
  *     - KSyncRootStorage::SetRenameT()으로 항목의 이전 경로명과 새 경로명을 추가한다.
  *     - 항목이 폴더이면,
  *       + KSyncRootStorage::SetRenameT()으로 경로명 변경에 따른 업데이트를 업데이트(by KSyncRootStorage::KRS_CHANGE_PATH)를 한다.
  *     .
  */ 
  BOOL OnStoreRenamed(LPCTSTR oldFileName, LPCTSTR newFileName, BOOL forceAppend);

  /**
  * @brief 이름 변경된 항목이 삭제되었을때 호출된다.
  * @param newFileName : 삭제된 경로명
  * @return int : 해당 항목이 이 동기화 루트 폴더 하위 항목이면 1을 리턴한다.
  * @details A. IsChildFolderOrFile()으로 경로명이 이 폴더 하위이면 1을 리턴한다.
  */
  int OnFileRenameRemoved(LPCTSTR newFileName);

  /**
  * @brief 이름 변경된 항목이 삭제되어 이 기록을 이름 변경 테이블에서 삭제할 때 호출된다.
  * @param newFileName : 새 파일 경로명
  * @param oldFileName : 이전 파일 경로명
  * @return BOOL : 정상적으로 삭제되면 TRUE를 리턴한다.
  * @details A. KSyncRootStorage::RemoveRenameItem()으로 이름 변경 정보를 삭제한다.
  */
  BOOL OnRemoveRenamed(LPCTSTR newFileName, LPCTSTR oldFileName);

  /**
  * @brief 동기화 작업 과정에 의해 이름 변경된 경우, 이것을 mRenamedStack에 저장하였다가, 마지막에 이름 변경 스토리지에서 삭제한다.
  * @param oldFileName : 이전 파일 경로명
  * @param newFileName : 새 파일 경로명
  * @return BOOL : 정상적으로 삭제되면 TRUE를 리턴한다.
  * @details A. 이전 경로명과 새 경로명을 DA_VALUE_SEPARATOR 로 합쳐서 스트링을 만들고 이를 mRenamedStack에 저장한다.
  */
  BOOL OnStoreCancelRenamed(LPCTSTR oldFileName, LPCTSTR newFileName);

  /**
  * @brief mRenamedStack에 저장된 이름 변경 내역을, 이름 변경 스토리지에서 삭제한다.
  * @param clearData : TRUE이면 이름 변경 스토리지에서 삭제한다.
  * @details A. mRenamedStack의 각 항목에 대해,
  *     - clearData가 TRUE이면,
  *       + oldname, newname으로 분리하여 KSyncRootStorage::RemoveRenameItem()으로 이름 변경 스토리지에서 삭제한다.
  *     - 항목을 해제한다.
  *     .
  * @n B. mRenamedStack를 클리어한다.
  */
  void clearCancelRenamedItem(BOOL clearData);

  // BOOL GetValidLocalRenamePath(LPTSTR newRelativePath);

#ifdef USE_BASE_FOLDER
  virtual LPCTSTR GetRootPath() { return mBaseFolder; }
#else
  virtual LPCTSTR GetRootPath() { return mpRelativePath; }
#endif

#if defined(USE_SCAN_BY_HTTP_REQUEST) && defined(USE_SYNC_GUI)
  /** 마지막 성공한 동기화 작업시에 하위 항목 조회를 요청한 시간. 다음번 하위 항목 조회를 요청할 때 이 시간값을 이용한다. */
  __int64 mPreviousScanTime;

  /** 마지막 하위 항목 조회를 요청한 시간. 동기화가 완료되면 이 값이 mPreviousScanTime으로 복사된다 */
  __int64 mLastScanTime;

  /** getModifiedFolderNDocument API로 요청하여 다운로드 받은 데이터를 저장하는 버퍼 */
  LPSTR m_ScanFolderData;

  /** getModifiedFolderNDocument API로 요청하여 다운로드 받은 데이터 버퍼의 크기 */
  int m_ScanFolderDataLength;

  /** getModifiedFolderNDocument API로 요청한 작업의 플래그 */
  int m_ScanFolderFlag;

  /** getModifiedFolderNDocument API로 요청하여 다운로드 받은 데이터에서 처리한 버퍼 오프셋 */
  int m_ScanFolderIndex;

  /** getModifiedFolderNDocument API로 요청한 폴더 항목 */
  KSyncFolderItem* m_ScanFolderItem;

  /** getModifiedFolderNDocument API로 요청한 데이터 파싱할때 이전 항목의 부모 폴더 캐시. */
  KSyncFolderItem* m_ScanParseLastParent;

  /**
  * @brief 마지막 성공한 동기화 작업시에 하위 항목 조회를 요청한 시간을 설정한다.
  */
  inline void SetPreviousScanTimeStamp(__int64 scanTime) { mPreviousScanTime = scanTime; }

  /**
  * @brief 마지막 성공한 동기화 작업시에 하위 항목 조회를 요청한 시간을 받아간다.
  */
  inline __int64 GetPreviousScanTimeStamp() { return mLastScanTime; }

  /** getModifiedFolderNDocument API로 요청한 결과값. */
  KDownloaderError mServerFolderScanResult;

  /**
  * @brief getModifiedFolderNDocument API로 요청한 결과값을 설정한다.
  */
  inline void SetServerFolderScanResult(KDownloaderError r) { mServerFolderScanResult = r; }
  /**
  * @brief getModifiedFolderNDocument API로 요청한 결과값을 받아간다.
  */
  inline KDownloaderError GetServerFolderScanResult() { return mServerFolderScanResult; }

#endif // USE_SCAN_BY_HTTP_REQUEST && USE_SYNC_GUI

#if defined(USE_SYNC_GUI) && defined(PURE_INSTANT_BACKUP)
  // 실시간 백업의 대상 항목 리스트
  KVoidPtrArray mModifiedItemList;
  // 실시간 백업의 대상 항목 리스트 db key
  LPTSTR mModifiedKey;

  BOOL AddModifiedItemList(KVoidPtrArray& modifiedList);

  void SetModifiedListKey(LPCTSTR modifiedKey);

#endif // USE_SYNC_GUI

  inline int GetDBVersion() { return mDbVersion; }

private:

  /** 이 동기화 루트 폴더의 동기화 정책 */
  KSyncPolicy mPolicy;

#ifdef USE_META_STORAGE

  /** 이 동기화 루트 폴더의 스토리지 개체 */
  KSyncRootStorage* mStorage;

  /** Old storage class used on Data immigration */
  KSyncRootStorage* moldStorage;
#endif

  /** 동기화 작업 상태. none, pending, synching, sync-done, sync-paused */
  int mSynchState;
  /** 동기화 일시 정지 상태. pause/resume */
  int mSyncPaused;
  /** 동기화 방식. manual or automatic */
  int mSyncMethod;

  int mDbVersion;

public:
  /** 동기화 전체 크기 */
  __int64 mSyncTotalSize;
  /** 동기화 완료된 크기 */
  __int64 mSyncDoneSize;

  /** 동기화 완료율을 계산하기 위한 완료된 크기 */
  __int64 mSyncDoneSizeProgress;
  /** 동기화 완료율을 계산하기 위한 전체 크기 */
  __int64 mSyncTotalSizeProgress;

  /** 동기화 전체 항목 개수 */
  int mSyncTotalCount;
  /** 동기화 처리된 폴더 항목 개수 */
  int mSyncFolderCount;
  /** 동기화 처리된 파일 항목 개수 */
  int mSyncFileCount;

  /** 동기화 대상 폴더 항목 개수 */
  int mTargetFolderCount;
  /** 동기화 대상 파일 항목 개수 */
  int mTargetFileCount;


  /** 동기화 작업중 업로드/다운로드 진행하는 파일의 전체 크기 */
  __int64 mSyncItemFileSize;
  /** 동기화 작업중 업로드/다운로드 진행하는 파일의 완료된 크기 */
  __int64 mSyncProcessedSize;

  /** 동기화 작업 시작 시간 */
  SYSTEMTIME mSyncBeginTime; // start
  /** 동기화 업로드/다운로드 시작 시간. 동기화 상태창의 진행률은 이 시간부터 계산한다 */
  SYSTEMTIME mSyncStartTime;
  /** 동기화 작업 완료 시간 */
  SYSTEMTIME mSyncDoneTime;

  // working buffer
  /** 부분 동기화 폴더 경로명 */
  LPTSTR mSyncOnlyPath;
  /** 동기화 추가 정보, 복원 작업시 기준 날짜 저장 */
  LPTSTR mSyncExtraInfo;
  /** 마지막 동기화 완료 시간 */
  SYSTEMTIME mLastSyncCompleteTime;

  /** 동기화 작업중인 파일 목록 */
  KVoidPtrArray mSyncingItems;

  /** 동기화 작업중 이름 변경 항목 목록 */
  KVoidPtrArray mRenamedStack;

#ifdef USE_BACKUP
  /** 서버 폴더에서 삭제 처리될 항목 리스트 */
  LPSTR mDOS_FolderList;
  int mDOS_FolderListSize;
  LPSTR mDOS_FileList;
  int mDOS_FileListSize;

  /* 실시간 백업 Retry 회수 */
  int mInstantBackupRetry;

  TCHAR mRestoreFolderOID[MAX_OID];

  /**
  * @brief 복원할 서버측 폴더 OID를 저장한다.
  */
  void SetRestoreFolderOID(LPCTSTR folderOID) { StringCchCopy(mRestoreFolderOID, MAX_OID, folderOID); }

  /**
  * @brief 백업 폴더에서 삭제 처리될(~#*.*) 항목으로 등록한다.
  * @n details 폴더는 mDOS_FolderList에, 파일은 mDOS_FileList에 삭제될 항목의 OID를 추가한다.
  */
  BOOL RegistDeleteOnServer(KSyncItem* item);

  /**
  * @brief 백업 폴더에서 삭제 처리될 항목을 서버측에 삭제요청한다.
  * @n details gpCoreEngine->RequestRemoveFiles()을 호출하여 삭제를 요청한다.
  */
  int RequestDeleteOnServerItemList();

  /**
  * @brief 서버측에서 삭제 처리를 하고 결과를 리턴받았을때 호출된다.
  * @n details 삭제 요청한 목록에서 처리 결과 완료된 항목을 제거하여 삭제가 미완료된 항목만 남겨둔다.
  */
  void OnDeleteOnServerItem(int isFolder, LPSTR data);

  /**
  * @brief 서버측에서 삭제 처리를 하고 결과를 리턴받았을때 호출된다.
  * @n details 삭제 요청한 목록에서 처리 결과 완료된 항목을 제거하여 삭제가 미완료된 항목만 남겨둔다.
  */
  void OnRequestDeleteOnServerItemList(LPSTR data, long length);

  /**
  * @brief 서버측에 삭제 처리 요청이 실패하였을때 호출된다.
  * @n details 삭제 요청한 항목들을 오류처리한다.
  */
  void SetDeleteOnServerItemResult(LPSTR p);

  /**
  * @brief 삭제 목록에서 지정한 항목을 완료로 설정한다.
  */
  BOOL ClearDeleteOnServerItemAsDone(LPSTR list, LPSTR itemOID);

  /**
  * @brief 서버측에 삭제 처리 요청이 실패하였을때 호출된다.
  * @n details 삭제 요청한 항목들을 오류처리한다.
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
  * @brief 동기화 루트 폴더 항목의 리스트를 해제한다.
  */
extern void FreeSyncRootItemList(KSyncRootItemArray &array);

#endif