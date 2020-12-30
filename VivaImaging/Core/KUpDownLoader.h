/**
* @file KUpDownLoader.h
* @date 2015.02
* @brief UpDownLoader class header file
*/

#pragma once

#ifndef _DESTINY_FILE_UP_DOWN_LOAD_CLASS_H_
#define _DESTINY_FILE_UP_DOWN_LOAD_CLASS_H_

#include "stdafx.h"
#include "KSyncItem.h"

#ifdef USE_SYNC_GUI
#include "../HttpDownloader/KHttpHeader.h"
#endif
// DestinyLocalSyncLib
#include "DestinyLocalSyncLib/common/DestinyLocalSyncIFShared.h"

#define USE_SCAN_BY_WORKER_THREAD
//class KWinSockDownloader;
class KDownloader;

typedef enum tagUpDownLoaderJob {
  /** file upload */
  FILE_UP,
  /** file download */
  FILE_DOWN,
  /** modified folder scan */
  FOLDER_SCAN,

#ifdef USE_BACKUP
  /** F-1303 by login */
  REQUEST_BACKUP_POLICY,
  /** F-1303 by change-policy push notification */
  REFRESH_BACKUP_POLICY,
  /** F-1304, p1=복구 이벤트 정보(type=F&s-count-folder=0&f-count-folder=0&s-count-file=1&f-count-file=1&message=백업작업이 완료되었습니다) */
  REGIST_BACKUP_EVENT,
  /** F-1305 */
  REQUEST_RESTORE_INFO,
  /** F-1306, p1=복구 폴더 OID */
  CLEAR_RESTORE_INFO,
  /** F-1307 */
  REQUEST_TRAFFIC_LEVEL,
  /** F-1308 */
  REQUEST_UNREADED_MESSAGE,
  /** F-1309, unused */
  CONFIRM_READ_MESSAGE,
  /** F-1310, p1=폴더OID '|' 리스트, p2=파일OID '|' 리스트 */
  REQUEST_REMOVE_FILES,
  /** F-1311, root-folder-oid=123456789&pc-id=0024ef0a310c */
  REQUEST_USE_BACKUP_FOLDER,
#endif

  /** max job */
  MAX_UPDOWN_JOB
  }UpDownLoaderJob;

typedef enum tagUpDownLoaderState {
  LOADER_INIT,
  LOADER_ING,
  LOADER_CLOSING,
  LOADER_CLOSED
}UpDownLoaderState;

/**
* @class KUpDownLoader
* @brief 파일의 업로드/다운로드 및 폴더의 하위 목록을 조회하는 HTTP 다운로드 기능을 수행하는 클래스.
* 파일의 업로드/다운로드는 Worker Thread에서 실행되고, HTTP 다운로드 기능은 소켓이 윈도우 UI Thread에서 동작하여야 하는 관계로 UI Thread에서 실행된다.
*/
class KUpDownLoader
{
public:
  KUpDownLoader(KSyncItem* root, UpDownLoaderJob job, LPCTSTR url, LPCTSTR localFileName, LPCTSTR serverOID,
    LPCTSTR serverFileOID, LPCTSTR serverStorageOID, LPCTSTR parentFolderOID, LPCTSTR parentFullPathIndex, __int64 timeStamp, void *p);
  KUpDownLoader(KSyncItem* root, UpDownLoaderJob job, LPCTSTR url, LPCTSTR localFileName, LPCTSTR serverOID,
    LPCTSTR p1, LPCTSTR p2);

  ~KUpDownLoader(void);

  /** 작업의 종류, 업로드/다운로드/폴더조회 */
  UpDownLoaderJob mJob;

  KSyncItem* mpRoot;

  /** 서버 조회 URL */
  TCHAR mServerUrl[KMAX_PATH];
  /** 로컬 파일 경로명 */
  TCHAR mLocalFileName[KMAX_PATH];
  /** 서버측 OID */
  TCHAR mServerOID[MAX_OID];
  /** 서버측 fileOID */
  TCHAR mServerFileOID[MAX_OID];
  /** 서버측 storageOID */
  TCHAR mServerStorageOID[MAX_STGOID];
  /* checkout을 위한 parentFolder OID 정보 */
  TCHAR mParentFolderOID[MAX_OID];
  /* checkout을 위한 parentFolder FullPathIndex 정보 */
  LPTSTR mParentFullPathIndex;
  // 업로드 완료된 파일 정보
  LocalSyncUploadFileDetails mUploadFileDetails;
  SYSTEMTIME mInitialLastModified;

  /** 서버 조회 기준 타임 스탬프 */
  __int64 mTimeStamp;
  /** 대상 동기화 항목 포인터 */
  void *mObject;
  /** 중지하는 중 플래그 */
  BOOL mStopping;

  LPTSTR mParam1;
  LPTSTR mParam2;

  /** 업다운 로더 상태 */
  UpDownLoaderState mState;
  /** 업다운 로더 완료 상태 */
  BOOL mDone;

  /** http request retry count */
  int mRetry;

  inline KSyncItem* GetRootItem() { return mpRoot; }

  /**
  * @brief 업다운 로더 상태를 리턴한다.
  */
  inline UpDownLoaderState GetState() { return mState; }

  BOOL mModifiedOnLoading;
  inline void SetModifiedOnUploading() { mModifiedOnLoading = TRUE; }


  BOOL IsNeedTimer() { return (mJob > FILE_DOWN); }

  /**
  * @brief 폴더 목록 조회에서 다운받은 폴더와 파일 개수를 리턴한다.
  */
  BOOL GetProcessedFolderFileCount(int& folders, int& files);

  /** 폴더 목록 조회에서 다운받은 데이터 */
  LPBYTE mScanData;
  /** 폴더 목록 조회에서 다운받은 데이터 크기 */
  long mScanDataLength;
  /** 폴더 목록 조회에서 다운받은 데이터의 플래그(전체를 받았는지, 특정 시점이후 변경된 항목만 받았는지) */
  int mScanFlag;

  /** 업다운 로더 결과값 */
  int mResult;

#ifdef USE_HTTP_DOWNLOADER
  /** HTTP 다운로더 */
  KDownloader* mDownloader;
  /** 폴더 목록 조회에서의 결과 */
  KDownloaderError mServerFolderScanResult;
#endif

  /** 폴더 목록 조회에서의 오류 메시지 */
  LPTSTR mErrorMessage;

  /**
  * @brief 작업을 시작한다.
  * @details A. 폴더 조회 작업은 메인 Thread에서 실행되므로, 별도로 StartAsIn()으로 시작하고, 리턴한다.
  * @n B. 업로드/다운로드 작업은 CThreadPool::QueueUserWorkItem()을 호출하여 백그라운드 쓰레드에서 ServiceWorkerThread()함수가 실행되도록 한다.
  */
  void Start();

  /**
  * @brief 폴더 조회 작업을 준비한다.
  * @details A. 폴더 조회 요청을 할 폴더의 OID와 TimeStamp를 포함하는 http request message를 작성한다.
  * @n B. HTTP 다운로더인 mDownloader에 Base URL과 request 내용을 설정하고, 
  * KHttpDownloader::DownloadRequest()를 호출하여 소켓을 연결하고 요청 메시지를 큐에 저장한다.
  * @n C. mLocalFileName에 사용자 세션 정보가 저장되어 있으면 이 정보가 있으면, EncodeBase64()으로 인코딩하여, 
  * KHttpDownloader::SetSessionKey()으로 세션키값을 설정한다.
  */
  BOOL StartAsIn();

  /**
  * @brief 폴더 조회 작업을 진행한다.
  * @details A. KHttpDownloader::ProcessDownload()를 호출하여 작업을 진행한다. 이 함수에서 소켓에 의한 HTTP request를 보내고 응답을 받는다.
  * @n B. 작업이 완료되지 않았으면 TRUE를 리턴한다.
  * @n C. KHttpDownloader가 다운로드를 완료하면 결과값을 받아오고, 결과가 200이면 다운로드 완료한 데이터를 mScanData에 받아온다.
  * @n D. 아니면 오류 코드와 오류 메시지를 받아서 mErrorMessage에 저장한다.
  * @n E. 작업이 완료되었으니 FALSE를 리턴한다.
  */
  BOOL ContinueAsIn();

  /**
  * @brief Worker Thread에서 실행될 함수.
  * @details A. 작업 종류가 다운로드이면,
  *     - LocalSync_FileDownload()함수로 파일 다운로드를 시작한다. 실패하면,
  *       + 오류 내용을 mErrorMessage에 저장하고 결과를 FILE_SYNC_DOWNLOAD_FAIL로 한다.
  *     .
  * @n B. 업로드이면,
  *     - LocalSync_FileUpload()함수로 파일 업로드를 시작한다. 실패하면,
  *       + 오류 내용을 mErrorMessage에 저장하고 결과를 FILE_SYNC_UPLOAD_FAIL로 한다.
  *     .
  */
  void ServiceWorkerThread(void);

  /**
  * @brief 작업을 멈춘다.
  * @details A. 작업 종류가 폴더 조회이면,
  *     - mDownloader의 CloseDownloadSocket()으로 소켓을 닫고 해제한다.
  *     - m_fStopping을 TRUE로 하고, WaitForSingleObject()으로 m_hStoppedEvent이벤트를 기다린다.
  */
  void OnStop();

  /**
  * @brief HTTP 다운로더를 해제한다.
  */ 
  BOOL OnClose();

  /**
  * @brief 작업이 완료되었는지 확인한다.
  * @details A. 작업 종류가 폴더 조회이면, ContinueAsIn()을 호출하고 그 리턴값이 TRUE이면 FALSe를 리턴한다.
  * @n B. mResult가 0보다 크거나 같으면 TRUE를 리턴한다.
  */ 
  BOOL IsDone();

  /** 작업을 중지하는 중이면 TRUE를 리턴한다. */
  inline BOOL IsStopping() { return mStopping; }

  /** 작업 중지 플래그. */
  //BOOL m_fStopping;

  /** 작업 중지 완료 이벤트 핸들. */
  HANDLE m_hStoppedEvent;

};

#endif
