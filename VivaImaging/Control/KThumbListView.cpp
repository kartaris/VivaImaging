// KThumbListView.cpp: 구현 파일
//

#include "stdafx.h"
#include "..\MainFrm.h"
#include "..\resource.h"
#include "..\VivaImaging.h"
#include "..\VivaImagingView.h"
#include "..\Platform\Graphics.h"
#include "..\Platform\Utils.h"

#include "KThumbListView.h"

// KThumbListView

KBitmap* KThumbListView::m_pScrollUpIcon = NULL;
KBitmap* KThumbListView::m_pScrollDownIcon = NULL;
KBitmap* KThumbListView::m_pCheckIcon = NULL;
KBitmap* KThumbListView::m_pFoldingNormIcon = NULL;
KBitmap* KThumbListView::m_pFoldingFocusIcon = NULL;
KBitmap* KThumbListView::m_pFolderIcon = NULL;

#define SCROLLBAR_NORMAL_COLOR 0xCCCCCC //DBCDBF
#define SCROLLBAR_ACTIVE_COLOR 0xCCCCCC //CBBDAF

#define THUMB_ACTIVE_BG_COLOR 0xC8DFF0
#define THUMB_ACTIVE_BORDER_COLOR 0xE68B14

#define THUMB_SELECTED_BG_COLOR 0xF0DFC8
#define THUMB_SELECTED_BORDER_COLOR 0xE68B14

#define THUMB_NORMAL_BORDER_COLOR 0xD8D2CB

#define THUMB_INSERTION_BAR_COLOR 0xCCCCCC

#define MOUSE_AUTO_SCROLL_INTERVAL 300

#define VARIABLE_THUMB_HEIGHT

IMPLEMENT_DYNAMIC(KThumbListView, CWnd)

KThumbListView::KThumbListView()
{
  m_ScrollOffset = 0;
  m_ScrollMax = 0;
  m_bScrollbarActive = 0;
  m_bScrollbarDragOffset = 0;
  m_MouseScrollTimer = NULL;
  m_RefreshTimer = NULL;

  m_MouseState = MouseState::MOUSE_OFF;
  m_LastButtonDownIndex = -1;
  m_LastClickSelect = -1;
  m_foldingOverIndex = -1;
  m_MovePageTarget.pageIndex = -1;

  m_pScrollUpIcon = NULL;
  m_pScrollDownIcon = NULL;

  m_pDocument = NULL;

  RegisterWndClass();
}

KThumbListView::~KThumbListView()
{
  if (m_pScrollUpIcon != NULL)
  {
    delete m_pScrollUpIcon;
    m_pScrollUpIcon = NULL;
  }
  if (m_pScrollDownIcon != NULL)
  {
    delete m_pScrollDownIcon;
    m_pScrollDownIcon = NULL;
  }
}


BEGIN_MESSAGE_MAP(KThumbListView, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSELEAVE()
  ON_WM_MOUSEWHEEL()
  ON_WM_TIMER()
  ON_WM_DROPFILES()
  ON_COMMAND(ID_EDIT_SELECT_ALL, &KThumbListView::OnEditSelectAll)
  ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &KThumbListView::OnUpdateEditSelectAll)
  ON_COMMAND(ID_EDIT_CUT, &KThumbListView::OnEditCut)
  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &KThumbListView::OnUpdateEditCut)
  ON_COMMAND(ID_EDIT_COPY, &KThumbListView::OnEditCopy)
  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &KThumbListView::OnUpdateEditCopy)
  ON_COMMAND(ID_EDIT_PASTE, &KThumbListView::OnEditPaste)
  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &KThumbListView::OnUpdateEditPaste)
  ON_COMMAND(ID_EDIT_DELETE, &KThumbListView::OnEditDelete)
  ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &KThumbListView::OnUpdateEditDelete)
END_MESSAGE_MAP()

TCHAR K_THUMB_LIST_VIEW_CLASS[] = _T("KIMAGINGTHUMBLISTWND");
TCHAR K_THUMB_LIST_CONTROL[] = _T("KImagingThumbListWnd");

BOOL KThumbListView::RegisterWndClass()
{
  WNDCLASS windowclass;
  HINSTANCE hInst = AfxGetInstanceHandle();
  if (!(::GetClassInfo(hInst, K_THUMB_LIST_VIEW_CLASS, &windowclass)))
  {
    windowclass.style = CS_HREDRAW;
    windowclass.lpfnWndProc = ::DefWindowProc;
    windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
    windowclass.hInstance = hInst;
    windowclass.hIcon = NULL;
    windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
    windowclass.lpszMenuName = NULL;
    windowclass.lpszClassName = K_THUMB_LIST_VIEW_CLASS;
    if (!AfxRegisterClass(&windowclass))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }
  return TRUE;
}

#define MARGIN_X_DLG 4
#define MARGIN_Y_DLG 4
#define MARGIN_INNER_X_DLG 2
#define MARGIN_INNER_Y_DLG 2
#define MARGIN_DEPTH_DLG 6

#define MOUSE_OVER_SCROLLBAR 1
//#define USE_MIN_THUMB_SIZE


int KThumbListView::m_MarginX = 0;
int KThumbListView::m_MarginY = 0;
int KThumbListView::m_MarginInnerX = 0;
int KThumbListView::m_MarginInnerY = 0;
int KThumbListView::m_MarginDepth = 0;
int KThumbListView::m_folder_height = 0;

BOOL KThumbListView::Create(DWORD windowStyle, RECT& rect, CWnd* parent, int controlId)
{
  return CWnd::Create(K_THUMB_LIST_VIEW_CLASS, K_THUMB_LIST_CONTROL, windowStyle, rect, parent, controlId);
}

// KThumbListView 메시지 처리기
int KThumbListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  DragAcceptFiles(TRUE);

  m_MarginX = DialogX2Pixel(GetSafeHwnd(), MARGIN_X_DLG);
  m_MarginY = DialogX2Pixel(GetSafeHwnd(), MARGIN_Y_DLG);
  m_MarginInnerX = DialogX2Pixel(GetSafeHwnd(), MARGIN_INNER_X_DLG);
  m_MarginInnerY = DialogY2Pixel(GetSafeHwnd(), MARGIN_INNER_Y_DLG);
  m_MarginDepth = DialogX2Pixel(GetSafeHwnd(), MARGIN_DEPTH_DLG);

  m_folder_height = DialogY2Pixel(24);

  return CWnd::OnCreate(lpCreateStruct);
}

void KThumbListView::ClearStatic()
{
  if (m_pScrollUpIcon != NULL)
  {
    delete m_pScrollUpIcon;
    m_pScrollUpIcon = NULL;
  }
  if (m_pScrollDownIcon != NULL)
  {
    delete m_pScrollDownIcon;
    m_pScrollDownIcon = NULL;
  }
  if (m_pCheckIcon != NULL)
  {
    delete m_pCheckIcon;
    m_pCheckIcon = NULL;
  }
  if (m_pFoldingNormIcon != NULL)
  {
    delete m_pFoldingNormIcon;
    m_pFoldingNormIcon = NULL;
  }
  if (m_pFoldingFocusIcon != NULL)
  {
    delete m_pFoldingFocusIcon;
    m_pFoldingFocusIcon = NULL;
  }
  if (m_pFolderIcon != NULL)
  {
    delete m_pFolderIcon;
    m_pFolderIcon = NULL;
  }
}

void KThumbListView::drawFolderIconImage(CDC& dc, RECT& d, KImageBase* p, BOOL hasFocus, BOOL isActive)
{
  if (m_pFoldingNormIcon == NULL)
  {
    m_pFoldingNormIcon = new KBitmap();
    m_pFoldingFocusIcon = new KBitmap();
    m_pFolderIcon = new KBitmap();

    int foldingNormRes[] = { IDB_FOLDING_NORM_16, IDB_FOLDING_NORM_24, IDB_FOLDING_NORM_32 };
    int foldingFocusRes[] = { IDB_FOLDING_FOCUS_16, IDB_FOLDING_FOCUS_24, IDB_FOLDING_FOCUS_32 };
    int folderIconRes[] = { IDB_FOLDER_ICON_32, IDB_FOLDER_ICON_48, IDB_FOLDER_ICON_64 };
    int i = 0;
    int screen_dpi = GetScreenDpi();
    if (screen_dpi > 200)
      i = 2;
    else if (screen_dpi > 100)
      i = 1;

    m_pFoldingNormIcon->LoadFromPNGResource(foldingNormRes[i]);
    m_pFoldingFocusIcon->LoadFromPNGResource(foldingFocusRes[i]);
    m_pFolderIcon->LoadFromPNGResource(folderIconRes[i]);
  }

  KBitmap* pIcon = hasFocus ? m_pFoldingFocusIcon : m_pFoldingNormIcon;
  if (pIcon != NULL)
  {
    RECT srect;
    srect.left = p->IsFolded() ? 0 : pIcon->height;
    srect.top = 0;
    srect.right = srect.left + pIcon->height;
    srect.bottom = pIcon->height;

    d.right = d.left + pIcon->height;
    d.top += (d.bottom - d.top - pIcon->height) / 2;
    d.bottom = d.top + pIcon->height;

    pIcon->AlphaBlend(dc, d, srect, 255);
    d.left += pIcon->height + m_MarginInnerX;
  }

  pIcon = m_pFolderIcon;
  if (pIcon != NULL)
  {
    RECT srect;
    srect.left = isActive ? pIcon->height : 0;
    srect.top = 0;
    srect.right = srect.left + pIcon->height;
    srect.bottom = pIcon->height;
    d.right = d.left + pIcon->height;
    d.top += (d.bottom - d.top - pIcon->height) / 2;
    d.bottom = d.top + pIcon->height;

    pIcon->AlphaBlend(dc, d, srect, 255);
    d.left += pIcon->height + m_MarginInnerX;
  }
}

void KThumbListView::OnPaint()
{
  CPaintDC dc(this);
  RECT client;
  GetClientRect(&client);
  RECT paintRect = dc.m_ps.rcPaint;

  if (m_pDocument == NULL)
  {
    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    m_pDocument = (CVivaImagingDoc *)pFrame->GetActiveDocument();
    if (m_pDocument == NULL)
      return;
  }

  if (m_ScrollOffset > 0)
  {
    paintRect.top += m_ScrollOffset;
    paintRect.bottom += m_ScrollOffset;
    dc.SetViewportOrg(0, -m_ScrollOffset);
  }

  RECT rect;

  //COLORREF bg_color = GetSysColor(COLOR_WINDOWFRAME);

  rect.left = client.left + m_MarginX;
  rect.right = client.right - m_MarginX;

#ifdef USE_MIN_THUMB_SIZE
  int min_width = m_MarginX * 7;
  if ((rect.left + min_width) > rect.right)
    rect.right = rect.left + min_width;
#endif

  CPen pen;
  CBrush brush;
  CPen* old_pen = NULL;
  CBrush* old_brush = NULL;

  int pages = m_pDocument->GetNumberOfPages(EXCLUDE_FOLDED);
  KImageBase* active = m_pDocument->GetActivePage();

  // ready check icon image
#if 0
  if (m_pCheckIcon == NULL)
  {
    m_pCheckIcon = new KBitmap();
    if (GetScreenDpi() < 100)
      m_pCheckIcon->LoadImageFromRGBABuffer(16, -16, 32, NULL, 0, TRUE, checkButton16Image);
    else
      m_pCheckIcon->LoadImageFromRGBABuffer(24, -24, 32, NULL, 0, TRUE, checkButton24Image);
  }
#endif

  rect.top = m_MarginY;
  for (int i = 0; i < pages; i++)
  {
    KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);
    int height = calcThumbHeight(p);
    rect.bottom = rect.top + height;

    // drag target page insert position
    if ((m_MouseState == MouseState::MOUSE_PAGE_DRAG) &&
      (((m_MovePageTarget.pageIndex == i) && (m_MovePageTarget.inside == InsertTargetType::TARGET_BEFORE)) ||
        (((m_MovePageTarget.pageIndex + 1) == i) && (m_MovePageTarget.inside == InsertTargetType::TARGET_NEXT))) )
    {
      if ((rect.top > paintRect.top) &&
        ((rect.top - m_MarginY) < paintRect.bottom))
      {
        RECT r;

        r.left = client.left + m_MarginInnerX;
        int depth = p->GetDepth();
        if ((m_MovePageTarget.inside == InsertTargetType::TARGET_NEXT) && (m_MovePageTarget.targetPage != NULL))
          depth = m_MovePageTarget.targetPage->GetDepth();
        r.left += (depth - 1) * m_MarginDepth;

        r.right = client.right - m_MarginInnerX;
        r.top = rect.top - m_MarginY / 2 - 2;
        r.bottom = r.top + 3;
        dc.FillSolidRect(&r, THUMB_INSERTION_BAR_COLOR);
      }
    }

    if ((rect.bottom < paintRect.top) ||
      (rect.top > paintRect.bottom))
    {
      rect.top += (height + m_MarginY);
      continue;
    }
    /*
    TCHAR str[64];
    StringCchPrintf(str, 64, _T("draw %d"), i);
    OutputDebugString(str);
    */

    int depth = (p != NULL) ? p->GetDepth() : 1;
    rect.left = client.left + m_MarginX + m_MarginDepth * (depth - 1);

    COLORREF pen_color = 0;
    int width = 1;

    // if it is drop target, border is black
    if ((m_MouseState == MouseState::MOUSE_PAGE_DRAG) &&
      (((m_MovePageTarget.pageIndex == i) && (m_MovePageTarget.inside == InsertTargetType::TARGET_INSIDE))))
    {
      pen_color = 0xFF000000;
    }
    else if (active == p)
    {
      pen_color = THUMB_ACTIVE_BORDER_COLOR;
      width = 1;
    }
    else if (p->IsSelected())
      pen_color = THUMB_NORMAL_BORDER_COLOR;// THUMB_SELECTED_BORDER_COLOR;
    else if (p->GetType() == IMAGE_PAGE)
      pen_color = THUMB_NORMAL_BORDER_COLOR; // GetSysColor(COLOR_WINDOWFRAME);
    else
      pen_color = GetSysColor(COLOR_WINDOW);

    pen.CreatePen(PS_SOLID, 1, pen_color);
    old_pen = (CPen*)dc.SelectObject(&pen);

    if (p->IsSelected())
    {
      brush.CreateSolidBrush(THUMB_SELECTED_BG_COLOR);
      old_brush = (CBrush*)dc.SelectObject(&brush);
    }
    else
    {
      old_brush = (CBrush*)dc.SelectStockObject(HOLLOW_BRUSH);
    }

    dc.Rectangle(&rect);

    dc.SelectObject(old_pen);
    dc.SelectObject(old_brush);
    pen.DeleteObject();
    if (brush.GetSafeHandle())
      brush.DeleteObject();

    if (p != NULL)
    {
      RECT d;
      d.left = rect.left + m_MarginInnerX;
      d.top = rect.top + m_MarginInnerY;
      d.right = rect.right - m_MarginInnerX;
      d.bottom = rect.bottom - m_MarginInnerY;

      // folder
      if (p->GetType() & FOLDER_PAGE)
      {
        drawFolderIconImage(dc, d, p, IsFolderFoldFocused(i), (active == p));

        d.top = rect.top + m_MarginInnerY;
        d.right = rect.right - m_MarginInnerX;
        d.bottom = rect.bottom - m_MarginInnerY;
      }

      KBitmap* b = p->GetThumbImage();
      if (b != NULL)
      {
        RECT dr = d;
        b->Blit(dc, dr, BLIT_FITTO);
      }
      else
      {
        LPCTSTR name = p->GetName();
        if (name != NULL)
        {
          CFont* old_font = dc.SelectObject(gpUiFont);
          dc.SetBkMode(TRANSPARENT);
          dc.DrawText(name, &d, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
          dc.SelectObject(old_font);
        }
      }

      if (p->IsSelected())
      {
#if 0
        if (m_pCheckIcon->handle() != NULL)
        {
          d.left = rect.left + m_MarginInnerX;
          d.top = rect.top + m_MarginInnerY;

          m_pCheckIcon->AlphaBlend(dc, d.left + 1, d.top + 1, 255);
        }
#endif
      }
    }
    rect.top += (height + m_MarginY);
  }

  // drag target page insert position on last
  if ((m_MouseState == MouseState::MOUSE_PAGE_DRAG) &&
    (((m_MovePageTarget.pageIndex == pages) && (m_MovePageTarget.inside == InsertTargetType::TARGET_BEFORE)) ||
      ((m_MovePageTarget.pageIndex == (pages - 1)) && (m_MovePageTarget.inside == InsertTargetType::TARGET_NEXT))) )
  {
    rect.bottom = rect.top + m_thumb_width;

    if ((rect.top > paintRect.top) &&
      ((rect.top - m_MarginY) < paintRect.bottom))
    {
      RECT r;
      r.left = client.left + m_MarginInnerX;

      if (m_MovePageTarget.inside == InsertTargetType::TARGET_NEXT)
      {
        int depth = m_MovePageTarget.targetPage->GetDepth();
        r.left += (depth - 1) * m_MarginDepth;
      }

      r.right = client.right - m_MarginInnerX;
      r.top = rect.top - m_MarginY / 2 - 2;
      r.bottom = r.top + 3;
      // dc.Rectangle(&r);
      dc.FillSolidRect(&r, THUMB_INSERTION_BAR_COLOR);
    }
  }

  if (m_ScrollOffset > 0)
    dc.SetViewportOrg(0, 0);

// #define _CONVERT_BMP

#ifdef _CONVERT_BMP
  KBitmap* icon = new KBitmap();

  LPCTSTR png_files[] = {
    _T("C:\\MyWork\\VivaImaging\\image-source\\main-x.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-xlarge.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-xsmall.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-xlarge.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-xsmall.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-large.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-small.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-large.png"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-small.png")
  };
  LPCTSTR bmp_files[] = {
    _T("C:\\MyWork\\VivaImaging\\image-source\\main-x.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-xlarge.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-xsmall.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-xlarge.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-xsmall.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-large.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\file-small.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-large.bmp"),
    _T("C:\\MyWork\\VivaImaging\\image-source\\image-small.bmp")
  };

  int count = sizeof(png_files) / sizeof(LPTSTR);
  for (int i = 0; i < count; i++)
  {
    if (icon->LoadImageFromFile(png_files[i]) == NULL)
    {
      TCHAR msg[300] = _T("File not found : ");
      StringCchCat(msg, 300, png_files[i]);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }
    else
    {
      icon->Flip(FLIP_VERTICAL);
      icon->StoreBMPFile(bmp_files[i], 0);
      icon->Clear();
    }
  }

  /*
  LPCTSTR dump_png_files[] = {
  _T("C:\\MyWork\\VivaImaging\\image-source\\folder-icon@32.png"),
  };
  int count = sizeof(dump_png_files) / sizeof(LPTSTR);
  for (int i = 0; i < count; i++)
  {
    icon->LoadImageFromFile(dump_png_files[i]);
    icon->Dump();
  }
  */
  delete icon;
#endif

#ifdef USING_SCROLL_UPDOWN_OVER_BUTTON
  if (m_pScrollUpIcon == NULL)
  {
    int sizeof_buffer = sizeof(upButtonImage);
    m_pScrollUpIcon = new KBitmap();
    m_pScrollUpIcon->LoadImageFromRGBABuffer(32, 33, 32, NULL, 0, TRUE, upButtonImage);
    m_pScrollUpIcon->Flip(FLIP_VERTICAL);

    m_pScrollDownIcon = new KBitmap();
    m_pScrollDownIcon->LoadImageFromRGBABuffer(32, 33, 32, NULL, 0, TRUE, upButtonImage);
  }

  if ((m_pScrollDownIcon->handle() != NULL) && (m_MouseState != MouseState::MOUSE_OFF))
  {
    int xp = (rect.left + rect.right) / 2 - 15;
    int yp = m_MarginY + 1;

    int alpha = (m_MouseState == MouseState::MOUSE_UP_SCROLL) ? 255 : 125;
    m_pScrollUpIcon->AlphaBlend(dc, xp, yp, alpha);
    yp += m_pScrollUpIcon->height;

    int ep = client.bottom - (m_MarginY + 1) - m_pScrollDownIcon->height;
    if (yp < ep)
    {
      alpha = (m_MouseState == MouseState::MOUSE_DOWN_SCROLL) ? 255 : 125;
      m_pScrollDownIcon->AlphaBlend(dc, xp, ep, alpha);
    }
  }
#else
  RECT r;
  if (GetActiveScrollbarRect(client, r))
  {
    CBrush bgbrush;

    r.left = r.right - m_MarginX +1;
    if (m_bScrollbarActive)
    {
      bgbrush.CreateSolidBrush(SCROLLBAR_ACTIVE_COLOR);
    }
    else
    {
      int width = m_MarginX / 3;
      r.left += width;
      r.right -= width;
      bgbrush.CreateSolidBrush(SCROLLBAR_NORMAL_COLOR);
    }
    dc.FillRect(&r, &bgbrush);
  }
#endif

  checkForNeedUpdateThumb(FALSE);
}

void KThumbListView::OnSize(UINT nType, int cx, int cy)
{
  RecalcScrollRange(cx, cy);
}

void KThumbListView::OnMouseMove(UINT nFlags, CPoint point)
{
  CRect client;
  GetClientRect(&client);
  /*
if (client.PtInRect(point))
  {
    if (m_MouseState == MouseState::MOUSE_OFF)
    {
      m_MouseState = MouseState::MOUSE_ENTER;

      TRACKMOUSEEVENT tm;
      tm.cbSize = sizeof(TRACKMOUSEEVENT);
      tm.dwFlags = TME_LEAVE;
      tm.dwHoverTime = 1000;
      tm.hwndTrack = GetSafeHwnd();
      TrackMouseEvent(&tm);

      Invalidate(1);
    }
  }
  */

  if (m_MouseState == MouseState::MOUSE_DRAG_SCROLL)
  {
    int dy = m_MousePoint.y - point.y;
    if (dy != 0)
    {
      ScrollPage(dy);
      m_MousePoint.y = point.y;
    }
  }
  else if (m_MouseState == MouseState::MOUSE_DOWN_FOLDING)
  {
    // check folding icon over
    int p = pageFromPoint(point);
    if (p >= 0)
    {
      int foldingOver = IsOverFoldingIcon(p, point);

      // 폴딩 버튼을 누른 상태에서는 다른 폴딩  버튼으로 올라가는거 무시한다.
      if (m_foldingDownIndex != foldingOver)
        foldingOver = -1;

      if (m_foldingOverIndex != foldingOver)
      {
        if (m_foldingOverIndex >= 0)
        {
          RECT rect;
          GetFoldingIconRect(m_foldingOverIndex, rect);
          InvalidateRect(&rect, 1);
        }
        m_foldingOverIndex = foldingOver;
        if (m_foldingOverIndex >= 0)
        {
          RECT rect;
          GetFoldingIconRect(m_foldingOverIndex, rect);
          InvalidateRect(&rect, 1);
        }
      }
    }
  }
  else if (m_MouseState == MouseState::MOUSE_DOWN_SELECT)
  {
    // check folding icon over
    int p = pageFromPoint(point);
    if ((p >= 0) && (m_LastButtonDownIndex != p))
    {
      // start page dragging
      m_MovePageTarget = pageInsertFromPoint(point);
      m_MouseState = MouseState::MOUSE_PAGE_DRAG;
      invalidatePage(m_MovePageTarget);
    }
  }
  else if (m_MouseState == MouseState::MOUSE_PAGE_DRAG)
  {
    checkDraggingScroll(point.y, client.bottom);

    PageDragTarget p = pageInsertFromPoint(point);

#ifdef _DEBUG
    if (p.pageIndex < -1)
      OutputDebugString(_T("Nowhere\n"));
#endif

    if ((p.pageIndex != m_MovePageTarget.pageIndex) ||
      (p.targetPage != m_MovePageTarget.targetPage) ||
      (p.inside != m_MovePageTarget.inside))
    {
      invalidatePage(m_MovePageTarget);
      m_MovePageTarget = p;
      invalidatePage(m_MovePageTarget);

#ifdef _DEBUG
      TCHAR msg[120];
      LPCTSTR targetName[] = {
        _T("Before"),
        _T("Inside"),
        _T("Next")
      };

      int depth = 0;
      if (m_MovePageTarget.targetPage != NULL)
        depth = m_MovePageTarget.targetPage->GetDepth();
      LPCTSTR insideStr = targetName[(int)p.inside];
      StringCchPrintf(msg, 120, _T("Drop target page : %d, %s, target_depth=%d\n"), p.pageIndex, insideStr, depth);
      OutputDebugString(msg);
#endif
    }
  }
  else if (m_MouseState == MouseState::MOUSE_SCROLLBAR_DRAG)
  {
    CRect client;
    CRect rect;

    GetClientRect(client);
    GetActiveScrollbarRect(client, rect);

    int view_height = client.bottom - client.top;
    int scrollbar_height = rect.bottom - rect.top;

    int top = point.y - m_bScrollbarDragOffset;
    float r = (float)top / (view_height - scrollbar_height);
    int scroll_offset = (int)(m_ScrollMax * r + 0.5);
    if (scroll_offset != m_ScrollOffset)
    {
      ScrollPage(scroll_offset - m_ScrollOffset);
    }
  }
  else
  {
    // check if over scrollbar
    CRect scroll;
    int active = m_bScrollbarActive;

    if (GetActiveScrollbarRect(client, scroll))
    {
      if (scroll.PtInRect(point))
        active = MOUSE_OVER_SCROLLBAR;
      else
        active = 0;

      if (m_bScrollbarActive != active)
      {
        m_bScrollbarActive = active;
        Invalidate();
      }
    }
  }

}

void KThumbListView::OnMouseLeave()
{
  /*
  if ((m_MouseState != MOUSE_OFF) || (m_bScrollbarActive > 0))
  {
    if (m_foldingOverIndex >= 0)
      m_foldingOverIndex = -1;

    m_MouseState = MOUSE_OFF;
    m_bScrollbarActive = 0;
    Invalidate(1);
  }
  */

  OutputDebugString(_T("OnMouseLeave\n"));
  if (m_MouseState == MouseState::MOUSE_PAGE_DRAG)
  {
    PageDragTarget p = { -2, NULL, InsertTargetType::TARGET_NEXT };

    if ((p.pageIndex != m_MovePageTarget.pageIndex) ||
      (p.targetPage != m_MovePageTarget.targetPage) ||
      (p.inside != m_MovePageTarget.inside))
    {
      invalidatePage(m_MovePageTarget);
      m_MovePageTarget = p;
    }
  }
}

void KThumbListView::OnLButtonDown(UINT nFlags, CPoint point)
{
  CRect rect;

#ifdef USING_SCROLL_UPDOWN_OVER_BUTTON
  GetScrollIconRect(rect, TRUE);

  if (rect.PtInRect(point))// && (m_ScrollOffset > 0))
  {
    m_MouseState = MOUSE_UP_SCROLL;
    if (m_MouseScrollTimer == NULL)
    {
      ScrollPage(-m_thumb_width);
      m_MouseScrollTimer = SetTimer(TIMER_MOUSE_SCROLL, MOUSE_AUTO_SCROLL_INTERVAL, NULL);
    }
    return;
  }

  GetScrollIconRect(rect, FALSE);
  if (rect.PtInRect(point))// && (m_ScrollOffset < m_ScrollMax))
  {
    m_MouseState = MOUSE_DOWN_SCROLL;
    if (m_MouseScrollTimer == NULL)
    {
      ScrollPage(m_thumb_width);
      m_MouseScrollTimer = SetTimer(TIMER_MOUSE_SCROLL, MOUSE_AUTO_SCROLL_INTERVAL, NULL);
    }
    return;
  }
#endif // USING_SCROLL_UPDOWN_OVER_BUTTON

  m_MousePoint = point;
  SetCapture();

  CRect scroll;
  GetClientRect(rect);
  if (GetActiveScrollbarRect(rect, scroll))
  {
    if ((scroll.left < point.x) && (point.x < scroll.right))
    {
      m_bScrollbarDragOffset = point.y - scroll.top;
      if (m_bScrollbarActive && scroll.PtInRect(point))
      {
        m_MouseState = MouseState::MOUSE_SCROLLBAR_DRAG;
      }
      else if (point.y < scroll.top)
      {
        m_MouseState = MouseState::MOUSE_UP_SCROLL;
      }
      else if (point.y > scroll.bottom)
      {
        m_MouseState = MouseState::MOUSE_DOWN_SCROLL;
      }

      if ((m_MouseState == MouseState::MOUSE_UP_SCROLL) ||
        (m_MouseState == MouseState::MOUSE_DOWN_SCROLL))
      {
        ScrollPage((m_MouseState == MouseState::MOUSE_UP_SCROLL) ? -m_thumb_width : m_thumb_width);

        if (m_MouseScrollTimer == NULL)
          m_MouseScrollTimer = SetTimer(TIMER_MOUSE_SCROLL, MOUSE_AUTO_SCROLL_INTERVAL, NULL);
      }

      return;
    }
  }

  // activate page
  int p = pageFromPoint(point);

  if (p >= 0)
  {
    int foldingOver = IsOverFoldingIcon(p, point);
    if (foldingOver >= 0)
    {
      m_MouseState = MouseState::MOUSE_DOWN_FOLDING;
      m_foldingDownIndex = m_foldingOverIndex = foldingOver;

      RECT rect;
      GetFoldingIconRect(m_foldingOverIndex, rect);
      InvalidateRect(&rect, 1);
      return;
    }
  }

  // dragging
  if ((p >= 0) && m_pDocument->IsEditable() && m_pDocument->IsFolderEditable())
  {
    m_MouseState = MouseState::MOUSE_DOWN_SELECT;

    KImageBase* page = m_pDocument->GetImagePage(EXCLUDE_FOLDED, p);
    if ((page != NULL) && page->IsSelected())
    {
      // 이미 선택된 페이지에서 드래그 시작하는 경우, 선택을 변경하지 않고 드래그 시작.
    }
    else
    {
      // 여기서 선택을 하여야 드래깅이 된다.
      if (nFlags & MK_SHIFT)
      {
        int p1, p2;

        if (m_LastClickSelect >= 0)
        {
          p1 = (p < m_LastClickSelect) ? p : m_LastClickSelect;
          p2 = (p < m_LastClickSelect) ? m_LastClickSelect : p;
          m_pDocument->SelectPageRange(p1, p2, EXCLUDE_FOLDED);
        }
      }
      else if (nFlags & MK_CONTROL)
      {
        m_pDocument->ToggleSelectPage(p);
      }
      else
      {
        m_pDocument->SelectPageRange(p, p, EXCLUDE_FOLDED | SELECT_EXCLUSIVE);
      }
      Invalidate(TRUE);
    }

    m_LastClickSelect = p;
    m_LastButtonDownIndex = p;
  }

  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->SetActivePageIndex(p);
}

void KThumbListView::OnLButtonUp(UINT nFlags, CPoint point)
{
  if (m_MouseScrollTimer != NULL)
  {
    KillTimer(m_MouseScrollTimer);
    m_MouseScrollTimer = NULL;
  }

  if (m_MouseState == MouseState::MOUSE_DOWN_FOLDING)
  {
    /*
    int p = pageFromPoint(point);
    if (p >= 0)
    {
      m_foldingOverIndex = IsOverFoldingIcon(p, point);
    }
    */
    if (m_foldingDownIndex == m_foldingOverIndex)
    {
      KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, m_foldingDownIndex);
      p->ToggleFolding();

      // height changed
      RecalcScrollRange();

      Invalidate(TRUE);
    }
    m_foldingDownIndex = m_foldingOverIndex = -1;
  }
  else if (m_MouseState == MouseState::MOUSE_DOWN_SELECT)
  {
    int p = pageFromPoint(point);

    int p1 = (p < m_LastClickSelect) ? p : m_LastClickSelect;
    int p2 = (p < m_LastClickSelect) ? m_LastClickSelect : p;
    int flag = EXCLUDE_FOLDED;
    if (!(nFlags & MK_SHIFT))
      flag |= SELECT_EXCLUSIVE;
    m_pDocument->SelectPageRange(p1, p2, flag);
    Invalidate(TRUE);
  }
  else if (m_MouseState == MouseState::MOUSE_PAGE_DRAG)
  {
    m_MovePageTarget = pageInsertFromPoint(point);

    int p = pageFromPoint(point);
    if ((p >= -1) &&
      (m_MovePageTarget.pageIndex >= 0) &&
      MoveSelectedPages(m_MovePageTarget))
    {
      LPCTSTR targetName[] = {
        _T("Before"),
        _T("Inside"),
        _T("Next")
      };
      TCHAR msg[60] = _T("Dropped at ");
      StringCchPrintf(&msg[lstrlen(msg)], 50, _T("%d, %s"), m_MovePageTarget.pageIndex, targetName[(int)m_MovePageTarget.inside]);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

      Invalidate(TRUE);
      m_MovePageTarget.pageIndex = -2;
    }
  }

  m_MouseState = MouseState::MOUSE_ENTER;
  ReleaseCapture();
}

BOOL KThumbListView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  if (m_ScrollMax > 0)
  {
    int d = m_thumb_width + m_MarginY;
    ScrollPage((zDelta < 0) ? d: -d);
  }
  return FALSE;
}

void KThumbListView::OnRButtonDown(UINT nFlags, CPoint point)
{
  int p = pageFromPoint(point);

  if (!(nFlags & MK_SHIFT))
    m_pDocument->SelectAllPages(FALSE);
  m_pDocument->SelectPage(p, TRUE);
}

void KThumbListView::OnRButtonUp(UINT nFlags, CPoint point)
{
  ClientToScreen(&point);
  BOOL r = theApp.GetContextMenuManager()->ShowPopupMenu(IDR_THUMBNAIL_MENU, point.x, point.y, this, TRUE);
}

void KThumbListView::OnTimer(UINT_PTR timerEvent)
{
  if (timerEvent == m_MouseScrollTimer)
  {
    int size = DialogX2Pixel(21);
    if (size > m_thumb_width)
      size = m_thumb_width;
    if ((m_MouseState == MouseState::MOUSE_UP_SCROLL) ||
      (m_MouseState == MouseState::MOUSE_DOWN_SCROLL))
    {
      ScrollPage((m_MouseState == MouseState::MOUSE_UP_SCROLL) ? -size : size);
    }
    else
    {
      POINT pt;
      RECT rect;
      int offset = 0;

      GetCursorPos(&pt);
      ScreenToClient(&pt);
      GetClientRect(&rect);

      if ((pt.y < (m_MarginY * 2)) && (m_ScrollOffset > 0))
      {
        offset = -size; // m_MarginY;
      }
      else if ((pt.y > (rect.bottom - m_MarginY * 2)) && (m_ScrollOffset < m_ScrollMax))
      {
        offset = size; // m_MarginY;
      }

      if (offset != 0)
      {
        ScrollPage(offset);
      }
      else if (m_MouseScrollTimer != NULL)
      {
        KillTimer(m_MouseScrollTimer);
        m_MouseScrollTimer = NULL;
      }
    }
  }
  if (timerEvent == m_RefreshTimer)
  {
    checkForNeedUpdateThumb(TRUE);
  }
}

void KThumbListView::OnDropFiles(HDROP hdrop)
{
  if (!m_pDocument->IsEditable() || !m_pDocument->IsFolderEditable())
    return;

  POINT pt;
  DragQueryPoint(hdrop, &pt);
  //int p = pageFromPoint(pt);
  PageDragTarget p = pageInsertFromPoint(pt);

  if (p.pageIndex >= 0)
    m_pDocument->SetActivePageIndex(p.pageIndex);

  UINT  numberOfFiles;
  TCHAR szNextFile[MAX_PATH];

  // Get the # of files being dropped.
  numberOfFiles = DragQueryFile(hdrop, -1, NULL, 0);
  int buffsize = 0;
  for (UINT uFile = 0; uFile < numberOfFiles; uFile++)
  {
    if (DragQueryFile(hdrop, uFile, szNextFile, MAX_PATH) > 0)
    {
      buffsize += lstrlen(szNextFile) + 1;
    }
  }
  buffsize++;

  LPTSTR filenames = new TCHAR[buffsize];
  LPTSTR buff = filenames;
  int len = buffsize;

  for (UINT uFile = 0; uFile < numberOfFiles; uFile++)
  {
    if (DragQueryFile(hdrop, uFile, szNextFile, MAX_PATH) > 0)
    {
      StringCchCopy(buff, len, szNextFile);
      buff += lstrlen(szNextFile) + 1;
    }
  }
  *buff = '\0';
  // Free up memory.
  DragFinish(hdrop);

  int flag = CLEAR_SELECT | ADD_SELECT | READY_META_INFO;
  if (p.pageIndex == -1)
    flag |= ADD_ON_TAIL;
  numberOfFiles = m_pDocument->AddImagePagesEx(filenames, buffsize, flag, NULL);
  delete[] filenames;

  Invalidate();

  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->OnActivePageChanged(CHANGED_NUMBER_OF_PAGES|CHANGED_ACTIVE_PAGE | UPDATE_ACTIVE_VIEW);
}

void KThumbListView::OnEditSelectAll()
{
  m_pDocument->SelectAllPages(TRUE);
  Invalidate();
}
void KThumbListView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
  BOOL able = m_pDocument->IsEditable();
  pCmdUI->Enable(TRUE);
}

void KThumbListView::OnEditCut()
{
  HANDLE h = m_pDocument->DeleteSelectedPages();

  if (h != NULL)
  {
    if (OpenClipboard() && EmptyClipboard())
    {
      SetClipboardData(KImageBase::gVivaCustomClipboardFormat, h);
      CloseClipboard();
    }
  }
}

void KThumbListView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
  BOOL able = m_pDocument->IsEditable();
  pCmdUI->Enable(able);
}

void KThumbListView::OnEditCopy()
{
  HANDLE h = m_pDocument->CopyToClipboard(TRUE);
  if (h != NULL)
  {
    if (OpenClipboard() && EmptyClipboard())
    {
      SetClipboardData(KImageBase::gVivaCustomClipboardFormat, h);
      CloseClipboard();
    }
  }
}
void KThumbListView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
  BOOL able = m_pDocument->IsEditable();
  pCmdUI->Enable(able);
}

void KThumbListView::OnEditPaste()
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  CVivaImagingView* v = (CVivaImagingView*)pFrame->GetActiveView();
  v->DoEditPaste();
}
void KThumbListView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
  BOOL able = m_pDocument->IsEditable();
  pCmdUI->Enable(able);
}

void KThumbListView::OnEditDelete()
{
  if (m_pDocument->DeleteSelectedPages())
  {
    Invalidate();
    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->OnActivePageChanged(CHANGED_ACTIVE_PAGE| UPDATE_ACTIVE_VIEW);
  }
}

void KThumbListView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
  BOOL able = m_pDocument->IsEditable();
  pCmdUI->Enable(able);
}

void KThumbListView::RecalcScrollRange()
{
  RECT rect;
  GetClientRect(&rect);
  RecalcScrollRange(rect.right - rect.left, rect.bottom - rect.top);
}

int KThumbListView::calcThumbHeight(KImageBase* p)
{
  if (p->GetType() & FOLDER_PAGE)
  {
    return m_folder_height;
  }
  else
  {
#ifdef VARIABLE_THUMB_HEIGHT
    SIZE sz = ((KImagePage*)p)->GetImageSizeRatio();
    if (sz.cx > 0 && sz.cy > 0)
    {
      float dx = (float)sz.cx / m_thumb_width;
      float dy = (float)sz.cy / m_thumb_width;
      if (dx < dy)
        dx = dy;
      // int ix = (int)(sz.cx / dx);
      return (int)(sz.cy / dx);
    }
    else
#endif
    {
      return m_thumb_width;
    }
  }
}

void KThumbListView::RecalcScrollRange(int cx, int cy)
{
#ifdef USE_MIN_THUMB_SIZE
  // minimum width
  int min_width = m_MarginX * 7;
  if (cx < min_width)
    cx = min_width;
#endif

  m_thumb_width = cx - (m_MarginX + m_MarginX);
  if (m_pDocument == NULL)
    return;

  int pages = m_pDocument->GetNumberOfPages(EXCLUDE_FOLDED);

#ifdef _USE_SAME_HEIGH
  m_ScrollMax = (m_thumb_width + m_MarginY) * pages + m_MarginY - cy;
#else
  int top = m_MarginY;
  int height;

  for (int i = 0; i < pages; i++)
  {
    KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);
    height = calcThumbHeight(p);
    top += height + m_MarginY;
  }
  m_ScrollMax = top - cy;
#endif
  if (m_ScrollMax < 0)
    m_ScrollMax = 0;

  if (m_ScrollOffset > m_ScrollMax)
    m_ScrollOffset = m_ScrollMax;
}

BOOL KThumbListView::GetActiveScrollbarRect(RECT& client, RECT& scrollbar)
{
  if (m_ScrollMax > 0)
  {
    int view_height = client.bottom - client.top;
    float r = (float) view_height / (view_height + m_ScrollMax);
    int scrollbar_height = (int)(view_height * r + 0.5);

    r = (float)m_ScrollOffset / m_ScrollMax;
    scrollbar.top = (int)((view_height - scrollbar_height) * r + 0.5) + client.top;
    scrollbar.bottom = scrollbar.top + scrollbar_height;
    scrollbar.right = client.right;

    scrollbar.left = scrollbar.right - m_MarginX;
    return TRUE;
  }
  return FALSE;
}

void KThumbListView::ScrollPage(int n)
{
  int dx = m_ScrollOffset + n;
  if (dx < 0)
    dx = 0;

  if (dx > m_ScrollMax)
    dx = m_ScrollMax;

  if (dx != m_ScrollOffset)
  {
    m_ScrollOffset = dx;
    Invalidate(1);
  }
}

void KThumbListView::GetScrollIconRect(RECT& rect, BOOL onUp)
{
  RECT client;
  GetClientRect(&client);

  rect.left = client.left + m_MarginX;
  rect.right = client.right - m_MarginX;
  int width = rect.right - rect.left;

  if (onUp)
  {
    rect.top = m_MarginY + 1;
    rect.left = (rect.left + rect.right) / 2 - m_pScrollUpIcon->width / 2;
    rect.bottom = rect.top + m_pScrollUpIcon->height;
    rect.right = rect.left + m_pScrollUpIcon->width;
  }
  else
  {
    rect.left = (rect.left + rect.right) / 2 - m_pScrollDownIcon->width / 2;
    rect.bottom = client.bottom - (m_MarginY + 1);
    rect.right = rect.left + m_pScrollDownIcon->width;
    rect.top = rect.bottom - m_pScrollDownIcon->height;
  }
}

void KThumbListView::checkDraggingScroll(int dy, int bottom)
{
  if (((dy < (m_MarginY * 2)) && (m_ScrollOffset > 0)) ||
    ((dy > (bottom - m_MarginY * 2)) && (m_ScrollOffset < m_ScrollMax)))
  {
    if (m_MouseScrollTimer == NULL)
      m_MouseScrollTimer = SetTimer(TIMER_MOUSE_SCROLL, 200, NULL);
    return;
  }

  if (m_MouseScrollTimer != NULL)
  {
    KillTimer(m_MouseScrollTimer);
    m_MouseScrollTimer = NULL;
  }
}

void KThumbListView::checkForNeedUpdateThumb(BOOL bRender)
{
  if (m_pDocument != NULL)
  {
    int thumb_size = m_thumb_width - 2 * m_MarginInnerX;
    int pages = m_pDocument->GetNumberOfPages(EXCLUDE_FOLDED);
    CDC* pDC = GetDC();

    for (int i = 0; i < pages; i++)
    {
      KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);

      if (!IsVisiblePage(i))
        continue;

      int depth = (p != NULL) ? p->GetDepth() : 1;
      int marginDepth = m_MarginDepth * (depth - 1);
      LPTSTR folderPath = m_pDocument->GetWorkingPath();

      if ((p != NULL) && (p->GetType() == IMAGE_PAGE) &&
        ((KImagePage*)p)->IsNeedRenderThumbImage(folderPath, thumb_size- marginDepth, thumb_size- marginDepth))
      {
        if (m_RefreshTimer == NULL)
          m_RefreshTimer = SetTimer(TIMER_THUMB_REFRESH, 100, NULL);
        if (!bRender)
          return;

        if (((KImagePage*)p)->RenderThumbImage(pDC, folderPath, thumb_size, thumb_size))
          invalidatePage(i, FALSE);

#ifdef _DEBUG
        TCHAR msg[120];
        StringCchPrintf(msg, 120, _T("Render thumb image for %d page\n"), i);
        OutputDebugString(msg);
#endif
        return;
      }
    }

    ReleaseDC(pDC);

    // no need timer
    if (m_RefreshTimer != NULL)
    {
      KillTimer(m_RefreshTimer);
      m_RefreshTimer = NULL;
    }
  }
}

void KThumbListView::getPageRect(int p, BOOL asInsert, RECT& rect)
{
  RECT client;
  GetClientRect(&client);

  rect.left = client.left + m_MarginX;
  rect.right = client.right - m_MarginX;
#ifdef USE_MIN_THUMB_SIZE
  int min_width = m_MarginX * 7;
  if ((rect.left + min_width) > rect.right)
    rect.right = rect.left + min_width;
#endif

#ifdef _USE_SAME_HEIGHT
  if (asInsert)
  {
    rect.top = (m_thumb_width + m_MarginY) * p - m_ScrollOffset + 1;
    rect.bottom = rect.top + m_MarginY - 1;
  }
  else
  {
    rect.top = (m_thumb_width + m_MarginY) * p + m_MarginY - m_ScrollOffset;
    rect.bottom = rect.top + m_thumb_width;
  }
#else
  int top;
  KImageBase* pg;
  int height = 0;

  if (asInsert)
    top = 1 - m_ScrollOffset;
  else
    top = m_MarginY - m_ScrollOffset;
  for (int i = 0; i < p; i++)
  {
    pg = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);
    height = calcThumbHeight(pg);
    top += (height + m_MarginY);
  }
  pg = m_pDocument->GetImagePage(EXCLUDE_FOLDED, p);
  if (pg != NULL)
    height = calcThumbHeight(pg);

  rect.top = top;

  if (asInsert)
    rect.bottom = rect.top + m_MarginY - 1;
  else
    rect.bottom = rect.top + height;
#endif
}

void KThumbListView::invalidatePage(PageDragTarget& target)
{
  int index = target.pageIndex;
  if (target.inside == InsertTargetType::TARGET_NEXT)
    index++;

  RECT rect;
  getPageRect(index, (target.inside != InsertTargetType::TARGET_INSIDE), rect);
  InvalidateRect(&rect, TRUE);
}

void KThumbListView::invalidatePage(int p, BOOL asInsert)
{
  RECT rect;

  getPageRect(p, asInsert, rect);
  InvalidateRect(&rect, TRUE);

#ifdef _DEBUG_UPDATE
  TCHAR msg[128];
  StringCchPrintf(msg, 128, _T("top=%d, bottom=%d"), rect.top, rect.bottom);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
}

int KThumbListView::pageFromPoint(POINT& point)
{
  CRect client;
  GetClientRect(&client);
 
  if (!client.PtInRect(point))
    return -2;

  CRect rect;
  rect.left = client.left + m_MarginX;
  rect.right = client.right - m_MarginX;
  int pages = m_pDocument->GetNumberOfPages(EXCLUDE_FOLDED);

  rect.top = m_MarginY - m_ScrollOffset;
  for (int i = 0; i < pages; i++)
  {
    KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);
    int height = calcThumbHeight(p);
    rect.bottom = rect.top + height;

    if (rect.PtInRect(point))
      return i;
    rect.top += (height + m_MarginY);
  }
  return -1;
}

PageDragTarget KThumbListView::pageInsertFromPoint(POINT& point)
{
  CRect client;
  GetClientRect(&client);
  PageDragTarget target = { -2, NULL, InsertTargetType::TARGET_NEXT };

  if (!client.PtInRect(point))
    return target;

  int pages = m_pDocument->GetNumberOfPages(EXCLUDE_FOLDED);

#ifdef _USE_SAME_HEIGHT
  int y = point.y + m_ScrollOffset - m_MarginY;
  target.pageIndex = y / (m_thumb_width + m_MarginY);
  int offset = y % (m_thumb_width + m_MarginY);
#else
  CRect rect;
  int offset = -1;
  int height = 0;
  rect.top = m_MarginY - m_ScrollOffset;


  // top of list
  /*
  if (point.y <= rect.top)
  {
    target.targetPage = NULL;
    target.inside = TARGET_BEFORE;
  }
  */

  for (int i = 0; i < pages; i++)
  {
    KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);
    height = calcThumbHeight(p);

    int bar = rect.top; // 1 quater
    bar += height / ((p->GetType() & FOLDER_PAGE) ? 4 : 2);
    if (point.y <= bar)
    {
      target.pageIndex = i;
      target.targetPage = p;
      target.inside = InsertTargetType::TARGET_BEFORE;
      break;
    }

    // 다음 항목의 depth 줄어드는 경우
    if (p->GetDepth() > 1)
    {
      KImageBase* n = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i + 1);
      if ((n == NULL) ||
        ((n != NULL) && (p->GetDepth() != n->GetDepth())))
      {
        bar = rect.top + height; // bottom
        if (point.y <= bar)
        {
          target.pageIndex = i;
          target.targetPage = p;
          target.inside = InsertTargetType::TARGET_NEXT;
          break;
        }
      }
    }
    else if (p->GetType() & FOLDER_PAGE)
    {
      bar = rect.top + height * 3 / 4; // bottom
      if (point.y <= bar)
      {
        target.pageIndex = i;
        target.targetPage = p;
        target.inside = InsertTargetType::TARGET_INSIDE;
        break;
      }
    }

    if (i < (pages-1))
      rect.top += (height + m_MarginY);
  }

  if (target.pageIndex < 0)
  {
    target.pageIndex = pages;
    target.targetPage = NULL;// m_pDocument->GetImagePage(EXCLUDE_FOLDED, pages - 1);
    target.inside = InsertTargetType::TARGET_BEFORE;
  }
#endif

  return target;
}

BOOL KThumbListView::IsFolderFoldFocused(int index)
{
  return (m_foldingOverIndex == index);
}

BOOL KThumbListView::GetFoldingIconRect(int index, RECT& rect)
{
  KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, index);
  if (p->GetType() == FOLDER_PAGE)
  {
    RECT client;
    GetClientRect(&client);
    int depth = (p != NULL) ? p->GetDepth() : 1;

#ifdef _USE_SAME_HEIGHT
    rect.top = (m_thumb_width + m_MarginY) * index + m_MarginY - m_ScrollOffset;
    rect.bottom = rect.top + m_thumb_width;
#else
    int top = m_MarginY - m_ScrollOffset;
    for (int i = 0; i < index; i++)
    {
      KImageBase* p = m_pDocument->GetImagePage(EXCLUDE_FOLDED, i);
      int height = calcThumbHeight(p);
      top += height + m_MarginY;
  }
    rect.top = top;
    rect.bottom = rect.top + m_folder_height;
#endif
    rect.left = client.left + m_MarginX + m_MarginDepth * (depth - 1);
    rect.right = client.right - m_MarginX;

    rect.left += m_MarginInnerX;
    rect.top += m_MarginInnerY;
    rect.right -= m_MarginInnerX;
    rect.bottom -= m_MarginInnerY;

    if (m_pFoldingNormIcon != NULL)
    {
      rect.top += (rect.bottom - rect.top - m_pFoldingNormIcon->height) / 2;
      rect.bottom = rect.top + m_pFoldingNormIcon->height;
      rect.right = rect.left + m_pFoldingNormIcon->height;
    }
    return TRUE;
  }
  return FALSE;
}

int KThumbListView::IsOverFoldingIcon(int index, POINT& point)
{
  CRect rect;

  if (GetFoldingIconRect(index, rect))
  {
    if (rect.PtInRect(point))
      return index;
  }
  return -1;
}

void KThumbListView::UpdateThumbView(KIntVector& pageIdList, int flag)
{
  if (m_RefreshTimer == NULL)
    m_RefreshTimer = SetTimer(TIMER_THUMB_REFRESH, 100, NULL);
}

void KThumbListView::UpdateThumbView(BOOL updateAll, int index, int flag)
{
  m_pDocument->ClearThumbImage(updateAll, index);

  if (flag & EFFECT_CHANGE_SIZE)
  {
    RecalcScrollRange();
    Invalidate(TRUE);
  }
  else
  {
    if (m_RefreshTimer == NULL)
      m_RefreshTimer = SetTimer(TIMER_THUMB_REFRESH, 100, NULL);
  }

  // refresh with timer to avoid flashing
  /*
  if (updateAll)
    Invalidate(TRUE);
  else
    invalidatePage(index, FALSE);
  */
}

void KThumbListView::OnActivePageChanged(int flag)
{
  if (m_pDocument == NULL)
  {
    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    m_pDocument = (CVivaImagingDoc *)pFrame->GetActiveDocument();
    if (m_pDocument == NULL)
      return;
  }

  if (flag & INIT_VIEW)
  {
    m_ScrollOffset = 0;
    m_ScrollMax = 0;
    if (m_MouseScrollTimer != NULL)
    {
      KillTimer(m_MouseScrollTimer);
      m_MouseScrollTimer = NULL;
    }

    m_MouseState = MouseState::MOUSE_OFF;
    m_LastButtonDownIndex = -1;
    m_LastClickSelect = -1;
    m_foldingOverIndex = -1;
    m_MovePageTarget.pageIndex = -1;
  }

  if (flag & BEGIN_THUMB_RENDER)
  {
    checkForNeedUpdateThumb(FALSE);
  }

  if (flag & CHANGED_NUMBER_OF_PAGES)
  {
    RecalcScrollRange();
  }

  // scroll to visible active page
  // KImageBase* active = m_pDocument->GetActivePage();
  int active_index = m_pDocument->GetActivePageIndex();
  if (active_index >= 0)
    ScrollToVisiblePage(active_index);

  Invalidate(TRUE);
}

BOOL KThumbListView::IsVisiblePage(int page_index)
{
  RECT client;
  GetClientRect(&client);

  RECT rect;
  getPageRect(page_index, FALSE, rect);

  if (rect.bottom < client.top)
    return FALSE;
  if (rect.top > client.bottom)
    return FALSE;
  return TRUE;
}

void KThumbListView::ScrollToVisiblePage(int page_index)
{
  RECT client;
  GetClientRect(&client);

  RECT rect;
  getPageRect(page_index, FALSE, rect);
  int offset = m_ScrollOffset;
  if (rect.top < 0)
    m_ScrollOffset += rect.top;
  else if (rect.bottom > client.bottom)
    m_ScrollOffset += (rect.bottom - client.bottom);

  if (offset != m_ScrollOffset)
    RecalcScrollRange();
}

BOOL KThumbListView::MoveSelectedPages(PageDragTarget& target)
{
  m_pDocument->MoveSelectedPages(target.pageIndex, target.inside, target.targetPage);
  return TRUE;
}
