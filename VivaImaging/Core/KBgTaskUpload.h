/**
* @file KBgTaskUpload.h
* @date 2019.01
* @author kylee@k2mobile.co.kr
* @brief Upload background task class header file
*/
#pragma once

#include "stdafx.h"
#include "KBgTaskLoader.h"

#include "../Core/KConstant.h"
#include "../Document/KImagePage.h"
#include "../Document/KImageFolder.h"
#include "../Platform/Utils.h"

/**
* @class KBgTaskUpload
* @brief Upload task class definition
*/
class KBgTaskUpload : public KBgTaskLoader
{
public:
  KBgTaskUpload(CWnd* owner, BgTaskType task, LPCTSTR arg);
  virtual ~KBgTaskUpload(void);


  void SetUploadInfo(LPCTSTR filename, LPCTSTR workingPath, KImageFolder* rootImageFolder,
    KEcmDocTypeInfo* info, ImagingSaveFormat saveFormat, UINT flag);

  UINT mDoneFolder;
  UINT mTotalFolder;
  UINT mDoneFile;
  UINT mTotalFile;

  UINT mFlag;

  KEcmDocTypeInfo* mEcmDocTypeInfo;

  ImagingSaveFormat mSaveFormat;
  LPCTSTR mFilename;
  KImageFolder* mpImageFolder;

  virtual void Do();

private:
  void sendProgressValue();
  int createFolder(EcmFolderItem* folder);
  int uploadFile(EcmFileItem* file);
};

