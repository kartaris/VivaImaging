// KDlgViewZoom.cpp: 구현 파일
//

#include "stdafx.h"
#include "VivaImaging.h"
#include "KDlgViewZoom.h"
#include "afxdialogex.h"
#include "Platform\Graphics.h"
#include "Core\KConstant.h"
#include "Resource.h"

// KDlgViewZoom 대화 상자

IMPLEMENT_DYNAMIC(KDlgViewZoom, CDialogEx)

KDlgViewZoom::KDlgViewZoom(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_ZOOM, pParent)
  , mRatio(0)
{
  mZoom = 0.0;
}

KDlgViewZoom::~KDlgViewZoom()
{
}

void KDlgViewZoom::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIO_25, mRatio);
  DDX_Control(pDX, IDC_CHECK_FITTO, mFitTo);
  DDX_Control(pDX, IDC_EDIT_ZOOM, mEditZoom);
  DDX_Control(pDX, IDC_SLIDER_ZOOM, mSliderZoom);
}


BEGIN_MESSAGE_MAP(KDlgViewZoom, CDialogEx)
  ON_WM_HSCROLL()
END_MESSAGE_MAP()


// KDlgViewZoom 메시지 처리기
BOOL KDlgViewZoom::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  if (gpUiFont != NULL)
  {
    int dlg_ctrl_ids[] = {
      IDC_RADIO_25,
      IDC_RADIO_33,
      IDC_RADIO_50,
      IDC_RADIO_75,
      IDC_RADIO_100,
      IDC_RADIO_150,
      IDC_RADIO_200,
      IDC_RADIO_300,
      IDC_RADIO_400,
      IDC_RADIO_500,
      IDC_RADIO_800,
      IDC_STATIC,
      IDC_EDIT_ZOOM,
      IDC_SLIDER_ZOOM,
      IDC_CHECK_FITTO,
      IDOK,
      IDCANCEL
    };

    SetDlgItemFont(this, dlg_ctrl_ids, sizeof(dlg_ctrl_ids) / sizeof(int), gpUiFont);
  }


  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_ZOOM, mResourceLanguage);
  SetWindowText(str);

  str.LoadString(AfxGetInstanceHandle(), IDS_ZOOM_LABEL, mResourceLanguage);
  CWnd *ctrl = (CWnd *)GetDlgItem(IDC_STATIC);
  ctrl->SetWindowText(str);

  str.LoadString(AfxGetInstanceHandle(), IDS_FIT_TO_WINDOW, mResourceLanguage);
  ctrl = (CWnd *)GetDlgItem(IDC_CHECK_FITTO);
  ctrl->SetWindowText(str);

  str.LoadString(AfxGetInstanceHandle(), IDS_OK, mResourceLanguage);
  ctrl = (CWnd *)GetDlgItem(IDOK);
  ctrl->SetWindowText(str);

  str.LoadString(AfxGetInstanceHandle(), IDS_CANCEL, mResourceLanguage);
  ctrl = (CWnd *)GetDlgItem(IDCANCEL);
  ctrl->SetWindowText(str);

  // 1 ~ 100 : N percent
  // 100 ~ 200 : (N - 100) * 10;
  mSliderZoom.SetRange(1, 200);
  SetZoom(mZoom, mbFitTo);

  return TRUE;
}

static float zoom_ratio[] = {
  (float) 0.25,
  (float) 0.33,
  (float) 0.50,
  (float) 0.75,
  (float) 1.0,
  (float) 1.5,
  (float) 2.0,
  (float) 3.0,
  (float) 4.0,
  (float) 5.0,
  (float) 8.0
};

BOOL KDlgViewZoom::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if ((IDC_RADIO_25 <= ctrl_id) && (ctrl_id <= IDC_RADIO_800))
    {
      SetZoom(zoom_ratio[ctrl_id - IDC_RADIO_25], FALSE);
      //return 1;
    }
    else if (IDC_CHECK_FITTO == ctrl_id)
    {
      mbFitTo = mFitTo.GetCheck() == BST_CHECKED;
      SetZoom(mZoom, mbFitTo);
    }
  }
  return CDialogEx::OnCommand(wParam, lParam);
}

void KDlgViewZoom::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if (pScrollBar != NULL)
  {
    int id = pScrollBar->GetDlgCtrlID();
    if (id == IDC_SLIDER_ZOOM)
    {
      int val = ((CSliderCtrl*)pScrollBar)->GetPos();
      if (val < 100)
        SetZoom((float)val / 100, FALSE);
      else if (val > 100)
        SetZoom((float)(val - 100) / 10 + (float)1.0, FALSE);
      else
        SetZoom(1.0, FALSE);
    }
  }
}

void KDlgViewZoom::SetZoom(float zoom, BOOL bFitTo)
{
  int nz = (int)(zoom * 100 + 0.5);

  TCHAR str[16];
  StringCchPrintf(str, 16, _T("%d"), nz);
  mEditZoom.SetWindowText(str);

  int radio_id = -1;
  for (int i = 0; i < 11; i++)
  {
    if (nz == (int)(zoom_ratio[i] * 100))
    {
      radio_id = i;
      break;
    }
  }
  if (radio_id >= 0)
  {
    if (mRatio != radio_id)
    {
      CButton *btn = (CButton*)GetDlgItem(IDC_RADIO_25 + mRatio);
      btn->SetCheck(BST_UNCHECKED);

      btn = (CButton*)GetDlgItem(IDC_RADIO_25 + radio_id);
      if (btn->GetCheck() != BST_CHECKED)
        btn->SetCheck(BST_CHECKED);
      mRatio = radio_id;
    }
  }

  BOOL checked = (mFitTo.GetCheck() == BST_CHECKED);
  if (bFitTo != checked)
  {
    mFitTo.SetCheck(bFitTo ? BST_CHECKED : BST_UNCHECKED);
  }

  checked = mSliderZoom.IsWindowEnabled();
  if (checked == bFitTo)
  {
    CButton *btn;
    BOOL bRatio = (zoom != 0.0);
    for (int i = 0; i < 11; i++)
    {
      btn = (CButton*)GetDlgItem(IDC_RADIO_25 + i);
      btn->EnableWindow(!bFitTo);
    }
    btn = (CButton*)GetDlgItem(IDC_EDIT_ZOOM);
    btn->EnableWindow(!bFitTo);

    mSliderZoom.EnableWindow(!bFitTo);
  }

  int pos = 100;
  if (zoom < 1.0)
    pos = (int)(zoom * 100 + 0.5);
  else if (zoom > 1.0)
    pos = (int)((zoom - (float)1.0) * 10 + 0.5) + 100;
  else
    pos = 100;

  if ((pos > 0) && (mSliderZoom.GetPos() != pos))
    mSliderZoom.SetPos(pos);
  mZoom = zoom;
}

float KDlgViewZoom::GetZoom()
{
  return (mbFitTo ? (float)0.0 : mZoom);
}

void KDlgViewZoom::OnOK()
{
  CDialogEx::OnOK();
}
