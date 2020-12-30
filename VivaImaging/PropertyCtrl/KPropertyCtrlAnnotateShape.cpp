// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlAnnotateShape.h"
// KPropertyCtrlAnnotateShape

IMPLEMENT_DYNAMIC(KPropertyCtrlAnnotateShape, CWnd)

KPropertyCtrlAnnotateShape::KPropertyCtrlAnnotateShape()
  :KPropertyCtrlBase()
{
}

KPropertyCtrlAnnotateShape::KPropertyCtrlAnnotateShape(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlAnnotateShape::~KPropertyCtrlAnnotateShape()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlAnnotateShape, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_MESSAGE(WMA_CHANGE_COLOR, OnChangeColor)
END_MESSAGE_MAP()



// KPropertyCtrlAnnotateShape 메시지 처리기

int KPropertyCtrlAnnotateShape::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int height = DialogY2Pixel(130);
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int dy = ym;

  mColorPicker.Create(WS_CHILD | WS_VISIBLE, CRect(xm, dy, right, dy + height), this, 401,
    KCOLOR_WITH_ALPHA | KCOLOR_WITH_DUAL);
  dy += height + ym;

  int width1 = DialogX2Pixel(80);
  int width2 = DialogX2Pixel(80);
  //int xm = DialogX2Pixel(5);
  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int x2 = width1 + xm;

  // line style
  CString str;
  str.LoadString(AfxGetInstanceHandle(), IDS_LINESTYLE, mResourceLanguage);
  str += _T(" :");
  mLabelLineStyle.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(0, dy, width1, dy + height), this, 402);
  mComboLineStyle.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, dy, x2 + width2, dy + height), this, 403, K_LINE_STYLE);

  // adjust combobox height
  RECT rect;
  mComboLineStyle.GetWindowRect(&rect);
  if (height < (rect.bottom - rect.top))
    height = (rect.bottom - rect.top);

  dy += height + ym;

  // line width
  str.LoadString(AfxGetInstanceHandle(), IDS_LINEWIDTH, mResourceLanguage);
  str += _T(" :");
  mLabelLineWidth.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(0, dy, width1, dy + height), this, 404);
  mComboLineWidth.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, dy, x2 + width2, dy + height), this, 405, K_LINE_WIDTH);
  //dy += height + ym;
  mMinHeight = dy + height + ym;


  for (int i = 0; i < NO_LINE_STYLE; i++)
  {
    mComboLineStyle.AddString(_T(""));
    mComboLineStyle.SetItemData(i, i);
  }
  for (int i = 0; i < NO_LINE_WIDTH; i++)
  {
    mComboLineWidth.AddString(_T(""));
    mComboLineWidth.SetItemData(i, i);
  }

  if (gpUiFont != NULL)
  {
    mColorPicker.SetFont(gpUiFont);

    mLabelLineStyle.SetFont(gpUiFont);
    mComboLineStyle.SetFont(gpUiFont);
    mLabelLineWidth.SetFont(gpUiFont);
    mComboLineWidth.SetFont(gpUiFont);
  }
  UpdateCurrentStyle();
  return 0;
}

void KPropertyCtrlAnnotateShape::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

void KPropertyCtrlAnnotateShape::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

HBRUSH KPropertyCtrlAnnotateShape::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if ((nCtlColor == CTLCOLOR_STATIC) || (nCtlColor == CTLCOLOR_BTN))
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlAnnotateShape::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == CBN_SELCHANGE)
  {
    if (ctrl_id == 403)
    {
      int index = mComboLineStyle.GetCurSel();
      KShapeEditor::mDefault.SetLineStyle(KLineComboBox::GetLineStyle(index));
    }
    else if (ctrl_id == 405)
    {
      int index = mComboLineWidth.GetCurSel();
      KShapeEditor::mDefault.SetLineWidth(KLineComboBox::GetLineWidth(index));
    }
    /*
    else if (ctrl_id == 407)
    {
      int index = mComboStartArrow.GetCurSel();
      SetDefaultStartArrow(index);
    }
    else if (ctrl_id == 409)
    {
      int index = mComboEndArrow.GetCurSel();
      SetDefaultEndArrow(index);
    }
    */
    return TRUE;
  }
  return FALSE;
}

LRESULT KPropertyCtrlAnnotateShape::OnChangeColor(WPARAM wParam, LPARAM lParam)
{
  int foreColor = LOWORD(lParam);
  int ctrl_id = HIWORD(lParam);
  if (ctrl_id == 401)
  {
    if (foreColor)
      KShapeEditor::mDefault.SetShapeFill(wParam);
    else
      KShapeEditor::mDefault.SetLineColor(wParam);
  }
  return 0;
}

void KPropertyCtrlAnnotateShape::UpdateCurrentStyle()
{
  mComboLineStyle.SetCurSel(0);
  mComboLineWidth.SetCurSel(0);

  OutlineStyle lineStyle;
  KShapeEditor::mDefault.GetLineStyle(lineStyle);

  mComboLineStyle.SetCurSel(KLineComboBox::FromLineStyle(lineStyle.Style));
  mComboLineWidth.SetCurSel(KLineComboBox::FromLineWidth(lineStyle.Width));
  mColorPicker.SetARGBColor(lineStyle.Color, FALSE);

  DWORD fill = KShapeEditor::mDefault.GetShapeFill();
  mColorPicker.SetARGBColor(fill, TRUE);
}

BOOL KPropertyCtrlAnnotateShape::GetShapeStyle(DWORD& fillcolor, OutlineStyle& linestyle)
{
  fillcolor = mColorPicker.GetARGBColor(TRUE);

  linestyle.Color = mColorPicker.GetARGBColor(FALSE);

  int index = mComboLineWidth.GetCurSel();
  linestyle.Width = KLineComboBox::GetLineWidth(index);

  index = mComboLineStyle.GetCurSel();
  linestyle.Style = KLineComboBox::GetLineStyle(index);

  return TRUE;
}
