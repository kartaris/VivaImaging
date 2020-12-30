// VivaImagingDoc.cpp: CVivaImagingDoc 클래스의 구현
// Copyright (C) 2019 K2Mobile
// All rights reserved.
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "VivaImaging.h"
#endif

#include "VivaImagingDoc.h"
#include "PropertiesWnd.h"
#include "MainFrm.h"
#include "Core/KConstant.h"
#include "Core/KSyncCoreEngine.h"
#include "Core/HttpDownloader/KMemory.h"
#include "Platform/crypting.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVivaImagingDoc

IMPLEMENT_DYNCREATE(CVivaImagingDoc, CDocument)

BEGIN_MESSAGE_MAP(CVivaImagingDoc, CDocument)
  ON_COMMAND(ID_FILE_CLOSE, &CVivaImagingDoc::OnFileClose)
END_MESSAGE_MAP()


// CVivaImagingDoc 생성/소멸

CVivaImagingDoc::CVivaImagingDoc()
{
  m_ActivePage = 0;
  mCreatorID[0] = '\0';
  mCreatorOID[0] = '\0';
  mCreatedDate[0] = '\0';
  mIsTempFolder = FALSE;
  mWorkingPath = NULL;
  mIsNewDocument = FALSE;
  mFileFormat = FORMAT_UNKNOWN;
  mServerUploadFolderOid[0] = '\0';
  mpDownloadedBodyDocName = NULL;

}

CVivaImagingDoc::~CVivaImagingDoc()
{
  clearDocument();
  if (mWorkingPath != NULL)
  {
    delete[] mWorkingPath;
    mWorkingPath = NULL;
  }
}

int GetCurrentDateTime(LPTSTR str, int buffLength)
{
  SYSTEMTIME systime;
  GetLocalTime(&systime);
  SystemTimeToTStringHM(str, buffLength, systime);
  return lstrlen(str);
}

LPTSTR GetCurrentUserID()
{
  return mUserName;
}

LPTSTR GetCurrentUserOID()
{
  return mUserOid;
}

BOOL IsValidImageFileExt(LPCTSTR e)
{
  return ((CompareStringICNC(e, _T("tif")) == 0) ||
    (CompareStringICNC(e, _T("tiff")) == 0) ||
    (CompareStringICNC(e, _T("png")) == 0) ||
    (CompareStringICNC(e, _T("bmp")) == 0) ||
    (CompareStringICNC(e, _T("jpeg")) == 0) ||
    (CompareStringICNC(e, _T("jpg")) == 0));
}

LPTSTR CVivaImagingDoc::GetCreatorUserID()
{
  if (lstrlen(mCreatorID) == 0)
    StringCchCopy(mCreatorID, MAX_USERNAME, GetCurrentUserID());
  return mCreatorID;
}

LPTSTR CVivaImagingDoc::GetCreatorUserOID()
{
  if (lstrlen(mCreatorOID) == 0)
    StringCchCopy(mCreatorOID, MAX_OID, GetCurrentUserOID());
  return mCreatorOID;
}

LPTSTR CVivaImagingDoc::GetCreatedDate()
{
  if (lstrlen(mCreatedDate) == 0)
    GetCurrentDateTime(mCreatedDate, MAX_DATETIME);
  return mCreatedDate;
}

void CVivaImagingDoc::SetModifiedFlag(BOOL bModified)
{
#ifdef _DEBUG
  TCHAR msg[4];
  StringCchPrintf(msg, 4, _T("%d"), bModified);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
  CDocument::SetModifiedFlag(bModified);
}

BOOL CVivaImagingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// SDI 문서는 이 문서를 다시 사용합니다.
  clearDocument();
  SetModifiedFlag(0);
  mIsNewDocument = TRUE;
  return readyDefaultProject();
}

BOOL readyTempFolderName(LPTSTR basePathName, LPTSTR path, int bufflen)
{
  TCHAR date[10];
  SYSTEMTIME tm;
  GetLocalTime(&tm);
  SystemTimeToTStringDateNum(date, 10, tm);

  StringCchCopy(path, bufflen, basePathName);
  int len = lstrlen(path);
  while (TRUE)
  {
    StringCchPrintf(&path[len], bufflen - len, _T("\\viva_%s_%.4x"), date, rand());
    if (!IsDirectoryExist(path))
      return TRUE;
  }
  return FALSE;
}

LPTSTR CVivaImagingDoc::GetWorkingBaseFolder()
{
  if (mGlobalOption & GO_TEMPFOLDERONSECUREDISK)
  {
    TCHAR drivename[8];

    if (gpCoreEngine->GetSecurityDisk(drivename))
    {
      StringCchCopy(mSecureWorkingBaseFolder, MAX_PATH, drivename);
      StringCchCat(mSecureWorkingBaseFolder, MAX_PATH, _T("\\Imaging"));

      if (!IsDirectoryExist(mSecureWorkingBaseFolder))
        CreateDirectory(mSecureWorkingBaseFolder);

      return mSecureWorkingBaseFolder;
    }
  }

  if (lstrlen(mDefaultWorkingBaseFolder) == 0)
  {
    LPTSTR pathName = NULL;
    SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathName);
    StringCchCopy(mDefaultWorkingBaseFolder, MAX_PATH, pathName);
    CoTaskMemFree(pathName);
  }
  return mDefaultWorkingBaseFolder;
}

void CVivaImagingDoc::SetWorkingBaseFolder(LPCTSTR path)
{
  if (lstrlen(path) > 0)
    StringCchCopy(mDefaultWorkingBaseFolder, MAX_PATH, path);
  else
    mDefaultWorkingBaseFolder[0] = '\0';
}

void CVivaImagingDoc::SetCurrentFormat(ImagingSaveFormat format)
{
  mFileFormat = format;
  if (format == ImagingSaveFormat::FORMAT_ORIGIN)
  {
    // 첫번째 이미지의 Original Extention 으로 바꿔치기
    KImageBase* p = GetImagePage(0, 0);
    if (p != NULL)
    {
      LPCTSTR imageFilename = p->GetName();
      LPCTSTR imageExt = FindFileExtension(imageFilename);

      if (imageExt != NULL)
      {
        CString currentPathname = GetPathName();
        LPCTSTR ext = FindFileExtension(currentPathname);
        if ((ext == NULL) || CompareStringICNC(ext, imageExt))
        {
          LPTSTR newFilename = ChangeFileExtension(currentPathname, imageExt);
          if (newFilename != NULL)
          {
            SetPathName(newFilename, FALSE);
            delete[] newFilename;
          }
        }
      }
    }
  }
}

void CVivaImagingDoc::SetServerUploadFolder(LPCTSTR folder_oid)
{
  if (folder_oid != NULL)
    StringCchCopy(mServerUploadFolderOid, MAX_OID, folder_oid);
  else
    mServerUploadFolderOid[0] = '\0';
}

LPCTSTR CVivaImagingDoc::GetServerUploadFolder()
{
  return mServerUploadFolderOid;
}

void CVivaImagingDoc::setWorkingPath(LPCTSTR path)
{
  if (mWorkingPath != NULL)
    delete[] mWorkingPath;
  mWorkingPath = AllocString(path);
}

BOOL CVivaImagingDoc::readyDefaultProject()
{
  LPTSTR pBaseFolder = GetWorkingBaseFolder();

  TCHAR path[MAX_PATH];
  if (!readyTempFolderName(pBaseFolder, path, MAX_PATH))
  {
    return FALSE;
  }
  mFileFormat = gSaveFormat;
  mIsTempFolder = TRUE;
  setWorkingPath(path);

  LPCTSTR name = StrRChr(mWorkingPath, NULL, '\\');
  if (name != NULL)
  {
    name++;
    int len = lstrlen(mWorkingPath) + lstrlen(name) + 10;
    LPTSTR pathName = new TCHAR[len];
    MakeFullPathName(pathName, len, mWorkingPath, name);

    LPCTSTR defExt = GetDocumentFileExt(mFileFormat);

    if (defExt != NULL)
    {
      StringCchCat(pathName, len, _T("."));
      StringCchCat(pathName, len, defExt);
    }
    SetPathName(pathName, FALSE);

    StoreLogHistory(_T(__FUNCTION__), pathName, SYNC_MSG_LOG);
    delete[] pathName;
  }

  return TRUE;
}

void CVivaImagingDoc::SetTitle(LPCTSTR lpszTitle)
{
  TCHAR title[MAX_PATH+16];
  StringCchCopy(title, MAX_PATH + 16, lpszTitle);

  DWORD dwAttrib = GetFileAttributes(m_strPathName);
  if (dwAttrib != INVALID_FILE_ATTRIBUTES)
  {
    if (dwAttrib & FILE_ATTRIBUTE_READONLY)
      StringCchCat(title, MAX_PATH + 16, _T("[ReadOnly]"));
  }
  CDocument::SetTitle(title);
}

ImagingSaveFormat CVivaImagingDoc::OnFileSave()
{
  if (mIsTempFolder && !IsDirectoryExist(mWorkingPath))
    CreateDirectory(mWorkingPath);

  // 새 파일은 임시 이름으로 일단 저장.
  /*
  BOOL r;
  DWORD dwAttrib = GetFileAttributes(m_strPathName);
  if ((m_strPathName.GetLength() == 0) ||
    (IsFileExist(m_strPathName) && (dwAttrib & FILE_ATTRIBUTE_READONLY)))
    r = DoSave(NULL);
  else
    r = DoSave(m_strPathName);

  if (r)
  */

  if (mFileFormat > FORMAT_ORIGIN)
  {
    LPCTSTR ext = GetImageFileExt(mFileFormat);
    LPTSTR pathName = ChangeFileExtension(m_strPathName, ext);
    m_strPathName = pathName;
  }
  else if (mIsNewDocument)
  {
    // update file extension
    SetCurrentFormat(mFileFormat);
  }

  HRESULT hr = K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (hr != S_OK)
    OutputDebugString(_T("CoInitializeEx failed\n"));

  BOOL saved;
  if (mIsNewDocument)
    saved = DoFileSave(); // 
  else
    saved = DoSave(m_strPathName, TRUE);

  K_CoUninitialize();

  if (saved)
  {
    SetModifiedFlag(0);
    return mFileFormat;
  }
  return FORMAT_UNKNOWN;
}

ImagingSaveFormat CVivaImagingDoc::OnFileSaveAs(LPCTSTR name)
{
  LPTSTR fullPathName = AllocPathName(mWorkingPath, name);
  SetPathName(fullPathName, FALSE);

  if (mIsTempFolder && !IsDirectoryExist(mWorkingPath))
    CreateDirectory(mWorkingPath);

  if (OnSaveDocument(fullPathName))
  {
    SetModifiedFlag(0);
    return mFileFormat;
  }
  return FORMAT_UNKNOWN;
}

void CVivaImagingDoc::OnFileClose()
{
  if (!SaveModified())
    return;

  if (mIsTempFolder)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Need to clear temp folder"), SYNC_EVENT_ERROR | SYNC_MSG_LOG);
  }

  // shut it down
  OnCloseDocument();
  // this should destroy the document
}

BOOL CVivaImagingDoc::ReadyWorkingPath()
{
  if (mIsTempFolder && !IsDirectoryExist(mWorkingPath))
    CreateDirectory(mWorkingPath);
  return TRUE;
}

BOOL CVivaImagingDoc::ReadyFileSave(LPCTSTR lpszPathName)
{
  // check if workingFolder changed
  if (!IsChildFolderOrFile(mWorkingPath, lpszPathName, FALSE))
  {
    SplitPathNameAlloc(lpszPathName, &mWorkingPath, NULL);
  }

  if ((mFileFormat != TIFF_MULTI_PAGE) && (mFileFormat != TIF_MULTI_PAGE))
  {
    // XML 저장시에 이미지 파일을 작업 폴더로 이동한다.
    m_Pages.MoveToWorkingFolder(mWorkingPath);
  }
  return TRUE;
}

HRESULT CVivaImagingDoc::ReadAttributes(IXmlReader* pReader)
{
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;

  HRESULT hr = pReader->MoveToFirstAttribute();
  while (hr != S_FALSE)
  {
    if (!pReader->IsDefault())
    {
      UINT cwchPrefix;
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      // pwszLocalName : Creator, LastModified, 
      if (CompareStringNC(pwszLocalName, tagCreator) == 0)
      {
        if (pwszValue != NULL)
          StringCchCopy(mCreatorID, 64, pwszValue);
        else
          mCreatorID[0] = '\0';
      }
      else if (CompareStringNC(pwszLocalName, tagCreatorOID) == 0)
      {
        if (pwszValue != NULL)
          StringCchCopy(mCreatorOID, 12, pwszValue);
        else
          mCreatorOID[0] = '\0';
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

BOOL CVivaImagingDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
  BOOL rtn = FALSE;
  int format = 0;
  CString errmsg;

  clearDocument();

  if (IsDirectoryExist(lpszPathName))
  {
    format = 3;
  }
  else
  {
    LPCTSTR e = StrRChr(lpszPathName, NULL, '.');
    if (e != NULL)
    {
      e++;

      /*
      if (CompareStringICNC(e, _T("xml")) == 0)
      {
        format = 1;
      }
      else 
      */
      if (IsValidImageFileExt(e))
      {
        format = 2;
      }
    }
  }

  LPTSTR kenc_temp_file = (LPTSTR)lpszPathName;

  /*
  if (format == 1) // xml
  {
    rtn = OnOpenXmlDocument(lpszPathName, errmsg);
    if (!rtn)
    {
      kenc_temp_file = AllocAddString(lpszPathName, '.', _T("tmp"));
      if (KDecryptFile(lpszPathName, kenc_temp_file))
      {
        rtn = OnOpenXmlDocument(kenc_temp_file, errmsg);
        if (rtn)
          StoreLogHistory(_T(__FUNCTION__), _T("CryptedFileReaded"), SYNC_MSG_DEBUG | SYNC_MSG_LOG);
      }
    }
    if (rtn)
    {
      mFileFormat = XML_WITH_JPG;
      SplitPathNameAlloc(lpszPathName, &mWorkingPath, NULL);
    }
    if (rtn == 0)
      format = 2;
  }
  */

  StoreLogHistory(_T(__FUNCTION__), lpszPathName, SYNC_MSG_LOG);

  if (format == 2)
  {
    // TODO: open on default folder? or open on target folder?
    //readyDefaultProject();
    SplitPathNameAlloc(lpszPathName, &mWorkingPath, NULL);

    KImageDocVector pageList;
    rtn = InsertImageFile(lpszPathName, &pageList, FALSE, errmsg);
    if (!rtn)
    {
      // try with decryption
      kenc_temp_file = AllocAddString(lpszPathName, '.', _T("tmp"));
      if (KDecryptFile(lpszPathName, kenc_temp_file))
      {
        rtn = InsertImageFile(kenc_temp_file, &pageList, FALSE, errmsg);
      }
    }
    if (rtn)
    {
      // set current file format
      LPCTSTR ext = StrRChr(lpszPathName, NULL, '.');
      if (ext != NULL)
      {
        ext++;
        if (CompareStringICNC(ext, _T("TIF")) == 0)
          mFileFormat = TIF_MULTI_PAGE;
        else if (CompareStringICNC(ext, _T("TIFF")) == 0)
          mFileFormat = TIFF_MULTI_PAGE;
        else if (CompareStringICNC(ext, _T("PNG")) == 0)
          mFileFormat = FORMAT_PNG;
        else if (CompareStringICNC(ext, _T("JPG")) == 0)
          mFileFormat = FORMAT_JPG;
        else //if (CompareStringICNC(ext, _T("JEPG")) == 0)
          mFileFormat = FORMAT_JPEG;
      }
    }
  }

  if (kenc_temp_file != lpszPathName)
  {
    KDeleteFile(kenc_temp_file);
    delete[] kenc_temp_file;
  }

  if (rtn)
  {
    SetActivePageIndex(0);
    if(m_ActivePage != NULL)
      m_ActivePage->SetSelected(TRUE);
    StoreLogHistory(_T(__FUNCTION__), lpszPathName, SYNC_MSG_LOG);
  }
  else
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_FILE_OPEN_FAILURE, mResourceLanguage);
    str += _T("\n");
    if (errmsg.GetLength() > 0)
    {
      str += errmsg + _T("\n");
    }
    str += lpszPathName;
    StoreLogHistory(_T(__FUNCTION__), str, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return TRUE;
  }
  return rtn;
}

BOOL CVivaImagingDoc::OpenDocumentFolder(LPCTSTR path)
{
  return FALSE;
}

#ifdef USE_XML_STORAGE
HRESULT CVivaImagingDoc::ReadCheckAttributes(IXmlReader* pReader)
{
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;

  HRESULT hr = pReader->MoveToFirstAttribute();
  while (hr != S_FALSE)
  {
    if (!pReader->IsDefault())
    {
      UINT cwchPrefix;
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      // pwszLocalName : Creator, LastModified, 
      /*
      if (CompareStringNC(pwszLocalName, tagCreator) == 0)
      {
        StringCchCopy(mCreatorID, 64, pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagCreatorOID) == 0)
      {
        StringCchCopy(mCreatorOID, 12, pwszValue);
      }
      */
    }

    hr = pReader->MoveToNextAttribute();
  }
  return hr;
}

BOOL CVivaImagingDoc::IsImagingXMLDocFile(LPCTSTR lpszPathName)
{
  LPCTSTR ext = FindFileExtension(lpszPathName);
  if ((ext != NULL) && (CompareStringICNC(ext, _T("xml")) == 0))
  {
    CComPtr<IStream> pInFileStream;
    CComPtr<IXmlReader> pReader;
    HRESULT hr;

    if (FAILED(hr = SHCreateStreamOnFileEx(lpszPathName, STGM_FAILIFTHERE, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &pInFileStream)))
    {
      return FALSE;
    }
    if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL)))
    {
      return FALSE;
    }

    if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))) {
      return FALSE;
    }

    if (FAILED(hr = pReader->SetInput(pInFileStream)))
    {
      return FALSE;
    }

    XmlNodeType nodeType;
    const WCHAR* pwszPrefix;
    const WCHAR* pwszLocalName;
    const WCHAR* pwszValue;
    UINT cwchPrefix;

    while (S_OK == (hr = pReader->Read(&nodeType))) {
      switch (nodeType) {

      case XmlNodeType_XmlDeclaration:
        if (FAILED(hr = ReadCheckAttributes(pReader))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error writing attributes, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
        break;

      case XmlNodeType_Element:
        if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
        if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }

        if (CompareStringICNC(pwszLocalName, vivaXmlDocName) == 0)
        {
          return TRUE;
        }

        break;

      case XmlNodeType_EndElement:
        if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
        if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
        break;

      case XmlNodeType_Text:

      case XmlNodeType_Whitespace:
        if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error getting value on Text, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
        //wprintf(L"Whitespace text: >%s<\n", pwszValue);
        break;

      case XmlNodeType_Comment:
        if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error getting value on Comment, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
        //wprintf(L"Comment: %s\n", pwszValue);
        break;

      case XmlNodeType_DocumentType:
        break;
      } // end of switch
    } // end of while
  }
  return FALSE;
}


BOOL CVivaImagingDoc::OnOpenXmlDocument(LPCTSTR lpszPathName, CString& errmsg)
{
  CComPtr<IStream> pInFileStream;
  CComPtr<IXmlReader> pReader;
  HRESULT hr;

  if (FAILED(hr = SHCreateStreamOnFileEx(lpszPathName, STGM_FAILIFTHERE, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &pInFileStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error opening file reader, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating xml reader, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))) {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting XmlReaderProperty_DtdProcessing, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pReader->SetInput(pInFileStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting input for reader, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  XmlNodeType nodeType;
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;
  UINT cwchPrefix;
  BOOL readed = FALSE;

  while (S_OK == (hr = pReader->Read(&nodeType))) {
    switch (nodeType) {

    case XmlNodeType_XmlDeclaration:
      if (FAILED(hr = ReadAttributes(pReader))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error writing attributes, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      break;

    case XmlNodeType_Element:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }

      if (CompareStringNC(pwszLocalName, tagFolder) == 0)
      {
        hr = m_Pages.ReadDocument(pReader, NULL, 0);
        if (FAILED(hr))
          return FALSE;
      }
      else if (CompareStringNC(pwszLocalName, tagExtDocType) == 0)
      {
        hr = mDocTypeInfo.ReadDocument(pReader, 0);
        if (FAILED(hr))
          return FALSE;
      }
      else
      {
        if (FAILED(hr = ReadAttributes(pReader))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error reading attributes, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
      }

      if (FAILED(hr = pReader->MoveToElement())) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      readed = TRUE;
      break;

    case XmlNodeType_EndElement:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      break;

    case XmlNodeType_Text:

    case XmlNodeType_Whitespace:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Text, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"Whitespace text: >%s<\n", pwszValue);
      break;

    case XmlNodeType_CDATA:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on CDATA, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"CDATA: %s\n", pwszValue);
      break;

    case XmlNodeType_ProcessingInstruction:
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting name on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
      break;

    case XmlNodeType_Comment:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Comment, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"Comment: %s\n", pwszValue);
      break;

    case XmlNodeType_DocumentType:
      break;
    } // end of switch
  } // end of while
  return readed;
}
#endif // USE_XML_STORAGE

LPTSTR readyTempBackupFilename(LPCTSTR lpszPathName)
{
  int len = lstrlen(lpszPathName) + 12;
  LPTSTR name = new TCHAR[len];
  if (name == NULL)
    return name;

  LPCTSTR e = StrRChr(lpszPathName, NULL, '\\');

#if 1
  if (e != NULL)
  {
    e++;
    StringCchCopyN(name, len, lpszPathName, (size_t)(e - lpszPathName));
  }
  else
  {
    name[0] = '\0';
    e = lpszPathName;
  }
  StringCchCat(name, len, _T("~$"));
  StringCchCat(name, len, e);

#else

  LPTSTR ext = StrRChr(lpszPathName, NULL, '.');
  if (ext != NULL)
    ext++;

  if (e != NULL)
  {
    int c = 0;
    LPTSTR n = StrStr(e, _T("-bk"));
    if (n != NULL)
    {
      n += 3;
      StringCchCopyN(name, len, lpszPathName, (size_t)(n - lpszPathName));

      wscanf_s(n, _T("%d"), c);
      c++;

      TCHAR index[10];
      StringCchPrintf(index, 10, _T("%d"), c);
      StringCchCat(name, len, index);
    }
    else
    {
      n = StrRChr(e, NULL, '.');
      if (n != NULL)
      {
        StringCchCopyN(name, len, lpszPathName, (size_t)(n - lpszPathName));
      }
      StringCchCat(name, len, _T("-bk1"));
    }
  }
  if (ext != NULL)
    StringCchCat(name, len, ext);
#endif
  return name;
}

BOOL CVivaImagingDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
  // check extension
  /*
  LPCTSTR ext = StrRChr(lpszPathName, NULL, '.');
  if (ext != NULL)
  {
    ext++;
    if (CompareStringICNC(ext, _T("tiff")) == 0)
      mFileFormat = TIFF_MULTI_PAGE;
    else if (CompareStringICNC(ext, _T("tif")) == 0)
      mFileFormat = TIF_MULTI_PAGE;
  }
  */

  ReadyFileSave(lpszPathName);

  //ECM 디스크에서는 백업본 만들지 않는다.

  // backup if file already exist
  if (IsFileExist(lpszPathName))
  {
    LPTSTR tempFile = readyTempBackupFilename(lpszPathName);
    if (tempFile != NULL)
    {
      if (IsFileExist(tempFile))
        DeleteFile(tempFile);
      KMoveFile(lpszPathName, tempFile);
      delete[] tempFile;
    }
  }

  if (mGlobalOption & GO_MERGEANNOTATE)
  {
    TCHAR msg[128];
    int p = m_Pages.MergeAnnotateObjects();
    StringCchPrintf(msg, 128, _T("%d images are merged with annotate"), p);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  // encoded save
  LPTSTR firstFileName = (LPTSTR)lpszPathName;
  if (mGlobalOption & GO_USECRYPTION)
    firstFileName = AllocAddString(lpszPathName, '_', _T("tmp"));

  CString errmsg;
  BOOL r;

  if ((mFileFormat == TIFF_MULTI_PAGE) || (mFileFormat == TIF_MULTI_PAGE))
  {
    r = OnSaveAsTiff(firstFileName, errmsg);
  }
  else
  {
    LPCTSTR firstFilenameOnly = StrRChr(firstFileName, NULL, '\\');
    if (firstFilenameOnly == NULL)
      firstFilenameOnly = firstFileName;
    else
      firstFilenameOnly++;

    KEcmDocTypeInfo* docTypeInfo = &mDocTypeInfo;

    m_Pages.StoreImageFile(mWorkingPath, docTypeInfo, firstFilenameOnly, FALSE);
    r = TRUE;

    // do not save xml
    // r = OnSaveXML(saveFileName, errmsg);
  }

  if (!r)
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_FILE_SAVE_FAILURE, mResourceLanguage);
    str += _T("\n");
    if (errmsg.GetLength() > 0)
    {
      str += errmsg + _T("\n");
    }
    str += lpszPathName;
    AfxMessageBox(str);
  }
  else
  {
    if (mGlobalOption & GO_USECRYPTION)
    {
      KEncryptFile(firstFileName, lpszPathName);
#ifndef _DEBUG
      KDeleteFile(firstFileName);
#endif
      delete[] firstFileName;
    }
    mIsTempFolder = FALSE;
    mIsNewDocument = FALSE;
    m_Pages.ClearTempFiles(mWorkingPath);
  }
  return r;
}

void CVivaImagingDoc::OnCloseDocument()
{
  if (mIsTempFolder)
  {
    if ((mWorkingPath != NULL) && IsDirectoryExist(mWorkingPath))
    {
      TCHAR msg[256] = _T("Need to clear temp folder : ");
      StringCchCat(msg, 256, mWorkingPath);

      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      EmptyDirectory(mWorkingPath, TRUE);
    }
  }
  else
  {
    m_Pages.ClearTempFiles(mWorkingPath);
  }

  CDocument::OnCloseDocument();
}

#ifdef USE_XML_STORAGE
BOOL CVivaImagingDoc::OnSaveXML(LPCTSTR lpszPathName, CString& errmsg)
{
  CComPtr<IStream> pOutFileStream;
  CComPtr<IXmlWriter> pWriter;
  HRESULT hr;

  if (FAILED(hr = SHCreateStreamOnFile(lpszPathName, STGM_CREATE | STGM_WRITE, &pOutFileStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating file writer, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating xml writer, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->SetOutput(pOutFileStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting output for writer, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartDocument, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartElement(NULL, vivaXmlDocName, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagDocInformation, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagCreator, NULL,
    GetCreatorUserID())))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagCreatorOID, NULL,
    GetCreatorUserOID())))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagCreatedDate, NULL,
    GetCreatedDate())))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagLastModifier, NULL,
    GetCurrentUserID())))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagLastModifierOID, NULL,
    GetCurrentUserOID())))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  TCHAR str[64];
  GetCurrentDateTime(str, 64);
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagLastModifiedDate, NULL,
    str)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteEndElement())) // end information
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  // store DocType info
  hr = mDocTypeInfo.StoreDocument(pWriter, FALSE);
  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, on StoreDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  // cleare invalid MetaDatas
  m_Pages.ValidateMetadatas(mDocTypeInfo);

  // store pages
  hr = m_Pages.StoreDocument(pWriter, FALSE);

  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, on StoreDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteEndDocument()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->Flush()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: Flush, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  return TRUE;
}

LPSTR CVivaImagingDoc::StoreDocTypeInfoToString()
{
    //LPSTREAM pStream;
    CComPtr<IStream> spStream;
    CComPtr<IXmlWriter> pWriter;
    HRESULT hr;
    CString errmsg;

    HGLOBAL hg = GlobalAlloc(GMEM_FIXED, 4096);
    if (FAILED(hr = CreateStreamOnHGlobal(hg, FALSE, &spStream)))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error creating file writer, error is %08.8lx"), hr);
      errmsg.SetString(msg);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }
    if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL)))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error creating xml writer, error is %08.8lx"), hr);
      errmsg.SetString(msg);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }

    if (FAILED(hr = pWriter->SetOutput(spStream)))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error setting output for writer, error is %08.8lx"), hr);
      errmsg.SetString(msg);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }

    if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteStartDocument, error is %08.8lx"), hr);
      errmsg.SetString(msg);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }

    hr = mDocTypeInfo.StoreDocument(pWriter, FALSE);
    if (FAILED(hr))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, on StoreDocument, error is %08.8lx"), hr);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }
    if (FAILED(hr = pWriter->WriteEndDocument()))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: WriteEndDocument, error is %08.8lx"), hr);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }
    if (FAILED(hr = pWriter->Flush()))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Error, Method: Flush, error is %08.8lx"), hr);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }

    BYTE* p = NULL;
    ULARGE_INTEGER streamSize;
    LARGE_INTEGER zero;
    zero.QuadPart = 0;
    spStream->Seek(zero, STREAM_SEEK_CUR, &streamSize);
    UINT nSize = (UINT)(streamSize.LowPart) + 2;
    if (nSize > 0)
    {
      p = new BYTE[nSize];
      memset(p, 0, nSize);
      spStream->Seek(zero, STREAM_SEEK_SET, nullptr);
      spStream->Read(p, nSize, nullptr);
    }

    return (LPSTR)p;
}
#endif // USE_XML_STORAGE

HANDLE CVivaImagingDoc::CopyToEditActionBackup(BOOL applyAll)
{
  KImageDocVector pages;
  if (applyAll)
    m_Pages.GetPageList(pages, IMAGE_ONLY);
  else
    m_Pages.GetPageList(pages, SELECTED_ONLY | IMAGE_ONLY);
    // pages.push_back(m_ActivePage);

  int count = pages.size();
  HANDLE bh = KMemoryAlloc(sizeof(EditActionBackupData) * count);
  EditActionBackupData* pBackup = (EditActionBackupData*)KMemoryLock(bh);

  KImageDocIterator it = pages.begin();
  while (it != pages.end())
  {
    KImageBase* p = *(it++);
    pBackup->targetPageId = p->GetID();

    // not preview but original
    pBackup->image = ((KImagePage*)p)->GetImage(GetWorkingPath(), NULL, TRUE)->GetDIB();
    pBackup++;
  }
  KMemoryUnlock(bh);
  return bh;
}

HANDLE CVivaImagingDoc::CopyToClipboard(BOOL allSelected)
{
  HANDLE h = NULL;
#ifdef USE_XML_STORAGE
  CComPtr<IStream> pOutStream;
  CComPtr<IXmlWriter> pWriter;
  HRESULT hr;

  hr = CreateStreamOnHGlobal(NULL, FALSE, &pOutStream);
  if (FAILED(hr))
  {
    TCHAR msg[256] = _T("Error creating MemStream for writing");
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating xml writer, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->SetOutput(pOutStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting output for writer, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartElement(NULL, vivaXmlDocName, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (allSelected)
  {
    hr = m_Pages.StoreDocument(pWriter, TRUE);
  }
  else
  {
    // store pages
    hr = m_ActivePage->StoreDocument(pWriter, TRUE);
  }

  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, on StoreDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteEndDocument()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->Flush()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: Flush, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  hr = GetHGlobalFromStream(pOutStream, &h);
  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, GetHGlobalFromStream, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
  }
#else
  KMemoryStream ms(NULL, 0, ReadWrite);
  int s = 0;

  if (allSelected)
  {
    s = m_Pages.StoreDocument(ms, TRUE);
  }
  else
  {
    // store pages
    s = m_ActivePage->StoreDocument(ms, TRUE);
  }

  if (s > 0)
  {
    h = GlobalAlloc(0, ms.Size());
    if (h != NULL)
    {
      void* p = GlobalLock(h);
      memcpy(p, ms.Data(), ms.Size());
      GlobalUnlock(h);
    }
  }
#endif // USE_XML_STORAGE
  return h;
}

BOOL CVivaImagingDoc::LoadDocTypeMetaData(HANDLE h)
{
  BOOL rtn = FALSE;
  LPBYTE p = (LPBYTE)GlobalLock(h);
  if (p != NULL)
  {
    DWORD size = GlobalSize(h);
    KMemoryStream ms(p, (int)size, ReadOnly);

    DWORD tag = ms.ReadDword();
    if (tag == KMS_DOCTYPE_GROUP)
    {
      mDocTypeInfo.Clear();
      mDocTypeInfo.LoadFromBuffer(ms);

      tag = ms.ReadDword();
      if (tag == KMS_FOLDER_ITEM)
        m_Pages.LoadMetaData(ms);
      rtn = TRUE;
    }

    GlobalUnlock(h);
  }
  return rtn;
}

HANDLE CVivaImagingDoc::SaveDocTypeMetaData()
{
  KMemoryStream ms;

  mDocTypeInfo.StoreToBuffer(ms);

  // CHECK : 각 이미지 페이지 ExtAttribute clear
  m_Pages.SaveMetaData(ms);

  HANDLE h = GlobalAlloc(0, ms.Size());
  if (h != NULL)
  {
    void* p = GlobalLock(h);
    if (p != NULL)
      memcpy(p, ms.Data(), ms.Size());
    GlobalUnlock(h);
  }
  return h;
}

UINT CVivaImagingDoc::GetImageFrameCount(LPCTSTR lpszPathName)
{
  IWICImagingFactory* pImageFactory = 0;
  UINT uiFrameCount = 0;

  K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {
    IWICBitmapDecoder *pDecoder = NULL;
    IWICStream *pStream = NULL;

    if (SUCCEEDED(pImageFactory->CreateStream(&pStream)))
      pStream->InitializeFromFilename(lpszPathName, GENERIC_READ);

    if ((pStream != NULL) &&
      SUCCEEDED(pImageFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)))
    {
      KImageBase* folder = NULL;

      pDecoder->GetFrameCount(&uiFrameCount);
      pDecoder->Release();
    }

    if (pStream != NULL)
      pStream->Release();
    pImageFactory->Release();
  }
  K_CoUninitialize();
  return uiFrameCount;
}

BOOL CVivaImagingDoc::InsertImageFile(LPCTSTR lpszPathName, KImageDocVector* pPageList, BOOL insertAsSubFolder, CString& errmsg, LPCTSTR folderName)
{
  IWICImagingFactory* pImageFactory = 0;
  int pages = 0;

  K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {
    IWICBitmapDecoder *pDecoder = NULL;
    IWICStream *pStream = NULL;

#if 0
    // in this case, it has a bug(it doesnot release file, so we cannot overwrite this file)
    if (SUCCEEDED(pImageFactory->CreateStream(&pStream)))
    {
      pStream->InitializeFromFilename(lpszPathName, GENERIC_READ);
    }
#else
    SHCreateStreamOnFileEx(lpszPathName, STGM_READ | STGM_SHARE_DENY_WRITE,
      FILE_ATTRIBUTE_NORMAL, FALSE, NULL, (IStream**)&pStream);
#endif

    if ((pStream != NULL) &&
      SUCCEEDED(pImageFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)))
    {
      UINT uiFrameCount = 0;
      KImageBase* folder = NULL;

      if (SUCCEEDED(pDecoder->GetFrameCount(&uiFrameCount)))
      {
        // if multiple page, insert it as subfolder

        if ((uiFrameCount > 1) && insertAsSubFolder)
        {
          LPTSTR name = NULL;
          SplitPathNameAlloc(lpszPathName, NULL, &name);
          if (name != NULL)
            TrimFileExtension(name);
          AddNewFolder(name, 0, NULL);
          folder = m_ActivePage;
          if (pPageList != NULL)
            pPageList->push_back(folder);
        }
        else
        {
          if (folderName != NULL)
          {
            folder = m_Pages.FindPageByName(folderName, FOLDER_PAGE);
          }

          if (folder == NULL)
          {
            folder = &m_Pages;
            if (m_ActivePage != NULL)
            {
              if (m_ActivePage->GetType() == FOLDER_PAGE)
                folder = m_ActivePage;
              else
                folder = m_ActivePage->GetParent();
            }
          }
        }

        KEcmDocTypeInfo* docTypeInfo = &mDocTypeInfo;

        for (UINT i = 0; i < uiFrameCount; i++)
        {
          IWICBitmapFrameDecode *pIBitmapFrame;

          if (SUCCEEDED(pDecoder->GetFrame(i, &pIBitmapFrame)))
          {
            KImagePage* page = new KImagePage(folder, lpszPathName);

            if (page->LoadImageFromFrameDecoder(pImageFactory, pIBitmapFrame, docTypeInfo))
            {
              page->SetMultiPageIndex(i);

#ifdef _LAST_TREE_BUILD_METHOD
              // check folder list
              if (i == 0)
              {
                LPTSTR folderList = page->GetFolderNameList();
                if (folderList != NULL)
                  m_Pages.ReadyFolderPages(folderList);
              }

              insertPage(page, folder);
#else
              KImageBase* pt = NULL;
              if (page->mpParentKeyName != NULL)
                pt = ((KImageFolder*)folder)->GetPageFromParentKeyName(page->mpParentKeyName, TRUE);
              else
                pt = folder;

              insertPage(page, pt);
#endif

              if (pPageList != NULL)
                pPageList->push_back(page);
              pages++;
            }
            docTypeInfo = NULL;
            pIBitmapFrame->Release();
          }
        }
      }
      pDecoder->Release();

#ifdef _LAST_TREE_BUILD_METHOD
      if (!insertAsSubFolder && (pPageList != NULL))
      {
        m_Pages.CheckReadyFolderPage(pPageList);
      }
#endif
    }
    else
    {
      TCHAR msg[256];
      FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        msg, 256, NULL);
      errmsg.SetString(msg);
      //errmsg.SetString(_T("Unknown image decoder"));
    }

    if (pStream != NULL)
      pStream->Release();
    pImageFactory->Release();
  }
  else
  {
    errmsg.SetString(_T("Unknown image factory"));
  }
  K_CoUninitialize();
  return(pages > 0);
}

/*
void CVivaImagingDoc::checkReadyFolderPage(KImageDocVector* pPageList, KImageDocVector& newPageList)
{
  KImageDocIterator it = pPageList->begin();
  while (it != pPageList->end())
  {
    KImageBase* page = *it++;

    if (page->mpParentKeyName != NULL)
    {
      KImageFolder* p = m_Pages.GetPageFromParentKeyName(page->mpParentKeyName);
      if ((p != NULL) && (p != page->GetParent()))
      {
        if (p->GetChildCount() == 0)
          newPageList.push_back(p);

        m_Pages.RemovePage(page);
        p->InsertPage(page, NULL);

        page = NULL;
      }
    }

    if (page != NULL)
    {
      newPageList.push_back(page);
    }
  }
}
*/

int CVivaImagingDoc::InsertPageData(HANDLE b, int flag)
{
  int r = 0;
  KImageDocVector newPages;
  KVoidPtrArray shapeList;
  HANDLE ann_backup = NULL;
  HRESULT hr;

#ifdef USE_XML_STORAGE
  CComPtr<IStream> pInStream;
  CComPtr<IXmlReader> pReader;

  /*
  SIZE_T size = GlobalSize(b);
  void* p = GlobalLock(b);
  GlobalUnlock(b);
  */
  if (FAILED(hr = CreateStreamOnHGlobal(b, FALSE, &pInStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error opening HGlobal Stream, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating xml reader, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))) {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting XmlReaderProperty_DtdProcessing, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }


  if (FAILED(hr = pReader->SetInput(pInStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting input for reader, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  XmlNodeType nodeType;
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;
  UINT cwchPrefix;

  while (S_OK == (hr = pReader->Read(&nodeType))) {
    switch (nodeType) {

    case XmlNodeType_XmlDeclaration:
      if (FAILED(hr = ReadAttributes(pReader))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error writing attributes, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      break;

    case XmlNodeType_Element:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (CompareStringNC(pwszLocalName, tagFolder) == 0)
      {
        r = ADD_PAGES;
        hr = m_Pages.ReadDocument(pReader, &newPages, flag);
        if (FAILED(hr))
          return FALSE;
      }
      else if (CompareStringNC(pwszLocalName, tagAnnotates) == 0)
      {
        if (r == 0)
        {
          r = ADD_SHAPES;
          ann_backup = ((KImagePage*)m_ActivePage)->SaveAnnotateData();
        }
        if ((m_ActivePage != NULL) && (m_ActivePage->GetType() == IMAGE_PAGE))
        {
          KVoidPtrArray* pList = NULL;

          if ((r == ADD_SHAPES) && (flag & SELECT_INSERTED_OBJECTS))
            pList = &shapeList;

          hr = ((KImagePage*)m_ActivePage)->ReadDocument(pReader, pList, flag);
          if (FAILED(hr))
            return FALSE;

          m_ActivePage->SetModified(FALSE, ALL_MODIFIED);
          if (flag & SELECT_INSERTED_OBJECTS)
            ((KImagePage*)m_ActivePage)->SetShapeSelected(shapeList);
        }
        else
        {
          StoreLogHistory(_T(__FUNCTION__), _T("no Active page or not an image page cannot paste Annotates"), SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
      }
      else 
      {
        if (FAILED(hr = ReadAttributes(pReader))) {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error reading attributes, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return FALSE;
        }
      }

      if (FAILED(hr = pReader->MoveToElement())) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      break;

    case XmlNodeType_EndElement:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      break;

    case XmlNodeType_Text:

    case XmlNodeType_Whitespace:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Text, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"Whitespace text: >%s<\n", pwszValue);
      break;

    case XmlNodeType_CDATA:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on CDATA, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"CDATA: %s\n", pwszValue);
      break;

    case XmlNodeType_ProcessingInstruction:
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting name on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
      break;

    case XmlNodeType_Comment:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Comment, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return FALSE;
      }
      //wprintf(L"Comment: %s\n", pwszValue);
      break;

    case XmlNodeType_DocumentType:
      //wprintf(L"DOCTYPE is not printed\n");
      break;
    } // end of switch
  } // end of while

  // set active page as last one
  if ((newPages.size() > 0) && (flag & SELECT_INSERTED_OBJECTS))
  {
    SelectAllPages(FALSE);
    KImageBase* page = NULL;
    KImageDocIterator it = newPages.begin();
    while (it != newPages.end())
    {
      page = *it++;
      page->SetSelected(TRUE);
    }
    SetActivePage(page);
  }

  if ((r == ADD_SHAPES) && (shapeList.size() > 0))
  {
    // store paste action
    HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
    EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
    ed->annotate = ann_backup;
    ed->targetPageId = m_ActivePage->GetID();
    KMemoryUnlock(h);

    mActionManager.SaveAction(EA_PasteAnnotates, NULL, h);
    ann_backup = NULL;
  }
  if (ann_backup != NULL)
    KMemoryFree(ann_backup);
#else
  DWORD size = GlobalSize(b);
  void* p = GlobalLock(b);
  KMemoryStream ms((LPBYTE)p, (int)size, ReadOnly);
  DWORD tag = ms.ReadDword();

  if (tag == KMS_FOLDER_ITEM)
  {
    // read images
    r = ADD_PAGES;
    hr = m_Pages.ReadDocument(ms, &newPages, flag);
    if (FAILED(hr))
      return FALSE;
  }
  else if (tag == KMS_ANNOTATE_GROUP)
  {
    // read shapes
    r = ADD_SHAPES;

    if ((m_ActivePage != NULL) && (m_ActivePage->GetType() == IMAGE_PAGE))
    {
      KVoidPtrArray* pList = NULL;

      ann_backup = ((KImagePage*)m_ActivePage)->SaveAnnotateData();

      if ((r == ADD_SHAPES) && (flag & SELECT_INSERTED_OBJECTS))
        pList = &shapeList;

      hr = ((KImagePage*)m_ActivePage)->LoadAnnotates(ms, pList, flag);
      if (FAILED(hr))
      {
        if (ann_backup != NULL)
          KMemoryFree(ann_backup);
        return FALSE;
      }
      m_ActivePage->SetModified(FALSE, PageModifiedFlag::ALL_MODIFIED);
      if (flag & SELECT_INSERTED_OBJECTS)
        ((KImagePage*)m_ActivePage)->SetShapeSelected(shapeList);
    }
    else
    {
      StoreLogHistory(_T(__FUNCTION__), _T("no Active page or not an image page cannot paste Annotates"), SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return FALSE;
    }
  }
  GlobalUnlock(b);

  // NEED_TODO
  if ((newPages.size() > 0) && (flag & SELECT_INSERTED_OBJECTS))
  {
    SelectAllPages(FALSE);
    KImageBase* page = NULL;
    KImageDocIterator it = newPages.begin();
    while (it != newPages.end())
    {
      page = *it++;
      page->SetSelected(TRUE);
    }
    SetActivePage(page);
  }

  if ((r == ADD_SHAPES) && (shapeList.size() > 0))
  {
    // store paste action
    HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
    EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
    ed->annotate = ann_backup;
    ed->targetPageId = m_ActivePage->GetID();
    KMemoryUnlock(h);

    mActionManager.SaveAction(EA_PasteAnnotates, NULL, h);
    ann_backup = NULL;
  }
  if (ann_backup != NULL)
    KMemoryFree(ann_backup);

#endif // USE_XML_STORAGE
  return r;
}

LPTSTR readyFolderFilename(LPCTSTR baseFilename, LPCTSTR folderName)
{
  LPTSTR pathName = NULL;
  LPCTSTR e = StrRChr(baseFilename, NULL, '\\');
  if (e != NULL)
  {
    int len = (e - baseFilename) + lstrlen(folderName) + 10;
    pathName = new TCHAR[len];
    MakeFullPathName(pathName, len, baseFilename, folderName);
    StringCchCat(pathName, len, _T(".tiff"));
  }
  return pathName;
}

BOOL CVivaImagingDoc::OnSaveAsTiff(LPCTSTR lpszPathName, CString& errmsg)
{
  // main folder
  if (m_Pages.GetNumberOfPages(IMAGE_ONLY) > 0)
  {
    KImageDocVector imagePages;

    m_Pages.GetPageList(imagePages, IMAGE_ONLY);
    if (imagePages.size() > 0)
    {
      return SaveImageAsTiff(lpszPathName, imagePages, errmsg);
    }
  }
  return FALSE;
}

BOOL CVivaImagingDoc::SaveImageAsTiff(LPCTSTR filename, KImageDocVector& imagePages, CString& errmsg)
{
  IWICImagingFactory* pImageFactory = 0;

  HRESULT hr = K_CoInitializeEx(0, COINIT_MULTITHREADED);
  if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory, (void**)&pImageFactory)))
  {

    IWICStream *piFileStream = NULL;
    IWICBitmapEncoder *piEncoder = NULL;

    TCHAR msg[256];

    // Create a file stream.
    hr = pImageFactory->CreateStream(&piFileStream);

    // Initialize our new file stream.
    if (SUCCEEDED(hr))
    {
      hr = piFileStream->InitializeFromFilename(filename, GENERIC_WRITE);
    }

    // Create the encoder.
    if (SUCCEEDED(hr))
    {
      hr = pImageFactory->CreateEncoder(GUID_ContainerFormatTiff, NULL, &piEncoder);

      if (FAILED(hr))
      {
        StringCchPrintf(msg, 256, _T("Fail on Create TIFF Encoder(Error=0x%x)"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }

    // Initialize the encoder
    if (SUCCEEDED(hr))
    {
      hr = piEncoder->Initialize(piFileStream, WICBitmapEncoderNoCache);
    }
    else
    {
      _com_error err(hr);
      errmsg.SetString(err.ErrorMessage());
      StoreLogHistory(_T(__FUNCTION__), errmsg, SYNC_MSG_LOG);

      piFileStream->Release();
      pImageFactory->Release();
      return FALSE;
    }

    // EcmDocTypeInfo 정보를 스트링으로 저장. 첫번째 프레임에 저장.
    KEcmDocTypeInfo* docTypeInfo = &mDocTypeInfo;

    int frame_count = 0;
    LPTSTR folderNames = NULL; // m_Pages.GetSubFolderNames(); 이제 저장하지 않음

    KImageDocIterator it = imagePages.begin();
    while (it != imagePages.end())
    {
      KImageBase* page = *it++;
      if (page->GetType() == IMAGE_PAGE)
      {
        // update last-modified if changed
        if (page->IsModified())
          page->InitMetaDatas(FALSE);

        if (!StoreFrame(pImageFactory, piEncoder, ImageEncodingFormat::Tiff, (KImagePage*)page, folderNames, docTypeInfo))
          it = imagePages.end();
        else
          frame_count++;

        // save it only on first frame
        if (folderNames != NULL)
        {
          delete[] folderNames;
          folderNames = NULL;
        }
        docTypeInfo = NULL;
      }
    }

    StringCchPrintf(msg, 256, _T("Store Multi-TIFF(frame=%d) : "), frame_count);
    StringCchCat(msg, 256, filename);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    
    piEncoder->Commit();
    piFileStream->Commit(STGC_DEFAULT);

    piEncoder->Release();
    piFileStream->Release();
    pImageFactory->Release();
  }
  K_CoUninitialize();

  if (IsFileEditing(filename))
    StoreLogHistory(_T(__FUNCTION__), _T("Saved file locked"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);

  return TRUE;
}

BOOL CVivaImagingDoc::StoreFrame(IWICImagingFactory* pImageFactory, IWICBitmapEncoder *piEncoder, ImageEncodingFormat encode, KImagePage* image, LPCTSTR folderList, KEcmDocTypeInfo* pInfo)
{
#if 1
  if (image != NULL)
  {
    KVoidPtrArray metalist;
    image->GetMetaDataSet(metalist, pInfo, META_FOR_STORE | META_INCLUDE_ANNOTATES);

    image->StoreImageWithFrameEncoder(pImageFactory, piEncoder, encode, &metalist);

    ClearKeyNameValueArray(metalist);
  }
  else
  {
    TCHAR msg[256] = _T("Load image fail : ");
    StringCchCat(msg, 256, image->GetLocalFilename());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return FALSE;
  }
#else
  IWICBitmapFrameEncode *piFrameEncode = NULL;
  // IWICMetadataQueryWriter *piFrameQWriter = NULL;
  IWICPalette* pPalette = NULL;
  IPropertyBag2 *pPropertybag = NULL;
  HRESULT hr = piEncoder->CreateNewFrame(&piFrameEncode, &pPropertybag);

  KBitmap* bm = image->GetImage(mWorkingPath);
  if (bm == NULL)
  {
    TCHAR msg[256] = _T("Load fail : ");
    StringCchCat(msg, 256, image->GetLocalFilename());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

  if (SUCCEEDED(hr) && (bm != NULL))
  {
    // This is how you customize the TIFF output.
    PROPBAG2 option = { 0 };
    option.pstrName = L"TiffCompressionMethod";
    VARIANT varValue;
    VariantInit(&varValue);
    varValue.vt = VT_UI1;
    varValue.bVal = WICTiffCompressionZIP;
    hr = pPropertybag->Write(1, &option, &varValue);
    if (SUCCEEDED(hr))
    {
      hr = piFrameEncode->Initialize(pPropertybag);
    }
    else
    {
      hr = piFrameEncode->Initialize(NULL);
    }

    WICPixelFormatGUID pixelFormat = { 0 };
    UINT count = 0;

    double dpiX = bm->horzDPI;
    double dpiY = bm->vertDPI;

    hr = piFrameEncode->SetSize(bm->width, bm->height);
    hr = piFrameEncode->SetResolution(dpiX, dpiY);

    if (bm->bitCount == 24)
      pixelFormat = GUID_WICPixelFormat24bppBGR;
    else if (bm->bitCount == 8)
      pixelFormat = GUID_WICPixelFormat8bppIndexed;
    else if (bm->bitCount == 4)
      pixelFormat = GUID_WICPixelFormat4bppIndexed;
    else if (bm->bitCount == 1)
      pixelFormat = GUID_WICPixelFormat1bppIndexed;
    else if (bm->bitCount == 32)
      pixelFormat = GUID_WICPixelFormat32bppPBGRA;

    hr = piFrameEncode->SetPixelFormat(&pixelFormat);

    if (bm->bitCount <= 8)
    {
      pImageFactory->CreatePalette(&pPalette);

      WICColor* pPaletteColors = new WICColor[bm->paletteNumber];
      for (int i = 0; i < bm->paletteNumber; i++)
      {
        pPaletteColors[i] = RGB(bm->palette[i].rgbRed, bm->palette[i].rgbGreen, bm->palette[i].rgbBlue);
      }

      pPalette->InitializeCustom(pPaletteColors, bm->paletteNumber);
      delete[] pPaletteColors;
      piFrameEncode->SetPalette(pPalette);
    }

    image->StoreMetaTagToFrameEncoder(piFrameEncode, folderList);
    image->StoreAnnotatesToFrameEncoder(piFrameEncode, pInfo);

    UINT cbBufferSize = bm->lineBufferLength * abs(bm->height);

#ifdef USE_TOPDOWN_KBITMAP
    LPBYTE buff = getTopDownImageBuffer();
    hr = piFrameEncode->WritePixels(abs(height), lineBufferLength, cbBufferSize, buff);
    delete[] buff;
#else
    hr = piFrameEncode->WritePixels(bm->height, bm->lineBufferLength, cbBufferSize, bm->bitImage);
#endif

    // Commit the frame.
    if (SUCCEEDED(hr))
    {
      hr = piFrameEncode->Commit();
    }
    else
    {
      _com_error err(hr);
      TCHAR msg[256];

      StringCchCopy(msg, 256, err.ErrorMessage());
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
  }
  if (pPalette != NULL)
    pPalette->Release();
  if (pPropertybag != NULL)
    pPropertybag->Release();
  if (piFrameEncode != NULL)
    piFrameEncode->Release();
#endif
  return TRUE;
}

void CVivaImagingDoc::ClearModifiedFlag()
{
  m_Pages.SetModified(TRUE, PageModifiedFlag::NO_MODIFIED);
}

BOOL CVivaImagingDoc::IsEditable()
{
  // check permission, no uploading
  return !(mGlobalOption & GO_READONLY);
}

BOOL CVivaImagingDoc::IsChangableToEdit()
{
  return (mGlobalOption & GO_EDITABLE);
}

BOOL CVivaImagingDoc::IsFolderEditable()
{
  return m_Pages.IsEditable();
}

// CVivaImagingDoc serialization

void CVivaImagingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CVivaImagingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CVivaImagingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CVivaImagingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CVivaImagingDoc 진단

#ifdef _DEBUG
void CVivaImagingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVivaImagingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVivaImagingDoc 명령
int CVivaImagingDoc::ImageAutoRename()
{
  HANDLE backup = m_Pages.BackupPageNames();
  mActionManager.SaveAction(EA_AutoRename, NULL, backup);

  int index = 1;
  int base = 1;
  SetModifiedFlag();

  return m_Pages.ImageAutoRename(mWorkingPath, index, base);
}

int CVivaImagingDoc::GetNumberOfPages(int flag)
{
  return m_Pages.GetNumberOfPages(flag);
}

BOOL CVivaImagingDoc::MovePreviousPage()
{
  if (m_ActivePage != NULL)
  {
    KImageBase* f = m_ActivePage->GetParent();
    if ((f != NULL) && (f->GetType() == FOLDER_PAGE))
    {
      int index = ((KImageFolder *)f)->IsChildOf(m_ActivePage);
      if (index > 0)
      {
        ((KImageFolder *)f)->RemovePage(m_ActivePage);
        ((KImageFolder *)f)->InsertPage(m_ActivePage, index - 1);
        SetModifiedFlag();
        return TRUE;
      }
    }
  }
  return FALSE;
}

BOOL CVivaImagingDoc::MoveNextPage()
{
  if (m_ActivePage != NULL)
  {
    KImageBase* f = m_ActivePage->GetParent();
    if ((f != NULL) && (f->GetType() == FOLDER_PAGE))
    {
      int index = ((KImageFolder *)f)->IsChildOf(m_ActivePage);
      if (index < (((KImageFolder *)f)->GetChildCount() - 1))
      {
        ((KImageFolder *)f)->RemovePage(m_ActivePage);
        ((KImageFolder *)f)->InsertPage(m_ActivePage, index + 1);
        SetModifiedFlag();
        return TRUE;
      }
    }
  }
  return FALSE;
}

BOOL CVivaImagingDoc::GotoPreviousPage()
{
  KImageBase* p[3] = { NULL, m_ActivePage, NULL };
  if (m_Pages.NextOrPreviousPage(p, 0, EXCLUDE_FOLDED))
  {
    if (p[0] != NULL)
    {
      SetActivePage(p[0]);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL CVivaImagingDoc::CanGotoPreviousPage()
{
  int active_page_index = GetActivePageIndex();
  return (active_page_index > 0);
}

BOOL CVivaImagingDoc::GotoNextPage()
{
  KImageBase* p[3] = { NULL, m_ActivePage, NULL };
  if (m_Pages.NextOrPreviousPage(p, 0, EXCLUDE_FOLDED))
  {
    if (p[2] != NULL)
    {
      SetActivePage(p[2]);
      return TRUE;
    }
  }
  return FALSE;
}
BOOL CVivaImagingDoc::CanGotoNextPage()
{
  KImageBase* p[3] = { NULL, m_ActivePage, NULL };
  if (m_Pages.NextOrPreviousPage(p, 0, EXCLUDE_FOLDED))
  {
    if (p[2] != NULL)
      return TRUE;
  }
  return FALSE;
}

BOOL CVivaImagingDoc::IsImagePage()
{
  return ((m_ActivePage != NULL) && (m_ActivePage->GetType() == IMAGE_PAGE));
}

LPTSTR CVivaImagingDoc::CopyFileToWorkspace(LPCTSTR pathName)
{
#if 0
  LPCTSTR e = StrRChr(pathName, NULL, '\\');
  if (e != NULL)
  {
    int pathLen = (e - pathName);
    if (pathLen == lstrlen(mWorkingPath) && (StrCmpN(mWorkingPath, pathName, pathLen) == 0))
    {
      return (LPTSTR)pathName;
    }

    e++;
    pathLen = lstrlen(mWorkingPath) + lstrlen(e) + 2;
    LPTSTR targetPathName = AllocPathName(mWorkingPath, e);

    if (!CopyFile(pathName, targetPathName, FALSE))
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("CopyFile lastErr=%d, %s => %s"), GetLastError(), pathName, targetPathName);
      return NULL;
    }
    return targetPathName;
  }
  else
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Bad file name:%s"), pathName);
    return NULL;
  }
#else
  return (LPTSTR)pathName;
#endif
}

int CVivaImagingDoc::AddImagePages(LPCTSTR pFilenames, KIntVector* pPageList, int flag, LPCTSTR deviceName)
{
  int count = 0;
  int len = lstrlen(pFilenames);
  LPCTSTR n = pFilenames;

  if (IsDirectoryExist(pFilenames))
  {
    n = pFilenames + (len + 1);
  }

  while ((n != NULL) && (*n != '\0'))
  {
    len = lstrlen(n);
    LPCTSTR fullName = NULL;
    LPTSTR folderName = NULL;
    LPTSTR e = StrChr(n, '|');
    BOOL isFolder = FALSE;

    if (e != NULL) // D|docOID|fileOID|storageOID|permission or F|folderOID|fullPathIndex|permission
    {
      *e = '\0';
      e++;

      isFolder = (*e == 'F');
      e = StrChr(e, '|');
      if (e != NULL)
        e++;
    }

#ifdef _DEBUG
    StoreLogHistory(_T(__FUNCTION__), n, SYNC_MSG_LOG);
#endif

    if (IsFullPath(n))
    {
      fullName = AllocString(n);
    }
    else
    {
      SplitPathNameAlloc(n, &folderName, NULL);
      LPCTSTR filename = StrRChr(n, NULL, '\\');
      if (filename != NULL)
        filename++;
      else
        filename = n;
      fullName = AllocPathName(pFilenames, filename);
    }

    if ((fullName != NULL) && (lstrlen(fullName) > 0))
    {
      int pid = 0;

      if (flag & ADD_WITH_FOLDER)
        m_ActivePage = NULL;

      if (IsDirectoryExist(fullName) || isFolder)
      {
        if (flag & ADD_WITH_FOLDER)
          pid = AddNewFolder(n, flag, e);
      }
      else
      {
        pid = AddNewPage(fullName, flag, deviceName, folderName, e);
      }

      if (pid > 0)
      {
        count++;
        if (pPageList != NULL)
          pPageList->push_back(pid);
        flag &= ~CLEAR_SELECT;
      }
      delete[] fullName;
    }
    else // this is root item
    {
      if (e != NULL)
        m_Pages.SetEcmInfo(e);
    }

    if (folderName != NULL)
      delete[] folderName;

    n += +(len + 1);
  }

  if (flag & ADD_SELECT_FIRST)
  {
    SetActivePageIndex(0);
    SelectPage(0, TRUE);
  }
  return count;
}

int CVivaImagingDoc::AddImagePagesEx(LPCTSTR pFilenames, int length, int flag, LPCTSTR deviceName)
{
  // it can be 'folder + files', or just one file
  KIntVector newPageList;

  AddImagePages(pFilenames, &newPageList, flag, deviceName);

  // save ActionManager for Undo/Redo
  int count = (int)newPageList.size();
  if (count > 0)
  {
    int* pageIdList = new int[count];
    KintIterator it = newPageList.begin();
    int i = 0;

    // store inserted page's ID to for backup data
    memset(pageIdList, 0, sizeof(int) * count);
    while(it != newPageList.end())
    {
      pageIdList[i++] = *it++;
    }

    // add device name to param
    int buffLength = length + lstrlen(deviceName) + 1;
    LPTSTR param = new TCHAR[buffLength];
    LPTSTR p = param;
    if (lstrlen(deviceName) > 0)
      StringCchCopy(param, buffLength, deviceName);
    else
      *p = '\0';
    p += lstrlen(deviceName) + 1;
    memcpy(p, pFilenames, length * sizeof(TCHAR));

    SetModifiedFlag();
    mActionManager.SaveAction(EA_InsertPage, (LPBYTE)param, buffLength * sizeof(TCHAR),
      (LPBYTE)pageIdList, sizeof(int) * count);

    if (flag & ADD_SELECT_FIRST)
    {
      KImageBase* a = m_Pages.FindPageByID(pageIdList[0]);
      if (a != NULL)
      {
        a->SetSelected(TRUE);
        SetActivePage(a);
      }
    }

    delete[] pageIdList;
    delete[] param;
  }
  return count;
}

/*
int CVivaImagingDoc::AddNewPages(KVoidPtrArray& filelist, int insert_flag, LPCTSTR deviceName)
{
  // change to string list
  int len = 0;
  KVoidPtrIterator it = filelist.begin();
  while (it != filelist.end())
  {
    LPTSTR filename = (LPTSTR)*(it++);
    len += lstrlen(filename) + 1;
  }
  len++;

  int bufflen = len;
  LPTSTR names = new TCHAR[len];
  LPTSTR p = names;
  it = filelist.begin();
  while (it != filelist.end())
  {
    LPTSTR filename = (LPTSTR)*(it++);
    StringCchCopy(p, len, filename);
    len -= lstrlen(filename) + 1;
    p += lstrlen(filename) + 1;
  }
  *p = '\0';
  len = AddImagePagesEx(names, bufflen, insert_flag, deviceName);
  delete names;
  return len;
}
*/

int CVivaImagingDoc::UpdatePages(LPCTSTR pFilenames, int length, int flag, KIntVector& pageList)
{
  //addImagePages(pFilenames, &newPageList, flag, deviceName);
  int count = 0;
  int len = lstrlen(pFilenames);
  LPCTSTR n = pFilenames;

  if (IsDirectoryExist(pFilenames))
  {
    n = pFilenames + (len + 1);
  }

  while ((n != NULL) && (*n != '\0'))
  {
    len = lstrlen(n);
    LPCTSTR fullName = NULL;
    LPTSTR folderName = NULL;
    LPCTSTR filename = NULL;
    LPTSTR e = StrChr(n, '|');
    BOOL isFolder = FALSE;

    if (e != NULL)// fileOID & storageOID & permission
    {
      *e = '\0';
      e++;

      isFolder = (*e == 'F');
      e = StrChr(e, '|');
      if (e != NULL)
        e++;
    }

    if (IsFullPath(n))
    {
      fullName = AllocString(n);
      filename = StrRChr(fullName, NULL, '\\');
      if (filename != NULL)
        filename++;
      else
        filename = fullName;
    }
    else
    {
      SplitPathNameAlloc(n, &folderName, NULL);
      filename = StrRChr(n, NULL, '\\');
      if (filename != NULL)
        filename++;
      else
        filename = n;
      fullName = AllocPathName(pFilenames, filename);
    }

    if ((fullName != NULL) && (lstrlen(fullName) > 0))
    {
      KImageBase* p = NULL;

      if (folderName != NULL)
      {
        p = m_Pages.FindPageByName(folderName, FOLDER_PAGE);
      }
      else
      {
        p = &m_Pages;
      }

      KImageBase* page = NULL;
      if (p != NULL)
        page = ((KImageFolder*)p)->FindPageByName(filename, IMAGE_PAGE);

      if (page != NULL)
      {
        page->ClearFlag(IS_FILE_NOT_READY);
        if (e != NULL)
          page->SetEcmInfo(e);
        pageList.push_back(page->GetID());
      }
#ifdef USE_XML_STORAGE
      else if (IsImagingXMLDocFile(fullName))
      {
        mpDownloadedBodyDocName = AllocString(filename);
      }
#endif // USE_XML_STORAGE

      delete[] fullName;
    }

    if (folderName != NULL)
      delete[] folderName;

    n += +(len + 1);
  }
  return pageList.size();
}

void CVivaImagingDoc::UpdateExtDocTypeInfo(EcmFolderItem* ecmRoot, KEcmDocTypeInfo* pInfo)
{
  int index = 0;
  KImageBase* p = NULL;

  if (pInfo != NULL)
    mDocTypeInfo = *pInfo;
  while ((p = m_Pages.GetIndexedPage(index, IMAGE_ONLY)) != NULL)
  {
    KImagePage* ip = (KImagePage * )p;
    EcmBaseItem* s = ecmRoot->FindItemByName(ip->mName);
    if ((s != NULL) && !s->IsFolder())
    {
      EcmFileItem* fs = (EcmFileItem*)s;
      if (fs->pMetadata != NULL)
        ip->SetDocExtAttributes(*(fs->pMetadata));
    }
    index++;
  }
}

void CVivaImagingDoc::insertPage(KImageBase* page, KImageBase* parent)
{
  if ((parent != NULL) && (parent->GetType() == FOLDER_PAGE))
  {
    ((KImageFolder*)parent)->InsertPage(page, m_ActivePage);
    SetActivePage(page);
    return;
  }
  // insert at next of current page
  if (m_ActivePage != NULL)
  {
    KImageBase* f = m_ActivePage->GetParent();
    if ((f != NULL) && (f->GetType() == FOLDER_PAGE))
    {
      ((KImageFolder*)f)->InsertPage(page, m_ActivePage);
      SetActivePage(page);
      return;
    }
  }

  // then 
  m_Pages.InsertPage(page, NULL);
  SetActivePage(page);
}

int CVivaImagingDoc::AddNewPage(LPCTSTR pPathName, int flag, LPCTSTR deviceName, LPCTSTR folderName, LPCTSTR ecmInfo)
{
  KImageDocVector pageList;
  CString str;

  if (!IsFileExist(pPathName))
  {
    if ((flag & ADD_WITH_EMPTY) && HasValidFileExtension(pPathName, FALSE))
    {
      KImageFolder* folder = NULL;
      if (folderName != NULL)
      {
        folder = (KImageFolder*)m_Pages.FindPageByName(folderName, FOLDER_PAGE);
        if (folder == NULL)
          folder = addNewFolder(folderName);
      }

      if (folder == NULL)
        folder = &m_Pages;

      // change it as relative pathname
      LPCTSTR filename = pPathName;
      if (IsChildFolderOrFile(mWorkingPath, pPathName, FALSE))
      {
        filename = &pPathName[lstrlen(mWorkingPath)];
        ASSERT((filename != NULL) && (*filename == '\\'));
        ++filename;
      }

      KImagePage* page = new KImagePage(folder, filename); // pPathName);
      if (page != NULL)
      {
        page->AddFlag(IS_FILE_NOT_READY);

        if (ecmInfo)
          page->SetEcmInfo(ecmInfo);
        insertPage(page, folder);

        if (flag & ADD_SELECT)
        {
          page->SetSelected(TRUE);
        }
        if (flag & READY_META_INFO)
        {
          page->InitMetaDatas(TRUE);
          page->SetAcquiredDevice(deviceName);
        }
        // 새로 추가된 이미지가 Modified 이어야 하는가?
        // page->SetModified(TRUE, ALL_MODIFIED);
        pageList.push_back(page);
      }
    }
  }
  else if (InsertImageFile(pPathName, &pageList, TRUE, str, folderName) > 0)
  {
    SetModifiedFlag();
    // change selection with flag
    if (flag & CLEAR_SELECT)
      m_Pages.SetSelectedRecursive(FALSE);

    int index = 0;
    KImageDocIterator it = pageList.begin();
    while (it != pageList.end())
    {
      KImageBase* page = *it++;
      if (flag & ADD_SELECT)
      {
        page->SetSelected(TRUE);
      }
      if (flag & READY_META_INFO)
      {
        page->InitMetaDatas(TRUE);
        page->SetAcquiredDevice(deviceName);
      }
      if (ecmInfo)
        page->SetEcmInfo(ecmInfo);

      // 새로 추가된 이미지가 Modified 이어야 하는가?
      //page->SetModified(TRUE, ALL_MODIFIED);

      if (index == 0)
      {
        LPTSTR name = NULL;
        SplitPathNameAlloc(pPathName, NULL, &name);
        page->SetName(name);
        if (name != NULL)
          delete[] name;
      }
      index++;
    }
  }

  TCHAR msg[256] = _T("Add image : ");
  StringCchCat(msg, 256, pPathName);
  if (str.GetLength() > 0)
  {
    StringCchCat(msg, 256, _T(","));
    StringCchCat(msg, 256, str);
  }
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

  KImageDocIterator it = pageList.begin();
  if (it != pageList.end())
  {
    KImageBase* page = *it;
    SetActivePage(page);
    return page->mID;
  }
  return 0;
}

int CVivaImagingDoc::AddNewPage(PBITMAPINFOHEADER pDIB, int flag, LPCTSTR deviceName)
{
  KImagePage* page = new KImagePage(NULL, pDIB);

  // need to store hear
  page->StoreImageFile(mWorkingPath, NULL, NULL, FALSE);

  LPCTSTR temp_filename = page->GetLocalFilename();

  int len = lstrlen(mWorkingPath) + lstrlen(temp_filename) + 4;
  LPTSTR filenames = new TCHAR[len];
  LPTSTR buff = filenames;
  StringCchCopy(buff, len, mWorkingPath);
  buff += lstrlen(buff) + 1;
  StringCchCopy(buff, len, temp_filename);
  buff += lstrlen(buff) + 1;
  buff[0] = '\0';
  delete page;

  int r = AddImagePagesEx(filenames, len, flag, deviceName);
  delete[] filenames;

  SetModifiedFlag();
  return r;
}

KImageFolder* CVivaImagingDoc::addNewFolder(LPCTSTR name)
{
  KImageFolder* page = new KImageFolder(&m_Pages);
  int index = -1; // at end

  TCHAR defPageName[64];
  if (name == NULL)
  {
    KImageFolder::ReadyDefaultName(defPageName);
    name = defPageName;
  }

  // 1 Depth만
  if ((m_ActivePage != NULL) && (m_ActivePage != &m_Pages))
  {
    KImageBase* p = m_ActivePage->GetParent();

    if (p == &m_Pages)
      p = m_ActivePage;

    index = m_Pages.IsChildOf(p);
    if (index >= 0)
      index++;
  }
  m_Pages.InsertPage(page, index);
  page->SetName(name);
  return page;
}

int CVivaImagingDoc::AddNewFolder(LPCTSTR name, int flag, LPCTSTR ecmInfo)
{
  KImageFolder* page = addNewFolder(name);

  if (page != NULL)
  {
    if (ecmInfo != NULL)
      page->SetEcmInfo(ecmInfo);
    SetModifiedFlag();

    // change selection with flag
    if (flag & CLEAR_SELECT)
      m_Pages.SetSelectedRecursive(FALSE);
    if (flag & ADD_SELECT)
      page->SetSelected(TRUE);

    SetActivePage(page);

    TCHAR msg[256] = _T("Add new Folder : ");
    StringCchCat(msg, 256, page->mName);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    return page->mID;
  }
  return 0;
}

BOOL CVivaImagingDoc::SetDocTypeInfo(KEcmDocTypeInfo& docTypeInfos)
{
  // 백업 저장
  HANDLE backup = SaveDocTypeMetaData();
  HANDLE param = docTypeInfos.ToHandle();

  mActionManager.SaveAction(EA_ModifyDocType, param, backup);

  // 확장속성 정보 업데이트
  mDocTypeInfo = docTypeInfos;

  // CHECK : 각 이미지 페이지 ExtAttribute clear
  m_Pages.ClearMetaData(TRUE);

  //Mainframe에 속성 업데이트 요청??
  SetModifiedFlag();
  return TRUE;
}

KImageBase* CVivaImagingDoc::GetRootImagePage()
{
  return &m_Pages;
}

KImageBase* CVivaImagingDoc::GetImagePage(int flag, int index)
{
  if (index < 0)
    return m_ActivePage;

  KImageBase* p = m_Pages.GetIndexedPage(index, flag);
  if (p == NULL)
    p = &m_Pages;
  return p;
  /*
  if ((index >= 0) && (index < (int)m_Pages.size()))
  {
    KImageDocIterator it = m_Pages.begin();
    if (index > 0)
      it += index;
    return *it;
  }
  return NULL;
  */
}

BOOL CVivaImagingDoc::OnAccessCacheImage(KImagePage* p)
{
  // it will diert total cache size
  mCacheManager.OnAccessImage(&m_Pages, p, mWorkingPath);
  return TRUE;
}

BOOL CVivaImagingDoc::GetCurrentPageInfo(int index, int& w, int& h, int& c, LPCTSTR* pName)
{
  KImageBase* p = GetImagePage(EXCLUDE_FOLDED, index);
  if (p != NULL)
  {
    if (pName != NULL)
      *pName = p->GetName();
    if (p->GetType() == IMAGE_PAGE)
    {
      KBitmap* b = ((KImagePage*)p)->GetImage(NULL);
      if (b != NULL)
      {
        w = b->width;
        h = b->height;
        c = b->bitCount;
        return TRUE;
      }
    }
    else if (p->GetType() == FOLDER_PAGE)
    {
      return TRUE;
    }
  }
  return FALSE;
}

void CVivaImagingDoc::ClearThumbImage(BOOL clearAll, int index)
{
  if (clearAll)
  {
    m_Pages.ClearThumbImage(TRUE);
  }
  else
  {
    KImageBase* p = (index < 0) ? m_ActivePage : m_Pages.GetIndexedPage(index, EXCLUDE_FOLDED);
    if (p != NULL)
      p->ClearThumbImage(clearAll);
  }
}

void CVivaImagingDoc::SetActivePage(KImageBase* page)
{
  if (m_ActivePage != page)
  {
    if ((m_ActivePage != NULL) &&
      (m_Pages.GetIndexOfPage(m_ActivePage, 0) >= 0) &&
      (m_ActivePage->GetType() == IMAGE_PAGE))
      ((KImagePage *)m_ActivePage)->ClearPreviewImage();

    m_ActivePage = page;
  }
}

int CVivaImagingDoc::GetActivePageIndex()
{
  return m_Pages.GetIndexOfPage(m_ActivePage, EXCLUDE_FOLDED);
}

BOOL CVivaImagingDoc::SetActivePageIndex(int index)
{
  if (index != GetActivePageIndex())
  {
    KImageBase* page = m_Pages.GetIndexedPage(index, EXCLUDE_FOLDED);
    SetActivePage(page);
    return TRUE;
  }
  return FALSE;
}

// CVivaImagingDoc private 명령

void CVivaImagingDoc::clearDocument()
{
  SetActivePage(NULL);
  m_Pages.Clear();

  mActionManager.Clear();
  mDocTypeInfo.Clear();

  if ((mWorkingPath != NULL) && (lstrlen(mWorkingPath) > 0))
  {
    if (mIsTempFolder && IsDirectoryExist(mWorkingPath))
    {
      RemoveDirectory(mWorkingPath);
    }
    delete[] mWorkingPath;
    mWorkingPath = NULL;
    mIsTempFolder = FALSE;
  }
  if (mpDownloadedBodyDocName != NULL)
  {
    delete[] mpDownloadedBodyDocName;
    mpDownloadedBodyDocName = NULL;
  }
}

BOOL CVivaImagingDoc::SelectPageRange(int p1, int p2, int flag)
{
  int index = 0;
  int selected = m_Pages.SelectPageRange(index, p1, p2, flag);
  return (selected > 0);
}

BOOL CVivaImagingDoc::ToggleSelectPage(int index)
{
  KImageBase* p = GetImagePage(EXCLUDE_FOLDED, index);
  if (p != NULL)
  {
    p->SetSelected(!p->IsSelected());
    return TRUE;
  }
  return FALSE;
}

BOOL CVivaImagingDoc::SelectPage(int index, BOOL bSelect)
{
  KImageBase* p = GetImagePage(EXCLUDE_FOLDED, index);
  if (p != NULL)
    return p->SetSelected(bSelect);
  return FALSE;
}

BOOL CVivaImagingDoc::SelectAllPages(BOOL bSelect)
{
  return m_Pages.SelectAllPages(bSelect);
}

HANDLE CVivaImagingDoc::DeleteSelectedPages()
{
  // store to undo buffer
  HANDLE h = CopyToClipboard(TRUE);
  HANDLE gh = KEditAction::CopyActionBuffer(h);
  mActionManager.SaveAction(EA_DeletePage, NULL, gh);
  SetModifiedFlag();

  int active_page_index = m_Pages.GetIndexOfPage(m_ActivePage, EXCLUDE_FOLDED);
  int r = m_Pages.DeleteSelectedPages();
  if (r > 0)
  {
    // adjust active page
    KImageBase* p = NULL;
    int total_index = GetNumberOfPages(EXCLUDE_FOLDED);
    if (total_index > 0)
    {
      if (active_page_index >= total_index)
        active_page_index = total_index - 1;
      if (active_page_index >= 0)
        p = GetImagePage(EXCLUDE_FOLDED, active_page_index);
    }

    if (p != NULL)
      p->SetSelected(TRUE);
    SetActivePage(p);
  }
  else
  {
    if (h != NULL)
    {
      KMemoryFree(h);
      h = NULL;
    }
  }
  return h;
}

BOOL CVivaImagingDoc::SetMetaData(KImageBase* page, LPCTSTR key, LPCTSTR str)
{
  /*
  if (CompareStringNC(key, _T("Name")) == 0) //PROPERTY_NAME == propertyId)
  {
    page->SetName(str);
    return;
  }
  */
  EcmExtDocTypeColumn* column = mDocTypeInfo.Get(key);
  if (column != NULL)
  {
    page->SetMetaData(key, str, column->type, MetaDataCategory::ExtDocType, column->name, TRUE);
    return TRUE;
  }
  else
  {
    TCHAR msg[256] = _T("Unknown MetaData key : ");
    StringCchCat(msg, 256, key);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
  }
  return FALSE;
}

int CVivaImagingDoc::PropertyChange(LPCTSTR key, int value)
{
  return 0;
}

int CVivaImagingDoc::PropertyChange(LPCTSTR key, LPCTSTR str)
{
  if (m_ActivePage != NULL)
  {
    if (lstrcmp(key, _T("Name")) == 0) // (page->GetType() == FOLDER_PAGE)
    {
      if (lstrlen(str) > 0)
      {
        HANDLE backup = m_ActivePage->BackupName();
        HANDLE param = KImageBase::AllocStringHandle(str);

        m_ActivePage->SetName(str);

        mActionManager.SaveAction(EA_ModifyName, param, backup);

        int r = CHANGED_PAGE_NAME;
        if (m_ActivePage->GetType() == FOLDER_PAGE)
          r |= UPDATE_ACTIVE_VIEW;
        return r;
      }
      else
      {
        TCHAR msg[32] = _T("Cannot erase folder name");
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      }
      return 0;
    }

    HANDLE backup = m_ActivePage->BackupMetaKeyValue(key);
    HANDLE param = KImageBase::StoreMetaKeyValue(m_ActivePage->GetID(), key, str);

    if (SetMetaData(m_ActivePage, key, str))
    {
      mActionManager.SaveAction(EA_ModifyMetaData, param, backup);
      return CHANGED_PAGE_NAME;
    }
    else
    {
      KMemoryFree(backup);
      KMemoryFree(param);
    }
  }
  return 0;
}

int CVivaImagingDoc::DetachSelectedPages(KVoidPtrArray& pages)
{
  return m_Pages.DetachSelectedPages(pages);
}

BOOL CVivaImagingDoc::InsertPages(int targetIndex, InsertTargetType inside, KImageBase* at, KVoidPtrArray& pages)
{
  if ((inside == InsertTargetType::TARGET_INSIDE) && (at != NULL) && (at->GetType() == FOLDER_PAGE))
  {
    ((KImageFolder*)at)->InsertPages(0, NULL, inside, pages); // insert at top of child
    if (pages.size() > 0)
      m_Pages.InsertPages(1, NULL, inside, pages);
  }

  SetModifiedFlag();

  return m_Pages.InsertPages(targetIndex, at, inside, pages);
}

BOOL CVivaImagingDoc::MoveSelectedPages(int targetPageIndex, InsertTargetType inside, KImageBase* target_page)
{
  // store current page ordering
  int count = m_Pages.StorePageIds(NULL, 10000, 0);
  HANDLE bh = KMemoryAlloc((count + 1) * sizeof(int));
  int* p = (int*)KMemoryLock(bh);

  // store current active page
  *p = m_ActivePage->GetID();
  p++;

  int stored = m_Pages.StorePageIds(p, count, 0);
  ASSERT(stored == count);
  KMemoryUnlock(bh);

  KVoidPtrArray pages;
  int nPages = DetachSelectedPages(pages);
  if (pages.size() > 0)
  {
    // store move action data
    HANDLE param = KMemoryAlloc((pages.size() + 5) * sizeof(int));
    p = (int*)KMemoryLock(param);

    p[0] = EA_MovePage;
    p[1] = targetPageIndex;
    p[2] = (int)inside;
    p[3] = (target_page != NULL) ? target_page->GetID() : 0;
    p[4] = pages.size();

    KVoidPtrIterator sit = pages.begin();
    stored = 5;
    while (sit != pages.end())
    {
      KImageBase* sp = (KImageBase*)*(sit++);
      p[stored++] = sp->GetID();
    }

    mActionManager.SaveAction(EA_MovePage, param, bh);

    InsertPages(targetPageIndex, inside, target_page, pages);

    SetModifiedFlag();
    return TRUE;
  }
  return FALSE;
}

int CVivaImagingDoc::ShapeEditCommand(int channel, int* params, void* extra, RECT& rect)
{
  KImageBase* p = GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
  {
    // store to action buffer
    HANDLE backup = ((KImagePage*)m_ActivePage)->SaveAnnotateData();

    int state = ((KImagePage *)p)->ShapeEditCommand(channel, params, extra, rect);

    if (state & R_ANNOTATE_CHANGED) // && (backup != NULL))
    {
      HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
      EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
      ed->annotate = backup;
      ed->targetPageId = m_ActivePage->GetID();
      KMemoryUnlock(h);

      SHAPE_COMMAND_TYPE commandType = (SHAPE_COMMAND_TYPE)params[0];
      EditActionType type;
      if (commandType <= SHAPE_ALIGN_BOTTOM)
        type = EA_AnnotateAlign;
      else if (commandType <= SHAPE_ORDER_BOTTOM)
        type = EA_AnnotateOrder;
      else // if (commandType <= SHAPE_APPLY_PROPERTY)
        type = EA_AnnotateProperty;

      mActionManager.SaveAction(type, NULL, h);
    }
    else
    {
      // change default property

      if (backup != NULL)
        KMemoryFree(backup);
    }
    return state;
  }
  return 0;
}

int CVivaImagingDoc::ApplyEffect(ImageEffectJob* effect)
{
  int r = 0;

  if (!m_ActivePage->IsEditable())
    return 0;

  // HANDLE h = CopyToClipboard(effect->applyAll);
  HANDLE bh = CopyToEditActionBackup(effect->applyAll);

  r = m_Pages.ApplyEffect(effect, effect->applyAll);
  SetModifiedFlag();

  if (bh != NULL)
  {
    if ((PropertyMode::PModeRotate <= effect->job) && (effect->job <= PropertyMode::PModeColorReverse))
    {
      UINT size = sizeof(ImageEffectJob);
      HANDLE param = KMemoryAlloc(size); //  StoreImageEffect(effect);
      LPVOID p = KMemoryLock(param);
      memcpy(p, effect, size);
      KMemoryUnlock(param);

      EditActionType type = (EditActionType)((int)effect->job + (int)EA_ImageEffect);
      mActionManager.SaveAction(type, param, bh);
    }
    else if (IsSelectBlockMode(effect->job)) // PModeSelectFill, PModePasteImage
    {
      UINT size = sizeof(ImageEffectJob);
      HANDLE param = KMemoryAlloc(size); //  StoreImageEffect(effect);
      LPVOID p = KMemoryLock(param);
      memcpy(p, effect, size);
      KMemoryUnlock(param);

      EditActionType type = (EditActionType)((int)effect->job - (int)PropertyMode::PModeSelectRectangle + (int)EA_SelectRectangle);
      mActionManager.SaveAction(type, param, bh);
    }
    else
    {
      KMemoryFree(bh);

      TCHAR msg[128] = _T("skip SaveAction : ");
      int len = lstrlen(msg);
      StringCchPrintf(&msg[len], 128 - len, _T("%d"), effect->job);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    }
  }
  return r;
}

int CVivaImagingDoc::OnLButtonUp(MouseState mouseState, int nFlags, CPOINT& point, KImageDrawer& info, LPRECT ptUpdate)
{
  if ((m_ActivePage != NULL) && m_ActivePage->IsEditable() && (m_ActivePage->GetType() == IMAGE_PAGE))
  {
    HANDLE param = NULL;
    HANDLE backup = NULL;
    if (IsAnnotateMouseEditMode(mouseState)) // except select mode
    {
      backup = ((KImagePage*)m_ActivePage)->SaveAnnotateData();
    }

    int state = m_ActivePage->OnLButtonUp(mouseState, nFlags, point, info, ptUpdate);

    if (state & R_ANNOTATE_CHANGED)
    {
      HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
      EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
      ed->annotate = backup;
      ed->targetPageId = m_ActivePage->GetID();
      KMemoryUnlock(h);

      EditActionType type = EA_Max;
      if (mouseState == MouseState::MOUSE_DRAW_SELECT_RESIZE)
        type = EA_AnnotateResize;
      if (mouseState == MouseState::MOUSE_DRAW_SELECT_MOVE)
        type = EA_AnnotateMove;
      else if (mouseState == MouseState::MOUSE_DRAW_LINE)
        type = EA_AnnotateAddLine;
      else if (mouseState == MouseState::MOUSE_DRAW_FREEHAND)
        type = EA_AnnotateAddFreehand;
      else if (mouseState == MouseState::MOUSE_DRAW_FREESHAPE)
        type = EA_AnnotateAddFreeshape;
      else if (mouseState == MouseState::MOUSE_DRAW_RECTANGLE)
        type = EA_AnnotateAddRectangle;
      else if (mouseState == MouseState::MOUSE_DRAW_ELLIPSE)
        type = EA_AnnotateAddEllipse;
      else if (mouseState == MouseState::MOUSE_DRAW_TEXTBOX)
        type = EA_AnnotateAddTextbox;
      else if (IsAnnotateMouseMode(mouseState))
        type = EA_AnnotateAddShapeSmart;
      else
        StoreLogHistory(_T(__FUNCTION__), _T("Unknown MouseState"), SYNC_EVENT_ERROR | SYNC_MSG_LOG);

      mActionManager.SaveAction(type, param, h);
      SetModifiedFlag();
    }
    else
    {
      if (backup != NULL)
        KMemoryFree(backup);
    }

    if (state & R_CHANGE_SELECT_TOOL)
      ((KImagePage*)m_ActivePage)->OnSelectionChanged(NULL);
    return state;
  }
  return 0;
}

int CVivaImagingDoc::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate)
{
  KImageBase* p = GetImagePage(0);
  if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
  {
    HANDLE param = NULL;
    HANDLE backup = NULL;

    if (IsAnnotateMode(mode))
      backup = ((KImagePage*)p)->SaveAnnotateData();

    int state = p->OnKeyDown(nChar, nFlags, mode, info, ptUpdate);
    if (state != 0)
    {
      if (state & R_ANNOTATE_CHANGED)
      {
        HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
        EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
        ed->annotate = backup;
        ed->targetPageId = p->GetID();
        KMemoryUnlock(h);

        mActionManager.SaveAction(EA_AnnotateMove, param, h);
        SetModifiedFlag();
        backup = NULL;
      }
    }
    if (backup != NULL)
      KMemoryFree(backup);
    return state;
  }
  return 0;
}

int CVivaImagingDoc::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags, PropertyMode mode, KImageDrawer& info, LPRECT ptUpdate)
{
  KImageBase* p = GetImagePage(0);
  if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
  {
    int state = p->OnKeyUp(nChar, nFlags, mode, info, ptUpdate);
    return state;
  }
  return 0;
}
int CVivaImagingDoc::DeleteSelection(LPRECT ptUpdate)
{
  KImageBase* p = GetImagePage(0);
  if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
  {
    HANDLE param = NULL;
    HANDLE backup = NULL;
    backup = ((KImagePage*)p)->SaveAnnotateData();

    int state = p->DeleteSelection(ptUpdate);
    if (state != 0)
    {
      if (state & R_ANNOTATE_CHANGED)
      {
        HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
        EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
        ed->annotate = backup;
        ed->targetPageId = p->GetID();
        KMemoryUnlock(h);

        mActionManager.SaveAction(EA_AnnotateDelete, param, h);
        SetModifiedFlag();
      }
      else
      {
        if (backup != NULL)
          KMemoryFree(backup);
      }
    }
    return state;
  }
  return 0;
}


int CVivaImagingDoc::EndFloatingImagePaste(KImagePage* p, BOOL bDone, LPRECT rect)
{
  int state = 0;
  if (p->HasFloatingImage())
  {
    if (bDone)
    {
      // save to undo buffer
      HANDLE h = KMemoryAlloc(sizeof(EditActionBackupData));
      EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(h);
      ed->image = p->GetImage(mWorkingPath)->GetDIB();
      ed->targetPageId = p->GetID();
      KMemoryUnlock(h);

      HANDLE param = KMemoryAlloc(sizeof(PasteImageActionParam));
      PasteImageActionParam* pr = (PasteImageActionParam*)KMemoryLock(param);
      BOOL valid = p->GetFloatingImagePasteParam(pr);
      KMemoryUnlock(param);

      if (valid && p->EndFloatingImagePaste(bDone, rect))
      {
        SetModifiedFlag();
        mActionManager.SaveAction(EA_PasteImage, param, h);
        state |= R_REDRAW_THUMB;
      }
      else
      {
        KMemoryFree(param);
        GlobalFree(ed->image);
        KMemoryFree(h);
      }
    }
    else
    {
      p->EndFloatingImagePaste(FALSE, rect);
    }
    state |= R_REDRAW_HANDLE;
  }
  return state;
}

#if 0
int CVivaImagingDoc::StoreToFile(KImageBase* page)
{
  if (page->GetType() == IMAGE_PAGE)
  {
    page->StoreImageFile(mWorkingPath);
  }
  return 1;
}
#endif

int CVivaImagingDoc::Undo()
{
  KEditAction* a = mActionManager.Undo();
  if (a != NULL)
  {
    TCHAR msg[256];
    a->ToString(msg, 256);
    StringCchCat(msg, 256, _T(" : undo"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    int r = 0;
    if (a->mType == EA_DeletePage)
      r = undoDeletePage(a->mParam, a->mBackup);
    else if (a->mType == EA_InsertPage)
      r = undoInsertPage(a->mParam, a->mBackup);
    else if (a->mType == EA_MovePage)
      r = undoMovePage(a->mParam, a->mBackup);
    else if (a->mType == EA_ModifyMetaData)
      r = undoEditMetaData(a->mParam, a->mBackup);
    else if (a->mType == EA_ModifyDocType)
      r = undoEditDocType(a->mParam, a->mBackup);
    else if (a->mType == EA_ModifyName)
      r = undoEditName(a->mParam, a->mBackup);
    else if (a->mType == EA_AutoRename)
      r = undoAutoRename(a->mParam, a->mBackup);
    else if ((EA_AnnotateAddLine <= a->mType) && (a->mType <= EA_AnnotateProperty))
      r = undoAnnotateEdit(a->mParam, a->mBackup);
    else if ((EA_ImageEffect <= a->mType) && (a->mType <= EA_PasteImage))
      /*
      EA_PasteImage,
      EA_ImageEffect,
      EA_Rotate,
      EA_Resize,
      EA_Crop,
      EA_PModeEditImage, // not-used
      EA_Monochrome,
      EA_ColorBalance,
      EA_ColorLevel,
      EA_ColorBright,
      EA_ColorHLS,
      EA_ColorHSV,
      EA_ColorReverse,
      EA_SelectRectangle,
      EA_SelectTriangle,
      EA_SelectRhombus,
      EA_SelectPentagon,
      EA_SelectStar,
      EA_SelectEllipse,
      EA_SelectFill,
      EA_CopyRectangle,
      EA_CopyTriangle,
      EA_CopyRhombus,
      EA_CopyPentagon,
      EA_CopyStar,
      EA_CopyEllipse,
      EA_PasteImage,
      */
      r = undoImageData(a->mParam, a->mBackup);

    if (r != 0)
      SetModifiedFlag();
    mActionManager.Dump();
    return r;
  }
  return 0;
}

BOOL CVivaImagingDoc::CanUndo()
{
  return mActionManager.CanUndo();
}

int CVivaImagingDoc::undoDeletePage(HANDLE param, HANDLE backup)
{
  if (backup != NULL)
  {
    // It should be GlobalAlloc memory
    InsertPageData(backup, ON_ORIGIN_ORDER);
  }
  return 1;
}

int CVivaImagingDoc::undoInsertPage(HANDLE param, HANDLE backup)
{
  if (backup != NULL)
  {
    deletePagesById(backup);
  }
  return 1;
}

int CVivaImagingDoc::undoMovePage(HANDLE param, HANDLE backup)
{
  // restore page order
  int count = KMemorySize(backup) / sizeof(int);
  int* p = (int*)KMemoryLock(backup);
  if (p != NULL)
  {
    int active_pid = *p++;
    count--;

    m_Pages.RestorePageOrdering(p, count);
    KMemoryUnlock(backup);

    if (active_pid > 0)
    {
      KImageBase* p = m_Pages.FindPageByID(active_pid);
      if (p == NULL)
        p = m_Pages.GetIndexedPage(0, 0);
      SetActivePage(p);
    }

    return CHANGE_PAGES;
  }
  return 0;
}

int CVivaImagingDoc::undoEditMetaData(HANDLE param, HANDLE backup)
{
  int rtn = 0;
  if (backup != NULL)
  {
    UINT* buff = (UINT*)KMemoryLock(backup);
    UINT active_pid = *buff;
    buff++;

    LPTSTR str = (LPTSTR)buff;
    KImageBase* p = m_Pages.FindPageByID(active_pid);
    if (p != NULL)
    {
      p->RestoreDocExtAttributes(str);
      rtn = CHANGE_CURRENT_PAGE;
    }
    else
    {
      TCHAR msg[120];
      StringCchPrintf(msg, 120, _T("Cannot found target page %d"), active_pid);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    KMemoryUnlock(backup);
  }
  return rtn;
}

int CVivaImagingDoc::undoEditDocType(HANDLE param, HANDLE backup)
{
  int rtn = 0;
  if (backup != NULL)
  {
    if (LoadDocTypeMetaData(backup))
    {
      rtn = CHANGE_CURRENT_PAGE;
    }
    else
    {
      TCHAR msg[] = _T("Cannot load DocTypeMetaData");
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    KMemoryUnlock(backup);
  }
  return rtn;
}

int CVivaImagingDoc::undoEditName(HANDLE param, HANDLE backup)
{
  int rtn = 0;
  if (backup != NULL)
  {
    UINT* buff = (UINT*)KMemoryLock(backup);
    UINT active_pid = *buff;
    buff++;

    LPTSTR str = (LPTSTR)buff;
    KImageBase* p = m_Pages.FindPageByID(active_pid);
    if (p != NULL)
    {
      p->SetName(str);
      rtn = CHANGE_CURRENT_PAGE;
    }
    else
    {
      TCHAR msg[120];
      StringCchPrintf(msg, 120, _T("Cannot found target page %d"), active_pid);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    KMemoryUnlock(backup);
  }
  return rtn;
}
int CVivaImagingDoc::undoAutoRename(HANDLE param, HANDLE backup)
{
  // HANDLE abackup = m_Pages.BackupPageNames();
  if (backup != NULL)
  {
    m_Pages.RestorePageNames(backup);
    return CHANGE_CURRENT_PAGE;
  }
  return 0;
}

int CVivaImagingDoc::undoImageData(HANDLE param, HANDLE backup)
{
  int r = 0;
  if (backup != NULL)
  {
    int count = KMemorySize(backup) / sizeof(EditActionBackupData);
    EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(backup);
    if (ed != NULL)
    {
      for (int i = 0; i < count; i++)
      {
        KImageBase* p = m_Pages.FindPageByID(ed->targetPageId);
        if ((p != NULL) && (p->GetType() == IMAGE_PAGE) && (ed->image != NULL))
        {
          // save current image for redo
          KBitmap* bm = ((KImagePage*)p)->GetImage(mWorkingPath);
          HANDLE h = bm->GetDIB();

          if (((KImagePage*)p)->restoreImageData(ed->image))
          {
            GlobalFree(ed->image); // this is GlobalAlloc
            ed->image = h;
            r = (r == 0) ? CHANGE_CURRENT_VIEW : CHANGE_ALL_VIEW;
            if (p != m_ActivePage)
              r = CHANGE_ALL_VIEW;
          }
          else
          {
            GlobalFree(h);
          }
        }
        else
        {
          TCHAR msg[128];
          StringCchPrintf(msg, 128, _T("Cannot found target page id=%d"), ed->targetPageId);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        ed++;
      }
      KMemoryUnlock(backup);
    }
  }
  return r;
}

int CVivaImagingDoc::undoAnnotateEdit(HANDLE param, HANDLE backup)
{
  int r = 0;
  if (backup != NULL)
  {
    int count = KMemorySize(backup) / sizeof(EditActionBackupData);
    EditActionBackupData* ed = (EditActionBackupData*)KMemoryLock(backup);
    if (ed != NULL)
    {
      for (int i = 0; i < count; i++)
      {
        KImageBase* p = m_Pages.FindPageByID(ed->targetPageId);
        if ((p != NULL) && (p->GetType() == IMAGE_PAGE) && (ed->annotate != NULL))
        {
          // save current image for redo
          HANDLE h = ((KImagePage*)p)->SaveAnnotateData();

          if (((KImagePage*)p)->RestoreAnnotateData(ed->annotate))
          {
            KMemoryFree(ed->annotate);
            ed->annotate = h;
            r = (r == 0) ? CHANGE_CURRENT_VIEW : CHANGE_ALL_VIEW;
            if (m_ActivePage != p)
              r = CHANGE_ALL_VIEW;
          }
          else
          {
            GlobalFree(h);
          }
        }
        else
        {
          TCHAR msg[128];
          StringCchPrintf(msg, 128, _T("Cannot found target page id=%d"), ed->targetPageId);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
        }
        ed++;
      }
      KMemoryUnlock(backup);
    }
  }
  return r;
}

int CVivaImagingDoc::Redo()
{
  KEditAction* a = mActionManager.Redo();
  if (a != NULL)
  {

    TCHAR msg[256];
    a->ToString(msg, 256);
    StringCchCat(msg, 256, _T(" : redo"));
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

    int r = 0;
    if (a->mType == EA_DeletePage)
      r = redoDeletePage(a->mParam, a->mBackup);
    else if (a->mType == EA_InsertPage)
      r = redoInsertPage(a->mParam, a->mBackup);
    else if (a->mType == EA_MovePage)
      r = redoMovePage(a->mParam, a->mBackup);
    else if (a->mType == EA_ModifyMetaData)
      r = redoEditMetaData(a->mParam, a->mBackup);
    else if (a->mType == EA_ModifyDocType)
      r = redoEditDocType(a->mParam, a->mBackup);
    else if (a->mType == EA_ModifyName)
      r = redoEditName(a->mParam, a->mBackup);
    else if (a->mType == EA_AutoRename)
      r = redoAutoRename(a->mParam, a->mBackup);
    else if ((EA_AnnotateAddLine <= a->mType) && (a->mType <= EA_AnnotateProperty))
      r = undoAnnotateEdit(a->mParam, a->mBackup);
    else if ((EA_ImageEffect <= a->mType) && (a->mType <= EA_PasteImage))
    {
      // same actio with undo
      r = undoImageData(a->mParam, a->mBackup);
    }

    if (r != 0)
      SetModifiedFlag();
    return r;
  }
  return 0;
}

BOOL CVivaImagingDoc::CanRedo()
{
  return mActionManager.CanRedo();
}

int CVivaImagingDoc::redoDeletePage(HANDLE param, HANDLE backup)
{
  if (param != NULL)
  {
    deletePagesById(param);
  }
  return 1;
}

int CVivaImagingDoc::redoInsertPage(HANDLE param, HANDLE backup)
{
  if (param != NULL)
  {
    LPTSTR p = (LPTSTR)KMemoryLock(param);
    if (p != NULL)
    {
      LPCTSTR deviceName = p;
      p += lstrlen(p) + 1;

      KIntVector pageList;
      AddImagePages(p, &pageList, 0, deviceName);

      // update page IDs
      int numOfPages = KMemorySize(backup) / sizeof(int);
      int* pPages = (int*)KMemoryLock(backup);
      int i = 0;
      ASSERT(numOfPages == (int)pageList.size());

      KintIterator it = pageList.begin();
      while (it != pageList.end())
      {
        pPages[i++] = *(it++);
      }
      KMemoryUnlock(backup);
    }
    KMemoryUnlock(param);

    // InsertPageData(backup, ON_ORIGIN_ORDER);
    return CHANGE_PAGES;
  }
  return 0;
}

int CVivaImagingDoc::redoMovePage(HANDLE param, HANDLE backup)
{
  int* p = (int*)KMemoryLock(param);
  if ((p != NULL) && (*p == EA_MovePage))
  {
    KVoidPtrArray pages;

    int targetPageIndex = p[1];
    InsertTargetType inside = (InsertTargetType)p[2];
    KImageBase* target_page = (p[3] > 0) ? m_Pages.FindPageByID(p[3]) : NULL;
    int count = p[4];
    p += 5;
    for (int i = 0; i < count; i++)
    {
      int pid = *p++;
      KImageBase* m = m_Pages.DetachPageById(pid);
      if (m != NULL)
      {
        pages.push_back(m);
      }
      else
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Cannot found page id=%d"), pid);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
    }
    KMemoryUnlock(param);
    InsertPages(targetPageIndex, inside, target_page, pages);
    return CHANGE_PAGES;
  }
  return 0;
}

int CVivaImagingDoc::redoEditMetaData(HANDLE param, HANDLE backup)
{
  int rtn = 0;

  if (param != NULL)
  {
    UINT* buff = (UINT*)KMemoryLock(param);
    if (buff != NULL)
    {
      UINT active_pid = *buff;
      KImageBase* page = m_Pages.FindPageByID(active_pid);
      buff++;

      if (page != NULL)
      {
        LPTSTR key = (LPTSTR)buff;
        if (key != NULL)
        {
          LPTSTR value = StrChr(key, DA_VALUE_SEPARATOR);
          if (value != NULL)
          {
            int len = value - key;
            value++;
            LPTSTR keystr = AllocString(key, len);
            SetMetaData(page, keystr, value);
            delete[] keystr;
            rtn = CHANGE_CURRENT_PAGE;
          }
        }
      }
      else
      {
        TCHAR msg[120];
        StringCchPrintf(msg, 120, _T("Cannot found target page %d"), active_pid);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      KMemoryUnlock(param);
    }
  }
  return rtn;
}

int CVivaImagingDoc::redoEditDocType(HANDLE param, HANDLE backup)
{
  int rtn = 0;
  if (param != NULL)
  {
    KEcmDocTypeInfo docTypeInfos;

    if (docTypeInfos.FromHandle(param))
    {
      mDocTypeInfo = docTypeInfos;

      // CHECK : 각 이미지 페이지 ExtAttribute clear
      m_Pages.ClearMetaData(TRUE);
      rtn = CHANGE_CURRENT_PAGE;
    }
  }
  return rtn;
}

int CVivaImagingDoc::redoEditName(HANDLE param, HANDLE backup)
{
  int rtn = 0;
  if ((backup != NULL) && (param != NULL))
  {
    UINT* buff = (UINT*)KMemoryLock(backup);
    UINT active_pid = *buff;
    KMemoryUnlock(backup);

    KImageBase* page = m_Pages.FindPageByID(active_pid);

    if (page != NULL)
    {
      LPTSTR name = (LPTSTR)KMemoryLock(param);
      page->SetName(name);
      rtn = CHANGE_CURRENT_PAGE;
      KMemoryUnlock(param);
    }
    else
    {
      TCHAR msg[120];
      StringCchPrintf(msg, 120, _T("Cannot found target page %d"), active_pid);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
  }
  return rtn;
}

int CVivaImagingDoc::redoAutoRename(HANDLE param, HANDLE backup)
{
  int index = 1;
  int base = 1;

  m_Pages.ImageAutoRename(mWorkingPath, index, base);
  return CHANGE_CURRENT_PAGE;
}

int CVivaImagingDoc::deletePagesById(HANDLE param)
{
  int active_page_index = m_Pages.GetIndexOfPage(m_ActivePage, EXCLUDE_FOLDED);
  int numOfPages = KMemorySize(param) / sizeof(int);
  int* pPages = (int*)KMemoryLock(param);
  KIntVector pageList;
  for (int i = 0; i < numOfPages; i++)
    pageList.push_back(pPages[i]);

  int r = m_Pages.DeletePagesById(pageList);
  if (r > 0)
  {
    // adjust active page
    //m_ActivePage = NULL;
    int total_index = GetNumberOfPages(EXCLUDE_FOLDED);
    if (active_page_index >= total_index)
      active_page_index = total_index - 1;
    if (active_page_index < 0)
      active_page_index = 0;
    KImageBase* p = GetImagePage(EXCLUDE_FOLDED, active_page_index);
    if (p != NULL)
      p->SetSelected(TRUE);
    SetActivePage(p);
    SetModifiedFlag();
  }
  return r;
}

BOOL CVivaImagingDoc::UpdateRenderImage(KImagePage* p, ImageEffectJob* effect, KBitmap* cacheImage)
{
  int page_index = m_Pages.GetIndexOfPage(p, 0);
  if (page_index >= 0)
  {
    return p->SetImageEffectCache(effect, cacheImage);
  }
  return FALSE;
}
