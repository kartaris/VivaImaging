// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlColorLevel.h"
// KPropertyCtrlColorLevel

IMPLEMENT_DYNAMIC(KPropertyCtrlColorLevel, CWnd)

KPropertyCtrlColorLevel::KPropertyCtrlColorLevel()
  :KPropertyCtrlBase()
{
  mSeparate = 0;
  mChannels = 0;
  mApplyAll = 0;
  mpHistogramBuffer = NULL;
}

KPropertyCtrlColorLevel::KPropertyCtrlColorLevel(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL, rect, parent, 101);
}

KPropertyCtrlColorLevel::~KPropertyCtrlColorLevel()
{
  if (mpHistogramBuffer != NULL)
  {
    delete[] mpHistogramBuffer;
    mpHistogramBuffer = NULL;
  }
}


BEGIN_MESSAGE_MAP(KPropertyCtrlColorLevel, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_MESSAGE(NOL_CHANGED, OnNOLChanged)
END_MESSAGE_MAP()



// KPropertyCtrlColorLevel 메시지 처리기

int KPropertyCtrlColorLevel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH); //  PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(50);
  int width2 = DialogX2Pixel(100);
  int x2 = xm * 2 + width1;
  //int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(16);
  int dy = ym;

  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_COMMON_APPLY_CHANNELS, mResourceLanguage);
  mCommonApplyChannels.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
    CRect(xm, dy, right, dy + height), this, 301);
  dy += height + ym / 2;

  str.LoadString(AfxGetInstanceHandle(), IDS_SEPARATE_APPLY_CHANNELS, mResourceLanguage);
  mSeparateApplyChannels.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 302);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_CHANNELS, mResourceLanguage);
  str += _T(" :");
  mLabel1.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(xm, dy, xm + width1, dy + height), this, 303);

  mChannelCombo.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
    CRect(x2, dy, x2 + width2, dy + height), this, 304);

  static int channel_name[3] = { IDS_CHANNEL_BLUE, IDS_CHANNEL_GREEN, IDS_CHANNEL_RED };
  for (int i = 0; i < 3; i++)
  {
    str.LoadString(AfxGetInstanceHandle(), channel_name [i], mResourceLanguage);
    mChannelCombo.AddString(str);
  }

  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_INPUT_LEVEL, mResourceLanguage);
  str += _T(" :");
  mLabel2.Create(str, WS_CHILD | WS_VISIBLE,
    CRect(xm, dy, right, dy + height), this, 305);
  dy += height + 1;

  height = DialogY2Pixel(76);
  mInputCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(xm, dy, right, dy + height), this, 306, IMAGE_INPUT_LEVEL);
  dy += height + ym;

  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  str.LoadString(AfxGetInstanceHandle(), IDS_OUTPUT_LEVEL, mResourceLanguage);
  str += _T(" :");
  mLabel3.Create(str, WS_CHILD | WS_VISIBLE,
    CRect(xm, dy, right, dy + height), this, 307);
  dy += height + 1;

  height = DialogY2Pixel(46);
  mOutputCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(xm, dy, right, dy + height), this, 308, 0);
  dy += height + ym * 2;

  str.LoadString(AfxGetInstanceHandle(), IDS_PREVIEW, mResourceLanguage);
  mPreview.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 104);
  dy += height + ym;

  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_ALL, mResourceLanguage);
  mAll.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 101);
  dy += height + ym;

  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  right = DialogX2Pixel(PC_X_MARGIN + PC_APPLY_WIDTH);

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_NOW, mResourceLanguage);
  mApply.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(xm, dy, right, dy + height), this, 102);

  int r2 = DialogX2Pixel(PC_APPLY_WIDTH / 2);
  str.LoadString(AfxGetInstanceHandle(), IDS_RESET, mResourceLanguage);
  mReset.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(xm + right, dy, xm + right + r2, dy + height), this, 103);

  mMinHeight = dy + height + ym;

  OnApplyMethod(mSeparate);
  mChannelCombo.SetCurSel(mChannels);
  mPreview.SetCheck(BST_CHECKED);
  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  if (gpUiFont != NULL)
  {
    mCommonApplyChannels.SetFont(gpUiFont);
    mSeparateApplyChannels.SetFont(gpUiFont);
    mLabel1.SetFont(gpUiFont);
    mChannelCombo.SetFont(gpUiFont);

    mLabel2.SetFont(gpUiFont);
    mInputCtrl.SetFont(gpUiFont);
    mLabel3.SetFont(gpUiFont);
    mOutputCtrl.SetFont(gpUiFont);

    mPreview.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
    mReset.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlColorLevel::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlColorLevel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlColorLevel::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 103)
      ResetProprety();
    else if (ctrl_id == 104)
      UpdatePreview();
    else if (ctrl_id == 102)
      ApplyEffect();

    else if ((ctrl_id == 301) || (ctrl_id == 302))
    {
      mSeparate = (ctrl_id == 302);
      mChannelCombo.EnableWindow(mSeparate);
      UpdateCurrentLevelValues();
      UpdatePreview();
    }
    return 1;
  }
  else if ((notify_code == CBN_SELCHANGE) && (ctrl_id == 304))
  {
    int ch = mChannelCombo.GetCurSel();
    if (ch != mChannels)
    {
      mChannels = ch;
      UpdateCurrentLevelValues();
    }
  }
  return 0;
}

LRESULT KPropertyCtrlColorLevel::OnNOLChanged(WPARAM wParam, LPARAM lParam)
{
  BYTE* p;

  if (mSeparate)
  {
    p = &mLevelParams[mChannels * 5];
  }
  else
  {
    p = mLevelParams;
  }
  mInputCtrl.GetCurrentValue(p, p + 1, p + 2);
  mOutputCtrl.GetCurrentValue(p + 3, p + 4, NULL);

  if (mSeparate == 0)
  {
    memcpy(mLevelParams + 5, mLevelParams, 5);
    memcpy(mLevelParams + 10, mLevelParams, 5);
  }
  UpdatePreview();
  return 0;
}

void KPropertyCtrlColorLevel::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlColorLevel::OnApplyMethod(BOOL bSeparately)
{
  mSeparateApplyChannels.SetCheck(bSeparately ? BST_CHECKED : BST_UNCHECKED);
  mCommonApplyChannels.SetCheck(bSeparately ? BST_UNCHECKED : BST_CHECKED);

  mChannelCombo.EnableWindow(bSeparately);
}

void KPropertyCtrlColorLevel::InitProperty(CVivaImagingDoc* doc)
{
  if (mpHistogramBuffer == NULL)
    mpHistogramBuffer = new BYTE[256 * 4];
  memset(mpHistogramBuffer, 0, 256 * 4);

  KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    KBitmap* b = ((KImagePage *)p)->GetImage(doc->GetWorkingPath());
    if (b != NULL)
    {
      // int cm = (mSeparate == 0) ? 0x07 : (0x01 << mChannels);
      b->GetHistogram(mpHistogramBuffer, 0x07);
      b->GetHistogram(mpHistogramBuffer+256, 0x01);
      b->GetHistogram(mpHistogramBuffer + 512, 0x02);
      b->GetHistogram(mpHistogramBuffer + 768, 0x04);
    }
  }

  ResetProprety();
}

void KPropertyCtrlColorLevel::ResetProprety()
{
  for (int c = 0; c < 3; c++)
  {
    mLevelParams[5 * c] = 0;
    mLevelParams[5 * c + 1] = 128;
    mLevelParams[5 * c + 2] = 255;
    mLevelParams[5 * c + 3] = 0;
    mLevelParams[5 * c + 4] = 255;
  }
  mSeparate = 0;
  mChannels = 0;
  OnApplyMethod(mSeparate);
  UpdateCurrentLevelValues();
  UpdatePreview();
}

void KPropertyCtrlColorLevel::UpdateCurrentLevelValues()
{
  int c = (mSeparate == 0) ? 0 : mChannels;
  mInputCtrl.SetCurrentValue(mLevelParams[5 * c], mLevelParams[5 * c+1], mLevelParams[5 * c+2]);
  mOutputCtrl.SetCurrentValue(mLevelParams[5 * c+3], mLevelParams[5 * c + 4], 0);

  int offset = 0;
  if (mSeparate)
    offset = (mChannels + 1) * 256;
  memcpy(mInputCtrl.GetHistogramBuffer(), mpHistogramBuffer + offset, 256);
}


void KPropertyCtrlColorLevel::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.channel = 0x07;
  effect.job = PropertyMode::PModeColorLevel;
  if (mSeparate == 0)
  {
    BYTE* d = (BYTE*)effect.param;
    for (int c = 0; c < 3; c++)
      memcpy(&d[c * 5], mLevelParams, 5);
  }
  else
  {
    memcpy(effect.param, mLevelParams, 15);
  }
  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlColorLevel::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);
}

void KPropertyCtrlColorLevel::UpdatePreview()
{
  ImageEffectJob effect;
  ImageEffectJob* pEffect = NULL;

  if (mPreview.GetCheck() == BST_CHECKED)
  {
    readyEffectJob(effect);
    pEffect = &effect;
  }

  KPropertyCtrlBase::UpdatePreview(&effect);
}
