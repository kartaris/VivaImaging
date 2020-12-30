#include "stdafx.h"
#include "KBgTask.h"

#include "KConstant.h"
#include "KSettings.h"

#include "ThreadPool.h"
#include <strsafe.h>
#include <Shlwapi.h>

#include "../Platform/Platform.h"
#include "../Core/KSettings.h"
#include "../Core/KSyncCoreEngine.h"

#include "HttpDownloader\KString.h"
#include "HttpDownloader\KWnetDownloader.h"

KBgTask::KBgTask(CWnd* owner, BgTaskType task, LPCTSTR arg)
{
  mOwner = owner;
  mTask = task;
  mArg = AllocString(arg);
  mState = BGT_INIT;
  m_hStoppedEvent = NULL;
  mProgressed = 0;
}

KBgTask::~KBgTask(void)
{
  if (mArg != NULL)
  {
    delete[] mArg;
    mArg = NULL;
  }
}

BOOL KBgTask::Start()
{
  mState = BGT_START;

  CThreadPool::QueueUserWorkItem(&KBgTask::ServiceWorkerThread, this);
  return TRUE;
}

BOOL KBgTask::Stop()
{
  mState = BGT_CLOSING;

  // wait closing event
  if (m_hStoppedEvent != NULL)
  {
    if (WaitForSingleObject(m_hStoppedEvent, 10 * 1000 /*INFINITE*/) != WAIT_OBJECT_0)
    {
    }
  }
#ifdef _DEBUG
  TCHAR msg[64];
  StringCchPrintf(msg, 64, _T("Killed forcely at %d%"), GetProgressed());
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
  mState = BGT_CLOSED;
  return TRUE;
}

void KBgTask::ServiceWorkerThread(void)
{
  m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (m_hStoppedEvent == NULL)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("CreateEvent fail"), SYNC_MSG_LOG);
    //throw GetLastError();
  }

  mState = BGT_ING;

  Do();

  // Signal the stopped event.
  SetEvent(m_hStoppedEvent);

  if (mOwner != NULL)
    mOwner->PostMessage(MSG_BGTASK_MESSAGE, BGTASK_DONE, (LPARAM)this);

}

void KBgTask::Do()
{

}

void KBgTask::SendLogMessage(LPCTSTR moduleName, LPCTSTR msg, int msgflag)
{

  if (msgflag & SYNC_EVENT_ERROR)
  {
    if (mError.GetLength() > 0)
      mError += _T("\r\n");
    mError += msg;
  }

  if (mOwner != NULL)
  {
    TCHAR flagstr[16];
    StringCchPrintf(flagstr, 16, _T("%d"), msgflag);
    int len = lstrlen(moduleName);
    len += lstrlen(msg) + lstrlen(flagstr) + 4;

    HANDLE h = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, len * sizeof(TCHAR));
    LPTSTR str = (LPTSTR)GlobalLock(h);
    StringCchCopy(str, len, moduleName);
    StringCchCat(str, len, _T(","));
    StringCchCat(str, len, flagstr);
    StringCchCat(str, len, _T(","));
    StringCchCat(str, len, msg);
    GlobalUnlock(h);

    mOwner->PostMessage(MSG_BGTASK_MESSAGE, BGTASK_LOG_MESSAGE, (LPARAM)h);
  }
}
