// CDlgImageFormat.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "CDlgImageFormat.h"
#include "afxdialogex.h"

#include "Platform\Graphics.h"

// CDlgImageFormat 대화 상자

IMPLEMENT_DYNAMIC(CDlgImageFormat, CDialogEx)

CDlgImageFormat::CDlgImageFormat(CWnd* pParent /*=nullptr*/, ImagingSaveFormat defaultFormat)
	: CDialogEx(IDD_DLG_IMAGE_FORMAT, pParent)
{
	mUploadFileType = defaultFormat;
}

CDlgImageFormat::~CDlgImageFormat()
{
}

void CDlgImageFormat::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_UPLOAD_FILE_TYPE, mComboUploadFileType);
}


BEGIN_MESSAGE_MAP(CDlgImageFormat, CDialogEx)
END_MESSAGE_MAP()


// CDlgImageFormat 메시지 처리기
BOOL CDlgImageFormat::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  CString str;
  for (int i = 1; i < MAX_SAVE_IMAGE_FORMAT; i++)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_FORMAT_UNKNOWN + i, mResourceLanguage);
    mComboUploadFileType.AddString(str);
  }

  if (mUploadFileType >= FORMAT_ORIGIN)
    mComboUploadFileType.SetCurSel(mUploadFileType - FORMAT_ORIGIN);

  if (gpUiFont != NULL)
  {
    mComboUploadFileType.SetFont(gpUiFont);

    int dlg_ctrl_ids[] = {
      IDC_STATIC_FORMAT,
      IDC_STATIC_IMAGE_TYPE,
      IDOK,
      IDCANCEL
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }
  return TRUE;
}

void CDlgImageFormat::OnOK()
{
  mUploadFileType = (ImagingSaveFormat)(mComboUploadFileType.GetCurSel() + FORMAT_ORIGIN);
  CDialog::OnOK();
}
