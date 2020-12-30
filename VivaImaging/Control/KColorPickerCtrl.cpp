// KColorPickerCtrl.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "KColorPickerCtrl.h"

#include "..\Platform\Graphics.h"

// KColorPickerCtrl

IMPLEMENT_DYNAMIC(KColorPickerCtrl, CWnd)

KColorPickerCtrl::KColorPickerCtrl()
{
  mStyle = 0;
  mHSV.Hue = 0;
  mHSV.Sat = 100;
  mHSV.Val = 100;
  mBgHSV.Hue = 0;
  mBgHSV.Sat = 0;
  mBgHSV.Val = 100;
  mForeColor = 1;

  mAlpha = 0xFF;
  mMouseDragMode = 0;
  RegisterWndClass();

  memset(mCustomPalette, 0, sizeof(HSV_C) * NO_CUSTOM_PALETTE);
  for (int c = 0; c < 18; c++)
  {
    mCustomPalette[c].Hue = c * 20;
    mCustomPalette[c].Sat = 100;
    mCustomPalette[c].Val = 100;
  }
}

KColorPickerCtrl::~KColorPickerCtrl()
{
}


BEGIN_MESSAGE_MAP(KColorPickerCtrl, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_CTLCOLOR()
  ON_WM_HSCROLL()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSELEAVE()
  ON_WM_MOUSEWHEEL()
  ON_WM_TIMER()
END_MESSAGE_MAP()



// KColorPickerCtrl 메시지 처리기


TCHAR K_COLOR_PICKER_CTRL_CLASS[] = _T("KColorPickerCtrlClass");
TCHAR K_COLOR_PICKER_CTRL_WINDOW[] = _T("KColorPickerCtrlWnd");

BOOL KColorPickerCtrl::RegisterWndClass()
{
  WNDCLASS windowclass;
  HINSTANCE hInst = AfxGetInstanceHandle();
  if (!(::GetClassInfo(hInst, K_COLOR_PICKER_CTRL_CLASS, &windowclass)))
  {
    windowclass.style = CS_HREDRAW;
    windowclass.lpfnWndProc = ::DefWindowProc;
    windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
    windowclass.hInstance = hInst;
    windowclass.hIcon = NULL;
    windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
    windowclass.lpszMenuName = NULL;
    windowclass.lpszClassName = K_COLOR_PICKER_CTRL_CLASS;
    if (!AfxRegisterClass(&windowclass))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }
  return TRUE;
}

BOOL KColorPickerCtrl::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int style)
{
  mStyle = style;
  return CWnd::Create(K_COLOR_PICKER_CTRL_CLASS, K_COLOR_PICKER_CTRL_WINDOW, windowStyle, rect, parent, controlId);
}

#define PC_X_MARGIN 8
#define PC_Y_MARGIN 6
#define PC_ITEM_HEIGHT 16
#define PC_ITEM_WIDTH 160

int KColorPickerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  RECT client;

  client.top = DialogY2Pixel(80);
  client.bottom = DialogY2Pixel(100);
  client.left = DialogX2Pixel(25);
  client.right = DialogX2Pixel(39);

  mAdd.Create(L">", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    client, this, 101);

  // 160 : 74 + 3 + 50 + 3 + 30
  int xm = DialogX2Pixel(PC_X_MARGIN);
  int ym = DialogY2Pixel(PC_Y_MARGIN);
  int height = DialogY2Pixel(PC_ITEM_HEIGHT);
  int right = DialogX2Pixel(PC_X_MARGIN + PC_ITEM_WIDTH);
  int width1 = DialogX2Pixel(74);
  int width2 = DialogX2Pixel(50);
  int x2 = width1 + DialogX2Pixel(3);
  int x3 = x2 + width2 + DialogX2Pixel(3);
  int width3 = DialogX2Pixel(30);
  int yo = DialogY2Pixel(2);
  int dy = client.bottom + ym;

  CString str;

  if (mStyle & KCOLOR_WITH_HEXA)
  {
    str.LoadString(AfxGetInstanceHandle(), IDS_HEXA_COLOR, mResourceLanguage);
    str += _T(" :");
    mLabelHexa.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
      CRect(0, dy + yo, width1, dy + height), this, 310);
    mEditHexaColor.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
      CRect(x2, dy, x2 + width2, dy + height), this, 311);
    dy += height + ym;
  }

  if (mStyle & KCOLOR_WITH_ALPHA)
  {
    width1 = DialogX2Pixel(50);
    width2 = DialogX2Pixel(74);
    x2 = width1 + DialogX2Pixel(3);
    x3 = x2 + width2 + DialogX2Pixel(3);

    str.LoadString(AfxGetInstanceHandle(), IDS_ALPHA, mResourceLanguage);
    str += _T(" :");
    mLabelAlpha.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
      CRect(xm, dy + yo, xm + width1, dy + height), this, 312);
    mSliderAlpha.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
      CRect(x2, dy, x2 + width2, dy + height), this, 313);

    if (x3 + width3 > lpCreateStruct->cx)
      width3 = lpCreateStruct->cx - x3;

    mEditAlpha.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
      CRect(x3, dy, x3 + width3, dy + height), this, 314);
    dy += height + ym;

    mSliderAlpha.SetRange(0, 255);
    UpdateColorSlider(8);
  }

  if (mStyle & KCOLOR_WITH_SLIDER)
  {
    width1 = DialogX2Pixel(50);
    width2 = DialogX2Pixel(74);
    x2 = width1 + DialogX2Pixel(3);
    x3 = x2 + width2 + DialogX2Pixel(3);

    str.LoadString(AfxGetInstanceHandle(), IDS_HUE, mResourceLanguage);
    str += _T(" :");
    mLabelHue.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
      CRect(xm, dy + yo, xm + width1, dy + height), this, 302);
    mSliderHue.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
      CRect(x2, dy, x2 + width2, dy + height), this, 303);
    mEditHue.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
      CRect(x3, dy, x3 + width3, dy + height), this, 304);
    dy += height + ym;

    str.LoadString(AfxGetInstanceHandle(), IDS_SATURATION, mResourceLanguage);
    str += _T(" :");
    mLabelSaturation.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
      CRect(xm, dy + yo, xm + width1, dy + height), this, 305);
    mSliderSaturation.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
      CRect(x2, dy, x2 + width2, dy + height), this, 306);
    mEditSaturation.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
      CRect(x3, dy, x3 + width3, dy + height), this, 307);
    dy += height + ym;

    str.LoadString(AfxGetInstanceHandle(), IDS_VALUE, mResourceLanguage);
    str += _T(" :");
    mLabelValue.Create(str, WS_CHILD | WS_VISIBLE | SS_RIGHT,
      CRect(xm, dy + yo, xm + width1, dy + height), this, 308);
    mSliderValue.Create(WS_CHILD | WS_VISIBLE | TBS_TOP,
      CRect(x2, dy, x2 + width2, dy + height), this, 309);
    mEditValue.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
      CRect(x3, dy, x3 + width3, dy + height), this, 310);
    dy += height + ym;

    mSliderHue.SetRange(0, 360);
    mSliderSaturation.SetRange(0, 100);
    mSliderValue.SetRange(0, 100);

    UpdateColorSlider(7);
  }

  UpdateCurrentColor(FALSE);

  if (gpUiFont != NULL)
  {
    if (mStyle & KCOLOR_WITH_HEXA)
    {
      mLabelHexa.SetFont(gpUiFont);
      mEditHexaColor.SetFont(gpUiFont);
    }

    if (mStyle & KCOLOR_WITH_ALPHA)
    {
      mLabelAlpha.SetFont(gpUiFont);
      mSliderAlpha.SetFont(gpUiFont);
      mEditAlpha.SetFont(gpUiFont);
    }

    if (mStyle & KCOLOR_WITH_SLIDER)
    {
      mLabelHue.SetFont(gpUiFont);
      mSliderHue.SetFont(gpUiFont);
      mEditHue.SetFont(gpUiFont);

      mLabelSaturation.SetFont(gpUiFont);
      mSliderSaturation.SetFont(gpUiFont);
      mEditSaturation.SetFont(gpUiFont);

      mLabelValue.SetFont(gpUiFont);
      mSliderValue.SetFont(gpUiFont);
      mEditValue.SetFont(gpUiFont);
    }
    mAdd.SetFont(gpUiFont);
  }
  return 0;
}

void KColorPickerCtrl::OnPaint()
{
  CPaintDC dc(this);

  RECT client;
  GetClientRect(&client);

  client.bottom = DialogY2Pixel(60);

#if 0
  hsv.Hue = mHSV.Hue;

  for (int x = client.left; x < client.right; x++)
  {
    hsv.Sat = (BYTE)(100 * (x - client.left) / (client.right - client.left));
    for (int y = client.top; y < client.bottom; y++)
    {
      hsv.Val = (BYTE)(100 * (y - client.top) / (client.bottom - client.top));
      RGB_C rgb = ConvertHSV2RGB(hsv);
      dc.SetPixel(x, y, RGB(rgb.Red, rgb.Grn, rgb.Blu));
    }
  }
#else

  if (dc.m_ps.rcPaint.top < client.bottom)
  {
    if (mCacheMap.handle() == NULL)
    {
      mCacheMap.CreateBitmap(100, 100, 24, FALSE, NULL, 0, NULL);
      ReadyHSVCacheMap();
    }

    HBITMAP h = mCacheMap.handle();
    if (h != NULL)
    {
      CDC memdc;
      memdc.CreateCompatibleDC(&dc);
      HBITMAP old_bm = (HBITMAP)memdc.SelectObject(h);

      dc.StretchBlt(client.left, client.top, client.right - client.left, client.bottom - client.top,
        &memdc, 0, 0, 100, 100, SRCCOPY);
      memdc.SelectObject(old_bm);
    }
  }
#endif

  client.top = DialogY2Pixel(65);
  client.bottom = DialogY2Pixel(75);

#if 0
  HSV_C hsv;
  hsv.Val = 100;
  hsv.Sat = 100;

  if (dc.m_ps.rcPaint.bottom > client.top)
  {
    int hue_bar_height = client.bottom - client.top;
    for (int x = client.left; x < client.right; x++)
    {
      hsv.Hue = (short)(360 * (x - client.left) / (client.right - client.left));
      RGB_C rgb = ConvertHSV2RGB(hsv);

      dc.FillSolidRect(x, client.top, 1, hue_bar_height, RGB(rgb.Red, rgb.Grn, rgb.Blu));
    }
  }
#else
  if (dc.m_ps.rcPaint.bottom > client.top)
  {
    if (mHueMap.handle() == NULL)
    {
      mHueMap.CreateBitmap(180, 1, 24, FALSE, NULL, 0, NULL);
      ReadyHueCacheMap();
    }
    HBITMAP h = mHueMap.handle();

    if (h != NULL)
    {
      CDC memdc;
      memdc.CreateCompatibleDC(&dc);
      HBITMAP old_bm = (HBITMAP)memdc.SelectObject(h);

      dc.StretchBlt(client.left, client.top, client.right - client.left, client.bottom - client.top,
        &memdc, 0, 0, mHueMap.width, mHueMap.height, SRCCOPY);

      memdc.SelectObject(old_bm);
    }
  }
#endif

  // current color and palette
  RGB_C rgb = ConvertHSV2RGB(mHSV);

  if (mStyle & KCOLOR_WITH_DUAL)
  {
    if (mForeColor == 1)
    {
      DrawBackColor(&dc);
      DrawFrontColor(&dc);
    }
    else
    {
      DrawFrontColor(&dc);
      DrawBackColor(&dc);
    }
  }
  else
  {
    client.top = DialogY2Pixel(80);
    client.bottom = DialogY2Pixel(100);
    client.right = DialogX2Pixel(20);
    dc.FillSolidRect(client.left, client.top, client.right - client.left, client.bottom - client.top, RGB(rgb.Red, rgb.Grn, rgb.Blu));
  }

  client.top = DialogY2Pixel(80);
  // insert button
  int palette_left = DialogX2Pixel(42);
  int palette_width = DialogX2Pixel(10);
  int palette_interval = DialogX2Pixel(12);
  int palette_height = DialogY2Pixel(9);

  for (int c = 0; c < NO_CUSTOM_PALETTE; c++)
  {
    int x = (c / 2) * palette_interval + palette_left;
    int y = (c % 2) * DialogY2Pixel(11) + client.top;

    rgb = ConvertHSV2RGB(mCustomPalette[c]);
    dc.FillSolidRect(x, y, palette_width, palette_height, RGB(rgb.Red, rgb.Grn, rgb.Blu));
  }

}

void KColorPickerCtrl::DrawBackColor(CDC* pDC)
{
	RGB_C rgb = ConvertHSV2RGB(mBgHSV);
	RECT client;
	client.top = DialogY2Pixel(85);
	client.bottom = DialogY2Pixel(100);
	client.right = DialogX2Pixel(13);
	client.left = 0;
	pDC->FillSolidRect(client.left, client.top, client.right - client.left, client.bottom - client.top, 0);
	client.left++;
	client.top++;
	client.right--;
	client.bottom--;
	pDC->FillSolidRect(client.left, client.top, client.right - client.left, client.bottom - client.top, RGB(rgb.Red, rgb.Grn, rgb.Blu));
}

void KColorPickerCtrl::DrawFrontColor(CDC* pDC)
{
	RGB_C rgb = ConvertHSV2RGB(mHSV);
	RECT client;
	client.left = DialogY2Pixel(10);
	client.top = DialogY2Pixel(80);
	client.bottom = DialogY2Pixel(95);
	client.right = DialogX2Pixel(22);
  pDC->FillSolidRect(client.left, client.top, client.right - client.left, client.bottom - client.top, 0);
  client.left++;
  client.top++;
  client.right--;
  client.bottom--;
	pDC->FillSolidRect(client.left, client.top, client.right - client.left, client.bottom - client.top, RGB(rgb.Red, rgb.Grn, rgb.Blu));
}

void KColorPickerCtrl::OnSize(UINT nType, int cx, int cy)
{
}


HBRUSH KColorPickerCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_STATIC)
  {
    HBRUSH br = ::GetSysColorBrush(COLOR_WINDOW);
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
    return br;
  }
  return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL KColorPickerCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == EN_CHANGE)
  {
    CString str;
    HSV_C* p = mForeColor ? &mHSV : &mBgHSV;

    if (ctrl_id == 304)
    {
      mEditHue.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (p->Hue != val))
      {
        p->Hue = val;
        if (mSliderHue.GetSafeHwnd() != NULL)
          mSliderHue.SetPos(p->Hue);
        UpdateCurrentColor();
      }
    }
    else if (ctrl_id == 307)
    {
      mEditSaturation.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (p->Sat != val))
      {
        p->Sat = val;
        if (mSliderSaturation.GetSafeHwnd() != NULL)
          mSliderSaturation.SetPos(p->Sat);
        UpdateCurrentColor();
      }
    }
    else if (ctrl_id == 310)
    {
      mEditValue.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (p->Val != val))
      {
        p->Val = val;
        if (mSliderValue.GetSafeHwnd() != NULL)
          mSliderValue.SetPos(p->Val);
        UpdateCurrentColor();
      }
    }
    else if (ctrl_id == 314)
    {
      mEditAlpha.GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0) &&
        (mAlpha != val))
      {
        mAlpha = val;
        if (mSliderAlpha.GetSafeHwnd() != NULL)
          mSliderAlpha.SetPos(mAlpha);
        //UpdateCurrentColor();
      }
    }
    return 1;
  }
  else if (notify_code == BN_CLICKED)
  {
    if (ctrl_id == 101)
    {
      AddToCustomPalette(mHSV);
    }
  }
  return 0;
}

void KColorPickerCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  if (pScrollBar != NULL)
  {
    int id = pScrollBar->GetDlgCtrlID();
    int pos = ((CSliderCtrl*)pScrollBar)->GetPos();
    CString str;
    HSV_C* p = mForeColor ? &mHSV : &mBgHSV;

    if (id == 303)
    {
      short v = pos;
      if (v != p->Hue)
      {
        p->Hue = v;
        str.Format(_T("%d"), p->Hue);
        mEditHue.SetWindowText(str);
        UpdateCurrentColor();
        ReadyHSVCacheMap();
        UpdateScreen(1);
      }
    }
    else if (id == 306)
    {
      BYTE v = (BYTE)pos;
      if (v != p->Sat)
      {
        p->Sat = v;
        str.Format(_T("%d"), p->Sat);
        mEditSaturation.SetWindowText(str);
        UpdateCurrentColor();
        UpdateScreen(2);
      }
    }
    else if (id == 309)
    {
      BYTE v = (BYTE)pos;
      if (v != p->Val)
      {
        p->Val = pos;
        str.Format(_T("%d"), p->Val);
        mEditValue.SetWindowText(str);
        UpdateCurrentColor();
        UpdateScreen(2);
      }
    }
    else if (id == 313)
    {
      BYTE v = (BYTE)pos;
      if (v != mAlpha)
      {
        mAlpha = pos;
        str.Format(_T("%d"), mAlpha);
        mEditAlpha.SetWindowText(str);
      }
    }
  }
}

void KColorPickerCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
  if ((mMouseDragMode > 0) && (mMouseDragMode < 3))
  {
    HSV_C hsv;
    ColorModeFromPoint(point, &hsv, mMouseDragMode);
    OnSelectColorMap(mMouseDragMode, hsv);
  }
}

void KColorPickerCtrl::OnMouseLeave()
{
  mMouseDragMode = 0;
}

void KColorPickerCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
  HSV_C hsv;
  mMouseDragMode = ColorModeFromPoint(point, &hsv, 0);
  OnSelectColorMap(mMouseDragMode, hsv);
  SetCapture();
}

void KColorPickerCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
  mMouseDragMode = 0;
  ReleaseCapture();
}

BOOL KColorPickerCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  return TRUE;
 }

void KColorPickerCtrl::OnTimer(UINT_PTR timerEvent)
{
}

void KColorPickerCtrl::OnToggleForeBackColor()
{
  mForeColor = !mForeColor;
  UpdateColorSlider(7);
  Invalidate(FALSE);
}

void KColorPickerCtrl::OnSelectColorMap(int mode, HSV_C& hsv)
{
  HSV_C* p = mForeColor ? &mHSV : &mBgHSV;

  if (mode == 1)
  {
    if (p->Hue != hsv.Hue)
    {
      p->Hue = hsv.Hue;
      UpdateColorSlider(1);

      ReadyHSVCacheMap();
      UpdateScreen(1);
      UpdateCurrentColor();
    }
  }
  else if (mode == 2)
  {
    p->Sat = hsv.Sat;
    p->Val = hsv.Val;
    UpdateColorSlider(6);
    UpdateScreen(2);
    UpdateCurrentColor();
  }
  else if (mode > 2)
  {
    p->Hue = hsv.Hue;
    p->Sat = hsv.Sat;
    p->Val = hsv.Val;
    UpdateColorSlider(7);

    ReadyHSVCacheMap();
    UpdateScreen(2);
    UpdateCurrentColor();
  }
}

void KColorPickerCtrl::SetMode(int mode)
{
  if (mStyle != mode)
  {
    mStyle = mode;
    Invalidate();
  }
}

void KColorPickerCtrl::SetColor(HSV_C hsv, BOOL foreColor)
{
  int update = 0;
  HSV_C* p = foreColor ? &mHSV : &mBgHSV;

  if (hsv.Hue != p->Hue)
  {
    update = 2;
    p->Hue = hsv.Hue;

    if (foreColor == mForeColor)
      ReadyHSVCacheMap();
  }
  if ((hsv.Sat != p->Sat) || (hsv.Val != p->Val))
  {
    update |= 1;
    p->Sat = hsv.Sat;
    p->Val = hsv.Val;
  }

  Invalidate(FALSE);
  /*
  if (update == 3)
    Invalidate(FALSE);
  else if (update > 0)
    UpdateScreen(update);
  */
}

DWORD KColorPickerCtrl::MakeARGBColor(RGB_C rgb, BYTE alpha)
{
  return MAKELONG(MAKEWORD(rgb.Blu, rgb.Grn), MAKEWORD(rgb.Red, alpha));
}

void KColorPickerCtrl::SetARGBColor(DWORD color, BOOL foreColor)
{
  RGB_C rgb;
  WORD w = LOWORD(color);
  rgb.Blu = LOBYTE(w);
  rgb.Grn = HIBYTE(w);
  w = HIWORD(color);
  rgb.Red = LOBYTE(w);
  mAlpha = HIBYTE(w);

  if (foreColor)
    mHSV = ConvertRGB2HSV(rgb);
  else
    mBgHSV = ConvertRGB2HSV(rgb);

  UpdateColorSlider(0x0F);

  Invalidate(FALSE);
}

DWORD KColorPickerCtrl::GetARGBColor(BOOL foreColor)
{
  RGB_C rgb = ConvertHSV2RGB(foreColor ? mHSV : mBgHSV);
  //DWORD r = MAKELONG(MAKEWORD(rgb.Blu, rgb.Grn), MAKEWORD(rgb.Red, mAlpha));
  return MakeARGBColor(rgb, mAlpha);
}

void KColorPickerCtrl::UpdateScreen(int mode)
{
  Invalidate(FALSE);

  /*
  RECT client;
  GetClientRect(&client);

  int hue_bar_height = (client.bottom - client.top) / 3;
  int height = DialogY2Pixel(10);

  if (hue_bar_height > height)
    hue_bar_height = height;

  if (mode == 1)
  {
    client.top = client.bottom - hue_bar_height;
  }
  else if (mode == 2)
  {
    client.bottom -= hue_bar_height * 3 / 2;
  }
  InvalidateRect(&client, FALSE);
  */
}

int KColorPickerCtrl::ColorModeFromPoint(POINT& point, HSV_C* hsv, int mode)
{
  RECT client;
  GetClientRect(&client);

  int ys = DialogY2Pixel(65);
  int height = DialogY2Pixel(10);

  // force to inside of hue-bar
  if (mode == 1)
  {
    if (point.y < ys)
      point.y = ys;
    if (point.y >= ys + height)
      point.y = ys + height - 1;
  }
  if (mode != 0)
  {
    if (point.x < client.left)
      point.x = client.left;
    if (point.x > client.right)
      point.x = client.right;
  }

  if ((mode < 2) &&
    (ys <= point.y) && (point.y <= (ys + height)))
  {
    if (hsv != NULL)
      hsv->Hue = (short)(360 * (point.x - client.left) / (client.right - client.left));
    return 1;//lower strip
  }

  ys = 0;
  height = DialogY2Pixel(60);

  if (mode == 2)
  {
    if (point.y < ys)
      point.y = ys;
    if (point.y >= ys + height)
      point.y = ys + height - 1;
  }

  if (((mode == 0) || (mode == 2)) &&
    (ys <= point.y) && (point.y <= ys + height))
  {
    hsv->Sat = (BYTE)(100 * (point.x - client.left) / (client.right - client.left));
    hsv->Val = (BYTE)(100 * (point.y - ys) / height);
    return 2;//upper box
  }

  if (mode == 0)
  {
    // check palette
    int palette_top = DialogY2Pixel(80);
    int palette_bottom = DialogY2Pixel(100);
    int palette_left = DialogX2Pixel(42);

    if ((palette_left <= point.x) &&
      (palette_top <= point.y) && (point.y < palette_bottom))
    {
      int palette_width = DialogX2Pixel(10);
      int palette_interval = DialogX2Pixel(12);
      int palette_height = DialogY2Pixel(9);
      int palette_y_interval = DialogY2Pixel(11);

      int cx = (point.x - palette_left) / palette_interval;
      int ox = (point.x - palette_left) % palette_interval;
      int cy = (point.y - palette_top) / palette_y_interval;
      int oy = (point.y - palette_top) / palette_y_interval;

      if ((ox < palette_width) && (oy < palette_height))
      {
        int ci = cx * 2 + cy;
        if ((0 <= ci) && (ci < NO_CUSTOM_PALETTE))
        {
          *hsv = mCustomPalette[ci];
          return (3 + ci);
        }
      }
    }
  }


  if (mStyle & KCOLOR_WITH_DUAL)
  {
	  RECT back, front;
	  back.top = DialogY2Pixel(85);
	  back.bottom = DialogY2Pixel(100);
	  back.right = DialogX2Pixel(13);
	  back.left = 0;

	  front.left = DialogY2Pixel(10);
	  front.top = DialogY2Pixel(80);
	  front.bottom = DialogY2Pixel(95);
	  front.right = DialogX2Pixel(22);

	  // back
	  if ((mForeColor == 1 && point.x <= back.right && point.x >= back.left && point.y >= back.top && point.y <= back.bottom)
		  || (mForeColor == 0 && point.x <= front.right && point.x >= front.left && point.y >= front.top && point.y <= front.bottom))
	  {
		  OnToggleForeBackColor();
	  }
  }

  return 0;
}

void KColorPickerCtrl::ReadyHSVCacheMap()
{
  HSV_C hsv;
  hsv.Hue = mHSV.Hue;

  for (int y = 0; y < 100; y++)
  {
    hsv.Val = (BYTE)y;
    BYTE* d = mCacheMap._GetScanline(y);
    for (int x = 0; x < 100; x++)
    {
      hsv.Sat = (BYTE)x;
      RGB_C rgb = ConvertHSV2RGB(hsv);

      *d++ = rgb.Blu;
      *d++ = rgb.Grn;
      *d++ = rgb.Red;
    }
  }
}

void KColorPickerCtrl::ReadyHueCacheMap()
{
  BYTE* d = mHueMap._GetScanline(0);
  HSV_C hsv;
  hsv.Val = 100;
  hsv.Sat = 100;

  for (int x = 0; x < mHueMap.width; x++)
  {
    hsv.Hue = (short)(x * 360 / mHueMap.width);
    RGB_C rgb = ConvertHSV2RGB(hsv);
    *d++ = rgb.Blu;
    *d++ = rgb.Grn;
    *d++ = rgb.Red;
  }
}

void KColorPickerCtrl::UpdateColorSlider(int param)
{
  CString str;
  HSV_C* p = mForeColor ? &mHSV : &mBgHSV;

  if ((param & 1) && (mEditHue.GetSafeHwnd() != NULL))
  {
    str.Format(_T("%d"), p->Hue);
    mEditHue.SetWindowText(str);
    mSliderHue.SetPos(p->Hue);
  }

  if ((param & 2) && (mEditSaturation.GetSafeHwnd() != NULL))
  {
    str.Format(_T("%d"), p->Sat);
    mEditSaturation.SetWindowText(str);
    mSliderSaturation.SetPos(p->Sat);
  }
  if ((param & 4) && (mEditValue.GetSafeHwnd() != NULL))
  {
    str.Format(_T("%d"), p->Val);
    mEditValue.SetWindowText(str);
    mSliderValue.SetPos(p->Val);
  }

  if ((param & 8) && (mEditAlpha.GetSafeHwnd() != NULL))
  {
    str.Format(_T("%d"), mAlpha);
    mEditAlpha.SetWindowText(str);
    mSliderAlpha.SetPos(mAlpha);
  }
}

void KColorPickerCtrl::UpdateCurrentColor(BOOL bNotify)
{
  RGB_C rgb = ConvertHSV2RGB(mForeColor ? mHSV : mBgHSV);
  // RGB_C rgb = ConvertHSV2RGB(mHSV);

  if (mEditHexaColor.GetSafeHwnd() != NULL)
  {
    CString str;
    str.Format(L"#%02X%02X%02X", rgb.Red, rgb.Grn, rgb.Blu);
    mEditHexaColor.SetWindowText(str);
  }

  CWnd* p = GetParent();
  if (bNotify && (p != NULL))
  {
    WPARAM param = MakeARGBColor(rgb, mAlpha);
    LPARAM lparam = MAKELPARAM(mForeColor, GetDlgCtrlID());
    p->PostMessage(WMA_CHANGE_COLOR, param, lparam);
  }
}

void KColorPickerCtrl::AddToCustomPalette(HSV_C hsv)
{
  if ((hsv.Hue != mCustomPalette[0].Hue) ||
    (hsv.Sat != mCustomPalette[0].Sat) ||
    (hsv.Val != mCustomPalette[0].Val))
  {
    for (int c = NO_CUSTOM_PALETTE - 2; c >= 0 ; c--)
      mCustomPalette[c+1] = mCustomPalette[c];
    mCustomPalette[0] = hsv;
    UpdateScreen(4);
  }
}

