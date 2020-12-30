// VivaCommandInfo.cpp : KVivaCommandInfo 구현
// Copyright (C) 2019 K2Mobile
// All rights reserved.


#include "stdafx.h"
#include "Resource.h"
#include "VivaImaging.h"
#include "VivaCommandInfo.h"

#include "Platform\Platform.h"
#include "Core\KConstant.h"
#include "Core\KSettings.h"

#include <strsafe.h>

KVivaCommandInfo::KVivaCommandInfo()
{
  mSystemOption = 0;
}

KVivaCommandInfo::~KVivaCommandInfo()
{
}

void KVivaCommandInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
  if (bFlag)
  {
    TCHAR paramname[64];
    LPCTSTR e = StrChr(pszParam, ':');
    if (e != NULL)
    {
      StringCchCopyN(paramname, 64, pszParam, e - pszParam);
      e++;
    }
    else
    {
      StringCchCopy(paramname, 64, pszParam);
    }

    if (lstrcmpi(paramname, _T("readonly")) == 0)
      mSystemOption |= GO_READONLY;
    else if (lstrcmpi(paramname, _T("editable")) == 0)
      mSystemOption |= GO_EDITABLE;
    else if (lstrcmpi(paramname, _T("newscan")) == 0)
      mSystemOption |= GO_STARTSCAN;
    else if (lstrcmpi(paramname, _T("checkupload")) == 0)
      mSystemOption |= GO_CHECKUPLOAD;
    else if (lstrcmpi(paramname, _T("cryption")) == 0)
      mSystemOption |= GO_USECRYPTION;
    else if (lstrcmpi(paramname, _T("securedisk")) == 0)
      mSystemOption |= GO_TEMPFOLDERONSECUREDISK;
    else if (lstrcmpi(paramname, _T("localsave")) == 0)
      mSystemOption |= GO_ENABLELOCALSAVE;
    else if (lstrcmpi(paramname, _T("mergeannotate")) == 0)
      mSystemOption |= GO_MERGEANNOTATE;

    else if (lstrcmpi(paramname, _T("uploadas_newname_ifexist")) == 0)
      mSystemOption |= GO_UPLOADAS_NEWNAME;
    else if (lstrcmpi(paramname, _T("uploadas_newversion_ifexist")) == 0)
      mSystemOption |= GO_UPLOADAS_NEWVERSION;
    else if (lstrcmpi(paramname, _T("clear_folder_before_upload")) == 0)
      mSystemOption |= GO_CLEARFOLDER_BEFOREUPLOAD;

    else if ((lstrcmpi(paramname, _T("filenameformat")) == 0) && (e != NULL))
    {
      if (gImageFilenameFormat != NULL)
        delete[] gImageFilenameFormat;
      gImageFilenameFormat = AllocString(e);
    }
    else if ((lstrcmpi(paramname, _T("defaultimageext")) == 0) && (e != NULL))
    {
      mDefautImageExt = e;
    }
    else if ((lstrcmpi(paramname, _T("defaultuploadformat")) == 0) && (e != NULL))
    {
      if (lstrcmp(e, _T("jpg")) == 0)
        gSaveFormat = FORMAT_JPG;
      else if (lstrcmp(e, _T("jpeg")) == 0)
        gSaveFormat = FORMAT_JPEG;
      else if (lstrcmp(e, _T("png")) == 0)
        gSaveFormat = FORMAT_PNG;
      else if (lstrcmp(e, _T("tif")) == 0)
        gSaveFormat = FORMAT_TIF;
      else if (lstrcmp(e, _T("tiff")) == 0)
        gSaveFormat = FORMAT_TIFF;
      else if (lstrcmp(e, _T("multi-tif")) == 0)
        gSaveFormat = TIF_MULTI_PAGE;
      else if (lstrcmp(e, _T("multi-tiff")) == 0)
        gSaveFormat = TIFF_MULTI_PAGE;
      else
      {
        gSaveFormat = FORMAT_ORIGIN;
        //TCHAR msg[256] = _T("Unknown format : ");
        //StringCchCat(msg, 256, e);
        //StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      }
    }
    else if ((lstrcmpi(paramname, _T("metadata")) == 0) && (e != NULL))
    {
      mMetadataStr = e;
    }
    else if ((lstrcmpi(paramname, _T("openlocalfolder")) == 0) && (e != NULL))
    {
      mOpenLocalFolder.SetString(e);
    }
    else if ((lstrcmpi(paramname, _T("openserverfolder")) == 0) && (e != NULL))
    {
      mOpenServerFolder.SetString(e);
    }
    else if ((lstrcmpi(paramname, _T("openserverfile")) == 0) && (e != NULL))
    {
      mOpenServerFile.SetString(e);
    }
    else if ((lstrcmpi(paramname, _T("uploadserverfolder")) == 0) && (e != NULL))
    {
      mUploadServerFolder.SetString(e);
    }
#if 0
    else // folder or filename
    {
      int len = 0;
      if (*pszParam == '"')
      {
        pszParam++;
        LPCTSTR ep = StrRChr(pszParam, NULL, '"');
        if (ep != NULL)
          len = ep - pszParam;
      }
      else
      {
        len = lstrlen(pszParam);
      }
      if (len > 0)
      {
        len++;

        mOpenFilename.SetString(pszParam, len);
        if (IsDirectoryExist(mOpenFilename))
          m_nShellCommand = FileNew;
      }
    }
#endif
    return;
  }

  //CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
  if (m_strFileName.GetLength() > 0)
    m_strFileName += _T(" ");
  m_strFileName += pszParam;
  m_nShellCommand = FileOpen;
}

LPTSTR gSaveFormatName[] = {
  _T("FORMAT_UNKNOWN"),
  _T("FORMAT_ORIGIN"),
  _T("FORMAT_JPG"),
  _T("FORMAT_JPEG"),
  _T("FORMAT_PNG"),
  _T("FORMAT_TIF"),
  _T("FORMAT_TIFF"),
  _T("TIF_MULTI_PAGE"),
  _T("TIFF_MULTI_PAGE")
};

void CatSystemOptionName(LPTSTR msg, int len, int option)
{
  if (option & GO_READONLY)
    StringCchCat(msg, len, _T(",readonly"));
  if (option & GO_EDITABLE)
    StringCchCat(msg, len, _T(",editable"));
  if (option & GO_STARTSCAN)
    StringCchCat(msg, len, _T(",newscan"));
  if (option & GO_CHECKUPLOAD)
    StringCchCat(msg, len, _T(",checkupload"));
  if (option & GO_USECRYPTION)
    StringCchCat(msg, len, _T(",cryption"));
  if (option & GO_TEMPFOLDERONSECUREDISK)
    StringCchCat(msg, len, _T(",securedisk"));
  if (option & GO_ENABLELOCALSAVE)
    StringCchCat(msg, len, _T(",localsave"));
  if (option & GO_MERGEANNOTATE)
    StringCchCat(msg, len, _T(",mergeannotate"));

  if (option & GO_UPLOADAS_NEWVERSION)
    StringCchCat(msg, len, _T("uploadas_newversion_ifexist"));
  if (option & GO_CLEARFOLDER_BEFOREUPLOAD)
    StringCchCat(msg, len, _T("clear_folder_before_upload"));

}

void KVivaCommandInfo::Dump()
{
  // copy cmdInfo to Global setting
  TCHAR msg[256];

  StringCchCopy(msg, 256, _T("FilenameFormat : "));
  StringCchCat(msg, 256, gImageFilenameFormat);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  StringCchCopy(msg, 256, _T("DefaultSaveFormat : "));
  StringCchCat(msg, 256, gSaveFormatName[gSaveFormat]);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  StringCchCopy(msg, 256, _T("SystemOption : "));
  CatSystemOptionName(msg, 256, mSystemOption);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  if (m_nShellCommand == FileOpen)
  {
    StringCchCopy(msg, 256, _T("FileOpen : "));
    StringCchCat(msg, 256, m_strFileName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

}