#include "stdafx.h"
#include "KBgTaskLocalScan.h"

#include "../Platform/Platform.h"

KBgTaskLocalScan::KBgTaskLocalScan(CWnd* owner, BgTaskType task, LPCTSTR arg)
  :KBgTask(owner, task, arg)
{
}

KBgTaskLocalScan::~KBgTaskLocalScan(void)
{
  ClearTStrArray(mFileList);
}

static int getPathDepth(LPCTSTR pathName)
{
  int depth = 0;
  LPCTSTR p = pathName;
  while (p != NULL)
  {
    p = StrChr(p, '\\');
    if (p != NULL)
    {
      p++;
      depth++;
    }
  }
  return depth;
}

void KBgTaskLocalScan::Do()
{
  // scan first depth
  scanFolder(mFileList, NULL, 1);

  // scan second depth
  KVoidPtrArray childs;
  KVoidPtrIterator it = mFileList.begin();
  while (IsContinue() && (it != mFileList.end()))
  {
    LPCTSTR pathName = (LPCTSTR)*(it++);
    int folderSubDepth = getPathDepth(pathName);
    if (folderSubDepth == 0)
    {
      LPTSTR fullName = AllocPathName(mArg, pathName);
      if (IsDirectoryExist(fullName))
      {
        scanFolder(childs, pathName, 1);
      }
      delete[] fullName;
    }
  }

  AddVoidPtrArray(mFileList, childs);
}

int KBgTaskLocalScan::scanFolder(KVoidPtrArray& childs, LPCTSTR path, int depthLimit)
{
  LPTSTR findPath = NULL;
  int count = 0;
  if (path != NULL)
    findPath = AllocPathName(mArg, path);
  else
    findPath = (LPTSTR)mArg;

  TCHAR filename[MAX_PATH];
  MakeFullPathName(filename, MAX_PATH, findPath, _T("*"));
  if (findPath != mArg)
    delete[] findPath;

  WIN32_FIND_DATA fd;
  memset(&fd, 0, sizeof(WIN32_FIND_DATA));
  fd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;

  HANDLE h = FindFirstFile(filename, &fd);
  if (h != NULL)
  {
    do {
      if (lstrlen(fd.cFileName) <= 0)
        continue;

      if ((CompareStringNC(fd.cFileName, L".") == 0) ||
        (CompareStringNC(fd.cFileName, L"..") == 0))
        continue;

      if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        int depth = getPathDepth(path);
        if (depth >= depthLimit)
          continue;
      }
      if (lstrlen(fd.cFileName) > 0)
      {
        LPTSTR pathName = AllocPathName(path, fd.cFileName);
        childs.push_back(pathName);
        count++;
      }
    } while (IsContinue() && FindNextFile(h, &fd));
    FindClose(h);
  }
  else
  {
    // mError = _T("path not found");
  }
  return count;
}
