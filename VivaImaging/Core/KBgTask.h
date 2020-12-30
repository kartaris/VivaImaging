/**
* @file KBgTask.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief Background task base class header file
*/
#pragma once

#include "stdafx.h"

/**
* @brief KBgTask task type enumerator.
*/
typedef enum tagBgTaskType
{
  BGTASK_EMPTY,
  BGTASK_LOCAL_SCAN,
  BGTASK_FOLDER_DOWNLOAD,
  BGTASK_UPLOAD,
  BGTASK_FILE_DOWNLOAD,

  BGTASK_EFFECT_RENDER,
  BGTASK_UNKNOWN
}BgTaskType;

/**
* @brief KBgTask task state enumerator.
*/
typedef enum tagBgTaskState {
  BGT_INIT,
  BGT_START,
  BGT_ING,
  BGT_CLOSING,
  BGT_CLOSED,
  BGT_DONE
}BgTaskState;

/**
* @brief KBgTask progress information structure.
*/
typedef struct tagTaskProgressInfo {
  UINT totalFolders;
  UINT totalFiles;
  UINT doneFolders;
  UINT doneFiles;
  HANDLE hErrorMsg;
}TaskProgressInfo;

#define _DEBUG_SLOW

#define MSG_BGTASK_MESSAGE (WM_USER + 440)

#define BGTASK_START 1
#define BGTASK_PROGRESS 2
#define BGTASK_DONE 3

#define BGTASK_DOWNLIST_READY 10
#define BGTASK_DOWNLIST_DONE 11
#define BGTASK_UPLOAD_DONE 12
#define BGTASK_LOG_MESSAGE 13
#define BGTASK_DOWNLOAD_ENDED 14

class KDownloader;

/**
* @class KSyncFolderItem
* @brief KBgTask class definition
*/
class KBgTask
{
public:
  KBgTask(CWnd* owner, BgTaskType task, LPCTSTR arg);
  virtual ~KBgTask(void);

  CWnd* mOwner;
  BgTaskType mTask;
  BgTaskState mState;
  LPCTSTR mArg;
  HANDLE m_hStoppedEvent;
  CString mError;
  UINT mProgressed;

  inline BgTaskType GetType() { return mTask; }

  /**
  * @brief KBgTask worker thread body.
  */
  void ServiceWorkerThread(void);
  
  /**
  * @brief show and store log message string
  */
  void SendLogMessage(LPCTSTR moduleName, LPCTSTR msg, int msgflag);

  /**
  * @brief set progress value of this task
  * @param int step progress value(0~100)
  */
  inline void SetProgressed(int step) { mProgressed = step; }
  /**
  * @brief return progress value of this task
  * @return int progress value(0~100)
  */
  inline UINT GetProgressed() { return mProgressed; }

  /**
  * @brief start this thread task
  */
  virtual BOOL Start();
  /**
  * @brief stop this thread task
  */
  virtual BOOL Stop();
  /**
  * @brief virtual function of body of thread task
  */
  virtual void Do();
  /**
  * @brief check if this thread task is continue
  */
  virtual BOOL IsContinue() { return(mState < BGT_CLOSING); }

  /**
  * @brief set the state of this task as done
  */
  virtual void SetDone() { mState = BGT_DONE; }
  /**
  * @brief check if current state of task is done 
  */
  virtual BOOL IsDone() { return(mState == BGT_DONE); }

};

