/**
* @file KSyncCoreEngine.h
* @date 2015.02
* @brief KSyncCoreEngine class header file
*/
#pragma once

#ifndef _LOCALSYNC_SYNCCOREENGINE_CLASS_H_
#define _LOCALSYNC_SYNCCOREENGINE_CLASS_H_

// #define EMUL_TEST
#define THREADED_LOADING

#include "KSyncCoreBase.h"
#include "KSyncItem.h"
#include "KSyncRootFolderItem.h"

#ifdef USE_SYNC_GUI
#include "KUpDownLoader.h"
#endif

#if defined(DIR_MONITOR_ON_ENGINE) || defined(DRIVE_MONITOR_ON_ENGINE)
#include "../DirectoryMonitor/KDirectoryMonitor.h"
#endif

#define SELECT_FOLDER 1
#define VIEW_PROPERTY 2

// SetFolderStatusChanged check_flag
#define FS_CHECK_FILETIME 1
#define FS_EXCLUDE_ITSELF 2
#define FS_SKIP_ALREADY_SYNCED 4

/**
* @class KSyncCoreEngine
* @brief 동기화 엔진의 implementation class. 이 엔진은 로컬싱크Exec 모듈뿐만 아니라
* Shell Extension인 OverIconDLL, ShellExt에서도 사용되는 모듈이다
*/
class KSyncCoreEngine : public KSyncCoreBase
{
public:
  KSyncCoreEngine(void);
  virtual ~KSyncCoreEngine(void);

  /**
  * @brief 개체가 삭제될 때, 내부 데이터를 Clear하는 가상함수
  */
  void ClearInternal();

  /**
  * @brief 개체가 삭제될 때, 외부 데이터를 Clear하는 가상함수
  */
  virtual void ClearExternal();

  /**
  * @brief 개체가 생성되고 초기화 될때 호출되는 가상함수
  */
  virtual BOOL Ready(BOOL asMain);

  static DWORD MakeSessionID();

  static void ClearSessionID();

  /**
  * @brief 로그인과 DeviceReadyToService가 완료되고 로그인 처리가 안되었으면 TRUE를 리턴한다.
  */
  BOOL CheckEngineReady(int state);

  /**
  * @brief 로그인 또는 로그아웃 되었을 때  호출되는 가상함수
  *@return BOOL : 동기화 타이머가 필요하면 TRUE를 리턴한다.
  */
  virtual BOOL OnLogged(BOOL bOn);

  /**
* @brief 동기화 설정을 위해 서버측 대상 폴더를 선택하는 대화상자를 연다.
* 이 대화상자는 ECM Agent thread에서 실행되므로 background thread 에서 호출하고, 결과는 메인 다이얼로그에서 받는다.
* @details A. THREAD_CALL_SATA struct를 준비하여 필요한 내용을 복사한 후,
* @n B. _beginthreadex() 메소드로 background thread로 procSelectFolderThread() 함수를 실행한다.
*/
  virtual BOOL SelectSyncServerFolderLocation(HWND hwnd, LPCTSTR localFolder, LPCTSTR server_folder_oid);

  /**
  * @brief 서버측 대상 폴더 하위에 새 폴더를 생성한다.
  * @param root : 동기화 루트 폴더.
  * @param server_folder_oid : 대상 폴더 OID.
  * @param server_folder_index : 대상 폴더 fullPathIndex.
  * @param name : 생성할 폴더 이름.
  * @param out_child_folder_oid : 생성된 폴더의 OID를 받아오는 버퍼.
  * @param out_child_folder_info : 생성된 폴더의 정보를 받아오는 버퍼.
  * @param pErrorMsg : 오류 발생시 오류 메시지를 받아오는 버퍼.
  * @return BOOL : 폴더가 생성되었으면 TRUE를 리턴한다.
  * @details A. LocalSync_InitEnv() 메소드를 호출하여 ERROR_ST struct를 초기화한다.
  * @n B. LocalSync_CreateFolder() 메소드를 호출한다.
  * @n C. 오류가 발생하였으면 해당 오류 메시지를 복사하여 두고,
  * @n D. 성공적으로 생성되었으면 폴더 정보와 생성된 폴더 OID를 복사한다.
  * @n E. 할당된 메모리를 해제한다.
  */
  virtual BOOL SyncServerCreateFolder(KSyncRootFolderItem *root, LPCTSTR IN server_folder_oid, LPCTSTR IN server_folder_index,
    LPCTSTR IN name, LPTSTR OUT out_child_folder_oid, SERVER_INFO& OUT out_child_folder_info, LPTSTR* OUT pErrorMsg);

  /**
  * @brief 서버측 대상 폴더의 폴더 이름 또는 전체 경로를 받아온다.
  * @param server_folder_oid : 대상 폴더 OID.
  * @param folder_name : 폴더 이름을 받아올 버퍼
  * @param fullPath : TRUE이면 폴더의 전체 경로를 복사한다.
  * @details A. LocalSync_InitEnv() 메소드를 호출하여 ERROR_ST struct를 초기화한다.
  * @n B. LocalSync_GetFolder() 메소드를 호출한다.
  * @n C. 오류가 발생하였으면 해당 오류 메시지를 복사하여 두고,
  * @n D. 성공하였으면 폴더 이름 또는 전체 경로를 복사한다.
  * @n E. 할당된 메모리를 해제한다.
  */
  virtual BOOL SyncServerGetFolderName(LPCTSTR server_folder_oid, LPTSTR folder_name, BOOL fullPath, LPTSTR folder_index);


  /*
  *  docTypeOID + '|' + docTypeName + ',' + ,,,구분자
  */
  int SyncServerGetDocTypesForFolder(LPCTSTR serverFolderOID, LPTSTR* pDocTypes);
  int SyncServerGetDocTypeData(LPCTSTR docTypeOID, void* ppXDocTypes);

  int SyncServerGetDocTypeDatasForAll(void* ppXDocType, int* pCount);

  int SyncServerFreeDocTypeData(void* pXDocType);

  int SyncServerFreeDocTypeData(void* pXDocType, int size);

  virtual  int SyncServerUploadFile(LPCTSTR folderOid, LPCTSTR folderIndex, LPTSTR docOID, LPTSTR serverFileOID,
    LPTSTR serverStorageOID, LPCTSTR name, LPCTSTR localFileName, KEcmDocTypeInfo* pInfo, KMetadataInfo* pMetadata, LPTSTR* pErrorMessage);

  virtual int SyncServerDownloadFile(LPCTSTR fileOID, LPCTSTR storageOID, LPCTSTR localFileName, LPTSTR* pErrorMessage);

  virtual BOOL SyncServerGetDocTypeInfo(LPCTSTR docTypeOid, KEcmDocTypeInfo* pInfo);

  virtual int SyncServerGetDocumentExt(LPCTSTR IN document_oid, KMetadataInfo* pInfo, LPTSTR* pErrorMessage);

  /**
  * @brief 로그인을 확인하거나 자동 로그인을 처리하기 위한 가상함수
  */
  virtual BOOL CheckAutoLogin(BOOL autoLogin);

  /**
  * @brief 서버의 설정값을 받아와 초기화하는 가상함수
  */
  virtual void InitializeServerSetting(int set, int logoutMethod);

  /**
  * @brief ECM 서버의 설정값을 받아오는 함수
  * @n A. 최근 동기화 항목에 표시할 개수.
  * @n A. 동기화 대상 최대 파일 크기.
  */
  void GetServerConfigValues();

  /**
  * @brief 사용자 로그인 정보를 저장해두기 위한 가상함수
  */
  virtual void SetLoginUserInfo(struct SessionValue *pSession);
  /**
  * @brief 사용자의 동기화 관련 정책을 ECM 서버에서 다시 받아 업데이트 한다.
  * @param flag : 업데이트할 항목 플래그(
  */
  virtual BOOL RefreshSyncPolicyInfo(int flag);

  /**
  * @brief 서버의 동기화 정책을 업데이트 한다.
  * @param pPolicy : 동기화 정책을 저장할 KSyncPolicy object pointer
  * @param updateSetting : TRUE이면 업데이트된 내용을 refreshLogUserInfoSetting() 메소드로 setting 파일에 저장한다.
  * @details 파라미터 PLSDD_POLICY_INFORMATIONEX의 값으로 사용자의 로컬, 보안, 서버 드락이브의 접근 권한과 드라이브의 표시 이름 등을 저장하여 둔다
  */
  virtual BOOL UpdateServerPolicyInfo(void *pPolicy, BOOL updateSetting);

  /**
  * @brief Shell Extersion DLL과 공유하기 위해 사용자 정보를 Setting 파일에 저장한다.
  * @details 보안 드라이브 문자, 사용자 드라이브 권한 정보, 표시 언어,
  * 사용자 OID, Home Folder 경로, 시스템 설정 동기화 제외 형식 등의 값을
  * 탭글자로 구분하여 setting 파일에 저장한다.(IF 설계서의 마)-2항 참조)
  */
  void refreshLogUserInfoSetting();

  /**
  * @brief 대상 폴더가 동기화 루트 폴더인지 체크한다.
  * @param rootFolder : 대상 폴더
  * @details 대상 폴더에 대한 KSyncRootFolderItem을 준비하고 Storage에서 확인하여 리턴한다.
  */
  int IsSyncRootFolder(LPCTSTR rootFolder);


  /**
  * @brief 파일의 업로드/다운로드 진행 상황을 받는다.
  * @param name : 업로드/다운로드 파일명
  * @param progress : 진행률(0~10000)
  * @details A. FindUpDownLoader() 메소드를 호출하여 KUpDownLoader 항목을 찾는다.
  * @n B. KUpDownLoader가 중지되었으면,
  *    - 현재 진행중인 동기화 루트 폴더 항목에서 KSyncRootFolderItem::FindItemExist() 메소드로 대상 파일 항목을 찾는다.
  *    - 대상 항목의 KSyncItem::OnLoadCancel()메소드를 호출한다.
  *    .
  * @n C. 동기화 루트 폴더 항목에서 KSyncRootFolderItem::OnFileSyncProgress() 메소드를 호출한다.
  */
  virtual LRESULT OnFileSyncProgress(LPCTSTR name, int progress);


  /**
  * @brief 대상 폴더의 이름과 부모 폴더 OID를 구한다.
  * @param folderOID : 동기화 대상 폴더의 OID
  * @param parentOID : 부모 폴더의 OID를 받아올 버퍼.
  * @param folderName : 대상 폴더의 이름을 받아올 버퍼.
  * @return BOOL : 성공하면 TRUE를 리턴한다.
  * @details A. 대상 폴더의 정보를 LocalSync_GetFolder() 메소드를 호출하여 받아온다.
  * @n B. 성공하면 폴더 정보의 name를 folderName 버퍼에, 폴더 정보의 parentOID를 parentOID 버퍼에 복사한다.
  * @n C. 할당된 메모리를 해제한다.
  */
  virtual BOOL GetFolderInfo(LPCTSTR IN folderOID, LPTSTR OUT parentOID, LPTSTR OUT folderName);

  /**
* @brief 서버측 대상 폴더를 삭제한다.
* @param folder_OID : 대상 폴더 OID.
* @param folderFullPathIndex : 대상 폴더의 fullPathIndex. 폴더의 위치가 변경되었는지 확인하기 위해 사용됨.
* @param pErrorMsg : 오류 발생시 오류 메시지를 받아오는 버퍼.
* @return BOOL : 오류가 발생하였으면 FALSE를, 아니면 TRUE를 리턴한다.
* @details LocalSync_DeleteFolder() 메소드를 호출하여 해당 폴더를 삭제한다.
*/
  virtual BOOL SyncServerDeleteFolder(LPCTSTR folder_OID, LPCTSTR folderFullPathIndex, LPTSTR* pErrorMsg);

  /**
  * @brief 서버측 대상 폴더가 존재하는지 확인한다.
  * @param folder_OID : 대상 폴더 OID.
  * @param folderFullPathIndex : 대상 폴더의 fullPathIndex. 폴더의 위치가 변경되었는지 확인하기 위해 사용됨.
  * @param pErrorMsg : 오류 발생시 오류 메시지를 받아오는 버퍼.
  * @return int : 존재하지 않거나 오류가 발생하였으면 오류값을, 아니면 0을 리턴한다.
  * @details LocalSync_IsExistedFolder() 메소드를 호출하여 해당 폴더를 확인한다.
  */
  virtual int SyncServerCheckFolderExist(LPCTSTR folder_OID, LPCTSTR folderFullPathIndex, LPTSTR pErrorMsg);

  /**
* @brief 메인 윈도우에 해당 메시지를 보낸다.
* @param msg : 메시지 ID
* @param wParam : 파라미터
* @param lParam : 파라미터
* @details A. AfxGetApp()->GetMainWnd() 함수로 메인 윈도우 핸들을 받는다.
* @n B. CWnd::PostMessage() 로 윈도우에 메시지를 보낸다.
*/
  virtual void PostMessageToMain(int msg, WPARAM wParam, LPARAM lParam);

  int mEngineReady;

#if defined(USE_SCAN_BY_HTTP_REQUEST)
  /**
  * @brief 사용자 세션키값. 로그인 이벤트에서 받아 저장해 둔다.
  */
  TCHAR m_SessionKey[60];
  /**
  * @brief StartScanFolder()에서 사용하는 서버 URL 주소
  */
  LPTSTR m_RpcServerUrl;

  inline LPCTSTR GetUserSessionKey() { return m_SessionKey; }
  inline LPCTSTR GetRpcServerUrl() { return m_RpcServerUrl; }

#endif

};

#endif
