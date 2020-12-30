// KDlgShowMessage.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgShowMessage.h"
#include "afxdialogex.h"

#include "Core\KSettings.h"
#include "Platform\Graphics.h"

// KDlgShowMessage 대화 상자

IMPLEMENT_DYNAMIC(KDlgShowMessage, CDialogEx)

KDlgShowMessage::KDlgShowMessage(CWnd* pParent, CString& label, CString& msg)
	: CDialogEx(IDD_DLG_SHOW_MESSAGES, pParent)
{
  mLabel = label;
  mMessage = msg;
}

KDlgShowMessage::~KDlgShowMessage()
{
}

void KDlgShowMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlgShowMessage, CDialogEx)
END_MESSAGE_MAP()


// KDlgShowMessage 메시지 처리기
BOOL KDlgShowMessage::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  if (mLabel.GetLength() > 0)
  {
    CEdit* edit = (CEdit*)GetDlgItem(IDC_LABEL1);
    if (edit != NULL)
      edit->SetWindowText(mLabel);
  }


  if (mMessage.GetLength() > 0)
  {
    CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_MESSAGE);
    if (edit != NULL)
      edit->SetWindowText(mMessage);
  }

  if (gpUiFont != NULL)
  {
    int dlg_ctrl_ids[] = {
      IDC_LABEL1,
      IDC_EDIT_MESSAGE,
      IDOK,
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }
  return TRUE;
}