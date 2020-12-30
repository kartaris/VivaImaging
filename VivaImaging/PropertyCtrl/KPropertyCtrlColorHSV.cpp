// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlColorHSV.h"
// KPropertyCtrlColorHSV

IMPLEMENT_DYNAMIC(KPropertyCtrlColorHSV, CWnd)

KPropertyCtrlColorHSV::KPropertyCtrlColorHSV()
  :KPropertyCtrlBase()
{
  mHueValue = 0;
  mSaturationValue = 0;
  mValueValue = 0;
  mApplyAll = 0;
}

KPropertyCtrlColorHSV::KPropertyCtrlColorHSV(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlColorHSV::~KPropertyCtrlColorHSV()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlColorHSV, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()



// KPropertyCtrlColorHSV 메시지 처리기

int KPropertyCtrlColorHSV::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

  str.LoadString(AfxGetInstanceHandle(), IDS_HUE, mResourceLanguage);
  str += _T(" :");
  mLabelHue.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 302);
  mSliderHue.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 303);
  mEditHue.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 304);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_SATURATION, mResourceLanguage);
  str += _T(" :");
  mLabelSaturation.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 305);
  mSliderSaturation.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 306);
  mEditSaturation.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 307);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_VALUE, mResourceLanguage);
  str += _T(" :");
  mLabelValue.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy + yo, xm + width1, dy + height), this, 308);
  mSliderValue.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
    CRect(x2, dy, x2 + width2, dy + height), this, 309);
  mEditValue.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 310);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_PREVIEW, mResourceLanguage);
  mPreview.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 104);
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

  mSliderHue.SetRange(0, 360);
  mSliderSaturation.SetRange(0, 200);
  mSliderValue.SetRange(0, 200);
  UpdateControlValue();

  if (gpUiFont != NULL)
  {
    mLabelHue.SetFont(gpUiFont);
    mEditHue.SetFont(gpUiFont);
    mLabelSaturation.SetFont(gpUiFont);
    mEditSaturation.SetFont(gpUiFont);
    mLabelValue.SetFont(gpUiFont);
    mEditValue.SetFont(gpUiFont);

    mPreview.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlColorHSV::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlColorHSV::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlColorHSV::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == EN_CHANGE)
  {
    CString str;

    if (ctrl_id == 304)
    {
      mEditHue.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (mHueValue != val))
      {
        mHueValue = val;
        mSliderHue.SetPos(180 + mHueValue);
        UpdatePreview();
      }
    }
    else if (ctrl_id == 307)
    {
      mEditSaturation.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (mSaturationValue != val))
      {
        mSaturationValue = val;
        mSliderSaturation.SetPos(100 + mSaturationValue);
        UpdatePreview();
      }
    }
    else if (ctrl_id == 310)
    {
      mEditValue.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (mValueValue != val))
      {
        mValueValue = val;
        mSliderValue.SetPos(100 + mValueValue);
        UpdatePreview();
      }
    }
    return 1;
  }
  else if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 104)
      UpdatePreview();
    else if (ctrl_id == 102)
      ApplyEffect();
  }
  return 0;
}

void KPropertyCtrlColorHSV::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if (pScrollBar != NULL)
  {
    int id = pScrollBar->GetDlgCtrlID();

    int pos = ((CSliderCtrl*)pScrollBar)->GetPos();
    CString str;

    if (id == 303)
    {
      int v = pos - 180;
      if (v != mHueValue)
      {
        mHueValue = v;
        str.Format(_T("%d"), mHueValue);
        mEditHue.SetWindowText(str);
        UpdatePreview();
      }
    }
    else if (id == 306)
    {
      int v = pos - 100;
      if (v != mSaturationValue)
      {
        mSaturationValue = v;
        str.Format(_T("%d"), mSaturationValue);
        mEditSaturation.SetWindowText(str);
        UpdatePreview();
      }
    }
    else if (id == 309)
    {
      int v = pos - 100;
      if (v != mValueValue)
      {
        mValueValue = pos - 100;
        str.Format(_T("%d"), mValueValue);
        mEditValue.SetWindowText(str);
        UpdatePreview();
      }
    }
  }
}

void KPropertyCtrlColorHSV::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}


void KPropertyCtrlColorHSV::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeColorHSV;
  effect.param[0] = mHueValue;
  effect.param[1] = mSaturationValue;
  effect.param[2] = mValueValue;

  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlColorHSV::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);

  mHueValue = 0;
  mSaturationValue = 0;
  mValueValue = 0;
  UpdateControlValue();
  UpdatePreview();
}

void KPropertyCtrlColorHSV::UpdatePreview()
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

void KPropertyCtrlColorHSV::UpdateControlValue()
{
  mSliderHue.SetPos(180 + mHueValue);
  mSliderSaturation.SetPos(100 + mSaturationValue);
  mSliderValue.SetPos(100 + mValueValue);

  CString str;
  str.Format(_T("%d"), mHueValue);
  mEditHue.SetWindowText(str);
  str.Format(_T("%d"), mValueValue);
  mEditValue.SetWindowText(str);
  str.Format(_T("%d"), mSaturationValue);
  mEditSaturation.SetWindowText(str);
  mPreview.SetCheck(BST_CHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);
}
