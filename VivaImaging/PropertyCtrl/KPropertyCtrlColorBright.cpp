// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlColorBright.h"
// KPropertyCtrlColorBright

IMPLEMENT_DYNAMIC(KPropertyCtrlColorBright, CWnd)

KPropertyCtrlColorBright::KPropertyCtrlColorBright()
  :KPropertyCtrlBase()
{
  mBrightValue = 0;
  mContrastValue = 0;
  mApplyAll = 0;
}

KPropertyCtrlColorBright::KPropertyCtrlColorBright(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlColorBright::~KPropertyCtrlColorBright()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlColorBright, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()



// KPropertyCtrlColorBright 메시지 처리기

int KPropertyCtrlColorBright::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(50);
  int width2 = DialogX2Pixel(80);
  int x2 = xm + width1 + DialogX2Pixel(3);
  int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(30);
  int yo = DialogY2Pixel(2);
  int dy = ym;

  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_BRIGHT, mResourceLanguage);
  str += _T(" :");
  mLabelBright.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 302);
  mSliderBright.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 303);
  mEditBright.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 304);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_CONTRAST, mResourceLanguage);
  str += _T(" :");
  mLabelContrast.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 305);
  mSliderContrast.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 306);
  mEditContrast.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 307);

  dy += height + ym * 2;

  str.LoadString(AfxGetInstanceHandle(), IDS_PREVIEW, mResourceLanguage);
  mPreview.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 103);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_ALL, mResourceLanguage);
  mAll.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 101);
  dy += height + ym;

  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  right = DialogX2Pixel(PC_X_MARGIN + PC_APPLY_WIDTH);

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_NOW, mResourceLanguage);
  mApply.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(xm, dy, right, dy + height), this, 102);

  mMinHeight = dy + height + ym;

  mSliderBright.SetRange(0, 200);
  mSliderContrast.SetRange(0, 200);
  UpdateControlValue();

  mPreview.SetCheck(BST_CHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  if (gpUiFont != NULL)
  {
    mLabelBright.SetFont(gpUiFont);
    mEditBright.SetFont(gpUiFont);
    mLabelContrast.SetFont(gpUiFont);
    mEditContrast.SetFont(gpUiFont);

    mPreview.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlColorBright::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlColorBright::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlColorBright::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == EN_CHANGE)
  {
    CString str;

    if (ctrl_id == 304)
    {
      mEditBright.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mBrightValue = val;
        mSliderBright.SetPos(100 + mBrightValue);
        UpdatePreview();
      }
    }
    else if (ctrl_id == 307)
    {
      mEditContrast.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mContrastValue = val;
        mSliderContrast.SetPos(100 + mContrastValue);
        UpdatePreview();
      }
    }
    return 1;
  }
  else if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 103)
      UpdatePreview();
    else if (ctrl_id == 102)
      ApplyEffect();
  }
  return 0;
}

void KPropertyCtrlColorBright::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if (pScrollBar != NULL)
  {
    int id = pScrollBar->GetDlgCtrlID();

    int pos = ((CSliderCtrl*)pScrollBar)->GetPos();
    CString str;
    str.Format(_T("%d"), pos - 100);

    if (id == 303)
    {
      mBrightValue = pos - 100;
      mEditBright.SetWindowText(str);
      UpdatePreview();
    }
    else if (id == 306)
    {
      mContrastValue = pos - 100;
      mEditContrast.SetWindowText(str);
      UpdatePreview();
    }
  }
}

void KPropertyCtrlColorBright::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlColorBright::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeColorBright;
  effect.param[0] = mBrightValue;
  effect.param[1] = mContrastValue;

  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlColorBright::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);

  // reset parameters
  mBrightValue = 0;
  mContrastValue = 0;
  UpdateControlValue();
}

void KPropertyCtrlColorBright::UpdatePreview()
{
  ImageEffectJob effect;
  ImageEffectJob* pEffect = NULL;

  if (mPreview.GetCheck() == BST_CHECKED)
  {
    readyEffectJob(effect);
    pEffect = &effect;
  }

  KPropertyCtrlBase::UpdatePreview(pEffect);
}

void KPropertyCtrlColorBright::UpdateControlValue()
{
  mSliderBright.SetPos(100 + mBrightValue);
  mSliderContrast.SetPos(100 + mContrastValue);

  CString str;
  str.Format(_T("%d"), mBrightValue);
  mEditBright.SetWindowText(str);
  str.Format(_T("%d"), mContrastValue);
  mEditContrast.SetWindowText(str);
}
