#include "stdafx.h"
#include "KActionList.h"
#include "../Core/KSettings.h"
#include "../Platform/Platform.h"

#define DEFAULT_UNDO_CACHE_SIZE 1024 * 1024 * 100

KActionList::KActionList()
{
  mActionIndex = 0;

#ifndef _DEBUG
  MEMORYSTATUSEX ms;
  ms.dwLength = sizeof(MEMORYSTATUSEX);
  if (GlobalMemoryStatusEx(&ms))
  {
    mMaxBufferSize = (LONGLONG)(ms.ullAvailPhys / 10);
  }
  else
#endif
  {
    mMaxBufferSize = DEFAULT_UNDO_CACHE_SIZE;
  }
}

KActionList::~KActionList()
{
  Clear();
}

void KActionList::Clear()
{
  ClearActionVector(mActionList);
}

BOOL KActionList::CanUndo()
{
  return (mActionIndex > 0);
}

BOOL KActionList::CanRedo()
{
  return (mActionIndex < mActionList.size());
}

int KActionList::GetUndoCount()
{
  return mActionIndex;
}

KEditAction* KActionList::GetAction(int index)
{
  if (index < (int)mActionList.size())
  {
    KActionIterator it = mActionList.begin();
    if (index > 0)
      it += index;
    return *(it++);
  }
  return NULL;
}
\
void KActionList::SaveAction(EditActionType type, LPBYTE pParam, int p_length, LPBYTE pBackup, int b_length)
{
  HGLOBAL p = NULL;
  HGLOBAL b = NULL;
  
  if ((pParam != NULL) && (p_length > 0))
    p = KEditAction::CopyActionBuffer(pParam, p_length);
  if ((pBackup != NULL) && (b_length > 0))
    b = KEditAction::CopyActionBuffer(pBackup, b_length);

  SaveAction(type, p, b);
}

void KActionList::SaveAction(EditActionType type, HANDLE pHandle, HANDLE bHandle)
{
  KEditAction* a = new KEditAction(type, pHandle, bHandle);
  SaveAction(a);
}

void KActionList::SaveAction(KEditAction* a)
{
  // clear undo-done items
  if (mActionIndex < mActionList.size())
  {
    KActionIterator it = mActionList.begin();
    if (mActionIndex > 0)
      it += mActionIndex;

    while (it != mActionList.end())
    {
      KEditAction* a = *it;
      delete a;
      it = mActionList.erase(it);
    }

    ASSERT(mActionIndex == mActionList.size());
  }

  mActionList.push_back(a);
  mActionIndex = mActionList.size();

  TCHAR msg[256];
  a->ToString(msg, 256);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  CheckBuferLimit();
  //Dump();
}

KEditAction* KActionList::Undo()
{
  if (mActionIndex > 0)
  {
    KActionIterator it = mActionList.begin();
    it += (mActionIndex - 1);
    mActionIndex--;
    return *it;
  }
  return NULL;
}

KEditAction* KActionList::Redo()
{
  if (mActionIndex < mActionList.size())
  {
    KActionIterator it = mActionList.begin();
    if (mActionIndex > 0)
      it += mActionIndex;
    mActionIndex++;
    return *it;
  }
  return NULL;
}

void KActionList::CheckBuferLimit()
{
  __int64 size = 0;
  KActionIterator it = mActionList.begin();
  while (it != mActionList.end())
  {
    size += (*(it++))->GetMemoryUsed();
    if (size >= mMaxBufferSize)
      break;
  }

  // free items
  while (it != mActionList.end())
  {
    KEditAction* a = *it;
    delete a;
    it = mActionList.erase(it);
  }

#ifdef _DEBUG
  TCHAR msg[256];
  int sizef = (int)(size * 100 / mMaxBufferSize);
  TCHAR maxSizeStr[16];
  FormatFileSize(maxSizeStr, 16, mMaxBufferSize);
  StringCchPrintf(msg, 256, _T("items=%d, Used=%d%%, total=%s"), mActionList.size(), sizef, maxSizeStr);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
}

void KActionList::Dump()
{
#ifdef _DEBUG
  TCHAR msg[256];
  __int64 size = 0;
  KActionIterator it = mActionList.begin();
  while (it != mActionList.end())
  {
    size += (*(it++))->GetMemoryUsed();
  }
  int sizef = (int)(size * 100 / mMaxBufferSize);
  TCHAR maxSizeStr[16];
  FormatFileSize(maxSizeStr, 16, mMaxBufferSize);
  StringCchPrintf(msg, 256, _T("items=%d, Used=%d%%, total=%s"), mActionList.size(), sizef, maxSizeStr);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
}
