// KDlgUploadAs.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgUploadAs.h"
#include "afxdialogex.h"

#include "Document\KImageBase.h"
#include "Core\KSyncCoreEngine.h"
#include "Core\KConstant.h"
#include "Platform\Graphics.h"

#include <strsafe.h>

// KDlgUploadAs 대화 상자

IMPLEMENT_DYNAMIC(KDlgUploadAs, CDialogEx)

KDlgUploadAs::KDlgUploadAs(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_UPLOAD_AS, pParent)
{
  mFormat = FORMAT_UNKNOWN;
  mServerFolderOID[0] = '\0';
  mFlag = 0;
}

KDlgUploadAs::~KDlgUploadAs()
{
}

void KDlgUploadAs::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LOCAL_WORKING_PATH, mEditLocalWorkingPath);
  DDX_Control(pDX, IDC_COMBO_UPLOAD_FILE_TYPE, mComboUploadFileType);
  DDX_Control(pDX, IDC_COMBO_FOLDER_OPTION, mComboUploadFolderOption);
  DDX_Control(pDX, IDC_FILENAME, mEditFilename);
  DDX_Control(pDX, IDC_SERVER_WORKING_PATH, mEditServerWorkingPath);
}


BEGIN_MESSAGE_MAP(KDlgUploadAs, CDialogEx)
  ON_BN_CLICKED(IDC_SELECT_SERVER_FOLDER, OnBnClickedSelectServerFolder)
  ON_MESSAGE(THREADED_CALL_RESULT, OnThreadedCallMessage)
  ON_CBN_SELENDOK(IDC_COMBO_UPLOAD_FILE_TYPE, OnCbnSelendokComboUploadFileType)
END_MESSAGE_MAP()


// KDlgUploadAs 메시지 처리기
BOOL KDlgUploadAs::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  mEditLocalWorkingPath.SetWindowText(mLocalWorkingPath);
  mEditFilename.SetWindowText(mFilename);
  mEditServerWorkingPath.SetWindowText(mServerWorkingPath);

  CString str;

  if (mResourceLanguage != 0x409)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_UPLOAD_TO_SERVER, mResourceLanguage);
    SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_LOCAL_WORKING_FOLDER, mResourceLanguage);
    GetDlgItem(IDC_STATIC_LOCAL_FOLDER)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_UPLOAD_IMAGE_FORMAT, mResourceLanguage);
    GetDlgItem(IDC_STATIC_FORMAT)->SetWindowText(str);
    
    str.LoadString(AfxGetInstanceHandle(), IDS_FILENAME, mResourceLanguage);
    GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(str);
    
    str.LoadString(AfxGetInstanceHandle(), IDS_SERVER_FOLDER, mResourceLanguage);
    GetDlgItem(IDC_STATIC_SERVER_FOLDER)->SetWindowText(str);

    str.LoadString(AfxGetInstanceHandle(), IDS_CHANGE, mResourceLanguage);
    GetDlgItem(IDC_SELECT_SERVER_FOLDER)->SetWindowText(str);
    
    str.LoadString(AfxGetInstanceHandle(), IDS_UPLOAD_OPTION, mResourceLanguage);
    GetDlgItem(IDC_STATIC_OPTION)->SetWindowText(str);
    
  }

  for (int i = 1; i < MAX_SAVE_IMAGE_FORMAT; i++)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_FORMAT_ORIGIN_COMMENT + i - 1, mResourceLanguage);
    mComboUploadFileType.AddString(str);
  }

  if (mFormat > FORMAT_UNKNOWN)
    mComboUploadFileType.SetCurSel(mFormat - 1);

  for (int i = 0; i < 4; i++)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_UPLOAD_FOLDER_NEW_VERSION + i, mResourceLanguage);
    mComboUploadFolderOption.AddString(str);
  }
  int idx = 0;
  if (mFlag & GO_CLEARFOLDER_BEFOREUPLOAD)
    idx = 1;
  mComboUploadFolderOption.SetCurSel(idx);

  if (gpUiFont != NULL)
  {
    int dlg_ctrl_ids[] = {
      IDC_STATIC_LOCAL_FOLDER,
      IDC_LOCAL_WORKING_PATH,
      IDC_STATIC_SERVER_FOLDER,
      IDC_SERVER_WORKING_PATH,
      IDC_STATIC_FORMAT,
      IDC_COMBO_UPLOAD_FILE_TYPE,
      IDC_STATIC_OPTION,
      IDC_COMBO_FOLDER_OPTION,
      IDC_STATIC_FILENAME,
      IDC_FILENAME,
      IDC_SELECT_SERVER_FOLDER,
      IDOK,
      IDCANCEL
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }
  return TRUE;
}

extern KSyncCoreEngine* gpSyncCoreEngine;

void KDlgUploadAs::SetUploadInfos(LPCTSTR serverFolderOID, LPCTSTR localWorkingPath, LPCTSTR name, ImagingSaveFormat format, UINT flag)
{
  mFormat = format;
  if (serverFolderOID != NULL)
  {
    StringCchCopy(mServerFolderOID, MAX_OID, serverFolderOID);

    TCHAR serverFolderPath[384];
    if (gpSyncCoreEngine->SyncServerGetFolderName(mServerFolderOID, serverFolderPath, TRUE, NULL))
      mServerWorkingPath.SetString(serverFolderPath);
  }
  mFilename.SetString(name);
  mLocalWorkingPath.SetString(localWorkingPath);
  mFlag = flag;
}

void KDlgUploadAs::OnBnClickedSelectServerFolder()
{
  if (gpSyncCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), NULL, (LPCTSTR)mServerFolderOID))
  {
    TCHAR serverFolderPath[384];

    if (gpSyncCoreEngine->SyncServerGetFolderName(mServerFolderOID, serverFolderPath, TRUE, NULL))
    {
      mServerWorkingPath.SetString(serverFolderPath);
      mEditServerWorkingPath.SetWindowText(mServerWorkingPath);
    }
  }
}

LRESULT KDlgUploadAs::OnThreadedCallMessage(WPARAM wParam, LPARAM lParam)
{
  if (wParam == SELECT_FOLDER)
  {
    THREAD_CALL_DATA *cd = (THREAD_CALL_DATA *)lParam;

    if (cd != NULL)
    {
      StringCchCopy(mServerFolderOID, MAX_OID, cd->folder_oid);

      TCHAR serverFolderPath[384];
      // TCHAR serverFolderIndex[KMAX_PATH];

      if (gpSyncCoreEngine->SyncServerGetFolderName(cd->folder_oid, serverFolderPath, TRUE, NULL))
      {
        mServerWorkingPath.SetString(serverFolderPath);
        mEditServerWorkingPath.SetWindowText(mServerWorkingPath);
      }
    }

    if (lParam != NULL)
      free((void *)lParam);
  }
  return 0;
}

void KDlgUploadAs::OnOK()
{
  if (mServerWorkingPath.GetLength() == 0)
  {
    AfxMessageBox(IDS_SELECT_SERVER_PATH);
    return;
  }
  mEditFilename.GetWindowText(mFilename);
  mFormat = (ImagingSaveFormat)(mComboUploadFileType.GetCurSel() + FORMAT_ORIGIN);
  int folder_option = mComboUploadFolderOption.GetCurSel();
  if (folder_option == 1)
    mFlag = GO_CLEARFOLDER_BEFOREUPLOAD;
  else
    mFlag = GO_UPLOADAS_NEWVERSION;

  CDialogEx::OnOK();
}

void KDlgUploadAs::OnCbnSelendokComboUploadFileType()
{
  mFormat = (ImagingSaveFormat)(mComboUploadFileType.GetCurSel() + FORMAT_ORIGIN);
  mEditFilename.GetWindowText(mFilename);

  LPTSTR ext = StrRChr(mFilename, NULL, '.');
  if (ext != NULL)
  {
    ext++;
    LPCTSTR defExt = GetDocumentFileExt(mFormat);
    if (lstrcmpi(ext, defExt) != 0)
    {
      int len = ext - mFilename.GetBuffer();
      mFilename = mFilename.Left(len) + defExt;
      mEditFilename.SetWindowText(mFilename);
    }
  }

}
