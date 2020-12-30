// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlMonochrome.h"
// KPropertyCtrlMonochrome

static float SliderValue2Gamma(int value)
{
  float g = 1.0;
  if (value < 50)
  {
    g = (float)(0.1 + 0.9 * value / 50);
  }
  else if (50 < value)
  {
    g = (float)(1.0 + 4.0 * (value - 50) / 50);
  }
  return g;
}

static int GammaValue2Slider(float g)
{
  int value = 50;
  if (g < 1.0)
  {
    value = (int)((g - 0.1) * 50 / 0.9);
  }
  else if (g > 1.0)
  {
    value = (int)((g - 1.0) * 50 / 4.0) + 50;
  }
  return value;
}

IMPLEMENT_DYNAMIC(KPropertyCtrlMonochrome, CWnd)

KPropertyCtrlMonochrome::KPropertyCtrlMonochrome()
  :KPropertyCtrlBase()
{
  mGammaValue = 50;
  mGrayscale = -1;
  mApplyAll = 0;
}

KPropertyCtrlMonochrome::KPropertyCtrlMonochrome(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  mGammaValue = 50;
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlMonochrome::~KPropertyCtrlMonochrome()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlMonochrome, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_WM_HSCROLL()
END_MESSAGE_MAP()



// KPropertyCtrlMonochrome 메시지 처리기

int KPropertyCtrlMonochrome::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(50);
  int width2 = DialogX2Pixel(80);
  int x2 = xm + width1 + DialogX2Pixel(3);
  int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(30);
  int yo = DialogY2Pixel(2);
  int dy = ym;

  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_NORMAL, mResourceLanguage);
  mNone.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
    CRect(xm, dy, right, dy + height), this, 201);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_BLACK_AND_WHITE, mResourceLanguage);
  mBlackAndWhiteCheck.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 202);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_GRAYSCALE, mResourceLanguage);
  mGrayscaleCheck.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 203);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_GAMMA, mResourceLanguage);
  str += _T(" :");
  mLabel1.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT | WS_GROUP,
    CRect(xm, dy + yo, xm+width1, dy + height), this, 302);
  mGammaSlider.Create(WS_CHILD | WS_VISIBLE | TBS_BOTTOM,
    CRect(x2, dy, x2 + width2, dy + height), this, 303);
  mGammaEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x3, dy, x3 + width3, dy + height), this, 304);
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

  mGammaSlider.SetRange(0, 100);

  InitProperty(NULL);

  mPreview.SetCheck(BST_CHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  UpdatePreview();

  if (gpUiFont != NULL)
  {
    mNone.SetFont(gpUiFont);
    mBlackAndWhiteCheck.SetFont(gpUiFont);
    mGrayscaleCheck.SetFont(gpUiFont);
    mLabel1.SetFont(gpUiFont);
    mGammaEdit.SetFont(gpUiFont);

    mPreview.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlMonochrome::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlMonochrome::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlMonochrome::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if ((201 <= ctrl_id) && (ctrl_id <= 203))
    {
      if (mNone.GetCheck() == BST_CHECKED)
        mGrayscale = -1;
      else
        mGrayscale = (mGrayscaleCheck.GetCheck() == BST_CHECKED) ? 1 : 0;

      SetEnableGrayscale(mGrayscale);
      UpdatePreview();
    }
    else if (ctrl_id == 102) // apply
    {
      ApplyEffect();
    }
    else if (ctrl_id == 104) // preview
    {
      UpdatePreview();
    }
  }
  if (notify_code == EN_CHANGE)
  {
    CString str;

    if (ctrl_id == 304)
    {
      CString str;
      mGammaEdit.GetWindowText(str);
      float val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%f"), &val) > 0))
      {
        mGammaValue = GammaValue2Slider(val);
        mGammaSlider.SetPos(mGammaValue);
        UpdatePreview();
      }
      return 1;
    }
  }
  return KPropertyCtrlBase::OnCommand(wParam, lParam);
}

void KPropertyCtrlMonochrome::SetEnableGrayscale(BOOL enable)
{
  /* always enable
  mGammaSlider.EnableWindow(enable);
  mGammaEdit.EnableWindow(enable);
  */
}

void KPropertyCtrlMonochrome::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if ((pScrollBar != NULL) && (pScrollBar->GetSafeHwnd() == mGammaSlider.GetSafeHwnd()))
  {
    int pos = mGammaSlider.GetPos();
    if (pos != mGammaValue)
    {
      mGammaValue = pos;

      float fValue = SliderValue2Gamma(mGammaValue);
      CString str;
      str.Format(_T("%.2f"), fValue);
      mGammaEdit.SetWindowText(str);

      UpdatePreview();
    }
  }
}

void KPropertyCtrlMonochrome::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlMonochrome::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  if (mGrayscale >= 0)
  {
    effect.job = PropertyMode::PModeMonochrome;
    effect.param[0] = mGrayscale;
    // gamma should 10 ~ 500, 100 is base-value
    effect.param[1] = (int)(SliderValue2Gamma(mGammaValue) * 100);
    mApplyAll = (mAll.GetCheck() == BST_CHECKED);
    effect.applyAll = mApplyAll;
  }
}

void KPropertyCtrlMonochrome::UpdatePreview()
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

void KPropertyCtrlMonochrome::InitProperty(CVivaImagingDoc* doc)
{
  mGammaValue = 50;
  mGrayscale = -1;
  mApplyAll = 0;

  mGammaSlider.SetPos(mGammaValue);
  CString str;
  str.Format(_T("%.2f"), SliderValue2Gamma(mGammaValue));
  mGammaEdit.SetWindowText(str);

  mNone.SetCheck((mGrayscale == -1) ? BST_CHECKED : BST_UNCHECKED);
  mBlackAndWhiteCheck.SetCheck((mGrayscale == 0) ? BST_CHECKED : BST_UNCHECKED);
  mGrayscaleCheck.SetCheck((mGrayscale == 1) ? BST_CHECKED : BST_UNCHECKED);

  SetEnableGrayscale(mGrayscale);
}

void KPropertyCtrlMonochrome::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);

  InitProperty(NULL);
  UpdatePreview();
}
