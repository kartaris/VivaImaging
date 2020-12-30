// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlColorBalance.h"
// KPropertyCtrlColorBalance

IMPLEMENT_DYNAMIC(KPropertyCtrlColorBalance, CWnd)

KPropertyCtrlColorBalance::KPropertyCtrlColorBalance()
  :KPropertyCtrlBase()
{
  mRedValue = 0;
  mGreenValue = 0;
  mBlueValue = 0;
  mApplyAll = 0;
}

KPropertyCtrlColorBalance::KPropertyCtrlColorBalance(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL, rect, parent, 101);
}

KPropertyCtrlColorBalance::~KPropertyCtrlColorBalance()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlColorBalance, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()



// KPropertyCtrlColorBalance 메시지 처리기

int KPropertyCtrlColorBalance::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN+ PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(50);
  int width2 = DialogX2Pixel(80);
  int x2 = xm + width1 + DialogX2Pixel(3);
  int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(30);
  int yo = DialogY2Pixel(2);
  int dy = ym;

  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_RED, mResourceLanguage);
  str += _T(" :");
  mLabelRed.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy+yo, xm + width1, dy + height), this, 302);
  mSliderRed.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 303);
  mEditRed.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 304);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_GREEN, mResourceLanguage);
  str += _T(" :");
  mLabelGreen.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 305);
  mSliderGreen.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 306);
  mEditGreen.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 307);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_BLUE, mResourceLanguage);
  str += _T(" :");
  mLabelBlue.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 308);
  mSliderBlue.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 309);
  mEditBlue.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 310);
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

  mSliderRed.SetRange(0, 200);
  mSliderGreen.SetRange(0, 200);
  mSliderBlue.SetRange(0, 200);
  UpdateControlValue();

  mPreview.SetCheck(BST_CHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  if (gpUiFont != NULL)
  {
    mLabelRed.SetFont(gpUiFont);
    mEditRed.SetFont(gpUiFont);
    mLabelGreen.SetFont(gpUiFont);
    mEditGreen.SetFont(gpUiFont);
    mLabelBlue.SetFont(gpUiFont);
    mEditBlue.SetFont(gpUiFont);

    mPreview.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  UpdatePreview();
  return 0;
}

void KPropertyCtrlColorBalance::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlColorBalance::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlColorBalance::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == EN_CHANGE)
  {
    CString str;

    if (ctrl_id == 304)
    {
      mEditRed.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mRedValue = val;
        mSliderRed.SetPos(100+ mRedValue);
        UpdatePreview();
      }
    }
    else if (ctrl_id == 307)
    {
      mEditGreen.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mGreenValue = val;
        mSliderGreen.SetPos(100 + mGreenValue);
        UpdatePreview();
      }
    }
    else if (ctrl_id == 310)
    {
      mEditBlue.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        mBlueValue = val;
        mSliderBlue.SetPos(100 + mBlueValue);
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


void KPropertyCtrlColorBalance::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if (pScrollBar != NULL)
  {
    int id = pScrollBar->GetDlgCtrlID();

    int pos = ((CSliderCtrl*)pScrollBar)->GetPos();
    CString str;
    str.Format(_T("%d"), pos - 100);

    if (id == 303)
    {
      int v = pos - 100;
      if (mRedValue != v)
      {
        mRedValue = v;
        mEditRed.SetWindowText(str);
        UpdatePreview();
      }
    }
    else if (id == 306)
    {
      int v = pos - 100;
      if (mGreenValue != v)
      {
        mGreenValue = v;
        mEditGreen.SetWindowText(str);
        UpdatePreview();
      }
    }
    else if (id == 309)
    {
      int v = pos - 100;
      if (mBlueValue != v)
      {
        mBlueValue = v;
        mEditBlue.SetWindowText(str);
        UpdatePreview();
      }
    }
  }
}

void KPropertyCtrlColorBalance::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlColorBalance::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeColorBalance;
  effect.param[0] = mRedValue;
  effect.param[1] = mGreenValue;
  effect.param[2] = mBlueValue;

  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlColorBalance::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);

  mRedValue = 0;
  mGreenValue = 0;
  mBlueValue = 0;
  UpdateControlValue();
}

void KPropertyCtrlColorBalance::UpdatePreview()
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

void KPropertyCtrlColorBalance::UpdateControlValue()
{
  mSliderRed.SetPos(100 + mRedValue);
  mSliderGreen.SetPos(100 + mGreenValue);
  mSliderBlue.SetPos(100 + mBlueValue);

  CString str;
  str.Format(_T("%d"), mRedValue);
  mEditRed.SetWindowText(str);
  str.Format(_T("%d"), mGreenValue);
  mEditGreen.SetWindowText(str);
  str.Format(_T("%d"), mBlueValue);
  mEditBlue.SetWindowText(str);
}

void KPropertyCtrlColorBalance::moveCtrlItems(int yoffset)
{
  /*
  int dlgitemids[] = {
    302, 303, 304,
    305, 306, 307,
    308, 309, 310,
    103, 101, 102
  };

  int count = sizeof(dlgitemids) / sizeof(int);
  for (int i = 0; i < count; i++)
  {
    DlgMoveToOffset(this, dlgitemids[i], 0, yoffset, FALSE);
  }
  */

  ScrollWindowEx(0, yoffset, NULL, NULL, NULL, NULL, SW_INVALIDATE|SW_SCROLLCHILDREN);
}