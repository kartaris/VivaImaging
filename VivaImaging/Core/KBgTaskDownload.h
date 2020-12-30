/**
* @file KBgTaskDownload.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief Download background task class header file
*/
#pragma once

#include "stdafx.h"
#include "KBgTaskLoader.h"
#include "KEcmBaseItem.h"

class KDownloader;

/**
* @class KBgTaskDownload
* @brief KBgTaskDownload class definition
*/
class KBgTaskDownload : public KBgTaskLoader
{
public:
  KBgTaskDownload(CWnd* owner, BgTaskType task, LPCTSTR arg);
  virtual ~KBgTaskDownload(void);

  virtual void Do();

  KEcmDocTypeInfo* mpDocTypeInfo;
private:

  //HANDLE createDownListData(KVoidPtrArray& fileList);
  int downloadFiles();
  int downloadFileItem(EcmFileItem* item, LPCTSTR localFileName);

};

