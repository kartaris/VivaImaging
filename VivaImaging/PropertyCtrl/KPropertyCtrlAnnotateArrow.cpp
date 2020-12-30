// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KPropertyCtrlAnnotateArrow.h"
// KPropertyCtrlAnnotateArrow

IMPLEMENT_DYNAMIC(KPropertyCtrlAnnotateArrow, CWnd)

KPropertyCtrlAnnotateArrow::KPropertyCtrlAnnotateArrow()
  :KPropertyCtrlBase()
{
}

KPropertyCtrlAnnotateArrow::KPropertyCtrlAnnotateArrow(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlAnnotateArrow::~KPropertyCtrlAnnotateArrow()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlAnnotateArrow, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_MESSAGE(WMA_CHANGE_COLOR, OnChangeColor)
END_MESSAGE_MAP()



// KPropertyCtrlAnnotateArrow 메시지 처리기

int KPropertyCtrlAnnotateArrow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int height = DialogY2Pixel(130);
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int dy = ym;

  mColorPicker.Create(WS_CHILD | WS_VISIBLE, CRect(xm, dy, right, dy + height), this, 401, KCOLOR_WITH_ALPHA);
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
  dy += height + ym;

  // line width
  str.LoadString(AfxGetInstanceHandle(), IDS_LINEWIDTH, mResourceLanguage);
  str += _T(" :");
  mLabelLineWidth.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(0, dy, width1, dy + height), this, 404);
  mComboLineWidth.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, dy, x2 + width2, dy + height), this, 405, K_LINE_WIDTH);
  dy += height + ym;

  // start arrow
  str.LoadString(AfxGetInstanceHandle(), IDS_STARTARROW, mResourceLanguage);
  str += _T(" :");
  mLabelStartArrow.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(0, dy, width1, dy + height), this, 406);
  mComboStartArrow.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, dy, x2 + width2, dy + height), this, 407, K_LINE_START_ARROW);
  dy += height + ym;

  // end arrow
  str.LoadString(AfxGetInstanceHandle(), IDS_ENDARROW, mResourceLanguage);
  str += _T(" :");
  mLabelEndArrow.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(0, dy, width1, dy + height), this, 408);
  mComboEndArrow.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, dy, x2 + width2, dy + height), this, 409, K_LINE_END_ARROW);
  //dy += height + ym;
  mMinHeight = dy + height + ym;

  //no hollow line
  for (int i = 1; i < NO_LINE_STYLE; i++)
  {
    mComboLineStyle.AddString(_T(""));
    mComboLineStyle.SetItemData(i-1, i); // without hollow
  }
  for (int i = 0; i < NO_LINE_WIDTH; i++)
  {
    mComboLineWidth.AddString(_T(""));
    mComboLineWidth.SetItemData(i, i);
  }
  for (int i = 0; i < NO_LINE_ARROW_STYLE; i++)
  {
    mComboStartArrow.AddString(_T(""));
    mComboStartArrow.SetItemData(i, i);
    mComboEndArrow.AddString(_T(""));
    mComboEndArrow.SetItemData(i, i);
  }

  if (gpUiFont != NULL)
  {
    mColorPicker.SetFont(gpUiFont);

    mLabelLineStyle.SetFont(gpUiFont);
    mComboLineStyle.SetFont(gpUiFont);
    mLabelLineWidth.SetFont(gpUiFont);
    mComboLineWidth.SetFont(gpUiFont);
    mLabelStartArrow.SetFont(gpUiFont);
    mComboStartArrow.SetFont(gpUiFont);
    mLabelEndArrow.SetFont(gpUiFont);
    mComboEndArrow.SetFont(gpUiFont);
  }
  UINT arrange_control_ids[] = {
    403, 402, 0, 404, 405, 0, 406, 407, 0, 408, 409, 0
  };

  RECT rect;
  mColorPicker.GetWindowRect(&rect);
  ScreenToClient(&rect);
  dy = rect.bottom + ym;
  dy = RearrangeControlYPosition(arrange_control_ids, 12, dy, height);

  if (dy > mMinHeight)
    mMinHeight = dy;

  UpdateCurrentStyle();
  return 0;
}


void KPropertyCtrlAnnotateArrow::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

void KPropertyCtrlAnnotateArrow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

HBRUSH KPropertyCtrlAnnotateArrow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlAnnotateArrow::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == CBN_SELCHANGE)
  {
    if (ctrl_id == 403)
    {
      int index = mComboLineStyle.GetCurSel();
      KShapeEditor::mDefault.SetLineStyle(KLineComboBox::GetLineStyle(index + 1)); // without hollow
    }
    else if (ctrl_id == 405)
    {
      int index = mComboLineWidth.GetCurSel();
      KShapeEditor::mDefault.SetLineWidth(KLineComboBox::GetLineWidth(index));
    }
    else if (ctrl_id == 407)
    {
      int index = mComboStartArrow.GetCurSel();
      KShapeEditor::mDefault.SetStartArrow(index);
    }
    else if (ctrl_id == 409)
    {
      int index = mComboEndArrow.GetCurSel();
      KShapeEditor::mDefault.SetEndArrow(index);
    }
    return TRUE;
  }
  return FALSE;
}

LRESULT KPropertyCtrlAnnotateArrow::OnChangeColor(WPARAM wParam, LPARAM lParam)
{
  int foreColor = LOWORD(lParam);
  int ctrl_id = HIWORD(lParam);
  if (ctrl_id == 401)
  {
    if (foreColor)
      KShapeEditor::mDefault.SetLineColor((DWORD)wParam);
  }
  return 0;
}

void KPropertyCtrlAnnotateArrow::UpdateCurrentStyle()
{
  OutlineStyle lineStyle;
  KShapeEditor::mDefault.GetLineStyle(lineStyle);

  mColorPicker.SetARGBColor(lineStyle.Color, TRUE);
  mComboLineStyle.SetCurSel(KLineComboBox::FromLineStyle(lineStyle.Style) - 1); // without hollow
  mComboLineWidth.SetCurSel(KLineComboBox::FromLineWidth(lineStyle.Width));

  int a = KShapeEditor::mDefault.GetStartArrow();
  mComboStartArrow.SetCurSel(a);
  a = KShapeEditor::mDefault.GetEndArrow();
  mComboEndArrow.SetCurSel(a);
}

BOOL KPropertyCtrlAnnotateArrow::GetLineStyle(OutlineStyle& linestyle, BYTE& startArrow, BYTE& endArrow)
{
  linestyle.Color = mColorPicker.GetARGBColor(TRUE);

  int index = mComboLineWidth.GetCurSel();
  linestyle.Width = KLineComboBox::GetLineWidth(index);

  index = mComboLineStyle.GetCurSel();
  linestyle.Style = KLineComboBox::GetLineStyle(index + 1); // without hollow

  startArrow = mComboStartArrow.GetCurSel();
  endArrow = mComboEndArrow.GetCurSel();
  return TRUE;
}
