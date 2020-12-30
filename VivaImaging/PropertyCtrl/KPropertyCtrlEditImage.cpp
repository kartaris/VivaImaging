// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlEditImage.h"
// KPropertyCtrlEditImage

IMPLEMENT_DYNAMIC(KPropertyCtrlEditImage, CWnd)

KPropertyCtrlEditImage::KPropertyCtrlEditImage()
  :KPropertyCtrlBase()
{
}

KPropertyCtrlEditImage::KPropertyCtrlEditImage(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlEditImage::~KPropertyCtrlEditImage()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlEditImage, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_MESSAGE(WMA_CHANGE_COLOR, OnChangeColor)
END_MESSAGE_MAP()



// KPropertyCtrlEditImage 메시지 처리기

int KPropertyCtrlEditImage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int dy = ym;

  CString str;
  str.LoadString(AfxGetInstanceHandle(), IDS_FILL_COLOR, mResourceLanguage);
  mLabel1.Create(str, WS_CHILD | WS_VISIBLE,
    CRect(xm, dy, right, dy + height), this, 301);
  dy += height + ym;

  height = DialogY2Pixel(170);
  mColorPicker.Create(WS_CHILD | WS_VISIBLE, CRect(xm, dy, right, dy + height),
    this, 302, KCOLOR_WITH_HEXA | KCOLOR_WITH_ALPHA);
  dy += height + ym;

  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);

  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_ALL, mResourceLanguage);
  mAll.Create(str, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
    CRect(xm, dy, right, dy + height), this, 101);
  dy += height + ym;

  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  int width = DialogX2Pixel(50);
  int x = xm;
  str.LoadString(AfxGetInstanceHandle(), IDS_FILL, mResourceLanguage);
  mFill.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, dy, x + width, dy + height), this, 102);
  x += xm + width;

  str.LoadString(AfxGetInstanceHandle(), IDS_COPY, mResourceLanguage);
  mCopy.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, dy, x + width, dy + height), this, 103);
  x += xm + width;

  str.LoadString(AfxGetInstanceHandle(), IDS_PASTE, mResourceLanguage);
  mPaste.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, dy, x + width, dy + height), this, 104);

  mMinHeight = dy + height + ym;

  if (gpUiFont != NULL)
  {
    mLabel1.SetFont(gpUiFont);
    mColorPicker.SetFont(gpUiFont);
    mAll.SetFont(gpUiFont);
    mFill.SetFont(gpUiFont);
    mCopy.SetFont(gpUiFont);
    mPaste.SetFont(gpUiFont);
  }
  UpdateCurrentStyle();
  return 0;
}

void KPropertyCtrlEditImage::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

HBRUSH KPropertyCtrlEditImage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if ((nCtlColor == CTLCOLOR_STATIC) || (nCtlColor == CTLCOLOR_BTN))
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlEditImage::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);

  if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 102)
    {
      ImageEffectJob effect;

      memset(&effect, 0, sizeof(ImageEffectJob));
      effect.job = PropertyMode::PModeSelectRectangle;
      effect.applyAll = (mAll.GetCheck() == BST_CHECKED);

      ApplyProperty(&effect);
    }
    else if (ctrl_id == 103) // copy
    {
      ImageEffectJob effect;

      memset(&effect, 0, sizeof(ImageEffectJob));
      effect.job = PropertyMode::PModeCopyRectangle;
      effect.applyAll = 0;

      ApplyProperty(&effect);
    }
    else if (ctrl_id == 104) // paste
    {
      ImageEffectJob effect;

      memset(&effect, 0, sizeof(ImageEffectJob));
      effect.job = PropertyMode::PModePasteImage;
      effect.applyAll = (mAll.GetCheck() == BST_CHECKED);

      ApplyProperty(&effect);
    }
  }
  return 0;
}

LRESULT KPropertyCtrlEditImage::OnChangeColor(WPARAM wParam, LPARAM lParam)
{
  int foreColor = LOWORD(lParam);
  int ctrl_id = HIWORD(lParam);
  if (ctrl_id == 302)
  {
    if (foreColor)
      KShapeEditor::mDefault.SetShapeFill(wParam);
    else
      KShapeEditor::mDefault.SetLineColor(wParam);
  }
  return 0;
}

void KPropertyCtrlEditImage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlEditImage::UpdateCurrentStyle()
{
  DWORD color = KShapeEditor::mDefault.GetShapeFill();
  mColorPicker.SetARGBColor(color, TRUE);
}

BOOL KPropertyCtrlEditImage::GetFillColor(DWORD& fillcolor)
{
  fillcolor = mColorPicker.GetARGBColor(TRUE);
  return TRUE;
}

void KPropertyCtrlEditImage::OnCloseProperty()
{
  ImageEffectJob effect;

  memset(&effect, 0, sizeof(ImageEffectJob));
  effect.job = PropertyMode::PModeSelectRectangle;
  CloseProperty(&effect);
}
