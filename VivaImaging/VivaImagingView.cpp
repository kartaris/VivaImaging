// Copyright (C) 2019 K2Mobile
// All rights reserved.


// VivaImagingView.cpp: CVivaImagingView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "VivaImaging.h"
#endif

#include "resource.h"
#include "MainFrm.h"
#include "VivaImagingDoc.h"
#include "VivaImagingView.h"
#include "resource.h"

#include "Core\KBgTaskLocalScan.h"
#include "Core\KBgTaskDownload.h"
#include "Core\KBgTaskUpload.h"
#include "Core\KBgTaskEffectRender.h"

#include "Core\KSyncCoreEngine.h"

#include "Platform\Graphics.h"
#include "Platform\Reg.h"

#include "CDlgImageFormat.h"
#include "KDlgViewZoom.h"
#include "KDlgSelectImageSource.h"
#include "KDlgSetDocType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define _DEBUG_POINTER_EVENT
#else
#define _RENDER_BY_MEMORY_BITMAP
#endif

// CVivaImagingView

IMPLEMENT_DYNCREATE(CVivaImagingView, CView)

BEGIN_MESSAGE_MAP(CVivaImagingView, CView)
	// 표준 인쇄 명령입니다.
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEWHEEL()
  ON_WM_SETCURSOR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_SIZE()
  ON_WM_CREATE()
  ON_WM_DROPFILES()
  ON_WM_TIMER()
  ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CVivaImagingView::OnUpdateFileNew)
  ON_COMMAND(ID_FILE_SAVE, &CVivaImagingView::OnFileSave)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CVivaImagingView::OnUpdateFileSave)
  ON_COMMAND(ID_FILE_SAVE_AS, &CVivaImagingView::OnFileSaveAs)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CVivaImagingView::OnUpdateFileSaveAs)
  ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CVivaImagingView::OnFilePrintPreview)
  ON_COMMAND(ID_FILE_SERVER_IMAGE_SETDOCTYPE, OnFileServerSetDocType)
  ON_UPDATE_COMMAND_UI(ID_FILE_SERVER_IMAGE_SETDOCTYPE, OnUpdateFileServerSetDocType)
  ON_COMMAND(ID_EDIT_SELECT_ALL, &CVivaImagingView::OnEditSelectAll)
  ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CVivaImagingView::OnUpdateEditSelectAll)
  ON_COMMAND(ID_EDIT_CUT, &CVivaImagingView::OnEditCut)
  ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CVivaImagingView::OnUpdateEditCut)
  ON_COMMAND(ID_EDIT_COPY, &CVivaImagingView::OnEditCopy)
  ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CVivaImagingView::OnUpdateEditCopy)
  ON_COMMAND(ID_EDIT_PASTE, &CVivaImagingView::OnEditPaste)
  ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CVivaImagingView::OnUpdateEditPaste)
  ON_COMMAND(ID_EDIT_DELETE, &CVivaImagingView::OnEditDelete)
  ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CVivaImagingView::OnUpdateEditDelete)
  ON_COMMAND(ID_EDIT_UNDO, &CVivaImagingView::OnEditUndo)
  ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CVivaImagingView::OnUpdateEditUndo)
  ON_COMMAND(ID_EDIT_REDO, &CVivaImagingView::OnEditRedo)
  ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CVivaImagingView::OnUpdateEditRedo)
  ON_COMMAND(ID_ALIGN_LEFT, &CVivaImagingView::OnEditAlignLeft)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_LEFT, &CVivaImagingView::OnUpdateEditAlignLeft)
  ON_COMMAND(ID_ALIGN_CENTER, &CVivaImagingView::OnEditAlignCenter)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_CENTER, &CVivaImagingView::OnUpdateEditAlignCenter)
  ON_COMMAND(ID_ALIGN_RIGHT, &CVivaImagingView::OnEditAlignRight)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHT, &CVivaImagingView::OnUpdateEditAlignRight)
  ON_COMMAND(ID_ALIGN_TOP, &CVivaImagingView::OnEditAlignTop)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_TOP, &CVivaImagingView::OnUpdateEditAlignTop)
  ON_COMMAND(ID_ALIGN_VCENTER, &CVivaImagingView::OnEditAlignVCenter)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_VCENTER, &CVivaImagingView::OnUpdateEditAlignVCenter)
  ON_COMMAND(ID_ALIGN_BOTTOM, &CVivaImagingView::OnEditAlignBottom)
  ON_UPDATE_COMMAND_UI(ID_ALIGN_BOTTOM, &CVivaImagingView::OnUpdateEditAlignBottom)
  ON_COMMAND(ID_ORDER_TOP, &CVivaImagingView::OnEditOrderTop)
  ON_UPDATE_COMMAND_UI(ID_ORDER_TOP, &CVivaImagingView::OnUpdateEditOrderTop)
  ON_COMMAND(ID_ORDER_BOTTOM, &CVivaImagingView::OnEditOrderBottom)
  ON_UPDATE_COMMAND_UI(ID_ORDER_BOTTOM, &CVivaImagingView::OnUpdateEditOrderBottom)
  ON_COMMAND(ID_ORDER_FORWARD, &CVivaImagingView::OnEditOrderForward)
  ON_UPDATE_COMMAND_UI(ID_ORDER_FORWARD, &CVivaImagingView::OnUpdateEditOrderForward)
  ON_COMMAND(ID_ORDER_BACKWARD, &CVivaImagingView::OnEditOrderBackward)
  ON_UPDATE_COMMAND_UI(ID_ORDER_BACKWARD, &CVivaImagingView::OnUpdateEditOrderBackward)
  ON_COMMAND(ID_INSERT_FOLDER, &CVivaImagingView::OnInsertFolder)
  ON_UPDATE_COMMAND_UI(ID_INSERT_FOLDER, &CVivaImagingView::OnUpdateInsertFolder)
  ON_COMMAND(ID_INSERT_SCAN_IMAGE, &CVivaImagingView::OnInsertScanImage)
  ON_UPDATE_COMMAND_UI(ID_INSERT_SCAN_IMAGE, &CVivaImagingView::OnUpdateInsertScanImage)
  ON_COMMAND(ID_INSERT_IMAGE, &CVivaImagingView::OnInsertFileImage)
  ON_UPDATE_COMMAND_UI(ID_INSERT_IMAGE, &CVivaImagingView::OnUpdateInsertFileImage)
  ON_COMMAND(ID_MOVEUP_IMAGE, &CVivaImagingView::OnMoveUpImage)
  ON_UPDATE_COMMAND_UI(ID_MOVEUP_IMAGE, &CVivaImagingView::OnUpdateMoveUpImage)
  ON_COMMAND(ID_MOVEDOWN_IMAGE, &CVivaImagingView::OnMoveDownImage)
  ON_COMMAND(ID_VIEW_ZOOM, &CVivaImagingView::OnViewZoom)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM, &CVivaImagingView::OnUpdateViewZoom)
  ON_UPDATE_COMMAND_UI(ID_MOVEDOWN_IMAGE, &CVivaImagingView::OnUpdateMoveDownImage)
  ON_COMMAND(ID_VIEW_ZOOMIN, &CVivaImagingView::OnViewZoomIn)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, &CVivaImagingView::OnUpdateViewZoomIn)
  ON_COMMAND(ID_VIEW_ZOOMOUT, &CVivaImagingView::OnViewZoomOut)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, &CVivaImagingView::OnUpdateViewZoomOut)
  ON_COMMAND(ID_VIEW_ZOOM_REAL, &CVivaImagingView::OnViewReal)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_REAL, &CVivaImagingView::OnUpdateViewReal)
  ON_COMMAND(ID_VIEW_ZOOM_FITTO, &CVivaImagingView::OnViewFitTo)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_FITTO, &CVivaImagingView::OnUpdateViewFitTo)
  ON_COMMAND(ID_VIEW_ZOOM_400, &CVivaImagingView::OnViewZoom400)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_400, &CVivaImagingView::OnUpdateViewZoom400)
  ON_COMMAND(ID_VIEW_ZOOM_200, &CVivaImagingView::OnViewZoom200)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_200, &CVivaImagingView::OnUpdateViewZoom200)
  ON_COMMAND(ID_VIEW_ZOOM_100, &CVivaImagingView::OnViewZoom100)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_100, &CVivaImagingView::OnUpdateViewZoom100)
  ON_COMMAND(ID_VIEW_ZOOM_75, &CVivaImagingView::OnViewZoom75)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_75, &CVivaImagingView::OnUpdateViewZoom75)
  ON_COMMAND(ID_VIEW_ZOOM_50, &CVivaImagingView::OnViewZoom50)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_50, &CVivaImagingView::OnUpdateViewZoom50)
  ON_COMMAND(ID_VIEW_ZOOM_30, &CVivaImagingView::OnViewZoom30)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_30, &CVivaImagingView::OnUpdateViewZoom30)
  ON_COMMAND(ID_IMAGE_ROTATE, &CVivaImagingView::OnImageRotate)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATE, &CVivaImagingView::OnUpdateImageRotate)
  ON_COMMAND(ID_IMAGE_RESIZE, &CVivaImagingView::OnImageResize)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_RESIZE, &CVivaImagingView::OnUpdateImageResize)
  ON_COMMAND(ID_IMAGE_CROP, &CVivaImagingView::OnImageCrop)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_CROP, &CVivaImagingView::OnUpdateImageCrop)
  ON_COMMAND(ID_IMAGE_MONOCHROME, &CVivaImagingView::OnImageMonochrome)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_MONOCHROME, &CVivaImagingView::OnUpdateImageMonochrome)
  ON_COMMAND(ID_IMAGE_COLOR_BALANCE, &CVivaImagingView::OnImageColorBalance)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_COLOR_BALANCE, &CVivaImagingView::OnUpdateImageColorBalance)
  ON_COMMAND(ID_IMAGE_COLOR_LEVEL, &CVivaImagingView::OnImageColorLevel)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_COLOR_LEVEL, &CVivaImagingView::OnUpdateImageColorLevel)
  ON_COMMAND(ID_IMAGE_COLOR_HLS, &CVivaImagingView::OnImageColorHLS)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_COLOR_HLS, &CVivaImagingView::OnUpdateImageColorHLS)
  ON_COMMAND(ID_IMAGE_COLOR_HSV, &CVivaImagingView::OnImageColorHSV)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_COLOR_HSV, &CVivaImagingView::OnUpdateImageColorHSV)
  ON_COMMAND(ID_IMAGE_COLOR_BRIGHT, &CVivaImagingView::OnImageColorBright)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_COLOR_BRIGHT, &CVivaImagingView::OnUpdateImageColorBright)
  ON_COMMAND(ID_IMAGE_COLOR_REVERSE, &CVivaImagingView::OnImageColorReverse)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_COLOR_REVERSE, &CVivaImagingView::OnUpdateImageColorReverse)
  ON_COMMAND(ID_IMAGE_SELECT_RECT, &CVivaImagingView::OnImageSelectRect)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_RECT, &CVivaImagingView::OnUpdateImageSelectRect)
  ON_COMMAND(ID_IMAGE_SELECT_TRIANGLE, &CVivaImagingView::OnImageSelectTriangle)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_TRIANGLE, &CVivaImagingView::OnUpdateImageSelectTriangle)
  ON_COMMAND(ID_IMAGE_SELECT_RHOMBUS, &CVivaImagingView::OnImageSelectRhombus)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_RHOMBUS, &CVivaImagingView::OnUpdateImageSelectRhombus)
  ON_COMMAND(ID_IMAGE_SELECT_PENTAGON, &CVivaImagingView::OnImageSelectPentagon)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_PENTAGON, &CVivaImagingView::OnUpdateImageSelectPentagon)
  ON_COMMAND(ID_IMAGE_SELECT_STAR, &CVivaImagingView::OnImageSelectStar)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_STAR, &CVivaImagingView::OnUpdateImageSelectStar)
  ON_COMMAND(ID_IMAGE_SELECT_ELLIPSE, &CVivaImagingView::OnImageSelectEllipse)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_ELLIPSE, &CVivaImagingView::OnUpdateImageSelectEllipse)
  ON_COMMAND(ID_IMAGE_SELECT_FILL, &CVivaImagingView::OnImageSelectFill)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_SELECT_FILL, &CVivaImagingView::OnUpdateImageSelectFill)
  ON_COMMAND(ID_IMAGE_AUTO_NAMING, &CVivaImagingView::OnImageAutoNaming)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_AUTO_NAMING, &CVivaImagingView::OnUpdateImageAutoNaming)
  ON_COMMAND(ID_IMAGE_ANNOTATE_SELECT, &CVivaImagingView::OnImageAnnotateSelect)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_SELECT, &CVivaImagingView::OnUpdateImageAnnotateSelect)
  ON_COMMAND(ID_IMAGE_ANNOTATE_LINE, &CVivaImagingView::OnImageAnnotateLine)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_LINE, &CVivaImagingView::OnUpdateImageAnnotateLine)
  ON_COMMAND(ID_IMAGE_ANNOTATE_FREEHAND, &CVivaImagingView::OnImageAnnotateFreehand)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_FREEHAND, &CVivaImagingView::OnUpdateImageAnnotateFreehand)
  ON_COMMAND(ID_IMAGE_ANNOTATE_RECTANGLE, &CVivaImagingView::OnImageAnnotateRectangle)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_RECTANGLE, &CVivaImagingView::OnUpdateImageAnnotateRectangle)
  ON_COMMAND(ID_IMAGE_ANNOTATE_ELLIPSE, &CVivaImagingView::OnImageAnnotateEllipse)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_ELLIPSE, &CVivaImagingView::OnUpdateImageAnnotateEllipse)
  ON_COMMAND(ID_IMAGE_ANNOTATE_POLYGON, &CVivaImagingView::OnImageAnnotatePolygon)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_POLYGON, &CVivaImagingView::OnUpdateImageAnnotatePolygon)
  ON_COMMAND(ID_IMAGE_ANNOTATE_TEXTBOX, &CVivaImagingView::OnImageAnnotateTextbox)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_TEXTBOX, &CVivaImagingView::OnUpdateImageAnnotateTextbox)
  ON_COMMAND(ID_IMAGE_ANNOTATE_SHAPES, &CVivaImagingView::OnImageAnnotateShapes)
  ON_UPDATE_COMMAND_UI(ID_IMAGE_ANNOTATE_SHAPES, &CVivaImagingView::OnUpdateImageAnnotateShapes)
#ifdef USE_POINTER_EVENT
  ON_MESSAGE(WM_POINTERENTER, OnWmPointerEnter)
  ON_MESSAGE(WM_POINTERLEAVE, OnWmPointerLeave)
  ON_MESSAGE(WM_POINTERUP, OnWmPointerUp)
  ON_MESSAGE(WM_POINTERDOWN, OnWmPointerDown)
  ON_MESSAGE(WM_POINTERUPDATE, OnWmPointerUpdate)
  ON_MESSAGE(WM_GESTURE, OnWmGesture)
#endif
  ON_MESSAGE(WM_SCAN_IMAGE_DONE, OnScanImageDone)
  ON_MESSAGE(THREADED_CALL_RESULT, OnThreadCallResult)
  ON_MESSAGE(MSG_BGTASK_MESSAGE, OnBgTaskMessage)
END_MESSAGE_MAP()

// CVivaImagingView 생성/소멸

CVivaImagingView::CVivaImagingView()
{
  mMode = PropertyMode::PModeImageView;
  mToggleEditMode = PropertyMode::PModeAnnotateSelect;

  //memset(&mDrawOption, 0, sizeof(IMAGE_DRAW_OPT));
  m_MouseScrollTimer = NULL;
  m_MouseState = MouseState::MOUSE_OFF;
  mpPreviewEffect = NULL;
  mpActiveShape = NULL;
  mpEditWnd = NULL;
  mToolCursor = 0;
}

CVivaImagingView::~CVivaImagingView()
{
  if (mpPreviewEffect != NULL)
  {
    delete mpPreviewEffect;
    mpPreviewEffect = NULL;
  }

  if (mpEditWnd != NULL)
  {
    mpEditWnd->DestroyWindow();
    delete mpEditWnd;
    mpEditWnd = NULL;
  }

}

BOOL CVivaImagingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

#ifdef USE_POINTER_EVENT
  EnableMouseInPointer(TRUE);
#endif

	return CView::PreCreateWindow(cs);
}

int CVivaImagingView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  BOOL enable = ((CVivaImagingDoc*)m_pDocument)->IsEditable();
  DragAcceptFiles(enable);
  return CView::OnCreate(lpCreateStruct);
}

void CVivaImagingView::OnDropFiles(HDROP hdrop)
{
  if (mGlobalOption & GO_READONLY)
    return;

  InsertImageDropFiles(hdrop);
}

// CVivaImagingView 그리기

void CVivaImagingView::OnDraw(CDC* pDC)
{
	CVivaImagingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

  RECT client;
  GetClientRect(&client);

  KImageBase* p = pDoc->GetImagePage(EXCLUDE_FOLDED);
  if (p != NULL)
  {
    if (p->GetType() == IMAGE_PAGE)
    {
#ifdef _RENDER_BY_BG_THREAD
      KBitmap* b = ((KImagePage*)p)->GetImage(pDoc->GetWorkingPath(), NULL);
#else
      KBitmap* b = ((KImagePage*)p)->GetImage(pDoc->GetWorkingPath(), mpPreviewEffect);
#endif

      if (b != NULL)
        pDoc->OnAccessCacheImage((KImagePage*)p);

      if ((b != NULL) && (b->width > 0))
      {
        // check for preview change image size
        if ((mDrawer.src_width != b->width) || (mDrawer.src_height != b->height))
        {
          mDrawer.Set(b->width, b->height, mDrawer.currentZoom, client.right - client.left, client.bottom - client.top);
        }
        //ASSERT((mDrawer.src_width == b->width) && (mDrawer.src_height == b->height));

        RECT rect = { 0, 0, b->width, b->height };
        RECT scrnRect = rect;
        mDrawer.ImageToScreen(scrnRect);

        // clear out-of-image area
        if (pDC->GetRuntimeClass() == RUNTIME_CLASS(CPaintDC))
        {
          CPaintDC* paintDC = (CPaintDC*)pDC;
          COLORREF color = GetSysColor(COLOR_WINDOW);
          mDrawer.PaintRect = paintDC->m_ps.rcPaint;

#ifdef _RENDER_BY_MEMORY_BITMAP
          CDC memDC;
          CBitmap cacheBitmap;
          CRect paintRect(paintDC->m_ps.rcPaint);

          memDC.CreateCompatibleDC(pDC);
          cacheBitmap.CreateCompatibleBitmap(pDC, paintRect.Width(), paintRect.Height());
          HANDLE oldbmp = memDC.SelectObject(cacheBitmap.GetSafeHandle());
          memDC.FillSolidRect(0, 0, paintRect.Width(), paintRect.Height(), color);
          memDC.SetViewportOrg(-paintRect.left, -paintRect.top);

          BITMAP bm;
          cacheBitmap.GetBitmap(&bm);

          b->Blit(memDC, scrnRect, mDrawer.drawOpt.zoom, rect, BLIT_FITTO);

          mDrawer.PaintRect = paintRect;

          // PropertyCtrl handle
          if ((mpPreviewEffect != NULL) &&
            KImagePage::IsPropertyNeedImageOverlayUpdate(mpPreviewEffect->job))
          {
            KPropertyCtrlBase::DrawPropertyCtrlPreview(&memDC, mDrawer, mpPreviewEffect);
          }
          else
          {
            // Annotate Shapes and editor
            Graphics g(memDC.GetSafeHdc());
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            p->DrawEditing(g, mDrawer, mMode, 0);
          }

          memDC.SetViewportOrg(0, 0);
          pDC->BitBlt(paintRect.left, paintRect.top, paintRect.Width(), paintRect.Height(), &memDC, 0, 0, SRCCOPY);

          memDC.SelectObject(oldbmp);
          cacheBitmap.DeleteObject();
          memDC.DeleteDC();

          return;
#else
          if (client.top < scrnRect.top)
          {
            CRect r = client;
            r.bottom = scrnRect.top;

            if (r.IntersectRect(&r, &paintDC->m_ps.rcPaint))
              pDC->FillSolidRect(&r, color);
            client.top = scrnRect.top;
          }
          if (scrnRect.bottom < client.bottom)
          {
            CRect r = client;
            r.top = scrnRect.bottom;

            if (r.IntersectRect(&r, &paintDC->m_ps.rcPaint))
              pDC->FillSolidRect(&r, color);
            client.bottom = scrnRect.bottom;
          }
          if (client.left < scrnRect.left)
          {
            CRect r = client;
            r.right = scrnRect.left;

            if (r.IntersectRect(&r, &paintDC->m_ps.rcPaint))
              pDC->FillSolidRect(&r, color);
          }
          if (scrnRect.right < client.right)
          {
            CRect r = client;
            r.left = scrnRect.right;

            if (r.IntersectRect(&r, &paintDC->m_ps.rcPaint))
              pDC->FillSolidRect(&r, color);
          }
#endif
        }
        else
        {
          mDrawer.PaintRect = client;
        }

        b->Blit(*pDC, scrnRect, mDrawer.drawOpt.zoom, rect, BLIT_FITTO);

        // PropertyCtrl handle
        if ((mpPreviewEffect != NULL) &&
          KImagePage::IsPropertyNeedImageOverlayUpdate(mpPreviewEffect->job))
        {
          KPropertyCtrlBase::DrawPropertyCtrlPreview(pDC, mDrawer, mpPreviewEffect);
        }
        else
        {
          // Annotate Shapes and editor
          Graphics g(pDC->GetSafeHdc());
          g.SetSmoothingMode(SmoothingModeAntiAlias);
          p->DrawEditing(g, mDrawer, mMode, 0);
        }
      }
    }
    else
    {
      if (pDC->GetRuntimeClass() == RUNTIME_CLASS(CPaintDC))
      {
        CPaintDC* paintDC = (CPaintDC*)pDC;
        COLORREF color = GetSysColor(COLOR_WINDOW);
        CRect paintRect(paintDC->m_ps.rcPaint);

        pDC->FillSolidRect(0, 0, paintRect.Width(), paintRect.Height(), color);
      }

      LPCTSTR name = p->GetName();
      if (name != NULL)
      {
        CFont* old_font = pDC->SelectObject(gpUiFont);
        pDC->DrawText(name, &client, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SelectObject(old_font);
      }
    }
  }
  else
  {
    if (pDC->GetRuntimeClass() == RUNTIME_CLASS(CPaintDC))
    {
      CPaintDC* paintDC = (CPaintDC*)pDC;
      COLORREF color = GetSysColor(COLOR_WINDOW);
      pDC->FillSolidRect(&paintDC->m_ps.rcPaint, color);
    }
  }
}

static LPCTSTR keyServerOpenFolder = _T("ServerOpenFolder");
static LPCTSTR keyServerSaveFolder = _T("ServerSaveFolder");
static LPCTSTR keyServerOpenFile = _T("ServerOpenFile");

// CVivaImagingView 인쇄

void CVivaImagingView::OnUpdateFileNew(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable();
  pCmdUI->Enable(able);
}

void CVivaImagingView::DoSaveToServer(ImagingSaveFormat saveFormat)
{
#ifdef _DUPLICATE_TO_SERVER_
  // check online
  if (gpCoreEngine->GetSyncStatus() < SYNC_STATUS_IDLE)
  {
    AfxMessageBox(IDS_ABORT_ACTION_BY_NO_LOGIN, MB_OK);
    return;
  }

  LPCTSTR serverFolder = ((CVivaImagingDoc*)m_pDocument)->GetServerUploadFolder();
  if ((serverFolder == NULL) || (lstrlen(serverFolder) == 0))
    serverFolder = mUploadServerFolderOID; // default upload folder

  TCHAR serverFolderPath[KMAX_PATH];
  if ((serverFolder == NULL) || (lstrlen(serverFolder) == 0) ||
    !gpCoreEngine->SyncServerGetFolderName(serverFolder, serverFolderPath, TRUE, NULL))
  {
    gpCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), keyServerSaveFolder, serverFolder);
    return;
  }
  else
  {
    ((CVivaImagingDoc*)m_pDocument)->SetServerUploadFolder(serverFolder);
  }

  if (lstrlen(serverFolder) > 0)
  {
    CString filename = ((CVivaImagingDoc*)m_pDocument)->GetTitle();
    OnStartUpload(serverFolder, filename, saveFormat, mGlobalOption);
  }
#endif
}

void CVivaImagingView::OnFileSave()
{
  int numberOfImages = ((CVivaImagingDoc*)m_pDocument)->GetNumberOfPages(IMAGE_ONLY);
  if (numberOfImages < 1)
  {
    CString str;
    str.LoadString(AfxGetInstanceHandle(), IDS_CANNOT_SAVE_NO_IMAGE, mResourceLanguage);
    AfxMessageBox(str, MB_OK);
    return;
  }

  ImagingSaveFormat saveFormat = ((CVivaImagingDoc*)m_pDocument)->OnFileSave();

#ifdef _DUPLICATE_TO_SERVER_
  if ((FORMAT_UNKNOWN < saveFormat) && (saveFormat < MAX_SAVE_IMAGE_FORMAT))
  {
    DoSaveToServer(saveFormat);
  }
#endif
}

void CVivaImagingView::OnUpdateFileSave(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnFileSaveAs()
{
  ImagingSaveFormat defaultFormat = ((CVivaImagingDoc*)m_pDocument)->GetCurrentFormat();

  CDlgImageFormat dlgImageFormat(this, defaultFormat);
  if (dlgImageFormat.DoModal())
  {
    ((CVivaImagingDoc*)m_pDocument)->ReadyWorkingPath();
    ((CVivaImagingDoc*)m_pDocument)->SetCurrentFormat(dlgImageFormat.mUploadFileType);
    BOOL result = m_pDocument->DoSave(NULL);
#ifdef _DUPLICATE_TO_SERVER_
    if (result)
    {
      DoSaveToServer(dlgImageFormat.mUploadFileType);
    }
#endif
  }
}

void CVivaImagingView::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable();
  pCmdUI->Enable(able);
}

void CVivaImagingView::DoFileSaveAs(LPCTSTR serverFolder, LPCTSTR name, ImagingSaveFormat format, UINT flag)
{
  ((CVivaImagingDoc*)m_pDocument)->SetCurrentFormat(format);
  ((CVivaImagingDoc*)m_pDocument)->SetServerUploadFolder(serverFolder);

  format = ((CVivaImagingDoc*)m_pDocument)->OnFileSaveAs(name);
  if (format > FORMAT_UNKNOWN)
  {
    OnStartUpload(serverFolder, name, format, flag);
  }
}

void CVivaImagingView::DoFileOpenServerFolder()
{
#ifdef SELECT_BY_ECM_SERVER_FOLDER
  TCHAR folderOID[MAX_OID] = _T("");
  gpCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), keyServerOpenFolder, folderOID);
#else
  BROWSEINFO brInfo;

  memset(&brInfo, 0x00, sizeof(BROWSEINFO));

  brInfo.hwndOwner = GetSafeHwnd();
  brInfo.pidlRoot = NULL;
  //brInfo.pszDisplayName = _T("Select open folder");
  brInfo.lpszTitle = _T("Select open folder");
  brInfo.ulFlags = 0; // BIF_RETURNONLYFSDIRS;
  brInfo.lpfn = NULL;

  ITEMIDLIST* pidlBrowse = SHBrowseForFolder(&brInfo);
  if (pidlBrowse != NULL)
  {
    TCHAR path[KLONG_PATH];
    if (SHGetPathFromIDListEx(pidlBrowse, path, KLONG_PATH, GPFIDL_DEFAULT))
      ((CVivaImagingDoc*)m_pDocument)->OpenDocumentFolder(path);
  }
#endif
}

void CVivaImagingView::DoFileOpenServerFile()
{
  TCHAR folderOID[MAX_OID] = _T("");

  gpCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), keyServerOpenFile, folderOID);
}

void CVivaImagingView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CVivaImagingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
  pInfo->SetMaxPage(((CVivaImagingDoc*)m_pDocument)->GetNumberOfPages(IMAGE_ONLY));
	return DoPreparePrinting(pInfo);
}

void CVivaImagingView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  mpActivePageBackup = ((CVivaImagingDoc*)m_pDocument)->GetActivePage();
  mBackupDrawer = mDrawer;
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CVivaImagingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
  if (mpActivePageBackup != NULL)
  {
    mDrawer = mBackupDrawer;
    ((CVivaImagingDoc*)m_pDocument)->SetActivePage(mpActivePageBackup);
  }
}

void CVivaImagingView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
  UNREFERENCED_PARAMETER(pInfo);

  // Print headers and/or footers, if desired.
  // Find portion of document corresponding to pInfo->m_nCurPage.
  ((CVivaImagingDoc*)m_pDocument)->SetActivePageIndex(pInfo->m_nCurPage - 1);
  KImagePage* p = (KImagePage*)((CVivaImagingDoc*)m_pDocument)->GetActivePage();
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    LPCTSTR path = ((CVivaImagingDoc*)m_pDocument)->GetWorkingPath();
    KBitmap* b = p->GetImage(path);
    int paper_width = pInfo->m_rectDraw.right - pInfo->m_rectDraw.left;
    int paper_height = pInfo->m_rectDraw.bottom - pInfo->m_rectDraw.top;

    mDrawer.Set(b->width, b->height, 0, paper_width, paper_height);
      // fit to page area
    OnDraw(pDC);
  }
}

void CVivaImagingView::OnFileServerSetDocType()
{
  KDlgSetDocType docTypeDlg(this);

  LPCTSTR oid = ((CVivaImagingDoc*)m_pDocument)->GetTargetFolderOid();
  if (lstrlen(oid) > 0)
    StringCchCopy(docTypeDlg.mServerFolderOID, MAX_OID, oid);
  oid = ((CVivaImagingDoc*)m_pDocument)->GetDocTypeOid();
  if (lstrlen(oid) > 0)
    StringCchCopy(docTypeDlg.mDocTypeOid, MAX_OID, oid);

  if (docTypeDlg.DoModal() == IDOK)
  {
    // update image metadata type
    ((CVivaImagingDoc*)m_pDocument)->SetDocTypeInfo(docTypeDlg.mDocTypeInfo);

     // property panel update
    CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
    pFrame->UpdatePropertyMode();
  }
}

void CVivaImagingView::OnUpdateFileServerSetDocType(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}

// CVivaImagingView 진단

#ifdef _DEBUG
void CVivaImagingView::AssertValid() const
{
	CView::AssertValid();
}

void CVivaImagingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVivaImagingDoc* CVivaImagingView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVivaImagingDoc)));
	return (CVivaImagingDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL CVivaImagingView::SetActivePageIndex(int pageIndex)
{
  // 페이지 이동시에 작업중인 내용 클리어
  ClearImageMode(FALSE, PropertyMode::PModeImageView);

  BOOL bOk = ((CVivaImagingDoc*)m_pDocument)->SetActivePageIndex(pageIndex);

  if (bOk)
  {
#ifdef _SET_DEFAULT_TOOL_WHEN_PAGE_CHANGE
    // 페이지 이동시에 툴 모드를 바꾼다면..
    SetImageMode(PropertyMode::PModeImageView);
#endif

    OnActivePageChanged(CHANGED_ACTIVE_PAGE);
    return TRUE;
  }
  return FALSE;
}

void CVivaImagingView::UpdateThumbView(KIntVector& pageIdList, int flag)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->UpdateThumbView(pageIdList, flag);
}

void CVivaImagingView::UpdateThumbView(BOOL updateAll, int index, int flag)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->UpdateThumbView(updateAll, index, flag);
}

void CVivaImagingView::OnActivePageChange(int flag)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->OnActivePageChanged(flag);
}

void CVivaImagingView::OnActivePageChanged(int flag)
{
  Invalidate(FALSE);

  // ZOOM_FIT_TO로 설정한다.
  // mDrawOption.zoom = 0.0;
  RECT client;
  GetClientRect(&client);

  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    LPCTSTR path = ((CVivaImagingDoc*)m_pDocument)->GetWorkingPath();
    KBitmap* b = ((KImagePage*)p)->GetImage(path);
    if (b != NULL)
      mDrawer.Set(b->width, b->height, 0.0, (client.right - client.left), (client.bottom - client.top));
  }
  else
  {
    mDrawer.Set(0, 0, 0.0, (client.right - client.left), (client.bottom - client.top));
  }
}

void CVivaImagingView::OnPageSizeChanged(UINT flag)
{
  RECT client;
  GetClientRect(&client);

  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    LPCTSTR path = ((CVivaImagingDoc*)m_pDocument)->GetWorkingPath();
    KBitmap* b = ((KImagePage*)p)->GetImage(path);

    mDrawer.Set(b->width, b->height, mDrawer.drawOpt.zoom, (client.right - client.left), (client.bottom - client.top));
  }
  else
  {
    mDrawer.Set(0, 0, mDrawer.drawOpt.zoom, (client.right - client.left), (client.bottom - client.top));
  }
  CheckScrollOffset();
}

// CVivaImagingView 메시지 처리기
void CVivaImagingView::OnEditSelectAll()
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    RECT updateRect;
    int state = p->SelectAll(mMode, mDrawer, &updateRect);
    if (state != 0)
    {
      mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
      InvalidateRect(&updateRect, FALSE);
    }
  }
}

void CVivaImagingView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditCut()
{
  HANDLE h = NULL;
  int clipboard_data_format = 0;
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if (mMode == PropertyMode::PModeImageView)
  {
    HANDLE h = ((CVivaImagingDoc*)m_pDocument)->DeleteSelectedPages();
  }
  else if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    h = p->CopyToClipboard(mMode, TRUE, clipboard_data_format);
  }
  if (h != NULL)
  {
    if (OpenClipboard() && EmptyClipboard())
    {
      SetClipboardData(clipboard_data_format, h);
      CloseClipboard();
    }

    if (mMode == PropertyMode::PModeImageView)
    {
      OnActivePageChange(CHANGED_ALL);
      OnActivePageChanged(CHANGED_ACTIVE_PAGE);
    }
  }
}

void CVivaImagingView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if (p != NULL)
    {
      if (p->IsEditable())
      {
        if (p->GetType() == IMAGE_PAGE)
        {
          if (IsSelectBlockMode(mMode))
            able = p->HasSelection(SELECTION_IMAGE_BLOCK);
          else if (IsAnnotateMode(mMode))
            able = p->HasSelection(SELECTION_ANNOTATE);
          else // PModeImageView
            able = TRUE;
        }
      }
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditCopy()
{
  HANDLE h = NULL;
  int clipboard_data_format = 0;
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if (mMode == PropertyMode::PModeImageView)
  {
    h = ((CVivaImagingDoc*)m_pDocument)->CopyToClipboard(TRUE);
    clipboard_data_format = KImageBase::gVivaCustomClipboardFormat;
  }
  else if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    h = p->CopyToClipboard(mMode, FALSE, clipboard_data_format);
  }

  if (h != NULL)
  {
    if (OpenClipboard() && EmptyClipboard())
    {
      SetClipboardData(clipboard_data_format, h);
      CloseClipboard();
    }
  }
}

void CVivaImagingView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
    {
      if (IsSelectBlockMode(mMode))
        able = p->HasSelection(SELECTION_IMAGE_BLOCK);
      else if (IsAnnotateMode(mMode))
        able = p->HasSelection(SELECTION_ANNOTATE);
      else // PModeImageView
        able = TRUE;
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditPaste()
{
  if ( OpenClipboard())
  {
    HANDLE b = NULL;
    BOOL done = FALSE;

    // 1. check native format
    b = GetClipboardData(KImageBase::gVivaCustomClipboardFormat);
    if (b != NULL)
    {
      PasteClipboardData(b);
      done = TRUE;
    }
    else
    {
      // 2. check files
      b = GetClipboardData(CF_HDROP);
      if (b != NULL)
      {
        HDROP hDrop = (HDROP)GlobalLock(b);
        done = ((hDrop != NULL) && (InsertImageDropFiles(hDrop) > 0));
        GlobalUnlock(b);
      }
    }

    // 3. check text format(as annotation)

    // 4. check image format
    if (!done)
    {
      b = GetClipboardData(CF_DIBV5);
      if (b == NULL)
        b = GetClipboardData(CF_DIB);
      if (b != NULL)
      {
        PasteClipboardImage(b);
      }
    }

    /*
    int errstr = IDS_NO_IMAGE_ON_CLIPBOARD;
    if (errstr > 0)
    {
      AfxMessageBox(errstr);
    }
    */
    CloseClipboard();
  }
}

void CVivaImagingView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable();
  if (able)
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    able = (p != NULL) && p->IsEditable();
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditDelete()
{
  if (IsAnnotateMode(mMode))
  {
    RECT updateRect = { 0, 0, 0, 0 };
    int state = ((CVivaImagingDoc*)m_pDocument)->DeleteSelection(&updateRect);
    if (state != 0)
    {
      int f = (state & R_REDRAW_HANDLE) ? WITH_HANDLE : 0;
      mDrawer.ImageToScreen(updateRect, f);
      InvalidateRect(&updateRect, FALSE);

      if (state & R_REDRAW_THUMB)
      {
        int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
        UpdateThumbView(FALSE, index, 0);
      }
    }

    // TODO:should save action.
    /*
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
    {
      RECT updateRect;
      if (p->DeleteSelection(&updateRect))
      {
        mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
        InvalidateRect(&updateRect, FALSE);
        return;
      }
    }
    */
    return;
  }

  // else delete slide
  HANDLE h = ((CVivaImagingDoc*)m_pDocument)->DeleteSelectedPages();
  if (h != NULL)
  {
    GlobalFree(h);
    OnActivePageChange(CHANGED_ALL);
    OnActivePageChanged(CHANGED_ACTIVE_PAGE);
  }
}

void CVivaImagingView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable())
    {
      if (p->GetType() == IMAGE_PAGE)
      {
        able = p->HasSelection(SELECTION_ANNOTATE);
      }
      if (!able)
      {
        able = ((CVivaImagingDoc*)m_pDocument)->GetNumberOfPages(EXCLUDE_FOLDED) > 0;
      }
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditUndo()
{
  int r = ((CVivaImagingDoc*)m_pDocument)->Undo();
  UpdateActionResult(r);
}
void CVivaImagingView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->CanUndo();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditRedo()
{
  int r = ((CVivaImagingDoc*)m_pDocument)->Redo();
  UpdateActionResult(r);
}
void CVivaImagingView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->CanRedo();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditAlignLeft()
{
  int params[] = { SHAPE_ALIGN_LEFT, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditAlignLeft(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditAlignCenter()
{
  int params[] = { SHAPE_ALIGN_CENTER, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditAlignCenter(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditAlignRight()
{
  int params[] = { SHAPE_ALIGN_RIGHT, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditAlignRight(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditAlignTop()
{
  int params[] = { SHAPE_ALIGN_TOP, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditAlignTop(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditAlignVCenter()
{
  int params[] = { SHAPE_ALIGN_VCENTER, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditAlignVCenter(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditAlignBottom()
{
  int params[] = { SHAPE_ALIGN_BOTTOM, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditAlignBottom(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditOrderTop()
{
  int params[] = { SHAPE_ORDER_TOP, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditOrderTop(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditOrderBottom()
{
  int params[] = { SHAPE_ORDER_BOTTOM, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditOrderBottom(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditOrderForward()
{
  int params[] = { SHAPE_ORDER_FORWARD, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditOrderForward(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnEditOrderBackward()
{
  int params[] = { SHAPE_ORDER_BACKWARD, 0 };
  ShapeEditCommand(0, params, NULL);
}

void CVivaImagingView::OnUpdateEditOrderBackward(CCmdUI* pCmdUI)
{
  BOOL able = FALSE;
  if (((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
    {
      able = p->HasSelection(SELECTION_ANNOTATE);
    }
  }
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnInsertFolder()
{
  if (m_pDocument != NULL)
  {
    if (((CVivaImagingDoc*)m_pDocument)->AddNewFolder(NULL, CLEAR_SELECT|ADD_SELECT, NULL))
    {
      OnActivePageChange(CHANGED_ACTIVE_PAGE|CHANGED_NUMBER_OF_PAGES);
    }
  }
}

void CVivaImagingView::OnUpdateInsertFolder(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->IsFolderEditable();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnInsertScanImage()
{
  KDlgSelectImageSource dlgSelectSource(this);

  // set working folder
  dlgSelectSource.SetWorkingPath(((CVivaImagingDoc*)m_pDocument)->GetWorkingPath());

  dlgSelectSource.DoModal();
}

void CVivaImagingView::OnUpdateInsertScanImage(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->IsFolderEditable();

  pCmdUI->Enable(able);
}

TCHAR StrOpenFileExtensions[] = _T("Image files(*.BMP,*.JPG,*.JPEG,*.PNG,*.TIF,*.TIFF)\0*.BMP;*.JPG;*.JPEG;*.PNG;*.TIF;*.TIFF\0All files(*.*)\0*.*\0\0");

void CVivaImagingView::OnInsertFileImage()
{
  OPENFILENAME of;
  LPTSTR filenames = new TCHAR[10240];
  filenames[0] = '\0';

  memset(&of, 0, sizeof(OPENFILENAME));
  of.lStructSize = sizeof(OPENFILENAME);
  of.hwndOwner = GetSafeHwnd();
  of.hInstance = AfxGetInstanceHandle();
  of.lpstrFilter = StrOpenFileExtensions;
  of.lpstrCustomFilter = NULL;
  of.nMaxCustFilter = 0;
  of.nFilterIndex = 0;
  of.lpstrFile = filenames;
  of.nMaxFile = 10240;
  of.lpstrFileTitle = NULL;
  of.nMaxFileTitle = 0;
  of.lpstrInitialDir = NULL;
  of.lpstrTitle = _T("Select files to insert");
  of.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_HIDEREADONLY;
  if ((m_pDocument != NULL) && GetOpenFileName(&of))
  {
    InsertNewImagePages(filenames);
  }
  delete[] filenames;
}

void CVivaImagingView::OnUpdateInsertFileImage(CCmdUI* pCmdUI)
{
  BOOL able = ((CVivaImagingDoc*)m_pDocument)->IsEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->IsFolderEditable();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnMoveUpImage()
{
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->MovePreviousPage())
  {
    OnActivePageChange(0);
  }
}
void CVivaImagingView::OnUpdateMoveUpImage(CCmdUI* pCmdUI)
{
  BOOL able = (m_pDocument != NULL) && ((CVivaImagingDoc*)m_pDocument)->IsEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->IsFolderEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->CanGotoPreviousPage();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnMoveDownImage()
{
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->MoveNextPage())
  {
    OnActivePageChange(0);
  }
}
void CVivaImagingView::OnUpdateMoveDownImage(CCmdUI* pCmdUI)
{
  BOOL able = (m_pDocument != NULL) && ((CVivaImagingDoc*)m_pDocument)->IsEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->IsFolderEditable() &&
    ((CVivaImagingDoc*)m_pDocument)->CanGotoNextPage();
  pCmdUI->Enable(able);
}

void CVivaImagingView::OnMenuPageUp()
{
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->GotoPreviousPage())
  {
    OnActivePageChange(CHANGED_ACTIVE_PAGE);
  }
}

void CVivaImagingView::OnUpdateMenuPageUp(CCmdUI* pCmdUI)
{
  if (m_pDocument != NULL)
    pCmdUI->Enable(((CVivaImagingDoc*)m_pDocument)->CanGotoPreviousPage());
}

void CVivaImagingView::OnMenuPageDown()
{
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->GotoNextPage())
  {
    OnActivePageChange(CHANGED_ACTIVE_PAGE);
  }
}
void CVivaImagingView::OnUpdateMenuPageDown(CCmdUI* pCmdUI)
{
  if (m_pDocument != NULL)
    pCmdUI->Enable(((CVivaImagingDoc*)m_pDocument)->CanGotoNextPage());
}

void CVivaImagingView::OnViewZoom()
{
  KDlgViewZoom dlgView(this);

  dlgView.mZoom = mDrawer.GetZoom();
  dlgView.mbFitTo = (dlgView.mZoom == (float)0.0);
  if (dlgView.mbFitTo)
    dlgView.mZoom = mDrawer.currentZoom;

  if (dlgView.DoModal())
  {
    mDrawer.SetZoom(dlgView.GetZoom());
    Invalidate(FALSE);
  }
}
void CVivaImagingView::OnUpdateViewZoom(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}

void CVivaImagingView::OnViewZoomIn()
{
  if (ZoomScreen(1))
    Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoomIn(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}

void CVivaImagingView::OnViewZoomOut()
{
  if (ZoomScreen(-1))
    Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoomOut(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}

void CVivaImagingView::OnViewReal()
{
  mDrawer.SetZoom(1.0);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewReal(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 1.0);
}

void CVivaImagingView::OnViewFitTo()
{
  mDrawer.SetZoom(0.0);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewFitTo(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 0.0);
}

void CVivaImagingView::OnViewZoom400()
{
  mDrawer.SetZoom(4.0);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoom400(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 4.0);
}

void CVivaImagingView::OnViewZoom200()
{
  mDrawer.SetZoom(2.0);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoom200(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 2.0);
}

void CVivaImagingView::OnViewZoom100()
{
  mDrawer.SetZoom(1.0);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoom100(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 1.0);
}

void CVivaImagingView::OnViewZoom75()
{
  mDrawer.SetZoom(0.75);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoom75(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 0.75);
}

void CVivaImagingView::OnViewZoom50()
{
  mDrawer.SetZoom(0.5);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoom50(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 0.5);
}

void CVivaImagingView::OnViewZoom30()
{
  mDrawer.SetZoom(0.0);
  Invalidate(FALSE);
}
void CVivaImagingView::OnUpdateViewZoom30(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
  pCmdUI->SetCheck(mDrawer.drawOpt.zoom == 0.3);
}

void CVivaImagingView::OnImageRotate()
{
  SetImageMode(PropertyMode::PModeRotate);
}
void CVivaImagingView::OnUpdateImageRotate(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeRotate);
}

void CVivaImagingView::OnImageResize()
{
  SetImageMode(PropertyMode::PModeResize);
}
void CVivaImagingView::OnUpdateImageResize(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeResize);
}

void CVivaImagingView::OnImageCrop()
{
  SetImageMode(PropertyMode::PModeCrop);
}
void CVivaImagingView::OnUpdateImageCrop(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeCrop);
}

void CVivaImagingView::OnImageMonochrome()
{
  SetImageMode(PropertyMode::PModeMonochrome);
}
void CVivaImagingView::OnUpdateImageMonochrome(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeMonochrome);
}

void CVivaImagingView::OnImageColorBalance()
{
  SetImageMode(PropertyMode::PModeColorBalance);
}
void CVivaImagingView::OnUpdateImageColorBalance(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeColorBalance);
}

void CVivaImagingView::OnImageColorLevel()
{
  SetImageMode(PropertyMode::PModeColorLevel);
}
void CVivaImagingView::OnUpdateImageColorLevel(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeColorLevel);
}
void CVivaImagingView::OnImageColorHLS()
{
  SetImageMode(PropertyMode::PModeColorHLS);
}

void CVivaImagingView::OnUpdateImageColorHLS(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable(EDITABLE_TRUECOLOR) && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeColorHLS);
}

void CVivaImagingView::OnImageColorHSV()
{
  SetImageMode(PropertyMode::PModeColorHSV);
}
void CVivaImagingView::OnUpdateImageColorHSV(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable(EDITABLE_TRUECOLOR) && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeColorHSV);
}

void CVivaImagingView::OnImageColorBright()
{
  SetImageMode(PropertyMode::PModeColorBright);
}
void CVivaImagingView::OnUpdateImageColorBright(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeColorBright);
}
void CVivaImagingView::OnImageColorReverse()
{
  SetImageMode(PropertyMode::PModeColorReverse);
}
void CVivaImagingView::OnUpdateImageColorReverse(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeColorReverse);
}

void CVivaImagingView::OnImageSelectRect()
{
  SetImageMode(PropertyMode::PModeSelectRectangle);
}
void CVivaImagingView::OnUpdateImageSelectRect(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectRectangle);
}
void CVivaImagingView::OnImageSelectTriangle()
{
  SetImageMode(PropertyMode::PModeSelectTriangle);
}
void CVivaImagingView::OnUpdateImageSelectTriangle(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectTriangle);
}
void CVivaImagingView::OnImageSelectRhombus()
{
  SetImageMode(PropertyMode::PModeSelectRhombus);
}
void CVivaImagingView::OnUpdateImageSelectRhombus(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectRhombus);
}
void CVivaImagingView::OnImageSelectPentagon()
{
  SetImageMode(PropertyMode::PModeSelectPentagon);
}
void CVivaImagingView::OnUpdateImageSelectPentagon(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectPentagon);
}
void CVivaImagingView::OnImageSelectStar()
{
  SetImageMode(PropertyMode::PModeSelectStar);
}
void CVivaImagingView::OnUpdateImageSelectStar(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectStar);
}

void CVivaImagingView::OnImageSelectEllipse()
{
  SetImageMode(PropertyMode::PModeSelectEllipse);
}
void CVivaImagingView::OnUpdateImageSelectEllipse(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectEllipse);
}
void CVivaImagingView::OnImageSelectFill()
{
  SetImageMode(PropertyMode::PModeSelectFill);
}
void CVivaImagingView::OnUpdateImageSelectFill(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeSelectFill);
}
void CVivaImagingView::OnImageAutoNaming()
{
  //SetImageMode(PModeColorHLS);
}
void CVivaImagingView::OnUpdateImageAutoNaming(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(TRUE);
}
void CVivaImagingView::OnImageAnnotateSelect()
{
  SetImageMode(PropertyMode::PModeAnnotateSelect);
}
void CVivaImagingView::OnUpdateImageAnnotateSelect(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotateSelect);
}
void CVivaImagingView::OnImageAnnotateLine()
{
  SetImageMode(PropertyMode::PModeAnnotateLine);
}
void CVivaImagingView::OnUpdateImageAnnotateLine(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotateLine);
}
void CVivaImagingView::OnImageAnnotateFreehand()
{
  SetImageMode(PropertyMode::PModeAnnotateFreehand);
}
void CVivaImagingView::OnUpdateImageAnnotateFreehand(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotateFreehand);
}
void CVivaImagingView::OnImageAnnotateRectangle()
{
  SetImageMode(PropertyMode::PModeAnnotateRectangle);
}
void CVivaImagingView::OnUpdateImageAnnotateRectangle(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotateRectangle);
}
void CVivaImagingView::OnImageAnnotateEllipse()
{
  SetImageMode(PropertyMode::PModeAnnotateEllipse);
}
void CVivaImagingView::OnUpdateImageAnnotateEllipse(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotateEllipse);
}
void CVivaImagingView::OnImageAnnotatePolygon()
{
  SetImageMode(PropertyMode::PModeAnnotatePolygon);
}
void CVivaImagingView::OnUpdateImageAnnotatePolygon(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotatePolygon);
}

void CVivaImagingView::OnImageAnnotateTextbox()
{
  SetImageMode(PropertyMode::PModeAnnotateTextbox);
}
void CVivaImagingView::OnUpdateImageAnnotateTextbox(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(GetImageMode() == PropertyMode::PModeAnnotateTextbox);
}

void CVivaImagingView::OnImageAnnotateShapes()
{
  CString str;
  int index = CMFCRibbonGallery::GetLastSelectedItem(ID_IMAGE_ANNOTATE_SHAPES);
  str.Format(_T("Insert Shape from Palette %d\n"), index);
  OutputDebugString(str);

  SetImageMode((PropertyMode)((int)PropertyMode::PModeAnnotateShape + index));
}

void CVivaImagingView::OnUpdateImageAnnotateShapes(CCmdUI* pCmdUI)
{
  BOOL enable = FALSE;
  if ((m_pDocument != NULL) &&
    ((CVivaImagingDoc*)m_pDocument)->IsEditable())
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
    enable = ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE));
  }
  pCmdUI->Enable(enable);
  pCmdUI->SetCheck(IsAnnotateShapeMode(GetImageMode()));
}

void CVivaImagingView::OnMouseMove(UINT nFlags, CPoint point)
{
  // pen/touch dragging 이면 생략한다
  if ((m_LastPointer.type == PT_TOUCH) || (m_LastPointer.type == PT_PEN))
    return;

  OnEditMouseMove(nFlags, point, 0);
}

void CVivaImagingView::OnMouseLeave()
{
  if (m_MouseState != MouseState::MOUSE_OFF)
  {
    m_MouseState = MouseState::MOUSE_OFF;
    Invalidate(FALSE);
  }
}

void CVivaImagingView::OnLButtonDown(UINT nFlags, CPoint point)
{
  // 펜&터치 다운이 마우스 이벤트로 오는 것 방지.
  if (m_MouseState == MouseState::MOUSE_OFF)
    OnEditLButtonDown(nFlags, point, 0);
}

void CVivaImagingView::OnLButtonUp(UINT nFlags, CPoint point)
{
  OnEditLButtonUp(nFlags, point, 0);
}

void CVivaImagingView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  SetImageMode(PropertyMode::PModeImageView);
}

void CVivaImagingView::OnRButtonDown(UINT nFlags, CPoint point)
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    // check if selected object exist on point
    RECT updateRect = { 0, 0, 0, 0 };
    POINT dp = point;
    mDrawer.ScreenToImage(dp);
    int state = p->OnRButtonDown(mMode, nFlags, dp, mDrawer, &updateRect);
    if (state != 0)
    {
      int f = (state & R_REDRAW_HANDLE) ? WITH_HANDLE : 0;
      mDrawer.ImageToScreen(updateRect, f);
      InvalidateRect(&updateRect, FALSE);
    }
  }
  m_MouseState = MouseState::MOUSE_CONTEXT_MENU;
}

void CVivaImagingView::OnRButtonUp(UINT nFlags, CPoint point)
{
  if (m_MouseState == MouseState::MOUSE_CONTEXT_MENU)
  {
    ClientToScreen(&point);
    
    int menu_res = 0;
    if (PropertyMode::PModeImageView == mMode)
      menu_res = IDR_POPUP_IMAGE_VIEW;
    else if ((PropertyMode::PModeSelectRectangle <= mMode) && (mMode <= PropertyMode::PModePasteImage))
      menu_res = IDR_POPUP_BLOCK_EDIT;
    else
      menu_res = IDR_POPUP_SHAPE_EDIT;
    BOOL r = theApp.GetContextMenuManager()->ShowPopupMenu(menu_res, point.x, point.y, this, TRUE);
    m_MouseState = MouseState::MOUSE_OFF;
  }
}

BOOL CVivaImagingView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  ZoomScreen((zDelta > 0) ? 1 : -1, &pt);
  return FALSE; // 사용하면 0 리턴
}

BOOL CVivaImagingView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
  LPCTSTR cursor = NULL;

  if (mToolCursor & (HT_CROP_TOP | HT_CROP_BOTTOM))
    cursor = IDC_SIZENS;
  else if (mToolCursor & (HT_CROP_LEFT | HT_CROP_RIGHT))
    cursor = IDC_SIZEWE;

  if (cursor != NULL)
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(cursor));
    return TRUE;
  }
  else
  {
    return CView::OnSetCursor(pWnd, nHitTest, message);
  }
}

void CVivaImagingView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if ((nChar == VK_LEFT) ||
    (nChar == VK_RIGHT) ||
    (nChar == VK_UP) ||
    (nChar == VK_DOWN) ||
    (nChar == VK_CONTROL) || // toggle group mode
    (nChar == 'S') ||
    (nChar == VK_ESCAPE) ||
    (nChar == VK_TAB))
  {
    RECT updateRect = { 0, 0, 0, 0 };
    int state = ((CVivaImagingDoc*)m_pDocument)->OnKeyDown(nChar, nRepCnt, nFlags, mMode, mDrawer, &updateRect);
    if (state != 0)
    {
      int f = (state & R_REDRAW_HANDLE) ? WITH_HANDLE : 0;
      mDrawer.ImageToScreen(updateRect, f);
      InvalidateRect(&updateRect, FALSE);

      if (state & R_REDRAW_THUMB)
      {
        int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
        UpdateThumbView(FALSE, index, 0);
      }
    }
  }
  else if (nChar == VK_DELETE)
  {
    OnEditDelete();
  }
  else if (nChar == VK_SPACE)
  {
    if (mMode != PropertyMode::PModeImageView)
      SetImageMode(PropertyMode::PModeImageView);
    else
      SetImageMode(mToggleEditMode);
  }
}

void CVivaImagingView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_CONTROL) // toggle group mode
  {
    RECT updateRect = { 0, 0, 0, 0 };
    int state = ((CVivaImagingDoc*)m_pDocument)->OnKeyUp(nChar, nRepCnt, nFlags, mMode, mDrawer, &updateRect);
    if (state != 0)
    {
      int f = (state & R_REDRAW_HANDLE) ? WITH_HANDLE : 0;
      mDrawer.ImageToScreen(updateRect, f);
      InvalidateRect(&updateRect, FALSE);

      if (state & R_REDRAW_THUMB)
      {
        int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
        UpdateThumbView(FALSE, index, 0);
      }
    }
  }
}

void CVivaImagingView::OnTimer(UINT_PTR timerEvent)
{
  if (timerEvent == m_MouseScrollTimer)
  {
    CRect rect;
    GetClientRect(rect);
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(&point);

    CheckAutoScrolling(rect, point, TRUE);
  }
}

void CVivaImagingView::OnSize(UINT nType, int cx, int cy)
{
  RECT client;
  GetClientRect(&client);

  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    LPCTSTR path = ((CVivaImagingDoc*)m_pDocument)->GetWorkingPath();
    KBitmap* b = ((KImagePage*)p)->GetImage(path);
    if (b != NULL)
      mDrawer.Set(b->width, b->height, mDrawer.drawOpt.zoom, (client.right - client.left), (client.bottom - client.top));
    if (mDrawer.drawOpt.zoom != 0.0)
      CheckScrollOffset();
  }
  else
  {
    mDrawer.Set(0, 0, 0.0, (client.right - client.left), (client.bottom - client.top));
  }
}

void CVivaImagingView::OnEditMouseMove(UINT nFlags, CPoint point, int pressure)
{
  int state = 0;

  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    if ((MouseState::MOUSE_DRAG_CROP_LEFT <= m_MouseState) && (m_MouseState <= MouseState::MOUSE_DRAG_CROP_BOTTOM))
    {
      RECT updateRect = { 0, 0, 0, 0 };
      POINT dpoint = point;
      mDrawer.ScreenToImage(dpoint);
      if (KPropertyCtrlBase::MouseDragging(m_MouseState, dpoint, mDrawer, mpPreviewEffect) > 0)
      {
        // update to propertyCtrl
        CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
        pFrame->OnPropertyValueChanged(mMode, m_MouseState, mpPreviewEffect);

        Invalidate(FALSE);
        return;
      }
    }
    else
    {
      RECT updateRect = { 0, 0, 0, 0 };
      mDrawer.ScreenToImage(point);
      CPOINT dpoint = { point.x, point.y, (WORD)pressure, 0 };
      state = p->OnMouseMoves(m_MouseState, nFlags, &dpoint, 1, mDrawer, &updateRect);
      if (state != 0)
      {
#ifdef _DEBUG
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Update(%d,%d,%d,%d)\n"), updateRect.left, updateRect.top, updateRect.right - updateRect.left, updateRect.bottom - updateRect.top);
        OutputDebugString(msg);
#endif
        mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
        InvalidateRect(&updateRect, FALSE);
      }
      else
      {
        if ((MouseState::MOUSE_ENTER == m_MouseState) && (mpPreviewEffect != NULL) &&
          KPropertyCtrlBase::UseMouseAction(mMode)) // crop mode
        {
          mToolCursor = KPropertyCtrlBase::MouseHitTest(point, mDrawer, mpPreviewEffect);
        }
      }
    }
  }

  if (state == 0)
  {
    //왜 TrackMouseEvent 를 할까?
    /*
    CRect client;
    GetClientRect(&client);
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

        Invalidate(FALSE);
      }
    }
    */

    if (m_MouseState == MouseState::MOUSE_DRAG_SCROLL)
    {
      int dx = m_MousePoint.x - point.x;
      int dy = m_MousePoint.y - point.y;
      if ((dx != 0) || (dy != 0))
      {
        ScrollPage(dx, dy);
        m_MousePoint = point;
      }
    }
  }
  else if (m_MouseState >= MouseState::MOUSE_DRAW_SELECT)
  {
    // auto scrolling
    CRect client;
    GetClientRect(&client);
    CheckAutoScrolling(client, point, FALSE);
  }
}

void CVivaImagingView::OnEditMouseMoves(UINT nFlags, LP_CPOINT pts, int count)
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    if ((count > 2) &&
      ((MouseState::MOUSE_DRAG_CROP_LEFT > m_MouseState) || (m_MouseState > MouseState::MOUSE_DRAG_CROP_BOTTOM)))
    {
      if (TRUE) //p->IsNeedPenDragging(m_MouseState))
      {
        RECT updateRect = { 0, 0, 0, 0 };
        mDrawer.ScreenToImages(pts, count);
        int state = p->OnMouseMoves(m_MouseState, nFlags, pts, count, mDrawer, &updateRect);
        if (state != 0)
        {
#ifdef _DEBUG
          TCHAR msg[128];
          StringCchPrintf(msg, 128, _T("Update(%d,%d,%d,%d)\n"), updateRect.left, updateRect.top, updateRect.right - updateRect.left, updateRect.bottom - updateRect.top);
          OutputDebugString(msg);
#endif
          mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
          InvalidateRect(&updateRect, FALSE);
        }
        return;
      }
    }
  }

  CPoint point(pts->x, pts->y);
  OnEditMouseMove(nFlags, point, pts->p);
}

void CVivaImagingView::OnEditLButtonDown(UINT nFlags, CPoint point, int pressure)
{
  CRect rect;
  MouseState state = MouseState::MOUSE_OFF;

  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    RECT updateRect = { 0, 0, 0, 0 };
    mDrawer.ScreenToImage(point);
    CPOINT dp = { point.x, point.y, (WORD)pressure, 0 };

    // 택스트 편집 모드이면 편집 종료한다.
    if (mMode == PropertyMode::PModeAnnotateEditText)
    {
      if (mpEditWnd != NULL)
      {
        StartTextEditing(NULL);
        SetImageMode(PropertyMode::PModeAnnotateSelect);
      }
    }

    if (p->IsEditable())
      state = p->OnLButtonDown(mMode, nFlags, dp, mDrawer, &updateRect);

    if ((mMode == PropertyMode::PModePasteImage) && (state == MouseState::MOUSE_OFF))
    {
      EndFloatingImagePaste(TRUE);
    }

    if (state != MouseState::MOUSE_OFF)
    {
      mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
      InvalidateRect(&updateRect, FALSE);
    }
    else
    {
      if (KPropertyCtrlBase::UseMouseAction(mMode) && (mpPreviewEffect != NULL))
      {
        int cs = KPropertyCtrlBase::MouseHitTest(point, mDrawer, mpPreviewEffect);
        if (cs == HT_CROP_TOP)
          state = MouseState::MOUSE_DRAG_CROP_TOP;
        else if (cs == HT_CROP_BOTTOM)
          state = MouseState::MOUSE_DRAG_CROP_BOTTOM;
        else if (cs == HT_CROP_LEFT)
          state = MouseState::MOUSE_DRAG_CROP_LEFT;
        else if (cs == HT_CROP_RIGHT)
          state = MouseState::MOUSE_DRAG_CROP_RIGHT;
      }
    }
  }

  if (state == MouseState::MOUSE_OFF)
  {
    // drag scrolling
    m_MouseState = MouseState::MOUSE_DRAG_SCROLL;
  }
  else
  {
    m_MouseState = state;
  }
  m_MousePoint = point;

  SetCapture();
}

void CVivaImagingView::OnEditLButtonUp(UINT nFlags, CPoint point, int pressure)
{
  int state = 0;

  RECT updateRect = { 0, 0, 0, 0 };
  mDrawer.ScreenToImage(point);
  CPOINT cp = { point.x, point.y, (WORD)pressure, 0 };
  state = ((CVivaImagingDoc*)m_pDocument)->OnLButtonUp(m_MouseState, nFlags, cp, mDrawer, &updateRect);

  /*
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    RECT updateRect = { 0, 0, 0, 0 };
    POINT dp = mDrawer.ScreenToImage(point);
    state = p->OnLButtonUp(m_MouseState, nFlags, dp, mDrawer, &updateRect);
    */
  if (state != 0)
  {
    PropertyMode m = PropertyMode::PModeAnnotateSelect;
    KShapeBase* a = NULL;

    if (state & R_CHANGE_SELECT_TOOL)
    {
      KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(0);
      if ((p != NULL) && p->IsEditable() && (p->GetType() == IMAGE_PAGE))
      {
        // 글상자 개체 생성하면 편집 모드로 전환
        if (m_MouseState == MouseState::MOUSE_DRAW_TEXTBOX)
        {
          a = ((KImagePage*)p)->IsNeedTextboxEditText();
        }
        else if (state & R_CHANGE_EDITTEXT_TOOL)
        {
          // 선택된 개체가 글상자 하나이면 텍스트 편집 모드로 전환
          a = ((KImagePage*)p)->IsNeedTextboxEditText();
        }
      }

      if (a != NULL)
        m = PropertyMode::PModeAnnotateEditText;

      SetImageMode(m);
      //((KImagePage*)p)->OnSelectionChanged(NULL);
    }

    int f = (state & R_REDRAW_HANDLE) ? WITH_HANDLE : 0;
    mDrawer.ImageToScreen(updateRect, f);
    InvalidateRect(&updateRect, FALSE);

    // we need to invalidate thumb view
    if (state & R_REDRAW_THUMB)
    {
      int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
      UpdateThumbView(FALSE, index, 0);
    }
    if (mMode == PropertyMode::PModeAnnotateEditText)
    {
      StartTextEditing(a);
    }
  }
  //}

  if (m_MouseScrollTimer != NULL)
  {
    KillTimer(m_MouseScrollTimer);
    m_MouseScrollTimer = NULL;
  }
  m_MouseState = MouseState::MOUSE_OFF;
  ReleaseCapture();
}

void CVivaImagingView::OnEditLButtonCancel()
{
}

#ifdef USE_POINTER_EVENT

LRESULT CVivaImagingView::OnWmPointerEnter(WPARAM wparam, LPARAM lparam)
{
#ifdef _DEBUG //_POINTER_EVENT
  TCHAR msg[256] = _T("");
  UINT pointerId = GET_POINTERID_WPARAM(wparam);
  POINTER_INPUT_TYPE pointerType = PT_POINTER;
  if (!GetPointerType(pointerId, &pointerType))
    pointerType = PT_POINTER;

  if (pointerType == PT_TOUCH)
    StringCchPrintf(msg, 256, _T("TouchEnter ID=%d\n"), pointerId);
  else if (pointerType == PT_PEN)
    StringCchPrintf(msg, 256, _T("PenEnter ID=%d\n"), pointerId);
  else
    StringCchPrintf(msg, 256, _T("PointerEnter ID=%d\n"), pointerId);
  OutputDebugString(msg);
#endif
  return 0;
}

LRESULT CVivaImagingView::OnWmPointerLeave(WPARAM wparam, LPARAM lparam)
{
  TCHAR msg[256] = _T("");
  UINT pointerId = GET_POINTERID_WPARAM(wparam);
  POINTER_INPUT_TYPE pointerType = PT_POINTER;
  if (!GetPointerType(pointerId, &pointerType))
    pointerType = PT_POINTER;

#ifdef _DEBUG //_POINTER_EVENT
  if (pointerType == PT_TOUCH)
    StringCchPrintf(msg, 256, _T("TouchLeave ID=%d\n"), pointerId);
  else if (pointerType == PT_PEN)
    StringCchPrintf(msg, 256, _T("PenLeave ID=%d\n"), pointerId);
  else
    StringCchPrintf(msg, 256, _T("PointerLeave ID=%d\n"), pointerId);
  OutputDebugString(msg);
#endif
  if (pointerType == PT_TOUCH)
    RemoveTouchHandle(pointerId);
  return 0;
}

LRESULT CVivaImagingView::OnWmPointerUp(WPARAM wparam, LPARAM lparam)
{
  UINT pointerId = GET_POINTERID_WPARAM(wparam);
  POINTER_INPUT_TYPE pointerType = PT_POINTER;
  POINTER_TOUCH_INFO   touchInfo;
  POINTER_PEN_INFO     penInfo;

  if (!GetPointerType(pointerId, &pointerType))
  {
    pointerType = PT_POINTER;
  }
  TCHAR msg[256] = _T("");

  switch (pointerType)
  {
  case PT_TOUCH:
    if (GetPointerTouchInfo(pointerId, &touchInfo))
    {
#ifdef _DEBUG //_POINTER_EVENT
      StringCchPrintf(msg, 256, _T("TouchUp id(%d), pos=(%d,%d), pressure=%d\n"), pointerId, touchInfo.pointerInfo.ptPixelLocation.x,
        touchInfo.pointerInfo.ptPixelLocation.y, touchInfo.pressure);
      OutputDebugString(msg);
#endif
      m_LastPointer.type = PT_TOUCH;
      m_LastPointer.pointerId = pointerId;
      m_LastPointer.x = touchInfo.pointerInfo.ptPixelLocation.x;
      m_LastPointer.y = touchInfo.pointerInfo.ptPixelLocation.y;
      m_LastPointer.pressure = touchInfo.pressure;

      if (IsSingleTouch(pointerId))
      {
        CPoint pt(touchInfo.pointerInfo.ptPixelLocation.x, touchInfo.pointerInfo.ptPixelLocation.y);
        ScreenToClient(&pt);
        OnEditLButtonUp(0, pt, touchInfo.pressure);
      }
      RemoveTouchHandle(pointerId);
    }
    break;
  case PT_PEN:
    if (GetPointerPenInfo(pointerId, &penInfo))
    {
#ifdef _DEBUG //_POINTER_EVENT
      StringCchPrintf(msg, 256, _T("PenUp pos=(%d,%d), pressure=%d\n"), penInfo.pointerInfo.ptPixelLocation.x,
        penInfo.pointerInfo.ptPixelLocation.y, penInfo.pressure);
      OutputDebugString(msg);
#endif
      m_LastPointer.type = PT_PEN;
      m_LastPointer.pointerId = pointerId;
      m_LastPointer.x = penInfo.pointerInfo.ptPixelLocation.x;
      m_LastPointer.y = penInfo.pointerInfo.ptPixelLocation.y;
      m_LastPointer.pressure = penInfo.pressure;

      CPoint pt(penInfo.pointerInfo.ptPixelLocation.x, penInfo.pointerInfo.ptPixelLocation.y);
      ScreenToClient(&pt);
      OnEditLButtonUp(0, pt, penInfo.pressure);
    }
    break;
  case PT_MOUSE:
  default:
    //StringCchPrintf(msg, 256, _T("MouseUp pos=(%d,%d)\n"), GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
    break;
  }
  DefWindowProc(WM_POINTERUP, wparam, lparam);
  memset(&m_LastPointer, 0, sizeof(WmPointerInfo));
  return 0;
}

LRESULT CVivaImagingView::OnWmPointerDown(WPARAM wparam, LPARAM lparam)
{
  UINT pointerId = GET_POINTERID_WPARAM(wparam);
  POINTER_INPUT_TYPE pointerType = PT_POINTER;
  POINTER_TOUCH_INFO   touchInfo;
  POINTER_PEN_INFO     penInfo;

  if (!GetPointerType(pointerId, &pointerType))
  {
    pointerType = PT_POINTER;
  }
  TCHAR msg[256] = _T("");

  switch (pointerType)
  {
  case PT_TOUCH:
    if (GetPointerTouchInfo(pointerId, &touchInfo))
    {
#ifdef _DEBUG //_POINTER_EVENT
      StringCchPrintf(msg, 256, _T("TouchDown id(%d), pos=(%d,%d), pressure=%d\n"), pointerId, touchInfo.pointerInfo.ptPixelLocation.x,
        touchInfo.pointerInfo.ptPixelLocation.y, touchInfo.pressure);
      OutputDebugString(msg);
#endif
      m_LastPointer.type = PT_TOUCH;
      m_LastPointer.pointerId = pointerId;
      m_LastPointer.x = touchInfo.pointerInfo.ptPixelLocation.x;
      m_LastPointer.y = touchInfo.pointerInfo.ptPixelLocation.y;
      m_LastPointer.pressure = touchInfo.pressure;

      AddTouchHandle(pointerId);
      if (IsSingleTouch(pointerId))
      {
        CPoint pt(touchInfo.pointerInfo.ptPixelLocation.x, touchInfo.pointerInfo.ptPixelLocation.y);
        ScreenToClient(&pt);
        OnEditLButtonDown(0, pt, touchInfo.pressure);
      }
      else
      {
        // cancel dagging
        OnEditLButtonCancel();
      }
    }
    break;
  case PT_PEN:
    if (GetPointerPenInfo(pointerId, &penInfo))
    {
#ifdef _DEBUG //_POINTER_EVENT
      StringCchPrintf(msg, 256, _T("PenDown pos=(%d,%d), pressure=%d\n"), penInfo.pointerInfo.ptPixelLocation.x,
        penInfo.pointerInfo.ptPixelLocation.y, penInfo.pressure);
      OutputDebugString(msg);
#endif
      m_LastPointer.type = PT_PEN;
      m_LastPointer.pointerId = pointerId;
      m_LastPointer.x = penInfo.pointerInfo.ptPixelLocation.x;
      m_LastPointer.y = penInfo.pointerInfo.ptPixelLocation.y;
      m_LastPointer.pressure = penInfo.pressure;

      CPoint pt(penInfo.pointerInfo.ptPixelLocation.x, penInfo.pointerInfo.ptPixelLocation.y);
      ScreenToClient(&pt);
      OnEditLButtonDown(0, pt, penInfo.pressure);
    }
    break;
  case PT_MOUSE:
  default:
    //StringCchPrintf(msg, 256, _T("MouseDown pos=(%d,%d)\n"), GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
    break;
  }
  DefWindowProc(WM_POINTERDOWN, wparam, lparam);
  return 0;
}

LRESULT CVivaImagingView::OnWmPointerUpdate(WPARAM wparam, LPARAM lparam)
{
  UINT pointerId = GET_POINTERID_WPARAM(wparam);
  UINT nCount = 0;
  POINTER_INFO pointerInfo;
  if (GetPointerInfo(pointerId, &pointerInfo))
    nCount = pointerInfo.historyCount;
  if (nCount == 0)
    return 1;

  TCHAR msg[256] = _T("");

  if (m_LastPointer.type == PT_TOUCH)
  {
    POINTER_TOUCH_INFO* pInfo = new POINTER_TOUCH_INFO[nCount];

    if (IsSingleTouch(pointerId) &&
      GetPointerTouchInfoHistory(m_LastPointer.pointerId, &nCount, pInfo))
    {
#ifdef _DEBUG_POINTER_EVENT
      StringCchPrintf(msg, 256, _T("TouchInfoHistory=%d, id=%d"), nCount, pointerId);
      if (nCount > 0)
      {
        TCHAR str[16];
        StringCchPrintf(str, 16, _T(",pressure=%d"), pInfo[0].pressure);
        StringCchCat(msg, 256, str);
      }
      StringCchCat(msg, 256, _T("\n"));
      OutputDebugString(msg);
#endif

      LP_CPOINT pts = new CPOINT[nCount];
      for (UINT i = 0; i < nCount; i++)
      {
        // reverse order
        CPoint pt(pInfo[nCount - i - 1].pointerInfo.ptPixelLocation.x, pInfo[nCount - i - 1].pointerInfo.ptPixelLocation.y);
        ScreenToClient(&pt);

        pts[i].x = pt.x;
        pts[i].y = pt.y;
        pts[i].p = pInfo[nCount - i - 1].pressure;
        pts[i].a = 0;
      }
      OnEditMouseMoves(0, pts, nCount);
      delete[] pts;
    }
    delete[] pInfo;
  }
  else if (m_LastPointer.type == PT_PEN)
  {
    POINTER_PEN_INFO* pInfo = new POINTER_PEN_INFO[nCount];
    if (GetPointerPenInfoHistory(m_LastPointer.pointerId, &nCount, pInfo))
    {
#ifdef _DEBUG_POINTER_EVENT
      StringCchPrintf(msg, 256, _T("PenInfoHistory=%d"), nCount);
      if (nCount > 0)
      {
        TCHAR str[16];
        StringCchPrintf(str, 16, _T(",pressure=%d"), pInfo[0].pressure);
        StringCchCat(msg, 256, str);
      }
      StringCchCat(msg, 256, _T("\n"));
      OutputDebugString(msg);
#endif

      LP_CPOINT pts = new CPOINT[nCount];
      for (UINT i = 0; i < nCount; i++)
      {
        CPoint pt(pInfo[nCount - i - 1].pointerInfo.ptPixelLocation.x, pInfo[nCount - i - 1].pointerInfo.ptPixelLocation.y);
        ScreenToClient(&pt);

        pts[i].x = pt.x;
        pts[i].y = pt.y;
        pts[i].p = pInfo[nCount - i - 1].pressure;
        pts[i].a = 0;
      }
      OnEditMouseMoves(0, pts, nCount);
      delete[] pts;
    }
    delete[] pInfo;
  }
  DefWindowProc(WM_POINTERUPDATE, wparam, lparam);
  return 0;
}

static UINT activeTouchId[5] = { 0 };

void CVivaImagingView::InitTouchHandle()
{
  memset(activeTouchId, 0, sizeof(UINT) * 5);
}

BOOL CVivaImagingView::AddTouchHandle(UINT pointerId)
{
  for (int i = 0; i < 5; i++)
  {
    if (activeTouchId[i] == pointerId)
    {
      return TRUE;
    }
  }
  for (int i = 0; i < 5; i++)
  {
    if (activeTouchId[i] == 0)
    {
      activeTouchId[i] = pointerId;
      return TRUE;
    }
  }
  OutputDebugString(_T("5 touch buffer overflow\n"));
  return FALSE;
}

BOOL CVivaImagingView::RemoveTouchHandle(UINT pointerId)
{
  for (int i = 0; i < 5; i++)
  {
    if (activeTouchId[i] == pointerId)
    {
      activeTouchId[i] = 0;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL CVivaImagingView::IsSingleTouch(UINT pointerId)
{
  int count = 0;

  for (int i = 0; i < 5; i++)
  {
    if (activeTouchId[i] > 0)
      ++count;
  }

  if ((count == 1) && (activeTouchId[0] == pointerId))
    return TRUE;
  return FALSE;
}

LRESULT CVivaImagingView::OnWmGesture(WPARAM wparam, LPARAM lparam)
{
  GESTUREINFO gi;

  ZeroMemory(&gi, sizeof(GESTUREINFO));
  gi.cbSize = sizeof(GESTUREINFO);

  BOOL bResult = GetGestureInfo((HGESTUREINFO)lparam, &gi);
  BOOL bHandled = FALSE;

#ifdef _DEBUG
  CString msg;
  msg.Format(_T("Gesture : flag=%x, id=%d, (%d,%d)\n"),
    gi.dwFlags, gi.dwID, gi.ptsLocation.x, gi.ptsLocation.y);
  OutputDebugString(msg);
#endif

  if (bResult && (gi.dwID == GID_PAN))
  {
    if (gi.dwFlags == GF_BEGIN)
    {
      m_GesturePoint.x = gi.ptsLocation.x;
      m_GesturePoint.y = gi.ptsLocation.y;
    }
    else
    {
      int dx = 0;// gi.ptsLocation.x - m_GesturePoint.x;
      int dy = m_GesturePoint.y - gi.ptsLocation.y;
      if ((dx != 0) || (dy != 0))
        ScrollPage(dx, dy);

      m_GesturePoint.x = gi.ptsLocation.x;
      m_GesturePoint.y = gi.ptsLocation.y;
    }
  }
  else
  {
    DWORD dwErr = GetLastError();
    if (dwErr > 0)
    {
      CString msg;
      msg.Format(_T("fail on GetGestureInfo : %x\n"), dwErr);
      OutputDebugString(msg);
    }
  }

  if (bHandled)
    return 0;
  return DefWindowProc(WM_GESTURE, wparam, lparam);
}

#endif // USE_POINTER_EVENT

LRESULT CVivaImagingView::OnScanImageDone(WPARAM wParam, LPARAM lParam)
{
  int done = 0;
  KVoidPtrArray* pArray = (KVoidPtrArray*)wParam;

  if ((pArray != NULL) && (pArray->size() > 0))
  {
    CVivaImagingDoc* pDoc = GetDocument();
    KVoidPtrIterator it = pArray->begin();
    int flag = CLEAR_SELECT | ADD_SELECT | READY_META_INFO;

    while (it != pArray->end())
    {
      HANDLE h = (HANDLE)*it;

      PBITMAPINFOHEADER pDIB = (PBITMAPINFOHEADER)GlobalLock(h);
      if ((pDIB != NULL) && (pDIB->biSize < 0x0FFF) && (pDIB->biBitCount > 0) && (pDIB->biWidth > 0) && (pDIB->biHeight != 0))
      {
        if (pDoc->AddNewPage(pDIB, flag, (LPCTSTR)lParam))
          done++;
        flag = ADD_SELECT;
      }
      else if ((lstrlen((LPTSTR)pDIB) > 0) && IsFileExist((LPCTSTR)pDIB))
      {
        if (pDoc->AddNewPage((LPCTSTR)pDIB, flag, (LPCTSTR)lParam))
          done++;
        flag = ADD_SELECT;
      }
      GlobalUnlock(h);
      GlobalFree(h);
      it++;
    }

    pArray->clear();
    delete pArray;

    if (done > 0)
    {
      OnActivePageChange(CHANGED_ALL);
      OnActivePageChanged(CHANGED_ALL);
    }
  }
  return (LRESULT)done;
}

LRESULT CVivaImagingView::OnThreadCallResult(WPARAM wParam, LPARAM lParam)
{
  if (SELECT_FOLDER == wParam)
  {
    THREAD_CALL_DATA *td = (THREAD_CALL_DATA *)lParam;
    if (td != NULL)
    {
      if (lstrlen(td->folder_oid) > 0)
      {
        if (lstrcmp(td->local_path, keyServerOpenFolder) == 0)
        {
          OpenServerFolder(td->folder_oid);
        }
        else if (lstrcmp(td->local_path, keyServerOpenFile) == 0)
        {
          OpenServerFolder(td->folder_oid);
        }
        else if (lstrcmp(td->local_path, keyServerSaveFolder) == 0)
        {
          StringCchCopy(mUploadServerFolderOID, MAX_OID, td->folder_oid);
          CString filename = ((CVivaImagingDoc*)m_pDocument)->GetTitle();

          ((CVivaImagingDoc*)m_pDocument)->SetServerUploadFolder(td->folder_oid);

          OnStartUpload(td->folder_oid, filename, ((CVivaImagingDoc*)m_pDocument)->GetCurrentFormat(), mGlobalOption);
        }
      }
      free(td);
    }
  }
  return 0;
}


LRESULT CVivaImagingView::OnBgTaskMessage(WPARAM wParam, LPARAM lParam)
{
  KBgTaskLocalScan* task = (KBgTaskLocalScan*)lParam;
  if (task != NULL)
  {
    if (wParam == BGTASK_DONE)
    {
      if ((task->GetType() == BGTASK_LOCAL_SCAN) && (task->mFileList.size() > 0))
      {
        int len = lstrlen(task->mArg) + 1;
        KVoidPtrIterator it = task->mFileList.begin();
        while (it != task->mFileList.end())
        {
          LPTSTR name = (LPTSTR)*(it++);
          len += lstrlen(name) + 1;
        }
        len += 2;

        LPTSTR buff = new TCHAR[len];
        LPTSTR p = buff;

        // add base folder path
        lstrcpy(p, task->mArg);
        p += lstrlen(task->mArg) + 1;

        it = task->mFileList.begin();
        while (it != task->mFileList.end())
        {
          LPTSTR name = (LPTSTR)*(it++);
          lstrcpy(p, name);
          p += lstrlen(name) + 1;
        }
        *p = '\0'; // add null

        int flag = CLEAR_SELECT | ADD_SELECT_FIRST | READY_META_INFO | ADD_WITH_FOLDER;
        int n = ((CVivaImagingDoc*)m_pDocument)->AddImagePagesEx(buff, len, flag);
        delete[] buff;

        if (n > 0)
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Load %d items from "), n);
          StringCchCat(msg, 256, task->mArg);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
          SetImageMode(PropertyMode::PModeImageView);
          OnActivePageChanged(CHANGED_ACTIVE_PAGE);
        }
      }
      else if (task->GetType() == BGTASK_FOLDER_DOWNLOAD)// && (task->mFileList.size() > 0))
      {
        // check if it has body xml
        LPCTSTR name = ((CVivaImagingDoc*)m_pDocument)->GetDownloadedBodyDoc();
        if ((name != NULL) && (lstrlen(name) > 0))
        {
          LPTSTR newFileName = AllocPathName(((CVivaImagingDoc*)m_pDocument)->GetWorkingPath(), name);
          if (IsFileExist(newFileName))
          {
            ((CVivaImagingDoc*)m_pDocument)->OnOpenDocument(newFileName);
            m_pDocument->SetPathName(newFileName, FALSE);
            m_pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);

            delete[] newFileName;
          }
        }

        // all loading done
        ((CVivaImagingDoc*)m_pDocument)->SetServerUploadFolder(task->mArg);

        OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
        SetImageMode(PropertyMode::PModeImageView);
        OnActivePageChanged(CHANGED_ACTIVE_PAGE);

        TCHAR msg[256];
        KBgTaskDownload* downloadTask = (KBgTaskDownload *)task;
        StringCchPrintf(msg, 256, _T("Download folder done, file(%d/%d) done"),
          downloadTask->mDoneFile, downloadTask->mTotalFile);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        ((CVivaImagingDoc*)m_pDocument)->UpdateExtDocTypeInfo((EcmFolderItem*)downloadTask->mEcmRoot, downloadTask->mpDocTypeInfo);

        if (downloadTask->mFailCount > 0)
        {
          CString str;
          CString fm;
          str.LoadString(AfxGetInstanceHandle(), IDS_FAIL_SOME_FILES_DOWNLOAD, mResourceLanguage);
          fm.Format(_T("(%d)"), downloadTask->mFailCount);
          str += fm;
          AfxMessageBox(str, MB_OK);
        }

        // update property
        OnActivePageChange(CHANGED_ACTIVE_PAGE);
      }
      else if (task->GetType() == BGTASK_UPLOAD)
      {
        TCHAR msg[256];
        KBgTaskUpload* uploadTask = (KBgTaskUpload*)task;
        StringCchPrintf(msg, 256, _T("Upload folder(%d/%d), file(%d/%d) done"),
          uploadTask->mDoneFolder, uploadTask->mTotalFolder, uploadTask->mDoneFile, uploadTask->mTotalFile);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
      }
      else if (task->GetType() == BGTASK_FILE_DOWNLOAD)
      {
        TCHAR msg[256];
        KBgTaskDownload* downloadTask = (KBgTaskDownload*)task;
        StringCchPrintf(msg, 256, _T("Download file(%d/%d) done"),
          downloadTask->mDoneFile, downloadTask->mTotalFile);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        /*
        if (downloadTask->mFailCount > 0)
        {
          CString str;
          CString fm;
          str.LoadString(AfxGetInstanceHandle(), IDS_FAIL_SOME_FILES_DOWNLOAD, mResourceLanguage);
          fm.Format(_T("(%d)"), downloadTask->mFailCount);
          str += fm;
          AfxMessageBox(str, MB_OK);
        }
        */
      }
      else if (task->GetType() == BGTASK_EFFECT_RENDER)
      {
        KBgTaskEffectRender* renderTask = (KBgTaskEffectRender*)task;
        
        if (task->IsDone() &&
          ((CVivaImagingDoc*)m_pDocument)->UpdateRenderImage(renderTask->m_pPage, renderTask->mpPreviewEffect, renderTask->m_CacheImage))
        {
          renderTask->DetachImage();
          Invalidate(FALSE);
        }
      }
      OnTaskEnd(task);
    }
    else if (wParam == BGTASK_DOWNLIST_READY)
    {
      // file list to download
      HANDLE h = (HANDLE)lParam;
      if (h != NULL)
      {
        LPTSTR buff = (LPTSTR)GlobalLock(h);
        UINT len = GlobalSize(h);

        ((CVivaImagingDoc*)m_pDocument)->ReadyWorkingPath();

        int flag = ADD_WITH_FOLDER | ADD_WITH_EMPTY | READY_META_INFO | ADD_SELECT_FIRST;

        KIntVector newPageList;
        int n = ((CVivaImagingDoc*)m_pDocument)->AddImagePages(buff, &newPageList, flag, NULL);
        //int n = ((CVivaImagingDoc*)m_pDocument)->AddNewPages(buff, len, flag);

        GlobalUnlock(h);
        GlobalFree(h);

        if (n > 0)
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Load %d items from server"), n);
          // StringCchCat(msg, 256, task->mArg);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

          OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
          SetImageMode(PropertyMode::PModeImageView);
          OnActivePageChanged(CHANGED_ACTIVE_PAGE);
        }
      }
    }
    else if (wParam == BGTASK_DOWNLIST_DONE)
    {
      // file list download completed
      HANDLE h = (HANDLE)lParam;
      if (h != NULL)
      {
        LPTSTR buff = (LPTSTR)GlobalLock(h);
        UINT len = GlobalSize(h);

        int flag = 0;
        KIntVector pageList;
        int n = ((CVivaImagingDoc*)m_pDocument)->UpdatePages(buff, len, flag, pageList);
        GlobalUnlock(h);
        GlobalFree(h);

        // update pageList
        KImageBase* a = ((CVivaImagingDoc*)m_pDocument)->GetActivePage();

        if ((a != NULL) && DoesListContainValue(pageList, a->GetID()))
        {
          OnActivePageChanged(0);
        }

        UpdateThumbView(pageList, 0);
      }
    }
    else if (wParam == BGTASK_UPLOAD_DONE)
    {
      // file list download completed
      HANDLE h = (HANDLE)lParam;
      if (h != NULL)
      {
        LPTSTR buff = (LPTSTR)GlobalLock(h);
        UINT len = GlobalSize(h);

        int flag = 0;
        KIntVector pageList;
        int n = ((CVivaImagingDoc*)m_pDocument)->UpdatePages(buff, len, flag, pageList);
        GlobalUnlock(h);
        GlobalFree(h);
      }
    }
    else if (wParam == BGTASK_PROGRESS)
    {
      HANDLE h = (HANDLE)lParam;
      if (h != NULL)
      {
        TaskProgressInfo* info = (TaskProgressInfo*)GlobalLock(h);

        CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
        pFrame->OnTaskProgress(info);

        if (info->hErrorMsg != NULL)
          GlobalFree(info->hErrorMsg);

        GlobalUnlock(h);
        GlobalFree(h);
      }
    }
    else if (wParam == BGTASK_LOG_MESSAGE)
    {
      HANDLE h = (HANDLE)lParam;
      if (h != NULL)
      {
        LPTSTR msg = NULL;
        LPTSTR func = (LPTSTR)GlobalLock(h);
        LPTSTR flags = StrChr(func, ',');
        if (flags != NULL)
        {
          *(flags++) = '\0';
          msg = StrChr(flags, ',');
          if (msg != NULL)
            *(msg++) = '\0';
        }

        if (msg != NULL)
        {
          int f = _wtoi(flags);
          StoreLogHistory(func, msg, f);
        }
        GlobalUnlock(h);
        GlobalFree(h);
      }
    }
  }
  return 0;
}

BOOL CVivaImagingView::ZoomScreen(int dir, LPPOINT zOrg)
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    LPCTSTR path = ((CVivaImagingDoc*)m_pDocument)->GetWorkingPath();
    KBitmap* b = ((KImagePage*)p)->GetImage(path);
    if (zOrg != NULL)
      ScreenToClient(zOrg);

    if (mDrawer.Zoom(dir, zOrg))
    {
      // check scroll offset
      CheckScrollOffset();
      Invalidate(FALSE);
      return TRUE;
    }
#if 0
    RECT client;
    GetClientRect(&client);
    int client_width = (client.right - client.left);
    int client_height = (client.bottom - client.top);

    POINT baseImagePos = { 0, 0 };
    if (zOrg != NULL)
    {
      ScreenToClient(zOrg);
      baseImagePos = ScreenToImage(*zOrg);
      // *zOrg = ImageToScreen(baseImagePos);
    }

    if (mDrawOption.zoom == 0.0)
    {
      float xr = (float)client_width / b->width;
      float yr = (float)client_height / b->height;
      mDrawOption.zoom = (xr < yr) ? xr : yr;

      int dw = (int)(mDrawOption.zoom * b->width);
      int dh = (int)(mDrawOption.zoom * b->height);

      // to center
      mDrawOption.scrollOffset.x = 0; // (dw - client_width) / 2;
      mDrawOption.scrollOffset.y = 0; // (dh - client_height) / 2;
    }

    if (dir != 0)
    {
      float diff = (mDrawOption.zoom / 4);
      if (dir < 0)
        diff = (-1) * diff;
      mDrawOption.zoom += diff;
      float newzoom = mDrawOption.zoom + diff;

      if (newzoom < 0.1)
        newzoom = (float)0.1;
      else if ((0.95 < newzoom) && (newzoom < 1.1))
        newzoom = (float)1.0;
      else if (newzoom > 10)
        newzoom = (float)10;

      if (newzoom != mDrawOption.zoom)
      {
        mDrawOption.zoom = newzoom;

        // check scroll range
        mDrawOption.displaySize.cx = (int)(mDrawOption.zoom * b->width);
        mDrawOption.displaySize.cy = (int)(mDrawOption.zoom * b->height);

        if (zOrg != NULL)
        {
          POINT curr = ImageToScreen(baseImagePos);

          // 차이만큼 화면 스크롤 한다.
          mDrawOption.scrollOffset.x += (curr.x - zOrg->x);
          if (mDrawOption.scrollOffset.x < 0)
            mDrawOption.scrollOffset.x = 0;
          mDrawOption.scrollOffset.y += (curr.y - zOrg->y);
          if (mDrawOption.scrollOffset.y < 0)
            mDrawOption.scrollOffset.y = 0;
        }

        // check scroll offset
        CheckScrollOffset();
        Invalidate(FALSE);
        return TRUE;
      }
    }
#endif
  }
  return FALSE;
}

void CVivaImagingView::UpdateActionResult(int r)
{
  if (r & CHANGE_PAGES)
  {
    OnActivePageChange(CHANGED_ALL);
    OnActivePageChanged(CHANGED_ALL);
  }
  else if (r & CHANGE_CURRENT_PAGE)
  {
    OnActivePageChange(CHANGED_ACTIVE_PAGE);
    Invalidate(FALSE);
  }
  else if (r & CHANGE_CURRENT_VIEW)
  {
    UpdateThumbView(FALSE, -1, 0);
    Invalidate(FALSE);
  }
  else if (r & CHANGE_ALL_VIEW)
  {
    UpdateThumbView(TRUE, -1, 0);
    Invalidate(FALSE);
  }

}

void CVivaImagingView::CheckScrollOffset()
{
  mDrawer.CheckScrollOffset();
}

BOOL CVivaImagingView::ScrollPage(int dx, int dy)
{
  if (mDrawer.ScrollPage(dx, dy))
  {
    Invalidate(FALSE);
    return TRUE;
  }
  return FALSE;
}

void CVivaImagingView::CheckAutoScrolling(CRect& client, POINT& point, BOOL bScroll)
{
  int margin = mDrawer.handleSize * 3;
  int dx = 0;
  int dy = 0;

  if (point.x < client.left + margin)
    dx = -(client.right - client.left) / 10;
  else if ((client.right - margin) < point.x)
    dx = (client.right - client.left) / 10;

  if (point.y < client.top + margin)
    dy = -(client.bottom - client.top) / 10;
  else if ((client.bottom - margin) < point.y)
    dy = (client.bottom - client.top) / 10;

  if (mDrawer.ScrollPage(dx, dy, bScroll))
  {
    if (m_MouseScrollTimer == NULL)
      m_MouseScrollTimer = SetTimer(TIMER_MOUSE_SCROLL, 300, NULL);

    Invalidate(FALSE);
  }
  else
  {
    if (m_MouseScrollTimer != NULL)
    {
      KillTimer(m_MouseScrollTimer);
      m_MouseScrollTimer = NULL;
      StoreLogHistory(_T(__FUNCTION__), _T("Stop AutoScroll timer"), SYNC_MSG_DEBUG);
    }
  }
}

int CVivaImagingView::InsertNewImagePages(LPCTSTR filenames)
{
  // calculage buffer size
  int buffLength = 0;
  LPCTSTR p = filenames;
  while (*p != '\0')
  {
    buffLength += lstrlen(p) + 1;
    p += lstrlen(p) + 1;
  }
  buffLength++;

  int n = ((CVivaImagingDoc*)m_pDocument)->AddImagePagesEx(filenames, buffLength, CLEAR_SELECT | ADD_SELECT | READY_META_INFO);

  if (n > 0)
  {
    OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
    SetImageMode(PropertyMode::PModeImageView);
    OnActivePageChanged(CHANGED_ACTIVE_PAGE);
  }
  return n;
}

void CVivaImagingView::ClearImageMode(BOOL bUpdate, PropertyMode nextMode)
{
  // clear preview effect
  if (mpPreviewEffect != NULL)
  {
    UpdatePreview(NULL);
  }
  if (mMode == PropertyMode::PModeAnnotateEditText)
    StartTextEditing(NULL);
  else if (mMode == PropertyMode::PModePasteImage)
    EndFloatingImagePaste(TRUE);

  // Annotate -> BlockEdit 모드가 되면 선택 개체를 해제한다.
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    RECT updateRect;
    int rtn = ((KImagePage *)p)->CloseToolMode(mMode, nextMode, &updateRect);

    if (bUpdate && (rtn != 0))
    {
      mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
      InvalidateRect(&updateRect, FALSE);
    }
  }
}

void CVivaImagingView::SetImageMode(PropertyMode mode)
{
  if (mMode != mode)
  {
    ClearImageMode(TRUE, mode);

    mMode = mode;
    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->OnPropertyModeChanged(mode);

    // store toggle mode
    if (IsSelectShapeMode(mMode) || IsAnnotateMode(mMode))
      mToggleEditMode = mMode;
  }
}

BOOL CVivaImagingView::StartTextEditing(KShapeBase* a)
{
  if ((mpEditWnd != NULL) && (mpActiveShape != NULL) && (mpActiveShape != a))
  {
    CString str;
    mpEditWnd->GetWindowText(str);
    ((KShapeTextbox*)mpActiveShape)->SetText(str);

    mpEditWnd->DestroyWindow();
    delete mpEditWnd;
    mpEditWnd = NULL;
    mpActiveShape = NULL;
  }


  if (a != NULL)
  {
    if (mpActiveShape != a)
    {
      mpActiveShape = a;

      RECT rect;
      mpActiveShape->GetRectangle(rect);
      mDrawer.ImageToScreen(rect);

      mpEditWnd = new CEdit();
      mpEditWnd->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        CRect(rect.left, rect.top, rect.right, rect.bottom), this, 303);

      LPCTSTR pstr = ((KShapeTextbox*)mpActiveShape)->GetText();
      if (lstrlen(pstr) > 0)
        mpEditWnd->SetWindowText(pstr);
      mpEditWnd->SetFocus();
    }
  }
  return (mpEditWnd != NULL);
}

void CVivaImagingView::UpdatePreview(ImageEffectJob* effect)
{
  BOOL need_cache_clear = FALSE;

  // clear preview image
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    if ((effect != NULL) && (effect->job > PropertyMode::PModeImageView) &&
      ((KImagePage*)p)->IsNeedRenderEffect(effect))
    {
      if (mpPreviewEffect == NULL)
        mpPreviewEffect = new ImageEffectJob;

      memcpy(mpPreviewEffect, effect, sizeof(ImageEffectJob));

      // BGThread로 프리뷰 업데이트하는 경우에는 캐시 클리어 및 화면 업데이트를 여기서 하지 않고,
      // 쓰레드 작업 완료후에 한다.
#ifdef _RENDER_BY_BG_THREAD
      if (((KImagePage*)p)->IsNeedRenderImageEffect(mpPreviewEffect))
      {
        if (!KBitmap::IsSimpleImageEffect(mpPreviewEffect))
          StartPreviewImageRender((KImagePage*)p, mpPreviewEffect);
        else
          Invalidate(FALSE); // crop/resize needs screen update
      }
#else
      need_cache_clear = KImagePage::IsPropertyNeedImageCacheUpdate(mpPreviewEffect->job);
#endif
    }
    else
    {
      if (mpPreviewEffect != NULL)
      {
        need_cache_clear = KImagePage::IsPropertyNeedImageCacheUpdate(mpPreviewEffect->job);
        delete mpPreviewEffect;
        mpPreviewEffect = NULL;
      }

      if (need_cache_clear)
      {
        ((KImagePage *)p)->ClearPreviewImage();
        Invalidate(FALSE);
      }

#ifdef _RENDER_BY_BG_THREAD
      ClearSameTask(BGTASK_EFFECT_RENDER, (KImagePage*)p);
#endif
    }

#ifndef _RENDER_BY_BG_THREAD
    BOOL need_screen_erase = (mpPreviewEffect != NULL) ?
      KImagePage::IsPropertyNeedSceenErase(mpPreviewEffect) : FALSE;

    Invalidate(need_screen_erase, FALSE);
#endif
  }
}

void CVivaImagingView::FillSelectedArea(ImageEffectJob* effect)
{
  if (IsSelectBlockMode(effect->job))
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
    if (p != NULL)
    {
      if ((p->GetType() == IMAGE_PAGE) &&
        ((KImagePage *)p)->GetImageSelection(effect->param))
      {
        ApplyEffect(effect);
      }
    }
  }
}

void CVivaImagingView::CopySelectedArea(ImageEffectJob* effect)
{
  if (IsSelectBlockMode(effect->job))
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
    if (p != NULL)
    {
      if ((p->GetType() == IMAGE_PAGE) &&
        ((KImagePage *)p)->GetImageSelection(effect->param))
      {
        OnEditCopy();
        /*
        effect->job = (PropertyMode)(effect->job + PModeCopyRectangle - PModeSelectRectangle);
        ApplyEffect(effect); // 여기서 선택영역을 KBitmap으로 만든다.

        if (effect->pResult != NULL)
        {
          KBitmap* b = (KBitmap *)effect->pResult;

          if (OpenClipboard() && EmptyClipboard())
          {
            // SetClipboardData(CF_BITMAP, b->CompatibleBitmap());

            HANDLE p = b->GetDIB();
            if (p != NULL)
            {
               SetClipboardData(CF_DIB, p);
            }
            CloseClipboard();
          }
          else
          {
            StoreLogHistory(_T(__FUNCTION__), _T("failure on OpenClipboard"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
          }

          delete b;
          effect->pResult = NULL;
        }
        */
      }
    }
  }
 }

int CVivaImagingView::InsertImageDropFiles(HDROP hdrop)
{
  UINT  numberOfFiles;
  TCHAR szNextFile[MAX_PATH];

  // Get the # of files being dropped.
  numberOfFiles = DragQueryFile(hdrop, -1, NULL, 0);
  int len = 0;

  for (UINT uFile = 0; uFile < numberOfFiles; uFile++)
  {
    // Get the next filename from the HDROP info.
    if (DragQueryFile(hdrop, uFile, szNextFile, MAX_PATH) > 0)
    {
      len += lstrlen(szNextFile) + 1;
    }
  }
  len++;
  LPTSTR filenames = NULL;
  UINT buffLength = len;

  if (numberOfFiles > 0)
  {
    filenames = new TCHAR[len];
    LPTSTR buff = filenames;
    for (UINT uFile = 0; uFile < numberOfFiles; uFile++)
    {
      // Get the next filename from the HDROP info.
      if (DragQueryFile(hdrop, uFile, szNextFile, MAX_PATH) > 0)
      {
        StringCchCopy(buff, len, szNextFile);
        len -= lstrlen(szNextFile) + 1;
        buff += lstrlen(szNextFile) + 1;
      }
    }
    if (len > 0)
      buff[0] = '\0';
  }
  DragFinish(hdrop);

  if (numberOfFiles > 0)
  {
    numberOfFiles = ((CVivaImagingDoc*)m_pDocument)->AddImagePagesEx(filenames, buffLength, CLEAR_SELECT | ADD_SELECT | READY_META_INFO);
    OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
    SetImageMode(PropertyMode::PModeImageView);
    OnActivePageChanged(CHANGED_ACTIVE_PAGE);
  }

  if (filenames != NULL)
    delete[] filenames;

  return numberOfFiles;
}

void CVivaImagingView::DoEditPaste()
{
  OnEditPaste();
}

void CVivaImagingView::PasteClipboardData(HANDLE b)
{
  int r = ((CVivaImagingDoc*)m_pDocument)->InsertPageData(b, SELECT_INSERTED_OBJECTS);
  if (r > 0)
  {
    if (r == ADD_SHAPES)
    {
      OnActivePageChanged(0);
      SetImageMode(PropertyMode::PModeAnnotateSelect);

      int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
      UpdateThumbView(FALSE, index, 0);
    }
    else
    {
      OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
      SetImageMode(PropertyMode::PModeImageView);
      OnActivePageChanged(CHANGED_ACTIVE_PAGE);
    }
  }
}

void CVivaImagingView::PasteClipboardImage(HANDLE b)
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);

  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    PBITMAPINFOHEADER bi = (PBITMAPINFOHEADER)GlobalLock(b);
    if (bi != NULL)
    {
      RECT updateRect;
      UINT buffSize = GlobalSize(b);
      SetImageMode(PropertyMode::PModePasteImage);
      if (((KImagePage*)p)->StartFloatingImagePaste(bi, buffSize, &updateRect))
      {
        mDrawer.ImageToScreen(updateRect, WITH_HANDLE);
        InvalidateRect(&updateRect, FALSE);
      }
      else
      {
        CString str;
        str.LoadString(AfxGetInstanceHandle(), IDS_PASTE_IMAGE_ONLY_24BITIMAGE, mResourceLanguage);
        AfxMessageBox(str, MB_OK);
      }
    }
    GlobalUnlock(b);
  }
  else
  {
    PBITMAPINFOHEADER bi = (PBITMAPINFOHEADER)GlobalLock(b);
    ((CVivaImagingDoc*)m_pDocument)->AddNewPage(bi, CLEAR_SELECT|ADD_SELECT);
    GlobalUnlock(b);

    OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES| UPDATE_ACTIVE_VIEW);
  }
}

void CVivaImagingView::ShapeEditCommand(int channel, int* params, void* extra)
{
  RECT rect;
  if (((CVivaImagingDoc*)m_pDocument)->ShapeEditCommand(channel, params, extra, rect))
  {
    mDrawer.ImageToScreen(rect, WITH_HANDLE);
    InvalidateRect(&rect, FALSE);

    // we need to invalidate thumb view
    int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
    UpdateThumbView(FALSE, index, 0);
  }
}

void CVivaImagingView::ApplyEffect(ImageEffectJob* effect)
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)m_pDocument;
  int r = doc->ApplyEffect(effect);
  if (r)
  {
    KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
    if (p != NULL)
    {
      RECT rect;
      if ((p->GetType() == IMAGE_PAGE) &&
        ((KImagePage *)p)->GetUpdateRect(effect, rect))
      {
        mDrawer.ImageToScreen(rect);
        InvalidateRect(&rect, FALSE);
      }
      else if (r & EFFECT_CHANGE_IMAGE)
      {
        Invalidate(!!(r & EFFECT_CHANGE_SIZE));
      }
    }

    if (r & EFFECT_CHANGE_SIZE)
      OnPageSizeChanged(0);

    // we need to invalidate thumb view
    int index = doc->GetActivePageIndex();
    UpdateThumbView(effect->applyAll, index, r);
  }
}

void CVivaImagingView::CloseProperty(ImageEffectJob* effect)
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if (p != NULL)
  {
    RECT rect;
    if ((p->GetType() == IMAGE_PAGE) &&
      ((KImagePage *)p)->CloseProperty(effect, rect))
    {
      mDrawer.ImageToScreen(rect, WITH_HANDLE);
      InvalidateRect(&rect, FALSE);
    }
  }
}

void CVivaImagingView::EndFloatingImagePaste(BOOL bDone)
{
  KImageBase* p = ((CVivaImagingDoc*)m_pDocument)->GetImagePage(EXCLUDE_FOLDED);
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    RECT rect;

    int state = ((CVivaImagingDoc*)m_pDocument)->EndFloatingImagePaste((KImagePage *)p, bDone, &rect);

    if (state != 0)
    {
      mDrawer.ImageToScreen(rect, WITH_HANDLE);
      InvalidateRect(&rect, FALSE);
    }

    if (state & R_REDRAW_THUMB)
    {
      int index = ((CVivaImagingDoc*)m_pDocument)->GetActivePageIndex();
      UpdateThumbView(FALSE, index, 0);
    }
  }
}

int CVivaImagingView::PropertyChange(LPCTSTR key, LPCTSTR str)
{
  CVivaImagingDoc* doc = (CVivaImagingDoc*)GetDocument();
  return doc->PropertyChange(key, str);
}

void CVivaImagingView::StartPreviewImageRender(KImagePage* p, ImageEffectJob* effect)
{
  // 동일한 task가 진행중이면 중지하고 다시 시작한다.
  ClearSameTask(BGTASK_EFFECT_RENDER, p);

  KBgTaskEffectRender* task = new KBgTaskEffectRender(this, BGTASK_EFFECT_RENDER, NULL);

  if (task->SetEffectRender(p, effect))
  {
    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->OnTaskStart(task);
    AddTask(task);

    task->Start();

    // copy to ImagePage
    p->SetImageEffect(effect);
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("EffectPreviewBgTask failed"), SYNC_MSG_LOG);
  }
}

void CVivaImagingView::OpenAndUploadLocalFile(LPCTSTR fileName, LPCTSTR folderOID)
{
  if (((CVivaImagingDoc*)m_pDocument)->OnOpenDocument(fileName))
  {
    m_pDocument->SetPathName(fileName, FALSE);
    m_pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);

    OnActivePageChange(CHANGED_ACTIVE_PAGE | CHANGED_NUMBER_OF_PAGES);
    SetImageMode(PropertyMode::PModeImageView);
    OnActivePageChanged(CHANGED_ACTIVE_PAGE);

    gpCoreEngine->SelectSyncServerFolderLocation(GetSafeHwnd(), keyServerSaveFolder, folderOID);
  }
  else
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Cannot open local file"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
}

void CVivaImagingView::OpenLocalFolder(LPCTSTR pathName)
{
  if (IsDirectoryExist(pathName))
  {
    // create background thread for load images below the folder
    KBgTaskLocalScan* task = new KBgTaskLocalScan(this, BGTASK_LOCAL_SCAN, pathName);

    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->OnTaskStart(task);
    AddTask(task);

    task->Start();
  }
}

void CVivaImagingView::OpenServerFolder(LPCTSTR serverFolderOID)
{
  if (lstrlen(mUserOid) > 0)
  {
    // create background thread for load images below the folder
    KBgTaskDownload* task = new KBgTaskDownload(this, BGTASK_FOLDER_DOWNLOAD, serverFolderOID);
    task->SetWorkingFolder(((CVivaImagingDoc*)m_pDocument)->GetWorkingPath());

    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->OnTaskStart(task);
    AddTask(task);

    task->Start();
  }
}

void CVivaImagingView::OpenServerFile(LPCTSTR serverFileOID)
{
  if (lstrlen(mUserOid) > 0)
  {
    // create background thread for load image file
    KBgTaskDownload* task = new KBgTaskDownload(this, BGTASK_FILE_DOWNLOAD, serverFileOID);

    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->OnTaskStart(task);
    AddTask(task);

    task->Start();
  }
}

void CVivaImagingView::OnStartUpload(LPCTSTR folderOid, LPCTSTR filename, ImagingSaveFormat saveFormat, UINT flag)
{
  KBgTaskUpload* task = new KBgTaskUpload(this, BGTASK_UPLOAD, folderOid);
  task->SetUploadInfo(filename, 
    ((CVivaImagingDoc*)m_pDocument)->GetWorkingPath(),
    (KImageFolder*)((CVivaImagingDoc*)m_pDocument)->GetRootImagePage(),
    &((CVivaImagingDoc*)m_pDocument)->mDocTypeInfo, saveFormat, flag); //  mGlobalOption);

  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->OnTaskStart(task);
  AddTask(task);

  task->Start();

  CString filePathName = ((CVivaImagingDoc*)m_pDocument)->GetPathName();
  int len = lstrlen(filePathName) + lstrlen(folderOid) + 4;
  LPTSTR arg = new TCHAR[len];
  StringCchCopy(arg, len, filePathName);
  StringCchCat(arg, len, _T("|"));
  StringCchCat(arg, len, folderOid);

  // store registry to restart if unfinished
  StoreRegistryValue(strLastUnfinishedDocument, arg);
 
  TCHAR msg[300] = _T("StoreLastUnfinishedDoc : ");
  StringCchCat(msg, 300, arg);
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);

  delete[] arg;
}

void CVivaImagingView::OnTaskEnd(KBgTask* task)
{
  CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
  pFrame->OnTaskEnd(task);
  RemoveTask(task);

  if ((task->GetType() == BGTASK_UPLOAD) && task->IsDone())
  {
    ((CVivaImagingDoc*)m_pDocument)->ClearModifiedFlag();

    StoreRegistryValue(strLastUnfinishedDocument, _T(""));
    StoreLogHistory(_T(__FUNCTION__), _T("Clear LastUnfinishedDoc"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }

  delete task;
}

void CVivaImagingView::StopAllTask()
{
  KVoidPtrIterator it = mBgTasks.begin();
  while (it != mBgTasks.end())
  {
    KBgTask* t = (KBgTask*)*(it++);
    t->Stop();
  }
}

void CVivaImagingView::AddTask(KBgTask* task)
{
  mBgTasks.push_back(task);
#ifdef _DEBUG_BG_TASK
  TCHAR msg[64];
  StringCchPrintf(msg, 64, _T("Number of bg task = %d"), mBgTasks.size());
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
}

void CVivaImagingView::RemoveTask(KBgTask* task)
{
  KVoidPtrIterator it = mBgTasks.begin();
  while (it != mBgTasks.end())
  {
    KBgTask* t = (KBgTask*)*it;
    if (t == task)
    {
      it = mBgTasks.erase(it);
      return;
    }
    else
    {
      ++it;
    }
  }
#ifdef _DEBUG_BG_TASK
  TCHAR msg[64];
  StringCchPrintf(msg, 64, _T("Number of bg task = %d"), mBgTasks.size());
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
}

void CVivaImagingView::ClearSameTask(BgTaskType type, KImagePage* targetPage)
{
  KVoidPtrIterator it = mBgTasks.begin();
  while (it != mBgTasks.end())
  {
    KBgTask* t = (KBgTask*)*it;
    if (t->GetType() == type)
    {
      t->Stop();
    }
    ++it;
  }

#ifdef _DEBUG_BG_TASK
  TCHAR msg[64];
  StringCchPrintf(msg, 64, _T("Number of bg task = %d"), mBgTasks.size());
  StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
#endif
}
