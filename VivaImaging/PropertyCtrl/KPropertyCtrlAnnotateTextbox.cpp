// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlAnnotateTextbox.h"
// KPropertyCtrlAnnotateTextbox

IMPLEMENT_DYNAMIC(KPropertyCtrlAnnotateTextbox, CWnd)

KPropertyCtrlAnnotateTextbox::KPropertyCtrlAnnotateTextbox()
  :KPropertyCtrlBase()
{
}

KPropertyCtrlAnnotateTextbox::KPropertyCtrlAnnotateTextbox(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlAnnotateTextbox::~KPropertyCtrlAnnotateTextbox()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlAnnotateTextbox, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_MESSAGE(WMA_CHANGE_COLOR, OnChangeColor)
END_MESSAGE_MAP()



// KPropertyCtrlAnnotateTextbox 메시지 처리기

int KPropertyCtrlAnnotateTextbox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int height = DialogY2Pixel(200);
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int y = ym;
  int x = xm;

  height = DialogY2Pixel(110);
  mColorPicker.Create(WS_CHILD | WS_VISIBLE, CRect(x, y, right, y + height), this, 401, 0);
  y += height + ym;

  int width1 = DialogX2Pixel(110);
  int width2 = DialogX2Pixel(40);
  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int x2 = width1 + xm * 2;

  mFaceNameCombo.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST,
    CRect(x, y, x + width1, y + height), this, 402);
  // adjust combobox height
  RECT rect;
  mFaceNameCombo.GetWindowRect(&rect);
  if (height < (rect.bottom - rect.top))
    height = (rect.bottom - rect.top);

  mFontSizeCombo.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST,
    CRect(x2, y, x2 + width2, y + height), this, 403);
  y += height + ym;

  int resids[3] = { IDB_FONT_24, IDB_FONT_32, IDB_FONT_48 };
  int id = GetHiDPIImageButtonSize(hWnd);

  int width = DialogX2Pixel(20);
  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  mBold.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 411, resids[id], 0);
  x += width + xm;
  mItalic.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 412, resids[id], 1);
  x += width + xm;
  mUnderline.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 413, resids[id], 2);
  //y += height + ym;
  mMinHeight = y + height + ym;

  if (gpUiFont != NULL)
  {
    mColorPicker.SetFont(gpUiFont);
    mFaceNameCombo.SetFont(gpUiFont);
    mFontSizeCombo.SetFont(gpUiFont);
  }

  mFaceNameCombo.InitItems(FONT_COMBO_FACENAME);
  mFontSizeCombo.InitItems(FONT_COMBO_FONTSIZE);
  UpdateShapeStyle(NULL);
  return 0;
}

void KPropertyCtrlAnnotateTextbox::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

void KPropertyCtrlAnnotateTextbox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

HBRUSH KPropertyCtrlAnnotateTextbox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if ((nCtlColor == CTLCOLOR_STATIC) || (nCtlColor == CTLCOLOR_BTN))
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlAnnotateTextbox::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == CBN_SELCHANGE)
  {
    if (ctrl_id == 402)
    {
      int index = mFaceNameCombo.GetCurSel();
      if (index >= 0)
      {
        CString str;
        mFaceNameCombo.GetLBText(index, str);
        if (str.GetLength() > 0)
          KShapeEditor::mDefault.SetFacename(str);
      }
    }
    if (ctrl_id == 403)
    {
      int index = mFontSizeCombo.GetCurSel();
      if (index >= 0)
        KShapeEditor::mDefault.SetFontSize(mFontSizeCombo.GetItemFontSize(index));
    }
    return TRUE;
  }
  else if (notify_code == BN_CLICKED)
  {
    if (411 == ctrl_id)
    {
      int n = KShapeEditor::mDefault.ToggleFontStyle(FONT_BOLD);
      mBold.SetSelected(n & FONT_BOLD);
    }
    else if (412 == ctrl_id)
    {
      int n = KShapeEditor::mDefault.ToggleFontStyle(FONT_ITALIC);
      mItalic.SetSelected(n & FONT_ITALIC);
    }
    else if (413 == ctrl_id)
    {
      int n = KShapeEditor::mDefault.ToggleFontStyle(FONT_UNDERLINE);
      mUnderline.SetSelected(n & FONT_UNDERLINE);
    }
  }

  return FALSE;
}

LRESULT KPropertyCtrlAnnotateTextbox::OnChangeColor(WPARAM wParam, LPARAM lParam)
{
  int foreColor = LOWORD(lParam);
  int ctrl_id = HIWORD(lParam);
  if (ctrl_id == 401)
  {
    KShapeEditor::mDefault.SetTextColor(wParam);
  }
  return 0;
}

void KPropertyCtrlAnnotateTextbox::UpdateShapeStyle(void *obj)
{
  KShapeBase* s = (KShapeBase*)obj;

  DWORD textcolor;
  if ((s == NULL) || !s->GetTextColor(textcolor))
    textcolor = KShapeEditor::mDefault.GetTextColor();

  mColorPicker.SetARGBColor(textcolor, TRUE);

  TCHAR facename[LF_FACESIZE];
  int pointSize = 0;
  DWORD attr = 0;
  if ((s == NULL) || !s->GetFont(facename, pointSize, attr))
    KShapeEditor::mDefault.GetFontStyle(facename, pointSize, attr);

  mFaceNameCombo.SetSelectFaceName(facename);
  mFontSizeCombo.SetSelectFontSize(pointSize);

  mBold.SetSelected(attr & FONT_BOLD);
  mItalic.SetSelected(attr & FONT_ITALIC);
  mUnderline.SetSelected(attr & FONT_UNDERLINE);
}