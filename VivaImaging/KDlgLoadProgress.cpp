// KDlgLoadProgress.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgLoadProgress.h"
#include "afxdialogex.h"
#include "Core\KSettings.h"
#include "Platform\Graphics.h"
// KDlgLoadProgress 대화 상자

IMPLEMENT_DYNAMIC(KDlgLoadProgress, CDialogEx)

KDlgLoadProgress::KDlgLoadProgress(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_LOAD_PROGRESS, pParent)
{
  Create(IDD_DLG_LOAD_PROGRESS, pParent);
}

KDlgLoadProgress::~KDlgLoadProgress()
{
}

void KDlgLoadProgress::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PROGRESS1, mProgress);
}


BEGIN_MESSAGE_MAP(KDlgLoadProgress, CDialogEx)
END_MESSAGE_MAP()

BOOL KDlgLoadProgress::OnInitDialog()
{
  CDialogEx::OnInitDialog();


  if (gpUiFont != NULL)
  {
    int dlg_ctrl_ids[] = {
      IDC_LABEL1,
      IDC_LABEL2,
      IDCANCEL
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }
  return TRUE;
}

// KDlgLoadProgress 메시지 처리기
void KDlgLoadProgress::SetLabel(int index, LPCTSTR str)
{
  CWnd* p = GetDlgItem((index == 0) ? IDC_LABEL1 : IDC_LABEL2);
  if (p != NULL)
    p->SetWindowText(str);
  else
    OutputDebugString(_T("cannot found label\n"));
}

void KDlgLoadProgress::SetProgress(int doneFolders, int totalFolders, int doneFiles, int totalFiles, HANDLE hError)
{
  CString str;
  CString fmt;

  str.LoadString(IDS_FOLDER_UPLOAD_DONE_TOTAL);
  fmt.Format(str, doneFolders, totalFolders);
  SetLabel(0, fmt);

  str.LoadString(IDS_FILE_UPLOAD_DONE_TOTAL);
  fmt.Format(str, doneFiles, totalFiles);
  SetLabel(1, fmt);

  if (hError != NULL)
  {
    LPTSTR errorStr = (LPTSTR)GlobalLock(hError);
    StoreLogHistory(_T(__FUNCTION__), errorStr, SYNC_MSG_LOG);

    /*
    CWnd* p = GetDlgItem(IDC_EDIT_ERRORSTR);
    CString str;

    p->GetWindowText(str);
    if (str.GetLength() > 0)
      str += _T("\r\n");
    str += errorStr;
    p->SetWindowText(str);
    */

    GlobalUnlock(hError);
  }

  int max = totalFolders + totalFiles;
  int pos = doneFolders + doneFiles;

  mProgress.SetRange(0, max);
  mProgress.SetPos(pos);
}

void KDlgLoadProgress::OnCancel()
{
  CWnd* parent = GetParent();
  if (parent != NULL)
    parent->PostMessage(WMA_CANCEL_PROGRESS, 0, 0);
  CDialogEx::OnCancel();
}
