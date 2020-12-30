// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlRotate.h"
// KPropertyCtrlRotate

IMPLEMENT_DYNAMIC(KPropertyCtrlRotate, CWnd)

KPropertyCtrlRotate::KPropertyCtrlRotate()
  :KPropertyCtrlBase()
{
  mRotateMode = RotateMode::ROTATE_NONE;
}

KPropertyCtrlRotate::KPropertyCtrlRotate(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlRotate::~KPropertyCtrlRotate()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlRotate, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// KPropertyCtrlRotate 메시지 처리기

int KPropertyCtrlRotate::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
  str.LoadString(AfxGetInstanceHandle(), IDS_ROTATE_90CW, mResourceLanguage);
  mRadioButton1.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 301);
  dy += height + ym;
  str.LoadString(AfxGetInstanceHandle(), IDS_ROTATE_90CCW, mResourceLanguage);
  mRadioButton2.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 302);
  dy += height + ym;

  str.LoadString(AfxGetInstanceHandle(), IDS_ROTATE_180, mResourceLanguage);
  mRadioButton3.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 303);
  dy += height + ym;
  str.LoadString(AfxGetInstanceHandle(), IDS_FLIP_HORZ, mResourceLanguage);
  mRadioButton4.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 304);
  dy += height + ym;
  str.LoadString(AfxGetInstanceHandle(), IDS_FLIP_VERT, mResourceLanguage);
  mRadioButton5.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
    CRect(xm, dy, right, dy + height), this, 305);
  dy += height + ym * 2;

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_ALL, mResourceLanguage);
  mAll.Create(str, WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 101);
  dy += height + ym;

  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  right = DialogX2Pixel(PC_X_MARGIN+PC_APPLY_WIDTH);

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_NOW, mResourceLanguage);
  mApply.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(xm, dy, right, dy + height), this, 102);

  mMinHeight = dy + height + ym;

  if (mRotateMode == RotateMode::ROTATE_90CW)
    mRadioButton1.SetCheck(1);
  else if (mRotateMode == RotateMode::ROTATE_90CCW)
    mRadioButton2.SetCheck(1);
  else if (mRotateMode == RotateMode::ROTATE_180)
    mRadioButton3.SetCheck(1);
  else if (mRotateMode == RotateMode::FLIP_HORZ)
    mRadioButton4.SetCheck(1);
  else if (mRotateMode == RotateMode::FLIP_VERT)
    mRadioButton5.SetCheck(1);
  else
    mRadioButton0.SetCheck(1);

  mAll.SetCheck(mApplyAll ? BST_CHECKED : BST_UNCHECKED);

  if (gpUiFont != NULL)
  {
    mRadioButton0.SetFont(gpUiFont);
    mRadioButton1.SetFont(gpUiFont);
    mRadioButton2.SetFont(gpUiFont);
    mRadioButton3.SetFont(gpUiFont);
    mRadioButton4.SetFont(gpUiFont);
    mRadioButton5.SetFont(gpUiFont);

    mAll.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);
  }

  return 0;
}

void KPropertyCtrlRotate::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlRotate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlRotate::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == BN_CLICKED)
  {
    if ((300 <= ctrl_id) && (ctrl_id <= 305))
      mRotateMode = (RotateMode)(ctrl_id - 300);
    else if (ctrl_id == 101)
    {
      mApplyAll = (mAll.GetCheck() == BST_CHECKED) ? 1 : 0;
    }
    else if (ctrl_id == 102) // apply
    {
      ApplyEffect();
    }

    if ((300 <= ctrl_id) && (ctrl_id <= 305))
      UpdatePreview();
  }
  return KPropertyCtrlBase::OnCommand(wParam, lParam);
}

void KPropertyCtrlRotate::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlRotate::InitProperty(CVivaImagingDoc* doc)
{
  KImageBase* p = doc->GetImagePage(EXCLUDE_FOLDED, -1);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    KBitmap* b = ((KImagePage *)p)->GetImage(doc->GetWorkingPath());
    if (b != NULL)
    {
      mRotateMode = RotateMode::ROTATE_NONE;
      for (int id = 300; id < 306; id++)
      {
        CButton* b = (CButton*)GetDlgItem(id);
        if (b->GetCheck())
          b->SetCheck(0);
      }
      mRadioButton0.SetCheck(1);
    }
  }
}
void KPropertyCtrlRotate::readyEffectJob(ImageEffectJob& effect)
{
  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeRotate;
  effect.param[0] = (int)mRotateMode;
  mApplyAll = (mAll.GetCheck() == BST_CHECKED);
  effect.applyAll = mApplyAll;
}

void KPropertyCtrlRotate::UpdatePreview()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::UpdatePreview(&effect);
}

void KPropertyCtrlRotate::ApplyEffect()
{
  ImageEffectJob effect;
  readyEffectJob(effect);
  KPropertyCtrlBase::ApplyProperty(&effect);
}