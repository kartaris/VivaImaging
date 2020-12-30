/**
* @file KBgTaskLoader.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief Load background task class header file
*/
#pragma once

#include "stdafx.h"
#include "KBgTask.h"
#include "KEcmBaseItem.h"
#include "KEcmFolderItem.h"

#define R_WARN_SERVER_BUSY -2
#define R_FAIL_SCAN_SERVER_FOLDERS -3

class KDownloader;

/**
* @class KBgTaskLoader
* @brief Upload download base class definition
*/
class KBgTaskLoader : public KBgTask
{
public:
  KBgTaskLoader(CWnd* owner, BgTaskType task, LPCTSTR arg);
  virtual ~KBgTaskLoader(void);

  void SetWorkingFolder(LPCTSTR pathName);
  void ClearDownFileInfoArray(KVoidPtrArray& list);

  /*
  KVoidPtrArray mFolderList;
  KVoidPtrArray mFileList;
  */
  EcmBaseItem* mEcmRoot;
  int mFailCount;

  int mDoneFile;
  int mTotalFile;
  int mDoneFolder;
  int mTotalFolder;

  int mResult;
  int mDownloadResult;

  LPBYTE mDownloadData;
  long mDownloadLength;
  LPTSTR mWorkingPath;

  int queryRequestFolderNDocument(KDownloader* downloader, LPCTSTR folderOID);

  int DownloadFolderList(CString& download_err);

  int parseFolderNDocument();

  EcmBaseItem* CreateFromStream(LPSTR d, long data_len, int& rtn);


  int GetLoadNeededFolder(int needed_flag);

  int GetLoadNeededFile(int needed_flag);

  EcmBaseItem* findFolderItemByName(LPCTSTR name);

  EcmBaseItem* findFileItemByName(LPCTSTR parentName, LPCTSTR name);

  EcmBaseItem* findFolderItemByOID(LPCTSTR oid);

  EcmBaseItem* findFileItemByOID(LPCTSTR oid);

  HANDLE createListData(KVoidPtrArray& fileList);

};

