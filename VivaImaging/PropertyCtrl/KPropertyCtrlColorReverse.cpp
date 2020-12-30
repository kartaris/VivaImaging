// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlColorReverse.h"
// KPropertyCtrlColorReverse

IMPLEMENT_DYNAMIC(KPropertyCtrlColorReverse, CWnd)

KPropertyCtrlColorReverse::KPropertyCtrlColorReverse()
  :KPropertyCtrlBase()
{
  mReverseMode = 0;
  mApplyAll = 0;
}

KPropertyCtrlColorReverse::KPropertyCtrlColorReverse(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlColorReverse::~KPropertyCtrlColorReverse()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlColorReverse, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// KPropertyCtrlColorReverse 메시지 처리기

int KPropertyCtrlColorReverse::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int dy = ym;
  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_NORMAL, mResourceLanguage);
  mRadioButton0.Create(str, WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 300);
  dy += height + ym;
  str.LoadString(AfxGetInstanceHandle(), IDS_COLOR_REVERSE, mResourceLanguage);
  mRadioButton1.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 301);
  dy += height + ym;
  str.LoadString(AfxGetInstanceHandle(), IDS_VALUE_REVERSE, mResourceLanguage);
  mRadioButton2.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 302);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_LIGHTNESS_REVERSE, mResourceLanguage);
  mRadioButton3.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 303);

  dy += height + ym * 2;

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

  if (mReverseMode == 1)
    mRadioButton1.SetCheck(1);
  else if (mReverseMode == 2)
    mRadioButton2.SetCheck(1);
  else if (mReverseMode == 3)
    mRadioButton3.SetCheck(1);
  else
    mRadioButton0.SetCheck(1);

  mPreview.SetCheck(BST_CHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  UpdatePreview();

  if (gpUiFont != NULL)
  {
    mRadioButton0.SetFont(gpUiFont);
    mRadioButton1.SetFont(gpUiFont);
    mRadioButton2.SetFont(gpUiFont);
    mRadioButton3.SetFont(gpUiFont);

    mPreview.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlColorReverse::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlColorReverse::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlColorReverse::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 300)
      mReverseMode = 0;
    else if (ctrl_id == 301)
      mReverseMode = 1;
    else if (ctrl_id == 302)
      mReverseMode = 2;
    else if (ctrl_id == 303)
      mReverseMode = 3;
    else if (ctrl_id == 101)
    {
      mApplyAll = (mAll.GetCheck() == BST_CHECKED) ? 1 : 0;
    }
    else if (ctrl_id == 102)
    {
      ApplyEffect();
    }
    else if (ctrl_id == 104)
    {
      UpdatePreview();
    }

    if ((300 <= ctrl_id) && (ctrl_id <= 303))
      UpdatePreview();

    return 1;
  }
  return KPropertyCtrlBase::OnCommand(wParam, lParam);
}

void KPropertyCtrlColorReverse::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlColorReverse::InitProperty(CVivaImagingDoc* doc)
{
  mReverseMode = 0;
  mRadioButton0.SetCheck(1);
  mRadioButton1.SetCheck(0);
  mRadioButton2.SetCheck(0);
  mRadioButton3.SetCheck(0);
}

void KPropertyCtrlColorReverse::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeColorReverse;
  effect.param[0] = mReverseMode;
  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlColorReverse::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);

  InitProperty(NULL);
  UpdatePreview();
}

void KPropertyCtrlColorReverse::UpdatePreview()
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
