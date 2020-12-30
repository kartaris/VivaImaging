// KDlgSetDocType.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgSetDocType.h"
#include "afxdialogex.h"

#include "Document/KImageBase.h"
#include "Core/KSyncCoreEngine.h"
#include "Core/KEcmDocType.h"
#include "Platform/Graphics.h"

#include "Core/DestinyLocalSyncLib/common/DestinyLocalSyncIFShared.h"


// KDlgSetDocType 대화 상자

IMPLEMENT_DYNAMIC(KDlgSetDocType, CDialogEx)

KDlgSetDocType::KDlgSetDocType(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SET_DOCTYPE, pParent)
{
  mServerFolderOID[0] = '\0';
  mDocTypeOid[0] = '\0';
}

KDlgSetDocType::~KDlgSetDocType()
{
  DocTypeInfoVectorIterator it = mDocTypeInfoVector.begin();
  while (it != mDocTypeInfoVector.end())
  {
    KEcmDocTypeInfo* pInfo = *(it++);
    delete pInfo;
  }
  mDocTypeInfoVector.clear();
  mAvailableDocTypeVector.clear();
}

void KDlgSetDocType::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_DOCTYPE, mAvailableDocTypes);
  DDX_Control(pDX, IDC_LIST_METADATA, mMetadataList);
  DDX_Control(pDX, IDC_EDIT_SERVER_FOLDER, mServerFolderPathEdit);
}


BEGIN_MESSAGE_MAP(KDlgSetDocType, CDialogEx)
  ON_MESSAGE(THREADED_CALL_RESULT, OnThreadedCallMessage)
  ON_BN_CLICKED(IDC_SELECT_SERVER_FOLDER, OnBnClickedSelectServerFolder)
  ON_LBN_SELCHANGE(IDC_LIST_DOCTYPE, &KDlgSetDocType::OnLbnSelchangeListDoctype)
END_MESSAGE_MAP()

extern KSyncCoreEngine* gpSyncCoreEngine;

// KDlgSetDocType 메시지 처리기

BOOL KDlgSetDocType::OnInitDialog()
{
  CDialogEx::OnInitDialog();

#ifdef DOCTYPE_FOR_GLOBAL
  UpdateGlobalDocType();
#else
  if (lstrlen(mServerFolderOID) > 0)
    UpdateAvaiableDocType();

  if (lstrlen(mDocTypeOid) > 0)
  {
    int index = 0;
    DocTypeVectorIterator it = mAvailableDocTypeVector.begin();
    while (it != mAvailableDocTypeVector.end())
    {
      StringStringPair p = *(it++);
      if (lstrcmp(mDocTypeOid, p.first.c_str()) == 0)
      {
        mAvailableDocTypes.SetCurSel(index);
        break;
      }
      index++;
    }
  }
#endif

  if (mResourceLanguage != 0x409)
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_SELECT_DOC_TYPE, mResourceLanguage);
    SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_AVAILABLE_DOCTYPE, mResourceLanguage);
    GetDlgItem(IDC_STATIC_AVAILABLE_DOCTYPE)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_DOCTYPE_INFO, mResourceLanguage);
    GetDlgItem(IDC_STATIC_DOCTYPE_INFO)->SetWindowText(str);
  }

  if (gpUiFont != NULL)
  {
    int dlg_ctrl_ids[] = {
      IDC_STATIC_AVAILABLE_DOCTYPE,
      IDC_STATIC_DOCTYPE_INFO,
      IDC_LIST_DOCTYPE,
      IDC_LIST_METADATA,
      IDOK,
      IDCANCEL
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }

  if (lstrlen(mDocTypeOid) > 0)
  {
    int index = OnSelectDocTypeOid(mDocTypeOid);
    if (index >= 0)
      mAvailableDocTypes.SetCurSel(index);
  }

  return TRUE;
}

void KDlgSetDocType::OnBnClickedSelectServerFolder()
{
  gpSyncCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), NULL, (LPCTSTR)mServerFolderOID);
}

LRESULT KDlgSetDocType::OnThreadedCallMessage(WPARAM wParam, LPARAM lParam)
{
  if (wParam == SELECT_FOLDER)
  {
    THREAD_CALL_DATA* cd = (THREAD_CALL_DATA*)lParam;

#ifndef DOCTYPE_FOR_GLOBAL
    if (cd != NULL)
    {
      StringCchCopy(mServerFolderOID, MAX_OID, cd->folder_oid);
      UpdateAvaiableDocType();
    }
#endif

    if (lParam != NULL)
      free((void*)lParam);
  }
  return 0;
}


#ifdef DOCTYPE_FOR_GLOBAL

void KDlgSetDocType::UpdateGlobalDocType()
{
  XDOCTYPE_ST* xdocType = NULL;
  int count = 0;

  if (gpSyncCoreEngine->SyncServerGetDocTypeDatasForAll(&xdocType, &count) != R_SUCCESS)
  {
    OutputDebugString(_T("Failure on SyncServerGetDocTypeDatasForAll\n"));
    return;
  }

  mAvailableDocTypes.ResetContent();
  mAvailableDocTypeVector.clear();

  if (count > 0)
  {
    for (int i = 0; i < count; i++)
    {
      if (xdocType[i].extAttribute == NULL)
        continue;

      std::wstring oid;
      std::wstring name;
      TCHAR msg[128];

      StringCchPrintf(msg, 128, _T("SelectedDocType : name=%s, oid=%s\n"), xdocType[i].name, xdocType[i].oid);
      OutputDebugString(msg);

      oid.assign(xdocType[i].oid);
      name.assign(xdocType[i].name);
      mAvailableDocTypeVector.push_back(std::make_pair(oid, name));

      KEcmDocTypeInfo* docTypeInfo = new KEcmDocTypeInfo();
      FromXDocTypeSt(*docTypeInfo, &xdocType[i]);
      mDocTypeInfoVector.push_back(docTypeInfo);

      // add to listbox
      mAvailableDocTypes.AddString(xdocType[i].name);
    }

    gpSyncCoreEngine->SyncServerFreeDocTypeData(xdocType, count);
  }
}

void KDlgSetDocType::FromXDocTypeSt(KEcmDocTypeInfo& docTypeInfo, void* docType)
{
  XDOCTYPE_ST* xdocType = (XDOCTYPE_ST * )docType;

  docTypeInfo.Clear();
  docTypeInfo.SetDocTypeInfo(_T(""), xdocType->oid, xdocType->name);

  int colCount = xdocType->extAttribute->extColumnsCNT;
  DestinyMsgpack_LSIF::XEXTCOLUMN_** extColumns = xdocType->extAttribute->extColumns;
  for (int i = 0; i < colCount; i++)
  {
    if (!extColumns[i]->flagVisible)
      continue;

    UINT maxLength = 0;
    TCHAR msg[128];

#ifdef _DEBUG
    StringCchPrintf(msg, 128, _T("Column : %s(%s, %s)(min=%s,max=%s)\n"),
      extColumns[i]->name,
      extColumns[i]->colType, extColumns[i]->dbColName,
      extColumns[i]->minInputValue, extColumns[i]->maxInputValue);
#else
    StringCchPrintf(msg, 128, _T("%s(%s)"),
      extColumns[i]->name,
      extColumns[i]->colType);
#endif
    OutputDebugString(msg);

    // mMetadataList.AddString(msg);

    if ((extColumns[i]->maxInputValue != NULL) && (lstrlen(extColumns[i]->maxInputValue) > 0))
    {
      swscanf_s(extColumns[i]->maxInputValue, _T("%d"), &maxLength);
    }

    EcmExtDocTypeColumnType type = KEcmDocTypeInfo::TypeFromString((LPCTSTR)extColumns[i]->colType);
    docTypeInfo.Add(type, (LPCTSTR)extColumns[i]->name, (LPCTSTR)extColumns[i]->dbColName, maxLength);
  }
}

#else

void KDlgSetDocType::UpdateAvaiableDocType()
{
  TCHAR serverFolderPath[384];

  if (gpSyncCoreEngine->SyncServerGetFolderName(mServerFolderOID, serverFolderPath, TRUE, NULL))
  {
    mServerWorkingPath.SetString(serverFolderPath);
    mServerFolderPathEdit.SetWindowText(mServerWorkingPath);

    LPTSTR docTypes = NULL;
    if ((gpSyncCoreEngine->SyncServerGetDocTypesForFolder(mServerFolderOID, &docTypes) == R_SUCCESS) &&
      (docTypes != NULL))
    {
      UpdateDocTypes(docTypes);
      delete[] docTypes;
    }
  }
}

void KDlgSetDocType::UpdateDocTypes(LPCTSTR docTypes)
{
  mAvailableDocTypes.ResetContent();
  mAvailableDocTypeVector.clear();

  while (docTypes != NULL)
  {
    LPCTSTR n;
    LPCTSTR e = StrChr(docTypes, ',');

    if (e != NULL)
    {
      n = e + 1;
      int len = e - docTypes;
      LPTSTR doctype = AllocString(docTypes, len);
      AddDocType(doctype);
      delete[] doctype;
    }
    else
    {
      n = NULL;
      AddDocType(docTypes);
    }

    docTypes = n;
  }
}

void KDlgSetDocType::AddDocType(LPCTSTR docTypeOidName)
{
  LPCTSTR snd = StrChr(docTypeOidName, '|');
  if (snd != NULL)
  {
    std::wstring oid;
    std::wstring name;

    oid.assign((LPWSTR)docTypeOidName, (size_t)(snd - docTypeOidName));
    snd++;
    name.assign((LPWSTR)snd, lstrlen(snd));

    mAvailableDocTypeVector.push_back(std::make_pair(oid, name));

    mAvailableDocTypes.AddString(snd);
  }
  else
  {
    OutputDebugString(_T("bad docType Oid '|' Name"));
    OutputDebugString(docTypeOidName);
  }
}

#endif

int KDlgSetDocType::OnSelectDocTypeOid(LPCTSTR selectedDocTypeOid)
{
  int index = -1;
#ifdef DOCTYPE_FOR_GLOBAL
  mMetadataList.ResetContent();
  mDocTypeInfo.Clear();
  int i = 0;

  DocTypeInfoVectorIterator it = mDocTypeInfoVector.begin();
  while (it != mDocTypeInfoVector.end())
  {
    KEcmDocTypeInfo* pInfo = *(it++);

    if (StrCmp(pInfo->mDocTypeOid, selectedDocTypeOid) == 0)
    {
      StringCchCopy(mDocTypeOid, MAX_OID, pInfo->mDocTypeOid);
      index = i++;

      // pInfo->GetColumnNumber();
      mDocTypeInfo.SetDocTypeInfo(mServerFolderOID, mDocTypeOid, pInfo->mpDocTypeName);

      KEcmExtDocTypeColumnConstIterator cit = pInfo->mArray.begin();
      while (cit != pInfo->mArray.end())
      {
        EcmExtDocTypeColumn* pColumn = *(cit++);
        TCHAR msg[128];

        StringCchPrintf(msg, 128, _T("%s, %s(type=%s)"),
          pColumn->name, pColumn->key, KEcmDocTypeInfo::GetValueTypeName(pColumn->type));
        // OutputDebugString(msg);

        mMetadataList.AddString(msg);

        // EcmExtDocTypeColumnType type = KEcmDocTypeInfo::TypeFromString((LPCTSTR)pColumn->colType);
        mDocTypeInfo.Add(pColumn->type, pColumn->name, pColumn->key, pColumn->maxValueLength);
      }
      break;
    }
  }
#else
  XDOCTYPE_ST* xdocType = NULL;

  if (gpSyncCoreEngine->SyncServerGetDocTypeData(selectedDocTypeOid, &xdocType) == R_SUCCESS)
  {
    TCHAR msg[128];

    StringCchPrintf(msg, 128, _T("SelectedDocType : name=%s, oid=%s"), xdocType->name, xdocType->oid);
    OutputDebugString(msg);
    StringCchCopy(mDocTypeOid, MAX_OID, xdocType->oid);

    mMetadataList.ResetContent();
    mDocTypeInfo.Clear();

    if ((xdocType != NULL) && (xdocType->extAttribute != NULL))
    {
      mDocTypeInfo.SetDocTypeInfo(mServerFolderOID, mDocTypeOid, xdocType->name);

      int colCount = xdocType->extAttribute->extColumnsCNT;
      DestinyMsgpack_LSIF::XEXTCOLUMN_** extColumns = xdocType->extAttribute->extColumns;
      for (int i = 0; i < colCount; i++)
      {
        if (!extColumns[i]->flagVisible)
          continue;

        UINT maxLength = 0;

#ifdef _DEBUG
        StringCchPrintf(msg, 128, _T("%s(%s, %s)(min=%s,max=%s)"),
          extColumns[i]->name,
          extColumns[i]->colType, extColumns[i]->dbColName,
          extColumns[i]->minInputValue, extColumns[i]->maxInputValue);
#else
        StringCchPrintf(msg, 128, _T("%s(%s)"),
          extColumns[i]->name,
          extColumns[i]->colType);

#endif
        OutputDebugString(msg);

        mMetadataList.AddString(msg);

        if ((extColumns[i]->maxInputValue != NULL) && (lstrlen(extColumns[i]->maxInputValue) > 0))
        {
          swscanf_s(extColumns[i]->maxInputValue, _T("%d"), &maxLength);
        }

        EcmExtDocTypeColumnType type = KEcmDocTypeInfo::TypeFromString((LPCTSTR)extColumns[i]->colType);
        mDocTypeInfo.Add(type, (LPCTSTR)extColumns[i]->name, (LPCTSTR)extColumns[i]->dbColName, maxLength);
      }
    }

    gpSyncCoreEngine->SyncServerFreeDocTypeData(xdocType);
  }
#endif
  return index;
}

void KDlgSetDocType::OnLbnSelchangeListDoctype()
{
  int index = mAvailableDocTypes.GetCurSel();
  if ((index >= 0) && (index < (int)mAvailableDocTypeVector.size()))
  {
    StringStringPair p = mAvailableDocTypeVector.at(index);
    OnSelectDocTypeOid(p.first.c_str());
  }
}
