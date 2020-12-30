// VivaCommandInfo.h: KVivaCommandInfo 헤더
// Copyright (C) 2019 K2Mobile
// All rights reserved.


#pragma once

#include "afxwinappex.h"
#include "Core\KConstant.h"
#include "Document\KImageBase.h"

class KVivaCommandInfo : public CCommandLineInfo
{
public:
  KVivaCommandInfo();
  virtual ~KVivaCommandInfo();

  virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

  void Dump();

  DWORD mSystemOption;

  CString mFilenameFormat;

  CString mDefautImageExt;

  ImagingSaveFormat mDefautSaveFormat;

  CString mMetadataStr;

  CString mOpenLocalFolder;
  CString mOpenServerFolder;
  CString mOpenServerFile;
  CString mUploadServerFolder;
};

