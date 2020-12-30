#include "StdAfx.h"
#include "Graphics.h"
// #include "../resource.h"
#include <Strsafe.h>

#include "Platform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFont* gpUiFont = NULL;
int gDlgFontBaseWidth = 0;
int gDlgFontBaseHeight = 0;
TCHAR gDefaultFacename[LF_FACESIZE];
// #define ENABLE_INTER_THREAD_SET_FOREGROUND_WND

void MakeTopActive(HWND hWnd, int toCenter, HWND parent)
{
  /**
  ::SetForegroundWindow(hwnd);

  HWND p = ::GetParent(hwnd);
  if (p != NULL)
    ::SetActiveWindow(p);
  ::SetActiveWindow(hwnd);
  */
  if(!::IsWindow(hWnd))
    return;
 
	//relation time of SetForegroundWindow lock
	DWORD lockTimeOut = 0;
	HWND hCurrWnd = parent;
  if (!::IsWindow(hCurrWnd))
    hCurrWnd = ::GetForegroundWindow();

#ifdef ENABLE_INTER_THREAD_SET_FOREGROUND_WND
	DWORD dwThisTID = ::GetCurrentThreadId();
	DWORD dwCurrTID = ::GetWindowThreadProcessId(hCurrWnd,0);
 
	//we need to bypass some limitations from Microsoft :)
	if(dwThisTID != dwCurrTID)
	{
		::AttachThreadInput(dwThisTID, dwCurrTID, TRUE);
 
		::SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT,0,&lockTimeOut,0);
		::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,0,0,SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
 
		::AllowSetForegroundWindow(ASFW_ANY);
	}
 #endif

	::SetForegroundWindow(hWnd);
  ::BringWindowToTop(hWnd);
 
#ifdef ENABLE_INTER_THREAD_SET_FOREGROUND_WND
	if(dwThisTID != dwCurrTID)
	{
		::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,0,(PVOID)lockTimeOut,SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
		::AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
	}
#endif

  // center Window
  if (toCenter)
  {
    MakeCenterWindow(hCurrWnd, hWnd, toCenter);
  }
}

BOOL GetCenterFromWindow(HWND hParentWnd, HWND hWnd, LPRECT rect)
{
  if (hParentWnd != NULL)
  {
    RECT area;

    ::GetWindowRect(hParentWnd, &area);
    ::GetWindowRect(hWnd, rect);

    int w = (rect->right - rect->left);
    int h = (rect->bottom - rect->top);
    rect->left = area.left + ((area.right - area.left) - w) / 2;
    rect->top = area.top + ((area.bottom - area.top) - h) / 2;
    rect->right = rect->left + w;
    rect->bottom = rect->top + h;
    return TRUE;
  }
  return FALSE;
}

BOOL GetCenterWindow(HWND hCurrWnd, HWND hWnd, LPRECT rect)
{
  //HMONITOR m = MonitorFromWindow(hCurrWnd, MONITOR_DEFAULTTOPRIMARY);
  POINT pt = { 1, 1 };
  HMONITOR m = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
	if(m != NULL)
	{
		MONITORINFO mi;

		mi.cbSize = sizeof(MONITORINFO);
		if(GetMonitorInfo(m, &mi))
    {
      ::GetWindowRect(hWnd, rect);
      int mw = mi.rcMonitor.right - mi.rcMonitor.left;
      int mh = mi.rcMonitor.bottom - mi.rcMonitor.top;
      int ox = (mw - (rect->right - rect->left)) / 2;
      int oy = (mh - (rect->bottom - rect->top)) / 2;
      if(ox < 0)
        ox = 0;
      if(oy < 0)
        oy = 0;

      rect->right = ox + (rect->right - rect->left);
      rect->bottom = oy + (rect->bottom - rect->top);
      rect->left = ox;
      rect->top = oy;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL MakeCenterWindow(HWND hCurrWnd, HWND hWnd, int toCenter)
{
  RECT rect;
  BOOL r;

  if (toCenter & KWND_CENTER)
    r = GetCenterWindow(hCurrWnd, hWnd, &rect);
  else if (toCenter & KWND_PARENT_CENTER)
    r = GetCenterFromWindow(hCurrWnd, hWnd, &rect);
  else
    r = GetWindowRect(hWnd, &rect);

  if (r)
  {
    ::SetWindowPos(hWnd, ((toCenter & KWND_TOPMOST) ? HWND_TOPMOST : HWND_TOP),
			rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top),
			SWP_FRAMECHANGED);
    return TRUE;
  }
  return FALSE;
}

void DrawImageCenter(HDC hDC, RECT &area, HBITMAP hbm, int rop)
{
  BITMAP bm;
  RECT rect;

  GetObject(hbm, sizeof(bm), &bm);
  int oy = (area.bottom - area.top - bm.bmHeight) / 2;
  int ox = (area.right - area.left - bm.bmWidth) / 2;
  rect.left = area.left + ox;
  rect.top = area.top + oy;
  rect.right = rect.left + bm.bmWidth;
  rect.bottom = rect.top + bm.bmHeight;

  HDC memDC = ::CreateCompatibleDC(hDC);
  HBITMAP oldbmp = (HBITMAP)::SelectObject(memDC, hbm);
  ::BitBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
    memDC, 0, 0, rop);
  ::SelectObject(memDC, oldbmp);
  ::DeleteDC(memDC);
}

void DrawImageFrame(HDC hDC, RECT& rect, HBITMAP hbm, int frame, int total)
{
  HDC memDC = ::CreateCompatibleDC(hDC);
  HBITMAP oldbmp = (HBITMAP)::SelectObject(memDC, hbm);
  int yp = (rect.bottom - rect.top) * frame;
  ::BitBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
    memDC, 0, yp, SRCCOPY);
  ::SelectObject(memDC, oldbmp);
  ::DeleteDC(memDC);
}

#ifdef USE_GDIPLUS_TRANSPARENT_IMAGE

#ifdef USE_GDIPLUS_HBITMAP

HBITMAP LoadTransparentImage(UINT resource_id)
{
  HINSTANCE instance = AfxGetInstanceHandle();
  HRSRC hResource = ::FindResource(instance, MAKEINTRESOURCE(resource_id), _T("PNG"));
  if (!hResource)
    return NULL;
    
  DWORD imageSize = ::SizeofResource(instance, hResource);
  if (!imageSize)
    return NULL;

  const void* pResourceData = ::LockResource(::LoadResource(instance, hResource));
  if (!pResourceData)
    return NULL;

  HBITMAP hbitmap = NULL;
  HGLOBAL buffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
  if (buffer != NULL)
  {
    void* pBuffer = ::GlobalLock(buffer);
    if (pBuffer)
    {
      CopyMemory(pBuffer, pResourceData, imageSize);

      IStream* pStream = NULL;
      if (::CreateStreamOnHGlobal(buffer, FALSE, &pStream) == S_OK)
      {
        Bitmap *bitmap = Bitmap::FromStream(pStream);
        Color bgcolor(0,0,0,0);
        bitmap->GetHBITMAP(bgcolor, &hbitmap);
        pStream->Release();
        delete bitmap;
      }
      ::GlobalUnlock(buffer);
    }
  ::GlobalFree(buffer);
  }
  return hbitmap;
}

void DrawTransparentImage(HDC hDC, RECT rect, HBITMAP hbm, BOOL asGray)
{
  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = asGray ? 0x70 : 0xff;
  bf.AlphaFormat = AC_SRC_ALPHA;

  BITMAP bm;
  if (GetObject(hbm, sizeof(BITMAP), &bm))
  {
    HDC memDC = CreateCompatibleDC(hDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hbm);

    if ((rect.right - rect.left) > bm.bmWidth)
    {
      rect.left += ((rect.right - rect.left) - bm.bmWidth) / 2;
      rect.right = rect.left  + bm.bmWidth;
    }
    if ((rect.bottom - rect.top) > bm.bmHeight)
    {
      rect.top += ((rect.bottom - rect.top) - bm.bmHeight) / 2;
      rect.bottom = rect.top + bm.bmHeight;
    }

    BOOL r = AlphaBlend (hDC, rect.left, rect.top, bm.bmWidth, bm.bmHeight, memDC, 0, 0, bm.bmWidth, bm.bmHeight, bf);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
  }
}

void DrawTransparentImageFrame(HDC hDC, RECT rect, HBITMAP hbm, int frame, int total, BOOL asGray)
{
  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = asGray ? 0x70 : 0xff;
  bf.AlphaFormat = AC_SRC_ALPHA;

  BITMAP bm;
  if (GetObject(hbm, sizeof(BITMAP), &bm))
  {
    HDC memDC = CreateCompatibleDC(hDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hbm);

    int height = bm.bmHeight / total;
    int sy = frame * height;

    BOOL r = AlphaBlend (hDC, rect.left, rect.top, bm.bmWidth, height, memDC, 0, sy, bm.bmWidth, height, bf);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
  }
}

void DrawTransparentImageHorz(HDC hDC, RECT rect, HBITMAP hbm, int frame, int total, BOOL asGray)
{
  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = asGray ? 0x70 : 0xff;
  bf.AlphaFormat = AC_SRC_ALPHA;

  BITMAP bm;
  if (GetObject(hbm, sizeof(BITMAP), &bm))
  {
    HDC memDC = CreateCompatibleDC(hDC);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hbm);

    int width = bm.bmWidth / total;
    int sx = frame * width;

    BOOL r = AlphaBlend (hDC, rect.left, rect.top, width, bm.bmHeight, memDC, sx, 0, width, bm.bmHeight, bf);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
  }
}

#else

Image *LoadTransparentImage(UINT resource_id)
{
  HINSTANCE instance = AfxGetInstanceHandle();
  HRSRC hResource = ::FindResource(instance, MAKEINTRESOURCE(resource_id), _T("PNG"));
  if (!hResource)
    return NULL;
    
  DWORD imageSize = ::SizeofResource(instance, hResource);
  if (!imageSize)
    return NULL;

  const void* pResourceData = ::LockResource(::LoadResource(instance, hResource));
  if (!pResourceData)
    return NULL;

  Image *image = NULL;
  HGLOBAL buffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
  if (buffer != NULL)
  {
    void* pBuffer = ::GlobalLock(buffer);
    if (pBuffer)
    {
      CopyMemory(pBuffer, pResourceData, imageSize);

      IStream* pStream = NULL;
      if (::CreateStreamOnHGlobal(buffer, FALSE, &pStream) == S_OK)
      {
        image = Image::FromStream(pStream);

        // bitmap = Gdiplus::Bitmap::FromStream(pStream);
        pStream->Release();
        if (image)
        {
          if (false) // image->GetLastStatus() != Gdiplus::Ok)
          {
            //return bitmap;

            delete image;
            image = NULL;
          }
        }
      }
      ::GlobalUnlock(buffer);
    }
  ::GlobalFree(buffer);
  }
  return image;
}

void DrawTransparentImage(HDC hDC, RECT& sub, Image *image)
{
  Graphics graphics(hDC);
  RectF dest((REAL)sub.left, (REAL)sub.top, (REAL)(sub.right - sub.left), (REAL)(sub.bottom - sub.top));

  if (dest.Width > image->GetWidth())
  {
    dest.X += (dest.Width - image->GetWidth()) / 2;
    dest.Width = (REAL)image->GetWidth();
  }
  if (dest.Height > image->GetHeight())
  {
    dest.Y += (dest.Height - image->GetHeight()) / 2;
    dest.Height = (REAL)image->GetHeight();
  }
  graphics.DrawImage(image, dest);
}

void DrawTransparentImageFrame(HDC hDC, RECT& sub, Image *image, int frame, int total)
{
  Graphics graphics(hDC);
  int x = sub.left;
  int y = sub.top;

  int width = sub.right - sub.left;
  if (width > (int)image->GetWidth())
  {
    x += (width - image->GetWidth()) / 2;
    width = image->GetWidth();
  }
  int dest_height = sub.bottom - sub.top;
  int height = image->GetHeight() / total;
  if (dest_height > height)
  {
    y += (dest_height - height) / 2;
  }
  int sy = frame * height;
  graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
  graphics.SetPageUnit(UnitPixel);
  graphics.DrawImage(image, x, y, 0, sy, width, height, UnitPixel);
  graphics.SetPixelOffsetMode(PixelOffsetModeNone);
}

#endif // USE_GDIPLUS_HBITMAP

#endif // USE_GDIPLUS_TRANSPARENT_IMAGE

#include <afxwin.h>
#include <CommCtrl.h>

HWND CreateToolTipWnd(HWND hDlg, HWND hItem, LPCTSTR pszText, LPRECT pRect)
{
  if (!hDlg || !hItem || !pszText)
    return FALSE;

  HINSTANCE hInst = AfxGetInstanceHandle();

  // Create the tooltip. g_hInst is the global instance handle.
  DWORD wflag = (pRect != NULL) ? WS_EX_TOPMOST : 0;

  HWND hwndTip = CreateWindowEx(wflag, TOOLTIPS_CLASS, NULL,
      WS_POPUP |TTS_ALWAYSTIP, // TTS_BALLOON
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT,
      hDlg, NULL, 
      hInst, NULL);

  if (!hwndTip)
    return (HWND)NULL;

  SetWindowPos(hwndTip, HWND_TOPMOST, 0, 0, 0, 0, 
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  // Associate the tooltip with the tool.
  TOOLINFO toolInfo = { 0 };
  toolInfo.cbSize = sizeof(toolInfo);
  toolInfo.hwnd = hDlg;
  toolInfo.uFlags = TTF_SUBCLASS;
  if (pRect == NULL)
  {
    toolInfo.uFlags |= TTF_IDISHWND;
    toolInfo.uId = (UINT_PTR)hItem;
  }
  else
    toolInfo.rect = *pRect;

  toolInfo.lpszText = (LPWSTR)pszText;
  SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

  return hwndTip;
}

int CharacterHeightToPointSize(int charHeight)
{
  HWND desktopWnd = ::GetDesktopWindow();
  HDC pDC = ::GetDC(desktopWnd);
  int ptSize = MulDiv(abs(charHeight), 72, GetDeviceCaps(pDC, LOGPIXELSY));
  ::ReleaseDC(desktopWnd, pDC);
  return ptSize;
}

int PointSizeToCharacterHeight(int pointSize)
{
  HWND desktopWnd = ::GetDesktopWindow();
  HDC pDC = ::GetDC(desktopWnd);
  int charHeight = -MulDiv(pointSize, GetDeviceCaps(pDC, LOGPIXELSY), 72);
  ::ReleaseDC(desktopWnd, pDC);
  return charHeight;
}

#define COMMON_DLG_UNIT // not use GetDialogBaseUnits()

void DialogUnit2Pixel(POINT &d)
{
#ifdef COMMON_DLG_UNIT
  d.x = MulDiv(d.x, gDlgFontBaseWidth, 4);
  d.y = MulDiv(d.y, gDlgFontBaseHeight, 8);
#else
  LONG unit = GetDialogBaseUnits();
  int baseunitX = LOWORD(unit);
  int baseunitY = HIWORD(unit);
  d.x = MulDiv(d.x, baseunitX, 4);
  d.y = MulDiv(d.y, baseunitY, 8);
#endif
}

int DialogY2Pixel(int d)
{
#ifdef COMMON_DLG_UNIT
  int pixel_w = MulDiv(d, gDlgFontBaseHeight, 8);
#else
  DWORD u = GetDialogBaseUnits();
  int basey = (int)HIWORD(u);
  int pixel_w = MulDiv(d, basey, 8);
#endif
  return pixel_w;
}

int DialogX2Pixel(int d)
{
#ifdef COMMON_DLG_UNIT
  int pixel_w = MulDiv(d, gDlgFontBaseWidth, 4);
#else
  DWORD u = GetDialogBaseUnits();
  int basex = (int)LOWORD(u);
  int pixel_w = MulDiv(d, basex, 4);
#endif
  return pixel_w;
}

void DialogUnit2Pixel(HWND hWnd, POINT &d)
{
  DialogUnit2Pixel(d);
  int dpi = GetScreenDpi(hWnd);
  d.x = d.x * dpi / 96;
  d.y = d.y * dpi / 96;
}

int DialogY2Pixel(HWND hWnd, int d)
{
  int r = DialogY2Pixel(d);
  int dpi = GetScreenDpi(hWnd);
  return (r * dpi / 96);
}

int DialogX2Pixel(HWND hWnd, int d)
{
  int r = DialogX2Pixel(d);
  int dpi = GetScreenDpi(hWnd);
  return (r * dpi / 96);
}

int GetHiDPIImageButtonSize(HWND hWnd)
{
  int dpi = GetScreenDpi(hWnd);
  if (dpi > 168) // 192
    return 2;
  else if (dpi >= 120) // 144
    return 1;
  else
    return 0;
}

void ShowHideFromTaskBar(HWND hWnd, BOOL bShow)
{
  ITaskbarList *pTaskList = NULL;
  HRESULT h = K_CoInitializeEx(0, COINIT_MULTITHREADED);
  // HRESULT initRet = CoInitialize(NULL);

  HRESULT createRet = CoCreateInstance( CLSID_TaskbarList,
                                        NULL,
                                        CLSCTX_INPROC_SERVER,
                                        IID_ITaskbarList,
                                        (LPVOID*)&pTaskList );

  if(createRet == S_OK)
  {
    HRESULT r;
    if (bShow)
      r = pTaskList->AddTab(hWnd);
    else
      r = pTaskList->DeleteTab(hWnd);

#ifdef _DEBUG
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("Taskbar Icon SHow/Hide : %d\n"), bShow);
    OutputDebugString(msg);
#endif

    pTaskList->Release();
  }
  K_CoUninitialize();
}

int getStringWidth(CDC &dc, LPCTSTR str)
{
  RECT rect = {0, 0, 100, 100 };
  dc.DrawText(str, &rect, DT_SINGLELINE|DT_CALCRECT);
  return rect.right;
}

// return number of char fit to width
int fitStringWidthRight(CDC &dc, LPCTSTR str, int width)
{
  int p = 0;
  int slen = lstrlen(str);
  LPCTSTR s = str + p;

  while(p < slen)
  {
    int w = getStringWidth(dc, s);
    if (w <= width)
      return (slen - p);
    ++p;
    ++s;
    if ((p < slen) && ((*s == 0x0d) || (*s == 0x0a)))
      break;
  }
  return (slen - p);
}

int fitStringWidthLeft(CDC &dc, LPCTSTR str, int width)
{
  int slen = lstrlen(str);
  for(int i = 2; i < slen; i++)
  {
    RECT rect = {0, 0, 100, 100 };
    dc.DrawText(str, i, &rect, DT_SINGLELINE|DT_CALCRECT);

    if (rect.right > width)
      return (i - 1);
    if ((i < slen) &&
      ((str[i] == 0x0d) || (str[i] == 0x0a)))
      return i;
  }
  return slen;
}

int FormatLineString(CDC &dc, CString &str, int width, int max_line, int flag)
{
  CString org(str);
  CString m;
  CStringList lineList;

  int dot_width = 0;
  if (flag & FORMAT_LINE_ELLIPSE)
    dot_width = getStringWidth(dc, _T("..."));

  while((org.GetLength() > 0) && (lineList.GetCount() < max_line))
  {
    int clip_width = width;

    if (flag & FORMAT_LINE_START_ELLIPSE)
    {
      if(lineList.GetCount() == (max_line-1))
        clip_width -= dot_width;

      int r_str_len = fitStringWidthRight(dc, org.GetBuffer(), clip_width);
      
      if (r_str_len < org.GetLength())
      {
        m = org.Right(r_str_len);
        org = org.Left(org.GetLength() - r_str_len);
      }
      else
      {
        m = org;
        org.Empty();
      }

      if (m.GetLength() > 0)
        m.Trim();

      if (lineList.GetCount() == (max_line-1))
        m = _T("...") + m;
      lineList.AddHead(m);
    }
    else
    {
      if((flag & FORMAT_LINE_END_ELLIPSE) && (lineList.GetCount() == (max_line-1)))
        clip_width -= dot_width;

      int str_len = fitStringWidthLeft(dc, org.GetBuffer(), clip_width);
      if (str_len < org.GetLength())
      {
        m = org.Left(str_len);
        org = org.Right(org.GetLength() - str_len);
      }
      else
      {
        m = org;
        org.Empty();
      }
      if((flag & FORMAT_LINE_END_ELLIPSE) && (lineList.GetCount() == (max_line-1)))
        m += _T("...");
      lineList.AddTail(m);
    }

    if (org.GetLength() > 0)
      org.Trim();
  }

  str.Empty();
  POSITION p = lineList.GetHeadPosition();
  for (int i = 0; i < lineList.GetCount(); i++)
  {
    str += lineList.GetNext(p);
    if (i < (lineList.GetCount()-1))
      str += _T("\n");
  }
  return lineList.GetCount();
}

void DlgMoveToOffset(CWnd* dlg, int itemId, int xoffset, int yoffset, BOOL bRepaint)
{
  CWnd* child = dlg->GetDlgItem(itemId);
  if (child != NULL)
    DlgMoveToOffset(dlg, child, xoffset, yoffset, bRepaint);
}

void DlgMoveToOffset(CWnd* dlg, CWnd* child, int xoffset, int yoffset, BOOL bRepaint)
{
  RECT rect;

  child->GetWindowRect(&rect);
  dlg->ScreenToClient(&rect);

  if (xoffset != 0)
  {
    rect.left += xoffset;
    rect.right += xoffset;
  }
  if (yoffset != 0)
  {
    rect.top += yoffset;
    rect.bottom += yoffset;
  }
  child->MoveWindow(&rect, bRepaint);
}

void DlgMoveToOffset(CWnd* dlg, int xp, int yp, int xe, int ye, RECT& client, CWnd* child, BOOL bRepaint)
{
    RECT rect;

    child->GetWindowRect(&rect);
    dlg->ScreenToClient(&rect);

    if (xp >= 0)
    {
      int w = rect.right - rect.left;
      rect.left = client.right - xp;
      rect.right = rect.left + w;
    }
    if (yp >= 0)
    {
      int h = rect.bottom - rect.top;
      rect.top = client.bottom - yp;
      rect.bottom = rect.top + h;
    }
    if (xe >= 0)
    {
      rect.right = client.right - xe;
    }
    if (ye >= 0)
    {
      rect.bottom = client.bottom - ye;
    }

    child->MoveWindow(&rect, bRepaint);
}

void DlgStoreButtonPos(CWnd* dlg, CWnd* child, RECT& client, POINT& pt, int dir)
{
  RECT rect;
  child->GetWindowRect(&rect);
  dlg->ScreenToClient(&rect);
  if (dir & DLGPOS_R_R)
    pt.x = client.right - rect.right;
  else if (dir & DLGPOS_R_L)
    pt.x = client.right - rect.left;

  if (dir & DLGPOS_B_B)
    pt.y = client.bottom - rect.bottom;
  else if (dir & DLGPOS_B_T)
    pt.y = client.bottom - rect.top;
}

typedef struct tagMonitorCoordCheck
{
  int result;
  int minimum_overlap;
  RECT window_rect;
} MonitorCoordCheck, *LP_MonitorCoordCheck;

BOOL CALLBACK pEnumWindowRectCheckProc(HMONITOR m, HDC hdc, LPRECT lprc, LPARAM dwData)
{
	LP_MonitorCoordCheck pMonitor;
	pMonitor = (LP_MonitorCoordCheck) dwData;

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if(GetMonitorInfo(m, &mi))
  {
    // check overlap for minimum
    if ((mi.rcMonitor.left < pMonitor->window_rect.right - pMonitor->minimum_overlap) &&
      (pMonitor->window_rect.left + pMonitor->minimum_overlap < mi.rcMonitor.right) &&
      (mi.rcMonitor.top < pMonitor->window_rect.bottom - pMonitor->minimum_overlap) &&
      (pMonitor->window_rect.top + pMonitor->minimum_overlap < mi.rcMonitor.bottom) )
    {
      pMonitor->result = 1;
    }
  }
	return true;
}

BOOL IsValidWindowPosition(RECT &rect)
{
#if 0
  int xv = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int yv = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

  if ((cx < 1) || (cy < 1))
    return FALSE;

  return ((rect.left >= xv) && (rect.top >= yv) &&
    (rect.right <= xv + cx) && (rect.bottom <= yv + cy));
#else
	HDC hdc;
  MonitorCoordCheck mc;

	hdc = GetDC(GetDesktopWindow());
	memset(&mc, 0, sizeof(MonitorCoordCheck));
  mc.window_rect = rect;
  mc.minimum_overlap = 60;
	EnumDisplayMonitors(hdc, NULL, pEnumWindowRectCheckProc, (LPARAM) &mc);
	ReleaseDC(GetDesktopWindow(), hdc);
  return mc.result;
#endif
}

int GetScreenDpi(HWND w)
{
  int dpi = 0;

  if (w == NULL)
    w = GetDesktopWindow();
  /*
  if (IsWindow10_1607_More())
  {
    dpi = GetDpiForWindow(w);
  }
  else
  */
  {
    HDC hDC = GetDC(w);
    dpi = GetDeviceCaps(hDC, LOGPIXELSX);
    ReleaseDC(w, hDC);
  }
  return dpi;
}

static POINT arrow_shape[] = { -50, -50, 50, 0, -50, 50 };

void readyArrowShape(POINT* points, int arrow_dir, int width, int cx, int cy)
{
  int dir = (arrow_dir == 180) ? -1 : 1;
  for (int i = 0; i < 3; i++)
  {
    points[i].x = arrow_shape[i].x * dir * width / 100 + cx;
    points[i].y = arrow_shape[i].y * dir * width / 100 + cy;
  }
}

void DrawLineArrow(CDC& dc, int cx, int cy, int width, int arrow_dir, int style, COLORREF color)
{
  CBrush brush;

  brush.CreateSolidBrush(color);
  CBrush* old_brush = dc.SelectObject(&brush);
  CPen* old_pen = (CPen*)dc.SelectStockObject(NULL_PEN);

  if (style == 1)
  {
    POINT shape[3];
    readyArrowShape(shape, arrow_dir, width, cx, cy);
    dc.Polygon(shape, 3);
  }
  else if (style == 2)
  {
    int half = width / 2;
    dc.Rectangle(cx - half, cy - half, cx + half, cy + half);
  }
  else if (style == 3)
  {
    int half = width / 2;
    dc.Ellipse(cx - half, cy - half, cx + half, cy + half);
  }

  dc.SelectObject(old_brush);
  dc.SelectObject(old_pen);
}

void SetDlgItemFont(CWnd* dlg, int* dlg_ctrl_ids, int count, CFont* pFont)
{
  for (int i = 0; i < count; i++)
  {
    CWnd* ctrl = dlg->GetDlgItem(dlg_ctrl_ids[i]);
    if ((ctrl != NULL) && ctrl->GetSafeHwnd())
      ctrl->SetFont(pFont);
  }
}
