// KImageOutputLevelCtrl.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\VivaImaging.h"
#include "..\Core\KConstant.h"
#include "..\Platform\Graphics.h"

#include "KImageOutputLevelCtrl.h"

// KImageOutputLevelCtrl

IMPLEMENT_DYNAMIC(KImageOutputLevelCtrl, CWnd)

KImageOutputLevelCtrl::KImageOutputLevelCtrl()
{
  RegisterWndClass();

  mpHistogramData = NULL;
  mCtrlId = 0;
  mMode = 0;

  mLevel[0] = 0;
  mLevel[1] = 128;
  mLevel[2] = 255;

  mTrackDrag = -1;
}

KImageOutputLevelCtrl::~KImageOutputLevelCtrl()
{
  if (mpHistogramData != NULL)
  {
    delete mpHistogramData;
    mpHistogramData = NULL;
  }
}


BEGIN_MESSAGE_MAP(KImageOutputLevelCtrl, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// KImageOutputLevelCtrl 메시지 처리기

#define LEVEL_TRACK_HALF_WIDTH 5
#define LEVEL_TRACK_HEIGHT 16

TCHAR K_IMAGE_OUTPUT_LEVEL_CTRL_CLASS[] = _T("KIMAGEOUTPUTLEVELCTRL");
TCHAR K_IMAGE_OUTPUT_LEVEL_CTRL_WINDOW[] = _T("KImageOutputLevelCtrl");

BOOL KImageOutputLevelCtrl::RegisterWndClass()
{
  WNDCLASS windowclass;
  HINSTANCE hInst = AfxGetInstanceHandle();
  if (!(::GetClassInfo(hInst, K_IMAGE_OUTPUT_LEVEL_CTRL_CLASS, &windowclass)))
  {
    windowclass.style = CS_HREDRAW;
    windowclass.lpfnWndProc = ::DefWindowProc;
    windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
    windowclass.hInstance = hInst;
    windowclass.hIcon = NULL;
    windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
    windowclass.lpszMenuName = NULL;
    windowclass.lpszClassName = K_IMAGE_OUTPUT_LEVEL_CTRL_CLASS;
    if (!AfxRegisterClass(&windowclass))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }
  return TRUE;
}

int KImageOutputLevelCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  int height = DialogY2Pixel(16); //  PC_ITEM_HEIGHT);
  int dy = lpCreateStruct->cy - height;
  int width = DialogX2Pixel(30);

  TCHAR str[16];

  mEditLevel[0].Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(0, dy, width, dy + height), this, 401);
  mEditLevel[0].SetFont(gpUiFont);
  StringCchPrintf(str, 16, _T("%d"), mLevel[0]);
  mEditLevel[0].SetWindowText(str);

  int x = (mMode & IMAGE_INPUT_LEVEL) ? (lpCreateStruct->cx - width) / 2 : (lpCreateStruct->cx - width);
  mEditLevel[1].Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
    CRect(x, dy, x + width, dy + height), this, 402);
  mEditLevel[1].SetFont(gpUiFont);
  StringCchPrintf(str, 16, _T("%d"), mLevel[1]);
  mEditLevel[1].SetWindowText(str);

  if (mMode & IMAGE_INPUT_LEVEL)
  {
    mEditLevel[2].Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
      CRect(lpCreateStruct->cx - width, dy, lpCreateStruct->cx, dy + height), this, 403);
    mEditLevel[2].SetFont(gpUiFont);
    StringCchPrintf(str, 16, _T("%d"), mLevel[2]);
    mEditLevel[2].SetWindowText(str);

    if (mpHistogramData == NULL)
    {
      mpHistogramData = new BYTE[256];
      memset(mpHistogramData, 0, 256);
    }
  }

  return 0;
}

BOOL KImageOutputLevelCtrl::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId, int mode)
{
  mMode = mode;

  mLevel[0] = 20;
  mLevel[1] = (mMode & IMAGE_INPUT_LEVEL) ? 128 : 255;
  mLevel[2] = 255;
  mCtrlId = (WORD)controlId;

  return CWnd::Create(K_IMAGE_OUTPUT_LEVEL_CTRL_CLASS, K_IMAGE_OUTPUT_LEVEL_CTRL_WINDOW,
    windowStyle, rect, parent, controlId);
}

void KImageOutputLevelCtrl::drawTrackHandle(CDC& dc, int xp, int yp)
{
  int dx = DialogX2Pixel(LEVEL_TRACK_HALF_WIDTH);
  int h = DialogY2Pixel(LEVEL_TRACK_HEIGHT);
  POINT trackHandle[5] = {
    { -dx, dx },
  { -dx, h },
  { dx, h },
  { dx, dx },
  { 0, 0 }
  };

  for (int i = 0; i < 5; i++)
  {
    trackHandle[i].x += xp;
    trackHandle[i].y += yp;
  }
  dc.Polygon(trackHandle, 5);
}

void KImageOutputLevelCtrl::drawImageHistogram(CDC& dc, RECT& rect)
{
#define HISTOGRAM_CHART_COLOR 0x404040
  if (rect.right > rect.left)
  {
    for (int x = rect.left; x < rect.right; x++)
    {
      int dx = x * 255 / (rect.right - rect.left - 1);
      int level = (rect.bottom - rect.top) - mpHistogramData[dx] * (rect.bottom - rect.top) / 255;

      dc.FillSolidRect(x, rect.top + level, 1, rect.bottom - rect.top - level, HISTOGRAM_CHART_COLOR);
    }
  }
}

void KImageOutputLevelCtrl::OnPaint()
{
  CPaintDC dc(this);
  RECT rect;
  GetClientRect(&rect);

  int dy = 0;
  int bar_height = DialogY2Pixel(46);

  if (mMode & IMAGE_INPUT_LEVEL)
  {
    rect.bottom -= bar_height;

    if (dc.m_ps.rcPaint.top < rect.bottom)
    {
      if (mpHistogramData != NULL)
        drawImageHistogram(dc, rect);
    }

    rect.top = rect.bottom;
    rect.bottom += bar_height;
    dy = rect.top;
  }

  TRIVERTEX tv[2] = {
    {0, 0, 0, 0, 0, 0xFF00 },
    { 0, 0, 0xFF00, 0xFF00, 0xFF00, 0xFF00 } };
  GRADIENT_RECT grect = { 0, 1 };

  bar_height = DialogY2Pixel(10);
  tv[0].y = dy;
  tv[1].x = rect.right;
  tv[1].y = dy + bar_height;

  if (dc.m_ps.rcPaint.top < (dy + bar_height))
  {
    dc.GradientFill(tv, 2, &grect, 1, GRADIENT_FILL_RECT_H);
  }
  dy += bar_height;

  CBrush brush;
  brush.CreateSolidBrush(0x0E3B157);
  CBrush* old_brush = (CBrush*)dc.SelectObject(&brush);
  CPen* old_pen = (CPen*)dc.SelectStockObject(NULL_PEN);

  int x = mLevel[0] * (rect.right - rect.left) / 255;
  drawTrackHandle(dc, x, dy);
  int p = 1;
  if (mMode & IMAGE_INPUT_LEVEL)
  {
    x = mLevel[p] * (rect.right - rect.left) / 255;
    drawTrackHandle(dc, x, dy);
    p++;
  }
  x = mLevel[p] * (rect.right - rect.left) / 255;
  drawTrackHandle(dc, x, dy);

  dc.SelectObject(old_brush);
  //dc.SelectObject(old_pen);
}


void KImageOutputLevelCtrl::OnSize(UINT nType, int cx, int cy)
{
}

void KImageOutputLevelCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
  if (mTrackDrag >= 0)
  {
    RECT rect;
    GetClientRect(&rect);

    // check min max
    point.x -= mTrackDragXOffset;
    if (point.x < rect.left)
      point.x = rect.left;
    if (point.x > rect.right)
      point.x = rect.right;

    BYTE v = (BYTE)((point.x - rect.left) * 255 / (rect.right - rect.left));
    SetLevelValue(mTrackDrag, v, TRUE);
  }
}

void KImageOutputLevelCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
  RECT rect;
  // int bar_height = DialogY2Pixel(10);
  int dy; // = bar_height * ((mMode & IMAGE_INPUT_LEVEL) ? 6 : 1);
  int h = DialogY2Pixel(LEVEL_TRACK_HEIGHT);

  GetClientRect(&rect);
  if (mMode & IMAGE_INPUT_LEVEL)
    dy = rect.bottom - DialogY2Pixel(36);
  else
    dy = DialogY2Pixel(10);

  if ((dy < point.y) && (point.y < (dy + h)))
  {
    int count = (mMode & IMAGE_INPUT_LEVEL) ? 3 : 2;
    int w = DialogX2Pixel(LEVEL_TRACK_HALF_WIDTH);
    for (int p = 0; p < count; p++)
    {
      int x = mLevel[p] * (rect.right - rect.left) / 255;
      if (((x - w) < point.x) && (point.x < (x + w)))
      {
        mTrackDrag = p;
        mTrackDragXOffset = point.x - x;
        SetCapture();
        return;
      }
    }
  }
}

void KImageOutputLevelCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
  mTrackDrag = -1;
  ReleaseCapture();
}


BOOL KImageOutputLevelCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
  WORD notify_code = HIWORD(wParam);
  WORD ctrl_id = LOWORD(wParam);
  if (notify_code == EN_CHANGE)
  {
    if ((401 <= ctrl_id) && (ctrl_id <= 403))
    {
      int p = ctrl_id - 401;
      CString str;
      mEditLevel[p].GetWindowText(str);
      int val = 0;
      if ((str.GetLength() > 0) &&
        (swscanf_s(str.GetBuffer(), _T("%d"), &val) > 0))
      {
        if (val < 0)
          val = 0;
        if (val > 255)
          val = 255;

        SetLevelValue(p, val, FALSE);
      }
    }
    return 1;
  }
  return 0;
}


void KImageOutputLevelCtrl::SetLevelValue(int m, BYTE value, BOOL updateEdit)
{
  // check low or high
  int last_index = (mMode & IMAGE_INPUT_LEVEL) ? 2 : 1;
  if ((m > 0) && (value < mLevel[m - 1]))
  {
    value = mLevel[m - 1];
  }
  if ((m < last_index) && (value > mLevel[m + 1]))
  {
    value = mLevel[m + 1];
  }

  if (value != mLevel[m])
  {
    RECT rect;
    GetClientRect(&rect);

    if (mMode & IMAGE_INPUT_LEVEL)
      rect.top = rect.bottom - DialogY2Pixel(36);
    else
      rect.top = DialogY2Pixel(10);

    //int bar_height = DialogY2Pixel(10);
    //rect.top = bar_height * ((mMode & IMAGE_INPUT_LEVEL) ? 6 : 1);
    rect.bottom = rect.top + DialogY2Pixel(LEVEL_TRACK_HEIGHT);

    mLevel[m] = value;

    if (updateEdit)
    {
      TCHAR str[16];
      StringCchPrintf(str, 16, _T("%d"), mLevel[m]);
      mEditLevel[m].SetWindowText(str);
    }

    GetParent()->PostMessage(NOL_CHANGED, MAKEWPARAM(mCtrlId, m), 0L);
    InvalidateRect(&rect);
  }
}

void KImageOutputLevelCtrl::SetCurrentValue(BYTE v1, BYTE v2, BYTE v3)
{
  mLevel[0] = v1;
  mLevel[1] = v2;
  mLevel[2] = v3;

  int count = (mMode & IMAGE_INPUT_LEVEL) ? 3 : 2;
  for (int m = 0; m < count; m++)
  {
    TCHAR str[16];
    StringCchPrintf(str, 16, _T("%d"), mLevel[m]);
    mEditLevel[m].SetWindowText(str);
  }
  Invalidate(TRUE);
}

void KImageOutputLevelCtrl::GetCurrentValue(BYTE *v1, BYTE* v2, BYTE* v3)
{
  if (v1 != NULL)
    *v1 = mLevel[0];
  if (v2 != NULL)
    *v2 = mLevel[1];
  if (v3 != NULL)
    *v3 = mLevel[2];
}
