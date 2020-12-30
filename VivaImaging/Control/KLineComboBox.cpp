// KLineComboBox.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Document\Shape\KShapeBase.h"
#include "KLineComboBox.h"

#include "..\Platform\Graphics.h"

// KLineComboBox

IMPLEMENT_DYNAMIC(KLineComboBox, CComboBox)

KLineComboBox::KLineComboBox()
{
  mStyle = 0;
}

KLineComboBox::~KLineComboBox()
{
}


BEGIN_MESSAGE_MAP(KLineComboBox, CWnd)
END_MESSAGE_MAP()



// KLineComboBox 메시지 처리기

BOOL KLineComboBox::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int style)
{
  mStyle = style;
  windowStyle |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;
  return CComboBox::Create(windowStyle, rect, parent, controlId);
}

int KLineComboBox::GetLineStyle()
{
  int index = GetCurSel();
  return GetLineStyle(index);
}

int KLineComboBox::GetLineWidth()
{
  int index = GetCurSel();
  return GetLineWidth(index);
}

static int K_LineWidth[NO_LINE_WIDTH] = { 1, 3, 5, 8, 12, 18, 24, 32 };
static int K_LineStyle[NO_LINE_STYLE] = { (int)LineStyle::LineHollow, (int)LineStyle::LineSolid, (int)LineStyle::LineDot, (int)LineStyle::LineDash, (int)LineStyle::LineDashDot };

int KLineComboBox::GetLineWidth(int index)
{
  if (index >= NO_LINE_WIDTH - 1)
    index = NO_LINE_WIDTH - 1;
  return K_LineWidth[index];
}

int KLineComboBox::GetLineStyle(int index)
{
  if (index >= NO_LINE_STYLE - 1)
    index = NO_LINE_STYLE - 1;
  return K_LineStyle[index];
}

int KLineComboBox::FromLineStyle(int style)
{
  for (int i = 0; i < NO_LINE_STYLE; i++)
  {
    if (K_LineStyle[i] == style)
      return i;
  }
  return 0;
}

int KLineComboBox::FromLineWidth(int width)
{
  for (int i = 0; i < NO_LINE_WIDTH; i++)
  {
    if (K_LineWidth[i] >= width)
      return i;
  }
  return 0;
}

void KLineComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC dc;

  dc.Attach(lpDrawItemStruct->hDC);
  COLORREF crOldTextColor = dc.GetTextColor();
  COLORREF crOldBkColor = dc.GetBkColor();

  // If this item is selected, set the background color 
  // and the text color to appropriate values. Erase
  // the rect by filling it with the background color.
  if ((lpDrawItemStruct->itemAction & ODA_SELECT) &&
    (lpDrawItemStruct->itemState  & ODS_SELECTED))
  {
    crOldTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    crOldBkColor=::GetSysColor(COLOR_HIGHLIGHT);
  }

  dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

  DWORD id = (DWORD)lpDrawItemStruct->itemData;
  CPen pen;

  if ((mStyle == K_LINE_STYLE) || (mStyle == K_LINE_STYLE_WITH_NULL))
  {
    if (id < 0)
      id = 0;
    if (id >= NO_LINE_STYLE)
      id = NO_LINE_STYLE - 1;

    if (mStyle == K_LINE_STYLE_WITH_NULL)
      id++;

    if (id == 0) // hollow
    {
      CString str(_T("Hollow"));
      id = dc.DrawText(str, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    }
    else
    {
      int style = K_LineStyle[id];
      int penStyle = 0;
      if (style == (int)LineStyle::LineSolid)
        penStyle = PS_SOLID;
      else if (style == (int)LineStyle::LineDot)
        penStyle = PS_DOT;
      else if (style == (int)LineStyle::LineDash)
        penStyle = PS_DASH;
      else if (style == (int)LineStyle::LineDashDot)
        penStyle = PS_DASHDOT;
      pen.CreatePen(penStyle, 1, crOldTextColor);

      CPen* old_pen = dc.SelectObject(&pen);
      int y = (lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom) / 2;
      dc.MoveTo(lpDrawItemStruct->rcItem.left, y);
      dc.LineTo(lpDrawItemStruct->rcItem.right, y);
      dc.SelectObject(old_pen);
    }
  }
  else if (mStyle == K_LINE_WIDTH)
  {
    if (id < 0)
      id = 0;
    if (id >= NO_LINE_WIDTH)
      id = NO_LINE_WIDTH - 1;

    int width = K_LineWidth[id];

    pen.CreatePen(PS_SOLID, width, crOldTextColor);
    CPen* old_pen = dc.SelectObject(&pen);
    int y = (lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom) / 2;
    dc.MoveTo(lpDrawItemStruct->rcItem.left, y);
    dc.LineTo(lpDrawItemStruct->rcItem.right, y);
    dc.SelectObject(old_pen);
  }
  else if ((mStyle == K_LINE_START_ARROW) || (mStyle == K_LINE_END_ARROW))
  {
    int y = (lpDrawItemStruct->rcItem.top + lpDrawItemStruct->rcItem.bottom) / 2;
    int cx = 0;
    int width = 3;

    pen.CreatePen(PS_SOLID, width, crOldTextColor);
    CPen* old_pen = dc.SelectObject(&pen);
    int dir = 180;

    if (mStyle == K_LINE_START_ARROW)
    {
      cx = (lpDrawItemStruct->rcItem.left + lpDrawItemStruct->rcItem.right) / 3;
      dc.MoveTo(cx, y);
      dc.LineTo(lpDrawItemStruct->rcItem.right, y);
    }
    else
    {
      cx = (lpDrawItemStruct->rcItem.left + lpDrawItemStruct->rcItem.right) * 2 / 3;
      dc.MoveTo(cx, y);
      dc.LineTo(lpDrawItemStruct->rcItem.left, y);
      dir = 0;
    }
    dc.SelectObject(old_pen);

    if (id > 0)
    {
      width = 3 * 5;
      DrawLineArrow(dc, cx, y, width, dir, id, crOldTextColor);
    }
  }

  dc.SetTextColor(crOldTextColor);
  dc.SetBkColor(crOldBkColor);

  dc.Detach();
}

int KLineComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
  if (lpCompareItemStruct->itemID1 > lpCompareItemStruct->itemID2)
    return 1;
  else if (lpCompareItemStruct->itemID1 < lpCompareItemStruct->itemID2)
    return -1;
  return 0;
}

void KLineComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
  CSize   sz;
  CDC*    pDC = GetDC();
  sz = pDC->GetTextExtent(_T("H"));
  ReleaseDC(pDC);
  UINT need = 3 * sz.cy / 2;
  if (lpmis->itemHeight < need)
    lpmis->itemHeight = need;
}
