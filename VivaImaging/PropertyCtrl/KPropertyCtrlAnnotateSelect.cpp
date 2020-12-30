// KPropertyCtrlBase.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"
#include "..\Document\KShapeEditor.h"
#include "KPropertyCtrlAnnotateSelect.h"
// KPropertyCtrlAnnotateSelect

IMPLEMENT_DYNAMIC(KPropertyCtrlAnnotateSelect, CWnd)

KPropertyCtrlAnnotateSelect::KPropertyCtrlAnnotateSelect()
  :KPropertyCtrlBase()
{
}

KPropertyCtrlAnnotateSelect::KPropertyCtrlAnnotateSelect(CWnd* parent, RECT& rect)
: KPropertyCtrlBase()
{
  Create(WS_CHILD|WS_VISIBLE, rect, parent, 101);
}

KPropertyCtrlAnnotateSelect::~KPropertyCtrlAnnotateSelect()
{
}


BEGIN_MESSAGE_MAP(KPropertyCtrlAnnotateSelect, CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_CTLCOLOR()
  ON_MESSAGE(WMA_CHANGE_COLOR, OnChangeColor)
END_MESSAGE_MAP()



// KPropertyCtrlAnnotateSelect 메시지 처리기

int KPropertyCtrlAnnotateSelect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  HWND hWnd = GetSafeHwnd();
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int width = DialogX2Pixel(20);
  int right = xm + DialogX2Pixel(PC_ITEM_WIDTH);
  int x = xm;
  int y = ym;

  CString str;

  str.LoadString(AfxGetInstanceHandle(), IDS_FILL_OUTLINE, mResourceLanguage);
  mLabelColor.Create(str, WS_CHILD | WS_VISIBLE,
    CRect(x, y, right, y + height), this, 400);
  y += height + ym;

  height = DialogY2Pixel(130);
  mColorPicker.Create(WS_CHILD | WS_VISIBLE, CRect(x, y, right, y + height), this, 401,
    KCOLOR_WITH_ALPHA | KCOLOR_WITH_DUAL);
  y += height + ym;

  int width1 = DialogX2Pixel(80);
  int width2 = DialogX2Pixel(80);
  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int x2 = width1 + xm;

  str.LoadString(AfxGetInstanceHandle(), IDS_LINESTYLE, mResourceLanguage);
  str += _T(" :");
  mLabelLineStyle.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(x, y, width1, y + height), this, 402);
  mComboLineStyle.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, y, x2 + width2, y + height), this, 403, K_LINE_STYLE);

  // adjust combobox height
  RECT rect;
  mComboLineStyle.GetWindowRect(&rect);
  if (height < (rect.bottom - rect.top))
    height = (rect.bottom - rect.top);

  y += height + ym;

  // line width
  str.LoadString(AfxGetInstanceHandle(), IDS_LINEWIDTH, mResourceLanguage);
  str += _T(" :");
  mLabelLineWidth.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
    CRect(x, y, width1, y + height), this, 404);
  mComboLineWidth.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
    CRect(x2, y, x2 + width2, y + height), this, 405, K_LINE_WIDTH);
  y += height + ym;

  // font style
  y = ym + DialogY2Pixel(PC_ITEM_HEIGHT) + ym + DialogY2Pixel(130) + ym;

  width1 = DialogX2Pixel(110);
  width2 = DialogX2Pixel(40);
  x2 = width1 + xm * 2;

  mFaceNameCombo.Create(WS_CHILD | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST,
    CRect(x, y, x + width1, y + height), this, 402);

  mFontSizeCombo.Create(WS_CHILD | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST,
    CRect(x2, y, x2 + width2, y + height), this, 403);

  y += height + ym;

  int fa_resids[3] = { IDB_FONT_24, IDB_FONT_32, IDB_FONT_48 };
  int id = GetHiDPIImageButtonSize(hWnd);

  width = DialogX2Pixel(20);
  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  mBold.Create(WS_CHILD | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 411, fa_resids[id], 0);
  x += width + xm;
  mItalic.Create(WS_CHILD | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 412, fa_resids[id], 1);
  x += width + xm;
  mUnderline.Create(WS_CHILD | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 413, fa_resids[id], 2);
  y += height + ym;

  mFaceNameCombo.InitItems(FONT_COMBO_FACENAME);
  mFontSizeCombo.InitItems(FONT_COMBO_FONTSIZE);

  // Apply button
  x = xm;
  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  right = DialogX2Pixel(PC_X_MARGIN + PC_APPLY_WIDTH);
  str.LoadString(AfxGetInstanceHandle(), IDS_APPLY_NOW, mResourceLanguage);
  mApply.Create(str, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, right, y + height), this, 102);
  y += height + ym;

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

  UpdateShapeStyle(NULL);

  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  str.LoadString(AfxGetInstanceHandle(), IDS_OBJECT_ALIGN, mResourceLanguage);
  mLabel1.Create(str, WS_CHILD | WS_VISIBLE,
    CRect(x, y, right, y + height), this, 200);
  y += height + ym;

  int resids[3] = { IDB_ALIGN_24, IDB_ALIGN_32, IDB_ALIGN_48};
  id = GetHiDPIImageButtonSize(hWnd);

  height = DialogY2Pixel(PC_APPLY_HEIGHT);
  mAlignLeft.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 201, resids[id], 0);
  x += width + xm;
  mAlignCenter.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 202, resids[id], 1);
  x += width + xm;
  mAlignRight.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 203, resids[id], 2);
  y += height + ym;

  x = xm;
  mAlignTop.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 204, resids[id], 3);
  x += width + xm;
  mAlignVCenter.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 205, resids[id], 4);
  x += width + xm;
  mAlignBottom.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 206, resids[id], 5);
  y += height + ym;

  x = xm;

  height = DialogY2Pixel(PC_ITEM_HEIGHT);
  str.LoadString(AfxGetInstanceHandle(), IDS_OBJECT_ORDER, mResourceLanguage);
  mLabel2.Create(str, WS_CHILD | WS_VISIBLE,
    CRect(x, y, right, y + height), this, 220);
  y += height + ym;

  height = DialogY2Pixel(PC_APPLY_HEIGHT); // PC_BUTTON_HEIGHT);
  mBringTop.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 207, resids[id], 6);
  x += width + xm;
  mBringUp.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 208, resids[id], 7);
  x += width + xm;
  mBringDown.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 209, resids[id], 8);
  x += width + xm;
  mBringBottom.Create(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    CRect(x, y, x + width, y + height), this, 210, resids[id], 9);

  mMinHeight = y + height + ym;

  if (gpUiFont != NULL)
  {
    mLabelColor.SetFont(gpUiFont);
    mColorPicker.SetFont(gpUiFont);
    mLabelLineStyle.SetFont(gpUiFont);
    mLabelLineWidth.SetFont(gpUiFont);
    mComboLineStyle.SetFont(gpUiFont);
    mComboLineWidth.SetFont(gpUiFont);

    mFaceNameCombo.SetFont(gpUiFont);
    mFontSizeCombo.SetFont(gpUiFont);
    mApply.SetFont(gpUiFont);

    mLabel1.SetFont(gpUiFont);
    mAlignLeft.SetFont(gpUiFont);
    mAlignCenter.SetFont(gpUiFont);
    mAlignRight.SetFont(gpUiFont);

    mAlignTop.SetFont(gpUiFont);
    mAlignVCenter.SetFont(gpUiFont);
    mAlignBottom.SetFont(gpUiFont);

    mLabel2.SetFont(gpUiFont);
    mBringTop.SetFont(gpUiFont);
    mBringUp.SetFont(gpUiFont);
    mBringDown.SetFont(gpUiFont);
    mBringBottom.SetFont(gpUiFont);
  }
  return 0;
}

void KPropertyCtrlAnnotateSelect::OnSize(UINT nType, int cx, int cy)
{
  UpdateScrollInfo();
}

void KPropertyCtrlAnnotateSelect::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  OnVerticalScroll(nSBCode, nPos);
}

void KPropertyCtrlAnnotateSelect::arrangeControls()
{
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int width = DialogX2Pixel(20);
  int x = xm;

  int y = ym + DialogY2Pixel(PC_ITEM_HEIGHT) + ym + DialogY2Pixel(130) + ym;

  if (mLabelLineStyle.IsWindowVisible())
  {
    int height = DialogY2Pixel(PC_ITEM_HEIGHT);

    //mLabelLineStyle
    //mComboLineStyle
    //mLabelLineWidth
    //mComboLineWidth
  }

}

HBRUSH KPropertyCtrlAnnotateSelect::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if ((nCtlColor == CTLCOLOR_STATIC) || (nCtlColor == CTLCOLOR_BTN))
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KPropertyCtrlAnnotateSelect::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);

  if (notify_code == BN_CLICKED)
  {
    if ((201 <= ctrl_id) && (ctrl_id <= 210))
    {
      ImageEffectJob effect;

      memset(&effect, 0, sizeof(ImageEffectJob));
      effect.job = PropertyMode::PModeAnnotateSelect;
      effect.param[0] = ctrl_id - 201;

      ApplyProperty(&effect);
      return TRUE;
    }
    else if (102 == ctrl_id) // apply color & linestyle
    {
      ImageEffectJob effect;

      DWORD fill = 0;
      OutlineStyle lineStyle;
      GetShapeStyle(fill, lineStyle);

      memset(&effect, 0, sizeof(ImageEffectJob));
      effect.job = PropertyMode::PModeAnnotateSelect;
      effect.param[0] = SHAPE_APPLY_PROPERTY;
      if (mComboLineStyle.IsWindowVisible())
      {
        effect.channel = ImageEffectFillStyle | ImageEffectLineStyle;
        effect.param[1] = fill; // foreground color
        effect.param[2] = lineStyle.Color;
        effect.param[3] = lineStyle.Style;
        effect.param[4] = lineStyle.Width;
      }

      if (mFaceNameCombo.IsWindowVisible())
      {
        effect.channel |= ImageEffectFontStyle;
        effect.param[5] = fill;
        effect.param[6] = mFontSizeCombo.GetSelectedFontSize();
        effect.param[7] = GetFontAttribute();
        effect.extra = mFaceNameCombo.GetSelectedFaceName();
      }

      ApplyProperty(&effect);
      FreeImageEffectJob(&effect);
      return TRUE;
    }
    else if (411 == ctrl_id)
    {
      mBold.SetSelected(!mBold.IsSelected());
    }
    else if (412 == ctrl_id)
    {
      mItalic.SetSelected(!mItalic.IsSelected());
    }
    else if (413 == ctrl_id)
    {
      mUnderline.SetSelected(!mUnderline.IsSelected());
    }
  }
  return 0;
}

LRESULT KPropertyCtrlAnnotateSelect::OnChangeColor(WPARAM wParam, LPARAM lParam)
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

void KPropertyCtrlAnnotateSelect::UpdateShapeStyle(void *obj)
{
  KShapeBase* s = (KShapeBase*)obj;

  int text_attr;
  int line_attr;

  if ((s != NULL) && (s->GetShapeType() == ShapeType::ShapeTextbox))
  {
    text_attr = SW_SHOW;
    line_attr = SW_HIDE;
    mColorPicker.SetMode(KCOLOR_WITH_ALPHA);
  }
  else
  {
    text_attr = SW_HIDE;
    line_attr = SW_SHOW;
    mColorPicker.SetMode(KCOLOR_WITH_ALPHA | KCOLOR_WITH_DUAL);
  }

  if (mFaceNameCombo.IsWindowVisible() != (text_attr == SW_SHOW))
  {
    mFaceNameCombo.ShowWindow(text_attr);
    mFontSizeCombo.ShowWindow(text_attr);
    mBold.ShowWindow(text_attr);
    mItalic.ShowWindow(text_attr);
    mUnderline.ShowWindow(text_attr);
  }
  if (mLabelLineStyle.IsWindowVisible() != (line_attr == SW_SHOW))
  {
    mLabelLineStyle.ShowWindow(line_attr);
    mComboLineStyle.ShowWindow(line_attr);
    mLabelLineWidth.ShowWindow(line_attr);
    mComboLineWidth.ShowWindow(line_attr);
  }
  arrangeControls();

  DWORD color;

  if (line_attr == SW_SHOW)
  {
    OutlineStyle linestyle;

    if ((s == NULL) || !s->GetOutlineStyle(linestyle))
      KShapeEditor::mDefault.GetLineStyle(linestyle);

    mColorPicker.SetARGBColor(linestyle.Color, FALSE);

    int index = KLineComboBox::FromLineStyle(linestyle.Style);
    mComboLineStyle.SetCurSel(index);

    index = KLineComboBox::FromLineWidth(linestyle.Width);
    mComboLineWidth.SetCurSel(index);

    if ((s == NULL) || !s->GetFillColor(color))
      color = KShapeEditor::mDefault.GetShapeFill();
  }
  if (text_attr == SW_SHOW)
  {
    TCHAR facename[LF_FACESIZE];
    int pointSize = 0;
    DWORD attr = 0;

    if ((s == NULL) || !s->GetFont(facename, pointSize, attr))
      KShapeEditor::mDefault.GetFontStyle(facename, pointSize, attr);

    if ((s == NULL) || !s->GetTextColor(color))
      color = KShapeEditor::mDefault.GetTextColor();

    mFaceNameCombo.SetSelectFaceName(facename);
    mFontSizeCombo.SetSelectFontSize(pointSize);

    mBold.SetSelected(attr & FONT_BOLD);
    mItalic.SetSelected(attr & FONT_ITALIC);
    mUnderline.SetSelected(attr & FONT_UNDERLINE);
  }
    
  mColorPicker.SetARGBColor(color, TRUE);
}

BOOL KPropertyCtrlAnnotateSelect::GetShapeStyle(DWORD& fillcolor, OutlineStyle& linestyle)
{ 
  linestyle.Color = mColorPicker.GetARGBColor(FALSE);
  linestyle.Style = mComboLineStyle.GetLineStyle();
  linestyle.Width = mComboLineWidth.GetLineWidth();

  fillcolor = mColorPicker.GetARGBColor(TRUE);
  return TRUE;
}

int KPropertyCtrlAnnotateSelect::GetFontAttribute()
{
  int attribute = 0;
  if (mBold.IsSelected())
    attribute |= FONT_BOLD;
  if (mItalic.IsSelected())
    attribute |= FONT_ITALIC;
  if (mUnderline.IsSelected())
    attribute |= FONT_UNDERLINE;
  return attribute;
}