// CDlgSetting.cpp: 구현 파일
// Copyright (C) 2019 K2Mobile
// All rights reserved.

#include "stdafx.h"
#include "VivaImaging.h"
#include "resource.h"
#include "CDlgSetting.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "VivaImagingDoc.h"
#include "Core\KConstant.h"
#include "Core\KSyncCoreEngine.h"
#include "Platform\Graphics.h"

#include "Document\KImageBase.h"

// CDlgSetting 대화 상자

IMPLEMENT_DYNAMIC(CDlgSetting, CDialogEx)

CDlgSetting::CDlgSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_FILE_SETTING, pParent)
  , mStrWorkingFolder(_T(""))
  , mStrUploadFolder(_T(""))
  , mStrNewFilenameFormat(_T(""))
{
  mUploadFileType = FORMAT_ORIGIN;
  mMergeAnnotatesToImage = FALSE;
  mChangeSelectAfterDraw = FALSE;
}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_WORKING_FOLDER, mEditWorkingFolder);
  DDX_Control(pDX, IDC_EDIT_UPLOAD_FOLDER, mEditUploadFolder);
  DDX_Control(pDX, IDC_EDIT_NEW_FILENAME_FORMAT, mEditNewFilenameFormat);
  DDX_Control(pDX, IDC_COMBO_UPLOAD_FILE_TYPE, mComboUploadFileType);
  DDX_Control(pDX, IDC_CHECK_MERGE_ANNOTATE, mCheckMergeAnnotates);
  DDX_Control(pDX, IDC_CHECK_CHANGE_SELECT_AFTER_DRAW, mCheckChangeSelectAfterDraw);
  DDX_Text(pDX, IDC_EDIT_WORKING_FOLDER, mStrWorkingFolder);
  DDX_Text(pDX, IDC_EDIT_UPLOAD_FOLDER, mStrUploadFolder);
  DDX_Text(pDX, IDC_EDIT_NEW_FILENAME_FORMAT, mStrNewFilenameFormat);
}


BEGIN_MESSAGE_MAP(CDlgSetting, CDialogEx)
  ON_BN_CLICKED(IDC_SELECT_WORKING_FOLDER, &CDlgSetting::OnBnClickedSelectWorkingFolder)
  ON_BN_CLICKED(IDC_SELECT_SERVER_FOLDER, &CDlgSetting::OnBnClickedSelectServerFolder)
  ON_MESSAGE(THREADED_CALL_RESULT, &CDlgSetting::OnThreadedCallMessage)
END_MESSAGE_MAP()


// CDlgSetting 메시지 처리기
BOOL CDlgSetting::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  CString str;
  for (int i = 1; i < MAX_SAVE_IMAGE_FORMAT; i++)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_FORMAT_UNKNOWN + i, mResourceLanguage);
    mComboUploadFileType.AddString(str);
  }

  if (mResourceLanguage != 0x409)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_SETTING_TITLE, mResourceLanguage);
    SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_LOCAL_WORKING_FOLDER, mResourceLanguage);
    GetDlgItem(IDC_STATIC_LOCAL_FOLDER)->SetWindowText(str);
    
    str.LoadString(AfxGetInstanceHandle(), IDS_SERVER_UPLOAD_FOLDER, mResourceLanguage);
    GetDlgItem(IDC_STATIC_SERVER_FOLDER)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_FILENAME_FORMAT, mResourceLanguage);
    GetDlgItem(IDC_STATIC_FORMAT)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_IMAGE_FORMAT, mResourceLanguage);
    GetDlgItem(IDC_STATIC_IMAGE_TYPE)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_MERGE_ANNOTATE, mResourceLanguage);
    GetDlgItem(IDC_CHECK_MERGE_ANNOTATE)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_FILENAME_FORMAT_HELP, mResourceLanguage);
    GetDlgItem(IDC_STATIC_FILENAME_FORMAT_HELP)->SetWindowText(str);
  }

  if (mStrWorkingFolder.GetLength() == 0)
  {
    LPTSTR path = CVivaImagingDoc::GetWorkingBaseFolder();
    mStrWorkingFolder.SetString(path);
  }
  mEditWorkingFolder.SetWindowText(mStrWorkingFolder);

  mEditUploadFolder.SetWindowText(mStrUploadFolder);

  mEditNewFilenameFormat.SetWindowTextW(mStrNewFilenameFormat);

  if (mUploadFileType >= FORMAT_ORIGIN)
    mComboUploadFileType.SetCurSel(mUploadFileType - FORMAT_ORIGIN);

  mCheckMergeAnnotates.SetCheck(mMergeAnnotatesToImage ? BST_CHECKED : 0);

  mCheckChangeSelectAfterDraw.SetCheck(mChangeSelectAfterDraw ? BST_CHECKED : 0);

  if (gpUiFont != NULL)
  {
    mComboUploadFileType.SetFont(gpUiFont);
    mEditWorkingFolder.SetFont(gpUiFont);
    mEditUploadFolder.SetFont(gpUiFont);
    mCheckMergeAnnotates.SetFont(gpUiFont);
    mCheckChangeSelectAfterDraw.SetFont(gpUiFont);
    mEditNewFilenameFormat.SetFont(gpUiFont);

    int dlg_ctrl_ids[] = {
      IDC_STATIC_LOCAL_FOLDER,
      IDC_STATIC_SERVER_FOLDER,
      IDC_STATIC_FORMAT,
      IDC_STATIC_IMAGE_TYPE,
      IDC_SELECT_WORKING_FOLDER,
      IDC_SELECT_SERVER_FOLDER,
      IDOK,
      IDCANCEL 
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }

  return TRUE;
}

void CDlgSetting::OnBnClickedSelectWorkingFolder()
{
  BROWSEINFO bi;
  TCHAR folderPath[MAX_PATH];
  CString title;

  mEditWorkingFolder.GetWindowText(folderPath, MAX_PATH);
  title.LoadString(AfxGetInstanceHandle(), IDS_WORKING_FOLDER_TITLE, mResourceLanguage);

  memset(&bi, 0, sizeof(BROWSEINFO));
  bi.lpszTitle = title;
  bi.hwndOwner = GetSafeHwnd();
  bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_USENEWUI;
  bi.pszDisplayName = folderPath;
  ITEMIDLIST *pidlBrowse = SHBrowseForFolder(&bi);
  if (pidlBrowse != NULL)
  {
    if (::SHGetPathFromIDList(pidlBrowse, folderPath))
    {
      mEditWorkingFolder.SetWindowText(folderPath);
    }
  }
}

extern KSyncCoreEngine* gpSyncCoreEngine;

void CDlgSetting::OnBnClickedSelectServerFolder()
{
  TCHAR folderOID[MAX_OID] = _T("");
  if (mStrUploadFolderOID.GetLength() > 0)
    StringCchCopy(folderOID, MAX_OID, mStrUploadFolderOID);

  if (gpSyncCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), NULL, (LPCTSTR)folderOID))
  {
    mStrUploadFolderOID.SetString(folderOID);
    TCHAR serverFolderPath[384];
    TCHAR serverFolderIndex[KMAX_PATH];

    if (gpSyncCoreEngine->SyncServerGetFolderName(folderOID, serverFolderPath, TRUE, serverFolderIndex))
      mStrUploadFolder.SetString(serverFolderPath);
  }
}

void CDlgSetting::OnOK()
{
  mEditWorkingFolder.GetWindowText(mStrWorkingFolder);
  // CVivaImagingDoc::SetWorkingBaseFolder(folderPath);

  // mStrUploadFolder
  //mEditUploadFolder.SetWindowText(mStrUploadFolder);

  mEditNewFilenameFormat.GetWindowTextW(mStrNewFilenameFormat);

  mUploadFileType = (ImagingSaveFormat)(mComboUploadFileType.GetCurSel() + FORMAT_ORIGIN);
  mMergeAnnotatesToImage = (mCheckMergeAnnotates.GetCheck() == BST_CHECKED);
  mChangeSelectAfterDraw = (mCheckChangeSelectAfterDraw.GetCheck() == BST_CHECKED);

  CDialog::OnOK();
}

LRESULT CDlgSetting::OnThreadedCallMessage(WPARAM wParam, LPARAM lParam)
{
  if (wParam == SELECT_FOLDER)
  {
    THREAD_CALL_DATA *cd = (THREAD_CALL_DATA *)lParam;
    if (cd != NULL)
    {
      mStrUploadFolderOID.SetString(cd->folder_oid);

      TCHAR serverFolderPath[384];
      TCHAR serverFolderIndex[KMAX_PATH];

      if (gpSyncCoreEngine->SyncServerGetFolderName(cd->folder_oid, serverFolderPath, TRUE, serverFolderIndex))
        mStrUploadFolder.SetString(serverFolderPath);

      free((void *)lParam);
    }
  }
  return 0;
}
